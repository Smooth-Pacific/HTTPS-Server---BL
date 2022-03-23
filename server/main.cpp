#include <httpserver.hpp>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <fstream>
#include "logging.h"
#include "startConfig.h"
#include "server_resource.h"
#include "transactions_resource.h"


using namespace httpserver;


int main(int argc, char** argv) {

    Logging log("log.txt");
    std::thread th(&Logging::run_log, &log);
    th.detach();

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
    mime_resource mr;
    transaction_resource tr;
    ws.register_resource("/", &hwr);
    ws.register_resource("/mime/{arg1}", &mr);
    ws.register_resource("/transaction/{arg1}", &tr);
    ws.start(true);
    return 0;
}




