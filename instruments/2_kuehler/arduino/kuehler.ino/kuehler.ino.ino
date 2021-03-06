/*Copyright (c) 2010 bildr community

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/


#include "FastLED.h"
#include "mpr121.h"

//LED control
#define NUM_LEDS 50
#define DATA_PIN 3
#define CLOCK_PIN 13

#define PI 3.14159265359

// Define the array of leds
CRGB leds[NUM_LEDS];
int nLedIndex[] = { 1,1,1,1,1,1,1,1,1,0,
                    0,0,0,0,0,0,0,4,4,4,
                    4,4,2,2,2,2,2,2,3,3,
                    3,3,3,3,3,3,3,3,3,3,
                    3,3,3,1,1,1,1,1,1,1};

//own variable to keep it simple
boolean touch[5];

//communication
// dummy byte indicating start of Ableton data
#define SYNC_BYTE 255

// Arduino -> RasPi send interval (microseconds)
#define SEND_INTERVAL 20*1000L // 20 ms = 50 fps
unsigned long lastSendTime = 0;

// baud rate for USB communication with RasPi
#define BAUD_RATE 115200L
byte serialIn[4] = {0,0,255,1}; //value, hue, saturation, isIdle

#include <Wire.h>

//variables for hello animation
unsigned long lastActivated = 0;
int lastState = 1;


int irqpin = 2;  // Digital 2
boolean touchStates[12]; //to keep track of the previous touch states

void setup(){
  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH); //enable pullup resistor
  
  Serial.begin(BAUD_RATE);
  Wire.begin();

  FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  mpr121_setup();

    for(int c = 0; c<NUM_LEDS; c++){
        leds[c] = CRGB(0,0,0);
    }
    FastLED.show();

  
}

void loop(){
  
  // check serial buffer for input
  int test = Serial.read();

  // start byte received?
  if( test == SYNC_BYTE )
  {
    // yes, read data bytes
    Serial.readBytes( serialIn, 4 );
  }


  if(serialIn[3] == 0 && lastState == 1){
      lastActivated = millis();
  }
  lastState = serialIn[3];

  //if not idle proceed normal
  if(serialIn[3] == 0){
    readTouchInputs();
    setLedColor();
  }
  else
  {
    doIdle();
  }
  
  
  
}


void readTouchInputs(){
  if(!checkInterrupt()){
    
    //read the touch state from the MPR121
    Wire.requestFrom(0x5A,2); 
    
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states

    
    for (int i=0; i < 12; i++){  // Check what electrodes were pressed
      if(touched & (1<<i)){
      
        if(touchStates[i] == 0){
          //pin i was just touched
          /*Serial.print("pin ");
          Serial.print(i);
          Serial.println(" was just touched");*/
         // leds[i]=CRGB::Red;
         if(i<=5) touch[i] = 1;
         sendTouchValues();
          
        }else if(touchStates[i] == 1){
          //pin i is still being touched
        }  
      
        touchStates[i] = 1;      
      }else{
        if(touchStates[i] == 1){
          /*Serial.print("pin ");
          Serial.print(i);
          Serial.println(" is no longer being touched");*/
          if(i<=5) touch[i] = 0;
          sendTouchValues();
          //leds[i]=CRGB::Black;
          
          //pin i is no longer being touched
       }
        
        touchStates[i] = 0;
      }
    
    }
    

    
  }
}

void setLedColor(){
  if(millis() - lastActivated <1000){
    float p = (millis() - lastActivated)/1000.0;

    float maxBrightness = 100;

    int value = 100;
    if (p<0.35) value = sin(p*2.857*PI*0.5)*maxBrightness;
    else if (p>0.5) value = sin(p*PI)*(maxBrightness-5)+5;
   

    for(int c = 0; c<NUM_LEDS; c++){
        leds[c] = CRGB(value,value,value);
    }
  }
  else{
      int hue = serialIn[1];
      int value = 255 - serialIn[0];
      int saturation = serialIn[2];

      CRGB color1 = CRGB(5,5,5);
      CRGB color2 = CHSV(hue, saturation, value);

      for(int c = 0; c<NUM_LEDS; c++){
        if(touchStates[nLedIndex[c]] == 0){
          leds[c] = color1;
        }
        else {
          leds[c] = color2;
        }
        
      }
    }
    FastLED.show();
}

  

void sendTouchValues(){
      // time to update RasPi with current value?
  if ( micros() - lastSendTime > SEND_INTERVAL )
  {
    // remember time
    lastSendTime = micros();
    
    // write start character
    Serial.print( ":" );
    
    for(int i=0; i<5; ++i){
    Serial.print(touch[i] == 0 ? 0 : 127);
    Serial.print(" ");
  }
  Serial.println();

    // wait for transmission to finish
    Serial.flush();
  }
  
  
}



void mpr121_setup(void){

  set_register(0x5A, ELE_CFG, 0x00); 
  
  // Section A - Controls filtering when data is > baseline.
  set_register(0x5A, MHD_R, 0x01);
  set_register(0x5A, NHD_R, 0x01);
  set_register(0x5A, NCL_R, 0x00);
  set_register(0x5A, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(0x5A, MHD_F, 0x01);
  set_register(0x5A, NHD_F, 0x01);
  set_register(0x5A, NCL_F, 0xFF);
  set_register(0x5A, FDL_F, 0x02);
  
  // Section C - Sets touch and release thresholds for each electrode
  set_register(0x5A, ELE0_T, TOU_THRESH);
  set_register(0x5A, ELE0_R, REL_THRESH);
 
  set_register(0x5A, ELE1_T, TOU_THRESH);
  set_register(0x5A, ELE1_R, REL_THRESH);
  
  set_register(0x5A, ELE2_T, TOU_THRESH);
  set_register(0x5A, ELE2_R, REL_THRESH);
  
  //set_register(0x5A, ELE3_T, TOU_THRESH);
  //set_register(0x5A, ELE3_R, REL_THRESH);

  set_register(0x5A, ELE3_T, 0x10);
  set_register(0x5A, ELE3_R, 0x10);
  
  
  set_register(0x5A, ELE4_T, TOU_THRESH);
  set_register(0x5A, ELE4_R, REL_THRESH);
  
  set_register(0x5A, ELE5_T, TOU_THRESH);
  set_register(0x5A, ELE5_R, REL_THRESH);
  
  set_register(0x5A, ELE6_T, TOU_THRESH);
  set_register(0x5A, ELE6_R, REL_THRESH);
  
  set_register(0x5A, ELE7_T, TOU_THRESH);
  set_register(0x5A, ELE7_R, REL_THRESH);
  
  set_register(0x5A, ELE8_T, TOU_THRESH);
  set_register(0x5A, ELE8_R, REL_THRESH);
  
  set_register(0x5A, ELE9_T, TOU_THRESH);
  set_register(0x5A, ELE9_R, REL_THRESH);
  
  set_register(0x5A, ELE10_T, TOU_THRESH);
  set_register(0x5A, ELE10_R, REL_THRESH);
  
  set_register(0x5A, ELE11_T, TOU_THRESH);
  set_register(0x5A, ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(0x5A, FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(0x5A, ATO_CFG0, 0x0B);
  set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(0x5A, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V
  
  set_register(0x5A, ELE_CFG, 0x0C);
  
}


boolean checkInterrupt(void){
  return digitalRead(irqpin);
}


void set_register(int address, unsigned char r, unsigned char v){
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}

void doIdle(){
  int pos = millis()%35000;
  int indexLeds = pos/7;
  pos = pos%7000;
  indexLeds/=1000;
  float p = pos/5000.0;

  float maxBrightness = 45;
  
  int value = sin(p*PI)*maxBrightness;
  if(p >1) value = 0;
  
  CRGB color1 = CRGB(value,value,value);
  CRGB color2 = CRGB::Black;

  for(int c = 0; c<NUM_LEDS; c++){
    if(nLedIndex[c] == indexLeds){
      leds[c] = color1;
    }
    else {
      leds[c] = color2;
    }
    
  }
  
  FastLED.show();
}
