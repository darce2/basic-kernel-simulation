/*******************************************************************************
/
/      filename:  globals.h
/
/   description:  all globals declarations and includes for OS sim and 
/                 declarations for job PCB
/
/        author:  D'Arcy, Arlen
/      login id:  FA_15_CPS356_21
/
/         class:  CPS 356
/    instructor:  Perugini
/    assignment:  Project #2
/
/      assigned:  September 29, 2015
/           due:  October 29, 2015
/
******************************************************************************/

#include <iostream>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>
#include <queue>
#include <iomanip>

using namespace std;
/*PCB job structur declaration*/
typedef struct PCB{

   char job_type;
   int job_arrival_time;
   int job_number;
   int memory;
   int burst_time_remaining;
   int untouched_runtime;
   int quantum;
   int cpu_start_time;
   int time_finished;
   char location; //where the process is in the system J(job scheduling), R(ready, r, I, C
   int I_O_start_time;
   int I_O_time_finished;
   int I_O_time_remaining;
   int wait_time; //time spent waiting
   int turn_around_time; //time it took from creation to finish
   bool event_type;// true = external; false = internal

}PCB;

/*Class to make priority queue sorted with lowest time first
-first job to come off IO wait Queue*/
class CompareIO_Time {
public:
   bool operator()(PCB& t1, PCB& t2)
   {
      if (t1.I_O_time_finished > t2.I_O_time_finished) return true;
      return false;
   }
};

/*Global Variables*/
int SYSYTEM_TIME = 0;
int JOBS_IN_SYSTEM = 0;
int FREE_RAM = 512;
int TOTAL_RAM = 512;
int TIME_QUANTUM = 100;
int RUN_TIME = 0;
bool ARRIVAL_TIME_NOT_SYS_TIME = false;
int ARRIVAL_TIME;
bool DONE = false;

char external_event_letter;
char intern_event;
int intern_event_time;
int external_event_time = 0;
int next_internal_event_time = 0;
bool read_next = true;
int total = 0;
int old = -1;
char EVENT_HOLDER;
bool CONTEXT_SWITCH = false;
bool EVENT_TYPE = true;   //True = external, false = internal 

/*Queues*/
priority_queue<PCB, vector<PCB>, CompareIO_Time> IO_Queue;
queue<PCB> bogus_jobq;
queue<PCB> ready_q1;
queue<PCB> ready_q2;
queue<PCB> job_scheduling_q;
queue<PCB> finished_list;
queue<PCB> CPU;
queue<PCB> SEM0;
queue<PCB> SEM1;
queue<PCB> SEM2;
queue<PCB> SEM3;
queue<PCB> SEM4;
/*holds semaphore values*/
int SEMVALUES[5] = { 1, 1, 1, 1, 1 };
/*PCB pointers*/
PCB tempJSQ;
PCB tempRQ1;
PCB tempRQ2;
PCB tempCPU;
PCB lastJOB;
PCB tempIO;
PCB tempSEM;
/*Functions prototypes*/
void processExterns(void);
bool processArrivals(PCB *j);
void update_jq_rQ1(void);
void runJob_rq1(void);
void runJob_rq2(void);//update ready q2
void core2quad(void);//update CPU node
void eventT(void);
void eventE(void);
void setNextEvent(char c);
void systemStatus(int t);
int get_next_internal_event(void);
void printFinalFinishedList(void);
void processIO(void);
void eventC(void);
void semW(void);
void semS(void);

/*location letter decoder (depricated)
V (vector)
S (secondary memory)
J (job scheduling queue)
R(ready queue level 1)
r (ready queue level 2)
I (I/O wait queue)
C (CPU)
V (semaphore wait queue 0)   //should each semaphore be initilzied to 1?
B (semaphore wait queue 1)
N (semaphore wait queue 2)
M (semaphore wait queue 3)
k (semaphore wait queue 4)
F (finished list)*/