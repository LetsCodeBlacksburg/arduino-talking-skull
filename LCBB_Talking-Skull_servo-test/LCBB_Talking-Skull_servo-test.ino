/***************************************************
  LCBB_Talking-Skull_servo-test
  By tweeks-pub1@theweeks.org

  Based on open source library: 
  Servo.h - by Michael Margolis
 ******************************
  Instructions:
  Servo test is on pin 6
  Simple program to test a servo between two position values (open and close).
  Experiment with values between 0-180 at first. Set (or rename) the values 
  mouthOpened and mouthClosed (used to move a mouth servo). Write down the 
  values you need and use them in your programs.

  After you get this working, move on to the LCBB_servo-skull-test program.
 */

// include the servo library
#include "Servo.h"
Servo myServo;  // create a servo object



//Mouth Settings
const int servoPin=6;   // Connecting the servo to this digital I/O pin

//**** Settings for 6" Skull, Right jaw-joint servo-axis pivot mount
//****    -expect max of around 40-60 degrees of travel for this jaw setup 
int mouthClosed=125 ;   // These values will have to be discovered for your servo configuration.
int mouthOpened=80 ;    // These values will have to be discovered for your servo configuration.
int mouthDelay=200;      // Amount of time to allow one open or closed move (1/2 cycle)

//// Change these values experimentially from 0-180 (or your max servo range)
//// TIP: If the servo-arm (horn) is in the wrong position, you can pull it off
////      and reposition it where you want it to start or end.
int mouthOpened=0 ;    // These values will have to be discovered for your servo configuration.
int mouthClosed=80 ;   // These values will have to be discovered for your servo configuration.


void setup() {
  myServo.attach(servoPin); // attaches the servo on pin 6 to the servo object
  Serial.begin(9600); // open a serial connection to your computer
}


// *************** MAIN LOOP *******************
void loop() {
  myServo.write(mouthOpened);
  delay(1000);

  myServo.write(mouthClosed);
  delay(1000);
  
  delay(5000);    // recommend this when first testing so you know which value is open vs closed
}


