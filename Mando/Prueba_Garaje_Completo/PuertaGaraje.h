#ifndef __PUERTAGARAJE_H
#define __PUERTAGARAJE_H

	#include "cmsis_os2.h"                          // CMSIS RTOS header file
	#include "stm32f4xx_hal.h"
	#include "stdio.h"
	#include "stdbool.h"
	
	
	#define PRESCALER_TIM4 210-1
	#define PERIOD_TIM4 8000-1
	
	
	void move_servo(void);
	void stop_servo(void);
	void init_servo(void);
	int Init_ThGaraje (void);
	void Init_PWM_Garaje(void);
  int Init_MsgQueue_Garaje(void);	
	void Config_PWM_Pulse_Garaje(uint8_t pulse);
	
	
#endif
