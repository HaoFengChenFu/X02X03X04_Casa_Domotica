#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
 extern "C" {
#endif 
#include "stdint.h"		// Hay que incluir esto porque el uint32_t, uint8_t y otros no estan definidos			
#include "stm32f4xx_hal.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"
#include "string.h"				// Usamos esta libreria para usar strlen
#include "stdio.h"				// Enunciado del ejercicio, queremos usar sprintf()
#include "cmsis_os2.h"

	typedef struct{
		uint8_t horas;
		uint8_t minutos;
		uint8_t segundos;
		
		uint8_t anno;
		uint8_t mes;
		uint8_t dia;
		
		float temperatura;
		float humedad;
	} DatosLCD;


 void LCD_reset(void);
 void delay(uint32_t microsegundos);
 void LCD_Init(void);
 void LCD_update(void);
 void LCD_symbolToLocalBuffer_L1(char symbol[], int size);
 void LCD_symbolToLocalBuffer_L2(char symbol[], int size);
 void symbolToLocalBuffer(uint8_t line, char symbol[], int size);
 void LCD_clear(void);

 int Init_ThLCD (void);
#ifdef __cplusplus
}
#endif

#endif
