#include "Mando.h"
#include "PuertaGaraje.h"
extern osThreadId_t tid_ThGaraje;                        // thread id
osThreadId_t tid_Rebotes; 
static GPIO_InitTypeDef GPIO_InitStruct;
osTimerId_t rebotes_id;  
static void Init_Timer_Rebotes (void);

	osThreadId_t tid_ThMando;



void Init_Mando_Pin(void)
{
   __HAL_RCC_GPIOD_CLK_ENABLE();
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
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

	while(1){
	  //medida de luminosidad cada 1 segundo
    osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);
		Init_Timer_Rebotes();
    osTimerStart(rebotes_id,50);
	
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

osThreadFlagsSet(tid_ThGaraje,0x01);
	
}
int Init_ThRebotes (void) {
 
  tid_Rebotes = osThreadNew(ThRebotes, NULL, NULL);
  if (tid_Rebotes == NULL) {
    return(-1);
  }

  return(0);
}
void ThMando (void *argument){

	while(1){
	  //medida de luminosidad cada 1 segundo
    osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	
	 osThreadYield();                            // suspend thread
	}
  
}


void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN){
	if(GPIO_PIN_14 == GPIO_PIN){
		if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_14) == GPIO_PIN_RESET){
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
			  osThreadFlagsSet(tid_Rebotes,0x01);
		}
    else
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

		
	}
}
