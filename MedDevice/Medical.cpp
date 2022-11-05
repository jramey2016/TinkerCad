
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

char mystr[5];

int HBTpin = 10; //Red LED
int Normalpin = 9; //Green LED
int LBTpin = 8; //Blue LED
int Motorpin = 7; //DC motor

void setup() {
  Serial.begin(9600);
  
  pinMode(HBTpin, OUTPUT);
  pinMode(Normalpin, OUTPUT);
  pinMode(LBTpin, OUTPUT);
  pinMode(Motorpin, OUTPUT);
  
  lcd.begin(16, 2); // set up the LCD's number of columns and rows:
 
}

void loop() {
  
  Serial.readBytes(mystr,4); //reads the temp incoming from sender board
  
  
  double Rtemp;
  double Rtemp2 = mystr[1];
  
  //Have to take the char array that was sent from the sender board
  //and convert it to a number to read for future functionality.
  if(mystr[0] == '0'){Rtemp = 0;} //convert ASCII values
  if(mystr[0] == '1'){Rtemp = 10;} 
  if(mystr[0] == '2'){Rtemp = 20;}
  if(mystr[0] == '3'){Rtemp = 30;}
  if(mystr[0] == '4'){Rtemp = 40;}
  if(mystr[0] == '5'){Rtemp = 50;}
  if(mystr[0] == '6'){Rtemp = 60;}
  if(mystr[0] == '7'){Rtemp = 70;}
  if(mystr[0] == '8'){Rtemp = 80;}
  if(mystr[0] == '9'){Rtemp = 90;}
  
  if(mystr[1] == '1'){Rtemp2 = 1;} //convert ASCII values
  if(mystr[1] == '2'){Rtemp2 = 2;}
  if(mystr[1] == '3'){Rtemp2 = 3;}
  if(mystr[1] == '4'){Rtemp2 = 4;}
  if(mystr[1] == '5'){Rtemp2 = 5;}
  if(mystr[1] == '6'){Rtemp2 = 6;}
  if(mystr[1] == '7'){Rtemp2 = 7;}
  if(mystr[1] == '8'){Rtemp2 = 8;}
  if(mystr[1] == '9'){Rtemp2 = 9;}
  
  double Tfinal = Rtemp + Rtemp2; //Get final Reading
  
  if(Tfinal < 36){ //Patient is under temp
   lcd.clear(); //Clear screen
   lcd.setCursor(0,0);
   lcd.print("WARNING: LBT");
   lcd.setCursor(0,1);
   lcd.print("Current Temp:");
   lcd.setCursor(13,1);
   lcd.print(mystr);  
   lcd.setCursor(15,1);
   lcd.print("C");
    
   digitalWrite(HBTpin,LOW); //Turn Blue on rest off
   digitalWrite(LBTpin,HIGH);
   digitalWrite(Normalpin,LOW); 
   digitalWrite(Motorpin,LOW); //Turn motor off
  }
  if(Tfinal > 38){//Patient over temp
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("WARNING: HBT");
   lcd.setCursor(0,1);
   lcd.print("Current Temp: ");
   lcd.setCursor(13,1);
   lcd.print(mystr);  
   lcd.setCursor(15,1);
   lcd.print("C");
    
   digitalWrite(HBTpin,HIGH); //Turn LED on the rest off
   digitalWrite(LBTpin,LOW);
   digitalWrite(Normalpin,LOW);
   digitalWrite(Motorpin,HIGH); //Turn Motor on
  }
  if( Tfinal >=36 && Tfinal <= 38){ //Patient temp is normal
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Normal Body Temp");
   lcd.setCursor(0,1);
   lcd.print("Current Temp: ");
   lcd.setCursor(13,1);
   lcd.print(mystr);  
   lcd.setCursor(15,1);
   lcd.print("C");
    
   digitalWrite(HBTpin,LOW);
   digitalWrite(LBTpin,LOW);
   digitalWrite(Normalpin,HIGH); //Turn Green on rest off
   digitalWrite(Motorpin,LOW); //Turn the motor off.
  }
  
  
  
  delay(1000);
}
 