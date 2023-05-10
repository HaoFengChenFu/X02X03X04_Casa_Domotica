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

static TIM_HandleTypeDef htim11;
static TIM_OC_InitTypeDef Channel_Tim11_Config;
static GPIO_InitTypeDef GPIO_InitStruct;

//static void Init_servo_TimeOut (void);
extern void ThGaraje (void *argument);                   // thread function
static void Callback_TimerServo (void *argument);
static void Init_servo_Timer_periodic (void);
//static void Callback_TimeOut(void *argument);

 
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



void ThGaraje (void *argument) {
	Init_MsgQueue_Garaje();
	//Init_PWM_Garaje();
	
  while (1) {	
		osMessageQueueGet(mid_MsgGaraje, &on_off_garaje,0,osWaitForever);
		if(on_off_garaje == 0){
				while(pulso < 2000){
				pulso+=2;
				Config_PWM_Pulse_Garaje(pulso);
				osDelay(10);
			}
		}
		else{
				while(pulso > 1500){
				pulso-=2;
				Config_PWM_Pulse_Garaje(pulso);
				osDelay(10);
			}
		}
		osThreadYield();                            // suspend thread
	}
}
