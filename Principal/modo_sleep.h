#ifndef __MODO_SLEEP_H
#define __MODO_SLEEP_H

	#include "stm32f4xx_hal.h"
	#include "stdio.h"
	#include "cmsis_os2.h"
	#include <stdio.h> 
  #include <string.h> 
  #include <stdlib.h>
	
	 /* Declaración de funciones */
	 
	void enter_sleep_mode(char * buffer_tx_flash, int num_palabras_flash);
	
#endif
