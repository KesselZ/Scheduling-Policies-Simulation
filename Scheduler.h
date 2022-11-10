#include <string>
#include <queue>
#include <deque>
#ifndef INC_2250_ASSN2_SCHEDULER_H
#define INC_2250_ASSN2_SCHEDULER_H
using namespace std;
#include "Process.h"

class Scheduler {
protected:
    deque<Process *> readyQ;
public:
    int TimeQuantum = 0;
    int MaxPrios = 4;
    string name= "Scheduler(Parent)";

    Scheduler();
    Scheduler(int num) {};


    virtual Process *get_next_process(){ return nullptr; };

    virtual void add_process(Process *p){};

    virtual void add_process(Process *p,bool preempt_add){};

    virtual void print_readyQ(){
        printf("readyQ:%d\n",readyQ.front()->AT_ArrivalTime);
    };

    virtual void setName(string new_name){
    };

    virtual int getQuantum(){
        return TimeQuantum;
    }

    virtual string getName(){
        return name;
    }

};

#endif //INC_2250_ASSN2_SCHEDULER_H
