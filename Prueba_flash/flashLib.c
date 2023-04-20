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
 *           Escritura de en la memoria flash en una determinada direccion.
 *       Una escritura en la flash consiste en:
 *       1º: habilitar la escritura de la flash
 *       2º: borrar el sector o secotores donde vamos a escribir
 *       3º: escritura en la flash palabra a palabra (de 32 en 32 bits) a partir de la direccion inicial
 *       4º: habilitar protección de escritura para prevenir posibles escrituras indeseadas en la flash
 *       
 *       argumentos de la funcion: 
 *       uint32_t sector_inicial: sector inicial a borrar (hay que borrar el sector donde se va a escribir informacion en la flash)
 *       uint32_t n_sectores: numero de sectores a borrar a partir del sector_inicial 
 *       uint16_t numero_palabras_escritura: numero de palabras que vamos a escribir en la flash
 *       uint32_t inicio_direccion_escritura: direccion a partir donde se van a escribir los datos
 *       uint32_t datos: datos a escribir a partir de la direccion inicial de escritura
 *---------------------------------------------------------------------------*/
 
 void escritura_flash(uint32_t sector_inicial,uint32_t n_sectores, uint16_t numero_palabras_escritura , uint32_t inicio_direccion_escritura , uint32_t *datos){
	 //DESBLOQUEO
	  habilitacion_flash();
	 //BORRADO 
	 //configuración de los parametros de borrado
	 FLASH_EraseInitTypeDef pEreaseInit;
	 
	 pEreaseInit.TypeErase   = FLASH_TYPEERASE_SECTORS;//nos interesa borrado por sectores
	 pEreaseInit.Sector = sector_inicial;
	 pEreaseInit.NbSectors = n_sectores; 
	 pEreaseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	   
	 uint32_t sectorError=0;
	 
	if( HAL_FLASHEx_Erase(&pEreaseInit,&sectorError) != HAL_OK){
		
	  printf("Error al borrar sector de la flash");
	}
	//ESCRITURA PALABRA A PALABRA
	int i_palabras=0;//iterador que se utiliza para recorrer palabra a palabra
	
    while (i_palabras<numero_palabras_escritura)//escritura en la flash palabra a palabra
	   {
	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, inicio_direccion_escritura, datos[i_palabras]) == HAL_OK)//escritura palabra a palabra
	     {
	    	 inicio_direccion_escritura += 4;  // incrementamos la direccion de escritura 4 bytes
	    	 i_palabras++;
	     }
	     else
	     {
	    	 printf("Error al escribir en la memoria flash");
	     }
	   }	
	//BLOQUEO
	proteccion_flash();
 }
 
 
 /*----------------------------------------------------------------------------
 *           Escritura de en la memoria flash en una determinada direccion cuando los datos a escribir son string
 *       Una escritura en la flash consiste en:
 *       1º: habilitar la escritura de la flash
 *       2º: borrar el sector o secotores donde vamos a escribir
 *       3º: escritura en la flash palabra a palabra (de 32 en 32 bits) a partir de la direccion inicial
 *       4º: habilitar protección de escritura para prevenir posibles escrituras indeseadas en la flash
 *       
 *       argumentos de la funcion: 
 *       uint32_t sector_inicial: sector inicial a borrar (hay que borrar el sector donde se va a escribir informacion en la flash)
 *       uint32_t n_sectores: numero de sectores a borrar a partir del sector_inicial 
 *       uint16_t numero_palabras_escritura: numero de palabras que vamos a escribir en la flash
 *       uint32_t inicio_direccion_escritura: direccion a partir donde se van a escribir los datos
 *       uint32_t datos: datos a escribir a partir de la direccion inicial de escritura
 *---------------------------------------------------------------------------*/
 
 void escritura_flash_string_format(uint32_t sector_inicial,uint32_t n_sectores, uint32_t inicio_direccion_escritura , char *datos){
	 
	 uint16_t numero_palabras_escritura =num_palabras_string(datos);//determinamos numero de palabras
	 escritura_flash( sector_inicial, n_sectores,  numero_palabras_escritura ,  inicio_direccion_escritura , (uint32_t*)datos);//escribimos en flash
	 
	 
 }
 

 

 
 /*----------------------------------------------------------------------------
 *      Lectura de datos a partir de una determinada direccion de memoria
 *
 *      argumentos de la función:
 *      uint32_t inicio_direccion_escritura: direccion de escritura incial
 *      uint16_t palabras_lectura: numero de palabras a leer
 *
 *     Return uint32_t buffer: buffer con los datos leidos 
 *---------------------------------------------------------------------------*/
 
 uint32_t* lectura_flash(uint32_t inicio_direccion_escritura, uint16_t palabras_lectura){
	 
	 uint32_t *buffer= (uint32_t*) malloc(palabras_lectura * sizeof(uint32_t));
	 int i=0;//iterador para guardar los datos leidos en el buffer
	 
	 while (palabras_lectura != 0)//hasta que no se hayan leido todas las palabras
	{
		uint32_t *lectura_palabra = (uint32_t *)inicio_direccion_escritura;//leemos la palabra que hay guarda en la direccion de memoria indicada por inicio_direccion_escritura 
		inicio_direccion_escritura += 4;//incrementamos 4 posiciones de memoria para leer la siguiente palabra
		buffer[i]= *lectura_palabra;//guardados dato leido en el buffer
		palabras_lectura--;
		i++;
	}
	 
	 return buffer;
 }
 
 
 /*----------------------------------------------------------------------------
 *      función de lectura de datos en formato strig que a partir de una determinada direccion de memoria.
 *      
 *
 *      argumentos de la función:
 *      uint32_t inicio_direccion_escritura: direccion de escritura incial
 *      uint16_t palabras_lectura: numero de palabras a leer
 *
 *     Return uint32_t buffer: buffer con los datos leidos 
 *---------------------------------------------------------------------------*/
 
 char* lectura_flash_string_format(uint32_t inicio_direccion_escritura, uint16_t palabras_lectura){
	 
	 char* buffer_str;//buffer donde vamos a guardar la informacion leida por la flash en formato string
	 
	 uint32_t *buffer= (uint32_t*) malloc(palabras_lectura * sizeof(uint32_t));
	 
	 int i=0;//iterador para guardar los datos leidos en el buffer
	 
	 while (palabras_lectura != 0)//hasta que no se hayan leido todas las palabras
	{
		uint32_t *lectura_palabra = (uint32_t *)inicio_direccion_escritura;//leemos la palabra que hay guarda en la direccion de memoria indicada por inicio_direccion_escritura 
		inicio_direccion_escritura += 4;//incrementamos 4 posiciones de memoria para leer la siguiente palabra
		buffer[i]= *lectura_palabra;//guardados dato leido en el buffer
		palabras_lectura--;
		i++;
	}
	
	buffer_str= conversion_string(buffer,buffer_str);
	 
	 return buffer_str;
 }
 
 /*----------------------------------------------------------------------------
 *      función que devuelve el número de palabras que contiene una 
 *      cadena de caracteres. Esta función se utiliza para determinar el nuemro de palabras
 *      que ocupa una cadena de caracteres en la memoria flash.
 *
 *     argumentos de la función:
 *     char* cadena_caracteres: variable de la cual queremos obtener el numero de palabras que ocupa en la flash
 *
 *     return n_words: numero de palabras que ocupa en la flash 
 *---------------------------------------------------------------------------*/
 
 int num_palabras_string(char * cadena_caracteres){
	 
	 int n_words = (strlen(cadena_caracteres)/4)+((strlen(cadena_caracteres)%4)!=0);//determinamos el numero de palabras que hay en la variable (1 palabra son 32 bits)
	 
	 return n_words;
 }
 
 /*----------------------------------------------------------------------------
 *      conversion de datos entregados por la flash a cadena de caracteres
 *      uint32_t --> char
 *
 *      argumentos de la función:
 *      uint32_t *datos: buffer con los datos entregados por la flash
 *      char *str: buffer con la conversion a string
 * 
 *      return str
 *---------------------------------------------------------------------------*/
 
 char* conversion_string(uint32_t *datos, char *str){
	 
  str = (char*) datos;//casteamos la variable datos a formato string.
	 
	return str;
	 
 }
 
 
 