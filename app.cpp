#include "csgi.hpp"
#include <iostream>

CSGI::Response app(CSGI::Env& env)
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

    std::string body = "Hello, world!";
    std::stringstream len;
    len << body.length();
    resp.first = 200;
    resp.second.first["Content-Type"] = "text/plain";
    resp.second.first["Content-Length"] = len.str().c_str();
    resp.second.second = body;

    return resp;
}

int main()
{
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
