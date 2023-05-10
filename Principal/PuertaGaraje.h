#ifndef __PUERTAGARAJE_H
#define __PUERTAGARAJE_H

	#include "cmsis_os2.h"                          // CMSIS RTOS header file
	#include "stm32f4xx_hal.h"
	#include "stdio.h"
	#include "stdbool.h"
	
	
	#define PRESCALER_TIM11 210-1
	#define PERIOD_TIM11 8000-1
	
	
	void mueve_garaje(void);
	void para_garaje(void);
	int Init_ThGaraje (void);
	void Init_PWM_Garaje(void);
  int Init_MsgQueue_Garaje(void);	
	void Config_PWM_Pulse_Garaje(uint8_t pulse);
	
	
#endif
