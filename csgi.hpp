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

typedef Response(*Application)(Env&);

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
    Server(Application app, int port) : app_(app), port_(port)
    {
        backlog_ = 10;
    }

    void run(bool async);
private:
    void serve();
    Env  parse_request(int fd);
    void send_response(Response& resp, int fd);
    Application app_;
    struct addrinfo hints_, *res_;
    int             sockfd_;
    int port_;
    int backlog_;
};

} //namespace

#endif
