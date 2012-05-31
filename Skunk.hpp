#ifndef SKUNK_H
#define SKUNK_H
#include <string>
#include <vector>
#include <map>

#include "csgi.hpp"

namespace Skunk {

struct Widget {
    virtual std::string GET()              = 0;
    virtual void        POST(std::string&) = 0;
    int id_;
};

struct Auth {
    virtual bool verify(std::string&, std::string&) = 0;
    virtual bool canGET(std::string&,  int)         = 0;
    virtual bool canPOST(std::string&, int)         = 0;
};

struct SimpleAuth : Auth {
    std::map<std::string, std::string> users_;

    virtual bool verify(std::string& user, std::string& pass) {
        if (users_.count(user) == 0)
            return false;
        return users_[user].compare(pass) == 0;
    }

    void addUser(std::string user, std::string pass) {
        users_[user] = pass;
    }

    virtual bool canGET(std::string& user, int id) {
        return true;
        (void)user; (void)id;
    }

    virtual bool canPOST(std::string& user, int id) {
        return user.compare("admin") == 0;
        (void)id;
    }
};

struct TextField : Widget {
    virtual std::string getValue() = 0;
    virtual void        setValue(std::string&) { };

    virtual std::string GET() {
        std::stringstream id_str;
        id_str << this->id_;

        std::string html = "";
        html.append("<input type='text' name='id");
        html.append(id_str.str());
        html.append("' value='");
        html.append(this->getValue());
        html.append("'/>\n");

        return html;
    }

    virtual void POST(std::string& s) {
        this->setValue(s);
    }
};

class Server : CSGI::Application {
    std::vector<Widget *> widgets_;
    std::map<std::string, Widget*>     widgets_map_;
    std::map<std::string, std::string> sessions_;
    Auth * auth_;
    int nextID_;
public:
    Server() : nextID_(0) { };
    int addWidget(Widget *);
    void setAuth(Auth *a) { auth_ = a; }
    bool isAuthed(CSGI::Env&);
    void run();
    CSGI::Response get(CSGI::Env&);
    virtual CSGI::Response operator()(CSGI::Env&);
};

}

#endif
