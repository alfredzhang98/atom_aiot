#include <math.h>
#include <cstring>
#include "esp_timer.h"

#undef  pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

#define RAD_TO_DEG (180.0/M_PI)
#define DEG_TO_RAD 0.0174533

#define I2C_BUFFER_LENGTH 128
#define I2CDEVLIB_WIRE_BUFFER_LENGTH I2C_BUFFER_LENGTH
#define PI M_PI

#define delayMicroseconds(us) esp_rom_delay_us(us)
#define delay(ms) esp_rom_delay_us(ms*1000)
#define micros() (unsigned long) (esp_timer_get_time())
#define map(x, in_min, in_max, out_min, out_max) (long) ((x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min)
