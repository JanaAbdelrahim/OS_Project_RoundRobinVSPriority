#ifndef SIMULATIONRESULT_H
#define SIMULATIONRESULT_H

#include <vector>
#include "../core/Process.h"
#include "../core/GanttEntry.h"
using namespace std;

class SimulationResult {
private:
    string algorithmName;
    vector<Process> processes;
    vector<GanttEntry> ganttChart;
    double averageWaitingTime;
    double averageTurnaroundTime;
    double averageResponseTime;

public:
    SimulationResult();
    SimulationResult(const string& name,vector<Process>& processes, vector<GanttEntry>& ganttChart);
    string getAlgorithmName() const;
    vector<Process> getProcesses() const;
    vector<GanttEntry> getGanttChart() const;
    double getAverageWaitingTime() const;
    double getAverageTurnaroundTime() const;
    double getAverageResponseTime() const;

    void setAlgorithmName(string& algorithmName);
    void setProcesses(const vector<Process>& processes);
    void setGanttChart(const vector<GanttEntry>& chart);
    void setAverageWaitingTime(double time);
    void setAverageTurnaroundTime(double time);
    void setAverageResponseTime(double time);
};

#endif