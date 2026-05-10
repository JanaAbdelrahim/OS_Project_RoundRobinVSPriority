#include "algorithms/PriorityScheduler.h"
#include <algorithm>
#include <climits>
#include <cassert>
#include <vector>
#include "../../include/core/Process.h"
#include "../../include/core/SimulationResult.h"

using namespace std;

PriorityScheduler::PriorityScheduler(Mode mode)
    : m_mode(mode) {}

string PriorityScheduler::name() const {
    return (m_mode == Mode::PREEMPTIVE)
               ? "Priority Scheduling (Preemptive)"
               : "Priority Scheduling (Non-Preemptive)";
}

Process* PriorityScheduler::pickHighest(vector<Process> &process, int currentTime)
{
    Process* best = nullptr;

    for (auto &p : process) {

        if (p.getArrivalTime() > currentTime || p.getRemainingTime() <= 0)
            continue;

        if (!best ||
            p.getPriority() < best->getPriority() ||
            (p.getPriority() == best->getPriority() &&
             (p.getArrivalTime() < best->getArrivalTime() ||
              (p.getArrivalTime() == best->getArrivalTime() &&
               p.getId() < best->getId()))))
        {
            best = &p;
        }
    }

    return best;
}

SimulationResult PriorityScheduler::simulate(vector<Process> process) {
    return (m_mode == Mode::PREEMPTIVE)
           ? runPreemptive(process)
           : runNonPreemptive(process);
}

// ================= PREEMPTIVE =================

SimulationResult PriorityScheduler::runPreemptive(vector<Process> &process) const
{
    int n = process.size();
    vector<GanttEntry> raw;
    int time = INT_MAX;
    int completed = 0;

    for (auto &p : process)
        time = min(time, p.getArrivalTime());

    while (completed < n) {

        Process* cur = pickHighest(process, time);

        if (!cur) {
            int nextAT = INT_MAX;

            for (auto &p : process)
                if (p.getRemainingTime() > 0)
                    nextAT = min(nextAT, p.getArrivalTime());

            raw.push_back({"IDLE", time, nextAT});
            time = nextAT;
            continue;
        }

        if (!cur->hasStarted()) {
            cur->setStarted(true);
            cur->setStartTime(time);
            cur->setResponseTime(time - cur->getArrivalTime());
        }

        raw.push_back({cur->getId(), time, time + 1});

        cur->setRemainingTime(cur->getRemainingTime() - 1);
        time++;

        if (cur->getRemainingTime() == 0) {
            cur->setFinishTime(time);
            cur->computeMetrics();
            completed++;
        }
    }

    // merge
    vector<GanttEntry> merged;
    for (auto &e : raw) {
        if (!merged.empty() && merged.back().id == e.id)
            merged.back().endTime = e.endTime;
        else
            merged.push_back(e);
    }

    // ================= AVERAGES =================
    double totalWT = 0, totalTAT = 0, totalRT = 0;

    for (auto &p : process) {
        totalWT += p.getWaitingTime();
        totalTAT += p.getTurnaroundTime();
        totalRT += p.getResponseTime();
    }

    SimulationResult result(name(), process, merged);
    result.setAverageWaitingTime(totalWT / n);
    result.setAverageTurnaroundTime(totalTAT / n);
    result.setAverageResponseTime(totalRT / n);

    return result;
}

// ================= NON PREEMPTIVE =================

SimulationResult PriorityScheduler::runNonPreemptive(vector<Process> &process)
{
    int n = process.size();
    vector<GanttEntry> gantt;
    int time = INT_MAX;
    int completed = 0;

    for (auto &p : process)
        time = min(time, p.getArrivalTime());

    while (completed < n) {

        Process* cur = pickHighest(process, time);

        if (!cur) {
            int nextAT = INT_MAX;

            for (auto &p : process)
                if (p.getRemainingTime() > 0)
                    nextAT = min(nextAT, p.getArrivalTime());

            gantt.push_back({"IDLE", time, nextAT});
            time = nextAT;
            continue;
        }

        if (!cur->hasStarted()) {
            cur->setStarted(true);
            cur->setStartTime(time);
            cur->setResponseTime(time - cur->getArrivalTime());
        }

        int runTime = cur->getRemainingTime();

        gantt.push_back({cur->getId(), time, time + runTime});

        time += runTime;

        cur->setRemainingTime(0);
        cur->setFinishTime(time);

        cur->computeMetrics();
        completed++;
    }

    // ================= AVERAGES =================
    double totalWT = 0, totalTAT = 0, totalRT = 0;

    for (auto &p : process) {
        totalWT += p.getWaitingTime();
        totalTAT += p.getTurnaroundTime();
        totalRT += p.getResponseTime();
    }

    SimulationResult result(name(), process, gantt);
    result.setAverageWaitingTime(totalWT / n);
    result.setAverageTurnaroundTime(totalTAT / n);
    result.setAverageResponseTime(totalRT / n);

    return result;
}