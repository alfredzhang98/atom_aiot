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

// TODO: SPI GPIO SETTING

void delay_ms(unsigned int ms)
{
  // TODO: delay ms function in your board
}

void writeByteCMD(unsigned char dat)
{
  // TODO: spi write bytes function in your board
}

void writeByteDATA(unsigned char dat)
{
  // TODO: spi write bytes function in your board
}

void oledSpiInit(void)
{
  // TODO: SPI initial

  // TODO: GPIO initial
}

void oledSpiReset(void)
{
  // TODO: Set the reset gpio level (high(100ms)-low(200ms)-high(200ms))
  // High

  delay_ms(100);
  // low

  delay_ms(100);
  // high
}