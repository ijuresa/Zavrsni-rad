
/* Includes */
#include "pcf8575.h"
#include "tm_stm32f4_i2c.h"

/**************************************************************************************
	*		A0,A1 and A2 are 0 (not soldered)	
	*		[0] [1] [0] [0] [A2] [A1] [A0] [WRITE/READ]
	*		Write = 0; Read = 1
	***************************************************************************************/
/* Read and write address */
#define readAddress 0x41		
#define writeAddress 0x40

//------------------------------------------------------------------------------
//    Name:        PCF8575_read
//    Description: Reading 2 byte from PCF8575
//    Input:       I2Cx which we want to use
//    Output:      16bit variable
//    Misc:		  	 -
//------------------------------------------------------------------------------
int PCF8575_read(I2C_TypeDef* I2Cx)
{
	uint8_t input[2];
	
	TM_I2C_ReadMultiNoRegister(I2Cx, readAddress, input, 2);

	return ((input[1] << 8) | input[0]);
}

//------------------------------------------------------------------------------
//    Name:        PCF8575_write
//    Description: Writing 2 byte to PCF8575, setting inputs and outputs
//    Input:       I2Cx which we want to use and data we want to send
//    Output:      -
//    Misc:		  	 -
//------------------------------------------------------------------------------
void PCF8575_write(I2C_TypeDef* I2Cx, int data)
{
	uint8_t output[2];
	output[0] = data;
	output[1] = data >> 8;
	
	TM_I2C_WriteMultiNoRegister(I2Cx, writeAddress, output, 2);
}
