//Group Project credit: Austin W, Joe C, Jordan R
bool signalDetected = false;
const int buzzer = 6;
const int LED = 7;
const int recieverPin = A5;
const int switchPin = A4;
const int transmitPin = 5;
const int secondValue = 62500;
int LEDState = 0;
int stopValue;
float minMaxTime;
float period;
float frequency;
int tempMax;
int tempMin;
int max;
int min;
 
void setup(){
  pinMode(buzzer, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(recieverPin, INPUT);
  pinMode(switchPin, INPUT);
  pinMode(transmitPin, OUTPUT);
  Serial.begin(9600);
  TIMSK1 |= (1 << TOIE1);
  interrupts();
}

void loop(){	
  int sample = analogRead(recieverPin);
  Serial.println(sample);
   
  if (sample > tempMax){ //upward slope
    tempMax = sample; 
  }
  else if (sample < tempMax){ //downward slope
    max = tempMax;
    timerStart();
  }
  
  if (sample < min){ //downward slope
   	tempMin = sample;
  }
  else if (sample > min){ //upward slope
    min = tempMin;
    timerStop(); 
  }
  
  analogWrite(transmitPin, sample/4);
  calcMinMaxTime();
  calcFrequency();
  
  //if (frequency > 480 && frequency < 520){
    //signalDetected = true;
  //}
  if (sample > 50 or sample < -50){
    signalDetected = true;
  }
  else{
    signalDetected = false;
  }
  //if (analogRead(switchPin) > 512){
  //  signalDetected = true;
  //}
  //else{
  //  signalDetected = false;
  //}
  
  buzzerLED();
}

void buzzerLED(){
  if (signalDetected == true){
    toggleLED();
  	//digitalWrite(LED, HIGH);
  	tone(buzzer,3000);
  }
  else{
    LEDState = 0;
 	digitalWrite(LED, LOW);
    noTone(buzzer);
}
}

void toggleLED(){
  if (LEDState == 0){
    LEDState = 1;
    digitalWrite(LED, HIGH);
  }
  else if (LEDState == 1){
    LEDState = 0;
  }
}


void calcFrequency(){
  period = minMaxTime*2;
  frequency = 1/period;
}

void timerStart(){
  TCNT1 = 0;
  TCCR1B = (TCCR1B | 00000100); //set prescalar to 256, start timer
}

void timerStop(){
  stopValue = TCNT1;
  TCCR1B = (TCCR1B | 00000000); //stop timer
}

ISR(TIMER1_OVF_vect){
  TCNT1 = 0;   
  digitalWrite(LED, LOW);
}

void calcMinMaxTime(){
  if ((stopValue / 62500) > 1){
    int seconds = stopValue/secondValue;
    int remainder = (seconds*secondValue)-stopValue;
    float secondsRemainder = 1/secondsRemainder;
    minMaxTime = seconds + secondsRemainder;
  }
  else{
    minMaxTime = 1/(stopValue - secondValue);
  }
}