#ifndef __PWM_H
#define __PWM_H

	#include "stdint.h"
	#include <stdio.h>
	#include "string.h"
	#include "stm32f4xx_hal.h"
	#include "cmsis_os2.h"                          // CMSIS RTOS header file
	#include <stdbool.h>
	#include "adc.h"

	#define PERIODO 8000-1
	#define PRESCALER 42-1
	

	
	void Init_PWM_Pin(void);
	void Config_PWM_Pulse(float pulse, bool PWM_Habilitado);
	int Init_ThPWM (void);
	
	
#endif
