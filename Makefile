main: main.o Event.o Process.o Scheduler.o FCFS.o LCFS.o SRTF.o RR.o PRIO.o
	g++ -std=c++11 main.o Event.o Process.o Scheduler.o FCFS.o LCFS.o SRTF.o RR.o PRIO.o -o scheduler

main.o: main.cpp Process.h Scheduler.h Event.cpp FCFS.cpp LCFS.cpp SRTF.cpp RR.cpp PRIO.cpp
	g++ -std=c++11 -c main.cpp

Event.o: Event.cpp Process.h
	g++ -std=c++11 -c Event.cpp

Process.o: Process.cpp Scheduler.h Process.h
	g++ -std=c++11 -c Process.cpp

Scheduler.o: Process.h Process.cpp Scheduler.cpp Scheduler.h 
	g++ -std=c++11 -c Scheduler.cpp 

FCFS.o: FCFS.cpp Scheduler.h Process.h
	g++ -std=c++11 -c FCFS.cpp

LCFS.o: LCFS.cpp Scheduler.h Process.h
	g++ -std=c++11 -c LCFS.cpp

SRTF.o: SRTF.cpp Scheduler.h Process.h
	g++ -std=c++11 -c SRTF.cpp

RR.o: RR.cpp Scheduler.h Process.h
	g++ -std=c++11 -c RR.cpp

PRIO.o: PRIO.cpp Scheduler.h Process.h
	g++ -std=c++11 -c PRIO.cpp

clean:
	rm -f scheduler *.o