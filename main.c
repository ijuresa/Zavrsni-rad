
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Variables for temperature and pressure */
short temperature_3_3, temperature_5_0;
long pressure_3_3, pressure_5_0;

/* Initial value from PCF8575 read */
uint16_t pic_read = 0x0000;

int main(void) 
{
	
	/* System initialization*/
  SystemInit();
	
  /* Initialize I2C1, SCL = PB6 and SDA = PB7 with 100kHz serial clock */
  TM_I2C_Init(I2C1, TM_I2C_PinsPack_1, 100000);
	
	/* Initialize I2C2, SCL = PB10 and SDA = PB11 with 100kHz serial clock */
  TM_I2C_Init(I2C2, TM_I2C_PinsPack_1, 100000);
	
	/* Initialize delay */
	TM_DELAY_Init();
	
	/**************************************************************************************
	*		2 pics are connected to PCF8575 at pins: P17 abd P16
	*		They are enabled with 0xC000
	*		If output is 1 then PIC is ON else PIC is off
	***************************************************************************************/
	PCF8575_write(I2C2, 0xC000);
	pic_read = PCF8575_read(I2C2);


	/**************************************************************************************
	*		There are 2 same sensors with same address so I used 2 I2C interfaces 
	*		temperature_3_3 and pressure_3_3 are for BMP085 which is connected to 3.3V
	*		temperature_5_0 and pressure_5_0 are for BMP085 which is connected to 5.0V
	*		
	*		BMP085_3.3V is using I2C1 interface and BMP_5.0V is using I2C2 interface
	*		I called BMP085 calibration twice(once for each sensor) because hardware is different
	***************************************************************************************/
	/* BMP calibration function */ 
	BMP085Calibration(I2C1);
	/* Fetching temperature value */
	temperature_3_3 = BMP085calculateTemperature(BMP085readUT(I2C1));
	/* Fetching pressure value */
	pressure_3_3 = BMP085calculatePressure(BMPreadUP(I2C1));
	/* Calculating temperature value */
	temperature_3_3 = temperature_3_3 * 0.1;	
	

	 /* BMP calibration function */ 
	BMP085Calibration(I2C2);
	/* Fetching pressure value */
	temperature_5_0 = BMP085calculateTemperature(BMP085readUT(I2C2));
		/* Fetching pressure value */
	pressure_5_0 = BMP085calculatePressure(BMPreadUP(I2C2));
	/* Calculating temperature value */
	temperature_5_0 = temperature_5_0 * 0.1;	
	
	
	
	while (1) 
	{
		
		
		
		
	}
	
	
}
