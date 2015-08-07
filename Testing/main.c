
/* Includes ------------------------------------------------------------------*/
#include "main.h"

short temperature;
long pressure;

int main(void) 
{
	/* System initialization*/
  SystemInit();
 
  /* Initialize I2C, SCL = PB6 and SDA = PB7 with 100kHz serial clock */
  TM_I2C_Init(I2C1, TM_I2C_PinsPack_1, 100000);

	TM_DELAY_Init();
	
	
	/* BMP calibration function */
	BMP085Calibration();
	/* Fetching temperature value*/
	temperature = BMP085calculateTemperature(BMP085readUT());
	/* Fetching pressure value */
	pressure = BMP085calculatePressure(BMPreadUP());
	/* Calculating temperature value */
	temperature = temperature * 0.1;	
	
  while (1) 
	{

	}
}
