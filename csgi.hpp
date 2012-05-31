#ifndef CSGI_H
#define CSGI_H

#include <map>
#include <string>
#include <sstream>
#include <algorithm>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace CSGI {

typedef std::map<std::string, std::string> _stringmap;

typedef _stringmap  Env;
typedef _stringmap  Headers;
typedef std::string Body;
typedef int         Status;

typedef std::pair<Headers, Body> Content;

struct Response {
    Status  status;
    Headers headers;
    Body    content;
};

class Application {
public:
    virtual CSGI::Response operator()(CSGI::Env&) = 0;
    virtual ~Application() { }
};

class Exception : public std::exception {
public:
    Exception(std::string msg) : message_(msg) { }

    virtual const char * what() const throw() {
        return message_.c_str();
    }

    virtual ~Exception() throw() { }
private:
    std::string message_;
};

class InvalidRequest : public std::exception { };

class Server {
public:
    Server(Application * app, int port) : app_(app), port_(port)
    {
        backlog_ = 10;

        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();

        ssl_method_ = (SSL_METHOD*)SSLv3_server_method();
        ssl_ctx_    = SSL_CTX_new(ssl_method_);
        if (ssl_ctx_ == NULL) {
            throw CSGI::Exception("FUCKUP");
        }

        SSL_CTX_use_certificate_file(ssl_ctx_, "cert.pem",
                                     SSL_FILETYPE_PEM);
        SSL_CTX_use_PrivateKey_file(ssl_ctx_, "cert.pem",
                                    SSL_FILETYPE_PEM);

        if (!SSL_CTX_check_private_key(ssl_ctx_)) {
            throw CSGI::Exception("Well, fuck");
        }

        ssl_ = SSL_new(ssl_ctx_);
    }

    void run(bool async);
private:
    void serve();
    Env  parse_request(SSL*);
    void send_response(Response&, SSL*);
    Application *app_;
    struct addrinfo hints_, *res_;
    int sockfd_;
    int port_;
    int backlog_;

    SSL_METHOD *ssl_method_;
    SSL_CTX    *ssl_ctx_;
    SSL        *ssl_;
};

} //namespace

#endif
