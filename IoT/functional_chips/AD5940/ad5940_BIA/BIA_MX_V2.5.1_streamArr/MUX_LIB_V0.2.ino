// Example 4 - Receive a number as text and convert it to an int
// Serial部分是基于这个帖子的https://forum.arduino.cc/t/serial-input-basics-updated/382007/3
// 此ino通过UART接收一个ASCII，转换为short。此short会正常溢出。
// V0.2:更改了AD5940_Sample_required flag的位置。不符合规则的输入现在不会再触发此flag了。

void MUX_setup() {
  pinMode(D0_pin, OUTPUT);
  pinMode(D1_pin, OUTPUT);
  pinMode(D2_pin, OUTPUT);
  pinMode(D3_pin, OUTPUT);
  pinMode(D4_pin, OUTPUT);

  pinMode(MUX_WR_1, OUTPUT);
  pinMode(MUX_WR_2, OUTPUT);
  pinMode(MUX_WR_3, OUTPUT);
  pinMode(MUX_WR_4, OUTPUT);
  
  digitalWrite(MUX_WR_1, HIGH);
  digitalWrite(MUX_WR_2, HIGH);
  digitalWrite(MUX_WR_3, HIGH);
  digitalWrite(MUX_WR_4, HIGH);
}



void handle_mux(){
  uint8_t ADG_PIN_NUMBER;
  for (uint8_t i = 0; i < 4; i++) {
    if (0 < dataNumber[i] && dataNumber[i] <= 16) {
      ADG_PIN_NUMBER = 16 + dataNumber[i];
    } 
    else if (16 < dataNumber[i] && dataNumber[i] <= 32) {
      ADG_PIN_NUMBER = 33 - dataNumber[i];
    } 
    else {
      ADG_PIN_NUMBER = 0;
    }
    MUX_WR_ENABLE(&i);
    control_Digital(&ADG_PIN_NUMBER);  
    MUX_WR_DISABLE(&i);
  }

}



void MUX_WR_ENABLE(uint8_t *pMUX_NUM) {
  uint8_t MUX_NUM = *pMUX_NUM;
  //WR* Latch, transparent Low
  switch (MUX_NUM) {
    case 0:
      digitalWrite(MUX_WR_1, LOW);
      break;
    case 1:
      digitalWrite(MUX_WR_2, LOW);
      break;
    case 2:
      digitalWrite(MUX_WR_3, LOW);
      break;
    case 3:
      digitalWrite(MUX_WR_4, LOW);
      break;
    default:
      break;
  }
}



void MUX_WR_DISABLE(uint8_t *pMUX_NUM) {
  uint8_t MUX_NUM = *pMUX_NUM;
  //WR* Latch
  switch (MUX_NUM) {
    case 0:
      digitalWrite(MUX_WR_1, HIGH);
      break;
    case 1:
      digitalWrite(MUX_WR_2, HIGH);
      break;
    case 2:
      digitalWrite(MUX_WR_3, HIGH);
      break;
    case 3:
      digitalWrite(MUX_WR_4, HIGH);
      break;
    default:
      break;
  }
}
