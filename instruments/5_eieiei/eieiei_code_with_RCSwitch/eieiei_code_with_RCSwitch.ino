// schaltung  
// linke seite motor - 1uF - arduino a5 - 10k - 3,3v
// rechte seite motor - 1uF - arduino a4 - 10k - 3,3v
// mitte motor - arduino gnd

// d10 - data pin of FS1000a

#include <RCSwitch.h>

// wireless communication
RCSwitch mySwitch = RCSwitch();

// analog ports for measuring
const int analogInPin5 = A5;   
const int analogInPin4 = A4;  

// State Booleans 
int sensorValue1 = 0;        // value read from the pot
int sensorValue2 = 0;        // value read from the pot

int isSensorOneNeg = 1;
int isSensorTwoNeg = 1;

double loopsToSensor1Neg = 0;
double loopsToSensor2Neg = 0;
double loopsToSensor1Pos = 0;
double loopsToSensor2Pos = 0;

// analog value for zero pass of motor (not moving)
int basevalue = 678;

// counter for time measuring
double loopcount = 0 ;
double lastlooplength = 0;
boolean isHalftime = false;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  mySwitch.enableTransmit(10);
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
    checkDirection();
    lastlooplength = loopcount;

    // beginn new loopcount
    loopcount=0;
    isHalftime= false;
  }

  // on half of last loopcount
  if(loopcount > lastlooplength /2 && isHalftime != true){
    isHalftime= true;
    //mySwitch.send(44, 24);
    Serial.println("reset");
  }
}


void checkDirection(){
  if(loopsToSensor1Neg < loopsToSensor2Pos){
    //mySwitch.send(11, 24); // Der 433mhz Sender versendet die Dezimalzahl „11“
    Serial.println("links");
  } else{
    //mySwitch.send(22, 24); 
    Serial.println("rechts");
  }
}




