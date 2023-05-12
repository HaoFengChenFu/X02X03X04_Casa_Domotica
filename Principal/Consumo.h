#ifndef __Consumo_H
#define __Consumo_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"
#include "stdint.h"
#include "stdio.h"
#include "cmsis_os2.h"
#include "adc.h"

	int Init_ThConsumo (void);
	int Init_MsgConsumo(void);
	int Init_timer_Consumo (void);
#endif
