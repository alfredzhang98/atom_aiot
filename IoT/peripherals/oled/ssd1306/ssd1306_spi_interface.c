/********************************************************
  * @file        	ssd1306_spi_interface.c
  * @author      	Alfred
  * @version     	V1.0.0
  * @date        	06/18/2022
  * @brief       	this is for the user to implement their own master side code
  *******************************************************
  *For 7-pin ssd1306 oled
  *GND						          ------>GND
  *VCC						          ------>3.3V - 5V
  *clock in 				        ------>D0
  *data in and response out	------>D1
  *reset					          ------>RES
  *data or command			    ------>DC
  *chip select				      ------>CS
********************************************************/

#include <ssd1306_spi_interface.h>

void delay_ms(unsigned int ms)
{
	//TODO: delay ms function in your board

}

void writeByteCMD(unsigned char dat){
    //TODO: spi write bytes function in your board

}

void writeByteDATA(unsigned char dat){
    //TODO: spi write bytes function in your board

}

void oledSpiReset(void)
{
	//TODO: Set the reset gpio level (high(100ms)-low(200ms)-high(200ms))
  //set gpio low
  
  delay_ms(100);
  //set gpio high

  delay_ms(100);
}


void oledSpiInit(void){
    //TODO: SPI initial

}
