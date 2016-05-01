/*******************************************************************************
/
/      filename:  Main.cpp
/
/   description:  Main for project 2 controls system while loop and controls
/                 Job movement functions
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
#include "utils.h"

int main() {
   /*System loop that will start while there is something to read
   in and stop once all Jobs jabe been processed*/
   while (!cin.eof() || JOBS_IN_SYSTEM > 0){//(SYSYTEM_TIME < 5000){

      if (read_next) cin >> external_event_letter >> external_event_time;

      next_internal_event_time = get_next_internal_event();

      if (next_internal_event_time <= external_event_time)
      {/*process internals*/
         /*if system time is equal to internal and external event
		must process internal event first
		then readin and process external event
		without incrementing time*/
         if (next_internal_event_time == external_event_time && SYSYTEM_TIME == next_internal_event_time)
         {
            eventC();
            /*jump to external event*/
            goto externalevent;
         }

         read_next = false;
			
      }/*end process internal else*/
		
      else 
      {/*process external events */
         if (external_event_time != SYSYTEM_TIME)//dont start job
         {
            read_next = false;
            goto skipEvent;
         }
			
         externalevent:
         read_next = true;
         /*struct init for external jobs*/
         PCB jobE = { external_event_letter, external_event_time, 0, 0, 0, 0, 0, 0, 0, 'V', 0, 0, 0, 0, 0, true };
         switch (jobE.job_type)
         {
            case 'A': /*Arrival*/
               bool err;
               err = processArrivals(&jobE);
               if (!err){/*job fits mem and will run */
                  JOBS_IN_SYSTEM++;
				   /*total++;*/
               }
               break;

            case 'D':/*Display Event*/
				systemStatus(external_event_time);
				break;

            case 'I':/*IO event*/
               processIO(); 
               break;

            case 'W':/*semaphore wait*/
               semW();
               break;

            case 'S':/*semaphore signal*/
               semS();
               break;

            default:
         break;
         }/*end switch*/

      }/*end process external (else) */ 
      skipEvent:
      update_jq_rQ1();
      runJob_rq1();
      runJob_rq2();
      eventC();
      core2quad();
      SYSYTEM_TIME++;
   }/*end system while  */
   printFinalFinishedList();
   return 0;
}/*end main*/     

