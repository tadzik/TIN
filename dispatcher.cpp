#include "dispatcher.hpp"
#include <sstream>

// removes leading and trailing slashes from a string
std::string stripslashes(std::string s)
{
    if (s.at(0) == '/') s.erase(0, 1);
    if (s.at(s.length() - 1) == '/') s.erase(s.length() - 1, 1);
    return s;
}

class NotFoundHandler : public CSGI::Application {
    virtual CSGI::Response operator()(CSGI::Env& env)
    {
        CSGI::Response ret;

        std::string body = "Not found";
        std::stringstream len;
        len << body.length();

        ret.status = 404;
        ret.headers["Content-Type"]   = "text/plain";
        ret.headers["Content-Length"] = len.str();
        ret.content = body;

        return ret;
        (void)env;
    }
};

Dispatcher::Dispatcher(CSGI::Application * default_app)
{
    if (default_app != NULL) {
        default_handler_   = default_app;
        freedefaulthandler = false;
    } else {
        default_handler_   = new NotFoundHandler();
        freedefaulthandler = true;
    }
}

void Dispatcher::add_handler(std::string s, CSGI::Application * a)
{
    handlers_[stripslashes(s)] = a;
}

CSGI::Response Dispatcher::operator()(CSGI::Env& env)
{
    CSGI::Application *y = handlers_[stripslashes(env["REQUEST_URI"])];
    if (y != NULL) {
        return (*y)(env);
    }
    return (*default_handler_)(env);
}
