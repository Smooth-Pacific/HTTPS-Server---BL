#include <httpserver.hpp>
#include <cstdlib>
#include <iostream>
#include "logging.h"
#include <thread>

using namespace httpserver;

class hello_world_resource : public http_resource {
public:
    const std::shared_ptr<http_response> render(const http_request&) {
        return std::shared_ptr<http_response>(new string_response("Hello, World!\n"));
    }
};

int getEnvVar( const std::string& key ) {
    char * val = getenv(key.c_str());
    return val == NULL ? 0 : std::stoi(std::string(val));
}

int main(int argc, char** argv) {
    int port = getEnvVar("PORT");
    int max_connections = getEnvVar("MAX_CONNECTIONS");
    int content_size = getEnvVar("CONTENT_SIZE");
    int timeout = getEnvVar("TIMEOUT");
    int mem_limit = getEnvVar("MEMORY_LIMIT");
    int connection_limit = getEnvVar("CONNECTION_LIMIT");

    // std::string s = "log.txt";
    // Logging log(s);
    // std::thread th([&log]() {
    //     int i = 5;
    //     while(i > 0) {
    //         i--;
    //         log.log();
    //     }
    // });


    webserver ws = create_webserver()
        .port(port)
        .max_connections(max_connections)
        .content_size_limit(content_size)
        .connection_timeout(timeout)
        .memory_limit(mem_limit)
        .per_IP_connection_limit(connection_limit)
        .use_dual_stack()
        .use_ssl()
        .max_threads(40)
        .https_mem_key("../certs/server.key")
        .https_mem_cert("../certs/server.crt");

    hello_world_resource hwr;
    ws.register_resource("/", &hwr);
    ws.start(true);
    // th.join();
    // ws.stop();
    return 0;
}



// curl -XGET -v -k 'https://localhost:8080'
