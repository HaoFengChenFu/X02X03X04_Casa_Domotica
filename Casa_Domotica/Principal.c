#include "Principal.h"


/*----------------------------------------------------------------------------
* 	Hilo Principal: Donde se van a gestionar las colas de mensajes
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_ThPrincipal;

static FechaHora datosFechaHora;
static DatosLCD LCDDatos;
static Mensaje_Temp_Hum datos_SHT30;


extern osMessageQueueId_t mid_MsgRTC;
extern osMessageQueueId_t mid_MsgLCD;
extern osMessageQueueId_t mid_MsgTemp_Hum;
extern osMessageQueueId_t mid_MsgPulsador;

/*****
Aqui hay que incluir la cola de mensajes del mando, sensor sismico, LDR, etc.
*****/

static uint8_t modo;

void ThPrincipal (void *argument); 

int Init_ThPrincipal (void) {
	
  tid_ThPrincipal = osThreadNew(ThPrincipal, NULL, NULL);
  if (tid_ThPrincipal == NULL) {
    return(-1);
  }
  return(0);
}

/*----------------------------------------------------------------------------
			Inicializar los pines
 *---------------------------------------------------------------------------*/
void Init_All_Pins(void){
	Init_Pin_Pulsador();
	Init_PIR_Pin();
}

/*----------------------------------------------------------------------------
			Inicializar los hilos
 *---------------------------------------------------------------------------*/
void Init_All_Threads (void){
	Init_ThLCD();
	Init_ThPulsador();
	Init_ThRTC();
	Init_ThSNTP();
	Init_ThTemp_Hum();
}

/*----------------------------------------------------------------------------
* 	Hilo Principal: Donde se van a gestionar las colas de mensajes
 *---------------------------------------------------------------------------*/
void ThPrincipal (void *argument) {
 
  while (1) {
	  
		osMessageQueueGet(mid_MsgPulsador, &modo, 0, 0);

		LCDDatos.modo = modo;
		//---------------------------------------------------------------------------------------------------------------------------
		LCDDatos.consumo = 0; // Aqui debe ir el de consumo
		//---------------------------------------------------------------------------------------------------------------------------
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
