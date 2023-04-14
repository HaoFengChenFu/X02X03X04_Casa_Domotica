#include "PIR.h"

osMessageQueueId_t mid_MsgPIR;
static uint8_t movimiento;
/*------------------------------------------------------------------------
             Inicialización de los pines para el PIR
 ------------------------------------------------------------------------*/	
void Init_PIR_Pin(void){
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

void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN){
	if(GPIO_PIN_11 == GPIO_PIN){
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == GPIO_PIN_RESET){
			printf("Presencia desaparecida\n");
			movimiento = 0;
			osMessageQueuePut(mid_MsgPIR, &movimiento, 0, 0);
		}
		else{
			printf("Presencia detectada\n");
			movimiento = 1;
			osMessageQueuePut(mid_MsgPIR, &movimiento, 0, 0);
		}
		
		
	}
}
