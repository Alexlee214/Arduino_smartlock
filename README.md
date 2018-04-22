# Description

The main goal of this project is to create a user-friendly, easy-to-use smart lock. Although the code was initially tested with an Arduino UNO, the ultimate goal is to create a stand alone device using the atmega328P-PU microcontroller that allows for minimal power consumption. To reduce cost and complexity, future goal includes to develop a version for the attiny85 while maintaining the same fundamental functionalities.


### Current functionalities
* April 13, 2018, Version 1.0.0
 * Implemented using a 4 digit passcode
 * Use the onboard EEPROM to store the correct passcode
 * Two LEDs are used to indicate the status of the lock
 * Allow users to securely change the passcode
 * 5 seconds after the lock is opened, close it again


### Future improvements
* Allow the chip to enter deep sleep mode
* Use button interrput to turn on the microcontroller
* Within a certain time of inactivity from the keypad, re-enter sleep mode
* Add photocell to determine when the door is opened and relock when closed again
* Add LCD
* Implement servo motor to replace the LED indication lights
* Decrease dynamic memory usage
* Implement for attiny85 (512 bytes SRAM)
* Use the concept of voltage divider to reduce pins used by key pad to only 1
