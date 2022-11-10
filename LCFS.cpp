#include "Scheduler.h"
#include "Process.h"
#include <string>
#include<sstream>
#include <queue>
#include <cstring>
#include<sstream>

using namespace std;

class LCFS : public Scheduler {

public:
    int TimeQuantum = 100000;
    int MaxPrios = 4;
    string name = "LCFS";

    LCFS() : Scheduler() {
    }

    Process *get_next_process() {
        if (readyQ.empty()) return nullptr;

        Process *p = readyQ.back();
        readyQ.pop_back();
        return p;
    }

    void add_process(Process *p) {
        readyQ.push_back(p);
    }

    int getQuantum() {
        return TimeQuantum;
    }

    string getName() {
        return name;
    }

};





