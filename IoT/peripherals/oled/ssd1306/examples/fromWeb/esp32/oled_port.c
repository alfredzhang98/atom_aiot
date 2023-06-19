
#include "oled_port.h" 

#define LCD_HOST    SPI2_HOST

#define PIN_NUM_MISO SPI2_IOMUX_PIN_NUM_MISO
#define PIN_NUM_MOSI SPI2_IOMUX_PIN_NUM_MOSI
#define PIN_NUM_CLK  SPI2_IOMUX_PIN_NUM_CLK
#define PIN_NUM_CS   SPI2_IOMUX_PIN_NUM_CS

#define PIN_NUM_DC   9
#define PIN_NUM_RST  4
#define PIN_NUM_BCKL 5

spi_device_handle_t spi;


void OLED_Write_SPI_Command(unsigned char SPI_Command)
	
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&SPI_Command;       //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

/* Send data to the LCD. Uses spi_device_polling_transmit, which waits until the
 * transfer is complete.
 *
 * Since data transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transactions is more than
 * just waiting for the transaction to complete.
 */
 
void OLED_Write_SPI_Data(unsigned char SPI_Data)
{
    esp_err_t ret;
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));       		//Zero out the transaction
    t.length=8;                 			//Len is in bytes, transaction length is in bits.
    t.tx_buffer=&SPI_Data;          		//Data
    t.user=(void*)1;                		//D/C needs to be set to 1
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            		//Should have had no issues.
}

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

void OLED_SPI_init()
{
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
		.mode=0,								//SPI mode 0
		.spics_io_num=PIN_NUM_CS,				//CS pin
		.queue_size=7,							//We want to be able to queue 7 transactions at a time
		.pre_cb=lcd_spi_pre_transfer_callback,	//Specify pre-transfer callback to handle D/C line
	};

	
    //Initialize non-SPI GPIOs
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);

	    //Reset the display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_RATE_MS);
	
	//Initialize the SPI bus
	ret=spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
	ESP_ERROR_CHECK(ret);

	//Attach the LCD to the SPI bus
	ret=spi_bus_add_device(LCD_HOST, &devcfg, &spi);
	ESP_ERROR_CHECK(ret);
	
}

