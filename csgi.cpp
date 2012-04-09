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
    struct sockaddr sa;
    socklen_t       salen;
    int             newfd;

    for (;;) {
        newfd = accept(sockfd_, &sa, &salen);
        CSGI::Env env;
        try {
            env = parse_request(newfd);
        } catch (CSGI::InvalidRequest&) {
            const char *res = "400 Bad Request";
            send(newfd, res, strlen(res), 0);
            close(newfd);
            continue;
        }
        Response resp = (*app_)(env);
        send_response(resp, newfd);
        close(newfd);
    }
}

CSGI::Env CSGI::Server::parse_request(int fd)
{
    CSGI::Env env;
    char buf[4096];
    recv(fd, buf, sizeof(buf), 0);
    
    std::stringstream s;
    s << std::string(buf);

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

void CSGI::Server::send_response(Response& resp, int fd) {
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
    send(fd, out.str().c_str(), out.str().length(), 0);
}
