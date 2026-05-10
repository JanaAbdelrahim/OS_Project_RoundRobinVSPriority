#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H

#include "core/SchedulerBase.h"

class RoundRobin : public SchedulerBase {
private:
    int quantum;

public:
    explicit RoundRobin(int quantum);

protected:
    SimulationResult simulate(vector<Process> processes) override;
};

#endif