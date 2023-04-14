#ifndef __PULSADOR_H
#define __PULSADOR_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

  int Init_ThPulsador (void);
	void Init_Pin_Pulsador(void);
  int Init_timer (void);
	int Init_MsgQueuePulsador (void);
	
#endif
