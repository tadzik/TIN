#ifndef SKUNK_H
#define SKUNK_H
#include <string>
#include <vector>
#include <map>

#include "csgi.hpp"

/**
 * @file Skunk.hpp
 * */


/**
 * Konwerter z typu int na typ string
 * @param i - podany int
 * @return przekonwertowany string
 * */
static std::string itoa(int i) {
    std::stringstream str;
    str << i;
    return str.str();
}

namespace Skunk {

/**
 * struktura przechowujace widgety
 * kazdy posiada metody obslugujace GET i POST
 * oraz swoje ID
 * */
struct Widget {
    virtual std::string GET()              = 0;
    virtual void        POST(std::string&) = 0;
    int id_;
};


/**
 * Struktura implementujace metody
 * verify - weryfikujaca
 * canPOST - sprawdzajaca czy uzytkownik ma dostep do metody POST
 * canGET - sprawdzajaca czy uzytkownik ma dostep odczytu przez GET
 * addUser - dodawanie uzytkownikow przez admina
 * */
struct Auth {
    virtual bool verify(std::string&, std::string&) = 0;
    virtual bool canGET(std::string&,  int)         = 0;
    virtual bool canPOST(std::string&, int)         = 0;
    virtual void addUser(std::string , std::string ) = 0;
};


/**
 * 
 * */
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


/**
 * Struktura przechowujaca metody umozliwiajace realizacje pola 
 * tekstowego, lacznie z jego wyswietlaniem
 * */
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
        html.append("' id='i");           /// id tylko dla javascriptu
        html.append(id_str.str());
        html.append("'/>\n");
        
        html.append("<input type='hidden' name='id");
        html.append(id_str.str());
        html.append("_changed' ");
        html.append("id='i");           /// id tylko dla javascriptu
        html.append(id_str.str());
        html.append("_changed'");
        html.append(" value='false'/>\n");

        return html;
    }

    virtual void POST(std::string& s) {
        this->setValue(s);
    }
};


/**
 * Struktura przechowujaca metody umozliwiajace realizacje 
 * pola wyboru typu radio
 * */
struct RadioButton : Widget {
    std::vector<std::string> listaWyboru;
    int index;
    std::string title;

    RadioButton() : title("Ratio Button:") { }

    virtual void setValue(int) { };

    virtual bool isDefault(int i){
        if(i == index) return true;
        else return false;
    }

    virtual int getElemsNum(){
        return listaWyboru.size();
    }

    virtual std::string getValue() {
        return listaWyboru[index];
    }

    virtual std::string getValue(int indx) {
        return listaWyboru[indx];
    }

    virtual void addChoice(std::string choice){
        listaWyboru.push_back(choice);
    }

    virtual void setTitle(std::string title_){
        title = title_;
    }

    virtual std::string GET() {
        std::stringstream id_str;
        std::stringstream i_str;
        std::stringstream count;
        
        count << this->getElemsNum();
        id_str << this ->id_;
        std::string html = "";
        html.append("<b>"+ title +"</b><br />\n");
        for(int i=0; i<this->getElemsNum(); i++){
            html.append("<input type='radio' name='");
            html.append("id"+id_str.str());
            html.append("'");
            html.append(" value='");
            html.append(""+ itoa(i));
            html.append("' ");
            if(this->isDefault(i)) html.append("checked = 'checked' ");
            html.append("id='");
            html.append(id_str.str());
            html.append("_");
            i_str.str("") ;
            i_str << i;
            html.append(i_str.str());
            html.append("'");
            html.append("/>");
            html.append(this->getValue(i));
            html.append("<br />\n");
            
            
            html.append("<input type='hidden' id='radio_count' value='");
            html.append(count.str());
            html.append("'/>\n");
            
            html.append("<input type='hidden' name='id");
            html.append(id_str.str());
            html.append("_changed' ");
            html.append("id='i");           /// id tylko dla javascriptu
            html.append(id_str.str());
            html.append("_");
            i_str.str("") ;
            i_str << i;
            html.append(i_str.str());
            html.append("_changed'");
            html.append(" value='false'/>\n");
        }
        return html;
    }

    virtual void POST(std::string& s) {
        this->setValue(atoi(s.c_str()));
    }
};


/**
 * Klasa ktora dodaje nowe funkcjonalnosci do serwera CSGI
 * */
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
    std::string isAuthed(CSGI::Env&);
    void removeSession(CSGI::Env&);
    void run();
    CSGI::Response get(CSGI::Env&, std::string&);
    virtual CSGI::Response operator()(CSGI::Env&);
};

}

#endif
