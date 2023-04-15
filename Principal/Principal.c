#include "Principal.h"

/*-------------------------------
  	Hilo Principal: Variables
 *-------------------------------*/
 
osThreadId_t tid_ThPrincipal;

static FechaHora datosFechaHora;
static DatosLCD LCDDatos;
static Mensaje_Temp_Hum datos_SHT30;

Mensaje_Iluminacion datos_luz;

extern osMessageQueueId_t mid_MsgPIR;
extern osMessageQueueId_t mid_MsgRTC;
extern osMessageQueueId_t mid_MsgLCD;
extern osMessageQueueId_t mid_MsgTemp_Hum;
extern osMessageQueueId_t mid_MsgPulsador;

extern osMessageQueueId_t mid_Msg_Ventilador_Temphum;
extern osMessageQueueId_t mid_MsgLDR;
extern osMessageQueueId_t mid_MsgIluminacion;

/*****
Aqui hay que incluir la cola de mensajes del mando, sensor sismico, LDR, etc.
*****/

void ThPrincipal (void *argument); 

int Init_ThPrincipal (void) {
	
  tid_ThPrincipal = osThreadNew(ThPrincipal, NULL, NULL);
  if (tid_ThPrincipal == NULL) {
    return(-1);
  }
	Init_All_Threads();
	
  return(0);
}

/*---------------------------------------
			Inicializar los pines
 *-------------------------------------*/
void Init_All_Pins(void){
	Init_Pin_Pulsador();
	Init_PIR_Pin();
	Init_PWM_Pin();
	init_servo();
	init_ventilador();
}

/*---------------------------------------
			Inicializar los hilos
 *-------------------------------------*/
void Init_All_Threads (void){
	Init_ThLCD();
	Init_ThPulsador();
	Init_ThRTC();
	Init_ThSNTP();
	Init_ThTemp_Hum();
	Init_ThPIR();
	
	Init_ThThermostato();
	Init_ThLDR();
	Init_ThIluminacion();
}


/*----------------------------------------------------------------------------
* 	Hilo Principal: Donde se van a gestionar las colas de mensajes
 *---------------------------------------------------------------------------*/
void ThPrincipal (void *argument) {
	uint8_t encender_luces = 0;
	uint8_t porcentaje = 0;
	uint8_t modo = 0;
  while (1) {

		/* ---------------------------------------------------------------------
							Obtencion de informacion de los modulos de entrada
		--------------------------------------------------------------------- */
		osMessageQueueGet(mid_MsgPulsador, &modo, 0, 0);
		osMessageQueueGet(mid_MsgPIR, &encender_luces, 0, 0);
		osMessageQueueGet(mid_MsgLDR, &porcentaje, 0, 0);
		osMessageQueueGet(mid_MsgTemp_Hum, &datos_SHT30, 0, 0);
		osMessageQueueGet(mid_MsgRTC, &datosFechaHora, 0, 0);
		osMessageQueueGet(mid_MsgTemp_Hum, &datos_SHT30, 0, 0);
		
		/* ---------------------------------------------------------------------
			 Asignacion de valores para enviar mensajes a los módulos de salida
		--------------------------------------------------------------------- */
		LCDDatos.modo = modo;
		
		if(datos_luz.porcentaje_pulso != porcentaje || datos_luz.encender_luz != encender_luces){
			datos_luz.porcentaje_pulso = porcentaje;
			datos_luz.encender_luz = encender_luces;
			osMessageQueuePut(mid_MsgIluminacion, &datos_luz, 0, osWaitForever);
		}
			
		LCDDatos.temperatura = datos_SHT30.temperatura;
		LCDDatos.humedad = datos_SHT30.humedad;
		
		LCDDatos.horas = datosFechaHora.horas;
		LCDDatos.minutos = datosFechaHora.minutos;
		LCDDatos.segundos = datosFechaHora.segundos;
		LCDDatos.anno = datosFechaHora.anno;
		LCDDatos.mes = datosFechaHora.mes;
		LCDDatos.dia = datosFechaHora.dia;
		
		//---------------------------------------------------------------------------------------------------------------------------
		LCDDatos.consumo = 0; // Aqui debe ir el de consumo
		//---------------------------------------------------------------------------------------------------------------------------
		

		/* ---------------------------------------------------
				Envio de mensajes a los modulos de salida
		---------------------------------------------------- */
		osMessageQueuePut(mid_MsgLCD, &LCDDatos, 0, 0);
		osDelay(250);		// Para evitar que se este actualizando todo el rato
		osThreadYield();
  }
}
