#ifndef __MANDO_H
#define __MANDO_H

	#include "stdint.h"
	#include <stdio.h>
	#include "string.h"
	#include "stm32f4xx_hal.h"
	#include "cmsis_os2.h"                          // CMSIS RTOS header file
	#include <stdbool.h>
//	#include "rl_net.h"			// Este deja usar el bool


	

	
int Init_ThMando  (void);
void	Init_Mando_Pin(void);
static void Callback_TimerRebotes (void *argument);
int Init_ThRebotes (void);
int Init_MsgQueue_Mando(void);	
#endif
