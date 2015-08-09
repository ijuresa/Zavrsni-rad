
/* Includes ------------------------------------------------------------------*/
#include "main.h"
uint8_t data;
short temperature, a;
long pressure;


int main(void) 
{
	volatile uint32_t timer, timer_now, timer_tmp;
	int  timer_2m,timer_3m,timer_4m,timer_5m;
	
	
	/* System initialization*/
  SystemInit();
 
  /* Initialize I2C, SCL = PB6 and SDA = PB7 with 100kHz serial clock */
  TM_I2C_Init(I2C1, TM_I2C_PinsPack_1, 100000);

	TM_DELAY_Init();
	/* initialize the timers*/
	timer_2m = timer_3m = timer_4m = timer_5m =0;
	timer_tmp=0;
	/* BMP calibration function */
	BMP085Calibration();
	/* Fetching temperature value*/
	temperature = BMP085calculateTemperature(BMP085readUT());
	/* Fetching pressure value */
	pressure = BMP085calculatePressure(BMPreadUP());
	/* Calculating temperature value */
	temperature = temperature * 0.1;	

	TM_I2C_WriteNoRegister(I2C1, 0x44, 0x00);
	
	timer_now = Tickms();
	while (1) 
	{
	/* insert all real time function */
		
	Alarm_read();


		
	if ((Tickms() - timer_now+ timer_tmp) > 600){
			timer_tmp=Tickms() - timer_now -600;
			timer_now	= Tickms();
	/* one minute function*/
		
			TM_I2C_WriteNoRegister(I2C1, 0x44, 0xfe);		
			
		
		
			timer_2m++;
			timer_3m++;
      timer_4m++;
			timer_5m++;
			
			if ( timer_2m == 2){
	/* two minute function*/					
				
				TM_I2C_WriteNoRegister(I2C1, 0x44, 0xfc);		
				
				
				timer_2m=0;
			}	
			if(timer_3m ==3){
	/* three minute function*/						
			
				TM_I2C_WriteNoRegister(I2C1, 0x44, 0xf8);
				
					
				timer_3m=0;
			}	
			if( timer_4m == 4){
	/* four minute function*/	

				TM_I2C_WriteNoRegister(I2C1, 0x44, 0xf0);
				
						
				timer_4m=0;
			}
			if( timer_5m ==5){
	/* five minute function*/	
							
				TM_I2C_WriteNoRegister(I2C1, 0x44, 0xe0);
				
							
				timer_5m=0;
			}
			
	}
	
	/*	
		if (a >= 0xff)
		{
		 a = 0;	
		}
		TM_I2C_WriteNoRegister(I2C1, 0x44, a);
		a = a + 1;
		Delayms(500);
	*/
	
	}
}
	
