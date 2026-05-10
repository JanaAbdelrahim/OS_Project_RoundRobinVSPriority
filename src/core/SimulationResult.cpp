#include "core/SimulationResult.h"
#include <iostream>
#include <iomanip>
#include <numeric>

SimulationResult::SimulationResult() {
    averageWaitingTime = 0;
    averageTurnaroundTime = 0;
    averageResponseTime = 0;
}
SimulationResult::SimulationResult(const string& name, vector<Process>& processes, vector<GanttEntry>& ganttChart) {
    this->algorithmName = name;
    this->processes = processes;
    this->ganttChart = ganttChart;
    averageWaitingTime = 0;
    averageTurnaroundTime = 0;
    averageResponseTime = 0;
}

string SimulationResult::getAlgorithmName() const {
    return algorithmName;
}
vector<Process> SimulationResult::getProcesses() const {
    return processes;
}
vector<GanttEntry> SimulationResult::getGanttChart() const {
    return ganttChart;
}
double SimulationResult::getAverageWaitingTime() const {
    return averageWaitingTime;
}
double SimulationResult::getAverageTurnaroundTime() const {
    return averageTurnaroundTime;
}
double SimulationResult::getAverageResponseTime() const {
    return averageResponseTime;
}

void SimulationResult::setAlgorithmName(string& algorithmName) {
    this->algorithmName = algorithmName;
}
void SimulationResult::setProcesses(const vector<Process>& processes) {
    this->processes = processes;
}
void SimulationResult::setGanttChart(const vector<GanttEntry>& chart) {
    ganttChart = chart;
}
void SimulationResult::setAverageWaitingTime(double time) {
    averageWaitingTime = time;
}
void SimulationResult::setAverageTurnaroundTime(double time) {
    averageTurnaroundTime = time;
}
void SimulationResult::setAverageResponseTime(double time) {
    averageResponseTime = time;
}