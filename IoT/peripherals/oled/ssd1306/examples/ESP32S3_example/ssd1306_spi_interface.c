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

#define LCD_HOST    SPI2_HOST

#define PIN_NUM_MISO 13 //not used
#define PIN_NUM_MOSI 11
#define PIN_NUM_CLK  12
#define PIN_NUM_CS   10

#define PIN_NUM_DC   7
#define PIN_NUM_RST  6
#define PIN_NUM_BCKL 5

void delay_ms(unsigned int ms);

spi_device_handle_t spi;

void lcd_cmd(spi_device_handle_t spi, const uint8_t cmd, bool keep_cs_active)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    if (keep_cs_active) {
      t.flags = SPI_TRANS_CS_KEEP_ACTIVE;   //Keep CS active after data transfer
    }
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

void lcd_data(spi_device_handle_t spi, const uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len==0) return;             //no need to send anything
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=data;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

void writeByteCMD(unsigned char dat){
    //TODO: spi write bytes function in your board
    lcd_cmd(spi, dat, false);
}

void writeByteDATA(unsigned char dat){
    //TODO: spi write bytes function in your board
    lcd_data(spi, &dat, 1);
}

void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

void oledSpiInit(void){
    //TODO: SPI initial
    esp_err_t ret;
    spi_bus_config_t buscfg={
    .miso_io_num=PIN_NUM_MISO,
    .mosi_io_num=PIN_NUM_MOSI,
    .sclk_io_num=PIN_NUM_CLK,
    .quadwp_io_num=-1,
    .quadhd_io_num=-1,
    //.max_transfer_sz=PARALLEL_LINES*128*2+8
    };

    spi_device_interface_config_t devcfg=
    {
      .clock_speed_hz=10*1000*1000,			//Clock out at 10 MHz
      .mode=0,								          //SPI mode 0
      .spics_io_num=PIN_NUM_CS,				  //CS pin
      .queue_size=7,							      //We want to be able to queue 7 transactions at a time
      .pre_cb=lcd_spi_pre_transfer_callback,	//Specify pre-transfer callback to handle D/C line
    };

    //Initialize non-SPI GPIOs
    gpio_reset_pin(PIN_NUM_DC);
    gpio_reset_pin(PIN_NUM_RST);
    gpio_reset_pin(PIN_NUM_BCKL);
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);

    //Initialize the SPI bus
    ret=spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(LCD_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

}

void oledSpiReset(void)
{
	//TODO: Set the reset gpio level (high(100ms)-low(200ms)-high(200ms))
  gpio_set_level(PIN_NUM_RST, 1);
  delay_ms(100);
  gpio_set_level(PIN_NUM_RST, 0);
  delay_ms(100);
  gpio_set_level(PIN_NUM_RST, 1);
}


void delay_ms(unsigned int ms)
{
	//TODO: delay ms function in your board
  vTaskDelay(ms / portTICK_PERIOD_MS);
}