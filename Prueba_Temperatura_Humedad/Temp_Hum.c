#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "Driver_i2c.h"
#include "Temp_Hum.h"
#include "stdio.h"
/*----------------------------------------------------------------------------
 *      Thread'ThTemp_Hum': Hilo para obtener el valor de la temperatura
 *---------------------------------------------------------------------------*/
osThreadId_t tid_ThTemp_Hum;                        // thread id
osMessageQueueId_t mid_MsgTemp_Hum;

extern ARM_DRIVER_I2C Driver_I2C1;
static ARM_DRIVER_I2C* I2Cdrv_1 = &Driver_I2C1;

static uint8_t periodic_adq_cmd[2];
static uint8_t sensor_output[6];

Mensaje_Temp_Hum Msg_Temp_Hum;

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
	if (event & ARM_I2C_EVENT_SLAVE_TRANSMIT) {
		osThreadFlagsSet(tid_ThTemp_Hum, 2);
	}
	
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

 
int Init_ThTemp_Hum (void) {
  tid_ThTemp_Hum = osThreadNew(ThTemp_Hum, NULL, NULL);
  if (tid_ThTemp_Hum == NULL) {
    return(-1);
  }
	__HAL_RCC_I2C1_CLK_ENABLE();

	I2Cdrv_1->Initialize(I2C_SignalEvent_TEMP_HUM);
	I2Cdrv_1->PowerControl(ARM_POWER_FULL);
	I2Cdrv_1->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);		// Se usa el Fast porque el sensor de temperatura como maximo soporta una frecuencia de 400 kHz
	I2Cdrv_1->Control(ARM_I2C_BUS_CLEAR, 0);
  return(0);
}
 
/*------------------------------------------------------------------
						Thread en ejecución
 -----------------------------------------------------------------*/
void ThTemp_Hum (void *argument) {
	periodic_adq_cmd[0] = CMD_PREIODIC_ADQ_MSB;
	periodic_adq_cmd[1] = CMD_PREIODIC_ADQ_LSB;
	
	Init_MsgTemp_Hum();
	
	printf("Inicializacializando comunicacion I2C\n");
	I2Cdrv_1->MasterTransmit(ADDR_TEMP_HUM, periodic_adq_cmd, 2, false);		// Direccion del sensor / Dato a transmitir (Para configurar el sensor) / Numero de bytes transmitidos / Falso porque no queremos que la operacion este pendiente
	osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);
  while (1) {
		osDelay(500);
		I2Cdrv_1->MasterReceive(ADDR_TEMP_HUM, sensor_output, 6, false);			// Debe recibir 6 bytes
		osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);
		
		
		Msg_Temp_Hum.temperatura = 175*((sensor_output[0]<<8)&(sensor_output[1]))/(65535)-45;
		Msg_Temp_Hum.humedad = 175*((sensor_output[3]<<8)&(sensor_output[4]))/(65535);
		
		
		osMessageQueuePut(mid_MsgTemp_Hum, &Msg_Temp_Hum, 0U, 0U);
		
		osThreadYield();
  }
}

