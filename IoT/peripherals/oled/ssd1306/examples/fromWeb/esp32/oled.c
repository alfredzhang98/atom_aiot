//******************************************************************************/

#include "oledfont.h" 
#include "oled.h" 

/*definition--------------------------------------------*/
#define OLED_CMD  				0	//写命令
#define OLED_DATA 				1	//写数据

#define SIZE 					16		//显示字符的大小
#define Max_Column				128		//最大列数
#define Max_Row					64		//最大行数
#define X_WIDTH 				128		//X轴的宽度
#define Y_WIDTH 				64	    //Y轴的宽度		

//发送一个字节
//向SSD1306写入一个字节。
//mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(unsigned char dat,unsigned char mode)
{
  if(mode){
    OLED_Write_SPI_Data(dat);
  }
  else{
    OLED_Write_SPI_Command(dat);
  }
}

//反显函数
void OLED_ColorTurn(unsigned char i)
{
  if(!i) 
  	OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
  else
  	OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
}

//屏幕旋转180度
void OLED_DisplayTurn(unsigned char i)
{
  if(i==0)
  {
      OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
      OLED_WR_Byte(0xA1,OLED_CMD);
  }
  else
  {
    OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
    OLED_WR_Byte(0xA0,OLED_CMD);
  }
}

//坐标设置
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
  OLED_WR_Byte(0xb0+y,OLED_CMD);
  OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
  OLED_WR_Byte((x&0x0f),OLED_CMD);
}
       
//开启OLED显示    
void OLED_Display_On(void)
{
  OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
  OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
  OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//关闭OLED显示     
void OLED_Display_Off(void)
{
  OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
  OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
  OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}
            
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!   
void OLED_Clear(void)  
{  
  unsigned char i,n;       
  for(i=0;i<8;i++)  
  {  
    OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
    OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
    OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
    for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
  } //更新显示
}

//在指定位置显示一个字符
//x:0~127
//y:0~63         
//sizey:选择字体 6x8  8x16
void OLED_ShowChar(unsigned char x,unsigned char y,const unsigned char chr,unsigned char sizey)
{       
  unsigned char c=0,sizex=sizey/2,temp;
  unsigned short i=0,size1;
  if(sizey==8)size1=6;
  else size1=(sizey/8+((sizey%8)?1:0))*(sizey/2);
  c=chr-' ';//得到偏移后的值
  OLED_Set_Pos(x,y);
  for(i=0;i<size1;i++)
  {
    if(i%sizex==0&&sizey!=8) OLED_Set_Pos(x,y++);
    if(sizey==8)
    {
      temp=asc2_0806[c][i];
      OLED_WR_Byte(temp,OLED_DATA);//6X8字号
    }
    else if(sizey==16) 
    {
      temp=asc2_1608[c][i];
      OLED_WR_Byte(temp,OLED_DATA);//8x16字号
    }
    else return;
  }
}

//m^n函数
unsigned int oled_pow(unsigned char m,unsigned char n)
{
  unsigned int result=1;  
  while(n--)result*=m;    
  return result;
}
         
//显示数字
//x,y :起点坐标
//num:要显示的数字
//len :数字的位数
//sizey:字体大小      
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char sizey)
{           
  unsigned char t,temp,m=0;
  unsigned char enshow=0;
  if(sizey==8)m=2;
  for(t=0;t<len;t++)
  {
    temp=(num/oled_pow(10,len-t-1))%10;
    if(enshow==0&&t<(len-1))
    {
      if(temp==0)
      {
        OLED_ShowChar(x+(sizey/2+m)*t,y,' ',sizey);
        continue;
      }else enshow=1;
    }
    OLED_ShowChar(x+(sizey/2+m)*t,y,temp+'0',sizey);
  }
}

//显示一个字符号串
void OLED_ShowString(unsigned char x,unsigned char y,const char *chr,unsigned char sizey)
{
  unsigned char j=0;
  while (chr[j]!='\0')
  {   
    OLED_ShowChar(x,y,chr[j++],sizey);
    if(sizey==8)x+=6;
    else x+=sizey/2;
  }
}

//显示汉字
void OLED_ShowChinese(unsigned char x,unsigned char y,const unsigned char no,unsigned char sizey)
{
  unsigned short i,size1=(sizey/8+((sizey%8)?1:0))*sizey;
  unsigned char temp;
  for(i=0;i<size1;i++)
  {
    if(i%sizey==0) 
		OLED_Set_Pos(x,y++);
    if(sizey==16) 
    {
      temp=Hzk[no][i];
      OLED_WR_Byte(temp,OLED_DATA);//16x16字号
    }
    //else if(sizey==xx) OLED_WR_Byte(xxx[c][i],OLED_DATA);//用户添加字号
    else return;
  }       
}


//显示图片
//x,y显示坐标
//sizex,sizey,图片长宽
//BMP：要显示的图片
void OLED_DrawBMP(unsigned char x,unsigned char y,unsigned char sizex, unsigned char sizey,const unsigned char BMP[])
{   
  unsigned short j=0;
  unsigned char i,m,temp;
  sizey=sizey/8+((sizey%8)?1:0);
  for(i=0;i<sizey;i++)
  {
    OLED_Set_Pos(x,i+y);
    for(m=0;m<sizex;m++)
    {
       temp=BMP[j++];
       OLED_WR_Byte(temp,OLED_DATA);
    }
  }
} 

//OLED的初始化
void OLED_Init(void)
{ 
  OLED_SPI_init(); //初始化I2C 管脚输出高电平
  
  OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
  OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
  OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
  OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
  OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
  OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
  OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
  OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
  OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
  OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
  OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset Shift Mapping RAM Counter (0x00~0x3F)
  OLED_WR_Byte(0x00,OLED_CMD);//-not offset
  OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
  OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
  OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
  OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
  OLED_WR_Byte(0x12,OLED_CMD);
  OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
  OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
  OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_WR_Byte(0x02,OLED_CMD);//
  OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
  OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
  OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
  OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
  OLED_Clear();
  OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
}

void oled_setup()
{
	OLED_Init();
	OLED_ColorTurn(0);//0正常显示 1反色显示
	OLED_DisplayTurn(0);//0正常显示 1翻转180度显示


	OLED_DrawBMP(0,0,128,64,BMP1); //显示图片
	vTaskDelay(1000 / portTICK_PERIOD_MS); //延时1秒 delay函数的单位为ms
	OLED_Clear();//清除界面

	//OLED_ShowString(0,0,"OLDZHAI",16);
	//OLED_ShowString(20,4,"2021/1/18",16);
	//OLED_ShowString(0,6,"ASCII:",16);  
	//OLED_ShowString(63,6,"CODE:",16);
	//vTaskDelay(1000 / portTICK_PERIOD_MS); //延时1秒 delay函数的单位为ms
	//OLED_Clear();

	OLED_ShowString(0,0,"PGG",16);

	OLED_ShowChinese(0,4,7,16);
	OLED_ShowChinese(20,4,8,16);

	OLED_ShowChinese(0,6,9,16);
	OLED_ShowChinese(20,6,10,16);
}


void oled_loop()
{
	int i = 0;
	for (;;) 
	{
		char show[8]={0};
		
        sprintf(show, "%d%%",i);
		OLED_ShowString(50,4,show,16);
		
        sprintf(show, "%d",i);
		OLED_ShowString(50,6,show,16);
		
        vTaskDelay(2000 / portTICK_PERIOD_MS);
		i++;
    }

}



