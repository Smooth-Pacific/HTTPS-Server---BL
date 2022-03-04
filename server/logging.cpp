#include "logging.h"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include "sys/types.h"
#include "sys/sysinfo.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "sys/times.h"
#include "sys/vtimes.h"


Logging::Logging(std::string file) {
	try {
		m_file.open(file, std::fstream::in | std::fstream::out | std::fstream::app);
        init();
	} catch(...) {
		std::cerr << "file not found";
	}
}

Logging::~Logging() {
	m_file.close();
}

void Logging::run_log() {
	while (true) {
		log();
		sleep(5);
	}
}

void Logging::log() {

    auto start = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(start);
    m_file << std::ctime(&time);

    m_file << "Number of Processors: " << numProcessors << std::endl;
	double currT = getTotalValue();
	m_file << "Current overall cpu usage %: " << currT << std::endl;
    double currP = getProcessValue();
    m_file << "Current process cpu usage %: " << currP << std::endl;
    int memtotal = parse_data("/proc/meminfo", "MemTotal");
    m_file << "Total memory: " << memtotal << " kb" << std::endl;
    int memfree = parse_data("/proc/meminfo", "MemFree");
    m_file << "Memory free: " << memfree << " kb" << std::endl;
    m_file << "Memory Used: " << (memtotal - memfree) << " kb" << std::endl;
    int cached = parse_data("/proc/meminfo", "Cached");
    m_file << "Amount of RAM used as cached memory: " << cached << " kb"  << std::endl;
    int swap_cache = parse_data("/proc/meminfo", "SwapCached");
    m_file << "Amount of swap used as cached memory: " << swap_cache << std::endl;
    int swaptotal = parse_data("/proc/meminfo", "SwapTotal");
    m_file << "Total amount of swap available: " << swaptotal << std::endl;
    int swapfree = parse_data("/proc/meminfo", "SwapFree");
    m_file << "Amount of swap free: " << swapfree << std::endl;


    m_file << std::endl;
    m_file.flush();



	// std::ifstream  src_network("/proc/net/dev", std::ios::binary);
	// m_file << src_network.rdbuf();
	// m_file << std::endl << std::endl;
}

int Logging::parse_line(char* line) {
    int i = strlen(line);
    const char* p = line;
    while (*p < '0' || *p > '9') {
        p++;
    }
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

int Logging::parse_data(const char* file_path, const char* field) {
    FILE* file = fopen(file_path, "r");
    int value = -1;
    char line[128];

    while(fgets(line, 128, file) != NULL) {
        if (strncmp(line, field, 9) == 0) {
            value = parse_line(line);
            break;
        }
    }
    fclose(file);
    return value;
}



void Logging::init(){
    FILE* file1 = fopen("/proc/stat", "r");
    fscanf(file1, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
        &lastTotalSys, &lastTotalIdle);
    fclose(file1);

    FILE* file2;
    struct tms timeSample;
    char line[128];

    lastCPU = times(&timeSample);
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;

    file2 = fopen("/proc/cpuinfo", "r");
    numProcessors = 0;
    while(fgets(line, 128, file2) != NULL){
        if (strncmp(line, "processor", 9) == 0) numProcessors++;
    }
    fclose(file2);
}

double Logging::getTotalValue(){
    double percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
        &totalSys, &totalIdle);
    fclose(file);

    if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow || totalSys < lastTotalSys || totalIdle < lastTotalIdle) {
        percent = -1.0;
    }
    else{
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) + (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
    }

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    return percent;
}

double Logging::getProcessValue(){
    struct tms timeSample;
    clock_t now;
    double percent;

    now = times(&timeSample);
    if (now <= lastCPU || timeSample.tms_stime < lastSysCPU || timeSample.tms_utime < lastUserCPU) {
        percent = -1.0;
    }
    else{
        percent = (timeSample.tms_stime - lastSysCPU) + (timeSample.tms_utime - lastUserCPU);
        percent /= (now - lastCPU);
        percent /= numProcessors;
        percent *= 100;
    }
    lastCPU = now;
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;

    return percent;
}