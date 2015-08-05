
 
 #ifndef __BMP085_H__
 #define __BMP085_H__
 
 /* Functions */
 void BMP085Calibration(void);
 int BMP085calculateTemperature(long ut);
 int BMP085readUT(void);
 int BMPreadUP(void);
 int BMP085calculatePressure(long up);
 
 #endif   // __BMP085_H__
 