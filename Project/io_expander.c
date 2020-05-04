#include "io_expander.h"




void io_expander_write_reg(uint8_t reg, uint8_t data){
  while (I2CMasterBusy(I2C1_BASE)){}
	i2cSetSlaveAddr(I2C1_BASE, MCP23017_DEV_ID, I2C_WRITE);
	i2cSendByte(I2C1_BASE, reg, I2C_MCS_START | I2C_MCS_RUN);
	i2cSendByte(I2C1_BASE, data, I2C_MCS_RUN | I2C_MCS_STOP);
}

uint8_t io_expander_read_reg(uint8_t reg){
  uint8_t data_read;
	i2c_status_t i2c_status;
	
	// Wait if master is busy
	while(I2CMasterBusy(I2C1_BASE)){};	    
		
	i2c_status = i2cSetSlaveAddr(I2C1_BASE, MCP23017_DEV_ID, I2C_WRITE); 
	if (i2c_status != I2C_OK )
		return i2c_status;
	
	i2c_status = i2cSendByte(I2C1_BASE, reg, I2C_MCS_START | I2C_MCS_RUN);
	if ( i2c_status != I2C_OK )
		return i2c_status;
	
	i2c_status = i2cSetSlaveAddr(I2C1_BASE, MCP23017_DEV_ID, I2C_READ);
	if ( i2c_status != I2C_OK )
		return i2c_status;
	
	i2c_status = i2cGetByte(I2C1_BASE, &data_read, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
	if ( i2c_status != I2C_OK )
		return i2c_status;
	
	return data_read;
}


bool io_expander_init(void){
	//==============================
  // Configure I2C GPIO Pins
  //==============================  
  if(gpio_enable_port(IO_EXPANDER_GPIO_BASE) == false)
    return false;
  if(gpio_enable_port(GPIOF_BASE) == false)
		return false;
	
  // Configure SCL 
  if(gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN)== false)
    return false;   
  if(gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN)== false)
    return false;
  if(gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PCTL_M, IO_EXPANDER_I2C_SCL_PIN_PCTL)== false)
    return false;
   
  // Configure SDA 
  if(gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
    return false;
  if(gpio_config_open_drain(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
    return false;
  if(gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
    return false;
  if(gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PCTL_M, IO_EXPANDER_I2C_SDA_PIN_PCTL)== false)
    return false;
  
  //  Initialize the TIVA board to be master
  if( initializeI2CMaster(IO_EXPANDER_I2C_BASE)!= I2C_OK)
    return false;
	
	// Config ALL 8 pins on portA to be outputs
	io_expander_write_reg(MCP23017_IODIRA_R, 0x00);
  
	// Config ALL 8 pins on portB to be inputs
	io_expander_write_reg(MCP23017_IODIRB_R, 0xFF);
	
	// Compare button with previous value
	io_expander_write_reg(MCP23017_IOCONB_R, 0);
	
	// Setup INTB to interrupt on input change
	io_expander_write_reg(MCP23017_GPINTENB_R, 0xF);
	
	// Pull-up resistor to fix krachy's mistake
	io_expander_write_reg(MCP23017_GPPUB_R, 0x0F);
	
	gpio_config_digital_enable(IO_EXPANDER_IRQ_GPIO_BASE, IO_EXPANDER_IRQ_PIN_NUM);
  gpio_config_enable_pullup(IO_EXPANDER_IRQ_GPIO_BASE, IO_EXPANDER_IRQ_PIN_NUM);
	
	// Enable edge triggering interrupts
	if(!gpio_config_falling_edge_irq(GPIOF_BASE, PF0)) 
		return false;
	NVIC_SetPriority(GPIOF_IRQn, 1);
	NVIC_EnableIRQ(GPIOF_IRQn);
	
	// Read cap register in every initialization to clear interrupt
	io_expander_read_reg(MCP23017_INTCAPB_R);


  
  return true;
}