#include "Test.h"
#include "Temp_Hum.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id
Mensaje_Temp_Hum datos;
extern osMessageQueueId_t mid_MsgTemp_Hum;

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
		if(osMessageQueueGetCount(mid_MsgTemp_Hum) != 0){
			osMessageQueueGet(mid_MsgTemp_Hum, &datos, 0, 0);
	
			printf("La temperatura actual es %.2f ºC y la humedad relativa es %.2f %%\n", datos.temperatura, datos.humedad);
			
		}
  }
}
