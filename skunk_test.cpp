#include "Skunk.hpp"
#include <string>
#include <vector>



/**
 * @file skunk_test.cpp
 * */



/**
 * Klasa dziedziczaca po TextField, umozliwia odczytywanie i ustawianie nowej
 * wartosci pola tesktowego
 * */
class PoleTekstowe : public Skunk::TextField {
    std::string wartosc_;

public:
    PoleTekstowe(std::string w) : wartosc_(w) { }           

    virtual std::string getValue() {                        /// odczytuje wartosc pola tekstowego
        return wartosc_;
    }

    virtual void setValue(std::string& neu) {               /// nadaje nowa wartosc polu tekstowemu
        wartosc_ = neu;
    }
};


/**
 * Klasa umozliwiajaca nadawanie polu typu radio nowych wartosci
 * */
class PoleRadiowe : public Skunk::RadioButton {
public:
    
    virtual void setValue(int neu) {                        /// nadaje nowa wartosc polu typu radio
        index = neu;
    }

};


/**
 * Klasa naglowka 
 * */
class WielkiNapis : public Skunk::Widget {
    virtual std::string GET() {
        return "<h1><b>TIN HTTP APP</b></h1>";
    }
    virtual void POST(std::string&) { }
};


/**
 * Main - tworzy nowy server z dostepnymi widgetami i uruchamia go
 * */
int main(void) {
    Skunk::Server *srv = new Skunk::Server();
    PoleTekstowe *pt1 = new PoleTekstowe("text1");
    PoleTekstowe *pt2 = new PoleTekstowe("text2");
    srv->addWidget(new WielkiNapis());
    srv->addWidget(pt1);
    srv->addWidget(pt2);
    PoleRadiowe *pr = new PoleRadiowe();
    srv->addWidget(pr);
    pr->setTitle("Preferencje");
    pr->addChoice("Opcja1");
    pr->addChoice("Opcja2");
    pr->addChoice("Opcja3");

    Skunk::SimpleAuth *auth = new Skunk::SimpleAuth();
    auth->addUser("admin", "admin.8");
    auth->addUser("user",  "user.8");

    srv->setAuth(auth);

    printf("running server\n");
    srv->run(true);
    
    int i=0;
    std::string napis = "kurwa";
    std::string tmp;
    for(;;){
        sleep(10);
        //pt1->blockWidget();
        std::cout << pt1->getValue() << std::endl;
        tmp = pt1->getValue() + itoa(i++);
        pt1->setValue(tmp);
        //pt1->unlockWidget();
        //printf("%s: \n",pt1->getValue());
    }
}
