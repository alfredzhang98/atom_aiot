#ifndef _OLED_H_
#define _OLED_H_

#include "oled_port.h" 

void oled_setup();
void oled_loop();
void OLED_Clear(void);
void OLED_ShowString(unsigned char x,unsigned char y,const char *chr,unsigned char sizey);
void OLED_DrawBMP(unsigned char x,unsigned char y,unsigned char sizex, unsigned char sizey,const unsigned char BMP[]);

#endif 
