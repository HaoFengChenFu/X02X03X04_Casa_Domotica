#include "Consumo.h"

/*---------------------------------------
			id del Thread
---------------------------------------*/
osThreadId_t tid_ThConsumo;  

osMessageQueueId_t mid_MsgConsumo;
static float registro_consumo=0;
float consumo=0;
static osTimerId_t timer_Consumo;

/*-----------------------------
			Prototipo funciones
 ----------------------------*/
void Consumo_Thread (void *argument); 
int Init_MsgConsumo(void);

/*------------------------------------------------------------------
						Iniciacion del thread
 -----------------------------------------------------------------*/
int Init_ThConsumo (void) {
  tid_ThConsumo = osThreadNew(Consumo_Thread, NULL, NULL);
  if (tid_ThConsumo == NULL) {
    return(-1);
  }

  return(0);
}

/*------------------------------------------------------------------
						Iniciacion de la cola de mensajes
 -----------------------------------------------------------------*/
int Init_MsgConsumo(void)
{
	osStatus_t status;
	mid_MsgConsumo = osMessageQueueNew(4, sizeof(float), NULL);
	if (mid_MsgConsumo != NULL){
			if( status != osOK){
				return -1;
			}
	}
	return 0;
}

/*------------------------------------------------------------------
    Callback del timer virtual para obtener datos cada segundos
 -----------------------------------------------------------------*/
static void timer_Consumo_Callback(void const *arg)      // Callback creada para el timer virtual
{
	osThreadFlagsSet(tid_ThConsumo, 0x1);
}

/*------------------------------------------------------------------
          Iniciacion del timer virtual
 -----------------------------------------------------------------*/
int Init_timer_Consumo (void)
{	uint32_t exec;
	osStatus_t status;
  exec = 15U;
  timer_Consumo = osTimerNew((osTimerFunc_t)&timer_Consumo_Callback, osTimerOnce, &exec, NULL);
  if(timer_Consumo != NULL){
    if( status != osOK){
      return -1;
    }
  }
  return NULL;
}

/*-----------------------------------------------------------------------------------------
			Thread donde se pone el timer virtual para que se lance cada 3 minutos
 -----------------------------------------------------------------------------------------*/
void Consumo_Thread(void *argument) 
{	

	float corriente=0;
	ADC_HandleTypeDef adchandle;
	ADC1_pins_F429ZI_config();
	Init_MsgConsumo();
	ADC_Init_Single_Conversion(&adchandle , ADC1);		// Cambiar al ADC2 o al ADC3 para que no esten usando el mismo ADC
	Init_timer_Consumo();
	while(1){
		printf("CONSUMO\n");
		registro_consumo = 0;
		corriente = 0;
		for(int i = 0; i < 50; i++){
			registro_consumo = ADC_getVoltage(&adchandle, 10);
			corriente=corriente+((registro_consumo-1.63)/185);
			osTimerStart(timer_Consumo, 2);
			osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);
		}
		consumo = (corriente*1000)/50 ;
		consumo =  consumo+(consumo/10);
		osMessageQueuePut(mid_MsgConsumo, &consumo, 0 , 0);
		osTimerStart(timer_Consumo, 250);
		osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);
	}
}
