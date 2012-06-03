#include "Skunk.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>

/**
 * @file Skunk.cpp
 * 
 * */
 
 
 /**
  * Przekazuje znak zapisany heksadecymalnie do typu char
  * @param a - podawany na wejscie string z heksadecymalnym znakiem
  * 
  * */
char CharFromHex (std::string a)
{
std::istringstream Blat (a);
int Z;
Blat >> std::hex >> Z;

return char(Z); // cast to char and return
}


/**
 * Odczytuje strumien z pola tekstowego formularza widocznego w przegladarce
 * i konwertuje go na typ string
 * 
 * @param coded - parametr typu string odczytywany z pola tekstowego
 * @return Text - zwraca przekonwertowany string
 * */
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


/**
 * Dodaje nowy widget do serwera
 * @param w - nowy widget 
 * @return zwraca wskaznik na puste miejsce w liscie widgetow
 * */
int Skunk::Server::addWidget(Skunk::Widget *w) {
    widgets_.push_back(w);
    w->id_ = nextID_;

    std::string id = "id" + itoa(nextID_);
    widgets_map_[id] = w;

    return nextID_++;
}


/**
 * Realizuje metode HTTP GET przy wiswietlaniu wszystkich dostepnych widgetow
 * @param env - parametry zapytania HTTP
 * @return zwraca odpowiedz serwera
 * 
 * */
CSGI::Response Skunk::Server::get(CSGI::Env& env, std::string& username) {
    CSGI::Response resp;
    int count; 
    std::stringstream id_str;
    resp.status = 200;
    std::vector<Skunk::Widget *>::iterator it;

    resp.content.append("<html>\n");
    resp.content.append("\t<head><title>2012 TIN SERVER</title></head>\n");
    resp.content.append("<script type='text/javascript' src='http://ajax.googleapis.com/ajax/libs/jquery/1.7.2/jquery.min.js'></script>");
    resp.content.append("\t<body>");
    resp.content.append("\n\t\t<form method='post' action='/'>\n");

    for (it = widgets_.begin(); it != widgets_.end(); it++) {
        Skunk::Widget *w = *it;
        if (auth_->canGET(username, w->id_)) {
            resp.content.append(w->GET());
            resp.content.append("<hr/>");
        }
    }
    
    count = widgets_.size();
    id_str << count;
    resp.content.append("<input type='hidden' name='count' id='wid_count' value='");
    resp.content.append(id_str.str());
    resp.content.append("'/>");
    
    resp.content.append("\n\t\t\t<input type='submit' value='Zmien'/>");
    resp.content.append("\n\t\t</form>");
    if(username == "admin"){
        resp.content.append("\n\n\n<a href='/adduser' id='useradd' >Add new user</a>");
    }

    resp.content.append("<form method='post' action='/logout'>");
    resp.content.append("<input type='submit' value='Logout'/>");
    resp.content.append("</form>\n");

    
    resp.content.append("<script type='text/javascript'>\n");
    resp.content.append("jQuery(document).bind('ready',function(){\n");
    resp.content.append("var count = $('#wid_count').attr('value');\n");
    resp.content.append("var i;\n");
    resp.content.append("var change_id;\n");
    resp.content.append("var radio_count = $('#radio_count').attr('value');\n");
    
    resp.content.append("for (i=0;i<count;++i){\n");
    resp.content.append("$('#i'+i).change(function(){\n");
    
    resp.content.append("change_id = $(this).attr('id');\n");
    resp.content.append("$('#'+change_id+'_changed').val('true');\n");
    
    
    resp.content.append("});\n");
    
        resp.content.append("for (j=0;j<=radio_count;++j){\n");                    ///wewnetrzny for dla radio radio_count
        resp.content.append("$('#i'+i+'_'+j).click(function(){\n");
        resp.content.append("alert('zmieniam');");
    
        resp.content.append("change_id = $(this).attr('id');\n");
        resp.content.append("alert(change_id);");
        resp.content.append("$('#'+change_id+'_changed').val('true');\n");
    
    
        resp.content.append("});\n");
        resp.content.append("}\n");         
    
    
    
    resp.content.append("}\n");
    
    
    ///(value).attr('src'));
    
    resp.content.append("});\n");
    resp.content.append("</script>\n");
    
    resp.content.append("\n\t</body>\n</html>\n");

    resp.headers["Content-Type"]   = "text/html";
    resp.headers["Content-Length"] = itoa(resp.content.length());

    return resp;
    (void)env;
}


/**
 * Parser wartosci kluczowych,uzywany do izolowania odpowiednich
 * wartosci z parametrow zapytania HTTP
 * @param src - string do parsowania
 * @param separator - zadany do parsowania separator, zwykle &
 * @return - zwraca sparsowana wartosc kluczowa
 * 
 * */
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


/**
 * Parser danych metody POST
 * @param env - parametry zapytania HTTP
 * @return sparsowane dane POST
 * */
StringMap parsePostData(CSGI::Env& env) {
    return parseKeyVals(env["csgi.input"], "&");
}


/**
 * Parser ciasteczek
 * @param src - string zrodlowy
 * @return - sparsowane ciasteczko (separator: ;)
 * */
StringMap parseCookies(std::string& src) {
    return parseKeyVals(src, ";");
}


/**
 * Sprawdza autoryzacje uzytkownika poprzez parsowanie parametrów zapytania HTTP
 * @param env - parametry zapytania HTTP
 * @return identyfikator uzytkownika sesji
 * */
std::string Skunk::Server::isAuthed(CSGI::Env& env) {
    StringMap cookies = parseCookies(env["HTTP_COOKIE"]);
    return sessions_[cookies["sessionid"]];
}


/**
 * Usuwa z ciasteczek biezaca sesje
 * @param env - parametry zapytania HTTP
 * 
 * */
void Skunk::Server::removeSession(CSGI::Env& env) {
    StringMap cookies = parseCookies(env["HTTP_COOKIE"]);
    sessions_[cookies["sessionid"]] = "";
}


/**
 * Wyswietla strone logowania
 * @return odpowiedz HTTP
 * 
 * */
CSGI::Response showLoginScreen() {
    CSGI::Response resp;

    resp.status = 200;
    ///wyglad strony
    resp.content.append("<!DOCTYPE html>\n<html>\n");
    resp.content.append("\t<head><title>2012 SKUNKS LOGIN</title></head>\n");
    resp.content.append("\t<body>");

    resp.content.append("<form method='post' action='/'>");

    resp.content.append("<p style='position:absolute; left:0px; top:0px;'> Username: <input name='user' type='text'/></p><br />");
    resp.content.append("<p style='position:absolute; left:300px;top:0px;'> Password: <input name='pass' type='password' /></p>"
                        "<br />");

    resp.content.append("<input style='postition:absolute; left:0px; top:50px;' type='submit' value='Zaloguj'/>");
    resp.content.append("</form>");
    resp.content.append("\n\t</body>\n</html>\n");
    resp.headers["Content-Type"]   = "text/html";
    resp.headers["Content-Length"] = itoa(resp.content.length());
    return resp;
}


/**
 * Wyswietla ekran dodawania nowego uzytkownika w panelu admina
 * @return odpowiedz HTTP
 * */
CSGI::Response addNewUser(){
    
    CSGI::Response resp;
    resp.status = 200;
    
    resp.content.append("<!DOCTYPE html>\n<html>\n");
    resp.content.append("\t<head><title>2012 SKUNKS ADD NEW USER</title></head>\n");
    resp.content.append("\t<body>");

    resp.content.append("<form method='post' action='/adduser'>");

    resp.content.append("<p style='position:absolute; left:0px; top:0px;'> New username: <input name='user' type='text'/></p><br />");
    resp.content.append("<p style='position:absolute; left:500px;top:0px;'> Password: <input name='pass' type='text' /></p>"
                        "<br />");

    resp.content.append("<input style='postition:absolute; left:0px; top:50px;' type='submit' value='Utworz nowego usera'/>");
    
    
    resp.content.append("</form>");

    
    resp.content.append("\n\t</body>\n</html>\n");
    resp.headers["Content-Type"]   = "text/html";
    resp.headers["Content-Length"] = itoa(resp.content.length());
    return resp;
}


/**
 * Sprawdza URI oraz typ metody HTTP i przydziela odpowiednie widoki 
 * w zaleznosci od potrzeb
 * URI - dalsza czesc adresu url
 * @param env - parametry zapytania HTTP
 * 
 * */
CSGI::Response Skunk::Server::operator()(CSGI::Env& env) {
    std::string session  = "";
    std::string username = isAuthed(env);

    if (env["REQUEST_URI"].compare("/logout") == 0) {
        removeSession(env);
        return showLoginScreen();
    }

    if (username.compare("") == 0) {
        if (env["REQUEST_METHOD"].compare("POST") == 0) {
            StringMap cred = parsePostData(env);
            if (auth_->verify(cred["user"], cred["pass"])) {
                session.append("SkunkSession");
                session.append(cred["user"]);
                session.append(itoa(rand()));
                username = sessions_[session] = cred["user"];
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
    
    if ((env["REQUEST_URI"].compare("/adduser") == 0)&&(username=="admin")){
        if (env["REQUEST_METHOD"].compare("POST") == 0) {
            StringMap cred = parsePostData(env);
            auth_->addUser(cred["user"],cred["pass"]);
        }
        else{
        return addNewUser();
        }
    }
    
    CSGI::Response resp = this->get(env, username);
    if (session.length() > 0)
        resp.headers["Set-Cookie"] = "sessionid=" + session
                                   + "; Max-Age=" + itoa(5*60);
                                   //FIXME: Minute is too low
    
    return resp;
}


/**
 * Uruchamia server CSGI
 * */
void Skunk::Server::run() {
    CSGI::Server srv(this, 8080);
    srv.run(false);
}
