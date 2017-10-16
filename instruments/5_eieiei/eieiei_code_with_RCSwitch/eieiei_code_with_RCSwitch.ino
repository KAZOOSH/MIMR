// schaltung  
// linke seite motor - 1uF - arduino a5 - 10k - 3,3v
// rechte seite motor - 1uF - arduino a4 - 10k - 3,3v
// mitte motor - arduino gnd

// d10 - data pin of FS1000a

// wireless library
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();


// Arduino -> RasPi send interval (microseconds)
#define SEND_INTERVAL 20*1000L // 20 ms = 50 fps
unsigned long lastSendTime = 0;

// baud rate for USB communication with RasPi
#define BAUD_RATE 115200L


// analog ports for measuring
const int motorPin2 = A5;   
const int motorPin1 = A4;  
const int distanceSensorPin = A3;  

// distance sensor variables
double distSensorValue =0;
double distSensorValue0 =0;
double distSensorValue1 =0;
double distSensorValue2 =0;

double distSensorValueMax =360;
double distSensorValueMin =260;

// rotation sensor
int sensorValueRot1 = 0;        // value read from the pot
int sensorValueRot1_0 = 0;
int sensorValueRot1_1 = 0;
int sensorValueRot1_2 = 0;
int sensorValueRot2 = 0;        // value read from the pot
double sensorValueRot1Max = 0;        // value read from the pot

int isSensorOneNeg = 1;
int isSensorTwoNeg = 1;

double loopsToSensor1Neg = 0;
double loopsToSensor2Neg = 0;
double loopsToSensor1Pos = 0;
double loopsToSensor2Pos = 0;

// analog value for zero pass of motor (not moving)
double minvalueMotor = 9999;
double maxvalueMotor = 0;

// counter for time measuring
double loopcount = 0 ;
int loopcountjitter1 = 0 ;
int loopcountjitter2 = 0 ;
int lastlooplength = 0;
boolean isHalftime = false;
int inByte = 0;

//output values
int outDist = 0;
int outRot = 0;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(BAUD_RATE);
  mySwitch.enableTransmit(10);
  //mySwitch.setRepeatTransmit(3);
}

void loop() {
  printDistance();

  loopcount++;

  
  sensorValueRot1 = analogRead(motorPin1);
//Serial.println(sensorValueRot1);
  sensorValueRot1_2 = sensorValueRot1_1;
  sensorValueRot1_1 = sensorValueRot1_0;
  sensorValueRot1_0 = sensorValueRot1;

  if(loopcount > 3){
    sensorValueRot1 = median(sensorValueRot1_2,sensorValueRot1_1,sensorValueRot1_0);
  }
  
  if(minvalueMotor > sensorValueRot1){
    minvalueMotor = (double)sensorValueRot1;
  }
  if(maxvalueMotor < sensorValueRot1){
    maxvalueMotor = (double)sensorValueRot1;
  }

  //Serial.println(sensorValueRot1); 
  
  //Serial.print(minvalueMotor); 
  //Serial.print(":"); 
  //Serial.println(maxvalueMotor);
  // Serial.print(" ");
  // Serial.println( min(100,max(0,(int((maxvalueMotor- minvalueMotor) /10)) - 1) * 10)); 
   outRot =  min(100,max(0,(int((maxvalueMotor- minvalueMotor) /10)) - 1) * 10);
  
   maxvalueMotor=maxvalueMotor*0.9999;
   minvalueMotor=min(minvalueMotor*1.0001,maxvalueMotor);

  
  // time to update RasPi with current value?
  if ( micros() - lastSendTime > SEND_INTERVAL )
  {
    // remember time
    lastSendTime = micros();
    
    // write start character
    Serial.print( ":" );
    
    // write value as ASCII and append line break
    Serial.print( outDist );
    Serial.print(" ");
    Serial.println(outRot);

    // wait for transmission to finish
    Serial.flush();
  }

// if (Serial.available() > 0) {
//
//    delay(1000);
//    mySwitch.send(rgbToDouble(0,255,0), 24);
//    Serial.println("grün");
//    delay(1000);
//    mySwitch.send(rgbToDouble(0,0,255), 24);
//    Serial.println("blau");
//    delay(1000);
//    mySwitch.send(rgbToDouble(255,0,0), 24);
//    Serial.println("rot");
// }


}


long rgbToDouble(int r,int g,int b){
  return (double)(r/10)*10000+(double)(g/10)*100+b/10;
}


int median(int val1,int val2,int val3){
  if(((val1 <= val3) && (val1 >= val2 )) || ((val1 <= val2) && (val1 >= val3))){
    return val1;
  } else if(((val2 <= val3) && (val2 >= val1)) || ((val2 <= val1) && (val2 >= val3))){
    return val2;
  } 
  return val3;
}

void printDistance(){
  distSensorValue2 =distSensorValue1;
  distSensorValue1 =distSensorValue0;
  distSensorValue0 = analogRead(distanceSensorPin);
  
  if(distSensorValue0 < distSensorValueMin){ distSensorValue0 =distSensorValueMin;}
  if(distSensorValue0 > distSensorValueMax){ distSensorValue0 =distSensorValueMax;}

  distSensorValue = median(distSensorValue0,distSensorValue1,distSensorValue2);

  outDist = (int)(((distSensorValue-distSensorValueMin)/(max(1,distSensorValueMax-distSensorValueMin))) * 127);
  //Serial.print(":");
  //Serial.print((int)(((distSensorValue-distSensorValueMin)/(max(1,distSensorValueMax-distSensorValueMin))) * 127));
  
}





