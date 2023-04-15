#ifndef __PRINCIPAL_H
#define __PRINCIPAL_H
	#include "cmsis_os2.h"
	#include "stm32f4xx_hal.h"
	#include "stdio.h"
	
	#include "rtc.h"
	#include "lcd.h"
	#include "Temp_Hum.h"
	#include "pulsador.h"
	#include "PIR.h"
	
	#include "ThTermostato.h"
	//#include "PWM.h"
	#include "Iluminacion.h"
	#include "LDR.h"
	
	int Init_ThPrincipal (void);
	void Init_All_Pins (void);
	void Init_All_Threads (void);

#endif
