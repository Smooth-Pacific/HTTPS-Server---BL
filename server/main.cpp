#include <httpserver.hpp>

using namespace httpserver;

class hello_world_resource : public http_resource {
public:
    const std::shared_ptr<http_response> render(const http_request&) {
        return std::shared_ptr<http_response>(new string_response("Hello, World!\n"));
    }
};

int main(int argc, char** argv) {
    webserver ws = create_webserver(8080)
        .use_ssl()
        .https_mem_key("../certs/server.test.key")
        .https_mem_cert("../certs/server.test.crt");

    hello_world_resource hwr;
    ws.register_resource("/hello", &hwr);
    ws.start(true);
    return 0;
}