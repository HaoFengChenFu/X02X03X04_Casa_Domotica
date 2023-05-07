#include "pulsador.h"

/*-------------------------------------------------------------------------------
 *      Thread 'ThPulsador': Hilo que gestiona las interrupciones del pulsador
 *-----------------------------------------------------------------------------*/
osThreadId_t tid_ThPulsador;

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
            Thread en ejecución
 -----------------------------------------------------------------*/
void ThPulsador (void *argument) {
  while (1) {
    osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);
		
    /*
		
		
		Aqui entra en el modo de bajo consumo
		Y se configura el PIR para salir del modo de bajo consumo
		
		
		*/
		osThreadYield();
  }
}
