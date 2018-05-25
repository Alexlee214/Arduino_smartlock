# Description
The main goal of this project is to create a user-friendly, easy-to-use smart lock. Although the code was initially tested with an Arduino UNO, the ultimate goal is to create a stand alone device using the atmega328P-PU microcontroller that allows for minimal power consumption. To reduce cost and complexity, future goal includes to develop a version for the attiny85 while maintaining the same fundamental functionalities.



## Current functionalities
* April 22, 2018, Version 1.0.1
 * Passcode can be anywhere from 1 to 8 characters long
 * Auto session timeout after 6 seconds in passcode entry mode
 * Memory optimization by reading password directly from EEPROM(Therotically reading from EEPROM would be slower than SRAM, but barely noticiable in this case)
 * Improved security by storing the length of the current password length
 * Support servo motor control (Synchronized with led light)
 
* April 13, 2018, Version 1.0.0
 * Implemented using a 4 digit passcode
 * Use the onboard EEPROM to store the correct passcode
 * Two LEDs are used to indicate the status of the lock
 * Allow users to securely change the passcode
 * 5 seconds after the lock is opened, close it again



## Future improvements
* Allow the chip to enter deep sleep mode
* Use button interrput to turn on the microcontroller
* Within a certain time of inactivity from the keypad, re-enter sleep mode
* Add photocell to determine when the door is opened and relock when closed again
* Add LCD
* Implement servo motor to replace the LED indication lights
* Decrease dynamic memory usage
* Implement for attiny85 (512 bytes SRAM)
* Use the concept of voltage divider to reduce pins used by key pad to only 1
* Implement password with hasing, salt and pepper for improved security



## Appendix
* EEPROM:
 - 0th index stores length of the passcode
 - 1st - 8th indices store the password
 
* When uploading code  should be done twice
 - First time to initialize EEPROM to the original passcode, which is set to 0214
 - Second time initializtion should be commented out (line 44)
