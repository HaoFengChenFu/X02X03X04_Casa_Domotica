#include "PIR.h"

extern osThreadId_t tid_ThPIR;
extern osThreadId_t tid_ThMando;
extern osThreadId_t tid_ThPulsador;


/*------------------------------------------------------------------
						Handler de la interrupcion
 -----------------------------------------------------------------*/
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);			// PIR
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);      // Usuario
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);			// Mando
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
			osThreadFlagsSet(tid_ThPIR, 2);
		}
	}
  else if(GPIO_PIN == GPIO_PIN_13){ //boton de usuario
 		
  }
  else if(GPIO_PIN == GPIO_PIN_12){    // Interrupcion del mando
		osThreadFlagsSet(tid_ThMando,0x01);
  }
}
