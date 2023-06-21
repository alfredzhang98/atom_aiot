bool control_Digital(uint8_t *ARD_PIN_NUM) {
  uint8_t temp = *ARD_PIN_NUM;
  if (temp < 1 || temp > 32) {
    return false;
  }
  else {
    temp = temp - 1; //00000对应的是s1
    if (bitRead( temp, 0)) {
      digitalWrite(D0_pin, HIGH);
    } else {
      digitalWrite(D0_pin, LOW);
    }
    if (bitRead( temp, 1)) {
      digitalWrite(D1_pin, HIGH);
    } else {
      digitalWrite(D1_pin, LOW);
    }
    if (bitRead( temp, 2)) {
      digitalWrite(D2_pin, HIGH);
    } else {
      digitalWrite(D2_pin, LOW);
    }
    if (bitRead( temp, 3)) {
      digitalWrite(D3_pin, HIGH);
    } else {
      digitalWrite(D3_pin, LOW);
    }
    if (bitRead( temp, 4)) {
      digitalWrite(D4_pin, HIGH);
    } else {
      digitalWrite(D4_pin, LOW);
    }
    if (debug_print) {
      for (uint8_t i = 0; i < 6; i++) {
        Serial.print(bitRead( temp, i));
        
      }
    }
  }
  return true;
}




/*FPC pin number和ADG732的对应关系*/
uint8_t check_table(uint8_t FPC_PIN_NUM) {
  uint8_t ADG_PIN_NUMBER;
  switch (FPC_PIN_NUM) {
    case 1:  // your hand is on the sensor
      ADG_PIN_NUMBER = 17;
      break;
    case 2:  // your hand is close to the sensor
      ADG_PIN_NUMBER = 18;
      break;
    case 3:  // your hand is a few inches from the sensor
      ADG_PIN_NUMBER = 19;
      break;
    case 4:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 20;
      break;
    case 5:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 21;
      break;
    case 6:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 22;
      break;
    case 7:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 23;
      break;
    case 8:  // your hand is on the sensor
      ADG_PIN_NUMBER = 24;
      break;
    case 9:  // your hand is close to the sensor
      ADG_PIN_NUMBER = 25;
      break;
    case 10:  // your hand is a few inches from the sensor
      ADG_PIN_NUMBER = 26;
      break;
    case 11:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 27;
      break;
    case 12:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 28;
      break;
    case 13:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 29;
      break;
    case 14:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 30;
      break;
    case 15:  // your hand is on the sensor
      ADG_PIN_NUMBER = 31;
      break;
    case 16:  // your hand is close to the sensor
      ADG_PIN_NUMBER = 32;
      break;
    case 17:  // your hand is a few inches from the sensor
      ADG_PIN_NUMBER = 16;
      break;
    case 18:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 15;
      break;
    case 19:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 14;
      break;
    case 20:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 13;
      break;
    case 21:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 12;
      break;
    case 22:  // your hand is on the sensor
      ADG_PIN_NUMBER = 11;
      break;
    case 23:  // your hand is close to the sensor
      ADG_PIN_NUMBER = 10;
      break;
    case 24:  // your hand is a few inches from the sensor
      ADG_PIN_NUMBER = 9;
      break;
    case 25:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 8;
      break;
    case 26:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 7;
      break;
    case 27:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 6;
      break;
    case 28:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 5;
      break;
    case 29:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 4;
      break;
    case 30:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 3;
      break;
    case 31:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 2;
      break;
    case 32:  // your hand is nowhere near the sensor
      ADG_PIN_NUMBER = 1;
      break;
    default:
      ADG_PIN_NUMBER = 0;
      Serial.println("Unsupported FPC pin number");
      break;
  }
  return ADG_PIN_NUMBER;
}
