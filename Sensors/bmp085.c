
#include "bmp085_bst.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_i2c.h"
#include "tm_stm32f4_timer_properties.h"
#include "defines.h"

#define readAddress 0xEF		
#define writeAddress 0xEE
 // Calibration values
short ac1;
short ac2; 
short ac3; 
unsigned short ac4;
unsigned short ac5;
unsigned short ac6;
short b1; 
short b2;
short mb;
short mc;
short md;
short ut;

long b5; 

/* Oversampling setting */
volatile uint8_t OSS = 0;
 
int BMP085readCalibration(uint8_t address)
{
	uint8_t msb, lsb;
	msb = TM_I2C_Read(I2C1, writeAddress, address);
	lsb = TM_I2C_Read(I2C1, writeAddress, address + 1);
	
	return (int) msb << 8 | lsb;
}

void BMP085Calibration()
{
	ac1 = BMP085readCalibration(0xAA);
  ac2 = BMP085readCalibration(0xAC);
  ac3 = BMP085readCalibration(0xAE);
  ac4 = BMP085readCalibration(0xB0);
  ac5 = BMP085readCalibration(0xB2);
  ac6 = BMP085readCalibration(0xB4);
  b1 = BMP085readCalibration(0xB6);
  b2 = BMP085readCalibration(0xB8);
  mb = BMP085readCalibration(0xBA);
  mc = BMP085readCalibration(0xBC);
  md = BMP085readCalibration(0xBE);
}

int BMP085readUT()
{
	
	TM_I2C_Write(I2C1, writeAddress, 0xF4, 0x2E);
	Delayms(5);
  
	
	ut = BMP085readCalibration(0xF6);
	
	return ut;
}

int BMPreadUP()
{
	uint8_t msb, lsb, xlsb;
	long up = 0;
	
	TM_I2C_Write(I2C1, writeAddress, 0xF4, 0x34 + (OSS << 6));
	Delayms(5);
	
	msb = TM_I2C_Read(I2C1, writeAddress, 0xF6);
	lsb = TM_I2C_Read(I2C1, writeAddress, 0xF7);
	xlsb = TM_I2C_Read(I2C1, writeAddress, 0xF8);
	
	up = (((msb << 16) | (lsb << 8) | xlsb) >> (8 - OSS));
	return up;
}


int BMP085calculateTemperature(long ut)
{
	long x1, x2;
  
  x1 = ((ut - ac6)*ac5) >> 15;
  x2 = (mc << 11)/(x1 + md);
  b5 = x1 + x2;

  return ((b5 + 8) >> 4); 
}

int BMP085calculatePressure(long up)
{
	long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;
  
  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;
  
  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;
  
  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;
    
  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;
  
  return p;

}

