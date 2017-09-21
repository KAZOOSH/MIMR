const int redLedPin  = 10; //red
const int greenLedPin   = 9; //yellow
const int blueLedPin  = 11; //white

const int whiteSwitchPin  = 7; //green
const int orangeSwitchPin  = 2; //yellow
const int redSwitchPin  = 4; //red


const int potiPin = 0;   //R + red
const int fanPin = 6; //R + yellow

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
}

void loop() {
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

  analogWrite(fanPin,map(analogRead(potiPin), 0, 1023, 0, 255));

  delay(100);
}
