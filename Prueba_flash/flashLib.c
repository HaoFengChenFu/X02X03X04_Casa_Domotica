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
 *           Escritura de en la memoria flash en una determinada direccion
 *
 *       argumentos de la funcion:
 *       uint32_t tipo_borrado: especifica si estamos realizando un borrado de todo el banco de memoria o un borrado de sector/página. 
 *       uint32_t bancos_borrados: especifica el banco que se quiere borrar en caso de un borrado masivo
 *       uint32_t sector_inicial: sector inicial a borrar cuando se selecciona el borrado por sectores
 *       uint32_t n_sectores: numero de sectores a borrar a partir del sector_inicial
 *       uint32_t rango_voltaje: define el paralelismo de borrado
 *       uint32_t n_byte: numero de bytes que se transfieren en la escritura 
 *       uint32_t direccion_escritura: direccion donde se van a escribir los datos
 *       uint32_t datos: datos a escribir en la direccion de escritura
 *---------------------------------------------------------------------------*/
 
 void escritura_flash(uint32_t sector_inicial,uint32_t n_sectores,uint32_t rango_voltaje, uint16_t numero_palabras_escritura , uint32_t inicio_direccion_escritura , uint32_t *datos){
	 //DESBLOQUEO
	  habilitacion_flash();
	 //BORRADO 
	 FLASH_EraseInitTypeDef pEreaseInit;
	 
	 pEreaseInit.TypeErase   = FLASH_TYPEERASE_SECTORS;//nos interesa borrado por sectores
	 pEreaseInit.Sector = sector_inicial;
	 pEreaseInit.NbSectors = n_sectores; 
	 pEreaseInit.VoltageRange = rango_voltaje;
	   
	 uint32_t sectorError=0;
	 
	if( HAL_FLASHEx_Erase(&pEreaseInit,&sectorError) != HAL_OK){
		
	  printf("Error al borrar sector de la flash");
	}
	//ESCRITURA PALABRA A PALABRA
	int i_palabras=0;//iterador que se utiliza para recorrer palabra a palabra
	
    while (i_palabras<numero_palabras_escritura)
	   {
	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, inicio_direccion_escritura, datos[i_palabras]) == HAL_OK)//escritura palabra a palabra
	     {
	    	 inicio_direccion_escritura += 4;  // incrementamos la direccion de escritura 4 bytes
	    	 i_palabras++;
	     }
	     else
	     {
	       /* Error occurred while writing data in Flash memory*/
	    	 printf("Error al escribir en la memoria flash");
	     }
	   }	
	//BLOQUEO
	proteccion_flash();
 }
 

 

 
 /*----------------------------------------------------------------------------
 *      Lectura de datos a partir de una determinada direccion de memoria
 *      argumentos de la función
 *      uint32_t inicio_direccion_escritura: direccion de escritura incial
 *      uint32_t *buffer: buffer donde se van a guardar los datos de la memoria flash
 *      uint16_t palabras_lectura: numero de palabras a leer
 *
 *---------------------------------------------------------------------------*/
 
 uint32_t* lectura_flash(uint32_t inicio_direccion_escritura, uint16_t palabras_lectura){
	 
	 uint32_t *buffer= (uint32_t*) malloc(palabras_lectura * sizeof(uint32_t));
	 int i=0;//iterador para guardar los datos leidos en el buffer
	 while (palabras_lectura != 0)
	{
		uint32_t *lectura_palabra = (uint32_t *)inicio_direccion_escritura;
		inicio_direccion_escritura += 4;//incrementamos 4 posiciones de memoria para leer la siguiente palabra
		buffer[i]= *lectura_palabra;//guardados dato leido en el buffer
		palabras_lectura--;
		i++;
	}
	 
	 return buffer;
 }
 
 /*----------------------------------------------------------------------------
 *      funcion que devuelve el nuemero de palabras que contiene una 
         cadena de caracteres. Esta función se utiliza para la escritura
 *---------------------------------------------------------------------------*/
 
 int num_palabras(char * cadena_caracteres){
	 
	 int n_words = (strlen(cadena_caracteres)/4)+((strlen(cadena_caracteres)%4)!=0);//determinamos el numero de palabras que hay en la variable (1 palabra son 32 bits)
	 
	 return n_words;
 }
 
 /*----------------------------------------------------------------------------
 *      conversion de datos de la flash a cadena de caracteres
 *---------------------------------------------------------------------------*/
 
 void conversion_string(uint32_t *datos, char *str){
	 
	 int num_bytes= ((strlen(datos)/4) + (strlen(datos)%4 != 0))*4;
	 
	 for(int i=0;i<num_bytes;i++){
		 str[i]=datos[i/4]>>(8*(i%4));
	 }
 }
 