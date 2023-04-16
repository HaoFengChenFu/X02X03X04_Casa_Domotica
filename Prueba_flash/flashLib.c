#include "flashLib.h"


/*----------------------------------------------------------------------------
 *      Habilitación de escritura y borrado de la memoria flash
 *---------------------------------------------------------------------------*/
 
 void habilitacion_flash(){
	 
	   if(HAL_FLASH_Unlock() == HAL_ERROR){
		   printf("Error al habilitar escritura de la memoria Flash");
	   }
 }
 
 /*----------------------------------------------------------------------------
 *      activacion de la protección de escritura y borrado de la memoria flash
 *---------------------------------------------------------------------------*/
 
 void proteccion_flash(){
	 
	   if(HAL_FLASH_Lock() == HAL_ERROR){
		   printf("Error al activar proteccion escritura de la memoria Flash");
	   }
 }
 
 /*----------------------------------------------------------------------------
 *       borrado de la memoria flash en un determinado bloque de datos
 *       previamente hay que desactivar la protecion de escritura y borrado
 *       argumentos de la funcion:
 *       uint32_t tipo_borrado: especifica si estamos realizando un borrado de todo el banco de memoria o un borrado de sector/página. 
 *       uint32_t bancos_borrados: especifica el banco que se quiere borrar en caso de un borrado masivo
 *       uint32_t sector_inicial: sector inicial a borrar cuando se selecciona el borrado por sectores
 *       uint32_t n_sectores: numero de sectores a borrar a partir del sector_inicial
 *       uint32_t rango_voltaje: define el paralelismo de borrado
 *---------------------------------------------------------------------------*/
 
 void borrado_bloque_flash(uint32_t tipo_borrado,uint32_t bancos_borrados,uint32_t sector_inicial,uint32_t n_sectores,uint32_t rango_voltaje){
	 FLASH_EraseInitTypeDef pEreaseInit;
	 
	 pEreaseInit.TypeErase   = tipo_borrado ;
	 
     if(pEreaseInit.TypeErase == FLASH_TYPEERASE_MASSERASE){//borrado masivo
	      pEreaseInit.Banks = bancos_borrados;
		 }else{//borrado por sectores
			 pEreaseInit.Sector = sector_inicial;
			 pEreaseInit.NbSectors = n_sectores;
		 } 
	 pEreaseInit.VoltageRange = rango_voltaje;
	   
	 uint32_t sectorError=0;
	 
	 HAL_FLASHEx_Erase(&pEreaseInit,&sectorError);
	 
 }
 
 /*----------------------------------------------------------------------------
 *      escritura de en la memoria flash en una determinada direccion
 *      uint32_t n_byte: numero de bytes que se transfieren en la escritura 
 *      uint32_t direccion_escritura: direccion donde se van a escribir los datos
 *      uint32_t datos: datos a escribir en la direccion de escritura
 *---------------------------------------------------------------------------*/
 
 void escritura_flash(uint32_t n_byte , uint32_t direccion_escritura , uint32_t datos){
	 
	  //habilitacion_flash();
	  //borrado_bloque_flash();
	  //HAL_FLASH_Program(n_byte,direccion_escritura,datos);
	  //proteccion_flash();
	 
 }
 
 