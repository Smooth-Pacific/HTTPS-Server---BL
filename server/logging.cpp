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

/* Constructor */
Logging::Logging(std::string file) {
	try {
		m_file.open(file, std::fstream::in | std::fstream::out | std::fstream::app);
        init();
	} catch(...) {
        throw std::runtime_error("log not created");
	}
}

/* Closes file on destruction */
Logging::~Logging() {
	m_file.close();
}

/* Runs log every 5 seconds */
void Logging::run_log() {
	while (true) {
		log();
		sleep(5);
	}
}

/* Logs info about time, cpu, memory, etc */
void Logging::log() {

    auto start = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(start);
    m_file << std::ctime(&time);

    m_file << "Number of Processors: " << numProcessors << "\n";
	double currT = getTotalValue();
	m_file << "Current overall cpu usage %: " << currT << "\n";
    double currP = getProcessValue();
    m_file << "Current process cpu usage %: " << currP << "\n";
    int memtotal = parse_data("/proc/meminfo", "MemTotal");
    m_file << "Total memory: " << memtotal << " kb" << "\n";
    int memfree = parse_data("/proc/meminfo", "MemFree");
    m_file << "Memory free: " << memfree << " kb" << "\n";
    m_file << "Memory Used: " << (memtotal - memfree) << " kb" << "\n";
    int cached = parse_data("/proc/meminfo", "Cached");
    m_file << "Amount of RAM used as cached memory: " << cached << " kb"  << "\n";
    int swap_cache = parse_data("/proc/meminfo", "SwapCached");
    m_file << "Amount of swap used as cached memory: " << swap_cache << "\n";
    int swaptotal = parse_data("/proc/meminfo", "SwapTotal");
    m_file << "Total amount of swap available: " << swaptotal << "\n";
    int swapfree = parse_data("/proc/meminfo", "SwapFree");
    m_file << "Amount of swap free: " << swapfree << "\n";


    m_file << "\n";
    m_file.flush();
}

/* Parses line */
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

/* Parses line in proc filesystem */
int Logging::parse_data(const char* file_path, const char* field) {
    FILE* file = fopen(file_path, "r");
    int value = -1;
    char line[128];

    while(fgets(line, 128, file) != NULL) {
        if (strncmp(line, field, 6) == 0) {
            value = parse_line(line);
            break;
        }
    }
    fclose(file);
    return value;
}


/* Gets inital conditions */
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

/* Gets cpu info */
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

/* Get current value by comparing with old values */
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