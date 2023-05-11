#include "Iluminacion.h" 
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_ThIluminacion;                        // thread id
osMessageQueueId_t mid_MsgIluminacion;

static TIM_HandleTypeDef htim4;
static TIM_OC_InitTypeDef Channel_Tim4_Config;
Mensaje_Iluminacion infoLuz;
osTimerId_t sismo_detectado_id;

void ThIluminacion (void *argument);                   // thread function
 
int Init_ThIluminacion(void) {
 
  tid_ThIluminacion = osThreadNew(ThIluminacion, NULL, NULL);
  if (tid_ThIluminacion == NULL) {
    return(-1);
  }
 
  return(0);
}

/*------------------------------------------------------------------------------
							Pines correspondientes para el PWM de cada Led
										Timer 4 / Canal 4  Pin :PD15
										
 -------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
										Iniciacion de un timer con PWM
 -------------------------------------------------------------------------------*/
void Init_PWM_Iluminacion_Pin(void)
{
	static GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	GPIO_InitStruct.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
  __HAL_RCC_GPIOD_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;		// Pagina 79, en la primea fila de la columna 4 indica que es AF2
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  __HAL_RCC_TIM4_CLK_ENABLE();
  htim4.Instance = TIM4;				 // Este timer usa 84 MHz
  htim4.Init.Prescaler = PRESCALER;   //El prescaler del timer (éste) es interno del timer
  htim4.Init.Period = PERIODO;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&htim4);
	
  Channel_Tim4_Config.OCMode = TIM_OCMODE_PWM1;
	Channel_Tim4_Config.Pulse = 0;				// Cuando es un nivel bajo el led se ilumina y cuando es una nivel alto el led se apaga
	Channel_Tim4_Config.OCPolarity =TIM_OCPOLARITY_HIGH;
	Channel_Tim4_Config.OCFastMode = TIM_OCFAST_DISABLE;
  
	HAL_TIM_PWM_ConfigChannel(&htim4, &Channel_Tim4_Config, TIM_CHANNEL_4);
}

void Config_PWM_Pulse(uint8_t pulse, bool PWM_Habilitado)			// PWM_Habilitado, true o false en funcion del un boton de la Web
{	
	if (PWM_Habilitado){
		HAL_TIM_PWM_DeInit(&htim4);
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
		htim4.Instance = TIM4;
		htim4.Init.Prescaler = PRESCALER;
		htim4.Init.Period = PERIODO;//se genera una frecuencia de 500Hz
		HAL_TIM_OC_Init(&htim4);
		HAL_TIM_PWM_Init(&htim4);
		
		Channel_Tim4_Config.OCMode = TIM_OCMODE_PWM1;	// TIM_OCMODE_PWM2 para RGB / TIM_OCMODE_PWM1 para los Leds de la placa
		
		if (pulse < 87){//mucha luminosidad
		Channel_Tim4_Config.Pulse = 0;		
		}
		else if(pulse >= 87 && pulse < 95){//rango 1
		Channel_Tim4_Config.Pulse = 80*50;				
		}else if(pulse >= 95 && pulse <= 97){//rango 2
		Channel_Tim4_Config.Pulse = 80*60;				
		}else if(pulse >= 97 && pulse <= 99){//rango 3
		Channel_Tim4_Config.Pulse = 80*70;				
		}else if(pulse >= 99 && pulse <= 100){//rango 4
		Channel_Tim4_Config.Pulse = 80*80;		
		}
		
		HAL_TIM_PWM_ConfigChannel(&htim4, &Channel_Tim4_Config, TIM_CHANNEL_4);				// Es importante para configurar el canal
		
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
	}
	else{
		HAL_TIM_PWM_DeInit(&htim4);
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
	}
}

static void Timer_Sismo_Detectado_Callback(void const *arg)      // Callback creada para el timer virtual
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}

/*------------------------------------------------------------------
          Iniciacion del timer virtual para rebotes
 -----------------------------------------------------------------*/
int Init_timer_Sismo_detectado (void)
{
	uint32_t exec;
	osStatus_t status;
  exec = 42U;
  sismo_detectado_id = osTimerNew((osTimerFunc_t)&Timer_Sismo_Detectado_Callback, osTimerOnce, &exec, NULL);
  if(sismo_detectado_id != NULL){
    if( status != osOK){
      return -1;
    }
  }
  return NULL;
}

/*------------------------------------------------------------------
						Iniciacion de la cola de mensajes
 -----------------------------------------------------------------*/
int Init_MsgIluminacion (void)
{
	osStatus_t status;
	mid_MsgIluminacion = osMessageQueueNew(4, sizeof(infoLuz), NULL);
	if (mid_MsgIluminacion != NULL){
			if( status != osOK){
				return -1;
			}
	}
	Init_PWM_Iluminacion_Pin();
	return 0;
}

void ThIluminacion (void *argument) {
	Init_MsgIluminacion();

  while (1) {
    osMessageQueueGet(mid_MsgIluminacion, &infoLuz, 0 , 0);
		Config_PWM_Pulse(infoLuz.porcentaje_pulso, infoLuz.encender_luz);
		
		if(infoLuz.sismo == 1){
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			osTimerStart(sismo_detectado_id, 30000);
		}
		
    osThreadYield();
  }
}
