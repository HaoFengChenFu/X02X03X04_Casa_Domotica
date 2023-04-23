#include "Mando.h"
#include "PuertaGaraje.h"
uint8_t on_off = 0;
osThreadId_t tid_Rebotes; 
static GPIO_InitTypeDef GPIO_InitStruct;
osTimerId_t rebotes_id;  
static void Init_Timer_Rebotes (void);
int cnt_pulsaciones=0;
	osThreadId_t tid_ThMando;

osMessageQueueId_t mid_MsgMando;

void Init_Mando_Pin(void)
{
   __HAL_RCC_GPIOD_CLK_ENABLE();
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/*---------------------------------------------------------------------------------------------------------
				Funcion para variar la intensidad del Led en funcion del pulso introducido como parametro
 --------------------------------------------------------------------------------------------------------*/


int Init_ThMando  (void) {
 
  tid_ThMando = osThreadNew(tid_ThMando, NULL, NULL);
  if (tid_ThMando == NULL) {
    return(-1);
  }

  return(0);
}
void ThRebotes (void *argument){
	
Init_MsgQueue_Mando();
Init_Timer_Rebotes();
	while(1){
	  //medida de luminosidad cada 1 segundo
    osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);
		
    osTimerStart(rebotes_id,100);
	
	 osThreadYield();                            // suspend thread
	}
  
}
static void Init_Timer_Rebotes (void)  {
 
 uint32_t exec1=1U;// argument for the timer call back
	
 rebotes_id = osTimerNew(Callback_TimerRebotes, osTimerOnce,&exec1, NULL); // 
if (rebotes_id != NULL) {
    // One-shoot timer created
  }else{
		printf("timer no se creo correctamente");

	}
}
static void Callback_TimerRebotes (void *argument) {
  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_14) == GPIO_PIN_RESET){
	cnt_pulsaciones++;
		
	if(on_off==0) 
		 on_off=1;
	else 
		 on_off=0;
	
  osMessageQueuePut(mid_MsgMando, &on_off, 0, 0);
	}
}

/*------------------------------------------------------------------------------
										Iniciacion de la cola de mensajes
-------------------------------------------------------------------------------*/

int Init_MsgQueue_Mando(void)
{
	osStatus_t status;
	mid_MsgMando = osMessageQueueNew(4, sizeof(uint8_t), NULL);
	if (mid_MsgMando != NULL){
		// status = osMessageQueueGet(mid_MsgQueue, &msg, NULL, osWaitForever);				// Usado para comprobar que funciona correctamente
			if( status != osOK){
				return -1;
			}
	}
	return 0;
}

int Init_ThRebotes (void) {
 
  tid_Rebotes = osThreadNew(ThRebotes, NULL, NULL);
  if (tid_Rebotes == NULL) {
    return(-1);
  }

  return(0);
}

void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN){
	if(GPIO_PIN_14 == GPIO_PIN){

			  osThreadFlagsSet(tid_Rebotes,0x01);

    }
	}
