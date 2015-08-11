
 
 #ifndef __BMP085_H__
 #define __BMP085_H__
 
 #include "tm_stm32f4_i2c.h"
 /* Functions */
 void BMP085Calibration(I2C_TypeDef* I2Cx);
 int BMP085calculateTemperature(long ut);
 int BMP085readUT(I2C_TypeDef* I2Cx);
 int BMPreadUP(I2C_TypeDef* I2Cx);
 int BMP085calculatePressure(long up);
 
 #endif   // __BMP085_H__

