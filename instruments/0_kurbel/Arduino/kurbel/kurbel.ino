/*
 * MIMR - KURBEL
 * KAZOOSH! - Brian Eschrich 2017
 * reads Values from the Kurbel, smooths them a little and sends it via serial to the pi
 */



const int numReadings = 5; //number of smoothing values
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


void setup() {
  //set pins
  pinMode(A3,OUTPUT);
  pinMode(A5,OUTPUT);
  digitalWrite(A5,HIGH);
  digitalWrite(A3,LOW);

  //init serial
  Serial.begin(115200);

  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void smooth(int value){
  // subtract the last reading:
  total = total - readings[readIndex];
  // add new value:
  readings[readIndex] = value;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
}


void loop() {
  //read kurbel value
  int value = analogRead(A4);
  
  //smooth the value
  smooth(value);
  
  //minimum read is 128, so subtract value
  value -= 128;

  //remap to 1Byte -> 0-127
  float ltemp = (float)value / (1023-128) * 127;

  //write to Serial
  Serial.println((int)(127 - ltemp));

  //wait for next read
  delay(10);
}
