#ifndef _OLED_PORT_H_
#define _OLED_PORT_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"


void OLED_Write_SPI_Command(unsigned char SPI_Command);
void OLED_Write_SPI_Data(unsigned char SPI_Data);

void OLED_SPI_init();

#endif

