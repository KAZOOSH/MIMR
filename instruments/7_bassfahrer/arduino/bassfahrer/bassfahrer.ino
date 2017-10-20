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

// sensor threshold for flickering (0..255)
#define FLICKER_THRESHOLD 180

// idle fade duration (milliseconds)
#define FADE_DURATION 4000

// sensor reading noise range
#define SENSOR_NOISE 1

// PWM configuration
#define PWM_STEPS 24

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
  static int pwmCounter = 0;

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


void idleAnimation()
{
  static unsigned long lastCycleTime = 0;

  // cycle progress
  int progress = (unsigned long)( millis() - lastCycleTime ) * 255 / FADE_DURATION;

  // reset
  if ( progress > 255 )
  {
    progress = 0;
    lastCycleTime = millis();
  }

  // ramp up
  if ( progress < 127 ) { driveOutputs( 0, progress ); }

  // ramp down
  if ( progress >= 127 ) { driveOutputs( 0, (255-progress) ); }

  // keep lamp off
  LampPin.write( LOW );
}


void loop()
{
  static unsigned char serialIn[1];
  static int sensorValue = 0;
  static unsigned long lastSendTime = 0;
  static char idleState = 0;
  
  // check serial buffer for input
  int test = Serial.read();

  // start byte received?
  if( test == SYNC_BYTE )
  {
    // yes, read data bytes
    Serial.readBytes( serialIn, 2 );

    // just leaving idle mode?
    if ( serialIn[0] == 0 && idleState == 1 )
    {
      // flash to say hello
      LampPin.write( HIGH );
      MeterPin1.write( HIGH );
      MeterPin2.write( HIGH );
      BacklightPin1.write( HIGH );
      BacklightPin2.write( HIGH );
      delay( 500 );
    }

    // save idle state
    idleState = serialIn[0];
  }

  if ( idleState == 1 )
  {
    // run teaser animation, nothing else
    idleAnimation();
    return;
  }

  // try to read latest data from sensor
  while ( SensorSerial.available() > 0 )
  {
    // update cached value (0..255)
    sensorValue = SensorSerial.read();

    // HACK: treat 255 as 0 (bug in sensor)
    if ( sensorValue == 255 ) { sensorValue = 0; }

    // HACK: coerce 1 to 0 (to reduce idle noise)
    if ( sensorValue <= SENSOR_NOISE ) { sensorValue = 0; }
  }

  // visualize data: show sensor value with meter, MIDI data dims light
  driveOutputs( sensorValue+20, 255-serialIn[1] );

  // flicker everything for high values
  if ( sensorValue > FLICKER_THRESHOLD )
  {
    LampPin.write( LOW ); delay( 5 ); LampPin.write( HIGH );
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
