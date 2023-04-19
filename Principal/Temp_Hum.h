#ifndef __TemperaturaHumedad_H
#define __TemperaturaHumedad_H
	#include "cmsis_os2.h"                          // CMSIS RTOS header file
	#include "stm32f4xx_hal.h"
	#include "Driver_i2c.h"
	#include "stdio.h"
	
	#define ADDR_TEMP_HUM 0x45
	#define CMD_SINGLE_ADQ_MSB 0x2C
	#define CMD_SINGLE_ADQ_LSB 0x06
	
	#define CMD_PERIODIC_ADQ_MSB 0x21
	#define CMD_PERIODIC_ADQ_LSB 0x26
	
	typedef struct
	{
		float temperatura;
		float humedad;
		
	} Mensaje_Temp_Hum;
	
	
	int Init_ThTemp_Hum (void);
	int Init_MsgTemp_Hum (void);
	
	void Config_Communication(uint8_t modo_periodico);
	
	void Set_SingleShot_Mode(void);
	void Set_PeriodicAdq_Mode(void);
	void ReadSensorData(void);
	
#endif
