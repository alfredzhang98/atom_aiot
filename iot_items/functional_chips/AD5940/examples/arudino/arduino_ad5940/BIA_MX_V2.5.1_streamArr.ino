/*
updated version from V2.5
work for read_4+.py etc
*/
#include "Variables_MUX.h"
#include "Variables_BIA.h"

// #define SERIAL_BUFFER_SIZE 256
bool AD5940_Sample_required   = false;
bool AD5940_Currently_Working = false;
bool Sample_Sequence_entered  = false;
int  current_loop_no          = 0;
const bool debug_print        = 0;
uint8_t read_repeat           = 2;

void setup() {
  Serial.begin(115200); // origin 250000 slow:9600 normal: 115200 faster: 921600(if we use the UCB to UART converter)
  Serial.flush(); // discard any startup data received
  /*init MUX*/
  MUX_setup();
  /*init AD5940*/
  AD5940_BIA_Setup();
  // AD5940_Sample_Main(true);
  delay(2000);//delay is needed
  // AppBIAISR(AppBuff, &temp);
}

uint8_t patUsed = 0;
uint8_t pat_size_1 = 0;
uint8_t pat_size_2 = 0;
// uint8_t pat_size_3 = 0;
// uint8_t pat_size_4 = 0;
uint8_t pat_all_size = 0;

uint8_t buff1_size = 0;
uint8_t buff2_size = 0;
uint8_t buffAll_size = 0;

void loop() {
  uint8_t Usr_cmd = 0;
  Usr_cmd = Serial.read();

  //receive the "~" 0x7e
  //function: revice the control data setting parameters: patUsed pat_size_1 pat_size_2
  if (Usr_cmd == 126){
    // block way to read the data and set patUsed pat_size_1 pat_size_2
    while (Serial.available() < 1);
      patUsed = Serial.read();
    while (Serial.available() < 1);
      pat_size_1 = Serial.read();
    while (Serial.available() < 1);
      pat_size_2 = Serial.read();
    // while (Serial.available() < 1);
    //   pat_size_3 = Serial.read();
    // while (Serial.available() < 1);
    //   pat_size_4 = Serial.read();
  }


  //receive the " " 0x7f
  //function: receive the data from serial to get the combine dataset and 2-D dataset dataseq: is [pat_all_size][4] to save the switch pattern
  if (Usr_cmd == 127){
    buff1_size = pat_size_1*4;
    buff2_size = pat_size_2*4;
    // initial the bytes buffer
    byte buffer_1[buff1_size]; 
    byte buffer_2[buff2_size]; 
    // byte buffer_3[pat_size_3*4]; 
    // byte buffer_4[pat_size_4*4];

    // loop to get the date from the Serial
    for (int i = 0; i < patUsed; i++) {
      if(i==0){
        while (Serial.available() < buff1_size);
        Serial.readBytes(buffer_1, buff1_size); 
      }
      if(i==1){
        while (Serial.available() < buff2_size);
        Serial.readBytes(buffer_2, buff2_size); 
      }
      // if(i==2){
      //   while (Serial.available() < 88);
      //   Serial.readBytes(buffer_3, 88); 
      // }
      // if(i==3){
      //   while (Serial.available() < 88);
      //   Serial.readBytes(buffer_4, 88); 
      // }
    }
    buffAll_size = buff1_size+buff2_size;

    byte combined_buffer[buffAll_size];
    memcpy(combined_buffer, buffer_1, sizeof(buffer_1));
    memcpy(combined_buffer + sizeof(buffer_1), buffer_2, sizeof(buffer_2));
    // memcpy(combined_buffer + sizeof(buffer_1)+ sizeof(buffer_2), buffer_3, sizeof(buffer_3));
    // memcpy(combined_buffer + sizeof(buffer_1)+ sizeof(buffer_2)+ sizeof(buffer_3), buffer_4, sizeof(buffer_4));

    // Copy the contents of buffer into dataseq
    pat_all_size = pat_size_1+pat_size_2;
    for (int i = 0; i < pat_all_size; i++) {
        for (int j = 0; j < 4; j++) {
        // Calculate the index of the next value in buffer
            int bufferIndex = i * 4 + j;
        // Copy the value from buffer into the corresponding position in dataseq
            dataseq[i][j] = combined_buffer[bufferIndex];
            // Serial.println(bufferIndex);
        }
    }
  }

  //receive the "" 0x80
  //function: this function is to work with the AD5940, read out every line in the dataseq to dataNumber[1-4]
  if (Usr_cmd == 128){ 
    for (int i = 0; i<pat_all_size; i++){
      dataNumber[0] = dataseq[i][0];
      dataNumber[1] = dataseq[i][1];
      dataNumber[2] = dataseq[i][2];
      dataNumber[3] = dataseq[i][3];

      //use the MUX to switch the electrode
      handle_mux();// 控制GPIO以控制MUX chip
      // Serial.print("received data: ");
      // Serial.print(dataNumber[0]);    
      // Serial.print(dataNumber[1]);
      // Serial.print(dataNumber[2]);
      // Serial.println(dataNumber[3]); 
  
      //Way1
      AppBIACtrl(BIACTRL_START, 0); //start AD5940
      AD5940_Sample_Main(true);
      AppBIACtrl(BIACTRL_SHUTDOWN, 0); //shutdown AD5940

      //Way2
      //chip select high to de-select AD5940 initially
      // AD5940_CsSet();
      // AD5940_RstSet();
      // AD5940_Sample_Main(false);

      //Way3
      //chip select high to de-select AD5940 initially
      // AD5940_CsSet();
      // AD5940_RstSet();
      // AppBIACtrl(BIACTRL_START, 0); //start AD5940
      // AD5940_Sample_Main(false);
      // AppBIACtrl(BIACTRL_SHUTDOWN, 0); //shutdown AD5940

      // AD5940_Delay10us(450); 

      union {
        float f;
        byte floatBytes[4];
      } floatData;

      floatData.f = mag_print;
      Serial.write(floatData.floatBytes, 4);
      floatData.f = freq_print;
      Serial.write(floatData.floatBytes, 4);
      floatData.f = phase_print;
      Serial.write(floatData.floatBytes, 4);
    }
  }
}

