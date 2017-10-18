/***************************************************
  LCBB_Talking-Skull_servo-skull-test
  By tweeks-pub1@theweeks.org

  Based on open source library: 
  Servo.h - by Michael Margolis
 ******************************
  Instructions:
  Servo test is on pin 6
  Uses the test mouthOpened and mouthClosed values you got from LCBB_servo-test 
  and lets you fine tune the mechanics of your skull and jaw motion. Includes a
  laughTest(n) function that will cycle the jaw n times. 

  Mechanical Config Assumptions:
  Assumes servo is mounted on the skull's right jaw side internally in one of
  the three listed configurations.
 */

// include the servo library
#include "Servo.h"
Servo myServo;  // create a servo object
const int servoPin=6;   // Connecting the servo to this digital I/O pin

//////////////// Different Skull/Servo Configuration Settings //////////////////////////
// uncommend the three config values you think you need for your skull mechanics and 
// plug in your mouthOpened and mouthClosed values.

//**** 6" Skull, Right jaw w/servo horn, wire from horn through slit into upper jar tab
//****   -around 80 degrees of travel
int mouthOpened=0 ;    // These values will have to be discovered for your servo configuration.
int mouthClosed=80 ;   // These values will have to be discovered for your servo configuration.
int moveDelay=200;     // Amount of time to allow one open or closed move (1/2 cycle) 

//**** 6" Skull, Right jaw-joint servo-axis pivot mount
//****    -around 60 degrees of travel 
//int mouthClosed=110 ;   // These values will have to be discovered for your servo configuration.
//int mouthOpened=50 ;    // These values will have to be discovered for your servo configuration.
//int moveDelay=200;      // Amount of time to allow one open or closed move (1/2 cycle)

//**** 3" Skull, Right jaw-joint servo-axis pivot mount 
//****    -around 15 degrees of travel
//int mouthClosed=105 ;   // These values will have to be discovered for your servo configuration.
//int mouthOpened=90 ;    // These values will have to be discovered for your servo configuration.
//int moveDelay=100;      // Amount of time to allow one open or closed move (1/2 cycle) 


void setup() {
  myServo.attach(servoPin); // attaches the servo on pin 6 to the servo object
  Serial.begin(9600); // open a serial connection to your computer
}


// *************** MAIN LOOP *******************
void loop() {

  myServo.write(mouthClosed);
  delay(3000);                    // Longer delay so you can note your starting closed position.

  
  myServo.write(mouthOpened);     // one open test
  delay(moveDelay);
  myServo.write(mouthClosed);     // one closed test
  delay(moveDelay);

  //laughTest(3);               // Cycles the jaw in a laughing motion
   
}


// *************** laughTest() ******************
// Opens & closes 4 times in a laughing motion. (use to set moutOpened/mouthClosed values)
void laughTest(int n){
  for ( int x=1 ; x < (n+1) ; x++){
  myServo.write(mouthOpened);
  delay(moveDelay);
  myServo.write(mouthClosed);
  delay(moveDelay);
  }
}



