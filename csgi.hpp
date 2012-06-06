#ifndef CSGI_H
#define CSGI_H

#include <map>
#include <string>
#include <sstream>
#include <algorithm>

#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <iostream> // FIXME REMOVEME
#include <pthread.h>

/**
 * @file csgi.hpp
 */

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

/**
 * Aplikacja CSGI
 *
 * Aplikacja CSGI to obiekt funkcyjny przyjmujący mapę CSGI::Env
 * oraz zwracający instancję CSGI::Response
 */
class Application {
public:
    virtual CSGI::Response operator()(CSGI::Env&) = 0;
    virtual ~Application() { }
};

/**
 * Podstawowa klasa do wyjątków rzucanych przez serwer CSGI
 */
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

/**
 * Wyjątek rzucany przy niepoprawnym zapytaniu HTTP;
 * do użytku wewnętrnego
 */
class InvalidRequest : public std::exception { };

/**
 * Serwer CSGI, serwujący wybraną aplikację
 */
class Server {
public:
    /**
     * Konstruktor serwera
     *
     * @param app  aplikacja którą serwer ma serwować
     * @param port port na którym serwer będzie nasłuchiwać
     */
    Server(Application * app, int port) : app_(app), port_(port)
    {
        backlog_ = 10;

        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();

        ssl_method_ = (SSL_METHOD*)SSLv3_server_method();
        ssl_ctx_    = SSL_CTX_new(ssl_method_);
        if (ssl_ctx_ == NULL) {
            throw CSGI::Exception("Could not initialize SSL context");
        }

        SSL_CTX_use_certificate_file(ssl_ctx_, "cert.pem",
                                     SSL_FILETYPE_PEM);
        SSL_CTX_use_PrivateKey_file(ssl_ctx_, "cert.pem",
                                    SSL_FILETYPE_PEM);

        if (!SSL_CTX_check_private_key(ssl_ctx_)) {
            throw CSGI::Exception("Error checking private key");
        }

        worker_ = NULL;
        stop_   = 0;
    }

    ~Server()
    {
        //std::cerr << "Cleaning up" << std::endl;
        if (worker_ != NULL) {
            stop_ = 1;
            pthread_join(*worker_, NULL);
            free(worker_);
            //std::cerr << "Child returned" << std::endl;
        }
        close(sockfd_);
        freeaddrinfo(res_);
        SSL_CTX_free(ssl_ctx_);
    }

    /**
     * Uruchomienie serwera
     *
     * @param async czy uruchomić serwer w osobnym procesie
     *              jeśli nie, wywołanie jest blokujące
     */
    void run(bool async);
    void serve();
private: // udokumentowane w csgi.cpp
    Env  parse_request(SSL*);
    void send_response(Response&, SSL*);
    Application *app_;
    struct addrinfo hints_, *res_;
    int sockfd_;
    int port_;
    int backlog_;
    int stop_;

    pthread_t  *worker_;
    SSL_METHOD *ssl_method_;
    SSL_CTX    *ssl_ctx_;
};

} //namespace

#endif
