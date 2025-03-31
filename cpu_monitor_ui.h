#ifndef CPU_MONITOR_UI_H
#define CPU_MONITOR_UI_H

#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QTimer>

struct CPUData {
    long user, nice, system, idle, iowait, irq, softirq, steal;
};

class CPUMonitor : public QMainWindow {
    Q_OBJECT

public:
    CPUMonitor(QWidget *parent = nullptr);
    ~CPUMonitor();

private slots:
    void updateCPUUsage();

private:
    CPUData getCPUData();
    float calculateCPUUsage(const CPUData& prev, const CPUData& current);

    QTimer *timer;
    QLabel *cpuUsageLabel;
    CPUData prevData;
};

#endif // CPU_MONITOR_UI_H
