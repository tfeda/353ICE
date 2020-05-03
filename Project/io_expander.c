#include "io_expander.h"


// ADD CODE
#define IODIRA    0x00
#define IODIRB    0x01
#define IO_EXPANDER_GPIOA   0x12
#define IO_EXPANDER_GPIOB   0x13

#define FIXME   0x00

//*****************************************************************************
// Initializes both the A and B ports of the IO expander to be outputs.
// The IODIRA and IODIRB registers need to be set to 0x00.
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//*****************************************************************************
bool io_expander_init(void)
{
  
  //==============================================================
  // Set Slave Address of the MCP23017 
  //==============================================================  
  i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE, MCP23017_DEV_ID, I2C_WRITE);
  
  //==============================================================
  // Set the Direction of IODIRA to be outputs
  // Write 0x00 to IODIRA
  //==============================================================
  // Send the IODIRA Address
  i2cSendByte( IO_EXPANDER_I2C_BASE, IODIRA, I2C_MCS_START | I2C_MCS_RUN );

  // Set PortA to be outputs
  i2cSendByte( IO_EXPANDER_I2C_BASE, IODIRA,  I2C_MCS_RUN | I2C_MCS_STOP );

  //==============================================================
  // Set the Direction of IODIRB to be outputs
  // Write 0x00 to IODIRB
  //==============================================================
  // Send the IODIRB Address
  i2cSendByte( IO_EXPANDER_I2C_BASE, IODIRB, I2C_MCS_START | I2C_MCS_RUN );

  // Set PortB to be outputs
  i2cSendByte( IO_EXPANDER_I2C_BASE, IODIRB, I2C_MCS_RUN | I2C_MCS_STOP );
	
	return true;
}

void io_expander_write_reg(uint8_t reg, uint8_t data){

}

uint8_t io_expander_read_reg(uint8_t data){

}
