#ifndef SPI_AD5940_PORT_H
#define SPI_AD5940_PORT_H

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include <ad5940lib/ad5940.h>

// Define GPIO pins for SPI and AD5940 control
#define SPI_SCK CONFIG_GPIO_SCLK
#define SPI_MISO CONFIG_GPIO_MISO
#define SPI_MOSI CONFIG_GPIO_MOSI
#define SPI_CS_AD5940_Pin CONFIG_GPIO_CS
#define AD5940_ResetPin CONFIG_GPIO_RESET
#define AD5940_IntPin CONFIG_GPIO_INT

// Define SPI clock speed
#define SPI_CLK 1200000 // 12 MHz

// Define SPI host
#ifndef HSPI_HOST
#define HSPI_HOST SPI2_HOST
#endif

// Function prototypes
void AD5940_CsClr(void);
void AD5940_CsSet(void);
void AD5940_RstSet(void);
void AD5940_RstClr(void);
void AD5940_Delay10us(uint32_t time);
uint32_t AD5940_GetMCUIntFlag(void);
uint32_t AD5940_ClrMCUIntFlag(void);
uint32_t AD5940_MCUResourceInit(void *pCfg);
void AD5940_ReadWriteNBytes(unsigned char *pSendBuffer, unsigned char *pRecvBuff, unsigned long length);

#endif // SPI_AD5940_PORT_H