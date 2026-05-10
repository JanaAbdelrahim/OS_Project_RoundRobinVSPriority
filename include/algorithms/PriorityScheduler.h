#pragma once
#include "../core/SchedulerBase.h"
using namespace std;
class PriorityScheduler : public SchedulerBase {
public:
    enum class Mode { PREEMPTIVE, NON_PREEMPTIVE };

    explicit PriorityScheduler(Mode mode = Mode::PREEMPTIVE);

    string name() const;
    Mode mode() const {
        return m_mode;
    }

protected:
    SimulationResult simulate(vector<Process> procs) override;

private:
    Mode m_mode;
    static Process* pickHighest(vector<Process>& procs, int currentTime);

    SimulationResult runPreemptive   (vector<Process>& procs) const;
    SimulationResult runNonPreemptive(vector<Process>& procs);
};