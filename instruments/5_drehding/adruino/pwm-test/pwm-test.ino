

void test1() {
  int adc;
  for(int co=255;co > 0;co--) {
    OCR1A = co;

    delay(100);
    adc = analogRead(1);
    Serial.print(co);
    Serial.print(" ");
    Serial.print(adc);
    Serial.print('\n');
  }
}

void test2() {
  const double quantum = 5.0 / 1023;
  int adc0;
  int adc1;
  byte duty = 0;
  while(1) {    
    adc1 = analogRead(1);

    double voltage1 = adc1 * quantum;
    double voltage2 = adc1 * quantum * 20.5;

    
  
    adc0 = analogRead(0);

    duty = adc0 / 4;
    OCR1A = duty;

    Serial.print(adc1);
    Serial.print(" ");
    Serial.print(voltage1);    
    Serial.print(" ");
    Serial.print(voltage2);
    Serial.print(" ");
    Serial.print(adc0);
    Serial.print(" ");
    Serial.print(duty);
    Serial.print("\n");       
  }  
}


void test3() {

  const float supply = 13.8;
  const float quantum = (5.0 / 1023) * 20.5;

 
  int adc0;
  int adc1;
  
  
  float setpoint;
  float feedback;
  float error;
  
  int change;
  byte op=0;
  byte duty;
  
  while(1) {
    adc0 = analogRead(0);//poti
    adc1 = analogRead(1);//divider

    
    setpoint = (adc0 / 8);

    if(setpoint > 65.0) setpoint = 65.0;
    
    feedback = adc1 * quantum - supply;
    error = setpoint - feedback;

    if(error > 0) {
      op++;
      change = 1;
    } else if(error < 0) {
      op--;
      change = -1;
    }

    
    duty = 255 - op;    //invert
    if(duty < 9) duty = 9; //safty
    OCR1A = duty;
    



    Serial.print("adc0: ");
    Serial.print(adc0);
    Serial.print(" adc1: ");
    Serial.print(adc1);

    Serial.print(" set: ");
    Serial.print(setpoint);
    Serial.print(" fedback: ");
    Serial.print(feedback);
    Serial.print(" error: ");
    Serial.print(error);
        
    Serial.print(" change: ");
    Serial.print(change);
    Serial.print(" duty: ");
    Serial.print(duty);
    Serial.print("\n");
  }  


}




void test4() {

  const float supply = 13.8;
  const float quantum = (5.0 / 1023) * 20.5;

  const float Kp = 4;
  const float Ki = 0.0;
  const float Kd = 0.0;

  static float P=0,I=0,D=0;
  float PID;



  
  int adc0;
  int adc1;
  
  
  float setpoint;
  float feedback;
  float Lfeedback;
  float error;
  
  int change;

  byte duty;
  
  while(1) {
    adc0 = analogRead(0);//poti
    adc1 = analogRead(1);//divider

    
    setpoint = (adc0 / 8);
    feedback = adc1 * quantum - supply;
    error = setpoint - feedback;

    P = Kp * error;
    I += Ki * error;
    D = Kd * (Lfeedback - feedback);

    Lfeedback = feedback;
    
    PID = P + I + D;
    
    change = PID;
    duty = PID;
    if(duty > 200) duty = 200;

    OCR1A = 255 - duty;
    //OCR1A = 150;



    Serial.print("adc0: ");
    Serial.print(adc0);
    Serial.print(" adc1: ");
    Serial.print(adc1);

    Serial.print(" set: ");
    Serial.print(setpoint);
    Serial.print(" fedback: ");
    Serial.print(feedback);
    Serial.print(" error: ");
    Serial.print(error);


    Serial.print(" P: ");
    Serial.print(P);
    Serial.print(" I: ");
    Serial.print(I);
    Serial.print(" D: ");
    Serial.print(D);
    Serial.print(" PID: ");
    Serial.print(PID);
        
    Serial.print(" change: ");
    Serial.print(change);
    Serial.print(" duty: ");
    Serial.print(duty);
    Serial.print("\n");
  }  


}





void setup() {
    Serial.begin(9600);
    pinMode(9, OUTPUT);
    //pinMode(6, OUTPUT);

    
    
    //leonardo pwm mode
    TCCR1A = (1 << WGM10);
    TCCR1B = (1 << WGM12);
    TCCR1C = 0;

    
    TCCR1B |= (1 << CS10); // no prescaler - 62KHz
    //TCCR1B |= (1 << CS11); // no prescaler - 62KHz

    TCCR1A |= (1 << COM1A1);

   //test1();
}


void loop() {
  //test1();
  //test2();
  test3();
}
