#ifndef __OLED_SSD1306__H
#define __OLED_SSD1306__H	

#include <oled_font_picture.h>
#include <ssd1306_spi_interface.h>

#define OLED_CMD  0
#define OLED_DATA 1	

#define SIZE 		    16	
#define Max_Column	128		
#define Max_Row		  64		
#define X_WIDTH 	  128		
#define Y_WIDTH 	  64

//function
void oledSSD1603Setup(void);

void oledClear(void);

void oledScroll(void);

void oledColorTurn(bool status);
void oledDisplayTurn(bool status);
void oledDisplayStatus(bool status);

void oledSetPostion(unsigned char x, unsigned char y);

//draw
void oledLoopTest(void);
void oledShowChar(unsigned char x, unsigned char y, const unsigned char chr, unsigned char sizey);
void oledShowNum(unsigned char x, unsigned char y, unsigned int num, unsigned char len, unsigned char sizey);
void oledShowString(unsigned char x,unsigned char y,const char *chr,unsigned char sizey);
void oledDrawDiagram(unsigned char x, unsigned char y, unsigned char sizex, unsigned char sizey, const unsigned char BMP[]);

#endif