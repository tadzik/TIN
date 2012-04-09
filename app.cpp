#include "csgi.hpp"
#include <iostream>

class MyApp : public CSGI::Application {
public:
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
    CSGI::Application *app = new MyApp;
    CSGI::Server srv(app, 8080);
    try {
        srv.run(false);
    } catch (CSGI::Exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    std::cerr << "Server running on port 8080" << std::endl;
    return 0;
}
