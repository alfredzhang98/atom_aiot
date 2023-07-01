#ifndef __OLED_SSD1306__H
#define __OLED_SSD1306__H	

#include <ssd1306_spi_interface.h>

#define SIZE 		    16	
#define Max_Column	128		
#define Max_Row		  64		
#define X_WIDTH 	  128		
#define Y_WIDTH 	  64


//function
extern void oledSSD1603Setup(void);

extern void oledClear(void);
extern void oledClearLines(unsigned char lineStart, unsigned char lineEnd);

extern void oledScroll(void);

extern void oledColorTurn(bool status);
extern void oledDisplayTurn(bool status);
extern void oledDisplayStatus(bool status);

extern void oledSetPostion(unsigned char x, unsigned char y);

//draw
extern void oledTest(void);
extern void oledShowChar(unsigned char x, unsigned char y, const unsigned char chr, unsigned char sizey);
extern void oledShowNum(unsigned char x, unsigned char y, unsigned int num, unsigned char len, unsigned char sizey);
extern void oledShowString(unsigned char x,unsigned char y,const char *chr,unsigned char sizey);
extern void oledDrawDiagram(unsigned char x, unsigned char y, unsigned char sizex, unsigned char sizey, const unsigned char BMP[]);

#endif