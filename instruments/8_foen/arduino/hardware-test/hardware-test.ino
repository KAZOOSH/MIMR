#include "myRGB.h"

const int redLedPin  = 10; //red
const int greenLedPin   = 6; //yellow
const int blueLedPin  = 11; //white

const int whiteSwitchPin  = 7; //green
const int orangeSwitchPin  = 2; //yellow
const int redSwitchPin  = 4; //red


const int potiPin = 0;   //R + red
const int fanPin = 9; //R + yellow





void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);


  
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  pinMode(fanPin, OUTPUT);

  //pullups on
  digitalWrite(whiteSwitchPin, HIGH);
  digitalWrite(orangeSwitchPin, HIGH);
  digitalWrite(redSwitchPin, HIGH);

  TCCR1B = TCCR1B & 0b11111000 | 0x01; // Setzt Timer1 (Pin 9 und 10) auf 31300Hz
}

void test1() {
  //io test
  static byte red,green,blue;
  static byte state;
  static byte update;
  
  //if(update++==0) {
    Serial.print("\nLOOP START\n");
  
    Serial.print("WHITE SWITCH: ");
    Serial.print(digitalRead(whiteSwitchPin));
  
    Serial.print("\nORANGE SWITCH: ");
    Serial.print(digitalRead(orangeSwitchPin));
  
    Serial.print("\nRED SWITCH: ");
    Serial.print(digitalRead(redSwitchPin));
  
    Serial.print("\n\nPOTI VALUE: ");
    Serial.print(analogRead(potiPin));
  
    //delay(1000);
  //}

  
  switch(state++) {
    case 0:
      red = 255; green = 0; blue = 0;
      break;
    case 1:
      red = 0; green = 255; blue = 0;
      break;
    case 2:
      red = 0; green = 0; blue = 255;
      break;
    default:
      red = 0; green = 0; blue = 0;
      state = 0;
  }
  

  analogWrite(redLedPin,red);
  analogWrite(greenLedPin,green);
  analogWrite(blueLedPin,blue);

  //analogWrite(fanPin,map(analogRead(potiPin), 0, 1023, 0, 255));
  analogWrite(9,map(analogRead(potiPin), 0, 1023, 0, 255));
  
  delay(100);
}
void test2() {
  String a;
  static byte red,green,blue;
  while(1) {
    Serial.print("START\n");
    

    Serial.print("\nR: ");
    while(Serial.available() == 0) {}
    a = Serial.readStringUntil('\n');
    red = a.toInt();
    Serial.print(red);

    Serial.print("\nG: ");
    while(Serial.available() == 0) {}
    a = Serial.readStringUntil('\n');
    green = a.toInt();
    Serial.print(green);

    Serial.print("\nB: ");
    while(Serial.available() == 0) {}
    a = Serial.readStringUntil('\n');
    blue = a.toInt();
    Serial.print(blue);

    analogWrite(redLedPin,red);
    analogWrite(greenLedPin,green);
    analogWrite(blueLedPin,blue);    

 
  }
}


void test3() {
  byte value;
  //fading red
  while(1) {
    Serial.print("UP\n");
    for(byte co=1;co++;co < 255) {
      value = co;
      analogWrite(redLedPin,value);
      delay(10);
    }
    Serial.print("DOWN\n");
    for(byte co=254;co--;co > 0) {
      value = co;
      analogWrite(redLedPin,value);
      delay(10);
    }    
  }
}






uint8_t interPol(float start,float ende, uint8_t range,uint8_t steep) {
  uint8_t res;
  res = (start + ((ende - start) / range) * steep);
  
  Serial.print("\n start Color: "); 
  Serial.print(start);  
  Serial.print("\n end Color: ");
  Serial.print(ende);
  Serial.print("\n range: ");
  Serial.print(range);
  Serial.print("\n steep: ");
  Serial.print(steep);
  
  Serial.print("\n ==> result: ");
  Serial.print(res);
        
  return res;
}

void fadePallette(myRGB par[],uint8_t elements,uint8_t steep) {
  uint8_t g = 255 / (elements - 1);
  uint8_t w = steep % g;
  uint8_t si = steep / g;
  uint8_t ei = si + 1;
  //if(ei == elements) ei = 0;

  //si = 2;ei = 0;g = 255;

  Serial.print("\n elements: ");
  Serial.print(elements);
  Serial.print("\n si: "); 
  Serial.print(si);  
  Serial.print("\n ei: ");
  Serial.print(ei);
  //return;

  myRGB r;

  Serial.print("\nred: ");
  r.r = interPol(par[si].r,par[ei].r,g,w);
  Serial.print("\ngreen: ");
  r.g = interPol(par[si].g,par[ei].g,g,w);
  Serial.print("\nblue: ");
  r.b = interPol(par[si].b,par[ei].b,g,w);

  
  analogWrite(redLedPin,r.r);
  analogWrite(greenLedPin,r.g);
  analogWrite(blueLedPin,r.b);    
}


void test4() {
  static int counter=0;
  static int direction=0;
  delay(10);
  int value = interPol(50,150,255,counter);
  Serial.print(value);
  Serial.print("\n");
  analogWrite(redLedPin,value);
  if(direction == 1) {
    counter--;
  } else {
    counter++;
  }
  if(counter == 255) {
    direction = 1;
  } else if (counter == 0) {
    direction = 0;  
  }
}

void test5() {
  /*
  static int counter=0;
  static int direction=0;
  
  fadePallette(pallette,palletteSize,counter);
  
  if(direction == 1) {
    counter--;
  } else {
    counter++;
  }
  if(counter == 255) {
    direction = 1;
  } else if (counter == 0) {
    direction = 0;  
  }
  */

  static int lv = 0;
  int cv = map(analogRead(potiPin), 0, 1023, 0, 255);

  if(lv!=cv) {
    lv = cv;
    fadePallette(pallette,palletteSize,cv);
  }

  
  //delay(100);
}






void loop() {
  //test1();
  //test2();
  //test3();
  //test4();
  test5();
}

