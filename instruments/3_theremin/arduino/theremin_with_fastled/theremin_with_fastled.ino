/* Theremin Test
 *
 * Therremin with TTL Oscillator 4MHz
 * Timer1 for freauency measurement
 * Timer2 for gate time
 * connect Oscillator on digital pin 5
 * connect Speaker with 1K Resistor in series on pin 8

 * KHM 2008 /  Martin Nawrath
 * Kunsthochschule fuer Medien Koeln
 * Academy of Media Arts Cologne

 */
#include "Arduino.h"
#include "FastLED.h"

#define NUM_LEDS 39
#define DATA_PIN 11
CRGB leds[NUM_LEDS];

// dummy byte indicating start of Ableton data
#define SYNC_BYTE 255

// mode,hue,sat, brightness
byte serialIn[4] = {0,0,0,0};

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

//! Macro that clears all Timer/Counter1 interrupt flags.
#define CLEAR_ALL_TIMER1_INT_FLAGS    (TIFR1 = TIFR1)

int pinLed = 13;                 // LED connected to digital pin 13
int pinFreq = 5;
int pinEmitter = 7;
int pin1KOhm = 2;

void setup()
{
  pinMode(pinLed, OUTPUT);      // sets the digital pin as output
  pinMode(pinFreq, INPUT);
  pinMode(pinEmitter, OUTPUT);
  pinMode(pin1KOhm, OUTPUT);
  digitalWrite(pin1KOhm, HIGH);
  pinMode(8, OUTPUT);

   FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  Serial.begin(57600);        // connect to the serial port

  // hardware counter setup ( refer atmega168.pdf chapter 16-bit counter1)
  TCCR1A=0;                   // reset timer/counter1 control register A
  TCCR1B=0;                   // reset timer/counter1 control register A
  TCNT1=0;                    // counter value = 0
  // set timer/counter1 hardware as counter , counts events on pin T1 ( arduino pin 5)
  // normal mode, wgm10 .. wgm13 = 0
  sbi (TCCR1B ,CS10);         // External clock source on T1 pin. Clock on rising edge.
  sbi (TCCR1B ,CS11);
  sbi (TCCR1B ,CS12);

  // timer2 setup / is used for frequency measurement gatetime generation
  // timer 2 presaler set to 256 / timer 2 clock = 16Mhz / 256 = 62500 Hz
  cbi (TCCR2B ,CS20);
  sbi (TCCR2B ,CS21);
  sbi (TCCR2B ,CS22);

  //set timer2 to CTC Mode
  cbi (TCCR2A ,WGM20);
  sbi (TCCR2A ,WGM21);
  cbi (TCCR2B ,WGM22);
  OCR2A = 124;                  // CTC at top of OCR2A / timer2 interrupt when coun value reaches OCR2A value

  // interrupt control

  sbi (TIMSK2,OCIE2A);          // enable Timer2 Interrupt

}

// steps from 0 to 1000 for binning
float bin1 = 1;
float bin2 = 3;
float bin3 = 10;
float bin4 = 50;
float bin5 = 100;
float bin6 = 1000;

int minval = 30000;
int maxval = 0;
float range = 0;

volatile byte i_tics;
volatile byte f_ready ;
volatile byte mlt ;
unsigned int ww;

int cal;
int cal_max;

char st1[32];
long freq_in;
long freq_zero;
long freq_cal;

unsigned int dds;
int tune;

int cnt=0;



void loop()
{
  // check serial buffer for input
  int test = Serial.read();

  // start byte received?
  if( test == SYNC_BYTE )
  {
    // yes, read data bytes
    Serial.readBytes( serialIn, 4 );

    /*Serial.print( serialIn[0] ); Serial.print( " " );
    Serial.print( serialIn[1] ); Serial.print( " " );
    Serial.print( serialIn[2] ); Serial.print( " " );
    Serial.print( serialIn[3] ); Serial.println();*/
  }
  
  cnt++;
  // add=analogRead(0);

  f_meter_start();

  tune=tune+1;
  while (f_ready==0) {            // wait for period length end (100ms) by interrupt
    PORTB=((dds+=tune) >> 15);    // kind of DDS tonegenerator / connect speaker to portb.0 = arduino pin8
  }
 tune = freq_in-freq_zero;
 // use the tune value here for your own purposes like control of servos, midi etc.

  // startup
  if (cnt==10) {
    freq_zero=freq_in;
    freq_cal=freq_in;
    cal_max=0;
    //Serial.print("** START **");
  }

  // autocalibration
  if (cnt % 20 == 0) {   // try autocalibrate after n cycles
    //Serial.print("*");
    if (cal_max <= 2) {
      freq_zero=freq_in;
      //Serial.print(" calibration");
    }
    freq_cal=freq_in;
    cal_max=0;
    //Serial.println("");
  }
  cal = freq_in-freq_cal;
  if ( cal < 0) cal*=-1;  // absolute value
  if (cal > cal_max) cal_max=cal;

  digitalWrite(pinLed,1);  // let LED blink
  
  //Serial.print(cnt);
  //Serial.print("  "); 

  if ( tune < 0) tune*=-1;  // absolute value
  sprintf(st1, " %04d",tune);
  
  //Serial.print(tune);
  
  if(tune < minval){
    minval = tune;
  }
  if(tune > maxval){
    maxval = tune;
  }
  range = maxval - minval;
/*
  if(tune < ( minval + (range * (bin1/1000 )))){
    Serial.print(0);
    setColor(10);
  }
  else if(tune < (  minval + (range * (bin2/1000 )))){
    Serial.print(1);
    setColor(30);
  }
  else if(tune < (  minval + (range * (bin3/1000 )))){
    Serial.print(2);
    setColor(90);
  }
  else if(tune < (  minval + (range * (bin4/1000 )))){
    Serial.print(3);
    setColor(140);
    
  }
  else if(tune < (  minval + (range * (bin5/1000 )))){
    Serial.print(4);
    setColor(180);
  }
  else {
    Serial.print(5);
    setColor(255);
  }
*/
  Serial.print(tune);
  Serial.println("");
  CRGB color = CRGB(tune/5,tune/5,tune/5); 

  for( int i = 0; i < NUM_LEDS; i++) {
        leds[i]  = color;
    }
    FastLED.show();
  
  
  // check percentual bins !!! 
  // wich val is cuurent bin!
  
  
  
  //Serial.print("  "); 

  //Serial.print(freq_in);
  //Serial.print("  ");
/*
  Serial.print(freq_zero);
  Serial.print("  ");
  Serial.print(cal_max);
*/
  digitalWrite(pinLed,0);
}

void setColor( uint8_t brightness)
{
  CHSV spectrumcolor;

    spectrumcolor.hue = serialIn[1]*2;
    spectrumcolor.saturation = serialIn[2]*2;
    if (serialIn[0] == 1){
      spectrumcolor.value = serialIn[3]*2;
    }
    else{
      spectrumcolor.value = brightness;
    }
    
  
    for( int i = 0; i < NUM_LEDS; i++) {
        hsv2rgb_spectrum( spectrumcolor, leds[i] );
    }
    FastLED.show();
}


//******************************************************************
void f_meter_start() {
  f_ready=0;                      // reset period measure flag
  i_tics=0;                        // reset interrupt counter
  sbi (GTCCR,PSRASY);              // reset presacler counting
  TCNT2=0;                         // timer2=0
  TCNT1=0;                         // Counter1 = 0
  cbi (TIMSK0,TOIE0);              // dissable Timer0 again // millis and delay
  sbi (TIMSK2,OCIE2A);             // enable Timer2 Interrupt
  TCCR1B = TCCR1B | 7;             //  Counter Clock source = pin T1 , start counting now
}

//******************************************************************
// Timer2 Interrupt Service is invoked by hardware Timer2 every 2ms = 500 Hz
//  16Mhz / 256 / 125 / 500 Hz
// here the gatetime generation for freq. measurement takes place: 

ISR(TIMER2_COMPA_vect) {

  if (i_tics==50) {         // multiple 2ms = gate time = 100 ms
                            // end of gate time, measurement ready
    TCCR1B = TCCR1B & ~7;   // Gate Off  / Counter T1 stopped
    cbi (TIMSK2,OCIE2A);    // disable Timer2 Interrupt
    sbi (TIMSK0,TOIE0);     // ensable Timer0 again // millis and delay
    f_ready=1;              // set global flag for end count period

                            // calculate now frequeny value
    freq_in=0x10000 * mlt;  // mukt #ovverflows by 65636
    freq_in += TCNT1;       // add counter1 value
    mlt=0;

  }
  i_tics++;                 // count number of interrupt events
  if (TIFR1 & 1) {          // if Timer/Counter 1 overflow flag
    mlt++;                  // count number of Counter1 overflows
    sbi(TIFR1,TOV1);        // clear Timer/Counter 1 overflow flag
  }

}
