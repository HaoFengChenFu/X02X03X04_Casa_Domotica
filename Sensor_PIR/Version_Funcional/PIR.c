#include "PIR.h"

osThreadId_t tid_ThPIR;

/*------------------------------------------------------------------
            Variables del timer virtual
 -----------------------------------------------------------------*/
static osTimerId_t timer_rebotes;
static uint32_t exec;
static osStatus_t status;

osMessageQueueId_t mid_MsgPIR;
static uint8_t movimiento;

/*------------------------------------------------------------------
            Funcion del Thread
 -----------------------------------------------------------------*/
void ThPIR (void *argument);
 
int Init_ThPulsador (void) {
  tid_ThPIR = osThreadNew(ThPIR, NULL, NULL);
  if (tid_ThPIR == NULL) {
    return(-1);
  }

	Init_PIR_Pin();
  
  return(0);
}

/*------------------------------------------------------------------------
             Inicialización de los pines para el PIR
 ------------------------------------------------------------------------*/	
void Init_PIR_Pin(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOE_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;			// Configuro para que detecte ambas
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = GPIO_PIN_11;				// Para el sensor PIR se ha usado el Pin PE11
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}


/*------------------------------------------------------------------
          Callback del timer virtual para los rebotes
 -----------------------------------------------------------------*/
static void Timer_PIR_Callback(void const *arg)      // Callback creada para el timer virtual
{
		movimiento = 0;
		osMessageQueuePut(mid_MsgPIR, &movimiento, 0, 0);
}

/*------------------------------------------------------------------
          Iniciacion del timer virtual para rebotes
 -----------------------------------------------------------------*/
int Init_timer_PIR (void)
{
  exec = 5U;
  timer_rebotes = osTimerNew((osTimerFunc_t)&Timer_PIR_Callback, osTimerOnce, &exec, NULL);
  if(timer_rebotes != NULL){
    if( status != osOK){
      return -1;
    }
  }
  return NULL;
}

/*------------------------------------------------------------------
						Iniciacion de la cola de mensajes
 -----------------------------------------------------------------*/
int Init_MsgPIR (void)
{
	osStatus_t status;
	mid_MsgPIR = osMessageQueueNew(4, sizeof(movimiento), NULL);
	if (mid_MsgPIR != NULL){
			if( status != osOK){
				return -1;
			}
	}
	return 0;
}

/*------------------------------------------------------------------
						Handler de la interrupcion
 -----------------------------------------------------------------*/
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}

/*------------------------------------------------------------------
						Callback de la interrupcion
 -----------------------------------------------------------------*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN)
{
	if(GPIO_PIN_11 == GPIO_PIN){
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == GPIO_PIN_RESET){
			printf("Presencia desaparecida\n");
			osThreadFlagsSet(tid_ThPIR, 1);				// Cuando ya no hay movimiento esperamos 5 segundos y añadimos que movimiento = 0 en la cola de mensajes
		}
		else if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == GPIO_PIN_SET){
			printf("Presencia detectada\n");
			movimiento = 1;
			osMessageQueuePut(mid_MsgPIR, &movimiento, 0, 0);
		}
	}
}

/*------------------------------------------------------------------
            Thread en ejecución
 -----------------------------------------------------------------*/
void ThPIR (void *argument) 
{
  Init_timer_PIR();
  Init_MsgPIR();

  while (1) {
    osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);
    osTimerStart(timer_rebotes, 5000);    // Esperamos 5 segundos

  }
}
