#pragma once

#include <fstream>
#include <iostream>

using namespace std;

class Logging {
	private:
		ofstream m_file;
	public:
		Logging(string file) {
			try {
				m_file.open(file, std::ios_base::app | fstream::in | fstream::out | fstream::trunc | fstream::binary);
			} catch(...) {
				cerr << "file not found";
			}
		}
		~Logging() {
			m_file.close();
		}

		void run_log() {
			while (true) {
				log();
				sleep(5);
			}
		}

		void log() {
			std::ifstream  src_meminfo("/proc/meminfo", std::ios::binary);
			std::ifstream  src_network("/proc/net/dev", std::ios::binary);
    		m_file << src_meminfo.rdbuf();
    		m_file << endl;
    		m_file << src_network.rdbuf();
    		m_file << endl << endl;
		}
};