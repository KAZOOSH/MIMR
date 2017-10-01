/*
 * MIMR - Bassfahrer
 * KAZOOSH! 2017
 */

#include <SoftwareSerial.h>

// serial connection to sensor controller
SoftwareSerial SensorSerial( 7, 8 );

// dummy byte indicating start of MIDI data
#define SYNC_BYTE 255

// Arduino -> RasPi send interval (microseconds)
#define SEND_INTERVAL 50*1000L // 50 ms = 20 fps
unsigned long lastSendTime = 0;

// baud rate for USB communication with RasPi
#define BAUD_RATE 115200L


void setup()
{
  // start USB connection to RasPi
  Serial.begin( BAUD_RATE );

  // start serial connection to sensor controller
  SensorSerial.begin( 9600 );
}


void loop()
{
  static unsigned char serialIn[4];
  static int sensorValue = 0;
  
  // check serial buffer for input
  int test = Serial.read();

  // start byte received?
  if( test == SYNC_BYTE )
  {
    // yes, read data bytes
    Serial.readBytes( serialIn, 4 );
  }

  // try to read latest data from sensor
  while ( SensorSerial.peek() >= 0 )
  {
    // update cached value
    sensorValue = SensorSerial.read();
  }

  // time to update RasPi with current value?
  if ( micros() - lastSendTime > SEND_INTERVAL )
  {
    // remember time
    lastSendTime = micros();
    
    // write start character
    Serial.print( ":" );
    
    // write value as ASCII and append line break
    Serial.println( sensorValue );

    // wait for transmission to finish
    Serial.flush();
  }
}
