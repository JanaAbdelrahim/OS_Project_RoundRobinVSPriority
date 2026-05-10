#ifndef PROCESS_H
#define PROCESS_H

#include <string>
using namespace std;

class Process {
private:
    string pId;
    int arrivalTime;
    int burstTime;
    int priority;
    int remainingTime;
    int startTime;
    int finishTime;
    int turnaroundTime; // finish-arrival
    int waitingTime; // turnaround-burst
    int responseTime;
    bool started;

public:
    Process();
    Process(string id, int arrivalTime, int burstTime, int priority);

    string getId() const;
    int getArrivalTime() const;
    int getBurstTime() const;
    int getPriority() const;
    int getRemainingTime() const;
    int getFinishTime() const;
    int getTurnaroundTime() const;
    int getWaitingTime() const;
    int getResponseTime() const;
    int getStartTime() const;
    bool hasStarted() const;

    void setRemainingTime(int time);
    void setFinishTime(int time);
    void setTurnaroundTime(int time);
    void setWaitingTime(int time);
    void setResponseTime(int time);
    void setStartTime(int time);
    void setStarted(bool value);

    void reset();
    void computeMetrics();
};

#endif