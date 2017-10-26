
/***************************************************
  LCBB_Talking-Skull_ping-test (w/getDist() function)
  By tweeks-pub1@theweeks.org

  Based on open source library:
   http://www.arduino.cc/en/Tutorial/Ping
   by David A. Mellis and Tom Igoe
 ***************************************************
  Instructions:
  Ping sensor uses pins 4(trigger) & 5(echo)

  // PING                  
  // US-100  \--5v-trg-echo-GND-GND--/    
  //             |   |   |   |   |        
  // Arduino    Vcc  4   5  GND           


  This has the ping sensor code which is a little complex. So once 
  you get it working, save it, and then add your skull servo lauging 
  code to this, also get it working and give IT a new meaningful name
  like Talking-Skull_ping+servo.  

 */

 

  // Wiring of Ultrasonic Ping Sensor (US-100 or HC-SR04)
  //
  // PING                  
  // US-100  \--5v-trg-echo-GND-GND--/     
  //             |   |   |   |   |         
  // Arduino    Vcc  4   5  GND  (if available, unused or clipped off)          
  //                                       
  // 

/////////// HARDWARE SETTINGS/HOOKUPS ////////

//Ping sensor
const   int trgPin = 4;     // pin we're sending the trigger/ping signal on
const   int echoPin = 5;    // pin we're reading back the echo on
const   long motionDist=6;  // distance that triggers a action
long    distance=5000;      // distance to be measured/recorded by ping sensor


//********************* LARGE SETUP BLOLCK **************************  
void setup() {
  Serial.begin(9600); // for sending serial text back to your computer
  
  // Set Up US-100 or HC-SR04 PING SENSOR I/O PINS
  pinMode(trgPin, OUTPUT);
  digitalWrite(trgPin, LOW);
  pinMode(echoPin, OUTPUT);    // just to make sure
  digitalWrite(echoPin, LOW);  // we clear any previous settings
  pinMode(echoPin, INPUT);     // and then use it as INPUT
  delay(500);
}


//************************ MAIN LOOP ********************************
void loop() {
  delay(100);
  distance=getDist();       // samples distance from ping sensor
  delay(100);

  ////// Loop here until someone comes closer than motionDist
  while(distance > motionDist ){
    distance=getDist();                   // sample distance
    delay(100);
    distance=(distance + getDist()) / 2;  // sample again and take the average
    delay(100);
    Serial.print("Distance = ");
    Serial.println(distance);    
    }

  Serial.println("************** Someone is close! **************");
  Serial.print("Distance = ");
  Serial.println(distance);    
  Serial.println("--move mouth, play sound--");
  delay(5000);  

}


// Code from Arduino ping
// ***********************************************************
// ***** getDist() *******************************************
// ***********************************************************
long getDist()
{
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, inches, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trgPin, HIGH);      // start the outgoing ping
  delayMicroseconds(10);            // do the ping for 10uS
  digitalWrite(trgPin, LOW);       // stop doing the ping
  duration = pulseIn(echoPin, HIGH);  // grab the delay of return echo
  // convert the time into a distance
  inches = microsecondsToInches(duration);
  //cm = microsecondsToCentimeters(duration);
  //Serial.print(inches);
  //Serial.print("in, ");
  //Serial.print(cm);
  //Serial.print("cm");
  //Serial.println();

  return (inches);
}


// Original code from the ping sensor library
long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}


