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

osTimerId_t timer_principal_id;
// Cambiarlo a variable statica
uint8_t encender_vent = 0;
uint8_t vent_forzado, vent_mode;
float consumo_rcv = 0;
	
 osStatus_t nueva_medida_temp_hum;
char* dataTemp, dataHum;
uint8_t numPalabras;
char* buffer;

 char  buffer_tx_flash[2800];//guardamos las ultimas 100 medidas (28 caracteres/palabra * 100)
static char string_escritura_flash[38];
int num_palabras_flash=0;//palabras a guardar en la flash
int cuenta=0;
uint8_t sismo;
/*---------------------------------
  	Cola de mensajes de entrada
 *---------------------------------*/
extern osMessageQueueId_t mid_MsgPIR;
extern osMessageQueueId_t mid_MsgRTC;
extern osMessageQueueId_t mid_MsgLDR;
extern osMessageQueueId_t mid_MsgTemp_Hum;
extern osMessageQueueId_t mid_MsgMando;
extern osMessageQueueId_t mid_MsgSismo;
extern osMessageQueueId_t mid_MsgConsumo;

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
	LCD_reset();
	LCD_Init();
	LCD_clear();
	Init_Pin_Pulsador();
	Init_PIR_Pin();
	Init_Mando_Pin();
	Init_PWM_Iluminacion_Pin();
	Init_PWM_Garaje();
	init_ventilador();
	Init_I2C_Temp_Hum();
}

/*---------------------------------------
			Inicializar los hilos
 *-------------------------------------*/
void Init_All_Threads (void){
	Init_ThThermostato();
	Init_ThLCD();
	Init_ThRTC();
	Init_ThSNTP();
	Init_ThTemp_Hum();
	Init_ThPIR();
	Init_ThLDR();
  //Init_ThPulsador();
	Init_ThIluminacion();
	Init_ThMando();
	Init_ThGaraje();
	Init_ThSismo();
	Init_ThConsumo();
}

static void Timer_Principal_Callback(void const *arg)      // Callback creada para el timer virtual
{
	osThreadFlagsSet(tid_ThPrincipal, 0x1);
}

/*------------------------------------------------------------------
          Iniciacion del timer virtual para rebotes
 -----------------------------------------------------------------*/
int Init_timer_Principal (void)
{
	uint32_t exec;
	osStatus_t status;
  exec = 54U;
  timer_principal_id = osTimerNew((osTimerFunc_t)&Timer_Principal_Callback, osTimerOnce, &exec, NULL);
  if(timer_principal_id != NULL){
    if( status != osOK){
      return -1;
    }
  }
  return NULL;
}


/*----------------------------------------------------------------------------
* 	Hilo Principal: Donde se van a gestionar las colas de mensajes
 *---------------------------------------------------------------------------*/
void ThPrincipal (void *argument) {
	uint8_t encender_luces = 0;
	uint8_t porcentaje = 0;
	uint8_t encender_vent_anterior = 0;
	uint8_t on_off_garaje = 0, on_off_garaje_anterior = 1;
	uint8_t sismo_anterior = 0;
	
	Init_timer_Principal();
	datos_luz.sismo = 0;
  while (1) {

		/* ---------------------------------------------------------------------
							Obtencion de informacion de los modulos de entrada
		--------------------------------------------------------------------- */
		osMessageQueueGet(mid_MsgPIR, &encender_luces, 0, 0);
		osMessageQueueGet(mid_MsgLDR, &porcentaje, 0, 0);
		osMessageQueueGet(mid_MsgTemp_Hum, &datos_SHT30, 0, 0);
		osMessageQueueGet(mid_MsgRTC, &datosFechaHora, 0, 0);
		nueva_medida_temp_hum=osMessageQueueGet(mid_MsgTemp_Hum, &datos_SHT30, 0, 120U);
		osMessageQueueGet(mid_MsgMando, &on_off_garaje, 0, 0);
		osMessageQueueGet(mid_MsgSismo, &sismo, 0, 0);
		
		//---------------------------------------------------------------------------------------------------------------------------
		
		// Aqui debe obtener el consumo
		osMessageQueueGet(mid_MsgConsumo, &consumo_rcv, 0, 0);
		//---------------------------------------------------------------------------------------------------------------------------
		
		/* ---------------------------------------------------------------------
			 Asignacion de valores para enviar mensajes a los módulos de salida
		--------------------------------------------------------------------- */
		LCDDatos.temperatura = datos_SHT30.temperatura;
		LCDDatos.humedad = datos_SHT30.humedad;
		
		LCDDatos.horas = datos_horarios.horas = datosFechaHora.horas;
		LCDDatos.minutos = datos_horarios.minutos = datosFechaHora.minutos;
		LCDDatos.segundos = datos_horarios.segundos = datosFechaHora.segundos;
		LCDDatos.anno = datos_horarios.anno = datosFechaHora.anno;
		LCDDatos.mes = datos_horarios.mes = datosFechaHora.mes;
		LCDDatos.dia = datos_horarios.dia = datosFechaHora.dia;
		
		
		if(datos_luz.porcentaje_pulso != porcentaje || datos_luz.encender_luz != encender_luces || sismo != sismo_anterior){
			datos_luz.sismo = sismo;
			datos_luz.porcentaje_pulso = porcentaje;
			datos_luz.encender_luz = encender_luces;
			osMessageQueuePut(mid_MsgIluminacion, &datos_luz, 0, osWaitForever);
		}
		
		sismo_anterior = sismo;

		
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
			if(buffer_tx_flash[2800-1]!= 0){//array lleno
				for(int i=0;i<2800-1;i++){//borramos el buffer
					buffer_tx_flash[i]=0;
				}
			}
		  sprintf(string_escritura_flash,"%.2d:%.2d:%.2d T:%.2f C H:%.2f%%\n",datosFechaHora.horas,datosFechaHora.minutos,datosFechaHora.segundos,datos_SHT30.temperatura,datos_SHT30.humedad);
		  strcat(buffer_tx_flash,string_escritura_flash);//concatenamos medidas realizadas
			num_palabras_flash=num_palabras_string(buffer_tx_flash);
			cuenta++;
		}
		
		// Para evitar que se este actualizando todo el rato
		osTimerStart(timer_principal_id, 250);
		osThreadYield();
		osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);

	}	
}
