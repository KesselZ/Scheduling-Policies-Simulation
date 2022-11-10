#include <string>
#include "Process.h"
#include <regex>

using namespace std;

extern vector<int> randvals;
extern int ofs;

extern int myrandom(int burst);

Process::Process(int at, int tc, int cb, int io, int maxpri, int id) {
    AT_ArrivalTime = at;
    TC_TotalCpuTime = tc;
    CB_CpuBurst = cb;
    IO_IoBurst = io;
    pid = id;

    IT_IoTime=0;
    CW_CpuWaitingTime=0;
    cpu_burst = 0;
    io_burst = 0;
    rem = TC_TotalCpuTime;

    PRIO_StaticPrio = myrandom(maxpri);
    dynamic_priority = PRIO_StaticPrio - 1;
}

