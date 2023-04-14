#include "Principal.h"


/*-------------------------------
  	Hilo Principal: Variables
 *-------------------------------*/
 
osThreadId_t tid_ThPrincipal;

static FechaHora datosFechaHora;
static DatosLCD LCDDatos;
static Mensaje_Temp_Hum datos_SHT30;

extern osMessageQueueId_t mid_MsgPIR;
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
	Init_All_Threads();
	
  return(0);
}

/*-----------------------------------------------------------------
HAY QUE ELIMINARLO: Es solo para probar la comunicacion del PIR
-----------------------------------------------------------------*/
void LED_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP; /*Elegimos modo Pull Up*/
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; /*Elegimos la velocidad*/
		
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_7|GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
}
//----------------------------------------------------------------------------------------------------------

/*---------------------------------------
			Inicializar los pines
 *-------------------------------------*/
void Init_All_Pins(void){
	Init_Pin_Pulsador();
	Init_PIR_Pin();
	LED_Init();
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
}


/*----------------------------------------------------------------------------
* 	Hilo Principal: Donde se van a gestionar las colas de mensajes
 *---------------------------------------------------------------------------*/
void ThPrincipal (void *argument) {
	uint8_t encender_luces = 0;
  while (1) {
	  
		osMessageQueueGet(mid_MsgPulsador, &modo, 0, 0);
		osMessageQueueGet(mid_MsgPIR, &encender_luces, 0, 0);
		
		if(encender_luces==1){
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		}
		else{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		}
		
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
