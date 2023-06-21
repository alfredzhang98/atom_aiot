#define D0_pin 37
#define D1_pin 35
#define D2_pin 33
#define D3_pin 31
#define D4_pin 29

//按照F+F- S+S-连接
#define MUX_WR_1 51
#define MUX_WR_2 25
#define MUX_WR_3 53
#define MUX_WR_4 23

// Define according to hardware connections.

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
char tempChars[numChars];
boolean newData = false;
bool GPIO_handle_required = false;

int dataseq_length = 0;

uint8_t dataseq[56][4]; // 提供一个buffer，记录采样序列
uint8_t dataNumber[4] = {0, 0, 0, 0};          // 用于记录用户输入