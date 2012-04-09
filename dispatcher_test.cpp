#include "dispatcher.hpp"
#include <cassert>
#include <sstream>
#include <iostream>

std::string itoa(int i)
{
    std::stringstream out;
    out << i;
    return out.str();
}

CSGI::Response make_response(std::string body)
{
    CSGI::Response ret;

    ret.status = 200;
    ret.headers["Content-Type"]   = "text/plain";
    ret.headers["Content-Length"] = itoa(body.length());
    ret.content = body;

    return ret;
}

CSGI::Response hello_handler(CSGI::Env& e)
{
    return make_response("Hello, world!");
    (void)e;
}

CSGI::Response aboot_handler(CSGI::Env& e)
{
    return make_response("Could you tell us again what your argument "
                         "is all ABOUT?");
    (void)e;
}

CSGI::Response placki_handler(CSGI::Env& e)
{
    return make_response("Lubię placki!");
    (void)e;
}

CSGI::Response another_handler(CSGI::Env& e)
{
    return make_response("Another prick in the wall");
    (void)e;
}

void assert_status(Dispatcher& d, const char * uri, int status)
{
    CSGI::Env env;
    env["REQUEST_URI"] = std::string(uri);
    assert(d.dispatch(env).status == status);
}

void assert_found(Dispatcher& d, const char * uri)
{
    assert_status(d, uri, 200);
}

int main()
{
    Dispatcher dispatcher(NULL);

    dispatcher.add_handler("/hello",   hello_handler);
    dispatcher.add_handler("aboot/",   aboot_handler);
    dispatcher.add_handler("/placki/", placki_handler);
    dispatcher.add_handler("another",  another_handler);

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
