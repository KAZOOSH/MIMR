/*
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
*/
void setup() {
    Serial.begin(9600);
    pinMode(9, OUTPUT);
    //pinMode(6, OUTPUT);

    
    
    //leonardo pwm mode
    TCCR1A = (1 << WGM10);
    TCCR1B = (1 << WGM12);
    TCCR1C = 0;

    
    TCCR1B |= (1 << CS10); // no prescaler - 62KHz
    //TCCR1B |= (1 << CS11); // no prescaler - 62KHz

    TCCR1A |= (1 << COM1A1);
   
}

int adc;
void loop() {

   
  adc = analogRead(0);
  //analogWrite(6,adc / 4);
  OCR1A = (adc / 4);
  Serial.print(adc);
  Serial.print("\n");
}
