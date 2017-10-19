/*
 * MIMR - Bassfahrer
 * KAZOOSH! 2017
 */

#include <FastPin.h>
#include <AltSoftSerial.h>

// driver pins for meter and light
FastPin BacklightPin1(  9, OUTPUT );
FastPin BacklightPin2( 10, OUTPUT );
FastPin MeterPin1( 11, OUTPUT );
FastPin MeterPin2( 12, OUTPUT );
FastPin LampPin( 13, OUTPUT );

// sensor threshold for switching on lamp (0..255)
#define LAMP_THRESHOLD 180

// PWM switching time (milliseconds)
#define PWM_SWITCHING_TIME 10*1000L // 10 ms
#define PWM_STEPS 20 // 20 steps * 10 ms = 200 ms cycle length

// serial connection to sensor controller
AltSoftSerial SensorSerial;

// dummy byte indicating start of MIDI data
#define SYNC_BYTE 255

// Arduino -> RasPi send interval (microseconds)
#define SEND_INTERVAL 50*1000L // 50 ms = 20 S/s

// baud rate for USB communication with RasPi
#define BAUD_RATE 115200L


void setup()
{
  // start USB connection to RasPi
  Serial.begin( BAUD_RATE );

  // flash lamp to say hello
  LampPin.write( HIGH );
  delay( 500 );

  // start serial connection to sensor controller
  SensorSerial.begin( 9600 );
}


void driveOutputs( int meter, int light )
{
  static unsigned long lastCycleTime = 0;
  static int pwmCounter = 0;

  // time to update?
  if ( micros() - lastCycleTime > PWM_SWITCHING_TIME )
  {
    // quantize duty cycles
    char digitalMeter = pwmCounter <= ( meter * PWM_STEPS / 256 ) ? HIGH : LOW;
    char digitalLight = pwmCounter <= ( light * PWM_STEPS / 256 ) ? HIGH : LOW;

    // drive meter pins and lamp pin
    MeterPin1.write( digitalMeter );
    MeterPin2.write( digitalMeter );

    // drive backlight pins
    BacklightPin1.write( digitalLight );
    BacklightPin2.write( digitalLight );

    // roll counter over
    pwmCounter = ( pwmCounter + 1 ) % PWM_STEPS;
  }
}


void loop()
{
  static unsigned char serialIn[1];
  static int sensorValue = 0;
  static unsigned long lastSendTime = 0;
  
  // check serial buffer for input
  int test = Serial.read();

  // start byte received?
  if( test == SYNC_BYTE )
  {
    // yes, read data bytes
    Serial.readBytes( serialIn, 1 );
  }

  // try to read latest data from sensor
  while ( SensorSerial.available() > 0 )
  {
    // update cached value (0..255)
    sensorValue = SensorSerial.read();

    // HACK: treat 255 as 0 (bug in sensor)
    if ( sensorValue == 255 ) { sensorValue = 0; }
  }

  // visualize data: show sensor value with meter, MIDI data dims light
  driveOutputs( sensorValue+20, 255-serialIn[0] );  

  // switch lamp on for high values
  LampPin.write( sensorValue > LAMP_THRESHOLD ? HIGH : LOW );

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
