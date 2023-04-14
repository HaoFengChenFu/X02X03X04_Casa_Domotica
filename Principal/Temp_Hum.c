#include "Temp_Hum.h"

/*----------------------------------------------------------------------------
  Thread'ThTemp_Hum': Hilo para obtener el valor de la temperatura y humedad
 *---------------------------------------------------------------------------*/
osThreadId_t tid_ThTemp_Hum;                        // thread id

/*------------------------
		  Variables usadas
 *-----------------------*/
osMessageQueueId_t mid_MsgTemp_Hum;

extern ARM_DRIVER_I2C Driver_I2C1;
static ARM_DRIVER_I2C* I2Cdrv_1 = &Driver_I2C1;

static uint8_t periodic_adq_cmd[2];
static uint8_t sensor_output[6];
static Mensaje_Temp_Hum Msg_Temp_Hum;

/*------------------------------------------------------------------
            Variables del timer virtual
 -----------------------------------------------------------------*/
static osTimerId_t timer_TempHum;
static uint32_t exec;

void I2C_SignalEvent_TEMP_HUM (uint32_t event);
void ThTemp_Hum (void *argument);

 /*--------------------------------------------------
			 Evento del I2C
 -------------------------------------------------*/
 void I2C_SignalEvent_TEMP_HUM (uint32_t event) {
	uint32_t I2C_Event;
	I2C_Event |= event;
	 
	if (event & ARM_I2C_EVENT_TRANSFER_DONE) {
		printf("Transmision I2C finalizada\n");
    osThreadFlagsSet(tid_ThTemp_Hum, 1);
  }
}

/*------------------------------------------------------------------
    Callback del timer virtual para obtener datos cada segundos
 -----------------------------------------------------------------*/
static void Timer_TempHum_Callback(void const *arg)      // Callback creada para el timer virtual
{
	osThreadFlagsSet(tid_ThTemp_Hum, 0x2);
}

/*------------------------------------------------------------------
          Iniciacion del timer virtual
 -----------------------------------------------------------------*/
int Init_timer_TempHum (void)
{
	osStatus_t status;
  exec = 20U;
  timer_TempHum = osTimerNew((osTimerFunc_t)&Timer_TempHum_Callback, osTimerOnce, &exec, NULL);
  if(timer_TempHum != NULL){
    if( status != osOK){
      return -1;
    }
  }
  return NULL;
}

/*------------------------------------------------------------------
						Iniciacion de la cola de mensajes
 -----------------------------------------------------------------*/
int Init_MsgTemp_Hum (void)
{
	osStatus_t status;
	mid_MsgTemp_Hum = osMessageQueueNew(4, sizeof(Mensaje_Temp_Hum), NULL);
	if (mid_MsgTemp_Hum != NULL){
			if( status != osOK){
				return -1;
			}
	}
	return 0;
}

/*--------------------------------------------------------
						Iniciacion del thread
 --------------------------------------------------------*/
int Init_ThTemp_Hum (void) {
  tid_ThTemp_Hum = osThreadNew(ThTemp_Hum, NULL, NULL);
  if (tid_ThTemp_Hum == NULL) {
    return(-1);
  }

	I2Cdrv_1->Initialize(I2C_SignalEvent_TEMP_HUM);
	I2Cdrv_1->PowerControl(ARM_POWER_FULL);
	I2Cdrv_1->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);		// Se usa el Fast porque el sensor de temperatura como maximo soporta una frecuencia de 400 kHz
	I2Cdrv_1->Control(ARM_I2C_BUS_CLEAR, 0);
  return(0);
}

/*------------------------------------------------------------------
					Configuracion inicial del sensor
	1: Modo de adquisición periodica		
	0: Modo Single Shot
 -----------------------------------------------------------------*/
void Config_Communication(uint8_t modo_periodico)
{
	(modo_periodico == 1)? Set_PeriodicAdq_Mode() : Set_SingleShot_Mode();
	printf("Inicializacializando comunicacion I2C\n");
	I2Cdrv_1->MasterTransmit(ADDR_TEMP_HUM, periodic_adq_cmd, 2, false);		// Direccion del sensor / Dato a transmitir (Para configurar el sensor) / Numero de bytes transmitidos / Falso porque no queremos que la operacion este pendiente
	osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);
}

/*------------------------------------------------------------------
				Ajustarlo en Single Shot
 -----------------------------------------------------------------*/
void Set_SingleShot_Mode(void)
{
	periodic_adq_cmd[0] = CMD_SINGLE_ADQ_MSB;
	periodic_adq_cmd[1] = CMD_SINGLE_ADQ_LSB;
}

/*------------------------------------------------------------------
				Ajustarlo en Periodic Shot
 -----------------------------------------------------------------*/
void Set_PeriodicAdq_Mode(void)
{
	periodic_adq_cmd[0] = CMD_PERIODIC_ADQ_MSB;
	periodic_adq_cmd[1] = CMD_PERIODIC_ADQ_LSB;
}

/*------------------------------------------------------------------
			Lectura de los datos de los registros
 -----------------------------------------------------------------*/
void ReadSensorData(void)
{
	uint16_t aux_temp, aux_hum;
	I2Cdrv_1->MasterReceive(ADDR_TEMP_HUM, sensor_output, 6, false);			// Debe recibir 6 bytes
	osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);
	
	aux_temp = (sensor_output[0]<<8)|(sensor_output[1]);
	aux_hum = (sensor_output[3]<<8)|(sensor_output[4]);
	Msg_Temp_Hum.temperatura = 175*(((float)aux_temp/65535))-45;
	Msg_Temp_Hum.humedad = 175*((float)(aux_hum)/(65535));
}

/*------------------------------------------------------------------
						Thread en ejecución
 -----------------------------------------------------------------*/
void ThTemp_Hum (void *argument) {
	Init_MsgTemp_Hum();
	Init_timer_TempHum();
	
	Config_Communication(1);
	osDelay(5);			// Esperar 5 ms para que almenos se actualice los registros y asi no leer valores nulos
	
  while (1) {		
		ReadSensorData();
		osMessageQueuePut(mid_MsgTemp_Hum, &Msg_Temp_Hum, 0U, 0U);
		osTimerStart(timer_TempHum, 1000);
		osThreadYield();
		osThreadFlagsWait(0x2, osFlagsWaitAny, osWaitForever);
  }
}