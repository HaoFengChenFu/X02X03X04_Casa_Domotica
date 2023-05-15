#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "PuertaGaraje.h"

/*----------------------------------------------------------------------------
 *      Hilo donde se gestiona el control del ventilador en funcion de 
 *      la temperatura del ambiente.
 *---------------------------------------------------------------------------*/
uint8_t on_off_garaje=NULL;
uint8_t hal;
static bool time_out=false;
uint16_t pulso = 2000;
osThreadId_t tid_ThGaraje;                        // thread id
osMessageQueueId_t mid_MsgGaraje;

osTimerId_t Timer_Garaje_id;                              //timer periodic id
osTimerId_t id_time_out;
osTimerId_t periodic_id_2;  
static TIM_HandleTypeDef htim11;
static TIM_OC_InitTypeDef Channel_Tim11_Config;
static GPIO_InitTypeDef GPIO_InitStruct;

//static void Init_servo_TimeOut (void);
extern void ThGaraje (void *argument);                   // thread function
static void Callback_TimerServo (void *argument);
static void Init_servo_Timer_periodic (void);
static void Callback_TimeOut(void *argument);

 
int Init_ThGaraje (void) {
  tid_ThGaraje = osThreadNew(ThGaraje, NULL, NULL);
	
  if (tid_ThGaraje == NULL) {
    return(-1);
  }
  return(0);
}
 



int Init_MsgQueue_Garaje(void){
	osStatus_t status;
	mid_MsgGaraje = osMessageQueueNew(4, sizeof(uint8_t), NULL);
	if (mid_MsgGaraje != NULL){
		// status = osMessageQueueGet(mid_MsgQueue, &msg, NULL, osWaitForever);				// Usado para comprobar que funciona correctamente
			if( status != osOK){
				return -1;
			}
	}
	return 0;
}



void Init_PWM_Garaje(void)
{
   __HAL_RCC_GPIOF_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Alternate = GPIO_AF3_TIM11;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  __HAL_RCC_TIM11_CLK_ENABLE();
  htim11.Instance = TIM11;				 // Este timer usa 84 MHz
  htim11.Init.Prescaler = PRESCALER_TIM11;   //El prescaler del timer (éste) es interno del timer
  htim11.Init.Period = PERIOD_TIM11;
	htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&htim11);
	
  Channel_Tim11_Config.OCMode = TIM_OCMODE_PWM1;
	Channel_Tim11_Config.Pulse = pulso;				// Cuando es un nivel bajo el led se ilumina y cuando es una nivel alto el led se apaga
	Channel_Tim11_Config.OCPolarity =TIM_OCPOLARITY_HIGH;
	Channel_Tim11_Config.OCFastMode = TIM_OCFAST_DISABLE;
  
	HAL_TIM_PWM_ConfigChannel(&htim11, &Channel_Tim11_Config, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);
}

/*---------------------------------------------------------------------------------------------------------
				Funcion para variar la intensidad del Led en funcion del pulso introducido como parametro
 --------------------------------------------------------------------------------------------------------*/

void Config_PWM_Pulse_Garaje(uint8_t pulse)
{
//	HAL_TIM_PWM_DeInit(&htim11);
//	HAL_TIM_PWM_Stop(&htim11, TIM_CHANNEL_1);
//	htim11.Instance = TIM4;
//	htim11.Init.Prescaler = PRESCALER_TIM11;
//	htim11.Init.Period = PERIOD_TIM11;
//	HAL_TIM_OC_Init(&htim11);
//	HAL_TIM_PWM_Init(&htim11);
//	
//	Channel_Tim11_Config.OCMode = TIM_OCMODE_PWM1;
	Channel_Tim11_Config.Pulse = pulso;		// Si queremos que no brille tanto cuando el volumen está al maximo se podria multiplicar por 100 o 50
	hal=HAL_TIM_PWM_ConfigChannel(&htim11, &Channel_Tim11_Config, TIM_CHANNEL_1);			// Es importante para configurar el canal
	
	HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);
}



void mueve_garaje(void){
	//set timer periodic 
	if (periodic_id_2 == NULL) {
    Init_servo_Timer_periodic ();
  }
	osTimerStart(periodic_id_2,10);
}

/*------------------------------------------------------------------
				función para parar el servomotor
 -----------------------------------------------------------------*/
void para_garaje(void){
	//set timer periodic 
	if (periodic_id_2 != NULL) {
    osTimerStop(periodic_id_2);
  }
	//paramos la señal PWM
	HAL_TIM_PWM_DeInit(&htim11);
	HAL_TIM_PWM_Stop(&htim11, TIM_CHANNEL_1);
}


/*------------------------------------------------------------------
				función para inicializar el servomotor, se calibrará el
        servo respecto a los 0º del servomotor. El servo funciona 
        con una señal PWM. Dependiendo del ancho del pulso se movera
        unos grados espefificos.
        funciona con una frecuencia de 50 Hz (datasheet)
        mover a 0º= ancho de 1,5 ms (pulse=600)
        mover a 90º= ancho de 2 ms  (pulse=800)
        mover a -90º= ancho de 1 ms (pulse=400)
 -----------------------------------------------------------------*/




static void Init_servo_TimeOut (void)  {
	 uint32_t exec1=27U;// argument for the timer call back
 id_time_out = osTimerNew(Callback_TimeOut, osTimerOnce,&exec1, NULL); // 
if (id_time_out != NULL) {
    // One-shoot timer created
  }else{
		printf("timer del servo no ha sido creado correctamente");
	}
}


static void Callback_TimeOut(void *argument){
  
	if(time_out==true)
		time_out=false;
	else
		time_out=true;
	
	mueve_garaje();


}

/*------------------------------------------------------------------
				inicialización de un timer periodico para rotar el 
        servomotor
 -----------------------------------------------------------------*/


static void Init_servo_Timer_periodic (void)  {
 
 uint32_t exec1=1U;// argument for the timer call back
	
 periodic_id_2 = osTimerNew(Callback_TimerServo, osTimerPeriodic,&exec1, NULL); // 
if (periodic_id_2 != NULL) {
    // One-shoot timer created
  }else{
		printf("timer del servo no ha sido creado correctamente");
	}
}
void ThGaraje (void *argument) {
	 Init_MsgQueue_Garaje();
	 Init_servo_TimeOut();
 Init_servo_Timer_periodic();
	 
  while (1) {	
		osMessageQueueGet(mid_MsgGaraje, &on_off_garaje,0,osWaitForever);




			mueve_garaje();
  
		osThreadYield();                            // suspend thread
  
	}
}

/*------------------------------------------------------------------
				           Callback del Timer periodico
        función para mover el servomotor que esta unido al 
        ventilador 180º de un lado para otro. Cada 20 ms se suma
        o se resta 1º para mover el ventilador 
 -----------------------------------------------------------------*/
static void Callback_TimerServo (void *argument) {
	
	if(on_off_garaje==0 && time_out==false){
		pulso++; //servo_bajando
		osTimerStop(id_time_out);
	}else if(on_off_garaje==1 && time_out==false){
		pulso--; //servo subiendo
	}else if(on_off_garaje==0 && time_out==true){
	  pulso--;
	}else{
	  pulso++ ;
	}

	//osTimerStop(id_time_out);
	if(pulso==2000 || pulso == 1500){

		para_garaje();
		if(pulso==1500)
			osTimerStart(id_time_out,5000);
	
	}
	
	
	
	Config_PWM_Pulse_Garaje(pulso);
	
}
