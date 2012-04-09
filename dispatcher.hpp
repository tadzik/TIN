#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "csgi.hpp"

#include <map>
#include <string>

class Dispatcher : public CSGI::Application {
public:
    Dispatcher(CSGI::Application *);
    virtual CSGI::Response operator()(CSGI::Env&);
    void add_handler(std::string, CSGI::Application *);
    virtual ~Dispatcher()
    {
        if (freedefaulthandler)
            delete default_handler_;
    }
private:
    std::map<std::string, CSGI::Application*> handlers_;
    CSGI::Application * default_handler_;
    bool freedefaulthandler;
};

#endif
