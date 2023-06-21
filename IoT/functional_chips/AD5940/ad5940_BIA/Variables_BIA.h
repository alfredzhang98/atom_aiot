extern "C" {
#include "ad5940.h"
}
#include "math.h"
extern "C" {
#include "BodyImpedance.h"
}
#include <SPI.h>

#define APPBUFF_SIZE 512
uint32_t AppBuff[APPBUFF_SIZE];
// unsigned int waitfor_ms = 50;
static uint32_t IntCount=0;
static uint32_t count;
uint32_t  NOD;
uint32_t temp;
int debbb;

#define SPI_CS_AD5940_Pin 10
#define AD5940_ResetPin A3
#define AD5940_IntPin 2
//*******************************************************
// uint32_t time_current_a;
float mag_print;
float freq_print;
float phase_print;
//*******************************************************
//declarations/definitions
volatile static uint32_t ucInterrupted = 0;       /* Flag to indicate interrupt occurred */
void Ext_Int0_Handler(void);
