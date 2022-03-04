#include <cstdlib>
#include <thread>
#include <string>
#include "startConfig.h"


startConfig::startConfig() {
	port = getenv("PORT") ? static_cast<int>(std::stoi(getenv("PORT"))) : 8080;
	if (port < 0 || port > 65535) {
		throw std::runtime_error("invalid port number");
	}

	max_connections = getenv("MAX_CONNECTIONS") ? static_cast<int>(std::stoi(getenv("MAX_CONNECTIONS"))) : 15;
	if (max_connections < 1 || max_connections > FD_SETSIZE - 4) {
		throw std::runtime_error("invalid number of connections");
	}

	content_size = getenv("CONTENT_SIZE") ? static_cast<int>(std::stoi(getenv("CONTENT_SIZE"))) : 1;
	if (content_size < -1) {
		throw std::runtime_error("invalid content size");
	}
	
	timeout = getenv("TIMEOUT") ? static_cast<int>(std::stoi(getenv("TIMEOUT"))) : 120;
	if (timeout < 0) {
		std::runtime_error("invalid timeout length");
	}

	mem_limit = getenv("MEMORY_LIMIT") ? static_cast<int>(std::stoi(getenv("MEMORY_LIMIT"))) : 32000;
	if (mem_limit < 0 || mem_limit > 128000) {
		throw std::runtime_error("invalid memory limit");
	}
	
	connection_limit = getenv("CONNECTION_LIMIT") ? static_cast<int>(std::stoi(getenv("CONNECTION_LIMIT"))) : 0;
	if (connection_limit < 0) {
		throw std::runtime_error("invalid connection limit");
	}
	
	max_threads = getenv("MAX_THREADS") ? static_cast<int>(std::stoi(getenv("MAX_THREADS"))) : 1;
	if (max_threads < 1) {
		throw std::runtime_error("invalid number of threads");
	} 

	use_dual = getenv("DUAL") ? static_cast<int>(std::stoi(getenv("DUAL"))) : 0;
	if (use_dual != 0 && use_dual != 1) {
		throw std::runtime_error("invalid input for dual stack");
	}

    mem_key = getenv("MEM_KEY") ? static_cast<std::string>(getenv("MEM_KEY")) : "";
    
    mem_cert = getenv("MEM_CERT") ? static_cast<std::string>(getenv("MEM_CERT")) : "";
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