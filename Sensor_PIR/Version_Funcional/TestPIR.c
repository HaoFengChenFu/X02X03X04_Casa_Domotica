#include "Test.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id
uint8_t detectado;
extern osMessageQueueId_t mid_MsgPIR;

void Thread (void *argument);                   // thread function

int Init_Thread (void) {
	
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
	LED_Init();
  return(0);
}

void LED_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP; /*Elegimos modo Pull Up*/
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; /*Elegimos la velocidad*/
		
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_7|GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
}


void Thread (void *argument) {
 
  while (1) {
		if(osMessageQueueGetMsgSize(mid_MsgPIR) != 0){
			osMessageQueueGet(mid_MsgPIR, &detectado, 0, 0);
			
			if(detectado==1){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
			}
			
			
		}
  }
}
