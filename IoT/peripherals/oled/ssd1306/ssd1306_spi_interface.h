#ifndef __SSD1306_SPI_INTERFACE__H
#define __SSD1306_SPI_INTERFACE__H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// TODO: the board libs

#define OLED_CMD 0
#define OLED_DATA 1

void oledSpiInit(void);
void writeByteCMD(unsigned char dat);
void writeByteDATA(unsigned char dat);
void oledSpiReset(void);
void delay_us(unsigned char num);
void delay_ms(unsigned int ms);

#endif