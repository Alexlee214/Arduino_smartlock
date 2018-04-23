//#include <avr/sleep.h>
//#include <avr/interrupt.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <Servo.h> 
#include <LiquidCrystal.h>
#include <MemoryFree.h>

const int servoPin = A0;
const int inputled = 11;
const int correctled = 10;
const int buzzer = A1;
const int interruptPin = 3;

byte correctLength = 4;
byte passMaxLength = 8;
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {3, 4, 5, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 8, 9}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
Servo Servo1; 

const int rs = A5, en = A4, d4 = A3, d5 = A2, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------setup
void setup()
{
  pinMode(inputled, OUTPUT);
  pinMode(correctled, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(interruptPin, INPUT);
  
  Serial.begin(9600);
  /*IMPORTANT: COMMENT OUT THE FOLLOWING LINE AND UPLOA AGAIN AFTER UPLOADING FOR THE FIRST TIME*/
  //initializeEEPROM();
  correctLength = EEPROM.read(0);
  Servo1.attach(servoPin); 
  Servo1.write(5);
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.clear();
  lcd.print(F(" Welcome Home!!"));
  lcd.setCursor(0, 1);
  lcd.print(F("Any key to start"));
}



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------loop
void loop()
{
  Serial.print("freeMemory()=");
    Serial.println(freeMemory());
    startDevice();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------startDevice
void startDevice(){
  char key = keypad.getKey();
  //go back to sleep after 5 seconds of inactivity
    if(key == '#'){
      enterLED();
      Serial.println(F("Enter passcode change"));
      changePasscode();
    }  
    else if(key == '*'){
      enterLED();
      unlockDoor();
      Serial.println(F("Exited out of code entry"));
    }
    else if (key != NO_KEY){
      idleLED();
      Serial.println(F("Press * to enter passcode, # to change pass"));
      lcd.clear();
      lcd.print(F(" * : enter code"));
      lcd.setCursor(0, 1);
      lcd.print(F(" # : change code"));
    }    
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------readPasscode
//password stored starting from index 0
char readPasscode(byte location){
  return EEPROM.read(location);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------updatePasscode
void updatePasscode(byte location, char newPass){
  EEPROM.write(location, newPass);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------initializeEEPROM
void initializeEEPROM(){
  EEPROM.write(0, correctLength);
  EEPROM.write(1, '0');
  EEPROM.write(2, '2');
  EEPROM.write(3, '1');
  EEPROM.write(4, '4');
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------LED
void enterLED(){
    digitalWrite(inputled, HIGH);
    digitalWrite(correctled, HIGH);
    delay(100);
    digitalWrite(inputled, LOW);
    digitalWrite(correctled, LOW);
}

void idleLED(){
    digitalWrite(inputled, HIGH);
    delay(100);
    digitalWrite(inputled, LOW);
}

void changedPassLED(){
    for(int count = 0; count <= 5; count++){
    digitalWrite(inputled, HIGH);
    digitalWrite(correctled, HIGH);
    delay(100);
    digitalWrite(inputled, LOW);
    digitalWrite(correctled, LOW);
    delay(100);
  }  
}

void enteredLED(){
    tone(buzzer, 2000); // Send 1KHz sound signal
    delay(300);
    noTone(buzzer);     // Stop sound
    
    Servo1.attach(servoPin);
    Servo1.write(95); 
    digitalWrite(correctled, HIGH);
    delay(5000);
    digitalWrite(correctled, LOW);
    Servo1.write(5);
    tone(buzzer, 2000);
    delay(1000);
    //detach after it is locked so that the lock is also turnable with key
    noTone(buzzer);
    Servo1.detach();
}

void wrongLED(){
      digitalWrite(inputled, HIGH);
      digitalWrite(correctled, HIGH);
      delay(1000);
      digitalWrite(inputled, LOW);
      digitalWrite(correctled, LOW);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------keyEntry
//have this turn off automatically after 10 seconds of inactivity
//return the length of the passcode entered as a byte
byte keyEntry(char *entered){
  char key;
  byte characterCount = 0;  

  //initialize all characters to '*' to make checking easier and to avoid segmentation fault
  for(byte countLength=0; countLength < passMaxLength; countLength++){
    *(entered + countLength) = '*';
  }

  unsigned long currTime = millis();
  unsigned long prevTime = millis();
  while(key != '#'){
    currTime = millis();
    //if more than 6 seconds on inactivity, return
    if((currTime - prevTime) > 6000){
      Serial.println(F("Session timeout"));
      lcd.clear();
      lcd.print(F("Session timedout"));
      delay(1000);
      return 0;
    }

    key = keypad.getKey();
    if(characterCount >= passMaxLength){
      Serial.println(F("Password too long!"));
      lcd.print(F("Code is too long"));
      
      return passMaxLength;
    }
    if(key!= NO_KEY && key != '#'){
      
      prevTime = currTime;
      enterLED();
      *(entered + characterCount) = key;
      characterCount++;
    }
  }
  enterLED();
  enterLED();

  return  characterCount;
}



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------unlockDoor
void unlockDoor(){
  //keep track of starting time to check for timeout
  
  Serial.println(F("you can now enter passcode"));
  lcd.clear();
  lcd.print(F("Enter passcode:"));
  //allocate memory for entering of passcode
  char *entered = malloc(passMaxLength*sizeof(char));

  //assign the entered passcode
  byte passLength = keyEntry(entered);
  bool correct = checkPassword(entered, passLength);

  if(correct == true){
    Serial.println(F("Door Unlocked"));
    lcd.clear();
    lcd.print(F("Door unlocked"));
    enteredLED();
  }else{
    Serial.println(F("Wrong passcode!"));
    lcd.clear();
    lcd.print(F("Wrong passcode"));
    wrongLED();
  }
  free(entered);
}



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------checkPassword
//check if the entered passcode match the correct passcode
boolean checkPassword(char* entered, byte passLength){
  //check all the characters possible
  //first check the lenght of the entered pass
  if (correctLength != passLength)
    return false;
  
  for(byte countIndex = 0; countIndex < passLength; countIndex++){
    if(*(entered + countIndex) != readPasscode(countIndex + 1))
      return false;
  }

  return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------checkMatch
boolean checkMatch(char* entered, char* check, byte passLength, byte checkLength){
  //check all the characters possible
  if (passLength != checkLength)
    return false;
  
  for(byte countIndex = 0; countIndex < passLength; countIndex++){
    if(*(entered + countIndex) != *(check + countIndex))
      return false;
  }

  return true;
}




//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------changePasscode
void changePasscode(){
  Serial.println(F("enter original passcode: "));
  //cannot return an array so use pointers
  char *entered = malloc(passMaxLength*sizeof(char));;
  byte passLength = keyEntry(entered);
  //pass the entered pointer and the correct pointer to be checked
  if(checkPassword(entered, passLength) == true){
    SetNewPass();
  }else{
    Serial.println(F("No admin access!"));
    wrongLED();
  }
  free(entered);
}



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------SetNewPass
void SetNewPass(){
  Serial.println(F("Enter new passcode"));
  lcd.clear();
  lcd.print(F("Enter new pass:"));
  char *entered = malloc(passMaxLength*sizeof(char));
  char *check = malloc(passMaxLength*sizeof(char));
  byte newLength = keyEntry(entered);

  //if session timed out
  if(newLength == 0){
    return;
  }  
  
  Serial.println(F("Enter again"));
  lcd.clear();
  lcd.print(F("Verify new pass:"));
  byte checkLength = keyEntry(check);

  //deal with cases where length not matching or password not matching
  if(newLength != checkLength || checkMatch(entered, check, newLength, checkLength) == false){
    Serial.println(F("Passcodes do not match"));
    lcd.clear();
    lcd.print(F("Pass don't match"));
    wrongLED();
  }else{
    correctLength = newLength;
    //change the stored passcode length
    EEPROM.write(0, correctLength);
    //change the passcode stored
    for(byte countIndex = 0; countIndex < newLength; countIndex++){
      updatePasscode(countIndex + 1, *(entered + countIndex));
    }
    changedPassLED();
    Serial.println(F("Passcode updated"));
    lcd.clear();
    lcd.print(F("Pass Updated!!!!"));
  }
  free(entered);
  free(check);
}

