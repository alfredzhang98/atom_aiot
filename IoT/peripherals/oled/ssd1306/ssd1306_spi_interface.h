#ifndef __SSD1306_SPI_INTERFACE__H
#define __SSD1306_SPI_INTERFACE__H			  	 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
//TODO: the board libs

void oledSpiInit(void);
void oledWriteByte(unsigned char dat,unsigned char cmd);
void oledSpiReset(void);
void delay_us(unsigned char num);
void delay_ms(unsigned int ms);

#endif