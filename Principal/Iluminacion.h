#ifndef __ILUMINACION_H
#define __ILUMINACION_H

	#include "stdint.h"
	#include "string.h"
	#include "stm32f4xx_hal.h"
	#include "cmsis_os2.h"                          // CMSIS RTOS header file
	#include <stdbool.h>
	#include "adc.h"
	#include "stdio.h"

	#define PERIODO 8000-1
	#define PRESCALER 42-1

	typedef struct
	{
		uint8_t porcentaje_pulso;
		bool encender_luz;
		
	} Mensaje_Iluminacion;
	
	void Init_PWM_Iluminacion_Pin(void);
	void Config_PWM_Pulse(uint8_t pulse, bool PWM_Habilitado);
	int Init_ThIluminacion (void);
	
	
#endif
