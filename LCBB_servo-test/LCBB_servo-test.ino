/***************************************************
  LCBB_servo-test
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
 */

// include the servo library
#include "Servo.h"

const int servoPin=6;   // Connecting the servo to this digital I/O pin

// Change these values experimentially from 0-180 (or your max servo range)
int mouthOpened=80 ;    // These values will have to be discovered for your servo configuration.
int mouthClosed=115 ;   // These values will have to be discovered for your servo configuration.
  // For my test servo, 0-90 degrees of movement = values 0 - 104. 
  // Use the map() function to correct if exact angles are needed.
  // For my servo hardware config, mouth OPENED = 80
  // For my servo hardware config, mouth CLOSED = 115



Servo myServo;  // create a servo object

void setup() {
  myServo.attach(servoPin); // attaches the servo on pin 6 to the servo object
  Serial.begin(9600); // open a serial connection to your computer
}


// *************** MAIN LOOP *******************
void loop() {
  myServo.write(mouthOpen);
  delay(1000);

  myServo.write(mouthClosed);
  delay(1000);
  
  delay(5000);    // recommend this when first testing so you know which value is open vs closed
}


