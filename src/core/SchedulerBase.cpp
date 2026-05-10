#include "core/SchedulerBase.h"
using namespace std;

SimulationResult SchedulerBase::run(const vector<Process>& processes) {
    vector<Process> copy = processes;
    for (auto& p : copy) p.reset();
    return simulate(copy);
}