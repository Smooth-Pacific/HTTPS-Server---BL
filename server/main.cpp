#include <httpserver.hpp>
#include <cstdlib>
#include <iostream>
#include "logging.h"
#include "startConfig.h"
#include <thread>

using namespace httpserver;

#define MY_OPAQUE "11733b200778ce33060f31c9af70a870ba96ddd4"

class server_resource : public http_resource {
public:
    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req) {
        bool reload_nonce = false;
        if (req.get_digested_user() == "myuser" && req.check_digest_auth("MyRealm", "mypass", 300, &reload_nonce)) {
            return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("SUCCESS, Hello World!\n", 200, "text/plain"));
        } else {
            return std::shared_ptr<httpserver::digest_auth_fail_response>(new httpserver::digest_auth_fail_response("FAIL\n", "MyRealm", MY_OPAQUE, reload_nonce));
        }
    }
};


int main(int argc, char** argv) {
    startConfig sc;
    create_webserver cw;
    
    cw.start_method(http::http_utils::INTERNAL_SELECT);
    cw.port(sc.get_port());
    cw.max_connections(sc.get_max_connections());
    cw.content_size_limit(sc.get_content_size());
    cw.connection_timeout(sc.get_timeout());
    cw.memory_limit(sc.get_mem_limit());
    cw.per_IP_connection_limit(sc.get_connection_limit());
    cw.max_threads(sc.get_max_threads());

    if (sc.get_dual() == 1) {
        cw.use_dual_stack();
    }

    cw.use_ssl();
    cw.https_mem_key(sc.get_mem_key());
    cw.https_mem_cert(sc.get_mem_cert());
    cw.digest_auth();


    webserver ws = cw;


    server_resource hwr;
    ws.register_resource("/", &hwr);
    ws.start(true);
    return 0;
}



// curl -XGET -v --digest --user myuser:mypass -k 'https://localhost:8080'



