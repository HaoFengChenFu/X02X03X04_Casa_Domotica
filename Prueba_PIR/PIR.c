#include "PIR.h"



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

void LED_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP; /*Elegimos modo Pull Up*/
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; /*Elegimos la velocidad*/
		
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_7|GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
}

void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN){
	if(GPIO_PIN_11 == GPIO_PIN){
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == GPIO_PIN_RESET){
			printf("Presencia desaparecida");
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
			
		}
		else{
			printf("Presencia detectada");
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		}
		
		
	}
}
