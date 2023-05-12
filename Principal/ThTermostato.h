#ifndef __THTERMOSTATO_H
#define __THTERMOSTATO_H
	#include "cmsis_os2.h"                          // CMSIS RTOS header file
	#include "stm32f4xx_hal.h"
	#include "stdio.h"
	#include "stdbool.h"
	

	int Init_ThThermostato (void);//init thread
  void init_ventilador(void);//inicializacion pin ventilador
	int Init_Ventilador_Msg (void);
	void encender_ventilador(void);
  void apagar_ventilador(void);
	
#endif
