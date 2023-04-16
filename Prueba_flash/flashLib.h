#ifndef __FLASHLIB_H
#define __FLASHLIB_H

	#include "stm32f4xx_hal.h"
	#include "stdio.h"
	#include "cmsis_os2.h"
	
	void unlock_flash();
	void proteccion_flash();
  void borrado_bloque_flash(uint32_t tipo_borrado,uint32_t bancos_borrados,uint32_t sector_inicial,uint32_t n_sectores,uint32_t rango_voltaje);
	void escritura_flash(uint32_t n_byte , uint32_t direccion_escritura , uint32_t datos);
	
#endif
