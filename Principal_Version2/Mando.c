#include "Mando.h"

osThreadId_t tid_ThMando;

/*------------------------------------------------------------------
            Variables del timer virtual y cola de mensajes
 -----------------------------------------------------------------*/
osTimerId_t rebotes_id;  
osMessageQueueId_t mid_MsgMando;
static uint8_t on_off = 0;


void ThMando (void *argument);
static void Init_Timer_Rebotes (void);

/*------------------------------------------------------------------
            Funcion del Thread
 -----------------------------------------------------------------*/
int Init_ThMando  (void) {
 
  tid_ThMando = osThreadNew(ThMando, NULL, NULL);
  if (tid_ThMando == NULL) {
    return(-1);
  }

  return(0);
}

/*------------------------------------------------------------------------
             Inicialización de los pines para el Mando
 ------------------------------------------------------------------------*/	
void Init_Mando_Pin(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
   __HAL_RCC_GPIOD_CLK_ENABLE();
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
  GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/*------------------------------------------------------------------
          Iniciacion del timer virtual para rebotes
 -----------------------------------------------------------------*/
static void Init_Timer_Rebotes (void)  {
 uint32_t exec1=26U;// argument for the timer call back
	
 rebotes_id = osTimerNew(Callback_TimerRebotes, osTimerOnce,&exec1, NULL); // 
if (rebotes_id != NULL) {
    // One-shoot timer created
  }else{
		printf("timer no se creo correctamente");

	}
}

/*------------------------------------------------------------------
          Callback del timer virtual para los rebotes
 -----------------------------------------------------------------*/
static void Callback_TimerRebotes (void *argument) {
  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_12) == GPIO_PIN_RESET){

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
			if( status != osOK){
				return -1;
			}
	}
	return 0;
}

/*------------------------------------------------------------------
            Thread en ejecución
 -----------------------------------------------------------------*/
void ThMando (void *argument){
	
	Init_MsgQueue_Mando();
	Init_Timer_Rebotes();
	
	while(1){

    osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);
		
    osTimerStart(rebotes_id,50);
	
		osThreadYield(); 
	}
  
}
