#include "FastLED.h"
#include "RCSwitch.h"

// How many leds in your strip?
#define NUM_LEDS 1
#define DATA_PIN 4
#define IR_PIN 3

// pin for pulsinf with transistor to keep powerbank active
#define PULSE_PIN 1

int count = 0;
int lastValue =0;

float min_v=9999;
float max_v=0;
float voltage =0;

RCSwitch mySwitch = RCSwitch();

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() { 
  mySwitch.enableReceive(0); // Receiver on interrupt 0 => that is pin #2
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  
  leds[0] = CRGB::Red;
  FastLED.show();
        
  pinMode(PULSE_PIN, OUTPUT); 
  digitalWrite(PULSE_PIN, HIGH);
}

void loop() { 
  
  // read ir
  voltage = analogRead(IR_PIN) * (5.0 / 1023.0);
  
  if(voltage < min_v){
    min_v= voltage;
  }
  if(voltage > max_v){
    max_v= voltage;
  }
  max_v = max_v * 0.999;
  if(voltage > (max_v-min_v)/2){
    leds[0] = CRGB::Green;
    FastLED.show();
  }else{
    leds[0] = CRGB::Black;
    FastLED.show();
  }
  
  //pulsing
  count++;
  if(count == 9900){
    digitalWrite(PULSE_PIN, LOW);
  }
  if(count == 10000){
    digitalWrite(PULSE_PIN, HIGH);
    count=0;
  }
  
  
  // wireless
  if (mySwitch.available()) {
    int value = mySwitch.getReceivedValue();

    if(value == 11) {
        // Turn the LED on, then pause
        leds[0] = CRGB::Red;
        FastLED.show();
    }
    
    if(value == 22){
        // Turn the LED on, then pause
        leds[0] = CRGB::Green;
        FastLED.show();
    }
    
    if(value == 33){
        // Turn the LED on, then pause
        leds[0] = CRGB::Blue;
        FastLED.show();
    }
    
    if(value == 44){
        // Turn the LED on, then pause
        leds[0] = CRGB::Black;
        FastLED.show();
    }
    mySwitch.resetAvailable();
  }

}
