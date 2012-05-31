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
//    if (!checkAuth(env)) {
//        return this->loginScreen();
//    }
    if (env["REQUEST_METHOD"].compare("POST") == 0) {
        std::string src = env["csgi.input"].c_str();
        std::string part, key, val;
        size_t from = 0, amp, eq;

        for (;;) {
            amp  = src.substr(from).find("&");
            part = src.substr(from, amp);
            eq   = part.find("=");
            key  = part.substr(0, eq);
            val  = part.substr(eq + 1);
            widgets_map_[key]->POST(val);
            if (amp == std::string::npos) break;
            from = from + amp + 1;
        }
    }
    return this->get(env);
}

void Skunk::Server::run() {
    CSGI::Server srv(this, 8080);
    srv.run(false);
}
