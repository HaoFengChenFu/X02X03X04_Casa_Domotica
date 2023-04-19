#ifndef __THTERMOSTATO_H
#define __THTERMOSTATO_H
	#include "cmsis_os2.h"                          // CMSIS RTOS header file
	#include "stm32f4xx_hal.h"
	#include "stdio.h"
	#include "stdbool.h"
	
	
	#define UMBRAL 20 //temperatura umbral para encender el ventilador
	#define PRESCALER_TIM3 210-1
	#define PERIOD_TIM3 8000-1
	

	int Init_ThThermostato (void);//init thread
  void init_ventilador(void);//inicializacion pin ventilador
	int Init_Ventilador_Msg (void);
	void encender_ventilador(void);
  void apagar_ventilador(void);
	void move_servo(void);
	void stop_servo(void);
	void init_servo(void);
	
	
	
#endif
