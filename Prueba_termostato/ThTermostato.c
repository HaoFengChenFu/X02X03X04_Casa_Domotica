#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "ThTermostato.h"


/*----------------------------------------------------------------------------
 *      Hilo donde se gestiona el control del ventilador en funcion de 
 *      la temperatura del ambiente.
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_ThTermostato;                        // thread id
 
extern void ThTermostato (void *argument);                   // thread function

osMessageQueueId_t mid_Msg_Ventilador_Temphum;//id cola que se comunicara con thread principal
Mensaje_Temp_Hum_ventilador info_temp_Hum;

static bool ventilador_encendido=false;//guardamos el estado del ventilador


 
int Init_ThThermostato (void) {
 
  tid_ThTermostato = osThreadNew(ThTermostato, NULL, NULL);
  if (tid_ThTermostato == NULL) {
    return(-1);
  }
  init_ventilador();
  return(0);
}
 
void ThTermostato (void *argument) {
  Init_Ventilador_MsgTemp_Hum ();
  while (1) {	
		if(osMessageQueueGetCount(mid_Msg_Ventilador_Temphum) != 0){
		  osMessageQueueGet(mid_Msg_Ventilador_Temphum, &info_temp_Hum, 0, 0);
		    if(info_temp_Hum.temperatura > UMBRAL && !ventilador_encendido){//comprobamos umbral
		      encender_ventilador();
		    }else if(info_temp_Hum.temperatura <= UMBRAL && ventilador_encendido){
					apagar_ventilador();
				}
	  }
  osThreadYield();                            // suspend thread
  }
}


/*------------------------------------------------------------------
				Inicializacion pin del ventilador
 -----------------------------------------------------------------*/
void init_ventilador(void){//PC6
	
	static GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
	
}
/*------------------------------------------------------------------
				Encender ventilador
 -----------------------------------------------------------------*/
void encender_ventilador(void){
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	ventilador_encendido=true;
}
/*------------------------------------------------------------------
				Apagar ventilador
 -----------------------------------------------------------------*/
void apagar_ventilador(void){
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
	ventilador_encendido=false;
	
}

/*------------------------------------------------------------------
				inicializacion cola de mensajes para la comunicacion con
        el thread principal donde se metera la informacion de la 
         temperatura y humedad
 -----------------------------------------------------------------*/
int Init_Ventilador_MsgTemp_Hum (void)
{
	osStatus_t status;
	mid_Msg_Ventilador_Temphum = osMessageQueueNew(4, sizeof(info_temp_Hum), NULL);
	if (mid_Msg_Ventilador_Temphum != NULL){
			if( status != osOK){
				return -1;
			}
	}
	return 0;
}

