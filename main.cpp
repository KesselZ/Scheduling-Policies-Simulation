#include <string>
#include "Process.h"
#include <queue>
#include <cstring>
#include<sstream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

#include "Event.cpp"
#include "FCFS.cpp"
#include "LCFS.cpp"
#include "SRTF.cpp"
#include "RR.cpp"
#include "PRIO.cpp"

vector<int> randvals = {};
vector<Process> process_list;
bool v = false;
char s_scheduler = 'n';   //n means not initialized
Scheduler *scheduler;
int s_quantum = -404;
int s_maxprio = -404;
bool PREEMPT = false;

int io_finish_time;
int termination_time;

Process *process_in_cpu = nullptr;
Process *process_in_io = nullptr;

int time_cpubusy;
int time_iobusy;
int time_turnaround;
int time_cpu_waiting=0;

int ofs = 0;

void read_rfile(char *name) {
    char buffer[256];
    getcwd(buffer, sizeof(buffer));
    strcat(buffer,"/");
    strcat(buffer,name);
    ifstream f(buffer); //taking file as inputstream
    string str;
    std::getline(f, str);
    while (std::getline(f, str)) {
        randvals.push_back(stoi(str));
    }
}

void read_input(char *filename) {
    char buffer[256];
    getcwd(buffer, sizeof(buffer));
    strcat(buffer,"/");
    strcat(buffer,filename);

    string token;
    ifstream file;
    file.open(buffer);
    int id = 0;

    while (1) {
        if (!(file >> token)) break;
        int at = stoi(token);
        file >> token;
        int tc = stoi(token);
        file >> token;
        int cb = stoi(token);
        file >> token;
        int io = stoi(token);

        if (s_maxprio != -404) {
            Process p(at, tc, cb, io, s_maxprio, id++);
            process_list.push_back(p);
        } else {
            Process p(at, tc, cb, io, 4, id++);
            process_list.push_back(p);
        }

    }

}

int myrandom(int burst) {
    int randnum = 1 + (randvals[ofs] % burst);
    ofs++;
    return randnum;
}

void choose_scheduler(char s) {
    switch (s) {
        case 'F':
            scheduler = new FCFS();
            break;
        case 'L':
            scheduler = new LCFS();
            break;
        case 'S':
            scheduler = new SRTF();
            break;
        case 'R':
            scheduler = new RR(s_quantum);
            break;
        case 'P':
            if (s_maxprio == -404) scheduler = new PRIO(s_quantum);
            else scheduler = new PRIO(s_quantum, s_maxprio);
            break;
        case 'E':
            if (s_maxprio == -404) scheduler = new PRIO(s_quantum);
            else scheduler = new PRIO(s_quantum, s_maxprio);
            scheduler->setName("PREPRIO");
            break;
    }
}

void read_arg(int argc, char **argv) {
    int c;
    char *cvalue = NULL;
    while ((c = getopt(argc, argv, "vs:")) != -1)
        switch (c) {
            case 'v':
                v = true;
                break;
            case 's': {
                cvalue = optarg;
                s_scheduler = cvalue[0];
                string value = "";
                for (int i = 1; i < strlen(cvalue); i++) {
                    value += cvalue[i];
                }

                value.find(":");
                if (value.length() == 0) {
                    //do nothing
                } else if (string::npos == value.find(":")) {
                    string token1 = value.substr(0, value.find(":"));
                    s_quantum = stoi(token1);
                } else {
                    string token1 = value.substr(0, value.find(":"));
                    string token2 = value.substr(value.find(":") + 1, value.length() - 1);
                    s_quantum = stoi(token1);
                    s_maxprio = stoi(token2);
                }
                choose_scheduler(s_scheduler);
                break;
            }
            case '?':
                if (optopt == 'c')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                return;
            default:
                abort();
        }
}


void Simulation(DES_Layer DES) {
    Event evt;
    bool CALL_SCHEDULER = false;
    while (DES.peek_event().CreateTime != -404) {
        evt = DES.get_event();
        Process *proc = evt.process; // this is the process the event works on
        int CURRENT_TIME = evt.TimeStamp;
        int transition = evt.transition;

        if (v) printf("%d %d ", CURRENT_TIME, proc->pid);
        switch (transition) {
            case TRANS_TO_READY: {
                if (v)
                    printf("%d: %s -> READY\n", CURRENT_TIME - proc->last_state_time,
                           evt.getStateName(evt.OldState).c_str());

                if (proc == process_in_io) process_in_io = nullptr;

                proc->ready_timestamp = CURRENT_TIME;
                proc->state_timestamp = CURRENT_TIME;

                if (s_scheduler == 'E' && process_in_cpu != nullptr) {
                    bool preempt = process_in_cpu->dynamic_priority < proc->dynamic_priority;
                    int small = min(s_quantum, process_in_cpu->last_cb);

                    int expectedTS;
                    if (PREEMPT) expectedTS = CURRENT_TIME;
                    else expectedTS = process_in_cpu->state_timestamp + small;


                    if (CURRENT_TIME >= expectedTS)
                        preempt = false;

                    if (v) {
                        printf("---> PRIO preemption %d by %d ? %d TS=%d now=%d) --> %s\n",
                               process_in_cpu->pid,
                               proc->pid,
                               process_in_cpu->dynamic_priority < proc->dynamic_priority ? 1 : 0,
                               expectedTS,
                               CURRENT_TIME,
                               preempt ? "YES" : "NO");
                    }

                    if (preempt) {
                        PREEMPT = true;

                        process_in_cpu->cpu_burst =
                                process_in_cpu->last_cb - (CURRENT_TIME - process_in_cpu->last_state_time);
                        process_in_cpu->rem =
                                process_in_cpu->last_rem - (CURRENT_TIME - process_in_cpu->last_state_time);

                        Event new_evt(CURRENT_TIME, TRANS_TO_PREEMPT, STATE_RUNNING_PRE, CURRENT_TIME, process_in_cpu);
                        DES.remove_event(process_in_cpu->pid);
                        DES.put_event(new_evt);
                    }


                }
                scheduler->add_process(proc);
                CALL_SCHEDULER = true;
                break;
            }

            case TRANS_TO_PREEMPT: {
                if (v)
                    printf("%d: %s -> READY cb=%d rem=%d prio=%d\n", (CURRENT_TIME - proc->last_state_time),
                           evt.getStateName(evt.OldState).c_str(), proc->cpu_burst, proc->rem, proc->dynamic_priority);

                if (evt.OldState == STATE_RUNNING_PRE) PREEMPT = false;
                process_in_cpu = nullptr;

                if (s_scheduler == 'P' || s_scheduler == 'E') scheduler->add_process(proc, true);
                else scheduler->add_process(proc);

                proc->ready_timestamp = CURRENT_TIME;
                proc->state_timestamp = CURRENT_TIME;
                CALL_SCHEDULER = true;
                break;
            }

            case TRANS_TO_RUN: {
                process_in_cpu = proc;

                if (proc->cpu_burst == 0) {
                    proc->cpu_burst = myrandom(proc->CB_CpuBurst);
                }

                if (proc->cpu_burst > proc->rem) {
                    proc->cpu_burst = proc->rem;
                }

                proc->last_cb = proc->cpu_burst;
                proc->last_rem = proc->rem;
                proc->last_state_time = evt.CreateTime;
                proc->state_timestamp = CURRENT_TIME;

                if (v)
                    printf("%d: %s -> RUNNG cb=%d rem=%d prio=%d\n", CURRENT_TIME - proc->ready_timestamp,
                           evt.getStateName(evt.OldState).c_str(), proc->cpu_burst, proc->rem, proc->dynamic_priority);

                proc->CW_CpuWaitingTime += CURRENT_TIME - proc->ready_timestamp;

                //could not finish this burst due to quantum/preempt:
                if (proc->cpu_burst > scheduler->getQuantum()) {
                    proc->rem -= scheduler->getQuantum();
                    proc->cpu_burst -= scheduler->getQuantum();

                    Event new_evt(CURRENT_TIME + scheduler->getQuantum(), TRANS_TO_PREEMPT, STATE_RUNNING, CURRENT_TIME,
                                  proc);
                    DES.put_event(new_evt);
                }

                    //process not finish after this burst
                else if (proc->cpu_burst < proc->rem) {
                    proc->rem -= proc->cpu_burst;
                    Event new_evt(CURRENT_TIME + proc->cpu_burst, TRANS_TO_BLOCK, STATE_RUNNING, CURRENT_TIME, proc);
                    DES.put_event(new_evt);
                    proc->cpu_burst = 0;
                }

                    //process finish after this burst
                else if (proc->cpu_burst >= proc->rem) {
                    proc->rem = 0;
                    Event new_evt(CURRENT_TIME + proc->cpu_burst, OVER, STATE_RUNNING, CURRENT_TIME, proc);
                    DES.put_event(new_evt);
                }

                break;
            }


            case TRANS_TO_BLOCK: {
                process_in_cpu = nullptr;
                proc->io_burst = myrandom(proc->IO_IoBurst);
                proc->IT_IoTime += proc->io_burst;
                proc->state_timestamp = CURRENT_TIME;

                if (v)
                    printf("%d: %s -> BLOCK ib=%d rem=%d\n", CURRENT_TIME - evt.CreateTime,
                           evt.getStateName(evt.OldState).c_str(), proc->io_burst, proc->rem);

                Event new_evt(CURRENT_TIME + proc->io_burst, TRANS_TO_READY, STATE_BLOCKED, CURRENT_TIME, proc);
                DES.put_event(new_evt);

                proc->dynamic_priority = proc->PRIO_StaticPrio - 1;

                if (process_in_io == nullptr) {
                    time_iobusy += proc->io_burst;
                    io_finish_time = CURRENT_TIME + proc->io_burst;
                    process_in_io = proc;
                } else {
                    if (CURRENT_TIME + proc->io_burst > io_finish_time) {
                        time_iobusy += proc->io_burst + CURRENT_TIME - io_finish_time;
                        io_finish_time = proc->io_burst + CURRENT_TIME;
                        process_in_io = proc;
                    }
                }

                CALL_SCHEDULER = true;
                break;
            }


            case OVER: {
                if (v)
                    printf("%d: %s -> OVER\n", (CURRENT_TIME - evt.CreateTime), evt.getStateName(evt.OldState).c_str());

                proc->FT_FinishTime = CURRENT_TIME;
                proc->TT_TurnaroundTime = proc->FT_FinishTime - proc->AT_ArrivalTime;
                process_in_cpu = nullptr;
                termination_time = CURRENT_TIME;
                CALL_SCHEDULER = true;
                break;
            }
        }

        if (CALL_SCHEDULER) {
            if (DES.get_next_event_time() == CURRENT_TIME) continue;
            CALL_SCHEDULER = false;
            if (process_in_cpu == nullptr) {
                process_in_cpu = scheduler->get_next_process();
                if (process_in_cpu == nullptr) continue;

                Event new_evt(CURRENT_TIME, TRANS_TO_RUN, STATE_READY, CURRENT_TIME, process_in_cpu);
                DES.put_event(new_evt);
                process_in_cpu = nullptr;
            }

        }

    }
}

int main(int argc, char **argv) {
//    char *data[] = {"./main", "-v", "-sF", "input5", "rfile",};
//    char **argv2 = data;
//    argc = 5;

    read_rfile(argv[argc - 1]);
    read_arg(argc, argv);
    read_input(argv[argc-2]);


    DES_Layer DES;
    for (auto &i: process_list) {
        Event event(i.AT_ArrivalTime, TRANS_TO_READY, STATE_CREATED, i.AT_ArrivalTime,
                    &i);
        DES.put_event(event);
    }

    Simulation(DES);

    printf("%s", scheduler->getName().c_str());
    if (s_quantum != -404) printf(" %d", s_quantum);
    printf("\n");

    for (auto &i: process_list) {
        time_cpubusy += i.TC_TotalCpuTime;
        time_turnaround += i.TT_TurnaroundTime;
        time_cpu_waiting += i.CW_CpuWaitingTime;

        printf("%04d: %4d %4d %4d %4d %4d | %5d %5d %5d %5d\n",
               i.pid,
               i.AT_ArrivalTime, //
               i.TC_TotalCpuTime,
               i.CB_CpuBurst,
               i.IO_IoBurst,
               i.PRIO_StaticPrio,
               i.FT_FinishTime,
               i.TT_TurnaroundTime,
               i.IT_IoTime,
               i.CW_CpuWaitingTime);
    }

    double cpu_util = 100.0 * (time_cpubusy / (double) termination_time);
    double io_util = 100.0 * (time_iobusy / (double) termination_time);
    double throughput = 100.0 * (process_list.size() / (double) termination_time);

    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
           termination_time,
           cpu_util,
           io_util,
           time_turnaround / (double) process_list.size(),
           time_cpu_waiting / (double) process_list.size(),
           throughput);
}