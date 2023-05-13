#ifndef __SISMO_H
#define __SISMO_H

	#include "stm32f4xx_hal.h"
	#include "cmsis_os2.h"                          // CMSIS RTOS header file
	#include "adc.h"

	int Init_ThSismo (void);
	int Init_timer_Sismo (void);
	int Init_MsgSismo(void);
	
#endif
