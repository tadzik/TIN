#include "Skunk.hpp"
#include <cstdio>
#include <cstring>

std::string itoa(int i) {
    std::stringstream str;
    str << i;
    return str.str();
}

void Skunk::Server::addWidget(Skunk::Widget *w) {
    widgets_.push_back(w);
    w->id_ = nextID_;

    std::string id = "id" + itoa(nextID_);
    widgets_map_[id] = w;

    nextID_++;
}

CSGI::Response Skunk::Server::get(CSGI::Env& env) {
    CSGI::Response resp;

    resp.status = 200;
    std::vector<Skunk::Widget *>::iterator it;

    resp.content.append("<form method='post' action='/'>");

    for (it = widgets_.begin(); it != widgets_.end(); it++) {
        Skunk::Widget *w = *it;
        resp.content.append(w->GET());
        resp.content.append("<hr/>");
    }

    resp.content.append("<input type='submit' value='Zmień'/>");
    resp.content.append("</form>");

    resp.headers["Content-Type"]   = "text/html";
    resp.headers["Content-Length"] = itoa(resp.content.length());

    return resp;
    (void)env;
}

CSGI::Response Skunk::Server::operator()(CSGI::Env& env) {
    if (env["REQUEST_METHOD"].compare("POST") == 0) {
        char *token;
        char *postdata = (char *)env["csgi.input"].c_str();
        token = strtok(postdata, "=&");
        while (token != NULL) {
            std::string key   = std::string(token);
            token = strtok(NULL, "=&");
            std::string value = std::string(token);
            token = strtok(NULL, "=&");

            widgets_map_[key]->POST(value);
        }
    }
    return this->get(env);
}

void Skunk::Server::run() {
    CSGI::Server srv(this, 8080);
    srv.run(false);
}
