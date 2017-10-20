#define NUMBER_OF_SHIFT_CHIPS   3
#define DATA_WIDTH NUMBER_OF_SHIFT_CHIPS * 8
#define PULSE_WIDTH_USEC 5
#define BYTES_VAL_T unsigned long

#define PI 3.14159265359

const int ploadPin  = 8; //blue
const int dataPin   = 7; //yellow
const int clockPin  = 4; //green

const int redLedPin = 5;   //R + red
const int yellowLedPin = 6 ; //R + yellow

const int supplyPwmPin = 9; //Supply PWM pin
const int supplyAdcPin = 1; //Feedback Input

const float supplyVoltage = 13.8; 	//Important: Always set DC input Voltage of Supply in Volt !!!
static float SupplySetpoint = 0;	//output voltage

const int bulbPins[] = {10,11,3};
const int bulbMinD[] = {17,10,10};
#define nBulbBrightnessValues (sizeof(bulbBrightness) / sizeof(byte))
const int bulbAnimationTime = 4500;
const int maxBrightness = 70;
unsigned long controlValueChanged = 0;

byte serialIn[2] = {0,1}; //intensity, isIdle

//communication to serial
// dummy byte indicating start of Ableton data
#define SYNC_BYTE 255

// baud rate for USB communication with RasPi
#define BAUD_RATE 115200L


//variables for hello animation
unsigned long lastActivated = 0;
int lastState = 1;



BYTES_VAL_T pinValues;
  BYTES_VAL_T oldPinValues;
  byte sw,swOld;

  

BYTES_VAL_T read_shift_regs() {
    long bitVal;
    BYTES_VAL_T bytesVal = 0;

    digitalWrite(ploadPin, LOW);
    delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(ploadPin, HIGH);
    
    for(int i = 0; i < DATA_WIDTH; i++)
    {
        bitVal = digitalRead(dataPin);
        bytesVal |= (bitVal << ((DATA_WIDTH-1) - i));

        digitalWrite(clockPin, HIGH);
        delayMicroseconds(PULSE_WIDTH_USEC);
        digitalWrite(clockPin, LOW);
    }

    return(bytesVal);
}

inline byte registerValuteToSwitchValue(BYTES_VAL_T regValue) {

  for(int i = 0; i < DATA_WIDTH; i++) {
    if(!((regValue >> i) & 1)) return i;
  }
  return 0;
}


inline void SupplySetVoltage(int voltage) {
    SupplySetpoint = voltage;
    if(SupplySetpoint > 65.0) SupplySetpoint = 65.0;
}
inline void SupplyFeedbackLoop() {

  const float quantum = (5.0 / 1023) * 20.5; // U / bits * voltage divider
  
  float feedback;
  float error;
  
  static byte op=0;
  byte duty;

  
  feedback = analogRead(supplyAdcPin) * quantum - supplyVoltage;
  error = SupplySetpoint - feedback;

  if(SupplySetpoint < 3.0) {
    //min voltage
    OCR1A = 255;
    return;
  }
  
  if(error > 0.0) {
    op++;
  } else if(error < 0) {
    op--;
  }
  
  duty = 255 - op;      //invert
  if(duty < 9) duty = 9;  //safty
  OCR1A = duty;     //set timer1 register 
}



inline void ledFlicker() {
  const byte flicker[] = {180, 30, 89, 23, 255, 200, 90, 150, 60, 230, 180, 45, 90};
  #define flickerCount (sizeof(flicker) / sizeof(byte))
  
  static byte co=0;
  if(co >= flickerCount) co = 0;
  analogWrite(redLedPin, flicker[co]);
  analogWrite(yellowLedPin, flicker[co]);
  co++;
}


void setBulbBrightness(){

  //hello animation
  if(millis() - lastActivated <1500){
    float p = (millis() - lastActivated)/1500.0;

    float maxBrightness = 120;

    int value = 120;
    if (p<0.35) value = sin(p*2.857*PI*0.5)*maxBrightness;
    else if (p>0.5) value = sin(p*PI)*(maxBrightness-5)+5;
     
    for (int i=0; i<3; ++i){
        analogWrite(bulbPins[i],bulbMinD[i] + value);
    }
    
  }
  else{
    int t = millis() % bulbAnimationTime;
    int activeLed = 0;
    int tOffset = 0;
    int valueChanged = 0;

    if(t > bulbAnimationTime/3*2){
      activeLed = 2;
      tOffset = bulbAnimationTime/3*2;
    }
    else if(t > bulbAnimationTime/3){
      activeLed = 1;
      tOffset = bulbAnimationTime/3;
    }
    

    float brightness = sin((t-tOffset)*PI*3/bulbAnimationTime)*maxBrightness;
    if(controlValueChanged + 50 > millis()) valueChanged = maxBrightness;

    for (int i=0; i<3; ++i){
      int base = serialIn[0] + bulbMinD[i];
      if(i == activeLed){
        analogWrite(bulbPins[i],min(base + min(brightness + valueChanged,maxBrightness),150));
      }
      else{ 
        analogWrite(bulbPins[i],min(base +min(valueChanged,maxBrightness),150));
      }
    }
  }
}


void setup()
{
    Serial.begin(BAUD_RATE);

	  //init shift registers for switch
    pinMode(ploadPin, OUTPUT);

    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, INPUT);

    digitalWrite(clockPin, LOW);
    digitalWrite(ploadPin, HIGH);

    pinMode(bulbPins[0],OUTPUT);
    pinMode(bulbPins[1],OUTPUT);
    pinMode(bulbPins[2],OUTPUT);
	
  	//init led pins
    pinMode(redLedPin, OUTPUT);
    pinMode(yellowLedPin, OUTPUT);
	
	  //init supply pins
	  pinMode(9, OUTPUT);

    //Adruino leonardo fast pwm mode usign TIMER1
    TCCR1A = (1 << WGM10);
    TCCR1B = (1 << WGM12);
    TCCR1C = 0;

  
    TCCR1B |= (1 << CS10); // no prescaler - 62KHz
    TCCR1A |= (1 << COM1A1);
}

void doIdle(){
  SupplySetVoltage(0);

   int t = millis() % bulbAnimationTime;
  int activeLed = 0;
  int tOffset = 0;
 int  maxBrightness = 10;

  if(t > bulbAnimationTime/3*2){
    activeLed = 2;
    tOffset = bulbAnimationTime/3*2;
  }
  else if(t > bulbAnimationTime/3){
    activeLed = 1;
    tOffset = bulbAnimationTime/3;
  }
  

  float brightness = sin((t-tOffset)*PI*3/bulbAnimationTime)*maxBrightness;

  for (int i=0; i<3; ++i){
    int base = serialIn[0] + bulbMinD[i];
    if(i == activeLed){
      analogWrite(bulbPins[i],base + brightness);
    }
    else{ 
      analogWrite(bulbPins[i],base);
    }
  }
}

void loop() {
	
	
	//while(1) {
       // check serial buffer for input
   int test = Serial.read();

    // start byte received?
    if( test == SYNC_BYTE )
    {
      // yes, read data bytes
      Serial.readBytes( serialIn, 2 );
    }

    if(serialIn[1] == 0 && lastState == 1){
      lastActivated = millis();
    }
    lastState = serialIn[1];

    //check if idle
    if(serialIn[1] == 0 )
    {
      pinValues = read_shift_regs();
    if(pinValues != oldPinValues) {
      sw = registerValuteToSwitchValue(pinValues);
      
      //nur alle "rastenden Schalterstellungen"
      //werte von 0-21 werden gesendet ...
      if(!(sw % 2)) {
        if(swOld != sw) {
          Serial.print(':');
          Serial.println(sw / 2);
          //Serial.print('\n');
          Serial.flush();
          swOld = sw;

          controlValueChanged = millis();
        }
      }
      
      SupplySetVoltage(sw*3);
      oldPinValues = pinValues;
    }

    SupplyFeedbackLoop();
    
    //led flicker efect
    ledFlicker();

   //set bulbs
   setBulbBrightness();
  //}
    //delay(POLL_DELAY_MSEC);
    }
    else
    {
      doIdle();
    }
    
		
}


