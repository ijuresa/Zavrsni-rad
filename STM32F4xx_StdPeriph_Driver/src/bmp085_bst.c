

/*  $Date: 2009/10/23 $
  *  $Revision: 1.2 $
  */

#include "bmp085_bst.h"
 
 
 bmp085_t *p_bmp085 = 0; 
 int bmp085_init( bmp085_t *bmp085 ) 
 {
   char comres = 0;
   unsigned char data;
   long dummy;
 
 
   p_bmp085 = bmp085; /* assign BMP085 ptr */
   p_bmp085->sensortype = E_SENSOR_NOT_DETECTED;
   p_bmp085->dev_addr = BMP085_I2C_ADDR; /* preset BMP085 I2C_addr */
   comres += p_bmp085->BMP085_BUS_READ_FUNC(p_bmp085->dev_addr, BMP085_CHIP_ID__REG, &data, 1);  /* read Chip Id */
   
   p_bmp085->chip_id = BMP085_GET_BITSLICE(data, BMP085_CHIP_ID);  
   p_bmp085->number_of_samples = 1;  
   p_bmp085->oversampling_setting=0;
 
   if( p_bmp085->chip_id == BMP085_CHIP_ID )
   {            /* get bitslice */
     p_bmp085->sensortype = BOSCH_PRESSURE_BMP085;
     
     comres += p_bmp085->BMP085_BUS_READ_FUNC(p_bmp085->dev_addr, BMP085_VERSION_REG, &data, 1); /* read Version reg */
     
     p_bmp085->ml_version = BMP085_GET_BITSLICE(data, BMP085_ML_VERSION);        /* get ML Version */
     p_bmp085->al_version = BMP085_GET_BITSLICE(data, BMP085_AL_VERSION);        /* get AL Version */
     bmp085_get_cal_param( ); /* readout bmp085 calibparam structure */
   }
   else
   {
     p_bmp085->sensortype = BOSCH_PRESSURE_SMD500;
     p_bmp085->smd500_t_resolution = SMD500_T_RESOLUTION_16BIT;
     p_bmp085->smd500_masterclock = SMD500_MASTERCLOCK_32768HZ;
     smd500_get_cal_param();
 
     /* calculate B1*/
     dummy =  (p_bmp085->cal_param.ac3 + SMD500_PARAM_M3);
     dummy *= SMD500_PARAM_ME;
     dummy >>= 11;
     dummy += SMD500_PARAM_MF;   
     p_bmp085->cal_param.b1 = dummy;
     
     /* calculate B2*/
     dummy = (p_bmp085->cal_param.ac2 + SMD500_PARAM_M2);
     dummy *= p_bmp085->cal_param.b1;
     dummy >>= 4;
     dummy /= (p_bmp085->cal_param.ac3 + SMD500_PARAM_M3);   // calculate B2      
     p_bmp085->cal_param.b2 = dummy;
 
     p_bmp085->sensortype = BOSCH_PRESSURE_SMD500;
     
     p_bmp085->cal_param.ac1 += SMD500_PARAM_M1;
     p_bmp085->cal_param.ac1 <<= 1;
 
     p_bmp085->cal_param.ac2 += SMD500_PARAM_M2;
     p_bmp085->cal_param.ac2 <<= 2;
 
     p_bmp085->cal_param.ac3 += SMD500_PARAM_M3;
     p_bmp085->cal_param.ac3 <<= 3;
 
     p_bmp085->cal_param.ac4 += SMD500_PARAM_M4;
     p_bmp085->cal_param.ac4 <<= 1;
 
     p_bmp085->cal_param.ac5 += SMD500_PARAM_M5;
     p_bmp085->cal_param.ac5 <<= 3;
 
     p_bmp085->cal_param.ac6 += SMD500_PARAM_M6;
     p_bmp085->cal_param.ac6 <<= 3;
     
     p_bmp085->cal_param.mb = SMD500_PARAM_MB;
     p_bmp085->cal_param.mc = SMD500_PARAM_MC;
     p_bmp085->cal_param.md = SMD500_PARAM_MD;   
   }
 
   return comres;
 }
 
 //int bmp085_read_cal_param(void)
 int bmp085_get_cal_param( void )
 {
   int comres;
   unsigned char data[22];
   comres = p_bmp085->BMP085_BUS_READ_FUNC( p_bmp085->dev_addr, BMP085_PROM_START__ADDR, data, BMP085_PROM_DATA__LEN );
   
   /*parameters AC1-AC6*/
   p_bmp085->cal_param.ac1 =  (data[0] <<8) | data[1];
   p_bmp085->cal_param.ac2 =  (data[2] <<8) | data[3];
   p_bmp085->cal_param.ac3 =  (data[4] <<8) | data[5];
   p_bmp085->cal_param.ac4 =  (data[6] <<8) | data[7];
   p_bmp085->cal_param.ac5 =  (data[8] <<8) | data[9];
   p_bmp085->cal_param.ac6 = (data[10] <<8) | data[11];
   
   /*parameters B1,B2*/
   p_bmp085->cal_param.b1 =  (data[12] <<8) | data[13];
   p_bmp085->cal_param.b2 =  (data[14] <<8) | data[15];
   
   /*parameters MB,MC,MD*/
   p_bmp085->cal_param.mb =  (data[16] <<8) | data[17];
   p_bmp085->cal_param.mc =  (data[18] <<8) | data[19];
   p_bmp085->cal_param.md =  (data[20] <<8) | data[21];
   
   return comres;  
 }
 
 //int smd500_read_cal_param(void) 
 int smd500_get_cal_param( void ) 
 {
   unsigned char data[SMD500_PROM_DATA__LEN];    
   int comres;
   comres = p_bmp085->BMP085_BUS_READ_FUNC( p_bmp085->dev_addr, SMD500_PROM_START__ADDR, data, SMD500_PROM_DATA__LEN );
 
   p_bmp085->cal_param.ac1 = (unsigned short) ( (data[0] <<8) | data[1] ) & 0x3FFF;
   p_bmp085->cal_param.ac2 = ( (data[0] & 0xC0) >> 6 ) + ( (data[2] & ~0x01) << 1 );
   p_bmp085->cal_param.ac3 = ( (data[2] & 0x01) << 8 ) + data[3];
   p_bmp085->cal_param.ac4 = ( (data[4] & 0x1f) << 8 ) + data[5];
   p_bmp085->cal_param.ac5 = ( (data[4] & ~0x1f) >> 5 ) + ( (data[6] & 0xf0) >> 1 );
   p_bmp085->cal_param.ac6 = ( (data[6] & 0x0f) << 8 ) + data[7];
   return comres;
 }
 
 
 //short bmp085_calc_temperature(unsigned long ut) 
 short bmp085_get_temperature( unsigned long ut ) 
 {
   short temperature;
   long x1,x2,x3,x4,y2,y3,y4;    
 
   if( p_bmp085->sensortype == BOSCH_PRESSURE_BMP085 )
   {
     x1 = (((long) ut - (long) p_bmp085->cal_param.ac6) * (long) p_bmp085->cal_param.ac5) >> 15;
     x2 = ((long) p_bmp085->cal_param.mc << 11) / (x1 + p_bmp085->cal_param.md);
     p_bmp085->param_b5 = x1 + x2;
   }
   else // SMD500
   {
     if( p_bmp085->smd500_t_resolution == SMD500_T_RESOLUTION_16BIT ) // check for SMD500 temp resolution mode
     {
       x1 = ((unsigned long) ((( ut * SMD500_PARAM_MJ) >> 16) - p_bmp085->cal_param.ac6));
     }
     else //13BIT
     {
       x1 = ut * 8  - p_bmp085->cal_param.ac6;
     }
 
     x2 = (x1 * x1) >> 13;
     y2 = (SMD500_PARAM_MB * x2) >> 13;
     x3 = (x2 * x1) >> 16;
     y3 = (SMD500_PARAM_MC * x3) >> 12;
     x4 = (x2 * x2) >> 16;
     y4 = (SMD500_PARAM_MD * x4) >> 14;
 
     p_bmp085->param_b5 = (((p_bmp085->cal_param.ac5) * ( (2*x1) + y2 + y3 + y4))) >> 13; // temperature in (1/160)?C
   }
   temperature = ((p_bmp085->param_b5 + 8) >> 4);  // temperature in 0.1?C
 
   return temperature;
 }
 
 //long bmp085_calc_pressure(unsigned long up)
 long bmp085_get_pressure( unsigned long up )
 {
    long pressure,x1,x2,x3,b3,b6;
    unsigned long b4, b7;
    
    b6 = p_bmp085->param_b5 - 4000;
    //*****calculate B3************
    x1 = (b6*b6) >> 12;           
    x1 *= p_bmp085->cal_param.b2;
    x1 >>= 11;
 
    x2 = (p_bmp085->cal_param.ac2*b6);
    x2 >>= 11;
 
    x3 = x1 +x2;
 
    b3 = (((((long)p_bmp085->cal_param.ac1 )*4 + x3) <<p_bmp085->oversampling_setting) + 2) >> 2;
 
    //*****calculate B4************
    x1 = (p_bmp085->cal_param.ac3* b6) >> 13;
    x2 = (p_bmp085->cal_param.b1 * ((b6*b6) >> 12) ) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (p_bmp085->cal_param.ac4 * (unsigned long) (x3 + 32768)) >> 15;
      
    b7 = ((unsigned long)(up - b3) * (50000>>p_bmp085->oversampling_setting));   
    if( b7 < 0x80000000 )
    {
      pressure = (b7 << 1) / b4;
    }
    else
    { 
      pressure = (b7 / b4) << 1;
    }
    
    x1 = pressure >> 8;
    x1 *= x1;
    x1 = (x1 * SMD500_PARAM_MG) >> 16;
    x2 = (pressure * SMD500_PARAM_MH) >> 16;
    pressure += (x1 + x2 + SMD500_PARAM_MI) >> 4;        // pressure in Pa  
 
    return pressure;
 }
 
 
 //unsigned short bmp085_read_ut ()
 unsigned short bmp085_get_ut( void )
 {
   unsigned short ut;
   unsigned char data[2];    
   unsigned char ctrl_reg_data;
   int wait_time;
   int comres;
 
   if( p_bmp085->chip_id == BMP085_CHIP_ID ) /* get bitslice */
   { 
     ctrl_reg_data = BMP085_T_MEASURE;
     wait_time = BMP085_TEMP_CONVERSION_TIME;
   }
   else
   {
     ctrl_reg_data = SMD500_T_MEASURE + p_bmp085->smd500_t_resolution + p_bmp085->smd500_masterclock;
   }
 
   // wait_time can be 9 ms for 13 bit smd500_t_resolution
   if( p_bmp085->smd500_t_resolution == SMD500_T_RESOLUTION_13BIT )
   { 
      wait_time = SMD500_TEMP_CONVERSION_TIME_13;
   }
   else
   {
      wait_time = SMD500_TEMP_CONVERSION_TIME_16;
   }
   comres = p_bmp085->BMP085_BUS_WRITE_FUNC(p_bmp085->dev_addr, BMP085_CTRL_MEAS_REG, &ctrl_reg_data, 1);
   
   
   p_bmp085->delay_msec (wait_time);  
   comres += p_bmp085->BMP085_BUS_READ_FUNC(p_bmp085->dev_addr, BMP085_ADC_OUT_MSB_REG, data, 2);
   ut = (data[0] <<8) | data[1];
   
   return ut;
 }
 
 
 //unsigned long bmp085_read_up ()
 unsigned long bmp085_get_up( void )
 {
   int i;
   unsigned long up = 0;
   unsigned char data[3];    
   unsigned char ctrl_reg_data;
   int comres = 0;
   if( p_bmp085->chip_id == BMP085_CHIP_ID )
   { 
     ctrl_reg_data = BMP085_P_MEASURE + (p_bmp085->oversampling_setting << 6);
     comres = p_bmp085->BMP085_BUS_WRITE_FUNC( p_bmp085->dev_addr, BMP085_CTRL_MEAS_REG, &ctrl_reg_data, 1 );
     
     p_bmp085->delay_msec ( 2 + (3 << (p_bmp085->oversampling_setting) ) );
     comres += p_bmp085->BMP085_BUS_READ_FUNC( p_bmp085->dev_addr, BMP085_ADC_OUT_MSB_REG, data, 3 );
     
     up = (((unsigned long) data[0] << 16) | ((unsigned long) data[1] << 8) | (unsigned long) data[2]) >> (8-p_bmp085->oversampling_setting);
     p_bmp085->number_of_samples = 1;
   }
   else
   {
     // SMD500 
     ctrl_reg_data = SMD500_P_MEASURE + p_bmp085->smd500_masterclock;
     p_bmp085->number_of_samples = (1 << (p_bmp085->oversampling_setting));
     for( i = 0; i < p_bmp085->number_of_samples; i++ )
     {
       comres += p_bmp085->BMP085_BUS_WRITE_FUNC( p_bmp085->dev_addr, BMP085_CTRL_MEAS_REG, &ctrl_reg_data, 1 );
       p_bmp085->delay_msec (34);
       
       comres += p_bmp085->BMP085_BUS_READ_FUNC( p_bmp085->dev_addr, BMP085_ADC_OUT_MSB_REG, data, 2 );
       up += (((unsigned long)data[0] <<8) | (unsigned long)data[1]);
     }    
   }
   return up;
 }
 
 
