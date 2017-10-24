/***************************************************
  LCBB_Talking-Skull_dfplayer-test
  By tweeks-pub1@theweeks.org

  Based on open source library:
    DFPlayer - A Mini MP3 Player For Arduino
    <https://www.dfrobot.com/index.php?route=product/product&product_id=1121>
    <Angelo.qiao@dfrobot.com>
 ***************************************************

Test code for controlling the DFPlayer Mini MP3 player over 3.3v serial
lines.  

WARNING: Must use voltage dropping 1k resistors on software serial TX/RX lines
to the DFPlayer.
*/
  
  // Hardware wiring of DFPlay
  //                                     
  // DFPlay mini  --Vcc(1)--RX(2)--TX(3)--DACR(4)--GND(7)  +(6) -(8)  ...  _BUSY
  //                 |       |      |       |       |      |    |           |
  //     Arduino    5v      11*    10*     A0      GND     |    |          12*     
  //                                                       \Spkr/
  //                              * requires 1k resisitor
  //     
 

#include "Arduino.h"
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>


/////////// HARDWARE SETTINGS/HOOKUPS ////////

//DFPlayer mp3 player
const int ardRX=10;       // The arduino software Receive lins (goes to DFPlayer TX)
const int ardTX=11;       // The arduino software Transmit line (goes to DFPlayer RX)
const int dfBusy=12;      // Coming from DFPlayer pin 16.. is active low (HIGH = not playing a sound)
int sndFile=1;            // Sound file pointer.
int fileCount=9;          // Either set the max # of files here, or load it from myDFPlayer.readFileCounts()
                          // NOTE: readFileCounts() also sees deleted files. May need to reformat to load new sounds.
int setVol=25;            //Set volume value (0~30)

// Software Serial Pins To DFPlayer
SoftwareSerial mySoftwareSerial(10,11); // RX, TX  

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);


/////////////////////////////////////////////////////////////////////
/////////////////////// LARGE SETUP BLOLCK //////////////////////////
/////////////////////////////////////////////////////////////////////
void setup(){
  
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
    while(true);      // Hang forever if error
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

  ////// If at the last sound file, then loop back to the top (1)
  if (sndFile == (fileCount+1) ) {
    sndFile = 1;
  }

  ////// Begin playing the next sound file...
  Serial.println("************** PLAYING ****************");
  myDFPlayer.play(sndFile);   //Play the next mp3
  delay(100);                 // wait to start


  ////// While DFPlayer _BUSY line is active(low), use preferred laugh function.
  while(!digitalRead(dfBusy) == true ){
    Serial.println("still playing...");
    Serial.println("Good place to flash eyes & move mouth... ;)");
    delay(300);
  }

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



