#include <cstdlib>
#include <thread>
#include <string>
#include "startConfig.h"


startConfig::startConfig() {
	port = getenv("PORT") ? static_cast<int>(std::stoi(getenv("PORT"))) : 8080;

	max_connections = getenv("MAX_CONNECTIONS") ? static_cast<int>(std::stoi(getenv("MAX_CONNECTIONS"))) : 20;

	content_size = getenv("CONTENT_SIZE") ? static_cast<int>(std::stoi(getenv("CONTENT_SIZE"))) : 1;
	
	timeout = getenv("TIMEOUT") ? static_cast<int>(std::stoi(getenv("TIMEOUT"))) : 120;

	mem_limit = getenv("MEMORY_LIMIT") ? static_cast<int>(std::stoi(getenv("MEMORY_LIMIT"))) : 32000;
	
	connection_limit = getenv("CONNECTION_LIMIT") ? static_cast<int>(std::stoi(getenv("CONNECTION_LIMIT"))) : 0;
	
	max_threads = getenv("MAX_THREADS") ? static_cast<int>(std::stoi(getenv("MAX_THREADS"))) : 1;

	use_dual = getenv("DUAL") ? static_cast<int>(std::stoi(getenv("DUAL"))) : 0;

    mem_key = getenv("MEM_KEY") ? static_cast<std::string>(getenv("MEM_KEY")) : "../certs/server.key";
    
    mem_cert = getenv("MEM_CERT") ? static_cast<std::string>(getenv("MEM_CERT")) : "../certs/server.cert";
}

int startConfig::get_port() { return port; }

int startConfig::get_max_connections() { return max_connections; }

int startConfig::get_content_size() { return content_size; }

int startConfig::get_timeout() { return timeout; }

int startConfig::get_mem_limit() { return mem_limit; }

int startConfig::get_connection_limit() { return connection_limit; }

int startConfig::get_dual() { return use_dual; }

std::string startConfig::get_mem_key() { return mem_key; }

std::string startConfig::get_mem_cert() { return mem_cert; }

int startConfig::get_max_threads() { 
	const int concurrent_threads = std::thread::hardware_concurrency();
    if (concurrent_threads > 4){
        return max_threads;
    }
    return 1;
}