#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "ThTermostato.h"


/*----------------------------------------------------------------------------
 *      Hilo donde se gestiona el control del ventilador en funcion de 
 *      la temperatura del ambiente.
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_ThTermostato;                        // thread id
 
extern void ThTermostato (void *argument);                   // thread function
 
int Init_ThThermostato (void) {
 
  tid_ThTermostato = osThreadNew(ThTermostato, NULL, NULL);
  if (tid_ThTermostato == NULL) {
    return(-1);
  }
  init_ventilador();
  return(0);
}
 
void ThTermostato (void *argument) {
 
  while (1) {
		//prueba del ventilador 
    osDelay(4000);
		encender_ventilador();
    osThreadYield();                            // suspend thread
  }
}


void init_ventilador(void){//PC6
	
	static GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
	
}

void encender_ventilador(void){
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
}

void apagar_ventilador(void){
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
	
}
