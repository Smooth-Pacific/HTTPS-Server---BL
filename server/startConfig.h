#include <string>

class startConfig {
	private:
		int port;
		int max_connections;
		int content_size;
		int timeout;
		int mem_limit;
		int connection_limit;
		int max_threads;
		int use_dual;
		std::string mem_key;
		std::string mem_cert;
	public:
		startConfig();
		int get_port();
		int get_max_connections();
		int get_content_size();
		int get_timeout();
		int get_mem_limit();
		int get_connection_limit();
		int get_max_threads();
		int get_dual();
		std::string get_mem_key();
		std::string get_mem_cert();
};