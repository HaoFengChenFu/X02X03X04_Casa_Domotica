#ifndef __PIR_H
#define __PIR_H

	#include "stm32f4xx_hal.h"
	#include "stdio.h"
	#include "cmsis_os2.h"
	
	int Init_ThPulsador(void);
	void Init_PIR_Pin(void);
	
	int Init_MsgPIR (void);
	int Init_timer_PIR (void);
#endif
