#include "alarms.h"
#include "tm_stm32f4_i2c.h"


uint8_t Alarm_data;

void Alarm_read(void){
	
	Alarm_data = TM_I2C_ReadNoRegister(I2C1, 0x45);
		
}

