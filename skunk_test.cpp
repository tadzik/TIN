#include "Skunk.hpp"
#include <string>
#include <vector>

class PoleTekstowe : public Skunk::TextField {
    std::string wartosc_;

public:
    PoleTekstowe(std::string w) : wartosc_(w) { }

    virtual std::string getValue() {
        return wartosc_;
    }

    virtual void setValue(std::string& neu) {
        wartosc_ = neu;
    }
};

class PoleRadiowe : public Skunk::RadioButton {
public:
    
    virtual void setValue(int neu) {
        index = neu;
    }

};

class WielkiNapis : public Skunk::Widget {
    virtual std::string GET() {
        return "<h1><b>LOL WTF</b></h1>";
    }
    virtual void POST(std::string&) { }
};

int main(void) {
    Skunk::Server *srv = new Skunk::Server();
    srv->addWidget(new WielkiNapis());
    srv->addWidget(new PoleTekstowe("dupa"));
    srv->addWidget(new PoleTekstowe("cycki"));
    PoleRadiowe *pr = new PoleRadiowe();
    srv->addWidget(pr);
    pr->setTitle("Preferencje dupczane");
    pr->addChoice("dupeczka");
    pr->addChoice("dupcia");
    pr->addChoice("dupiszon");

    Skunk::SimpleAuth *auth = new Skunk::SimpleAuth();
    auth->addUser("admin", "dupa.8");
    auth->addUser("user",  "dupa.8");

    srv->setAuth(auth);

    srv->run();
}
