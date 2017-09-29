#define NUMBER_OF_SHIFT_CHIPS   3
#define DATA_WIDTH NUMBER_OF_SHIFT_CHIPS * 8
#define PULSE_WIDTH_USEC 5
#define BYTES_VAL_T unsigned long

const int ploadPin  = 8; //blue
const int dataPin   = 7; //yellow
const int clockPin  = 4; //green

const int redLedPin = 5;   //R + red
const int yellowLedPin = 6 ; //R + yellow



const int supplyPwmPin = 9; //Supply PWM pin
const int supplyAdcPin = 1; //Feedback Input

const float supplyVoltage = 13.8; 	//Important: Always set DC input Voltage of Supply in Volt !!!
static float SupplySetpoint = 0;	//output voltage


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




void setup()
{
    Serial.begin(9600);

	  //init shift registers for switch
    pinMode(ploadPin, OUTPUT);

    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, INPUT);

    digitalWrite(clockPin, LOW);
    digitalWrite(ploadPin, HIGH);
	
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


void loop() {
	
	BYTES_VAL_T pinValues;
	BYTES_VAL_T oldPinValues;
	byte sw,swOld;
	while(1) {
		pinValues = read_shift_regs();
		if(pinValues != oldPinValues) {
			sw = registerValuteToSwitchValue(pinValues);
      
      //nur alle "rastenden Schalterstellungen"
      if(!(sw % 2)) {
        if(swOld != sw) {
          Serial.print(':');
          Serial.println(sw / 2);
          //Serial.print('\n');
          Serial.flush();
          swOld = sw;
        }
      }
      
			SupplySetVoltage(sw*3);
			oldPinValues = pinValues;
		}

		SupplyFeedbackLoop();
		
		//led flicker efect
		ledFlicker();
	}
    //delay(POLL_DELAY_MSEC);
}

