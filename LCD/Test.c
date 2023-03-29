#include "Test.h"
#include "rtc.h"
#include "lcd.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id
FechaHora datosFechaHora;

extern osMessageQueueId_t mid_MsgRTC;

void Thread (void *argument);                   // thread function

int Init_Thread (void) {
	
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
  return(0);
}

void Thread (void *argument) {
 
  while (1) {
		osMessageQueueGet(mid_MsgRTC, &datosFechaHora, 0, 0);
  }
}
