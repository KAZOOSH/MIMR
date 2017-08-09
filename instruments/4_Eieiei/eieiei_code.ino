// schaltung  
// linke seite motor - 1uF - arduino a5 - 10k - 3,3v
// rechte seite motor - 1uF - arduino a4 - 10k - 3,3v
// mitte motor - arduino gnd


// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin5 = A5;  // Analog input pin that the potentiometer is attached to
const int analogInPin4 = A4;  // Analog input pin that the potentiometer is attached to

int sensorValue1 = 0;        // value read from the pot
int sensorValue2 = 0;        // value read from the pot

int isSensorOneNeg = 1;
int isSensorTwoNeg = 1;

double loopcount = 0 ;

double loopsToSensor1Neg = 0;
double loopsToSensor2Neg = 0;
double loopsToSensor1Pos = 0;
double loopsToSensor2Pos = 0;

int basevalue = 678;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
}

void loop() {

  loopcount++;
  
  // read the analog in value:
  sensorValue1 = analogRead(analogInPin5)- basevalue;
  sensorValue2 = analogRead(analogInPin4)- basevalue;

  //Serial.println(sensorValue1);
  //Serial.println(sensorValue2);

  // check if sensor one changes from pos to neg
  if(isSensorOneNeg == 1 && sensorValue1 > 1){
    isSensorOneNeg = 0;
    loopsToSensor1Pos = loopcount;
  }
  if(isSensorOneNeg == 0 && sensorValue1 < -1){
    isSensorOneNeg = 1;
    loopsToSensor1Neg = loopcount;
  }

  // check if sensor two changes from pos to neg
  if(isSensorTwoNeg == 1 && sensorValue2 > 1){
    loopsToSensor2Pos = loopcount;
    isSensorTwoNeg = 0;
  }
  if(isSensorTwoNeg == 0 && sensorValue2 < -1){
    isSensorTwoNeg = 1;
    loopsToSensor2Neg = loopcount;
    loopcount=0;
    checkDirection();
  }
}


void checkDirection(){
  if(loopsToSensor1Neg < loopsToSensor2Pos){
    Serial.println("links");
  } else{
    Serial.println("rechts");
  }
  
}



void usart_prozess2graph(uint32_t data2graph)
  {
    /*    CONVERT UNSIGNED INTEGER 2 LOG GRAPH WITH 1/4 BIT  AND SHOW  IT NICE    */
    data2graph <<= 2;
    for (uint8_t i=0; data2graph > 3; i++)
    {
      Serial.print("||||");
      if(data2graph < 8)
      {
        if(data2graph & 2)
        Serial.print("||");
        if(data2graph & 1)
        Serial.print('|');
      }
      data2graph >>= 1;
    }
    Serial.println();
  }

