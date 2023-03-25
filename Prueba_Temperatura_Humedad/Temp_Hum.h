#ifndef __TemperaturaHumedad_H
#define __TemperaturaHumedad_H

	#define ADDR_TEMP_HUM 0x45
	#define CMD_PREIODIC_ADQ_MSB 0x2C
	#define CMD_PREIODIC_ADQ_LSB 0x06
	
	typedef struct
	{
		float temperatura;
		float humedad;
		
	} Mensaje_Temp_Hum;
	
	
	int Init_ThTemp_Hum (void);
	int Init_MsgTemp_Hum (void);

#endif
