/***************************************************
  LCBB_Talking-Skull_ping-servo-dfplayer_complete
  By tweeks-pub1@theweeks.org

  Based on open source library:
    DFPlayer - A Mini MP3 Player For Arduino
    <https://www.dfrobot.com/index.php?route=product/product&product_id=1121>
    <Angelo.qiao@dfrobot.com>
 ***************************************************

Mostly complete code for the Let's Code Talking Skull project. Includes
the distance/ping sensor, the servo and laughTest(n) code, and
motion activated mp3 player of all available sound files.

***********Notice and Trouble shooting***************
 1. LCBB schematic here: <XXXXXXXXXXXXXx>
 2. Recommend supplying DFPlay mini board with 5v (not 3.3)
 3. Must place 1k resistor in series with pin 10 & DF-TX and pin 11 & DF-RX
 4. Use dfBusy to determine when sound stops
 ****************************************************/

  
  // Hardware wiring and Theory of DFPlay + Ping Sensor (US-100) and Servo
  //
  // LED Eyes   eyePinL=2   eyePinR=3
  //
  // PING                  
  // US-100  \--5v-trg-echo-GND-GND--/    
  //             |   |   |   |   |        
  // Arduino    Vcc  4   5  GND           
  //                                     
  // DFPlay mini  --Vcc(1)--RX(2)--TX(3)--DACR(4)--GND(7)  +(6) -(8)  ...  _BUSY
  //                 |       |      |       |       |      |    |           |
  //     Arduino    5v      11*    10*     A0      GND     |    |          12*     
  //                                                       \Spkr/
  //                              * requires 1k resisitor
  //     
  //  Servo is on digital I/O pin 6 
  //
  // The Ultrasonic distance sensor is triggered, sends the myDFPlayer.play(sndFile) command and then
  // monitors the DFPlayer's _BUSY line (pin 16) on arduino's pin12 (note the 1k 5:3.3v resistors)
  // and continues playing and animating the mouth servo. Each time the distance sensor is triggered, 
  // it plays the next sound, eventually looping back to the first sound.
  //
  // There are two ways to animate the mouth.. the easy way and the hard way.
  //
  // EASY MOUTH: laughCount(n) (recommend doing this first)
  // Just use the laughCount(n) function in conjunction with the number of laughs, or do a laughCount(1) while
  // the !digitalRead(dfBusy) == true (which means the DFPlayer is playing).
  //
  // HARD MOUTH: laughWsound() (do this only if you have finished everything else)
  // A strategy for lip-syncing the sound output can be done by using a sliding-window "audio envelope", but 
  // is not trivial. To start off, you sample the DFPlayer's audio out (DACR) on arduino's A0 as many times as
  // yo ucan within 20ms, average the top three MAX values to get a soundMax value, and as long as 
  // dfBusy = LOW, the soundSilence value is used to compensate for any DC drift out from the DFPlayer to 
  // subtract the rolling soundMax from the silenceValue to get the rolling, 20ms soundAmplitude value. 
  // This value should be in a time boxed sliding window to give you an approximate "VU" style volume level. 
  // Then you just use map() to mapping the silenceValue, soundAmplitude --> mouthClosed to mouthOpened values 
  // and send that value to the mouth servo (while _BUSY stays low)
  // <whew>


#include "Arduino.h"
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Servo.h>

Servo myServo;  // create a servo object


/////////////////////////////////////////////////////////////////////
///////////////////////// VARIABLES /////////////////////////////////
/////////////////////////////////////////////////////////////////////

/////////// HARDWARE SETTINGS/HOOKUPS ////////

//DFPlayer mp3 player
const int ardRX=10;       // The arduino software Receive lins (goes to DFPlayer TX)
const int ardTX=11;       // The arduino software Transmit line (goes to DFPlayer RX)
const int dfBusy=12;      // Coming from DFPlayer pin 16.. is active low (HIGH = not playing a sound)
int sndFile=1;            // Sound file pointer.
int fileCount=9;          // Either set the max # of files here, or load it from myDFPlayer.readFileCounts()
                          // NOTE: readFileCounts() also sees deleted files. May need to reformat to load new sounds.
int setVol=25;            //Set volume value (0~30)

//Eyes
const int eyePinL=2;
const int eyePinR=3;


//Ping sensor
const   int trgPin = 4;     // pin we're sending the trigger/ping signal on
const   int echoPin = 5;    // pin we're reading back the echo on
const   long motionDist=5; // distance that triggers a sound (5 for testing)
//const   long motionDist=35; // distance that triggers a sound (29-40 for real life)
long    distance=0;         // distance to be measured/recorded by ping sensor

//Sound Vars
const int soundPin=0;       // sound in pin (from DFPlayer DAC_R audio output)
int       soundVal=0;       // Sample analog sound values for moving jaw
int       soundMax=100;     // For tracking sound wave peak values
int       soundMin=500;     // For tracking sound wave peak values
int       silenceVal=250;   // The sounds... of silence... :)  (sampled so MP3 DC drift doesn't affect servo mapping)
int       soundAmplitude=150; // Avg max deltas for my sound files (e.g. map servo to silenceVal +- 1/2*soundAmplitude)


//Mouth Settings
const int servoPin=6;
//**** Settings for 6" Skull, Right jaw-joint servo-axis pivot mount
//****    -around 60 degrees of travel 
int mouthClosed=125 ;   // These values will have to be discovered for your servo configuration.
int mouthOpened=90 ;    // These values will have to be discovered for your servo configuration.
int mouthDelay=200;      // Amount of time to allow one open or closed move (1/2 cycle)


// Software Serial Pins To DFPlayer
SoftwareSerial mySoftwareSerial(10,11); // RX, TX  

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);


/////////////////////////////////////////////////////////////////////
/////////////////////// LARGE SETUP BLOLCK //////////////////////////
/////////////////////////////////////////////////////////////////////
void setup()
{  
  // Set Up US-100 PING SENSOR I/O PINS
  pinMode(trgPin, OUTPUT);
  digitalWrite(trgPin, LOW);
  pinMode(echoPin, OUTPUT);    // just to make sure
  digitalWrite(echoPin, LOW);  // we clear any previous settings
  pinMode(echoPin, INPUT);     // and then use it as INPUT
  delay(500);

  // Set up servo
  pinMode(servoPin, OUTPUT);
  myServo.attach(servoPin); // attaches the servo on pin 9 to the servo object
  delay(500);
  myServo.write(mouthOpened);
  delay(500);
  myServo.write(mouthClosed);

  // Set up eyes
  pinMode(eyePinL, OUTPUT);
  pinMode(eyePinR, OUTPUT);
  
  //  Set up DFPlay mini
  delay(250);
  pinMode(ardRX, INPUT);       
  pinMode(ardTX, OUTPUT);      
  pinMode(dfBusy, INPUT);       
  delay(100);
  mySoftwareSerial.begin(9600);
  delay(100);
  Serial.begin(9600);
  Serial.println(F("DFRobot DFPlayer Setup"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  // Check for DFPlay initialization via softserial
  if (!myDFPlayer.begin(mySoftwareSerial)) {    //Use softwareSerial to communicate with mp3.
    // if it did not work
    delay(20);
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    // T-SHOOTING
    Serial.print("INFO: player state / file counts: ");
    Serial.print(myDFPlayer.readState()); //read mp3 state
    Serial.print(" / ");
    Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
    while(true);      // Hang forever
  }

  // if serial setup worked
  Serial.println(F("DFPlayer Mini online."));
  // configure settings
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  myDFPlayer.volume(setVol);  //Set volume value (0~30).
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

///// Print Status:
  Serial.print("SETUP-INFO: player state= ");
  Serial.print(myDFPlayer.readState());         //read mp3 state
  Serial.print(" / volume setting(0-30)= ");
  Serial.print(myDFPlayer.readVolume());        //read current volume
  Serial.print(" / EQ setting= ");
  Serial.print(myDFPlayer.readEQ());          //read EQ setting
  Serial.print(" / fileCount= ");
  Serial.println(myDFPlayer.readFileCounts());  //read all file counts in SD card

}


// ***********************************************************
// *********************** MAIN LOOP *************************
// ***********************************************************
void loop()
{
  delay(200);
  distance=getDist();
  delay(60);

  ////// Wait Here For Someone Coming Near (distance <= motionDist)
  ////// Also grabs the sound output "silenceVal" (DC offset)
  while(distance > motionDist){
    //soundVal=analogRead(soundPin);
    silenceVal=analogRead(soundPin);
    Serial.print(" silenceVal= ");
    Serial.print(silenceVal);    
    Serial.print("    distance= ");
    Serial.println(distance);    
    distance=getDist();
    delay(120);
  }

  // PING > 1000 Bug Fix:
  // Use this if you see buggy distance >1000 then =1 issues
  //if (distance > 1000 ){
  //  while (distance > 1000 || distance==1 ){
  //    delay(150);
  //    distance=getDist();
  //  }
  //}


  ////// If at the last sound file, then loop back to the top (1)
  if (sndFile == (fileCount+1) ) {
    sndFile = 1;
  }

  ////// Begin playing the next sound file...
  Serial.println("************** PLAYING ****************");
  myDFPlayer.play(sndFile);   //Play the next mp3
  Serial.print(" silenceVal= ");
  Serial.print(silenceVal);    
  Serial.print("    distance= ");
  Serial.println(distance);    
  delay(100);                 // wait to start


  ////// While DFPlayer _BUSY line is active(low), use preferred laugh function.
  while(!digitalRead(dfBusy) == true ){
    digitalWrite(eyePinL, HIGH);
    digitalWrite(eyePinR, HIGH);
    laughCount(1);            // does 1 open/close cycle. Comment out if using laughWsound()
    //laughWsound();          // use this if you get the sound/mouth tracking working
    delay(20);
  }

 ////// Shut off eyes, close mouth, any relivant audio settings
  digitalWrite(eyePinL, LOW);
  digitalWrite(eyePinR, LOW);
  myServo.write(mouthClosed);
  delay(200);
  sndFile++;     // Incriment to the next sound file
  
}

// *****************************************************************
// ******************* Useful T-shooting Output ********************
// **** just copy into play and uncomment **************************

    //Serial.print("NEAR-INFO: getDist()= ");
    //Serial.print(distance);  
    //Serial.print(" / _BUSY= ");
    //Serial.print(digitalRead(dfBusy)); //read mp3 state
    //Serial.print(" / file count= ");
    //Serial.print(fileCount); //read all file counts in SD
    //Serial.print(" / snd-file= ");
    //Serial.println(sndFile);

    //Serial.print(" silenceVal= ");
    //Serial.print(silenceVal);
    //Serial.print(" soundVal= ");
    //Serial.print(soundVal);
    //Serial.print(" soundMax= ");
    //Serial.print(soundMax);
    //Serial.print(" soundMin= ");
    //Serial.print(soundMin);
    //Serial.print(" soundAmplitude= ");
    //Serial.println(soundAmplitude);




//*****************DFPlay Functions You Can Use *************************
 
////////----Set volume----
  //myDFPlayer.volume(10);  //Set volume value (0~30).
  //myDFPlayer.volumeUp(); //Volume Up
  //myDFPlayer.volumeDown(); //Volume Down
  
////////----Set different EQ----
//  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
//  myDFPlayer.EQ(DFPLAYER_EQ_POP);
//  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
//  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
//  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
//  myDFPlayer.EQ(DFPLAYER_EQ_BASS);
  
////////----Set device we use SD as default----
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_U_DISK);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_AUX);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SLEEP);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_FLASH);
  
////////----Mp3 control----
//  myDFPlayer.sleep();     //sleep
//  myDFPlayer.reset();     //Reset the module
//  myDFPlayer.enableDAC();  //Enable On-chip DAC
//  myDFPlayer.disableDAC();  //Disable On-chip DAC
//  myDFPlayer.outputSetting(true, 15); //output setting, enable the output and set the gain to 15
  
////////----Mp3 play----
//  myDFPlayer.next();  //Play next mp3
//  myDFPlayer.previous();  //Play previous mp3
//  myDFPlayer.play(1);  //Play the first mp3
//  myDFPlayer.loop(1);  //Loop the first mp3
//  myDFPlayer.pause();  //pause the mp3
//  myDFPlayer.start();  //start the mp3 from the pause
//  myDFPlayer.playFolder(15, 4);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
//  myDFPlayer.enableLoopAll(); //loop all mp3 files.
//  myDFPlayer.disableLoopAll(); //stop loop all mp3 files.
//  myDFPlayer.playMp3Folder(4); //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
//  myDFPlayer.advertise(3); //advertise specific mp3 in SD:/ADVERT/0003.mp3; File Name(0~65535)
//  myDFPlayer.stopAdvertise(); //stop advertise
//  myDFPlayer.playLargeFolder(2, 999); //play specific mp3 in SD:/02/004.mp3; Folder Name(1~10); File Name(1~1000)
//  myDFPlayer.loopFolder(5); //loop all mp3 files in folder SD:/05.
//  myDFPlayer.randomAll(); //Random play all the mp3.
//  myDFPlayer.enableLoop(); //enable loop.
//  myDFPlayer.disableLoop(); //disable loop.

//////----Read information----
//  Serial.println(myDFPlayer.readState()); //read mp3 state
//  Serial.println(myDFPlayer.readVolume()); //read current volume
//  Serial.println(myDFPlayer.readEQ()); //read EQ setting
//  Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
//  Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
//  Serial.println(myDFPlayer.readFileCountsInFolder(3)); //read fill counts in folder SD:/03






///////////////////////////////////////////////////////////
///////////////////// printDetail() ///////////////////////
///////////////////////////////////////////////////////////
void printDetail(uint8_t type, int value){

  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}



// ***********************************************************
// ********************* laughCount() ************************
// ***********************************************************
// Opens & closes n times in a laughing motion. (Uses your custom moutOpened/mouthClosed/mouthDelay values)
void laughCount(int n){
  for ( int x=1 ; x < (n+1) ; x++){
  myServo.write(mouthOpened);
  delay(mouthDelay);
  myServo.write(mouthClosed);
  delay(mouthDelay);
  }
}


// ************************************************************
// ********************* laughWsound() ************************
// ************************************************************
// Opens & closes mouth by tracking the auto-envelope (samples) of the audio output.
void laughWsound(){
   while(!digitalRead(dfBusy) == true ){
    soundVal=analogRead(soundPin);
    delay(10);
    //soundVal=((soundVal + analogRead(soundPin)) /2);
    // This fairly quickly self-sets the soundMax and soundMin values (in case I need them)

    if (soundVal > soundMax) {
      soundMax=soundVal;
    }
    if (soundVal < soundMin) {
      soundMin=soundVal;  
    }
    
    ////// Map Audio Level and move the mouth
    //soundVal = map(soundVal, soundMin, soundMax, mouthClosed, soundAmplitude);
    //soundVal = map(soundVal, (soundMax-soundAmplitude), soundMax, mouthClosed, mouthOpened);
    soundVal = map(soundVal, silenceVal, soundMax, mouthClosed, mouthOpened);
    myServo.write(soundVal);
    soundMax=100;
    soundMin=500;
   }
}



// ***********************************************************
// ********************** getDist() **************************
// ***Code from Arduino ping *********************************
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

// Original code from the ping sensor library
long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}



