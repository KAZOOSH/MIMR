#include <FastLED.h>

#define PI 3.14159265359

//hardware
const int redLedPin  = 10; //red
const int greenLedPin   = 6; //yellow
const int blueLedPin  = 11; //white

const int whiteSwitchPin  = 7; //green
const int orangeSwitchPin  = 2; //yellow
const int redSwitchPin  = 4; //red


const int potiPin = 0;   //R + red
const int fanPin = 9; //R + yellow

//led control
byte serialIn[2] = {0,1}; //intensity,isidle
unsigned long lastSwitchTime = 0; 

//protocol
#define FLAG_RED 1
#define FLAG_ORANGE 2
#define FLAG_WHITE 4
#define FLAG_INVALID 0

const byte protocolSwitchOnValue = 127;
const byte protocolSwitchOffValue = 0;
const char protocolDelimeter = ' '; //seperator between values

//communication to serial
// dummy byte indicating start of Ableton data
#define SYNC_BYTE 255

// Arduino -> RasPi send interval (microseconds)
#define SEND_INTERVAL 20*1000L // 20 ms = 50 fps
unsigned long lastSendTime = 0;

// baud rate for USB communication with RasPi
#define BAUD_RATE 115200L


void setup() {
  Serial.begin(BAUD_RATE);

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


void showAnalogRGB( const CRGB& rgb)
{
  analogWrite(redLedPin,   rgb.r );
  analogWrite(greenLedPin, rgb.g );
  analogWrite(blueLedPin,  rgb.b );
}

void setLedColor(int pos,byte button1, byte button2, byte button3){
  //multiplier for midi value
  float value = (255.0 - serialIn[0])/255.0;

  //calculate min brightness
  float minBrightness = 5;
  float offset = (255.0-minBrightness)/255.0;

  //alltogether
  int r = (button1*pos*offset + minBrightness)*value;
  int g = (button2*pos/2*offset + minBrightness)*value;
  int b = (button3*pos/4*offset + minBrightness)*value;

  //flickering when switched led
  if(micros() - lastSwitchTime < 10000)
  {
    r = 255;
    g = 255;
    b = 255;
  }

  showAnalogRGB( CRGB(r,g,b) );
}


inline void sendSwitchValue(byte flag) {
  if(flag) {
    Serial.print(protocolSwitchOnValue);
  } else {
    Serial.print(protocolSwitchOffValue);
  }
  Serial.print(protocolDelimeter);
}

void doIdle(){
  int pos = millis()%6500;
  pos = pos%6500;
  float p = pos/5200.0;

  float maxBrightness = 45;
  
  int value = sin(p*PI)*maxBrightness;
  if(p >1) value = 0;

  analogWrite(fanPin,0);
  showAnalogRGB( CRGB(value,value,value) );
}

void loop() {
  // check serial buffer for input
    int test = Serial.read();

  // start byte received?
  if( test == SYNC_BYTE )
  {
    // yes, read data bytes
    Serial.readBytes( serialIn, 2 );
  }

//idle
  if(serialIn[1] == 1){
    doIdle();
  }
  else
  {
    //read sensors
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

  if(cFlags != lFlags) lastSwitchTime = micros();
  
  byte tmp = map(cPotiValue, 0, 1023, 0, 255);
  //set fan speed according to poti value
  analogWrite(fanPin,tmp);

  //set led Color Fading according to poti value
  //fadePallette(pallette,palletteSize,tmp);
  setLedColor(tmp,cFlags & FLAG_RED,cFlags & FLAG_ORANGE,cFlags & FLAG_WHITE);


  //send values if necessary
  if (((cFlags != lFlags) || (sendPotiValue != lPotiValue))
  && micros() - lastSendTime > SEND_INTERVAL) {

    // remember time
    lastSendTime = micros();
    
    lFlags = cFlags;
    lPotiValue = sendPotiValue;
    
    //send status
    Serial.print(':');
    sendSwitchValue(cFlags & FLAG_RED);
    sendSwitchValue(cFlags & FLAG_ORANGE);
    sendSwitchValue(cFlags & FLAG_WHITE);

    Serial.println(sendPotiValue);
    // wait for transmission to finish
    Serial.flush();
    
  }
  }
  
}

