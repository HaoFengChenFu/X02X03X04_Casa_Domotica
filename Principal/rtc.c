#include "rtc.h"

/*---------------------------------------
			id del Thread
---------------------------------------*/
osThreadId_t tid_ThRTC;  

RTC_HandleTypeDef hrtc;
RTC_AlarmTypeDef sAlarm;
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

static FechaHora datos_RTC; 
osMessageQueueId_t mid_MsgRTC;

/*-----------------------------
			Prototipo funciones
 ----------------------------*/
void RTC_Thread (void *argument); 
int Init_timer_RTC (void);
int Init_MsgRTC(void);
/*-----------------------
			Timer virtual
 ----------------------*/
osTimerId_t timer_RTC;
static uint32_t exec;

/*---------------------------------------------------------
							Inicialización del RTC
 --------------------------------------------------------*/
void RTC_Init(void)
{
	//__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);					// RCC_RTCCLKSOURCE_HSE_DIV10			RCC_RTCCLKSOURCE_LSE
	__HAL_RCC_RTC_ENABLE();
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_PWR_CLK_ENABLE();		// Para poder leer los registros

	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;					// 32,768kHz/((127+1)(255+1)) = 1 Hz			// Para LSE 127 y 255, para HSE de 8 MHz es 127 y 6249
	hrtc.Init.AsynchPrediv = 127;			//127;		//0x7f;
	hrtc.Init.SynchPrediv = 255;			//255;		//0x17ff;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);		// Habilitamos las interrupciones de las alarma del RTC
	HAL_RTC_Init(&hrtc);
}

/*-----------------------------------------------------------
						Obtención del tiempo en binario
 ----------------------------------------------------------*/
void Get_RTC_Time(void)
{
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}

/*----------------------------------------------------------
						Ajusta un nuevo tiempo
 ---------------------------------------------------------*/
void Set_RTC_Time(uint8_t hour, uint8_t minute, uint8_t second)
{
	sTime.Hours = hour;
	sTime.Minutes = minute;
	sTime.Seconds = second;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}
/*---------------------------------------------------------
						Obtención de la fecha en binario
 --------------------------------------------------------*/
void Get_Date_RTC(void)
{
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

/*-------------------------------------------------------
						Ajusta una nueva fecha
 ------------------------------------------------------*/
void Set_RTC_Date(uint8_t year, uint8_t month, uint8_t week, uint8_t date)
{
	sDate.Year = year;
	sDate.Month = month;
	sDate.WeekDay = week;
	sDate.Date = date;
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}
/*-------------------------------------------------
						Coloca una alarma
 ------------------------------------------------*/
void Set_Alarm(uint8_t hour, uint8_t minute, uint8_t second)
{
	sAlarm.AlarmTime.Hours = hour;
	sAlarm.AlarmTime.Minutes = minute;
	sAlarm.AlarmTime.Seconds = second;
	
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 21;
	
	sAlarm.AlarmMask = RTC_ALARMMASK_MINUTES;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE; 
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.Alarm = RTC_ALARM_A;
	
	HAL_RTC_AlarmIRQHandler(&hrtc);
	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
	HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);

}

/*------------------------------------------------------------------
						Handler de las alarmas
 -----------------------------------------------------------------*/
void RTC_Alarm_IRQHandler(void)
{
  HAL_RTC_AlarmIRQHandler(&hrtc);
}

/*------------------------------------------------------------------
						Callback de la alarma
 -----------------------------------------------------------------*/
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) 
{ 
	if(hrtc->Instance == RTC){
	}
}

/*------------------------------------------------------------------
			Muestra la fecha y el tiempo en el display
 -----------------------------------------------------------------*/
void Get_Date_Time(void)
{
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);//Primero el time y luego el date siempre
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	
	datos_RTC.horas = sTime.Hours;
	datos_RTC.minutos = sTime.Minutes;
	datos_RTC.segundos = sTime.Seconds;
	
	datos_RTC.anno = sDate.Year;
	datos_RTC.mes = sDate.Month;
	datos_RTC.dia = sDate.Date;

}

/*------------------------------------------------------------------
						Iniciacion del thread
 -----------------------------------------------------------------*/
int Init_ThRTC (void) {
  tid_ThRTC = osThreadNew(RTC_Thread, NULL, NULL);
  if (tid_ThRTC == NULL) {
    return(-1);
  }
	RTC_Init();
  return(0);
}

/*------------------------------------------------------------------
						Iniciacion de la cola de mensajes
 -----------------------------------------------------------------*/
int Init_MsgRTC(void)
{
	osStatus_t status;
	mid_MsgRTC = osMessageQueueNew(4, sizeof(FechaHora), NULL);
	if (mid_MsgRTC != NULL){
			if( status != osOK){
				return -1;
			}
	}
	return 0;
}

/*--------------------------------------------
				Callback del timer virtual
 -------------------------------------------*/
static void TimerRTC_Callback(void const *arg)			// Callback creada para el timer virtual
{
	Get_Date_Time();
	osMessageQueuePut(mid_MsgRTC, &datos_RTC, 0, 0);
	osThreadFlagsSet(tid_ThRTC, 1);
	
}

/*---------------------------------------------------
				Inicialización del timer virtual
 --------------------------------------------------*/
int Init_timer_RTC (void)
{
	exec = 4U;
	timer_RTC = osTimerNew((osTimerFunc_t)&TimerRTC_Callback, osTimerOnce, &exec, NULL);				// Se ha elegido periodic para que se lance cada 3 minutos
	if(timer_RTC != NULL){
		return -1;
	}
	return NULL;
}

/*-----------------------------------------------------------------------------------------
			Thread donde se pone el timer virtual para que se lance cada 3 minutos
 -----------------------------------------------------------------------------------------*/
void RTC_Thread(void *argument) 
{
	Init_timer_RTC();
	Init_MsgRTC();
	while(1){
		osTimerStart(timer_RTC, 1000);
		osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);
	}
}
