#include "myRGB.h"

//hardware
const int redLedPin  = 10; //red
const int greenLedPin   = 6; //yellow
const int blueLedPin  = 11; //white

const int whiteSwitchPin  = 7; //green
const int orangeSwitchPin  = 2; //yellow
const int redSwitchPin  = 4; //red


const int potiPin = 0;   //R + red
const int fanPin = 9; //R + yellow


//protocol
#define FLAG_RED 1
#define FLAG_ORANGE 2
#define FLAG_WHITE 4
#define FLAG_INVALID 0

const byte protocolSwitchOnValue = 127;
const byte protocolSwitchOffValue = 0;
const char protocolDelimeter = ' '; //seperator between values



void setup() {
  Serial.begin(9600);

  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  pinMode(fanPin, OUTPUT);

  //pullups on
  digitalWrite(whiteSwitchPin, HIGH);
  digitalWrite(orangeSwitchPin, HIGH);
  digitalWrite(redSwitchPin, HIGH);

  //Set Timer1 (Pin 9 und 10) on 31300Hz - wichtig damit luefter nicht "singt"
  TCCR1B = TCCR1B & 0b11111000 | 0x01;
}

uint8_t interPol(float start,float ende, uint8_t range,uint8_t steep) {
  uint8_t res;
  res = ((start + ((ende - start) / range) * steep));
  return res;
}

void fadePallette(myRGB par[],uint8_t elements,uint8_t steep) {
  uint8_t g = 255 / (elements - 1);
  uint8_t w = steep % g;
  uint8_t si = steep / g;
  uint8_t ei = si + 1;

  myRGB r;
  
  r.r = interPol(par[si].r,par[ei].r,g,w);
  r.g = interPol(par[si].g,par[ei].g,g,w);
  r.b = interPol(par[si].b,par[ei].b,g,w);

  analogWrite(redLedPin,r.r);
  analogWrite(greenLedPin,r.g);
  analogWrite(blueLedPin,r.b);    
}

inline void sendSwitchValue(byte flag) {
  if(flag) {
    Serial.print(protocolSwitchOnValue);
  } else {
    Serial.print(protocolSwitchOffValue);
  }
  Serial.print(protocolDelimeter);
}

void loop() {
  int cPotiValue = analogRead(potiPin);
  byte sendPotiValue = map(cPotiValue, 0, 1023, 0, 127);
  unsigned long ts = millis();
  
  static unsigned long deltaW=0;
  static byte lPotiValue;
  
  static byte cFlags = FLAG_INVALID; 
  static byte lFlags;

  //reading switches set Flags
  if(deltaW < ts) {
    deltaW = ts + 300; //300 ms Debounce
    
    if(digitalRead(whiteSwitchPin)) {
      cFlags |= FLAG_WHITE;
    } else {
      cFlags &= ~FLAG_WHITE;
    }
    if(digitalRead(orangeSwitchPin)) {
      cFlags |= FLAG_ORANGE;
    } else {
      cFlags &= ~FLAG_ORANGE;
    }
    if(digitalRead(redSwitchPin)) {
      cFlags |= FLAG_RED;
    } else {
      cFlags &= ~FLAG_RED;
    }        
  }
  
  byte tmp = map(cPotiValue, 0, 1023, 0, 255);
  //set fan speed according to poti value
  analogWrite(fanPin,tmp);

  //set led Color Fading according to poti value
  fadePallette(pallette,palletteSize,tmp);

  //send values if necessary
  if ((cFlags != lFlags) || (sendPotiValue != lPotiValue)) {
    lFlags = cFlags;
    lPotiValue = sendPotiValue;
    
    //send status
    Serial.print(':');
    sendSwitchValue(cFlags & FLAG_RED);
    sendSwitchValue(cFlags & FLAG_ORANGE);
    sendSwitchValue(cFlags & FLAG_WHITE);

    Serial.println(sendPotiValue);
    
  }
}

