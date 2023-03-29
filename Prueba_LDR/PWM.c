#include "PWM.h"
#include "adc.h"
static GPIO_InitTypeDef GPIO_InitStruct;

static ADC_HandleTypeDef adc;

/*------------------------------------------------------------------------------
										Variables de los Timers
Importante Cambiar el nombre para evitar lios del nombre del timer o del canal usado

Tambiés habría que cambiar Init_PWM_Pin y Config_PWM_Pulse
 -------------------------------------------------------------------------------*/
static TIM_HandleTypeDef htim4;
static TIM_OC_InitTypeDef Channel_Tim4_Config;

osThreadId_t tid_ThPWM;

void ThPWM (void *argument);                   // thread function


/*------------------------------------------------------------------------------
							Pines correspondientes para el PWM de cada Led
										Timer 4 / Canal 4  Pin :PD15
										
 -------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
										Iniciacion de un timer con PWM
 -------------------------------------------------------------------------------*/
void Init_PWM_Pin(void)
{
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

/*---------------------------------------------------------------------------------------------------------
				Funcion para variar la intensidad del Led en funcion del pulso introducido como parametro
 --------------------------------------------------------------------------------------------------------*/
void Config_PWM_Pulse(float pulse, bool PWM_Habilitado)			// PWM_Habilitado, true o false en funcion del un boton de la Web
{	uint8_t porcentaje;
	porcentaje =  ((pulse*100)/ 3.3f);
	
	if (PWM_Habilitado){
		HAL_TIM_PWM_DeInit(&htim4);
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
		htim4.Instance = TIM4;
		htim4.Init.Prescaler = PRESCALER;
		htim4.Init.Period = PERIODO;//se genera una frecuencia de 500Hz
		HAL_TIM_OC_Init(&htim4);
		HAL_TIM_PWM_Init(&htim4);
		
		Channel_Tim4_Config.OCMode = TIM_OCMODE_PWM1;			// TIM_OCMODE_PWM2 para RGB / TIM_OCMODE_PWM1 para los Leds de la placa
		
		if (porcentaje < 85){
		Channel_Tim4_Config.Pulse = 0;		// Ciclo de trabajo del 70 % para que no ilumine demasiado por la noche
		}
		else{
		Channel_Tim4_Config.Pulse = 80*80;		// Si no se quiere cambiar la configuracion anterior, para el RGB se puede usar PERIODO - pulse*PERIODO/100
		}
		
		HAL_TIM_PWM_ConfigChannel(&htim4, &Channel_Tim4_Config, TIM_CHANNEL_4);				// Es importante para configurar el canal
		
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
	}
	else{
		HAL_TIM_PWM_DeInit(&htim4);
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
	}
}

int Init_ThPWM (void) {
 
  tid_ThPWM = osThreadNew(ThPWM, NULL, NULL);
  if (tid_ThPWM == NULL) {
    return(-1);
  }
	Init_PWM_Pin();
  return(0);
}

void ThPWM (void *argument){
float luzz=0;
	while(1){
	  //medida de luminosidad cada 1 segundo
		osDelay(2000);
		ADC_Init_Single_Conversion(&adc,ADC1);
		luzz=ADC_getVoltage(&adc,13);
		
		//actualizamos ancho del PWM (LEDS)
		Config_PWM_Pulse(luzz,true);
	
	 osThreadYield();                            // suspend thread
	}
}
