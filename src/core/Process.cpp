#include "core/Process.h"

Process::Process() {
    pId = "";
    arrivalTime = 0;
    burstTime = 0;
    priority = 0;
    remainingTime = 0;
    finishTime = 0;
    turnaroundTime = 0;
    waitingTime = 0;
    responseTime = 0;
    startTime = -1;
    started = false;
}

Process::Process(string pID, int arrivalTime, int burstTime, int priority) {
    this->pId = pID;
    this->arrivalTime = arrivalTime;
    this->burstTime = burstTime;
    this->priority = priority;
    remainingTime = burstTime;
    finishTime = 0;
    turnaroundTime = 0;
    waitingTime = 0;
    responseTime = 0;
    startTime = -1;
    started = false;
}

string Process::getId() const {
    return pId;
}
int Process::getArrivalTime() const {
    return arrivalTime;
}
int Process::getBurstTime() const {
    return burstTime;
}
int Process::getPriority() const {
    return priority;
}
int Process::getRemainingTime() const {
    return remainingTime;
}
int Process::getFinishTime() const {
    return finishTime;
}
int Process::getTurnaroundTime() const {
    return turnaroundTime;
}
int Process::getWaitingTime() const {
    return waitingTime;
}
int Process::getResponseTime() const {
    return responseTime;
}
int Process::getStartTime() const {
    return startTime;
}
bool Process::hasStarted() const {
    return started;
}

void Process::setRemainingTime(int time) {
    remainingTime = time;
}
void Process::setFinishTime(int time) {
    finishTime = time;
}
void Process::setTurnaroundTime(int time) {
    turnaroundTime = time;
}
void Process::setWaitingTime(int time) {
    waitingTime = time;
}
void Process::setResponseTime(int time) {
    responseTime = time;
}
void Process::setStartTime(int time) {
    startTime = time;
}
void Process::setStarted(bool value) {
    started = value;
}

void Process::reset() {
    remainingTime = burstTime;
    startTime = -1;
    finishTime = 0;
    turnaroundTime = 0;
    waitingTime = 0;
    responseTime = 0;
    started = false;
}
void Process::computeMetrics() {
    turnaroundTime = finishTime - arrivalTime;
    waitingTime    = turnaroundTime - burstTime;
    responseTime   = startTime - arrivalTime;
}