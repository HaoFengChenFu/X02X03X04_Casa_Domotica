#ifndef __PRINCIPAL_H
#define __PRINCIPAL_H
	#include "cmsis_os2.h"
	#include "stm32f4xx_hal.h"
	#include <stdio.h>
	#include <string.h>
	
	#include "rtc.h"
	#include "lcd.h"
	#include "Temp_Hum.h"
	#include "PIR.h"
	
	#include "ThTermostato.h"
	#include "Iluminacion.h"
	#include "LDR.h"
	
	#include "Mando.h"
	#include "PuertaGaraje.h"
	#include "flashLib.h"
	
	#include "pulsador.h"
	#include "Sismo.h"
	
	typedef struct{
		uint8_t horas;
		uint8_t minutos;
		uint8_t segundos;
		
		uint8_t anno;
		uint8_t mes;
		uint8_t dia;
	}Tiempo_Fecha;
	
	int Init_ThPrincipal (void);
	void Init_All_Pins (void);
	void Init_All_Threads (void);

#endif
