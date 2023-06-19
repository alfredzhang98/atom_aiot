/********************************************************
 * @file        	oled_ssd1306.c
 * @author      	Alfred
 * @version     	V1.0.0
 * @date        	06/18/2022
 * @brief       	the driver of the ssd1306 oled
 *******************************************************
 *For 7-pin oled
 *GND						------>GND
 *VCC						------>3.3V - 5V
 *clock in 				    ------>D0
 *data in and response out	------>D1
 *reset					    ------>RES
 *data or command			------>DC
 *chip select				------>CS
 ********************************************************/

#include <oled_ssd1306.h>
#include <oled_font_picture.h>

#define INIT_DATA 27
#define SCROLL_DATA 9
#define DISPLAY_DATA 6

static u_int8_t oled_init_data[INIT_DATA] = {
    0xAE, // turn off oled panel
    0x00, // set low column address
    0x10, // set high column address
    0x40, // set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    0x81, // set contrast control register
    0xCF, // set SEG Output Current Brightness
    0xA1, // set SEG/Column Mapping     0xa0 reverse 0xa1 normal
    0xC8, // set COM/Row Scan Direction   0xc0 reverse 0xc8 normal
    0xA6, // set normal display
    0xA8, // set multiplex ratio(1 to 64)
    0x3F, // 1/64 duty
    0xD3, // set display offset Shift Mapping RAM Counter (0x00~0x3F)
    0x00, // not offset
    0xD5, // set display clock divide ratio/oscillator frequency
    0x80, // set divide ratio, Set Clock as 100 Frames/Sec
    0xD9, // set pre-charge period
    0xF1, // set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    0xDA, // set com pins hardware configuration
    0x12,
    0xDB, // set vcomh
    0x40, // set VCOM Deselect Level 0x30 some programme
    0x20, // set Page Addressing Mode (0x00/0x01/0x02)
    0x02,
    0x8D, // set Charge Pump enable/disable
    0x14, // set(0x10) disable
    0xA4, // disable Entire Display On (0xa4/0xa5)
    0xA6  // disable Inverse Display On (0xa6/a7)
};

static u_int8_t oled_scroll_data[SCROLL_DATA] = {
    0x2E,
    0x27,
    0x00,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0xFF,
    0x2F};

static u_int8_t oled_display_data[DISPLAY_DATA] = {
    0x8D,
    0x14,
    0xAF,
    0x8D,
    0x10,
    0xAE,
};

//*******************************function***************************************

/**
 * @brief write one date
 */
void oledWriteByte(unsigned char dat, unsigned char cmd)
{
    if (cmd == OLED_DATA)
    {
        writeByteDATA(dat);
    }
    else if (cmd == OLED_CMD)
    {
        writeByteCMD(dat);
    }
}

/**
 * @brief clean the screen
 */
void oledClear(void)
{
    unsigned char i, j;
    for (i = 0; i < 8; i++)
    {
        oledWriteByte(0xb0 + i, OLED_CMD);
        oledWriteByte(0x00, OLED_CMD);
        oledWriteByte(0x10, OLED_CMD);
        for (j = 0; j < 128; j++)
        {
            oledWriteByte(0, OLED_DATA);
        }
    }
}

/**
 * @brief set the pixel position on the OLED
 */
void oledSetPostion(unsigned char x, unsigned char y)
{
    oledWriteByte(0xb0 + y, OLED_CMD);
    oledWriteByte((x & 0x0f), OLED_CMD);
    oledWriteByte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
}

/**
 * @brief Screen colour trun
 */
void oledColorTurn(bool status)
{
    if (status == false)
    {
        oledWriteByte(0xA6, OLED_CMD); // normal
    }
    else if (status == true)
    {
        oledWriteByte(0xA7, OLED_CMD); // reverse
    }
}

/**
 * @brief Screen rotation 180 degrees
 */
void oledDisplayTurn(bool status)
{
    if (status == false)
    {
        oledWriteByte(0xC8, OLED_CMD); // normal
        oledWriteByte(0xA1, OLED_CMD);
    }
    else if (status == true)
    {
        oledWriteByte(0xC0, OLED_CMD); // turn 180 degrees
        oledWriteByte(0xA0, OLED_CMD);
    }
}

/**
 * @brief OLED scroll function, range 0 to 1 page, horizontal to left
 */
void oledScroll(void)
{
    u_int8_t i;
    for (i = 0; i < SCROLL_DATA; i++)
    {
        oledWriteByte(oled_scroll_data[i], OLED_CMD);
    }
}

/**
 * @brief open and close the display
 */
void oledDisplayStatus(bool status)
{
    u_int8_t i;
    // on
    if (status == true)
    {
        for (i = 0; i < SCROLL_DATA / 2; i++)
        {
            oledWriteByte(oled_display_data[i], OLED_CMD);
        }
        // off
    }
    else if (status == false)
    {
        for (i = 3; i < SCROLL_DATA / 2; i++)
        {
            oledWriteByte(oled_display_data[i], OLED_CMD);
        }
    }
}

/**
 * @remark should be init after the spi interface init
 */
void oledSSD1603Init(void)
{
    u_int8_t i;
    oledSpiReset();
    for (i = 0; i < INIT_DATA; i++)
    {
        oledWriteByte(oled_init_data[i], OLED_CMD);
    }
    oledClear();
    oledWriteByte(0xAF, OLED_CMD);
    oledSetPostion(0, 0);
}

/**
 * @brief the set up for all (spi and oled)
 */
void oledSSD1603Setup(void)
{
    oledSpiInit();
    oledSSD1603Init();
    oledColorTurn(false);
    oledDisplayStatus(false);
}

//*******************************draw***************************************

/**
 * @brief Display a character
 */
void oledShowChar(unsigned char x, unsigned char y, const unsigned char chr, unsigned char sizey)
{
    unsigned char c = 0, sizex = sizey / 2, temp;
    unsigned short i = 0, size1;
    if (sizey == 8)
        size1 = 6;
    else
        size1 = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * (sizey / 2);
    c = chr - ' ';
    oledSetPostion(x, y);
    for (i = 0; i < size1; i++)
    {
        if (i % sizex == 0 && sizey != 8)
            oledSetPostion(x, y++);
        if (sizey == 8)
        {
            temp = asc2_0806[c][i];
            oledWriteByte(temp, OLED_DATA); // 6X8 size
        }
        else if (sizey == 16)
        {
            temp = asc2_1608[c][i];
            oledWriteByte(temp, OLED_DATA); // 8x16 size
        }
        else
            return;
    }
}

/**
 * @brief calculated m^n
 */
unsigned int oledPow(unsigned char m, unsigned char n)
{
    unsigned int result = 1;
    while (n--)
        result *= m;
    return result;
}

/**
 * @brief Display a number of the specified length at the specified position
 */
void oledShowNum(unsigned char x, unsigned char y, unsigned int num, unsigned char len, unsigned char sizey)
{
    unsigned char t, temp, m = 0;
    unsigned char enshow = 0;
    if (sizey == 8)
        m = 2;
    for (t = 0; t < len; t++)
    {
        temp = (num / oledPow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                oledShowChar(x + (sizey / 2 + m) * t, y, ' ', sizey);
                continue;
            }
            else
                enshow = 1;
        }
        oledShowChar(x + (sizey / 2 + m) * t, y, temp + '0', sizey);
    }
}

/**
 * @brief Display a number of the specified length at the specified position
 */
void oledShowString(unsigned char x, unsigned char y, const char *chr, unsigned char sizey)
{
    unsigned char j = 0;
    while (chr[j] != '\0')
    {
        oledShowChar(x, y, chr[j++], sizey);
        if (sizey == 8)
            x += 6;
        else
            x += sizey / 2;
    }
}

/**
 * @brief Show images
 */
void oledDrawDiagram(unsigned char x, unsigned char y, unsigned char sizex, unsigned char sizey, const unsigned char BMP[])
{
    unsigned short j = 0;
    unsigned char i, m, temp;
    sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
    for (i = 0; i < sizey; i++)
    {
        oledSetPostion(x, i + y);
        for (m = 0; m < sizex; m++)
        {
            temp = BMP[j++];
            oledWriteByte(temp, OLED_DATA);
        }
    }
}

/**
 * @brief a loop show test
 */
void oledLoopTest(void)
{
    int i = 0;

    oledDrawDiagram(0, 0, 128, 64, image_test);
    delay_ms(2000);
    oledClear();

    oledShowString(0, 0, "Alfred", 16);

    for (;;)
    {
        char show[20] = {0};

        sprintf(show, "%d", i);
        oledShowString(50, 4, show, 16);

        delay_ms(2000);
        i++;
    }
}