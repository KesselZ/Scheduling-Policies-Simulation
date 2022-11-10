//
// Created by Kessel on 10/27/2022.
//

#ifndef INC_2250_ASSN2_PROCESS_H
#define INC_2250_ASSN2_PROCESS_H


class Process{
public:
    int AT_ArrivalTime;
    int TC_TotalCpuTime;
    int CB_CpuBurst;
    int IO_IoBurst;

    int PRIO_StaticPrio;
    int FT_FinishTime;
    int TT_TurnaroundTime;
    int IT_IoTime;
    int CW_CpuWaitingTime;

    int ready_timestamp;
    int state_timestamp;

    int cpu_burst;
    int io_burst;
    int pid;

    int dynamic_priority;

    int rem;
    int last_rem;
    int last_cb;
    int last_state_time=0;
    Process(int ,int, int ,int ,int, int);

};


#endif //INC_2250_ASSN2_PROCESS_H
