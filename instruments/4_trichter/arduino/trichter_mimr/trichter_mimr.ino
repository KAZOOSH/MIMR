/* 
 * MIMR - TRICHTER
 * KAZOOSH! - Martin Herrmann, Brian Eschrich 2017
 * reads Values from the Trichter and sends it via serial to the pi
 * 
  braun :         GND + Wiederstand(407 ohm)
  braun-weiss :   GND
  grün :          RECHTS
  grün-weiss :    LINKS
  orange :        5V  

 */

 //min max values for trichter
const int pMin = 0;
const int pMax = 127;

 //pins and patterns
int pin_l = 13;
int pin_r = 11;

int test_led_l=0;
int test_led_r=0;

int patternR1 = 1; // (SS,SW)
int patternR2 = 0; // (WS,SS)
int patternL1 = 0; // (SS,WS)
int patternL2 = 1; // (SW,SS)

int left[2] = {0,0};
int right[2] = {0,0};

int leftvalue;
int rightvalue;

//current position
int pos = 63;



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
      patternR1 = 1;
      patternR2 = 0;
      patternL2 = 0;
      patternL1 = 1;
      if(pos < pMax){
        pos++;
        Serial.println(pos);
      }
    }
  }
  
  if (left[0]==1 && left[1]==0 && right[0]==0 && right[1]==0) {
   if(patternR1 == 1) {
      patternR2 = 1;
      patternR1 = 0;
      patternL1 = 0;
      patternL2 = 1;
      if(pos < pMax){
        pos++;
        Serial.println(pos);
      }
    }
  }
  
  if (left[0]==0 && left[1]==1 && right[0]==0 && right[1]==0) {
    if(patternL1 == 1) {
      patternL2 = 1;
      patternL1 = 0;
      patternR2 = 1;
      patternR1 = 0;
      if(pos > pMin){
        pos--;
        Serial.println(pos);
      }
    }
  }
  
  if (left[0]==0 && left[1]==0 && right[0]==1 && right[1]==0) {
    if(patternL2 == 1) {
      patternL1 = 1;
      patternL2 = 0;
      patternR2 = 0;
      patternR1 = 1;
      if(pos > pMin){
        pos--;
        Serial.println(pos);
      }
    }
  }
}




