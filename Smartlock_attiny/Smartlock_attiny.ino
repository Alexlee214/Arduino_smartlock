#include <Keypad.h>
#include <EEPROM.h>

const int inputled = 11;
const int correctled = 2;

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

char correctPasscode[4];


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------setup
void setup()
{
  pinMode(inputled, OUTPUT);
  pinMode(correctled, OUTPUT);
  readPasscode();
}



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------loop
void loop()
{
  char key = keypad.getKey();

  if(key == '#'){
    enterLED();
    changePasscode();
    updatePasscode();
  }  
  else if(key == '*'){
    enterLED();
    unlockDoor();
  }
  else if (key != NO_KEY){
    idleLED();
  }    
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------read from eeprom
void readPasscode(){
  correctPasscode[0] = EEPROM.read(1);
  correctPasscode[1] = EEPROM.read(2);
  correctPasscode[2] = EEPROM.read(3);
  correctPasscode[3] = EEPROM.read(4);
}

void updatePasscode(){
  EEPROM.write(1, correctPasscode[0]);
  EEPROM.write(2, correctPasscode[1]);
  EEPROM.write(3, correctPasscode[2]);
  EEPROM.write(4, correctPasscode[3]);
}


void initializeEEPROM(){
  EEPROM.write(1, '0');
  EEPROM.write(2, '2');
  EEPROM.write(3, '1');
  EEPROM.write(4, '4');
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
void keyEntry(char *entered){
  byte characterCount = 0;
  char key;

  while(characterCount < 4){
    key = keypad.getKey();
    if(key!= NO_KEY){
      enterLED();
      *(entered + characterCount) = key;
      characterCount++;
    }
  }
  enterLED();
  enterLED();
}



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------unlockDoor
void unlockDoor(){
  //allocate memory for entering of passcode
  char *entered = malloc(4*sizeof(char));

  //assign the entered passcode
  keyEntry(entered);
  bool correct = checkPassword(entered, &correctPasscode[0]);

  if(correct == true){
    enteredLED();
  }else{
    wrongLED();
  }
  free(entered);
}



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------checkPassword
//check if the entered passcode match the correct passcode
boolean checkPassword(char* entered, char* check){
  for(byte countIndex = 0; countIndex < 4; countIndex++){
    if(*(entered + countIndex) != *(check + countIndex)) return false;
  }

  return true;
}




//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------changePasscode
void changePasscode(){
  //cannot return an array so use pointers
  char *entered = malloc(4*sizeof(char));;
  keyEntry(entered);
  //pass the entered pointer and the correct pointer to be checked
  if(checkPassword(entered, &correctPasscode[0]) == true){
    SetNewPass();
  }else{
    wrongLED();
  }
  free(entered);
}



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------SetNewPass
void SetNewPass(){
  char *entered = malloc(4*sizeof(char));
  char *check = malloc(4*sizeof(char));
  keyEntry(entered);
  keyEntry(check);

  if(checkPassword(entered, check) == false){
    wrongLED();
  }else{
    for(byte countIndex = 0; countIndex < 4; countIndex++){
      correctPasscode[countIndex] = *(entered + countIndex);
    }
    changedPassLED();
  }
}

