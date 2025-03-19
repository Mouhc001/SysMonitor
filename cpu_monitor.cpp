#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <chrono>

struct CPUData {
    long user, nice, system, idle, iowait, irq, softirq, steal;
};

CPUData getCPUData() {
    std::ifstream file("/proc/stat");
    std::string line;
    CPUData cpu;
    
    if (file.is_open() && std::getline(file, line)) {
        std::istringstream iss(line);
        std::string cpuLabel;
        iss >> cpuLabel >> cpu.user >> cpu.nice >> cpu.system >> cpu.idle
            >> cpu.iowait >> cpu.irq >> cpu.softirq >> cpu.steal;
    }
    return cpu;
}

float calculateCPUUsage(const CPUData& prev, const CPUData& current) {
    long prevIdle = prev.idle + prev.iowait;
    long idle = current.idle + current.iowait;
    
    long prevNonIdle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    long nonIdle = current.user + current.nice + current.system + current.irq + current.softirq + current.steal;
    
    long prevTotal = prevIdle + prevNonIdle;
    long total = idle + nonIdle;
    
    float totalDiff = total - prevTotal;
    float idleDiff = idle - prevIdle;
    
    return (totalDiff - idleDiff) / totalDiff * 100.0;
}

int main() {
    std::cout << "Monitoring CPU usage..." << std::endl;
    CPUData prevData = getCPUData();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    while (true) {
        CPUData currentData = getCPUData();
        float usage = calculateCPUUsage(prevData, currentData);
        std::cout << "CPU Usage: " << usage << " %" << std::endl;
        prevData = currentData;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
