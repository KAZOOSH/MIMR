/* 
  braun :         GND + Wiederstand(407 ohm)
  braun-weiss :   GND
  grün :          RECHTS
  grün-weiss :    LINKS
  orange :        5V  

  DigitalReadSerial
 Reads a digital input on pin 2, prints the result to the serial monitor 
 
 This example code is in the public domain.
 */

int pin_l = 13;
int pin_r = 11;

int test_led_l=0;
int test_led_r=0;

int patternR1 = 1; // (SS,SW)
int patternR2 = 0; // (WS,SS)
int patternL1 = 0; // (SS,WS)
int patternL2 = 1; // (SW,SS)

int pos = 0;

int left[2] = {0,0};
int right[2] = {0,0};

int leftvalue;
int rightvalue;

void setup() {
  Serial.begin(9600);
  pinMode(pin_l, INPUT);
  pinMode(pin_r, INPUT);
  digitalWrite(pin_l, HIGH);       // turn on pullup resistors
  digitalWrite(pin_r, HIGH);       // turn on pullup resistors
}

void loop() {
  leftvalue = digitalRead(pin_l);
  rightvalue = digitalRead(pin_r);
  
  //Serial.print(leftvalue);
  //Serial.println(rightvalue);
  if(leftvalue!=test_led_l){
    digitalWrite(4, leftvalue);
    test_led_l=leftvalue;
  }
  
  if(rightvalue!=test_led_r){
    digitalWrite(5, rightvalue);
    test_led_r=rightvalue;
  }

  //push left
  left[0] = left[1];
  left[1] = (int)leftvalue;
  
  //push right
  right[0] = right[1];
  right[1] = (int)rightvalue;
  
  
  if (left[0]==0 && left[1]==0 && right[0]==0 && right[1]==1) {
    if(patternR2 == 1) {
      pos++;
      patternR1 = 1;
      patternR2 = 0;
      patternL2 = 0;
      patternL1 = 1;
      Serial.println(pos);
    }
  }
  
  if (left[0]==1 && left[1]==0 && right[0]==0 && right[1]==0) {
   if(patternR1 == 1) {
      pos++;
      patternR2 = 1;
      patternR1 = 0;
      patternL1 = 0;
      patternL2 = 1;
      Serial.println(pos);
    }
  }
  
  if (left[0]==0 && left[1]==1 && right[0]==0 && right[1]==0) {
    if(patternL1 == 1) {
      pos--;
      patternL2 = 1;
      patternL1 = 0;
      patternR2 = 1;
      patternR1 = 0;
      Serial.println(pos);
    }
  }
  
  if (left[0]==0 && left[1]==0 && right[0]==1 && right[1]==0) {
    if(patternL2 == 1) {
      pos--;
      patternL1 = 1;
      patternL2 = 0;
      patternR2 = 0;
      patternR1 = 1;
      Serial.println(pos);
    }
  }
}




