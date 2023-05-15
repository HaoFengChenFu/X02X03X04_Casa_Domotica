#include "LDR.h"

static ADC_HandleTypeDef adc;

static float voltaje_sismo;
static uint8_t sismo_detectado = 0;
osThreadId_t tid_ThSismo;   

osMessageQueueId_t mid_MsgSismo;
osTimerId_t sismo_id;

void ThSismo (void *argument);                   // thread function

/*------------------------------------------------------------------------------
										
 -------------------------------------------------------------------------------*/

int Init_ThSismo (void) {
 
  tid_ThSismo = osThreadNew(ThSismo, NULL, NULL);
  if (tid_ThSismo == NULL) {
    return(-1);
  }
	ADC1_pins_F429ZI_config();
	
  return(0);
}

static void Timer_Sismo_Callback(void const *arg)      // Callback creada para el timer virtual
{
	voltaje_sismo = ADC_getVoltage(&adc, 3);		// PA3
	if(voltaje_sismo > 1.5){
		sismo_detectado = 1;
		osMessageQueuePut(mid_MsgSismo, &sismo_detectado, 0 , 0);
		}else{
		sismo_detectado = 0;
		osMessageQueuePut(mid_MsgSismo, &sismo_detectado, 0 , 0);
	}
	osThreadFlagsSet(tid_ThSismo, 0x1);
}

/*------------------------------------------------------------------
          Iniciacion del timer virtual para rebotes
 -----------------------------------------------------------------*/
int Init_timer_Sismo (void)
{
	uint32_t exec;
	osStatus_t status;
  exec = 39U;
  sismo_id = osTimerNew((osTimerFunc_t)&Timer_Sismo_Callback, osTimerOnce, &exec, NULL);
  if(sismo_id != NULL){
    if( status != osOK){
      return -1;
    }
  }
  return NULL;
}

/*------------------------------------------------------------------
						Iniciacion de la cola de mensajes
 -----------------------------------------------------------------*/
int Init_MsgSismo (void)
{
	osStatus_t status;
	mid_MsgSismo = osMessageQueueNew(4, sizeof(uint8_t), NULL);
	if (mid_MsgSismo != NULL){
			if( status != osOK){
				return -1;
			}
	}
	return 0;
}

/*------------------------------------------------------------------------------
										
 -------------------------------------------------------------------------------*/
void ThSismo (void *argument){
	Init_MsgSismo();
	Init_timer_Sismo();
	
	ADC_Init_Single_Conversion(&adc,ADC1);
	while(1){
		
		osTimerStart(sismo_id, 70);
		osThreadYield(); 
		osThreadFlagsWait(0x1, osFlagsWaitAny, osWaitForever);
		
	}
}
