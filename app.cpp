#include "csgi.hpp"
#include <iostream>

class MyApp : public CSGI::Application {
    int counter_;
public:
    MyApp() : counter_(0) { }

    int increment() { return ++counter_; }

    virtual CSGI::Response operator()(CSGI::Env& env)
    {
        CSGI::Response resp;

        CSGI::Env::iterator it;

        std::cerr << "=================================================="
                  << std::endl
                  << "CAN HAS REQUEST FOR " << env["REQUEST_URI"]
                  << std::endl
                  << "=================================================="
                  << std::endl;

        for (it = env.begin(); it != env.end(); it++) {
            std::cerr << it->first << " => " << it->second << std::endl;
        }

        resp.content = "Hello, world!";
        resp.content.append(" Counter = ");
        std::stringstream counter;
        counter << counter_;
        resp.content.append(counter.str());

        std::stringstream len;
        len << resp.content.length();

        resp.status = 200;
        resp.headers["Content-Type"] = "text/plain";
        resp.headers["Content-Length"] = len.str().c_str();

        return resp;
    }
};

int main()
{
    MyApp *app = new MyApp;
    CSGI::Server srv(app, 8080);
    try {
        srv.run(true);
    } catch (CSGI::Exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    for (int i = 0; i < 10; i++) {
        sleep(1);
        std::cerr << "counter is now " << app->increment() << std::endl;
    }
    std::cerr << "App cleaning up" << std::endl;
    delete app;
    return 0;
}
