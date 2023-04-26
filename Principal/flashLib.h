#ifndef __FLASHLIB_H
#define __FLASHLIB_H

	#include "stm32f4xx_hal.h"
	#include "stdio.h"
	#include "cmsis_os2.h"
	#include <stdio.h> 
  #include <string.h> 
  #include <stdlib.h>
	
	void unlock_flash(void);
	void proteccion_flash(void);
  void escritura_flash(uint32_t sector_inicial,uint32_t n_sectores, uint16_t numero_palabras_escritura , uint32_t inicio_direccion_escritura , uint32_t *datos);
	void escritura_flash_string_format(uint32_t sector_inicial,uint32_t n_sectores, uint32_t inicio_direccion_escritura , char *datos);
	uint32_t* lectura_flash(uint32_t inicio_direccion_escritura, uint16_t palabras_lectura);
	char* lectura_flash_string_format(uint32_t inicio_direccion_escritura, uint16_t palabras_lectura);
	int num_palabras_string(char * cadena_caracteres);
	char* conversion_string(uint32_t *datos, char *str);
	
#endif
