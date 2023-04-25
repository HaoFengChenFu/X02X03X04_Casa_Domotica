#include "Test.h"

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id
uint8_t on_off_test;
uint8_t on_off_test2;

extern osMessageQueueId_t mid_MsgMando;
extern osMessageQueueId_t mid_MsgGaraje;

 uint8_t tipo_operacion; // -- 1 = ABRIR PUERTA
                         // -- 0 = CERRAR PUERTA

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
		
		osMessageQueueGet(mid_MsgMando, &on_off_test,0,osWaitForever);
		osMessageQueuePut(mid_MsgGaraje, &on_off_test, 0, 0);
		

		/*
		osMessageQueuePut(mid_MsgMando, &on_off, 0, 0);
		osMessageQueueGet(mid_MsgPulsador, &modo, 0, 0);

		LCDDatos.modo = modo;

		LCDDatos.consumo = LCDDatos.consumo + 4.37;
		osMessageQueueGet(mid_MsgRTC, &datosFechaHora, 0, 0);
		
		LCDDatos.horas = datosFechaHora.horas;
		LCDDatos.minutos = datosFechaHora.minutos;
		LCDDatos.segundos = datosFechaHora.segundos;
		LCDDatos.anno = datosFechaHora.anno;
		LCDDatos.mes = datosFechaHora.mes;
		LCDDatos.dia = datosFechaHora.dia;
		
		osMessageQueueGet(mid_MsgTemp_Hum, &datos_SHT30, 0, 0);
		LCDDatos.temperatura = datos_SHT30.temperatura;
		LCDDatos.humedad = datos_SHT30.humedad;
		
		osDelay(500);
		osMessageQueuePut(mid_MsgLCD, &LCDDatos, 0, 0);
		*/
		
  }
	
	
}
