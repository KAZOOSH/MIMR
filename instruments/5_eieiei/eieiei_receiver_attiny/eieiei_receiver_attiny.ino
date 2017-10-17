#include "FastLED.h"
#include "RCSwitch.h"

// How many leds in your strip?
#define NUM_LEDS 17
#define DATA_PIN 4
#define IR_PIN 3

// pin for pulsinf with transistor to keep powerbank active
#define PULSE_PIN 1

// counter for 
int count = 0;
int roundcount =1;

float min_v=9999;
float max_v=0;
float voltage =0;

int low =0;
int i = 0;
int j = 0;
RCSwitch mySwitch = RCSwitch();

// Define the array of leds
CRGB leds[NUM_LEDS];

CRGB currentcolor = CRGB(100,0,150);

void setup() { 
  
  mySwitch.enableReceive(0); // attiny pin 2 !
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
 
  leds[0] = currentcolor;
  //leds[4] = CRGB::Red;

  FastLED.show();
        
  pinMode(PULSE_PIN, OUTPUT); 
  digitalWrite(PULSE_PIN, HIGH);
}

void loop() { 
  count++;
  
  // read ir
  voltage = analogRead(IR_PIN);
  
  if(voltage < min_v){
    min_v= voltage;
  }
  if(voltage > max_v){
    max_v= voltage;
  }
  
  // check for voltage to 
  if(voltage > (max_v-min_v)/2){
    leds[16] = CRGB::Red;
    if(low){
      low = 0;
      roundcount = count;
      count = 0;
    }
    
  }else{
    if(!low){
      low = 1;
    }
    leds[16] = CRGB::Green;
  }
  
  int currentpos = ((8 * count)/roundcount)-1;
  

  if(currentpos>4) {
    currentpos = 4 -(currentpos-4); 
  }
  if(currentpos==4){
    currentpos=3;
  }
  for(i = 0; i<4; i++){
      for(j = 0; j<4; j++){
        if(i == currentpos){
          leds[(j*4)+i] = currentcolor;
        } else{
          leds[(j*4)+i] = CRGB(6,20,6);
        }
      }
    }
     FastLED.show();

  
  //pulsing
  
//  if(count == 990){
//    //leds[4] = CRGB::Blue;
//    //FastLED.show();
//    digitalWrite(PULSE_PIN, LOW);
//  }
//  if(count == 1000){
//    //leds[4] = CRGB::Black;
//    //FastLED.show();
//    digitalWrite(PULSE_PIN, HIGH);
//    max_v = max_v * 0.99;
//    count=0;
//  }
  
  
  // wireless color setting
  if (mySwitch.available()) {
   double value = (double)mySwitch.getReceivedValue();
   
   int r = ((int)(value/10000)); 
   int g = ((int)((value-((double)r)*10000)/100)); 
   int b = ((int)((value-((double)r)*10000-((double)g)*100))); 
   
   currentcolor = CRGB(r*10,g*10,b*10);
    mySwitch.resetAvailable();
  }
  

}
