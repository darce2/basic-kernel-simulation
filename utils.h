/*******************************************************************************
/
/      filename:  utils.h
/
/   description:  all functions for os control which move jobs around
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
#include "globals.h"

/*function to get time of internal event
will return time of next C,E or T if possible*/
int get_next_internal_event(){
   int eventIOC = 0;
   int eventET = 0;
   PCB q;
   if (!IO_Queue.empty())
   {/*get next io completion time*/
      tempIO = IO_Queue.top();
      eventIOC = tempIO.I_O_time_finished;
   }
   if (!CPU.empty())
   {/*get termination time or premept time*/
   q = CPU.front();
      if (q.burst_time_remaining <= (q.quantum - RUN_TIME))
      {
         eventET = (q.burst_time_remaining + SYSYTEM_TIME);
      }
      else if (q.burst_time_remaining > (q.quantum - RUN_TIME))
      {
         eventET = (q.quantum - RUN_TIME + SYSYTEM_TIME);
      }
   }
   /*if no internal events detected */
   if ((eventIOC == 0) && (eventET == 0)) return external_event_time + 1;

   if (eventIOC > 0 && eventET == 0) return eventIOC;

   if (eventIOC == 0 && eventET > 0) return eventET;
   /*returns which event happens first*/
   if (eventIOC > eventET) return eventET;
   return eventIOC;
}/*end get next internal event*/

/*function to process arrivals
reads in the rest of the input and pushes job onto 
job scheduling queue*/
bool processArrivals(PCB* j){
   int job_n, mem, run_time;
   cin >> job_n >> mem >> run_time;
   cout << "Event:	A" << "   Time: " << SYSYTEM_TIME << endl;
   if (mem > TOTAL_RAM)
   {
      cout << "This job exceeds the system's main memory capacity.\n";
      return true;
   }
   else{
      j->job_number = job_n;
      j->memory = mem;
      j->burst_time_remaining = run_time;
      j->untouched_runtime = run_time;
	  j->quantum = TIME_QUANTUM;

      /*push onto job_schedule_Queue*/
      job_scheduling_q.push(*j);
   }
   return false;
};/*end processArrivals*/

/*move jobs from job scheduling queue
to readyqueue 1 if there is enought memory
if there is enough memory move subtrack from RAM_USAGE
and then push into ready queue 1*/
void update_jq_rQ1(){
   if (job_scheduling_q.empty()) return;

   tempJSQ = job_scheduling_q.front();
   if (FREE_RAM >= tempJSQ.memory){
      FREE_RAM = FREE_RAM - tempJSQ.memory;
      tempJSQ.wait_time = SYSYTEM_TIME + 1 - tempJSQ.job_arrival_time;
      ready_q1.push(tempJSQ);
      job_scheduling_q.pop();
   }
   return;
}/*end updateQ function*/

/*move jobs from readyqueue 1 to the CPU if there
is nothing running on the CPU and readyqueue 1 is not empty
If readyqueue 1 is empty or CPU is working return;*/
void runJob_rq1(){
   if (ready_q1.empty() || !CPU.empty()) return;
   else{ /*if nothing on CPU and jobs to run*/
      tempRQ1 = ready_q1.front();
      tempRQ1.cpu_start_time = SYSYTEM_TIME;
      CPU.push(tempRQ1);
      ready_q1.pop();
      RUN_TIME = 0;
   }
   return;
}//end runJob_rq1

/*run job from ready queue 2 only if no jobs in rq1*/
void runJob_rq2(){
   if (ready_q2.empty()) return;
   if (ready_q1.empty() && CPU.empty()){/*no jobs on rq1 or CPU so push onto cpu*/
      tempRQ2 = ready_q2.front();
      CPU.push(tempRQ2);
      ready_q2.pop();
      RUN_TIME = 0;
   }
   return;
}/*end runJob_rq2*/

/*Cpu processing function which control burst time subtraction
and checks for termination and preemtions*/
void core2quad(){

   if (CPU.empty()) return;
   tempCPU = CPU.front();
   /*must prempt job with rq1 job*/
   if (tempCPU.quantum == 300 && !ready_q1.empty()){
      ready_q2.push(tempCPU);
      CPU.pop();
      runJob_rq1();/*move job from rq1 to CPU*/
      core2quad();//changed
   return;

   }
   if (tempCPU.burst_time_remaining > 0){/*update job on CPU*/
      tempCPU.burst_time_remaining--;
      if (tempCPU.burst_time_remaining == 0)
      {
         eventT();
         return;
      }
      RUN_TIME++;/*update how long process has been on cpu*/
      CPU.pop();
      CPU.push(tempCPU);
   }
	
   if (RUN_TIME == tempCPU.quantum){/*job exceded time quant and is preceded to level 2*/
      eventE();
      return;
   }
   return;
}

/*Process semaphore wait. subtracts semaphore value and pushes onto
queue if not avialable */
void semW(){
   int semNumber;
   cin >> semNumber;
   cout << "Event:	W" << "   Time: " << SYSYTEM_TIME << endl;
   SEMVALUES[semNumber]--;
   //check to make sure semaphore is available
   if (SEMVALUES[semNumber] >= 0) return;

   else/*semaphore not available*/
   {
      tempSEM = CPU.front();
      if (semNumber == 0) SEM0.push(tempSEM);
      if (semNumber == 1) SEM1.push(tempSEM);
      if (semNumber == 2) SEM2.push(tempSEM);
      if (semNumber == 3) SEM3.push(tempSEM);
      if (semNumber == 4) SEM4.push(tempSEM);
      CPU.pop();
      return;
   }	
}

/*Signal a semaphore signal. Releases a semaphore if on wait queue
and adds one to the semaphore value*/
void semS(){
   int semNumber;
   cin >> semNumber;
   SEMVALUES[semNumber]++;
   cout << "Event:	S" << "   Time: " << SYSYTEM_TIME << endl;
   if (semNumber == 0 && !SEM0.empty())
   {
      tempSEM = SEM0.front();
      ready_q1.push(tempSEM);
      SEM0.pop();
   }
   if (semNumber == 1 && !SEM1.empty())
   {
      tempSEM = SEM1.front();
      ready_q1.push(tempSEM);
      SEM1.pop();
   }
   if (semNumber == 2 && !SEM2.empty())
   {
      tempSEM = SEM2.front();
      ready_q1.push(tempSEM);
      SEM2.pop();
   }
   if (semNumber == 3 && !SEM3.empty())
   {
      tempSEM = SEM3.front();
      ready_q1.push(tempSEM);
      SEM3.pop();
   }
   if (semNumber == 4 && !SEM4.empty())
   {
      tempSEM = SEM4.front();
      ready_q1.push(tempSEM);
      SEM4.pop();
   }
   return;
}

/*IO function. When job gets io signal it moves off cpu and onto 
IO queue and waits to time to move back to ready queue*/
void processIO(){
   cout << "Event:	I" << "   Time: " << SYSYTEM_TIME << endl;
   int io_time;
   cin >> io_time;
   /*set io burst time and when the job will get off the io waitlist*/
   tempIO = CPU.front();
   tempIO.I_O_time_remaining = io_time;
   tempIO.I_O_start_time = SYSYTEM_TIME;
   tempIO.I_O_time_finished = SYSYTEM_TIME + io_time;
   /*push onto priority queue sorted by first job to finish io*/
   IO_Queue.push(tempIO);
   CPU.pop();
   return;
}

void eventC(){

   if (IO_Queue.empty())
   {
      //cout << "returning from eventC()\n";
      return;
   }

   PCB t = IO_Queue.top();
   if (t.I_O_time_finished == SYSYTEM_TIME)
   {
      cout << "Event:	C" << "   Time: " << SYSYTEM_TIME << endl;
      t.quantum = 100;
      ready_q1.push(t);
      IO_Queue.pop();
      //runJob_rq2();
      runJob_rq1();
   }
   return;
}

void eventE(){

   cout << "Event:	E" << "   Time: " << SYSYTEM_TIME+1 << endl;
   tempCPU = CPU.front();
   tempCPU.quantum = 300;
   ready_q2.push(tempCPU);
   CPU.pop();
   runJob_rq1();//changed
   runJob_rq2();
   CONTEXT_SWITCH = false;
   EVENT_TYPE = true;
   return;
}

void eventT(){// job just finished on CPU
   cout << "Event:	T" << "   Time: " << SYSYTEM_TIME + 1 << endl;
   tempCPU = CPU.front();
   FREE_RAM = FREE_RAM + tempCPU.memory;
   tempCPU.time_finished = SYSYTEM_TIME + 1;
   tempCPU.turn_around_time = tempCPU.time_finished - tempCPU.job_arrival_time;
   finished_list.push(tempCPU);
   CPU.pop();
   JOBS_IN_SYSTEM--;
   return;
}//end eventT 

/*Function to Display all QUEUEs */
void systemStatus(int t){

   cout << "Event:	D" << "   Time: " << t << endl << endl;
   cout << "************************************************************" << endl << endl;
   cout << "The status of the simulator at time " << t << "." << endl << endl;
   cout << "The contents of the JOB SCHEDULING QUEUE" << endl;
   cout << "________________________________________" << endl << endl;

   /*create copy of job scheduler queue and output*/
   queue<PCB> copy_job_scheduling_q = job_scheduling_q;
   if (copy_job_scheduling_q.size() == 0){
      cout << "The Job Scheduling Queue is empty." << endl << endl;
   }
   else{
      cout << "Job #  Arr. Time  Mem. Req.  Run Time" << endl;
      cout << "_____  _________  _________  ________" << endl << endl;

      while (copy_job_scheduling_q.size() != 0){
         PCB temp = copy_job_scheduling_q.front();
         cout << right << setw(5) << temp.job_number
            << right << setw(11) << temp.job_arrival_time
            << right << setw(11) << temp.memory
            << right << setw(10) << temp.untouched_runtime << endl;
            copy_job_scheduling_q.pop();
      }
   }
   cout << endl;

   cout << "The contents of the FIRST LEVEL READY QUEUE" << endl;
   cout << "___________________________________________" << endl << endl;

   /*create copy of first level ready queue and output*/
   queue<PCB> copy_ready_q1 = ready_q1;
   if (copy_ready_q1.size() == 0){
      cout << "The First Level Ready Queue is empty." << endl << endl;
   }
   else{
      cout << "Job #  Arr. Time  Mem. Req.  Run Time" << endl;
      cout << "_____  _________  _________  ________" << endl << endl;

      while (copy_ready_q1.size() != 0){
         PCB temp = copy_ready_q1.front();
         cout << right << setw(5) << temp.job_number
         << right << setw(11) << temp.job_arrival_time
         << right << setw(11) << temp.memory
         << right << setw(10) << temp.untouched_runtime << endl;
         copy_ready_q1.pop();
      }
   }
   cout << endl;
   cout << "The contents of the SECOND LEVEL READY QUEUE" << endl;
   cout << "____________________________________________" << endl << endl;

   /*create copy of second level ready queue and output */
   queue<PCB> copy_ready_q2 = ready_q2;
   if (copy_ready_q2.size() == 0){
      cout << "The Second Level Ready Queue is empty." << endl << endl;
   }
   else{
      cout << "Job #  Arr. Time  Mem. Req.  Run Time" << endl;
      cout << "_____  _________  _________  ________" << endl << endl;

      while (copy_ready_q2.size() != 0){
         PCB temp = copy_ready_q2.front();
         cout << right << setw(5) << temp.job_number
            << right << setw(11) << temp.job_arrival_time
            << right << setw(11) << temp.memory
            << right << setw(10) << temp.untouched_runtime << endl;
            copy_ready_q2.pop();
      }
   }
   cout << endl;
   cout << "The contents of the I/O WAIT QUEUE" << endl;
   cout << "__________________________________" << endl << endl;

   /*copy IO queue and output*/
   priority_queue<PCB, vector<PCB>, CompareIO_Time> IO_Queue_copy = IO_Queue;
   if (IO_Queue_copy.size() == 0) cout << "The I/O Wait Queue is empty.\n\n";
   else{
      cout << "Job #  Arr. Time  Mem. Req.  Run Time  IO Start Time  IO Burst  Comp. Time" << endl;
      cout << "_____  _________  _________  ________  _____________  ________  __________" << endl << endl;

      while (IO_Queue_copy.size() != 0){
         PCB temp = IO_Queue_copy.top();
         cout << right << setw(5) << temp.job_number
            << right << setw(11) << temp.job_arrival_time
            << right << setw(11) << temp.memory
            << right << setw(10) << temp.untouched_runtime
            << right << setw(15) << temp.I_O_start_time
            << right << setw(10) << temp.I_O_time_remaining
            << right << setw(11) << temp.I_O_time_finished << endl;
            IO_Queue_copy.pop();
      }
   }

   cout << endl;

   /*create copies of semaphore queues and output*/
   queue<PCB> copy_sem0 = SEM0;
   queue<PCB> copy_sem1 = SEM1;
   queue<PCB> copy_sem2 = SEM2;
   queue<PCB> copy_sem3 = SEM3;
   queue<PCB> copy_sem4 = SEM4;

   cout << "The contents of SEMAPHORE ZERO" << endl;
   cout << "______________________________" << endl << endl;
   cout << "The value of semaphore 0 is " << SEMVALUES[0] << "." << endl << endl;
   if (copy_sem0.size() == 0)cout << "The wait queue for semaphore 0 is empty." << endl;
   else{
      while (copy_sem0.size() != 0){
         PCB temp = copy_sem0.front();
         cout << temp.job_number << endl;
         copy_sem0.pop();
      }
   }
   cout << endl << endl;
	
   cout << "The contents of SEMAPHORE ONE" << endl;
   cout << "_____________________________" << endl << endl;
   cout << "The value of semaphore 1 is " << SEMVALUES[1] << "." << endl << endl;
   if (copy_sem1.size() == 0)cout << "The wait queue for semaphore 1 is empty." << endl;
   else{
   while (copy_sem1.size() != 0){
      PCB temp = copy_sem1.front();
      cout << temp.job_number << endl;
      copy_sem1.pop();
   }
   }
   cout << endl << endl;

   cout << "The contents of SEMAPHORE Two" << endl;
   cout << "_____________________________" << endl << endl;
   cout << "The value of semaphore 2 is " << SEMVALUES[2] << "." << endl << endl;
   if (copy_sem2.size() == 0)cout << "The wait queue for semaphore 2 is empty." << endl;
   else{
      while (copy_sem2.size() != 0){
         PCB temp = copy_sem2.front();
         cout << temp.job_number << endl;
         copy_sem2.pop();
      }
   }
	
   cout << endl << endl;
	
   cout << "The contents of SEMAPHORE THREE" << endl;
   cout << "_______________________________" << endl << endl;
   cout << "The value of semaphore 3 is " << SEMVALUES[3] << "." << endl << endl;
   if (copy_sem3.size() == 0)cout << "The wait queue for semaphore 3 is empty." << endl;
   else{
      while (copy_sem3.size() != 0){
         PCB temp = copy_sem3.front();
         cout << temp.job_number << endl;
         copy_sem3.pop();
      }
   }

   cout << endl << endl;

   cout << "The contents of SEMAPHORE Four" << endl;
   cout << "______________________________" << endl << endl;

   cout << "The value of semaphore 4 is " << SEMVALUES[4] << "." << endl << endl;
   if (copy_sem4.size() == 0)cout << "The wait queue for semaphore 4 is empty." << endl;
   else{
      while (copy_sem4.size() != 0){
         PCB temp = copy_sem4.front();
         cout << temp.job_number << endl;
         copy_sem4.pop();
      }
   }

   cout << endl << endl;

   cout << "The CPU  Start Time  CPU burst time left" << endl;
   cout << "_______  __________  ___________________" << endl << endl;

   /*create copy of CPU queue and print info*/
   queue<PCB> copy_CPU = CPU;
   if (copy_CPU.size() == 0){
      cout << "The CPU is idle." << endl << endl << endl;
   }
   else{
      while (copy_CPU.size() != 0){
		  PCB temp = copy_CPU.front();
		  cout << right << setw(7) << temp.job_number
             << right << setw(12) << temp.cpu_start_time
             << right << setw(21) << temp.burst_time_remaining << endl << endl;
          copy_CPU.pop();
       }
   }
	
   cout << "The contents of the FINISHED LIST" << endl;
   cout << "_________________________________" << endl << endl;

   /*create copy of finished queue and output */
   queue<PCB> copy_finished_list = finished_list;
   if (copy_finished_list.size() == 0){
      cout << "The Finished Queue is empty." << endl << endl;
   }
   else{
      cout << "Job #  Arr. Time  Mem. Req.  Run Time  Start Time  Com. Time" << endl;
      cout << "_____  _________  _________  ________  __________  _________" << endl << endl;

      while (copy_finished_list.size() != 0){
         PCB temp = copy_finished_list.front();
         cout << right << setw(5) << temp.job_number
            << right << setw(11) << temp.job_arrival_time
            << right << setw(11) << temp.memory
            << right << setw(10) << temp.untouched_runtime
            << right << setw(12) << temp.cpu_start_time
            << right << setw(11) << temp.time_finished << endl;
            copy_finished_list.pop();
      }
   }
   cout << endl << endl;
   cout << "There are " << FREE_RAM << " blocks of main memory available in the system" << endl << endl;

   return;
}
/*Print out finished list after all jobs through system*/
void printFinalFinishedList()
{
   float average_waiting_time, average_turn_around_time;
   float total_waiting_time = 0.000, total_turn_around_time = 0.000, total_jobs = 0.000;

   cout << "\nThe contents of the FINAL FINISHED LIST" << endl;
   cout << "_________________________________" << endl << endl;

   queue<PCB> copy_finished_list = finished_list;
   if (copy_finished_list.size() == 0){
      cout << "The Finished Queue is empty." << endl << endl;
   }
   else{
      cout << "Job #  Arr. Time  Mem. Req.  Run Time  Start Time  Com. Time" << endl;
      cout << "_____  _________  _________  ________  __________  _________" << endl << endl;

      while (copy_finished_list.size() != 0){
         PCB temp = copy_finished_list.front();
         cout << right << setw(5) << temp.job_number
            << right << setw(11) << temp.job_arrival_time
            << right << setw(11) << temp.memory
            << right << setw(10) << temp.untouched_runtime
            << right << setw(12) << temp.cpu_start_time
            << right << setw(11) << temp.time_finished << endl;
            /*calculate average wait time and average turnaround time*/ 
         total_jobs++;
         total_waiting_time += (float) temp.wait_time;
         total_turn_around_time += (float) temp.turn_around_time;
         copy_finished_list.pop();
      }
   }

   average_waiting_time = total_waiting_time / total_jobs;
   average_turn_around_time = total_turn_around_time / total_jobs;

   cout << "\n\nThe Average Turnaround Time for the simulation was "
      << average_turn_around_time << " units.";

   cout << "\n\nThe Average Job Scheduling Wait Time for the simulation was "
      << average_waiting_time << " units.";

   cout << "\n\nThere are " << FREE_RAM << " blocks of main memory available in the system.\n\n";

   return;
}