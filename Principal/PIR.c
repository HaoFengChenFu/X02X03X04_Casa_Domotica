#include "PIR.h"

osThreadId_t tid_ThPIR;

/*------------------------------------------------------------------
            Variables del timer virtual
 -----------------------------------------------------------------*/
static osTimerId_t timer_PIR;
static uint32_t exec;

osMessageQueueId_t mid_MsgPIR;
static uint8_t movimiento;

/*------------------------------------------------------------------
            Funcion del Thread
 -----------------------------------------------------------------*/
void ThPIR (void *argument);
 
int Init_ThPIR(void) {
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
	osStatus_t status;
  exec = 5U;
  timer_PIR = osTimerNew((osTimerFunc_t)&Timer_PIR_Callback, osTimerOnce, &exec, NULL);
  if(timer_PIR != NULL){
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
            Thread en ejecución
 -----------------------------------------------------------------*/
void ThPIR (void *argument) 
{
  Init_timer_PIR();
  Init_MsgPIR();

  while (1) {
		osThreadFlagsWait(2, osFlagsWaitAny, osWaitForever);		// Primero espera a que se detecta movimiento
		movimiento = 1;
    osMessageQueuePut(mid_MsgPIR, &movimiento, 0, 0);
		
    osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);		// Esperamos hasta que no haya movimiento para apagar las luces
    osTimerStart(timer_PIR, 5000);    // Esperamos 5 segundos
		osThreadYield();  
  }
}
