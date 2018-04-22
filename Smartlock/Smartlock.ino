//#include <avr/sleep.h>
#include <Keypad.h>
#include <EEPROM.h>

const int inputled = 11;
const int correctled = 10;

byte correctLength = 4;
byte passMaxLength = 8;

const int led = 10;
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


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------setup
void setup()
{
  pinMode(inputled, OUTPUT);
  pinMode(correctled, OUTPUT);
  Serial.begin(9600);
  initializeEEPROM();
}



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------loop
void loop()
{
  char key = keypad.getKey();

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
  }    
}

//password stored starting from index 0
char readPasscode(byte location){
  Serial.print("EEPROM IS READING: ");
  Serial.println(EEPROM.read(location));
  return EEPROM.read(location);
}

void updatePasscode(byte location, char newPass){
  EEPROM.write(location, newPass);
}


void initializeEEPROM(){
  EEPROM.write(0, '0');
  EEPROM.write(1, '2');
  EEPROM.write(2, '1');
  EEPROM.write(3, '4');
}


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
    digitalWrite(correctled, HIGH);
    delay(5000);
    digitalWrite(correctled, LOW);
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
      Serial.println("Session timeout");
      return 0;
    }

    key = keypad.getKey();
    if(characterCount >= passMaxLength){
      Serial.println(F("Password too long!"));
      return passMaxLength;
    }
    if(key!= NO_KEY && key != '#'){
      
      prevTime = currTime;
      enterLED();
      Serial.print(F("pushed: "));
      Serial.println(key);
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
  //allocate memory for entering of passcode
  char *entered = malloc(passMaxLength*sizeof(char));

  //assign the entered passcode
  byte passLength = keyEntry(entered);
  Serial.println("The length of the pass entered is: ");
  Serial.println(passLength);
  bool correct = checkPassword(entered, passLength);

  if(correct == true){
    Serial.println(F("Door Unlocked"));
    enteredLED();
  }else{
    Serial.println(F("Wrong passcode!"));
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
    if(*(entered + countIndex) != readPasscode(countIndex))
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
  Serial.println(F("enter original passcode to continue"));
  //cannot return an array so use pointers
  char *entered = malloc(passMaxLength*sizeof(char));;
  byte passLength = keyEntry(entered);
  //pass the entered pointer and the correct pointer to be checked
  if(checkPassword(entered, passLength) == true){
    SetNewPass();
  }else{
    Serial.println(F("You do not have admin access!"));
    wrongLED();
  }
  free(entered);
}



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------SetNewPass
void SetNewPass(){
  Serial.println(F("Enter new passcode"));
  char *entered = malloc(passMaxLength*sizeof(char));
  char *check = malloc(passMaxLength*sizeof(char));
  byte newLength = keyEntry(entered);

  //if session timed out
  if(newLength == 0){
    Serial.println(F("Failed to change pass"));
    return;
  }  
  
  Serial.println(F("Now enter again"));
  byte checkLength = keyEntry(check);

  //deal with cases where length not matching or password not matching
  if(newLength != checkLength || checkMatch(entered, check, newLength, checkLength) == false){
    Serial.println(F("new passcodes do not match"));
    wrongLED();
  }else{
    correctLength = newLength;
    for(byte countIndex = 0; countIndex < newLength; countIndex++){
      updatePasscode(countIndex, *(entered + countIndex));
    }
    changedPassLED();
    Serial.println(F("Passcode updated"));
  }
}

