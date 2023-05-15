#ifndef __LDR_H
#define __LDR_H

	#include "stm32f4xx_hal.h"
	#include "cmsis_os2.h"                          // CMSIS RTOS header file
	#include "adc.h"

	int Init_ThLDR (void);
	int Init_MsgLDR (void);
	int Init_timer_LDR (void);
#endif
