#ifndef SCHEDULERBASE_H
#define SCHEDULERBASE_H

#include <vector>
#include "../core/Process.h"
#include "../core/SimulationResult.h"
using namespace std;

class SchedulerBase {
public:
    virtual ~SchedulerBase() = default;
    SimulationResult run(const std::vector<Process>& processes);
protected:
    virtual SimulationResult simulate(vector<Process> processes) = 0;
};

#endif