#include "IWDG.h"

static IWDG_HandleTypeDef hiwdg;

void init_iwdg(void){
	//RECORDAR QUE EL LSI ES DE UNOS 32Khz
	//con prescaler de 64 tenemos una frec de 0,5KHz y un periodo de 2 ms
	//el registro de RL del IWDG tendra que ser de 2500 periodos para que el IWDG salte a los 5 s y se reinicie el sistema
	
	hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64;// div_64
  hiwdg.Init.Reload = 2500;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
  }
}

void refresco_iwdg(void){
	HAL_IWDG_Refresh(&hiwdg);//0xAAAA en el registro IWDG_KR
}
