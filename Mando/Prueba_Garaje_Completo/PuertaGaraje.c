#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "PuertaGaraje.h"


/*----------------------------------------------------------------------------
 *      Hilo donde se gestiona el control del ventilador en funcion de 
 *      la temperatura del ambiente.
 *---------------------------------------------------------------------------*/
 uint8_t on_off_garaje=NULL;

 static bool time_out=false;
 uint16_t pulso = 3100;
 osThreadId_t tid_ThGaraje;                        // thread id
osMessageQueueId_t mid_MsgGaraje;

osTimerId_t periodic_id;                              //timer periodic id
osTimerId_t id_time_out;

static TIM_HandleTypeDef htim4;
static TIM_OC_InitTypeDef Channel_Tim4_Config;
static GPIO_InitTypeDef GPIO_InitStruct;

static void Init_servo_TimeOut (void);
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
 
void ThGaraje (void *argument) {
	 Init_MsgQueue_Garaje();
	 Init_servo_TimeOut();

	 
  while (1) {	
		osMessageQueueGet(mid_MsgGaraje, &on_off_garaje,0,osWaitForever);




			move_servo();
  
		osThreadYield();                            // suspend thread
  
	}
}


int Init_MsgQueue_Garaje(void)
{
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
	HAL_TIM_PWM_DeInit(&htim4);
	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
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
	//on_off_garaje=0;
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

void Init_PWM_Garaje(void)
{
   __HAL_RCC_GPIOD_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;		// Pagina 79, en la primea fila de la columna 4 indica que es AF2
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  __HAL_RCC_TIM4_CLK_ENABLE();
  htim4.Instance = TIM4;				 // Este timer usa 84 MHz
  htim4.Init.Prescaler = PRESCALER_TIM4;   //El prescaler del timer (éste) es interno del timer
  htim4.Init.Period = PERIOD_TIM4;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&htim4);
	
  Channel_Tim4_Config.OCMode = TIM_OCMODE_PWM1;
	Channel_Tim4_Config.Pulse = 3100;				// Cuando es un nivel bajo el led se ilumina y cuando es una nivel alto el led se apaga
	Channel_Tim4_Config.OCPolarity =TIM_OCPOLARITY_HIGH;
	Channel_Tim4_Config.OCFastMode = TIM_OCFAST_DISABLE;
  
	HAL_TIM_PWM_ConfigChannel(&htim4, &Channel_Tim4_Config, TIM_CHANNEL_2);
}

/*---------------------------------------------------------------------------------------------------------
				Funcion para variar la intensidad del Led en funcion del pulso introducido como parametro
 --------------------------------------------------------------------------------------------------------*/

void Config_PWM_Pulse_Garaje(uint8_t pulse)
{
	HAL_TIM_PWM_DeInit(&htim4);
	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = PRESCALER_TIM4;
	htim4.Init.Period = PERIOD_TIM4;
	HAL_TIM_OC_Init(&htim4);
	HAL_TIM_PWM_Init(&htim4);
	
	Channel_Tim4_Config.OCMode = TIM_OCMODE_PWM2;
	Channel_Tim4_Config.Pulse = pulso;		// Si queremos que no brille tanto cuando el volumen está al maximo se podria multiplicar por 100 o 50
	HAL_TIM_PWM_ConfigChannel(&htim4, &Channel_Tim4_Config, TIM_CHANNEL_2);			// Es importante para configurar el canal
	
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
}
	

/*------------------------------------------------------------------
				           Callback del Timer periodico
        función para mover el servomotor que esta unido al 
        ventilador 180º de un lado para otro. Cada 20 ms se suma
        o se resta 1º para mover el ventilador 
 -----------------------------------------------------------------*/
static void Callback_TimerServo (void *argument) {
	
	if(on_off_garaje==0 && time_out==false){
		pulso--; //servo_bajando
	}else if(on_off_garaje==1 && time_out==false){
		pulso++; //servo subiendo
	}else if(on_off_garaje==0 && time_out==true){
	  pulso++;
	}else{
	  pulso--;
	}

	
	if(pulso==3100 || pulso == 3800){

		stop_servo();
		if(pulso==3800)
			osTimerStart(id_time_out,5000);
	
	}
	
	
	Config_PWM_Pulse_Garaje(pulso);
	
}








