#ifndef GANTTENTRY_H
#define GANTTENTRY_H
#include <string>
using namespace std;

struct GanttEntry{
    string id;
    int startTime;
    int endTime;

    GanttEntry() = default;
    GanttEntry(string pId, int start, int end) {
        id = pId;
        startTime = start;
        endTime = end;
    }

    int duration() const{
        return endTime - startTime;
    }
};

#endif