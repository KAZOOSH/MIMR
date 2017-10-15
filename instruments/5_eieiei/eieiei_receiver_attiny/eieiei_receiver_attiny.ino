#include "FastLED.h"
#include "RCSwitch.h"

// How many leds in your strip?
#define NUM_LEDS 10
#define DATA_PIN 4
#define IR_PIN 3

// pin for pulsinf with transistor to keep powerbank active
#define PULSE_PIN 1

// counter for 
int count = 0;
int roundcount =0;

float min_v=9999;
float max_v=0;
float voltage =0;

int low =0;
RCSwitch mySwitch = RCSwitch();

// Define the array of leds
CRGB leds[NUM_LEDS];

CRGB currentcolor = CRGB::Blue;

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
    if(low){
      low = 0;
      roundcount = count;
      count = 0;
    }
    
  }else{
    if(!low){
      low = 1;
    }

    FastLED.show();
  }
  
  if(count > 0 && count < roundcount/10){
    leds[0] = currentcolor;
    leds[5] = currentcolor;
    FastLED.show();
  }
  if(count > roundcount/10 && count < roundcount*2/10){
    leds[1] = currentcolor;
    leds[6] = currentcolor;
    FastLED.show();
  }
    if(count > roundcount*2/10 && count < roundcount* 3/10){
    leds[2] = currentcolor;
    leds[7] = currentcolor;
    FastLED.show();
  }
    if(count > roundcount*3/10 && count < roundcount*4/10){
    leds[3] = currentcolor;
    leds[8] = currentcolor;
    FastLED.show();
  }
  if(count > roundcount*4/10 && count < roundcount*5/10){
    leds[4] = currentcolor;
    leds[9] = currentcolor;
    FastLED.show();
  }
  if(count > roundcount*5/10 && count < roundcount*6/10){
    leds[4] = CRGB::Black;
    leds[9] = CRGB::Black;
    FastLED.show();
  }
  if(count > roundcount*6/10 && count < roundcount*7/10){
    leds[3] = CRGB::Black;
    leds[8] = CRGB::Black;
    FastLED.show();
  }
  if(count > roundcount*7/10 && count < roundcount*8/10){
    leds[2] = CRGB::Black;
    leds[7] = CRGB::Black;
    FastLED.show();
  }
  if(count > roundcount*8/10 && count < roundcount*9/10){
    leds[1] = CRGB::Black;
    leds[6] = CRGB::Black;
    FastLED.show();
  }
   if(count > roundcount*9/10 && count < roundcount){
    leds[0] = CRGB::Black;
    leds[5] = CRGB::Black;
    FastLED.show();
  }
  
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
