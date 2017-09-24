#include "FastLED.h"
#include "RCSwitch.h"

// How many leds in your strip?
#define NUM_LEDS 1

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 4
#define CLOCK_PIN 13

RCSwitch mySwitch = RCSwitch();

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() { 
  mySwitch.enableReceive(0); // Receiver on interrupt 0 => that is pin #2
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop() { 
  if (mySwitch.available()) {
    int value = mySwitch.getReceivedValue();
    
    if(value == 11){
        // Turn the LED on, then pause
        leds[0] = CRGB::Red;
        FastLED.show();
        
    }
    if(value == 22){
        // Turn the LED on, then pause
        leds[0] = CRGB::Blue;
        FastLED.show();
    }
    delay(50);
    // Now turn the LED off, then pause
    leds[0] = CRGB::Black;
    FastLED.show();
    mySwitch.resetAvailable();
  }
 
}