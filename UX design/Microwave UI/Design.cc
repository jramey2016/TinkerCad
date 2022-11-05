//Credit to Group members: Austin W, Joe C, Jordan R.
//Library for the LCD and keypad
#include <Keypad.h>
#include <LiquidCrystal.h>

//Initialize digital pins and for LCD/////
LiquidCrystal lcd(13, 12, 11, 10, A4, A5);
/////////////////////////////////////////

//Initialize digital pins for the key pad/
const byte ROWS = 4; //four rows on the keypad.
const byte COLS = 4; //four coloumns on the keypad.
char keys [ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};


byte rowP [ROWS] = {2, 3, 4, 5}; //Initialize pin layout for rows.
byte colP [COLS] = {6, 7, 8, 9}; //Initialize pin layout for columns.
Keypad keypad = Keypad(makeKeymap(keys), rowP, colP, ROWS, COLS);

volatile byte row0 = rowP[0];
byte row1 = rowP[1];
byte row2 = rowP[2];
byte row3 = rowP[3];
volatile byte col0 = colP[0];
byte col1 = colP[1];
byte col2 = colP[2];
byte col3 = colP[3];
///////////////////////////////////////////////
int system_state = 0; //state 0 = normal mode, state 1 = programming mode, state 2 = shortcut mode, state 3 = operating mode, state 4 = menu mode
char cook_time[10] = "0000"; //HHMM
char timer1[10] = "0000"; //HHMM
char timer2[10] = "0000"; //HHMM
int cook_minutes = 0;
int cook_seconds = 0;
int timer1_min = 0;
int timer1_sec = 0;
int timer2_min = 0;
int timer2_sec = 0;
int timeLeftBuzzer = 15;
int timerDisplay = 3;
//////////////////////////////////////////
char mainMenu[][20] = {"1. Set Time", "2. Set Timer", "3. Show Timer"};
char timerMenu[][20] = {"1. Timer 1", "2. Timer 2"};
/////////////////////////////////////////
const int LED = A3;
const int piezo = A2;

//Set up time for the display//
int hour = 11; //Just a random combination of time, come back to this.
int minute = 12;
int second = 33;


void setup()
{
  
  Serial.begin(9600); //Serial connect to Computer, can view data
  					  //in console.

  lcd.begin(16,2); //Coloums and Rows of LCD
  //keypad.setHoldTime(100);
  //noInterrupts(); 
  //sei();
  
  ////////////////////////KEYPAD PINS/////////////////
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  ///////////////////////////////////////////////////
  
  ///////////////////////LCD Pins////////////////////
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  ///////////////////////////////////////////////////
  
  pinMode(LED, OUTPUT); //pinmodes for LED and piezo
  pinMode(piezo, OUTPUT);
  
  TCCR1A = 0;
  TCCR1B = 0;
  
  TCNT1 = 3036; //1 second
  //TCNT1 = 0; //1 second
  TCCR1B |= (1 << CS12); //set prescaler to /256
  TIMSK1 |= (1 << TOIE1);//enable timer1 overflow interrupt
  
  
  //interrupts(); 

  normalMode(); 
}

void loop() 
{
  char key = keypad.getKey();
  if (key == 'A'){
    system_state = 4;
  }
  if(key == 'D'){ // D is the designated button for shortcut mode
    system_state = 3;  
  }
  if (system_state == 0){
      if (key != NO_KEY){
        system_state = 1;
        lcd.clear();
        //resetCookTime();
      }
    }
    //else if (system_state == 0){normalMode();}
    if (system_state == 1){ //Prog mode @ system state 1 (
      programmingMode(key);
    } 
    if (system_state == 2){ //operating mode @ system state 2 ("" was pressed.)
        operatingMode(key);
    }
    if (system_state == 3){ //shortcut mode @ system state 3 ("B" was pressed.)
    
    }
  if (system_state == 4){ //Menu mode @ system state 4 ("A" was pressed)
    menuMode(key);
  }
    }

void menuMode(char key){
  if(key == NO_KEY){
    displayMenu();
  	key = keypad.getKey();
  }
  if (key == '1'){
    lcd.clear();
    lcd.print('1');             }
  if (key == '2'){setTimer();}
  //else if (key == '3'){displayTimer();}
  
}

void displayMenu(){
  lcd.clear();
  lcd.print(mainMenu[0]);
  lcd.setCursor(0,1);
  lcd.print(mainMenu[1]);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(mainMenu[2]);
  delay(2000);
}
void normalMode(){
  printTimeHMS(hour, minute, second);
}

void programmingMode(char key){
  if (key != '*' & key != '#' & key != 'D' & key != 'C' & key != 'B' & key != 'A' & key != NO_KEY){
    lcd.clear();
    shiftLeft();
    cook_time[3] = key;
   
    updateCookMS();
    printTimeMS(cook_minutes, cook_seconds);
  }
  else if(key == '#'){ // # is the designated button to execute 
    if ((((cook_minutes *60) + cook_seconds) <= 600) & (cook_minutes <= 10) & (cook_seconds <= 60)){
      system_state = 2;
    }
    else{
      lcd.clear();
      lcd.print("Invalid time");
      delay(2000);
      resetCookTime();
      printTimeMS(cook_minutes, cook_seconds);
    }
  }
}

void operatingMode(char key){
  if (((cook_minutes *60) + cook_seconds) != 0){
  	digitalWrite(LED,HIGH);
  }
  else if (((cook_minutes *60) + cook_seconds) == 0){
   resetCookTime();
   digitalWrite(LED,LOW);
   digitalWrite(piezo,HIGH); 
   delay(2000);
   digitalWrite(piezo,LOW);
   system_state = 0;
  }
  if(key == 'C'){//cancel
    digitalWrite(LED,LOW);
    digitalWrite(piezo,HIGH); 
    delay(1000);
    digitalWrite(piezo,LOW);
    resetCookTime();
    printTimeMS(cook_minutes, cook_seconds);
    system_state = 2;
  }
}

void shortcutMode(){ //function with system state paramater
  	lcd.clear();
 	lcd.print("1:Veggie "); //various print statements for common foods.
 	lcd.print("2:Meat");
 	lcd.setCursor(0,1);
 	lcd.print("3:Fish ");
    lcd.print("4:Popcorn");
while(system_state == 3){ //while loop that will wait and make the next step based on user input.
 char key = keypad.getKey(); 
  if(key != NO_KEY){
  	if(key == 'B'){ // 'B' being pressed will head back to the main loop.
		 lcd.clear(); //clear lcd
      	 lcd.setCursor(0,0);
      	 lcd.print("cook time: ");
         system_state = 0; //head back to the main loop.
  	  }
    if(key == '1'){ //The meat option is selected.
     	 lcd.clear();
      	 lcd.print("Veggie Selected"); //print user selection.
      	 delay(5000);
      	 lcd.clear();  
      	 lcd.print("cook time: "); //reset the lcd
      	 //FIXME: we will determine preset times later.
      	 system_state = 0; //Set state back to zero to head back to main loop.
    }
    if(key == '2'){ //Vegetable option is selected.
     	 lcd.clear();
      	 lcd.print("Veggie Selected");
      	 delay(300);
      	 lcd.clear();
      	 lcd.print("cook time: "); //reset the lcd
      	 //FIXME: we will determine preset times later.
         system_state = 0; //set state back to zero and head back to main loop.
    }
    if(key == '3'){ //Fish option is selected.
         lcd.clear();
      	 lcd.print("Fish Selected"); //notify the user
      	 delay(300);
      	 lcd.clear();
      	 lcd.print("cook time: "); //reset back to lcd
      	 //FIXME: we will determine preset times later.
      	 system_state = 0; //set state back to zero and head back to main loop.
    }
    if(key == '4') { //popcorn option is selected.
         lcd.clear();
      	 lcd.print("Popcorn Selected");
      	 delay(300);
      	 lcd.clear();
      	 lcd.print("cook time: ");
      	 //FIXME: we will determine preset times 
      	 system_state = 0;
    }
    }
}
}

void printTimeHMS(int hours, int minutes, int seconds){
  lcd.clear(); //clear the lcd
  lcd.setCursor(0,0); //set cursor to the orgin
  if (hours < 10){lcd.print("0");}
  lcd.print(hours);
  lcd.print(":");
  if (minutes < 10){lcd.print("0");}
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10){lcd.print("0");}
  lcd.print(seconds);
}

void printTimeMS(int minutes, int seconds){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Cook Time: ");
  if (minutes < 10){lcd.print("0");}
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10){lcd.print("0");}
  lcd.print(seconds);
}

void shiftLeft(){
  char temp[8];
  for (int i = 0; i < strlen(cook_time); i++){
    temp[i] = cook_time[i];
  }
  cook_time[0] = temp[1];
  cook_time[1] = temp[2];
  cook_time[2] = temp[3];
}
      
void updateCookMS(){
  	lcd.clear();
	char tempMin[3]= "00";
    char tempSec[3] = "00";
    tempMin[0] = cook_time[0];
    tempMin[1] = cook_time[1];
    tempSec[0] = cook_time[2];
    tempSec[1] = cook_time[3];
    
    sscanf(tempMin, "%d", &cook_minutes);
    sscanf(tempSec, "%d", &cook_seconds);
}

void resetCookTime(){
  for (int i = 0; i < strlen(cook_time); i++){
    cook_time[i] = '0';
  }
  updateCookMS();
}

void setTimer(){
  lcd.clear();
  lcd.print("Timer:");
  while(1){}
  
  
}

ISR(TIMER1_OVF_vect)        // interrupt service routine every one second
{
  TCNT1 = 3036; //reset timer
  
  //update system time
  if (second == 60){
    minute++;
    second = 0;
  }
  else if (second < 60){
    second++; 
  }
  if (minute == 60){
    hour++;
    minute = 0;
  }
  if (hour == 24){
    hour = 0; 
  }
  if (system_state == 0){
  	printTimeHMS(hour, minute, second);
  }
  
  //update cook time
  if (system_state == 2){
  if (cook_seconds == 0 & cook_minutes > 0){
    cook_minutes--;
    cook_seconds = 59;
  }
  else if (cook_seconds > 0){
  	cook_seconds--;
  }
  printTimeMS(cook_minutes, cook_seconds);  
  }
  
  //update timer 1
  if(timer1_sec > 0){
    timer1_sec = timer1_sec - 1;
    if(timerDisplay > 0){
    	timerDisplay = timerDisplay - 1;
      	//printTimeTimer1(timer1_min,timer1_sec);
    }
  }
  else{
    if(timer1_min >0){
      timer1_min = timer1_min - 1;
      timer1_sec = 59;
    }
    else{
      if(timeLeftBuzzer > 0){
      	digitalWrite(piezo,HIGH);
        timeLeftBuzzer = timeLeftBuzzer - 1;
      }
      else{
        digitalWrite(piezo,LOW);
      }
    }
  }
  
  //update timer2
  if(timer2_sec > 0){
    timer2_sec = timer1_sec - 1;
    if(timerDisplay > 0){
    	timerDisplay = timerDisplay - 1;
      	//printTimeTimer2(timer2_min,timer2_sec);
    }
  }
  else{
    if(timer2_min >0){
      timer2_min = timer2_min - 1;
      timer2_sec = 59;
    }
    else{
      if(timeLeftBuzzer > 0){
      	digitalWrite(piezo,HIGH);
        timeLeftBuzzer = timeLeftBuzzer - 1;
      }
      else{
        digitalWrite(piezo,LOW);
        //will have to set timeLeftBuzzer back to 15 when start a timer
      }
    }
  } 
  
}
