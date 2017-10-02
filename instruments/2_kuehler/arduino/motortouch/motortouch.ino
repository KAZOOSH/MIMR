#include <CapacitiveSensor.h>

/*
 * CapitiveSense Library Demo Sketch
 * Paul Badger 2008
 * 25mmHg 2017
 * Uses a high value resistor e.g. 10M between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 * MIMR: motortouch 3 objects
 */


CapacitiveSensor cs_A5 = CapacitiveSensor(A0,A5);        // pin A5 is sensor grünes Kabel
CapacitiveSensor cs_A4 = CapacitiveSensor(A0,A4);        // pin A4 is sensor blaues Kabel
CapacitiveSensor cs_A3 = CapacitiveSensor(A0,A3);        // pin A3 is sensor violettes Kabel
const int treshold = 150;

void setup()                    
{
   //cs_A3.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate
   //cs_A4.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate
   //cs_A5.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate
   Serial.begin(115200);
   pinMode(13,OUTPUT); // rotes Kabel   LED links oben
   pinMode(12,OUTPUT); // oranges Kabel LED links unten
   pinMode(11,OUTPUT); // gelbes Kabel  LED rechts
   digitalWrite(13, LOW); 
   digitalWrite(12, LOW);
   digitalWrite(12, LOW);
}

void loop()                    
{
    long start = millis();
    long totalA5 =  cs_A5.capacitiveSensor(10);
    long totalA4 =  cs_A4.capacitiveSensor(10);
    long totalA3 =  cs_A3.capacitiveSensor(10);

    Serial.print(millis() - start);        // check on performance in milliseconds
    Serial.print("\t\t");

    Serial.print(totalA3);                  // sensor violett
    digitalWrite(13, totalA3 > treshold);
    
    Serial.print(totalA4);                  // sensor blau
    Serial.print("\t");
    digitalWrite(12, totalA4 > treshold);
    
    Serial.println(totalA5);                // sensor grün

    delay(2);                               // limit data to serial port 
}
