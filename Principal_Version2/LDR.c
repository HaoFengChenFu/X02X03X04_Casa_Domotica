#include "LDR.h"

static ADC_HandleTypeDef adc;

	float voltaje;		//********************************************************* POR QUITAR
	uint8_t porcentaje_pulso;
osThreadId_t tid_ThLDR;    
osMessageQueueId_t mid_MsgLDR;

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
	uint8_t porcentaje_anterior = 0;
	ADC_Init_Single_Conversion(&adc,ADC1);
	while(1){
		voltaje = ADC_getVoltage(&adc,13);
		porcentaje_pulso=(uint8_t)((voltaje*100)/3.3f);
		if(porcentaje_pulso != porcentaje_anterior){
			osMessageQueuePut(mid_MsgLDR, &porcentaje_pulso, 0, osWaitForever);
		}
		
		porcentaje_anterior = porcentaje_pulso;

		osThreadYield();                            // suspend thread
	}
}
