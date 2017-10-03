#include <CapacitiveSensor.h>

/*
 * MIMR - KUEHLER
 * KAZOOSH! - 25mmHg 2017
 * using CapitiveSense Library Demo Sketch by Paul Badger 2008
 * 
 * Uses a high value resistor e.g. 10M between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 * MIMR: motortouch 3 objects
 */


CapacitiveSensor cs_A5 = CapacitiveSensor(A0,A5);        // pin A5 is sensor grünes Kabel
CapacitiveSensor cs_A4 = CapacitiveSensor(A0,A4);        // pin A4 is sensor blaues Kabel
CapacitiveSensor cs_A3 = CapacitiveSensor(A0,A3);        // pin A3 is sensor violettes Kabel
const int treshold = 100;

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
   digitalWrite(11, LOW);
}

void loop()                    
{
    //long start = millis();
    long totalA5 =  cs_A5.capacitiveSensor(10);
    long totalA4 =  cs_A4.capacitiveSensor(10);
    long totalA3 =  cs_A3.capacitiveSensor(10);

    byte value = 0;
    
    // sensor violett
    if (totalA3 > treshold) ++value;
      
    digitalWrite(13, totalA3 > treshold);
    
    // sensor blau
    if (totalA4 > treshold) value+=2; 
      
    digitalWrite(12, totalA4 > treshold);
    
    // sensor grün
    if (totalA5 > treshold) value+=4; 
    digitalWrite(11, totalA5 > treshold);

/*
    if (totalA3 > treshold) Serial.print(totalA3);
    Serial.print("\t");
    if (totalA4 > treshold) Serial.print(totalA4);
    Serial.print("\t");
    if (totalA5 > treshold) Serial.print(totalA5);
  Serial.println("");*/
    //Serial.println((int)value);

  Serial.print(totalA3);
  Serial.print(" ");
  Serial.print(totalA4);
  Serial.print(" ");
  Serial.println(totalA5);

    delay(10);                               // limit data to serial port 
}
