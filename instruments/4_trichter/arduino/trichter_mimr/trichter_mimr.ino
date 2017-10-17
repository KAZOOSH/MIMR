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

#include <FastLED.h>

//LED control
#define NUM_LEDS 1
#define DATA_PIN 5

#define PI 3.14159265359

CRGB leds[NUM_LEDS];
byte serialIn[4] = {0,60,0,1}; //intensity, hue-min, hue-max, isIdle

//communication to serial
// dummy byte indicating start of Ableton data
#define SYNC_BYTE 255

// Arduino -> RasPi send interval (microseconds)
#define SEND_INTERVAL 20*1000L // 20 ms = 50 fps
unsigned long lastSendTime = 0;

// baud rate for USB communication with RasPi
#define BAUD_RATE 115200L


 //min max values for trichter
 const int delta = 4;
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

//idlemode
int isInIdleMode =1;

//current position
int pos = 63;

// idle vars
const int idleAnimationTime = 4500;
const int maxIdleBrightness = 70;

void setup() {
  //init serial
  Serial.begin( BAUD_RATE );


  //init led
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);

  pinMode(pin_l, INPUT);
  pinMode(pin_r, INPUT);
  digitalWrite(pin_l, HIGH);       // turn on pullup resistors
  digitalWrite(pin_r, HIGH);       // turn on pullup resistors
}

void sendValue(int value){
    // time to update RasPi with current value?
  if ( micros() - lastSendTime > SEND_INTERVAL )
  {
    // remember time
    lastSendTime = micros();
    
    // write start character
    Serial.print( ":" );
    
    // write value as ASCII and append line break
    Serial.println( value );

    // wait for transmission to finish
    Serial.flush();
  }
}

void setLedColor(){
  int hue = map(pos,0,127,serialIn[1],serialIn[2]);
  int value = 255 - serialIn[0];

  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    leds[dot].setHSV( hue, 255, value);
  }
  FastLED.show();
}

void idleAnimation(){
  int value = maxIdleBrightness+(maxIdleBrightness-1)*sin(2*PI/idleAnimationTime*millis());
  
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    leds[dot].setRGB( value, value, value);
  }
  FastLED.show();
}

void readSensor(){
 
  leftvalue = digitalRead(pin_l);
  rightvalue = digitalRead(pin_r);
  
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
        pos+=delta;
        pos = min(127,pos);
        sendValue(pos);
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
        pos+=delta;
        pos = min(127,pos);
        sendValue(pos);
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
        pos-=delta;
        pos = max(0,pos);
        sendValue(pos);
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
        pos-=delta;
        pos = max(0,pos);
        sendValue(pos);
      }
    }
  }

}

void loop() {
  // check serial buffer for input
  int test = Serial.read();

  // start byte received?
  if( test == SYNC_BYTE )
  {
    // yes, read data bytes
    Serial.readBytes( serialIn, 4 );
  }

  
  if(serialIn[3] == 1){
    // idle mode
    isInIdleMode = 1;
   } else if(serialIn[3] == 0){
     isInIdleMode = 0;
   }

  if(isInIdleMode == 1){
    idleAnimation();
  } else{
     // set LEDs  
     setLedColor();

     // read rotation sensor
     readSensor();  
  }
}




