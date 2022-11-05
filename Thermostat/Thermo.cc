//Group credit: Austin W, Joe C, Jordan R.
//Library for LCD
#include <LiquidCrystal.h>

////////Target Temp Storage//////////////
volatile int targetTemp[2] = {20,20}; //first value is current target, second is the previous target
String heatOrCool = "";

//location of set temp, current temp on LCD
const int LCDSetTempPos[2] = {0,0};
const int LCDCurrentTempPos[2] = {0,1};

//current temperature
float currvolt;
float currentTempC;

////////TMP sensor pin///////////////////
int tempPin = 0;

///////Piezo Pin////////////////////////
int buzzer = 6;

///////Button Initializaiton Pins///////
const int tempUpPin = 8;
int buttonStateUp = 0;

const int tempDownPin = 9;
int buttonStateDown = 0;

const int changeScreenPin = 10;
volatile int changeScreenState = 0; //0 = C, 1 = F

const int saveTargetPin = 11;
int currentTargetTemp = targetTemp[0];

const int lastTargetPin = 13;
int lastTargetState = 0;
/////////////////////////////////////

//////Initialize DC Motor Pins/////////
const int motorPinHeat = A1;
const int motorPinCool = A2;
///////////////////////////////////////

//////Initialize LED Pins//////////////
const int ledB = A3; //Blue LED
const int ledR = A4; //Red LED
///////////////////////////////////////

//Initialize LCD with pin layout. LCD is set up 4-bit 5 X 8 matrix
LiquidCrystal lcd(12,7,5,4,3,2);

void setup()
{
  Serial.begin(9600); //Serial connect to Computer, can view data
  					  //in console. 
  pinMode(tempUpPin,INPUT); //Push buttons
  pinMode(tempDownPin,INPUT);
  pinMode(changeScreenPin,INPUT);
  pinMode(saveTargetPin, INPUT);
  pinMode(LED_BUILTIN, INPUT);
  
 //enable interrupts for push buttons
  PCMSK0 = B00101111; //enable pin change interrupt on pins 8,9,10,11,13
  PCIFR = B00000000; //clear all interrupt flags
  PCICR = B00000001; //enable PCIE0 group
  sei(); //enable global interrupts
  
  pinMode(ledB,OUTPUT); //LEDS
  pinMode(ledR,OUTPUT);
  
  pinMode(motorPinHeat, OUTPUT); //Motors
  pinMode(motorPinCool, OUTPUT);
  
  pinMode(buzzer, OUTPUT); //Piezo
  
  lcd.begin(16,2); //Coloums and Rows of LCD
}

void loop()
{
  //Reads from the sensor and convert to Celcius.
  int tempRead = analogRead(tempPin);
  currvolt = (tempRead * 5.0) / 1024.0;
  currentTempC = (currvolt - 0.5) * 100; //Variable that will store the temp.
  
  //Buzzer if nominal temp is above the target
   if(currentTempC > targetTemp[0]){ 
       //tone(buzzer,3000,200);
  }
  
  //Real time graph will record temperatures
   Serial.println(currentTempC); 
  
  //For motor and LED to be powered depending on the relative target and nominal temp
  if(currentTempC < targetTemp[0]){
    digitalWrite(motorPinHeat, HIGH);
    digitalWrite(ledR, HIGH);
    
    digitalWrite(motorPinCool, LOW);
    digitalWrite(ledB, LOW);
    lcd.setCursor(0,1);
    heatOrCool = "Heat On";
  }
  else if (currentTempC == targetTemp[0]){
    digitalWrite(motorPinHeat, LOW);
    digitalWrite(ledR, LOW);
    
    digitalWrite(motorPinCool, LOW);
    digitalWrite(ledB, LOW);
    
    heatOrCool = "";  }
  else{
    digitalWrite(motorPinHeat, LOW);
    digitalWrite(ledR, LOW);
    
    digitalWrite(motorPinCool, HIGH);
    digitalWrite(ledB, HIGH);
    
    heatOrCool = "Fan On";
  }

  updateLCD();
  delay(600);
}

ISR(PCINT0_vect){
  cli();
  if(digitalRead(tempUpPin) == HIGH){tempUpISR();} //PCINT0 changed
  else if(digitalRead(tempDownPin) == HIGH){tempDownISR();} //PCINT1 changed
  else if(digitalRead(changeScreenPin) == HIGH){changeScreenISR();} //PCINT2 changed
  else if(digitalRead(saveTargetPin) == HIGH){saveTargetISR();}
  else if(digitalRead(lastTargetPin) == HIGH){lastTargetISR();}
  sei();
}

void tempUpISR(){
    currentTargetTemp = currentTargetTemp + 1;
}

void tempDownISR(){
    currentTargetTemp = currentTargetTemp - 1;
}

void changeScreenISR(){
    if (changeScreenState == 0){changeScreenState = 1;}
    else if (changeScreenState == 1){changeScreenState = 0;}
}

void lastTargetISR(){
  	currentTargetTemp = targetTemp[1];
    targetTemp[1] = targetTemp[0];
  	targetTemp[0] = currentTargetTemp;
}

void saveTargetISR(){
  	targetTemp[1] = targetTemp[0];
  	targetTemp[0] = currentTargetTemp; 
  	lcd.clear();
  	lcd.print("Target Saved");
  	delay(500);
}
          
void updateLCD(){
  lcd.clear();
  
  //update set temp
  lcd.setCursor(LCDSetTempPos[0],LCDSetTempPos[1]);
  lcd.print("Set Temp: ");
  if (changeScreenState == 0){
    lcd.print(currentTargetTemp);
    lcd.print("C");
  }
  else if (changeScreenState == 1){
    lcd.print(convCtoF(currentTargetTemp));
    lcd.print("F");
  }
  
  //update heat/cool message
  lcd.setCursor(LCDCurrentTempPos[0], LCDCurrentTempPos[1]);
  if (changeScreenState == 0){
    lcd.print(heatOrCool);
   }
  else if (changeScreenState == 1){
    lcd.print(heatOrCool);
  }
}

int toggleState(int state){
  if (state == 0){state == 1;}
  else if (state == 1){state == 0;}
  return state;
}

float convCtoF(float degreeC){
   return (degreeC * 1.8) + 32;
}