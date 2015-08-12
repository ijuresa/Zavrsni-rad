 
 
 #ifndef __PCF8575_H__
 #define __PCF8575_H__
 #include "tm_stm32f4_i2c.h"
 
 /* Functions */
 int PCF8575_read(I2C_TypeDef* I2Cx);
 void PCF8575_write(I2C_TypeDef* I2Cx, int data);
 
 
  
 #endif   // __PCF8575_H__
 
