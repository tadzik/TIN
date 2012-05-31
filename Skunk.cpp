#include "Skunk.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>


char CharFromHex (std::string a)
{
std::istringstream Blat (a);
int Z;
Blat >> std::hex >> Z;

return char(Z); // cast to char and return
}

std::string urldecoder(std::string coded){
    std::string Text = coded;
    //std::string Text (Luthien[1]);
    std::string::size_type Pos;
    std::string Hex;
    while (std::string::npos != (Pos = Text.find('%')))
    {
    Hex = Text.substr(Pos + 1, 2);
    Text.replace(Pos, 3, 1, CharFromHex(Hex));} 

    while (std::string::npos != (Pos = Text.find('+')))
    {
    Text.replace(Pos, 1, 1, ' ');} 

    std::cout << Text << std::endl;
    return Text;
}







typedef std::map<std::string, std::string> StringMap;

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

StringMap parsePostData(std::string& src) {
    StringMap ret;
    std::string part, key, val;
    size_t from = 0, amp, eq;

    for (;;) {
        amp      = src.substr(from).find("&");
        part     = src.substr(from, amp);
        eq       = part.find("=");
        key      = part.substr(0, eq);
        val      = part.substr(eq + 1);
        ret[key] = val;
        if (amp == std::string::npos) break;
        from = from + amp + 1;
    }
    return ret;
}

CSGI::Response Skunk::Server::operator()(CSGI::Env& env) {
    if (env["REQUEST_METHOD"].compare("POST") == 0) {
        std::string src = env["csgi.input"].c_str();
        StringMap data  = parsePostData(src);
        StringMap::iterator it;
        for (it = data.begin(); it != data.end(); it++) {
            if (widgets_map_[it->first] != NULL) {
                std::string decoded = urldecoder(it->second);
                widgets_map_[it->first]->POST(decoded);
            }
        }
    }
    return this->get(env);
}

void Skunk::Server::run() {
    CSGI::Server srv(this, 8080);
    srv.run(false);
}
