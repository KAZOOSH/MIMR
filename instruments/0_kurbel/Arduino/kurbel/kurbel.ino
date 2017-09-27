/*
 * MIMR - KURBEL
 * KAZOOSH! - Brian Eschrich, Nick Schwarzenberg 2017
 * reads Values from the Kurbel, smooths them a little and sends it via serial to the pi
 */

#include <FastLED.h>

//LED control
#define NUM_LEDS 60
#define DATA_PIN 6

// dummy byte indicating start of Ableton data
#define SYNC_BYTE 255

// Arduino -> RasPi send interval (microseconds)
#define sendInterval 20*1000L // 20 ms = 50 fps
unsigned long lastSendTime = 0;


CRGB leds[NUM_LEDS];

byte serialIn[4] = {0,0,0,0};

//kurbel control
#define GROUND_PIN A0
#define SENSOR_PIN A1
#define POWER_PIN A2

const int numReadings = 5; //number of smoothing values
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


void setup() {
  //set pins
  pinMode(GROUND_PIN,OUTPUT);
  pinMode(POWER_PIN,OUTPUT);
  digitalWrite(POWER_PIN,HIGH);
  digitalWrite(GROUND_PIN,LOW);

  //init serial
  Serial.begin(38400);

  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  //init led
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void smooth(int value){
  // subtract the last reading:
  total = total - readings[readIndex];
  // add new value:
  readings[readIndex] = value;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
}

void ledAutoMode(int value){
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    leds[dot].setRGB( value, value, value);  
  }
}

void ledColorMode(int value){
  CHSV spectrumcolor;
  spectrumcolor.hue = serialIn[1];
  spectrumcolor.saturation =  serialIn[2];
  spectrumcolor.value =     value;
 
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    hsv2rgb_spectrum( spectrumcolor, leds[dot] );
  }
}

void ledColorManual(){
  CHSV spectrumcolor;
  spectrumcolor.hue = serialIn[1];
  spectrumcolor.saturation =  serialIn[2];
  spectrumcolor.value =     serialIn[3];
 
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    hsv2rgb_spectrum( spectrumcolor, leds[dot] );
  }
}

void setLedColor(int value){
  if (serialIn[0] == 0){
    ledAutoMode(value);
  }
  else if (serialIn[0] == 1){
    ledColorMode(value);
  }
  else if (serialIn[0] == 2){
    ledColorManual();
  }
  
  FastLED.show();
}

void loop() {

  // check serial buffer for input
  int test = Serial.read();

  // start byte received?
  if( test == SYNC_BYTE )
  {
    // yes, read data bytes
    Serial.readBytes( serialIn, 4 );

    /*Serial.print( serialIn[0] ); Serial.print( " " );
    Serial.print( serialIn[1] ); Serial.print( " " );
    Serial.print( serialIn[2] ); Serial.print( " " );
    Serial.print( serialIn[3] ); Serial.println();*/
  }
   
  //read kurbel value
  int value = analogRead(SENSOR_PIN);
  //smooth the value
  smooth(value);
  //minimum read is approx. 100, so subtract value
  value = average - 100;

  //remap to 1Byte -> 0-127
  float ltemp = (float)value / (1023-100) * 127;

  //Serial.print(ltemp);
  //Serial.print(' ');
  //Serial.println((int)(127 - ltemp));

  //led color
  setLedColor(127-ltemp);

  // time to update RasPi with current value?
  if ( micros() - lastSendTime > sendInterval )
  {
    // remember time
    lastSendTime = micros();
    
    // write start character
    Serial.print( ":" );
    
    // write value as ASCII and append line break
    Serial.println( (int) (127 - ltemp) );

    // wait for transmission to finish
    Serial.flush();
  }
}
