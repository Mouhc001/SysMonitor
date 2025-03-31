#include "cpu_monitor_ui.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>
#include <QtWidgets/QLabel>
#include <QTimer>
#include <fstream>
#include <sstream>
#include <iostream>

CPUData CPUMonitor::getCPUData() {
    std::ifstream file("/proc/stat");
    std::string line;
    CPUData cpu = {0};
    
    if (file.is_open() && std::getline(file, line)) {
        std::istringstream iss(line);
        std::string cpuLabel;
        iss >> cpuLabel >> cpu.user >> cpu.nice >> cpu.system >> cpu.idle
            >> cpu.iowait >> cpu.irq >> cpu.softirq >> cpu.steal;
    }
    return cpu;
}

float CPUMonitor::calculateCPUUsage(const CPUData& prev, const CPUData& current) {
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

CPUMonitor::CPUMonitor(QWidget *parent) : QMainWindow(parent), prevData(getCPUData()) {
    setWindowTitle("CPU Usage Monitor");
    resize(300, 100);

    // Create a QLabel to display the CPU usage
    cpuUsageLabel = new QLabel("CPU Usage: 0%", this);
    cpuUsageLabel->setAlignment(Qt::AlignCenter);
    cpuUsageLabel->setStyleSheet("font-size: 24px; font-weight: bold;");

    // Create a QVBoxLayout and add the label
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(cpuUsageLabel);

    // Set the layout on a QWidget
    QWidget *centralWidget = new QWidget();
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Set up the timer to update CPU usage every second
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CPUMonitor::updateCPUUsage);
    timer->start(1000);  // 1000 ms = 1 second
}

CPUMonitor::~CPUMonitor() {}

void CPUMonitor::updateCPUUsage() {
    CPUData currentData = getCPUData();
    float usage = calculateCPUUsage(prevData, currentData);
    prevData = currentData;
    
    // Update the label with the new CPU usage
    cpuUsageLabel->setText(QString("CPU Usage: %1%").arg(usage, 0, 'f', 2));  // Show 2 decimal points
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CPUMonitor monitor;
    monitor.show();

    return app.exec();
}
