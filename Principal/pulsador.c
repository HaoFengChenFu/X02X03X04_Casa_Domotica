#include "cmsis_os2.h" 
#include "pulsador.h"
#include "stm32f4xx_hal.h"

/*-------------------------------------------------------------------------------
 *      Thread 'ThPulsador': Hilo que gestiona las interrupciones del pulsador
 *-----------------------------------------------------------------------------*/
osThreadId_t tid_ThPulsador;

/*------------------------------------------------------------------
            Variables del timer virtual
 -----------------------------------------------------------------*/
static osTimerId_t timer_rebotes;
static uint32_t exec;
static osStatus_t status;

/*------------------------------------------------------------------
						Variables de la cola de mensajes
 -----------------------------------------------------------------*/
osMessageQueueId_t mid_MsgPulsador;
static uint8_t modo;

/*------------------------------------------------------------------
            Funcion del Thread
 -----------------------------------------------------------------*/
void ThPulsador (void *argument);
 
int Init_ThPulsador (void) {
  tid_ThPulsador = osThreadNew(ThPulsador, NULL, NULL);
  if (tid_ThPulsador == NULL) {
    return(-1);
  }

	Init_Pin_Pulsador();
  
  return(0);
}

/*------------------------------------------------------------------
          Funciona que inicializa el pin del pulsador
 -----------------------------------------------------------------*/
void Init_Pin_Pulsador(void)
{  
	GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;                               // Usuario
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);                          //Puertos C
  
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/*------------------------------------------------------------------
          Callback del timer virtual para los rebotes
 -----------------------------------------------------------------*/
static void Timer_Bounce_Callback(void const *arg)      // Callback creada para el timer virtual
{
    if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == 1 ){		//Usuario
			modo = !modo;
			osMessageQueuePut(mid_MsgPulsador, &modo, 0, 0);
    }
}

/*------------------------------------------------------------------
          Iniciacion del timer virtual para rebotes
 -----------------------------------------------------------------*/
int Init_timer (void)
{
  exec = 5U;
  timer_rebotes = osTimerNew((osTimerFunc_t)&Timer_Bounce_Callback, osTimerOnce, &exec, NULL);
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
int Init_MsgQueuePulsador (void)
{
	osStatus_t status;
	mid_MsgPulsador = osMessageQueueNew(4, sizeof(modo), NULL);
	if (mid_MsgPulsador != NULL){
			if( status != osOK){
				return -1;
			}
	}
	return 0;
}

/*------------------------------------------------------------------
            Thread en ejecución
 -----------------------------------------------------------------*/
void ThPulsador (void *argument) {
  Init_timer();
  Init_MsgQueuePulsador();
	modo = 0;
  while (1) {
    osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);
    osTimerStart(timer_rebotes, 50);    // Esperamos 50 ms

  }
}
