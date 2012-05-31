#include "csgi.hpp"

void CSGI::Server::run(bool async)
{
    int i;

    memset(&hints_, 0, sizeof hints_);
    hints_.ai_family = AF_INET;
    hints_.ai_socktype = SOCK_STREAM;
    hints_.ai_flags = AI_PASSIVE;

    std::stringstream port;
    port << port_;

    i = getaddrinfo(NULL, port.str().c_str(), &hints_, &res_);
    if (i) {
        std::string err = "getaddrinfo(): ";
        err.append(gai_strerror(i));
        throw CSGI::Exception(err);
    }
    sockfd_ = socket(res_->ai_family, res_->ai_socktype,
                     res_->ai_protocol);
    if (sockfd_ == -1) {
        freeaddrinfo(res_);
        std::string err = "socket(): ";
        err.append(strerror(errno));
        throw CSGI::Exception(err);
    }

    int y = 1;
    i = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &y, sizeof y);
    if (i == -1) {
        freeaddrinfo(res_);
        std::string err = "setsockopt()";
        err.append(strerror(errno));
        throw CSGI::Exception(err);
    }

    i = bind(sockfd_, res_->ai_addr, res_->ai_addrlen);
    if (i == -1) {
        close(sockfd_);
        freeaddrinfo(res_);
        std::string err = "bind(): ";
        err.append(strerror(errno));
        throw CSGI::Exception(err);
    }
    i = listen(sockfd_, backlog_);
    if (i == -1) {
        close(sockfd_);
        freeaddrinfo(res_);
        std::string err = "listen(): ";
        err.append(strerror(errno));
        throw CSGI::Exception(err);
    }
    if (async) {
        if (fork() == 0) serve();
    } else {
        serve();
    }
}

void CSGI::Server::serve()
{
    int newfd;

    for (;;) {
        SSL *ssl = SSL_new(ssl_ctx_);
        newfd    = accept(sockfd_, 0, 0);
        SSL_set_fd(ssl, newfd);
        int err = SSL_accept(ssl);
        if (err != 1) {
            std::string res;
            res.append("HTTP/1.1 400 Bad Request\r\n\r\n");
            res.append("SSL error: ");
            res.append(ERR_error_string(SSL_get_error(ssl, err), 0));
            res.append("\r\n");
            write(newfd, res.c_str(), res.length());
            close(newfd);
            continue;
        }
        CSGI::Env env;
        try {
            env = parse_request(ssl);
        } catch (CSGI::InvalidRequest&) {
            const char *res = "HTTP/1.1 400 Bad Request\r\n\r\nbad request";
            SSL_write(ssl, res, strlen(res));
            close(newfd);
            continue;
        }
        Response resp = (*app_)(env);
        send_response(resp, ssl);
        close(newfd);
        SSL_free(ssl);
    }
}

CSGI::Env CSGI::Server::parse_request(SSL *ssl)
{
    CSGI::Env env;
    std::stringstream s;
    char buf[4096];
    
    int  ret = SSL_read(ssl, buf, sizeof(buf) - 1);
    if (ret == -1) {
        throw CSGI::InvalidRequest();
    }
    buf[ret] = '\0';
    s << std::string(buf);

    if (ret < 5) { // dirty hack, I hope it's a temporary solution
        ret = SSL_read(ssl, buf, sizeof(buf) - 1);
        if (ret == -1) {
            throw CSGI::InvalidRequest();
        }
        buf[ret] = '\0';
        s << std::string(buf);
    }

    std::string line;
    getline(s, line);
    size_t pos = line.find(" ");
    if (pos == std::string::npos) {
        throw CSGI::InvalidRequest();
    }
    std::string meth = line.substr(0, pos);
    env["REQUEST_METHOD"] = meth;
    std::string rest = line.substr(pos + 1);
    pos = rest.find(" ");
    if (pos == std::string::npos) {
        throw CSGI::InvalidRequest();
    }
    env["REQUEST_URI"]     = rest.substr(0, pos);

    for (;;) {
        getline(s, line);
        pos = line.find(":");
        if (pos == std::string::npos) break;
        std::string v = line.substr(pos + 2);
        std::string k = "HTTP_" + line.substr(0, pos);
        std::transform(k.begin(), k.end(), k.begin(), ::toupper);
        if (k == "HTTP_HOST"
        && (pos = v.find(":")) != std::string::npos) {
            env["HTTP_HOST"] = v.substr(0, pos);
            env["HTTP_PORT"] = v.substr(pos + 1);
            continue;
        }
        env[k] = v;
    }

    if (env.count("HTTP_CONTENT-LENGTH")) {
        int n = atoi(env["HTTP_CONTENT-LENGTH"].c_str());
        char * buf = new char[n + 1];
        s.read(buf, n);
        buf[n] = '\0';
        env["csgi.input"] = std::string(buf);
        delete []buf;
    }

    return env;
}

void CSGI::Server::send_response(Response& resp, SSL *ssl) {
    std::stringstream out;
    out << "HTTP/1.1 " << resp.status << " OK\r\n";
    Headers::iterator it;
    for (it = resp.headers.begin();
         it != resp.headers.end();
         it++) {
        out << it->first << ": " << it->second << "\r\n";
    }
    out << "\r\n";
    out << resp.content << "\r\n";
    SSL_write(ssl, out.str().c_str(), out.str().length());
}
