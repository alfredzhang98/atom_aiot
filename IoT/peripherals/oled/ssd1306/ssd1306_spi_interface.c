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

void delay_us(unsigned char num);
void delay_ms(unsigned int ms);

void oledSpiInit(void){
    //TODO: SPI initial


}

void oledWriteByte(unsigned char dat,unsigned char cmd){
    //TODO: spi write bytes function in your board

}

void oledSpiReset(void)
{
	//TODO: Set the reset gpio level (high(100ms)-low(200ms)-high(200ms))
  
  delay_ms(100);

  delay_ms(200);

  delay_ms(200);
  
}


void delay_us(unsigned char num)
{
	//TODO: delay us function in your board

}

void delay_ms(unsigned int ms)
{
	//TODO: delay ms function in your board

}