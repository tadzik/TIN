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
        html.append("'></input>");

        return html;
    }

    virtual void POST(std::string& s) {
        this->setValue(s);
    }
};

class Server : CSGI::Application {
    std::vector<Widget *> widgets_;
    std::map<std::string, Widget*> widgets_map_;
    int nextID_;
public:
    Server() : nextID_(0) { };
    void addWidget(Widget *);
    void run();
    CSGI::Response get(CSGI::Env&);
    virtual CSGI::Response operator()(CSGI::Env&);
};

}

#endif
