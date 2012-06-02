#include "dispatcher.hpp"
#include <cassert>
#include <sstream>
#include <iostream>

static std::string itoa(int i) {
    std::stringstream str;
    str << i;
    return str.str();
}

class SimpleResponder : public CSGI::Application {
public:
    SimpleResponder(std::string msg) : msg_(msg) { }

    virtual CSGI::Response operator()(CSGI::Env& e)
    {
        CSGI::Response ret;

        ret.status = 200;
        ret.headers["Content-Type"]   = "text/plain";
        ret.headers["Content-Length"] = itoa(msg_.length());
        ret.content = msg_;

        return ret;
        (void)e;
    }
private:
    std::string msg_;
};

void assert_status(Dispatcher& d, const char * uri, int status)
{
    CSGI::Env env;
    env["REQUEST_URI"] = std::string(uri);
    assert(d(env).status == status);
}

void assert_found(Dispatcher& d, const char * uri)
{
    assert_status(d, uri, 200);
}

int main()
{
    Dispatcher dispatcher((CSGI::Application *)NULL);

    SimpleResponder hello("Hello, world!");
    SimpleResponder aboot("Could you tell us again what your argument "
                          "is all ABOUT?");
    SimpleResponder placki("Lubię placki!");
    SimpleResponder another("Another prick in the wall");

    dispatcher.add_handler("/hello",   &hello);
    dispatcher.add_handler("aboot/",   &aboot);
    dispatcher.add_handler("/placki/", &placki);
    dispatcher.add_handler("another",  &another);

    assert_found(dispatcher, "/hello");
    assert_found(dispatcher, "/hello/");
    assert_found(dispatcher,  "hello/");
    assert_found(dispatcher,  "hello");

    assert_found(dispatcher, "/aboot");
    assert_found(dispatcher, "/aboot/");
    assert_found(dispatcher,  "aboot/");
    assert_found(dispatcher,  "aboot");

    assert_found(dispatcher, "/placki");
    assert_found(dispatcher, "/placki/");
    assert_found(dispatcher,  "placki/");
    assert_found(dispatcher,  "placki");

    assert_found(dispatcher, "/another");
    assert_found(dispatcher, "/another/");
    assert_found(dispatcher,  "another/");
    assert_found(dispatcher,  "another");

    assert_status(dispatcher, "dupa", 404);

    std::cerr << "All tests OK" << std::endl;

    return 0;
}
