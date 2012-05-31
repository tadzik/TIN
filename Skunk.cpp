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
    std::string::size_type Pos;
    std::string Hex;
    while (std::string::npos != (Pos = Text.find('%')))
    {
    Hex = Text.substr(Pos + 1, 2);
    Text.replace(Pos, 3, 1, CharFromHex(Hex));} 

    while (std::string::npos != (Pos = Text.find('+')))
    {
    Text.replace(Pos, 1, 1, ' ');} 

    return Text;
}

typedef std::map<std::string, std::string> StringMap;

std::string itoa(int i) {
    std::stringstream str;
    str << i;
    return str.str();
}

int Skunk::Server::addWidget(Skunk::Widget *w) {
    widgets_.push_back(w);
    w->id_ = nextID_;

    std::string id = "id" + itoa(nextID_);
    widgets_map_[id] = w;

    return nextID_++;
}

CSGI::Response Skunk::Server::get(CSGI::Env& env) {
    CSGI::Response resp;

    resp.status = 200;
    std::vector<Skunk::Widget *>::iterator it;

    resp.content.append("<!DOCTYPE html>\n<html>\n");
    resp.content.append("\t<head><title>2012 TIN MF</title></head>\n");
    resp.content.append("\t<body>");
    resp.content.append("\n\t\t<form method='post' action='/'>\n");

    std::string username = isAuthed(env);
    for (it = widgets_.begin(); it != widgets_.end(); it++) {
        Skunk::Widget *w = *it;
        if (auth_->canGET(username, w->id_)) {
            resp.content.append(w->GET());
            resp.content.append("<hr/>");
        }
    }

    resp.content.append("\n\t\t\t<input type='submit' value='Zmień'/>");
    resp.content.append("\n\t\t</form>");
    resp.content.append("\n\t</body>\n</html>\n");

    resp.headers["Content-Type"]   = "text/html";
    resp.headers["Content-Length"] = itoa(resp.content.length());

    return resp;
    (void)env;
}

StringMap parseKeyVals(std::string& src, std::string separator) {
    StringMap ret;
    std::string part, key, val;
    size_t from = 0, amp, eq;

    for (;;) {
        amp      = src.substr(from).find(separator);
        part     = src.substr(from, amp);
        eq       = part.find("=");
        key      = part.substr(0, eq);
        val      = part.substr(eq + 1);
        ret[key] = val;
        if (amp == std::string::npos) break;
        from = from + amp + separator.length();
    }
    return ret;
}

StringMap parsePostData(CSGI::Env& env) {
    return parseKeyVals(env["csgi.input"], "&");
}

StringMap parseCookies(std::string& src) {
    return parseKeyVals(src, ";");
}

std::string Skunk::Server::isAuthed(CSGI::Env& env) {
    StringMap cookies = parseCookies(env["HTTP_COOKIE"]);
    return sessions_[cookies["sessionid"]];
}

CSGI::Response showLoginScreen() {
    CSGI::Response resp;

    resp.status = 200;

    resp.content.append("<form method='post' action='/'>");

    resp.content.append("User: <input name='user' type='text' /><br />");
    resp.content.append("Pass: <input name='pass' type='password' />"
                        "<br />");

    resp.content.append("<input type='submit' value='Zmień'/>");
    resp.content.append("</form>");

    resp.headers["Content-Type"]   = "text/html";
    resp.headers["Content-Length"] = itoa(resp.content.length());

    return resp;
}

CSGI::Response Skunk::Server::operator()(CSGI::Env& env) {
    std::string session  = "";
    std::string username = isAuthed(env);
    if (username.compare("") == 0) {
        if (env["REQUEST_METHOD"].compare("POST") == 0) {
            StringMap cred = parsePostData(env);
            if (auth_->verify(cred["user"], cred["pass"])) {
                session.append("SkunkSession");
                session.append(cred["user"]);
                session.append(itoa(rand()));
                sessions_[session] = cred["user"];
            } else {
                return showLoginScreen();
            }
        } else {
            return showLoginScreen();
        }
    }
    if (env["REQUEST_METHOD"].compare("POST") == 0) {
        StringMap data  = parsePostData(env);
        StringMap::iterator it;
        for (it = data.begin(); it != data.end(); it++) {
            Widget *w = widgets_map_[it->first];
            if (w != NULL) {
                if (auth_->canPOST(username, w->id_)) {
                    std::string decoded = urldecoder(it->second);
                    w->POST(decoded);
                }
            }
        }
    }
    CSGI::Response resp = this->get(env);
    if (session.length() > 0)
        resp.headers["Set-Cookie"] = "sessionid=" + session
                                   + "; Max-Age=" + itoa(5*60);
                                   //FIXME: Minute is too low
    return resp;
}

void Skunk::Server::run() {
    CSGI::Server srv(this, 8080);
    srv.run(false);
}
