#ifndef __SSD1306_SPI_INTERFACE__H
#define __SSD1306_SPI_INTERFACE__H			  	 



void oledSpiInit(void);
void oledWriteByte(unsigned char dat,unsigned char cmd);
void oledSpiReset(void);
void delay_us(unsigned char num);
void delay_ms(unsigned int ms);

#endif