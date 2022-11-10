#include "Scheduler.h"
#include "Process.h"
#include <string>
using namespace std;

class RR : public Scheduler {

public:
    int TimeQuantum = 100000;
    int MaxPrios = 4;
    string name = "RR";

    RR() : Scheduler() {
    }

    RR(int quantum) : Scheduler() {
        TimeQuantum=quantum;
    }

    Process *get_next_process() {
        if (readyQ.empty()) return nullptr;

        Process *p = readyQ.front();
        readyQ.pop_front();
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





