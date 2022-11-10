#include "Scheduler.h"
#include "Process.h"
#include <string>
#include<sstream>
#include <queue>
#include <cstring>
#include<sstream>

using namespace std;

class SRTF : public Scheduler {

public:
    int TimeQuantum = 100000;
    int MaxPrios = 4;
    string name = "SRTF";

    SRTF() : Scheduler() {
    }



    Process *get_next_process() {
        if (readyQ.empty()) return nullptr;

        int min_rem = 100000;
        Process *p;
        int short_location = 0;

        for (int i = 0; i < readyQ.size(); i++) {
            if (readyQ.at(i)->rem < min_rem) {
                short_location = i;
                min_rem = readyQ.at(i)->rem;
            }
        }
        p = readyQ.at(short_location);
        readyQ.erase(readyQ.begin() + short_location);
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