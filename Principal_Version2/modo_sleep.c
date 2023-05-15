#include "modo_sleep.h"
#include "flashLib.h"
#include "pulsador.h"
#include "principal.h"
#include "rl_net.h"                     
#include "stm32f4xx_hal.h"  



/**
  * @brief  Esta función permite entrar al bajo consumo en el modo sleep.
  *         ==========  
  *            - System Running at (168MHz)
  *            - Wakeup using EXTI Line (user Button)
	*
  * @param  buffer_tx_flash -> buffer con los datos a enviar a la flash antes de entrar al modo sleep 
	* @param  num_palabras_escritura_flash -> numero de palabras de 32 bits que se escriben en la flash antes de ir al modo sleep
  * @retval None
  */

void enter_sleep_mode(char * buffer_tx_flash, int num_palabras_flash){
	
	/* Escritura de las medidas en la flash */
	
  escritura_flash_string_format(FLASH_SECTOR_7 ,1, 0x08060000 , buffer_tx_flash);//guardamos info
	
	char * buffer_rx=lectura_flash_string_format(0x08060000, num_palabras_flash);
	
	
		printf("%s",buffer_rx);
	
	 GPIO_InitTypeDef GPIO_InitStruct;

  /* Disable USB Clock */
////  __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

//  /* Disable Ethernet Clock */
   __HAL_RCC_ETH_CLK_DISABLE();

  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
  /* Enable GPIOs clock */
//  __HAL_RCC_GPIOA_CLK_ENABLE();
//  //__HAL_RCC_GPIOB_CLK_ENABLE();
//  //__HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
//  //__HAL_RCC_GPIOE_CLK_ENABLE();
//  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_All;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);

  __HAL_RCC_GPIOC_CLK_ENABLE();
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//  /* Disable GPIOs clock */
//  __HAL_RCC_GPIOA_CLK_DISABLE();
//  //__HAL_RCC_GPIOB_CLK_DISABLE();
//  //__HAL_RCC_GPIOC_CLK_DISABLE();
//  //__HAL_RCC_GPIOD_CLK_DISABLE();
//  //__HAL_RCC_GPIOE_CLK_DISABLE();
//  __HAL_RCC_GPIOF_CLK_DISABLE();
//  __HAL_RCC_GPIOG_CLK_DISABLE();
//  __HAL_RCC_GPIOH_CLK_DISABLE();
//  __HAL_RCC_GPIOI_CLK_DISABLE();
//  __HAL_RCC_GPIOJ_CLK_DISABLE();
//  __HAL_RCC_GPIOK_CLK_DISABLE();

  /* desactivamos la interrupcion del PIR */
	 desactivacion_PIR();
	 
	 Init_Pin_Pulsador();
	 Init_Mando_Pin();
	 
	 HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
	
  /* Suspend Tick increment to prevent wakeup by Systick interrupt. 
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
  
  HAL_SuspendTick();

  /* Request to enter SLEEP mode */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//  //__HAL_RCC_GPIOB_CLK_ENABLE();
//  //__HAL_RCC_GPIOC_CLK_ENABLE();
//  //__HAL_RCC_GPIOD_CLK_ENABLE();
//  //__HAL_RCC_GPIOE_CLK_ENABLE();
//  __HAL_RCC_GPIOF_CLK_ENABLE();
//  __HAL_RCC_GPIOG_CLK_ENABLE();
//  __HAL_RCC_GPIOH_CLK_ENABLE();
//  __HAL_RCC_GPIOI_CLK_ENABLE();
//  __HAL_RCC_GPIOJ_CLK_ENABLE();
//  __HAL_RCC_GPIOK_CLK_ENABLE();

  /* Resume Tick interrupt if disabled prior to sleep mode entry */
  HAL_ResumeTick();
	
	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
	
	Init_All_Pins();
  /* inicializacion ETHERNET */
  ETH_PhyExitFromPowerDownMode();
	//osDelay(3000);
	
}

void ETH_PhyExitFromPowerDownMode(void)
{
  // ETH_HandleTypeDef heth;
   GPIO_InitTypeDef GPIO_InitStruct;
   uint32_t phyregval = 0;
   
  /* ETH CLKs and GPIOs initialization ******************************/
  /* To be removed when the function is called from HAL_ETH_MspInit() when 
     exiting from Standby mode */
	
	/* Enable GPIO clocks*/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  /* Configure PA2 */
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL; 
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* Configure PC1*/
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  /* Enable ETH CLK */
  __HAL_RCC_ETH_CLK_ENABLE();

}
