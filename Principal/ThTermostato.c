#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "ThTermostato.h"


/*----------------------------------------------------------------------------
 *      Hilo donde se gestiona el control del ventilador en funcion de 
 *      la temperatura del ambiente.
 *---------------------------------------------------------------------------*/
 
 static int pulso=400;//pulso del PWM del servomotor
 volatile static bool servo_sentido_horario=false;//servo moviendose hacia la izquierda
 
osThreadId_t tid_ThTermostato;                        // thread id

osTimerId_t periodic_id;                              //timer periodic id

static TIM_HandleTypeDef htim3;
static TIM_OC_InitTypeDef Channel_Tim3_Config;
static GPIO_InitTypeDef GPIO_InitStruct;

 
extern void ThTermostato (void *argument);                   // thread function
static void Callback_TimerServo (void *argument);
static void Init_servo_Timer_periodic (void);

osMessageQueueId_t mid_Msg_Ventilador_Temphum;//id cola que se comunicara con thread principal
uint8_t encender;


 
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
	init_servo();
  while (1) {	
		osMessageQueueGet(mid_Msg_Ventilador_Temphum, &encender, 0, osWaitForever);
		  if(encender==1){//comprobamos umbral
		    encender_ventilador();
				// -------------------------------------------------------------- POR DESCOMENTAR -------------------------------------------------------------
				//move_servo();
		  }else {
			  apagar_ventilador();
				//stop_servo();	
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

}
/*------------------------------------------------------------------
				Apagar ventilador
 -----------------------------------------------------------------*/
void apagar_ventilador(void){
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);

	
}

/*------------------------------------------------------------------
				inicializacion cola de mensajes para la comunicacion con
        el thread principal donde se metera la informacion de la 
         temperatura y humedad
 -----------------------------------------------------------------*/
int Init_Ventilador_MsgTemp_Hum (void)
{
	osStatus_t status;
	mid_Msg_Ventilador_Temphum = osMessageQueueNew(4, sizeof(uint8_t), NULL);
	if (mid_Msg_Ventilador_Temphum != NULL){
			if( status != osOK){
				return -1;
			}
	}
	return 0;
}

/*------------------------------------------------------------------
				función para mover el servomotor que esta unido al 
        ventilador 180º de un lado para otro
 -----------------------------------------------------------------*/

void move_servo(void){
	//set timer periodic 
	if (periodic_id == NULL) {
    Init_servo_Timer_periodic ();
  }
	osTimerStart(periodic_id,20);
}

/*------------------------------------------------------------------
				función para parar el servomotor
 -----------------------------------------------------------------*/
void stop_servo(void){
	//set timer periodic 
	if (periodic_id != NULL) {
    osTimerStop(periodic_id);
  }
	//paramos la señal PWM
	HAL_TIM_PWM_DeInit(&htim3);
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}


/*------------------------------------------------------------------
				función para inicializar el servomotor, se calibrará el
        servo respecto a los 0º del servomotor. El servo funciona 
        con una señal PWM. Dependiendo del ancho del pulso se movera
        unos grados espefificos.
        funciona con una frecuencia de 50 Hz (datasheet)
        mover a 0º= ancho de 1,5 ms (pulse=600)
        mover a 90º= ancho de 2 ms  (pulse=800)
        mover a -90º= ancho de 1 ms (pulse=600)
 -----------------------------------------------------------------*/

void init_servo(void){//MODO PWM generamos una señal de 50 Hz
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;		
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	
}

/*------------------------------------------------------------------
				inicialización de un timer periodico para rotar el 
        servomotor
 -----------------------------------------------------------------*/


static void Init_servo_Timer_periodic (void)  {
 
 uint32_t exec1=12U;// argument for the timer call back
	
 periodic_id = osTimerNew(Callback_TimerServo, osTimerPeriodic,&exec1, NULL); // 
if (periodic_id != NULL) {
    // One-shoot timer created
  }else{
		printf("timer del servo no ha sido creado correctamente");
	}
}


/*------------------------------------------------------------------
				           Callback del Timer periodico
        función para mover el servomotor que esta unido al 
        ventilador 180º de un lado para otro. Cada 20 ms se suma
        o se resta 1º para mover el ventilador 
 -----------------------------------------------------------------*/
static void Callback_TimerServo (void *argument) {
	
	htim3.Instance = TIM3;				 // Este timer usa 84 MHz
  htim3.Init.Prescaler = PRESCALER_TIM3;   //El prescaler del timer (éste) es interno del timer
  htim3.Init.Period = PERIOD_TIM3;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&htim3);
	
  Channel_Tim3_Config.OCMode = TIM_OCMODE_PWM1;
	Channel_Tim3_Config.Pulse = pulso; //control de servomotor en funcion del ancho del pulso
	Channel_Tim3_Config.OCPolarity =TIM_OCPOLARITY_HIGH;
	Channel_Tim3_Config.OCFastMode = TIM_OCFAST_DISABLE;
  
	HAL_TIM_PWM_ConfigChannel(&htim3, &Channel_Tim3_Config, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	
	
	if(pulso == 800){//cuando ha llegado a los 90º rotamos hacia el otro sentido
		servo_sentido_horario=true;
	}else if(pulso == 400){
		servo_sentido_horario=false;
	}
	
	if(servo_sentido_horario){
		pulso--;
	}else{
		pulso++;
	}
  
}








