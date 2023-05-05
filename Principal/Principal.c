#include "Principal.h"

/*-------------------------------
  	Hilo Principal: Variables
 *-------------------------------*/
 
osThreadId_t tid_ThPrincipal;

static FechaHora datosFechaHora;
static DatosLCD LCDDatos;
Mensaje_Temp_Hum datos_SHT30;
Tiempo_Fecha datos_horarios;
Mensaje_Iluminacion datos_luz;

// Cambiarlo a variable statica
uint8_t encender_vent = 0;
uint8_t vent_forzado, luz_forzada, vent_mode;

static osStatus_t nueva_medida_temp_hum;
char* dataTemp, dataHum;
uint8_t numPalabras;
char* buffer;

 char *buffer_rx_flash;
 char  buffer_tx_flash[5000];
static char string_escritura_flash[38];
 int numero_palabras_escritura_flash;//numero de palabras que se guardan en una unica escritura de la flash
static int numero_palabras_totales_flash=0;//numero de palabras totales que hemos guardado en la flash



/*---------------------------------
  	Cola de mensajes de entrada
 *---------------------------------*/
extern osMessageQueueId_t mid_MsgPIR;
extern osMessageQueueId_t mid_MsgRTC;
extern osMessageQueueId_t mid_MsgLDR;
extern osMessageQueueId_t mid_MsgTemp_Hum;
extern osMessageQueueId_t mid_MsgPulsador;
extern osMessageQueueId_t mid_MsgMando;

/*---------------------------------
  	Cola de mensajes de salida
 *---------------------------------*/
extern osMessageQueueId_t mid_Msg_Ventilador;
extern osMessageQueueId_t mid_MsgLDR;
extern osMessageQueueId_t mid_MsgLCD;
extern osMessageQueueId_t mid_MsgIluminacion;
extern osMessageQueueId_t mid_MsgGaraje;


extern uint8_t umbralTemp;
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
	Init_Mando_Pin();
	Init_PWM_Iluminacion_Pin();
	Init_PWM_Garaje();
	init_servo();
	init_ventilador();
}

/*---------------------------------------
			Inicializar los hilos
 *-------------------------------------*/
void Init_All_Threads (void){
	Init_ThThermostato();
	Init_ThLCD();
	Init_ThPulsador();
	Init_ThRTC();
	Init_ThSNTP();
	Init_ThTemp_Hum();
	Init_ThPIR();
	Init_ThLDR();

	Init_ThIluminacion();
	Init_ThMando();
	Init_ThGaraje();

}


/*----------------------------------------------------------------------------
* 	Hilo Principal: Donde se van a gestionar las colas de mensajes
 *---------------------------------------------------------------------------*/
void ThPrincipal (void *argument) {
	uint8_t encender_luces = 0;
	uint8_t porcentaje = 0;
	uint8_t modo = 0;
	uint8_t encender_vent_anterior = 0;
	uint8_t on_off_garaje = 0, on_off_garaje_anterior;
	
	
  while (1) {

		/* ---------------------------------------------------------------------
							Obtencion de informacion de los modulos de entrada
		--------------------------------------------------------------------- */
		osMessageQueueGet(mid_MsgPulsador, &modo, 0, 0);
		osMessageQueueGet(mid_MsgPIR, &encender_luces, 0, 0);
		osMessageQueueGet(mid_MsgLDR, &porcentaje, 0, 0);
		osMessageQueueGet(mid_MsgTemp_Hum, &datos_SHT30, 0, 0);
		osMessageQueueGet(mid_MsgRTC, &datosFechaHora, 0, 0);
		nueva_medida_temp_hum=osMessageQueueGet(mid_MsgTemp_Hum, &datos_SHT30, 0, 0);
		osMessageQueueGet(mid_MsgMando, &on_off_garaje, 0, 0);
	
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
		
		LCDDatos.horas = datos_horarios.horas = datosFechaHora.horas;
		LCDDatos.minutos = datos_horarios.minutos = datosFechaHora.minutos;
		LCDDatos.segundos = datos_horarios.segundos = datosFechaHora.segundos;
		LCDDatos.anno = datos_horarios.anno = datosFechaHora.anno;
		LCDDatos.mes = datos_horarios.mes = datosFechaHora.mes;
		LCDDatos.dia = datos_horarios.dia = datosFechaHora.dia;
		
		//---------------------------------------------------------------------------------------------------------------------------
		LCDDatos.consumo = 0; // Aqui debe ir el de consumo
		//---------------------------------------------------------------------------------------------------------------------------
		
		if(vent_mode == 0){
			encender_vent = (umbralTemp < datos_SHT30.temperatura) ? 1 : 0;
		}
		else{
			encender_vent = vent_forzado;
		}
		
		/* ---------------------------------------------------
				Envio de mensajes a los modulos de salida
		---------------------------------------------------- */
		if(encender_vent != encender_vent_anterior){
			osMessageQueuePut(mid_Msg_Ventilador, &encender_vent, 0, 0);
		}
		
		if( on_off_garaje_anterior != on_off_garaje){
			osMessageQueuePut(mid_MsgGaraje, &on_off_garaje, 0, 0);
		}
		on_off_garaje_anterior = on_off_garaje;
		encender_vent_anterior = encender_vent;
		
		osMessageQueuePut(mid_MsgLCD, &LCDDatos, 0, 0);
		
		/* ---------------------------------------------------------------------------------
				Escritura de las nuevas medidas de temperatura y humedad junto su time stamp
		    NOTA: (METER TODAS LAS MEDIDAS CUANDO VAYAMOS AL MODO SLEEP)
		------------------------------------------------------------------------------------ */
		
		if(nueva_medida_temp_hum == osOK){
		  sprintf(string_escritura_flash,"%.2d:%.2d:%.2d->T:%.2f|C H:%.2f%%\n",datosFechaHora.horas,datosFechaHora.minutos,datosFechaHora.segundos,datos_SHT30.temperatura,datos_SHT30.humedad);
		  strcat(buffer_tx_flash,string_escritura_flash);//concatenamos medidas realizadas
		}
		
  	osDelay(250);		// Para evitar que se este actualizando todo el rato
		osThreadYield();

	}	
}
