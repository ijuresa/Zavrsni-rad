
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Variables for temperature and pressure */
float BMP085_temp_B, BMP085_temp_K;
long BMP085_press_B, BMP085_press_K;

/* Initial value for PCF8575 read */
uint16_t PCF8575_H = 0x0000;
uint16_t PCF8575_B = 0x0000;

uint8_t d[5];


uint16_t temperature, humidity, variabla;
volatile uint32_t time;


void DHT11_init()
{
	TM_GPIO_Init(GPIOD, GPIO_PIN_5, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
}

int DHT11_read()
{

	uint8_t j, i;
	
	TM_GPIO_SetPinAsOutput(GPIOD, GPIO_PIN_5);
	Delay(10);
	TM_GPIO_SetPinLow(GPIOD, GPIO_PIN_5);
	Delay(20000);
	
	TM_GPIO_SetPinHigh(GPIOD, GPIO_PIN_5);
	Delay(40);
	
	TM_GPIO_SetPinAsInput(GPIOD, GPIO_PIN_5);
	Delay(10);
	
	time = 0;
	//d[0] = TM_GPIO_GetInputPinValue(GPIOD, GPIO_PIN_7);
	for (j = 0; j < 5; j++) {
		for (i = 8; i > 0; i--) {
			/* We are in low signal now, wait for high signal and measure time */
			time = 0;
			/* Wait high signal, about 57-63us long (measured with logic analyzer) */
			while (!TM_GPIO_GetInputPinValue(GPIOD, GPIO_PIN_5)) {
		
				/* Increase time */
				time++;
				/* Wait 1 us */
				Delay(1);
			//}
			/* High signal detected, start measure high signal, it can be 26us for 0 or 70us for 1 */
			time = 0;
			/* Wait low signal, between 26 and 70us long (measured with logic analyzer) */
			while (TM_GPIO_GetInputPinValue(GPIOD, GPIO_PIN_5)) {
			
				 //Increase time 
				time++;
				// Wait 1 us 
				Delay(1);
			}
		
		if (time<35) {
			d[j/8] <<=1;
    }
   else {
		d[j/8] |= 1;
    }
		}
	}
}		
	humidity = d[0] << 8 | d[1];
	if (d[2] & 0x80) {
		temperature = -((d[2] & 0x7F) << 8 | d[3]);
	} else {
		temperature = (d[2]) << 8 | d[3];
	}
	if (((d[0] + d[1] + d[2] + d[3]) & 0xFF) != d[4])
	{
		return 1;
	}
	
	return 0;
	}


int main(void) 
{
	
	char stringPrint[30];
	uint8_t button1, button2, button3, buttonPressed;
	/* System initialization*/
  SystemInit();
	
  /* Initialize I2C1, SCL = PB8 and SDA = PB9 with 100kHz serial clock */
  TM_I2C_Init(I2C1, TM_I2C_PinsPack_2, 100000);
	
	/* Initialize I2C2, SCL = PB10 and SDA = PB11 with 100kHz serial clock */
  TM_I2C_Init(I2C2, TM_I2C_PinsPack_1, 100000);
	
	/* Initialize delay */
	TM_DELAY_Init();
	
	/* LCD initialization */
  TM_ILI9341_Init();
	TM_ILI9341_Fill(ILI9341_COLOR_BLUE2);
	
	/* Structure for touch */
	TM_STMPE811_TouchData touchData;
	
	/* Button initialization */
  TM_ILI9341_Button_t button;
		
	/**************************************************************************************
	*		2 PCF8575 I2C expaders are used with 2 PICs each connected to it's own PCF
	*		PICs are connected to PD17 and set as input 0x800
	*		If output is 0x8000 then PIC is ON else PIC is off
	***************************************************************************************/
	PCF8575_write(I2C1, 0x8000);
	PCF8575_write(I2C2, 0x8000);

	/* Initialize UART4 at 9600 baud, TX: PA0, RX: PA1 */
  TM_USART_Init(UART4, TM_USART_PinsPack_1, 9600);
	
	/* LCD screen rotation */
  TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_1);
  
	
  /* Touch initialization */
  if (TM_STMPE811_Init() != TM_STMPE811_State_Ok) 
	{
		TM_ILI9341_Puts(20, 20, "STMPE811 Error", &TM_Font_11x18, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
    while (1);
  }
   
  /* Touch screen orientation */
  touchData.orientation = TM_STMPE811_Orientation_Landscape_1;
    
  /* Button 1, default configuration 
  Red with black border and black font 11x18 */
  button.x = 0;
  button.y = 190;
  button.width = 100;
  button.height = 50;
  button.background = ILI9341_COLOR_RED;
  button.borderColor = ILI9341_COLOR_BLACK;
  button.label = "Button 1";
  button.color = ILI9341_COLOR_BLACK;
  button.font = &TM_Font_11x18;
  /* Add button */
  button1 = TM_ILI9341_Button_Add(&button);
	
	/* Button 2, default configuration
  Red with black border and black font 11x18 */
  button.x = 110;
  button.y = 190;
  button.width = 100;
  button.height = 50;
  button.background = ILI9341_COLOR_RED;
  button.borderColor = ILI9341_COLOR_BLACK;
  button.label = "Button 2";
  button.color = ILI9341_COLOR_BLACK;
  button.font = &TM_Font_11x18;
 /* Add button */
  button2 = TM_ILI9341_Button_Add(&button);
	
	/* Button 3, default configuration
  Red with black border and black font 11x18 */
  button.x = 220;
  button.y = 190;
  button.width = 100;
  button.height = 50;
  button.background = ILI9341_COLOR_RED;
  button.borderColor = ILI9341_COLOR_BLACK;
  button.label = "Button 3";
  button.color = ILI9341_COLOR_BLACK;
  button.font = &TM_Font_11x18;
  /* Add button */
  button3 = TM_ILI9341_Button_Add(&button);
	
	/* Draw buttons */
  TM_ILI9341_Button_DrawAll();

	/* Setting time counter */
	TM_DELAY_SetTime(0);
	DHT11_init();
	DHT11_read();

	while (1) 
	{
		/* Checking if LCD has been touched */
		if (TM_STMPE811_ReadTouch(&touchData) == TM_STMPE811_State_Pressed) 
		{
			buttonPressed = TM_ILI9341_Button_Touch(&touchData);
			if(buttonPressed == button1)
			{
				/* BMP calibration function */
				BMP085Calibration(I2C1);
				
				/* Fetching temperature value */
				BMP085_temp_K = BMP085calculateTemperature(BMP085readUT(I2C1));
				/* Fetching pressure value */
				BMP085_press_K = BMP085calculatePressure(BMPreadUP(I2C1));
				
				/* Calculating temperature value */
				BMP085_temp_K = BMP085_temp_K * 0.1;	
				
				/* Title */
				sprintf(stringPrint, "KUHINJA");
				TM_ILI9341_Puts(100, 25, stringPrint, &TM_Font_16x26, ILI9341_COLOR_RED, ILI9341_COLOR_CYAN);
				
				/* Displaying message on LCD */
				sprintf(stringPrint, "Temperature: %.1f", BMP085_temp_K);
				TM_ILI9341_Puts(5, 70, stringPrint, &TM_Font_16x26, ILI9341_COLOR_RED, ILI9341_COLOR_CYAN);
				sprintf(stringPrint, "Pressure: %ld", BMP085_press_K);
				TM_ILI9341_Puts(5, 100, stringPrint, &TM_Font_16x26, ILI9341_COLOR_RED, ILI9341_COLOR_CYAN);
				
			}
			else if(buttonPressed == button2)
			{		
				/* BMP calibration function */
				BMP085Calibration(I2C2);
				
				/* Fetching temperature value */
				BMP085_temp_B = BMP085calculateTemperature(BMP085readUT(I2C2));
				/* Fetching pressure value */
				BMP085_press_B = BMP085calculatePressure(BMPreadUP(I2C2));
				
				/* Calculating temperature value */
				BMP085_temp_B = BMP085_temp_B * 0.1;
				
				/* Overwrite */ 
				sprintf(stringPrint, "       ");
				TM_ILI9341_Puts(100, 25, stringPrint, &TM_Font_16x26, ILI9341_COLOR_BLUE2, ILI9341_COLOR_BLUE2);
				
				/* Title */
				sprintf(stringPrint, "BOARD");
				TM_ILI9341_Puts(110, 25, stringPrint, &TM_Font_16x26, ILI9341_COLOR_RED, ILI9341_COLOR_CYAN);
				
				/* Displaying message on LCD */
				sprintf(stringPrint, "Temperature: %.1f", BMP085_temp_B);
				TM_ILI9341_Puts(5, 70, stringPrint, &TM_Font_16x26, ILI9341_COLOR_RED, ILI9341_COLOR_CYAN);
				sprintf(stringPrint, "Pressure: %ld", BMP085_press_B);
				TM_ILI9341_Puts(5, 100, stringPrint, &TM_Font_16x26, ILI9341_COLOR_RED, ILI9341_COLOR_CYAN);
				
				/* Send Data VIA UART4 Board*/
				sprintf(stringPrint, "TEB%f", BMP085_temp_B);
				TM_USART_Puts(UART4, stringPrint);
				sprintf(stringPrint, "PRB%ld", BMP085_press_B);
				TM_USART_Puts(UART4, stringPrint);
			}
		}
		/* 900000 ms = 15min */
		if(TM_DELAY_Time() >= 900000)
		{
				/* Resetting timer */
				TM_DELAY_SetTime(0);
				/* BMP calibration function */
				BMP085Calibration(I2C1);
				
				/* Fetching temperature value */
				BMP085_temp_K = BMP085calculateTemperature(BMP085readUT(I2C1));
				/* Fetching pressure value */
				BMP085_press_K = BMP085calculatePressure(BMPreadUP(I2C1));
				
				/* Send Data VIA UART4, Kuhinja*/
				sprintf(stringPrint, "TEK%f", BMP085_temp_K);
				TM_USART_Puts(UART4, stringPrint);
				sprintf(stringPrint, "PRK%ld", BMP085_press_K);
				TM_USART_Puts(UART4, stringPrint);
				//////////////////////////////////////////////////////////////////////////////
				/* BMP calibration function */
				BMP085Calibration(I2C2);
				
				/* Fetching temperature value */
				BMP085_temp_B = BMP085calculateTemperature(BMP085readUT(I2C2));
				/* Fetching pressure value */
				BMP085_press_B = BMP085calculatePressure(BMPreadUP(I2C2));
				
				/* Send Data VIA UART4 Board*/
				sprintf(stringPrint, "TEB%f", BMP085_temp_B);
				TM_USART_Puts(UART4, stringPrint);
				sprintf(stringPrint, "PRB%ld", BMP085_press_B);
				TM_USART_Puts(UART4, stringPrint);

			}
		
		/* Checking if condition is TRUE -> PIR has seen activity 
			H---> HODNIK																						*/
		if((PCF8575_H = PCF8575_read(I2C1)) == 0x8000)
		{
				
				sprintf(stringPrint, "Movement Detected, PIR1");
				TM_ILI9341_Puts(5, 145, stringPrint, &TM_Font_11x18, ILI9341_COLOR_RED, ILI9341_COLOR_CYAN);
				
				/* UART */
				sprintf(stringPrint, "PI1");
				TM_USART_Puts(UART4, stringPrint);
			
		}
		else
		{
				/* Overwrite */
				sprintf(stringPrint, "                       ");
				TM_ILI9341_Puts(5, 145, stringPrint, &TM_Font_11x18, ILI9341_COLOR_BLUE2, ILI9341_COLOR_BLUE2);
		}

		/* Checking if condition is TRUE -> PIR has seen activity 
			B---> BORAVAK																						*/
		if((PCF8575_B = PCF8575_read(I2C2)) == 0x8000)
		{
			
				sprintf(stringPrint, "Movement Detected, PIR2");
				TM_ILI9341_Puts(5, 165, stringPrint, &TM_Font_11x18, ILI9341_COLOR_RED, ILI9341_COLOR_CYAN);
			
				/* UART */
				sprintf(stringPrint, "PI2");
				TM_USART_Puts(UART4, stringPrint);
			
		}
		else
		{
				/* Overwrite */
				sprintf(stringPrint, "                       ");
				TM_ILI9341_Puts(5, 165, stringPrint, &TM_Font_11x18, ILI9341_COLOR_BLUE2, ILI9341_COLOR_BLUE2);
		}

		
  }
}
