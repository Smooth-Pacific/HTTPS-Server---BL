#pragma once

#include <fstream>
#include <iostream>

class Logging {
	private:
		std::ofstream m_file;
		clock_t lastCPU, lastSysCPU, lastUserCPU;
		int numProcessors;
		unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
	public:
		Logging(std::string file);
		~Logging();

		void run_log();
		void log();
		void init();
		double getTotalValue();
		double getProcessValue();
		int parse_line(char* line);
		int parse_data(const char* file_path, const char* field);
};