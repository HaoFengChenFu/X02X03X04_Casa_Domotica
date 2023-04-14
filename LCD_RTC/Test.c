#include "Test.h"
#include "rtc.h"
#include "lcd.h"
#include "Temp_Hum.h"
#include "pulsador.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id
FechaHora datosFechaHora;
DatosLCD LCDDatos;
Mensaje_Temp_Hum datos_SHT30;
extern osMessageQueueId_t mid_MsgRTC;
extern osMessageQueueId_t mid_MsgLCD;
extern osMessageQueueId_t mid_MsgTemp_Hum;
extern osMessageQueueId_t mid_MsgPulsador;
static uint8_t modo;

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
  }
}
