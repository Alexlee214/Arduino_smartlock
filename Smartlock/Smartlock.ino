/*
 * Created by Alex Lee
 * April 13, 2018
 */

#include <avr/sleep.h>
#include <Keypad.h>
#include <StackArray.h>
#include <QueueArray.h>

const int interruptPin = 2;
const int outPin = 11;
const int led = 10;

char correctPasscode[] = "0214";
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


//The setup function is called once at startup of the sketch
void setup()
{
  Serial.begin(9600);
  keypad.setHoldTime(1000);
}



// The loop function is called in an endless loop
void loop()
{
  char key = keypad.getKey();

  if(key == '#'){
    Serial.println("Enter passcode change");
    changePasscode();
    Serial.println("cannot change passcode");
  }  
  else if(key == '*'){
    unlockDoor();
    Serial.println("Exited out of code entry");
  }
  else if (key != NO_KEY)
    Serial.println("Press * to enter passcode, # to change pass");  
}



//have this turn off automatically after 10 seconds of inactivity
QueueArray<char> keyEntry(){
  QueueArray <char> enteredPass;

  while(true){
    char key = keypad.getKey();
    if(key != NO_KEY && key != '#'){
      Serial.print("pushed: ");
      Serial.println(key);
      enteredPass.push(key);
    }  
    else if(key == '#')
      break;  
  }

  return enteredPass;
}



void unlockDoor(){
  Serial.println("you can now enter passcode");
  QueueArray<char> entered = keyEntry();
  bool correct = checkPassword(entered);

  if(correct == true)
    Serial.println("Door Unlocked");
  else{
    Serial.println("Wrong passcode!");
    Serial.println(entered.count());
  }

  
}



//check if the entered passcode match the correct passcode
boolean checkPassword(QueueArray<char> entered){
  //if the size do not match, wrong password
  if(entered.count() != strlen(correctPasscode)){
    return false;
  //otherwise check each character  
  }else{
    for(int count = 0; count < strlen(correctPasscode); count++){
      if(entered.peek() != correctPasscode[count])
        return false;
      entered.pop();  
    }
  }
  return true;
}


void changePasscode(){
  Serial.println("enter original passcode to continue");
  QueueArray<char> entered = keyEntry();
  if(checkPassword(entered) == true){
    SetNewPass();
  }else{
    Serial.println("You do not have admin access!");
  }
}



void SetNewPass(){
  Serial.println("Enter new passcode");
  QueueArray<char> entered = keyEntry();
  Serial.println("Now enter again to set passcode");
  QueueArray<char> check = keyEntry();

  QueueArray<char> newPassCpy = check;

  if(entered.count() != check.count()){
    Serial.println("new passcodes do not match");
  //otherwise check each character  
  }else{
    //check if new passcodes match
    for(int count = 0; count < entered.count(); count++){
      if(entered.peek() != check.peek()){
        Serial.println("new passcodes do not match");
        return;
      }
      entered.pop(); 
      check.pop(); 
    }

    //otherwise they match, change passcode
    String newPasscode = "";
    while(!newPassCpy.isEmpty()){
      newPasscode += newPassCpy.peek();
      newPassCpy.pop();
    }

    char charBuf[5];
    newPasscode.toCharArray(charBuf, 5);

    memcpy(correctPasscode, charBuf, 5);
    Serial.println("Passcode changed");
  }
}
