#include "Process.h"
#include <iostream>
#include <fstream>
#include <string>
#include<sstream>
#include <queue>
#include <cstring>
#include<sstream>
#include <regex>

using namespace std;

extern vector<int> randvals;

typedef enum {
    TRANS_TO_READY, TRANS_TO_RUN, TRANS_TO_BLOCK, TRANS_TO_PREEMPT, OVER
} trasition;

typedef enum {
    STATE_CREATED, STATE_READY, STATE_RUNNING, STATE_BLOCKED,STATE_RUNNING_PRE
} process_state_t;


class Event {
public:
    int TimeStamp;
    Process *process;
    int OldState;
    int transition;
    int CreateTime;

    Event(){};

    Event(int timestamp, int tr, int old_st, int create_time, Process *p) {
        TimeStamp = timestamp;
        CreateTime=create_time;
        process = p;
        transition = tr;
        OldState = old_st;
    }

    string getStateName(int state){
        switch (state) {
            case 0:
                return "CREATED";
            case 1:
                return "READY";
            case 2:
                return "RUNNG";
            case 3:
                return "BLOCK";
        }

    }

};

class DES_Layer {
    vector<Event> eventQ;

public:
    Event get_event() {
        if (eventQ.empty()) {
            Event event(-404, -404, -404,-404, nullptr);
            return event;
        }
        Event event = eventQ.front();
        eventQ.erase(eventQ.begin());
        return event;
    }

    Event peek_event() {
        if (eventQ.empty()) {
            Event event(-404, -404, -404, -404, nullptr);
            return event;
        }
        return eventQ.front();
    }

    void put_event(Event evt) {
        int offset = 0;
        while (offset < eventQ.size() && evt.TimeStamp >= eventQ[offset].TimeStamp) {
            offset = offset + 1;
        }
        eventQ.insert(eventQ.begin() + offset, evt);
    }

    int get_next_event_time() {
        if (eventQ.empty()) return -1;
        return eventQ.front().TimeStamp;
    }

    void remove_event(int pid_target){
        int size=eventQ.size();
        int it=0;
        while(it < size) {
            if(eventQ[it].process->pid==pid_target)  {
                eventQ.erase(eventQ.begin()+it);
                it--;
                size--;
            }
            it++;
        }
    }

};

