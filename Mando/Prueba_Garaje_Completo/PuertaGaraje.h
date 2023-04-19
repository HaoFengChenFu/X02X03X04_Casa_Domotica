#ifndef __PRUEBAGARAJE_H
#define __PRUEBAGARAJE_H
	#include "cmsis_os2.h"                          // CMSIS RTOS header file
	#include "stm32f4xx_hal.h"
	#include "stdio.h"
	#include "stdbool.h"
	
	
	#define UMBRAL 20 //temperatura umbral para encender el ventilador
	#define PRESCALER_TIM3 210-1
	#define PERIOD_TIM3 8000-1
	
	typedef struct
	{
		float temperatura;
		float humedad;
		
	} Mensaje_Temp_Hum_ventilador;

	int Init_ThThermostato (void);//init thread
  void init_ventilador(void);//inicializacion pin ventilador
	int Init_Ventilador_MsgTemp_Hum (void);
	void encender_ventilador(void);
  void apagar_ventilador(void);
	void move_servo(void);
	void stop_servo(void);
	void init_servo(void);
	int Init_ThGaraje (void);
	void init_PWM(void);
	
	
	
#endif
