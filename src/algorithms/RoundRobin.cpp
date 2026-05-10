#include "algorithms/RoundRobin.h"
#include <queue>
#include <algorithm>

RoundRobin::RoundRobin(int quantum) {
    this->quantum = quantum;
}

SimulationResult RoundRobin::simulate(vector<Process> processes) {
    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        return a.getArrivalTime() < b.getArrivalTime();
    });

    queue<int> q;
    vector<GanttEntry> gantt;

    int n = processes.size();
    int currentTime = 0;
    int completed = 0;

    vector<bool> inQueue(n, false);

    double totalWT = 0;
    double totalTAT = 0;
    double totalRT = 0;


    for (int i = 0; i < n; i++) {
        if (processes[i].getArrivalTime() <= currentTime) {
            q.push(i);
            inQueue[i] = true;
        }
    }

    while (completed < n) {


        if (q.empty()) {
            currentTime++;

            for (int i = 0; i < n; i++) {
                if (!inQueue[i] &&
                    processes[i].getArrivalTime() <= currentTime &&
                    processes[i].getRemainingTime() > 0) {

                    q.push(i);
                    inQueue[i] = true;
                }
            }
            continue;
        }

        int idx = q.front();
        q.pop();
        inQueue[idx] = false;

        Process &process = processes[idx];


        if (!process.hasStarted()) {
            process.setStarted(true);
            process.setStartTime(currentTime);
            process.setResponseTime(currentTime - process.getArrivalTime());
        }

        int executionTime = min(quantum, process.getRemainingTime());


        gantt.push_back(
            GanttEntry(
                process.getId(),
                currentTime,
                currentTime + executionTime
            )
        );

        currentTime += executionTime;
        process.setRemainingTime(process.getRemainingTime() - executionTime);


        for (int i = 0; i < n; i++) {
            if (!inQueue[i]
                && processes[i].getArrivalTime() <= currentTime
                && processes[i].getRemainingTime() > 0
                && i != idx) {

                q.push(i);
                inQueue[i] = true;
            }
        }


        if (process.getRemainingTime() > 0) {
            q.push(idx);
            inQueue[idx] = true;
        }
        else {
            process.setFinishTime(currentTime);
            process.computeMetrics();
            completed++;
        }
    }


    for (auto &p : processes) {
        totalWT += p.getWaitingTime();
        totalTAT += p.getTurnaroundTime();
        totalRT += p.getResponseTime();
    }

    SimulationResult result;
    result.setProcesses(processes);
    result.setGanttChart(gantt);
    result.setAverageWaitingTime(totalWT / n);
    result.setAverageTurnaroundTime(totalTAT / n);
    result.setAverageResponseTime(totalRT / n);

    return result;
}