#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "PuertaGaraje.h"


/*----------------------------------------------------------------------------
 *      Hilo donde se gestiona el control del ventilador en funcion de 
 *      la temperatura del ambiente.
 *---------------------------------------------------------------------------*/
  static bool on_off=true;//puerta cerrada tras un reset del sistema
	static bool first_time=true;//puerta cerrada tras un reset del sistema
  int cnt_pulsaciones=0;
 static int pulso=400;//pulso del PWM del servomotor
 volatile static bool servo_sentido_horario=false;//servo moviendose hacia la izquierda
 
 osThreadId_t tid_ThGaraje;                        // thread id

osTimerId_t periodic_id;                              //timer periodic id
osTimerId_t id_time_out;

static TIM_HandleTypeDef htim3;
static TIM_OC_InitTypeDef Channel_Tim3_Config;
static GPIO_InitTypeDef GPIO_InitStruct;

 static void Init_servo_TimeOut (void);
extern void ThGaraje (void *argument);                   // thread function
static void Callback_TimerServo (void *argument);
static void Init_servo_Timer_periodic (void);
static void Callback_TimeOut(void *argument);
static bool ventilador_encendido=false;//guardamos el estado del ventilador


 
int Init_ThGaraje (void) {
 
  tid_ThGaraje = osThreadNew(ThGaraje, NULL, NULL);
  if (tid_ThGaraje == NULL) {
    return(-1);
  }
  return(0);
}
 
void ThGaraje (void *argument) {

  while (1) {	
  osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);
	 Init_servo_TimeOut();
		cnt_pulsaciones++;
	if(!on_off)
		on_off=true;
	else if(on_off){
		on_off=false;
		osTimerStop(id_time_out);
	}
	if(first_time)
		on_off=true;
	if(first_time)	
		first_time=!first_time;
	 if(on_off==1){
	servo_sentido_horario=true;
	}
	else
	servo_sentido_horario=false;
	move_servo();
  osThreadYield();                            // suspend thread
  }
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
	osTimerStart(periodic_id,10);
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
        mover a -90º= ancho de 1 ms (pulse=400)
 -----------------------------------------------------------------*/

void init_servo(void){//MODO PWM generamos una señal de 50 Hz
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;		
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	
}
static void Init_servo_TimeOut (void)  {
	 uint32_t exec1=1U;// argument for the timer call back
 id_time_out = osTimerNew(Callback_TimeOut, osTimerOnce,&exec1, NULL); // 
if (id_time_out != NULL) {
    // One-shoot timer created
  }else{
		printf("timer del servo no ha sido creado correctamente");
	}
}
static void Callback_TimeOut(void *argument){
servo_sentido_horario=true;
move_servo();
}

/*------------------------------------------------------------------
				inicialización de un timer periodico para rotar el 
        servomotor
 -----------------------------------------------------------------*/


static void Init_servo_Timer_periodic (void)  {
 
 uint32_t exec1=1U;// argument for the timer call back
	
 periodic_id = osTimerNew(Callback_TimerServo, osTimerPeriodic,&exec1, NULL); // 
if (periodic_id != NULL) {
    // One-shoot timer created
  }else{
		printf("timer del servo no ha sido creado correctamente");
	}
}
void init_PWM(void){
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

}
	

/*------------------------------------------------------------------
				           Callback del Timer periodico
        función para mover el servomotor que esta unido al 
        ventilador 180º de un lado para otro. Cada 20 ms se suma
        o se resta 1º para mover el ventilador 
 -----------------------------------------------------------------*/
static void Callback_TimerServo (void *argument) {
	
init_PWM();
	/*
	if(on_off==1){
	servo_sentido_horario=true;
	}
	else
	servo_sentido_horario=false;
	*/
	if(pulso == 800){//cuando ha llegado a los 90º rotamos hacia el otro sentido
		servo_sentido_horario=true;
		stop_servo();
		osTimerStart(id_time_out,5000);

		
	}else if(pulso == 400){
		servo_sentido_horario=false;
		stop_servo();
	}
	
	if(servo_sentido_horario){
		pulso--;
	}else{
		pulso++;
	}
  
}








