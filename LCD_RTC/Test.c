#include "Test.h"
#include "rtc.h"
#include "lcd.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id
FechaHora datosFechaHora;
DatosLCD LCDDatos;
extern osMessageQueueId_t mid_MsgRTC;
extern osMessageQueueId_t mid_MsgLCD;
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
		LCDDatos.horas = datosFechaHora.horas;
		LCDDatos.minutos = datosFechaHora.minutos;
		LCDDatos.segundos = datosFechaHora.segundos;
		LCDDatos.anno = datosFechaHora.anno;
		LCDDatos.mes = datosFechaHora.mes;
		LCDDatos.dia = datosFechaHora.dia;
		osMessageQueuePut(mid_MsgLCD, &LCDDatos, 0, 0);
  }
}
