#include "Scheduler.h"
#include "Process.h"
#include <string>
#include<sstream>
#include <queue>
#include <cstring>
#include<sstream>

using namespace std;

class PRIO : public Scheduler {

public:
    int TimeQuantum = 100000;
    int MaxPrios = 4;
    string name = "PRIO";

    vector<queue<Process *>> *activeQ;
    vector<queue<Process *>> *expiredQ;

    PRIO() : Scheduler() {
    }

    PRIO(int quantum, int maxprio) : Scheduler() {
        TimeQuantum = quantum;
        MaxPrios = maxprio;
        activeQ = new vector<queue<Process *>>(MaxPrios, queue<Process *>());
        expiredQ = new vector<queue<Process *>>(MaxPrios, queue<Process *>());
    }

    PRIO(int quantum) : Scheduler() {
        TimeQuantum = quantum;
        activeQ = new vector<queue<Process *>>(MaxPrios, queue<Process *>());
        expiredQ = new vector<queue<Process *>>(MaxPrios, queue<Process *>());
    }

    void setName(string new_name){
        name=new_name;
    }

    Process *get_next_process() {
        Process *p;
        for (int i = MaxPrios - 1; i >= 0; i--) {
            if (activeQ->at(i).empty()) {
                continue;
            } else {
                p = activeQ->at(i).front();
                activeQ->at(i).pop();
                return p;
            }
        }

        swap(activeQ, expiredQ);
        for (int i = MaxPrios - 1; i >= 0; i--) {
            if (activeQ->at(i).empty()) {
                continue;
            } else {
                p = activeQ->at(i).front();
                activeQ->at(i).pop();
                return p;
            }
        }

        return nullptr;
    }

    void add_process(Process *p) {
        activeQ->at(p->dynamic_priority).push(p);
    }

    void add_process(Process *p, bool preempt_add) {
        if (p->dynamic_priority <= 0) {
            p->dynamic_priority = p->PRIO_StaticPrio - 1;
            expiredQ->at(p->dynamic_priority).push(p);
        } else {
            p->dynamic_priority--;
            activeQ->at(p->dynamic_priority).push(p);
        }
    }

    int getQuantum() {
        return TimeQuantum;
    }

    string getName() {
        return name;
    }


};





