/*
   Timer1 for freauency measurement
   Timer2 for gate time
   connect Oscillator on digital pin 5
   KHM 2008 /  Martin Nawrath
   Kunsthochschule fuer Medien Koeln
   Academy of Media Arts Cologne

*/
#include "Arduino.h"
#include "FastLED.h"

#define NUM_LEDS 39
#define DATA_PIN 11
CRGB leds[NUM_LEDS];

#define PI 3.14159265359


// dummy byte indicating start of Ableton data
#define SYNC_BYTE 255

// Arduino -> RasPi send interval (microseconds)
#define SEND_INTERVAL 20*1000L // 20 ms = 50 fps
unsigned long lastSendTime = 0;

// baud rate for USB communication with RasPi
#define BAUD_RATE 115200L

// intensity,color,isIdle
byte serialIn[3] = {0, 0, 0};

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

//! Macro that clears all Timer/Counter1 interrupt flags.
#define CLEAR_ALL_TIMER1_INT_FLAGS    (TIFR1 = TIFR1)

int pinLed = 13;
int pinFreq = 5;
int pinEmitter = 7;
int pin1KOhm = 2;


int value = 0;

void setup()
{
  pinMode(pinLed, OUTPUT);
  pinMode(pinFreq, INPUT);
  pinMode(pinEmitter, OUTPUT);
  pinMode(pin1KOhm, OUTPUT);
  digitalWrite(pin1KOhm, HIGH);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  Serial.begin(BAUD_RATE);        // connect to the serial port

  // hardware counter setup ( refer atmega168.pdf chapter 16-bit counter1)
  TCCR1A = 0;                 // reset timer/counter1 control register A
  TCCR1B = 0;                 // reset timer/counter1 control register A
  TCNT1 = 0;                  // counter value = 0
  // set timer/counter1 hardware as counter , counts events on pin T1 ( arduino pin 5)
  // normal mode, wgm10 .. wgm13 = 0
  sbi (TCCR1B , CS10);        // External clock source on T1 pin. Clock on rising edge.
  sbi (TCCR1B , CS11);
  sbi (TCCR1B , CS12);

  // timer2 setup / is used for frequency measurement gatetime generation
  // timer 2 presaler set to 256 / timer 2 clock = 16Mhz / 256 = 62500 Hz
  cbi (TCCR2B , CS20);
  sbi (TCCR2B , CS21);
  sbi (TCCR2B , CS22);

  //set timer2 to CTC Mode
  cbi (TCCR2A , WGM20);
  sbi (TCCR2A , WGM21);
  cbi (TCCR2B , WGM22);
  OCR2A = 124;                  // CTC at top of OCR2A / timer2 interrupt when coun value reaches OCR2A value

  // interrupt control
  sbi (TIMSK2, OCIE2A);         // enable Timer2 Interrupt
}

unsigned int minval = 30;

volatile byte i_tics;
volatile byte f_ready ;
volatile byte mlt ;
unsigned int ww;

char st1[32];
long freq_in;
int tune;

void loop()
{
  // check serial buffer for input
  int test = Serial.read();

  // start byte received?
  if ( test == SYNC_BYTE )
  {
    // yes, read data bytes
    Serial.readBytes( serialIn, 3 );
  }

  //if not idle proceed normal
  if (serialIn[2] == 0) {
    f_meter_start();
    while (f_ready == 0); //warte! INT2
    tune = freq_in;

    digitalWrite(pinLed, 1); // let LED blink
    
    if ( tune < 0) tune *= -1;

    /*  WURZEL ZIEHEN     */
    int i = 0;
    while (i * i <= tune)
      i++;
    value = i;
    value = 2*average(value);
    value = value-7;
    if(value < 0)value = 0;
    
    

    Serial.print(tune);
    Serial.print(" ");
    Serial.print(value);
    Serial.println(" ");

    setColor(value);
    //sendValue();
    digitalWrite(pinLed, 0);
  }
  else
  {
    doIdle();
  }
}

void sendValue() {
  // time to update RasPi with current value?
  if ( micros() - lastSendTime > SEND_INTERVAL )
  {
    // remember time
    lastSendTime = micros();

    // write start character
    Serial.print( ":" );

    // write value as ASCII and append line break
    Serial.println( value );

    // wait for transmission to finish
    Serial.flush();
  }
}

// averaging function
int average( int value )
{
  static const int length = 4;
  static int buffer[length] = {0};
  static int index = 0;
  long output = 0;

  // replace value at current index
  buffer[index] = value;

  // advance write index
  index = ( index + 1 ) % length;

  // sum buffered values
  for ( int i = 0; i < length; i++ ) {
    output += buffer[i];
  }

  // divide by length and return
  return output / length;
}

//******************************************************************

void setColor( uint8_t brightness)
{
  CHSV spectrumcolor;

  spectrumcolor.hue = serialIn[1] * 2;
  spectrumcolor.saturation = serialIn[2] * 2;
  if (serialIn[0] == 1) {
    spectrumcolor.value = serialIn[3] * 2;
  }
  else {
    spectrumcolor.value = brightness;
  }


  for ( int i = 0; i < NUM_LEDS; i++) {
    hsv2rgb_spectrum( spectrumcolor, leds[i] );
  }
  FastLED.show();
}

void f_meter_start() {
  f_ready = 0;                    // reset period measure flag
  i_tics = 0;                      // reset interrupt counter
  sbi (GTCCR, PSRASY);             // reset presacler counting
  TCNT2 = 0;                       // timer2=0
  TCNT1 = 0;                       // Counter1 = 0
  cbi (TIMSK0, TOIE0);             // dissable Timer0 again // millis and delay
  sbi (TIMSK2, OCIE2A);            // enable Timer2 Interrupt
  TCCR1B = TCCR1B | 7;             //  Counter Clock source = pin T1 , start counting now
}

//******************************************************************
// Timer2 Interrupt Service is invoked by hardware Timer2 every 2ms = 500 Hz
//  16Mhz / 256 / 125 / 500 Hz
// here the gatetime generation for freq. measurement takes place:

ISR(TIMER2_COMPA_vect) {

  if (i_tics == 50) {       // multiple 2ms = gate time = 100 ms
    // end of gate time, measurement ready
    TCCR1B = TCCR1B & ~7;   // Gate Off  / Counter T1 stopped
    cbi (TIMSK2, OCIE2A);   // disable Timer2 Interrupt
    sbi (TIMSK0, TOIE0);    // ensable Timer0 again // millis and delay
    f_ready = 1;            // set global flag for end count period

    // calculate now frequeny value
    freq_in = 0x10000 * mlt; // mukt #ovverflows by 65636
    freq_in += TCNT1;       // add counter1 value
    mlt = 0;

  }
  i_tics++;                 // count number of interrupt events
  if (TIFR1 & 1) {          // if Timer/Counter 1 overflow flag
    mlt++;                  // count number of Counter1 overflows
    sbi(TIFR1, TOV1);       // clear Timer/Counter 1 overflow flag
  }
}

void doIdle() {
  int pos = millis() % 6000;
  pos = pos % 6000;
  float p = pos / 5000.0;

  float maxBrightness = 45;

  int value = sin(p * PI) * maxBrightness;
  if (p > 1) value = 0;

  for (int c = 0; c < NUM_LEDS; c++) {
    leds[c] = CRGB(value, value, value);


  }

  FastLED.show();
}
