/*
 * MIMR - KURBEL
 * KAZOOSH! - Brian Eschrich, Nick Schwarzenberg 2017
 * reads Values from the Kurbel, smooths them a little and sends it via serial to the pi
 */

#include <FastLED.h>

//LED control
#define NUM_LEDS 20
#define DATA_PIN 6

#define PI 3.14159265359

// dummy byte indicating start of Ableton data
#define SYNC_BYTE 255

// Arduino -> RasPi send interval (microseconds)
#define SEND_INTERVAL 20*1000L // 20 ms = 50 fps
unsigned long lastSendTime = 0;

// baud rate for USB communication with RasPi
#define BAUD_RATE 115200L


CRGB leds[NUM_LEDS];

byte serialIn[3] = {0,0,1}; //intensity, hue, isIdle

//kurbel control
#define GROUND_PIN A0
#define SENSOR_PIN A1
#define POWER_PIN A2


void setup() {
  //set pins
  pinMode(GROUND_PIN,OUTPUT);
  pinMode(POWER_PIN,OUTPUT);
  digitalWrite(POWER_PIN,HIGH);
  digitalWrite(GROUND_PIN,LOW);

  //init serial
  Serial.begin( BAUD_RATE );

  //init led
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}


// averaging function
int average( int value )
{
  static const int length = 128;
  static int buffer[length] = {0};
  static int index = 0;
  long output = 0;

  // replace value at current index
  buffer[index] = value;

  // advance write index
  index = ( index + 1 ) % length;

  // sum buffered values
  for ( int i = 0; i < length; i++ ) { output += buffer[i]; }

  // divide by length and return
  return output / length;
}


void setLedColor(int value){

  bool isRed = bitRead(serialIn[1],0);
  bool isGreen = bitRead(serialIn[1],1);
  bool isBlue = bitRead(serialIn[1],2);
  
  if(!isRed && !isGreen && !isBlue){
    isRed = true; isGreen = true; isBlue = true;
  }
  
  int brightness = max(value - serialIn[0],0);

  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    leds[dot].setRGB( isRed *brightness, isGreen *brightness, isBlue *brightness);
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
    Serial.readBytes( serialIn, 3 );
  }
//idle
  if(serialIn[2] == 1){
    doIdle();
  }
  else 
  {
     // read and invert current voltage
    int value = 1023 - analogRead(SENSOR_PIN);
  
    // smoothen input value
    int smoothed = average( value );
  
    // map sensor value to 0…127
    // (multiply with >127 to use full range)
    int mapped = min( (long)(smoothed) * (127+20) / 1024, 127 );
  
    // set LEDs
    setLedColor(mapped);
  
    // time to update RasPi with current value?
    if ( micros() - lastSendTime > SEND_INTERVAL )
    {
      // remember time
      lastSendTime = micros();
      
      // write start character
      Serial.print( ":" );
      
      // write value as ASCII and append line break
      Serial.println( mapped );
  
      // wait for transmission to finish
      Serial.flush();
    }
  }
   
 
}

void doIdle(){
  int pos = millis()%6000;
  pos = pos%6000;
  float p = pos/5000.0;

  float maxBrightness = 45;
  
  int value = sin(p*PI)*maxBrightness;
  if(p >1) value = 0;

  for(int c = 0; c<NUM_LEDS; c++){
      leds[c] = CRGB(value,value,value);

    
  }
  
  FastLED.show();
}
