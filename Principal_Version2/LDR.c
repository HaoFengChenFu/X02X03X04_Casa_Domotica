#include "LDR.h"

static ADC_HandleTypeDef adc;

static float voltaje;
static uint8_t porcentaje_pulso;
osThreadId_t tid_ThLDR;    
osMessageQueueId_t mid_MsgLDR;

osTimerId_t timer_LDR_id;

void ThLDR (void *argument);                   // thread function

/*------------------------------------------------------------------------------
										
 -------------------------------------------------------------------------------*/

int Init_ThLDR (void) {
 
  tid_ThLDR = osThreadNew(ThLDR, NULL, NULL);
  if (tid_ThLDR == NULL) {
    return(-1);
  }
	ADC1_pins_F429ZI_config();
	
  return(0);
}

static void Timer_LDR_Callback(void const *arg)      // Callback creada para el timer virtual
{
	osThreadFlagsSet(tid_ThLDR, 0x1);
}

/*------------------------------------------------------------------
          Iniciacion del timer virtual para rebotes
 -----------------------------------------------------------------*/
int Init_timer_LDR (void)
{
	uint32_t exec;
	osStatus_t status;
  exec = 57U;
  timer_LDR_id = osTimerNew((osTimerFunc_t)&Timer_LDR_Callback, osTimerOnce, &exec, NULL);
  if(timer_LDR_id != NULL){
    if( status != osOK){
      return -1;
    }
  }
  return NULL;
}

/*------------------------------------------------------------------
						Iniciacion de la cola de mensajes
 -----------------------------------------------------------------*/
int Init_MsgLDR (void)
{
	osStatus_t status;
	mid_MsgLDR = osMessageQueueNew(4, sizeof(float), NULL);
	if (mid_MsgLDR != NULL){
			if( status != osOK){
				return -1;
			}
	}
	return 0;
}

/*------------------------------------------------------------------------------
										
 -------------------------------------------------------------------------------*/
void ThLDR (void *argument){
	Init_MsgLDR();
	Init_timer_LDR();
	uint8_t porcentaje_anterior = 0;
	ADC_Init_Single_Conversion(&adc,ADC1);
	while(1){
		voltaje = ADC_getVoltage(&adc,13);
		porcentaje_pulso=(uint8_t)((voltaje*100)/3.3f);
		if(porcentaje_pulso != porcentaje_anterior){
			osMessageQueuePut(mid_MsgLDR, &porcentaje_pulso, 0, osWaitForever);
		}
		
		porcentaje_anterior = porcentaje_pulso;
		osTimerStart(timer_LDR_id, 250);
		osThreadFlagsWait(0x1, osFlagsWaitAny, osWaitForever);
	}
}
