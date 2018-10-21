// =======================================================================================
//                 SHADOW :  Small Handheld Arduino Droid Operating Wand
// =======================================================================================
//                          Last Revised Date: 10/05/14
//                             Written By: KnightShade
//                        Inspired by the PADAWAN by danf
//                              Hacked By: CJE1985
// =======================================================================================
//
//         This program is free software: you can redistribute it and/or modify it .
//         This program is distributed in the hope that it will be useful,
//         but WITHOUT ANY WARRANTY; without even the implied warranty of
//         MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// =======================================================================================
//   Note: You will need a Arduino Mega 2560 or Uno to run this sketch
//
//   This is written to be a UNIVERSAL Sketch - supporting multiple controller options
//      - Single PS3 Move Navigation
//      - Pair of PS3 Move Navigation
//
//   PS3 Bluetooth library - developed by Kristian Lauszus (kristianl@tkjelectronics.com)
//   For more information visit my blog: http://blog.tkjelectronics.dk/ or
//
// =======================================================================================
//        Updated:  12/26/2017
// ---------------------------------------------------------------------------------------
//                          User Settings
// ---------------------------------------------------------------------------------------

//Primary Controller
String PS3MoveNavigatonPrimaryMAC = "04:76:6E:5A:9A:5B"; //If using multiple controlers, designate a primary

byte joystickDeadZoneRange = 10;  // For controllers that centering problems, use the lowest number with no drift

int steeringNeutral = 90;        // Move this by one or two to set the center point for the steering servo

#define reverseSteering          // Comment this to have normal stering direction (Lots of RC cars are built with reverseSteering by default)
int steeringRightEndpoint = 120; // Move this down (below 180) if you need to set a narrower Right turning radius
int steeringLeftEndpoint = 0;    // Move this up (above 0) if you need to set a narrower Left turning radius

int driveNeutral = 91;           // Move this by one or two to set the center point for the drive ESC
int maxForwardSpeed = 115;       // Move this down (below 180, but above 90) if you need a slower forward speed
int maxReverseSpeed = 65;        // Move this up (above 0, but below 90) if you need a slower reverse speed

//#define TEST_CONROLLER         //Support coming soon
//#define SHADOW_DEBUG           //uncomment this for console DEBUG output
#define SHADOW_VERBOSE           //uncomment this for console VERBOSE output

// ---------------------------------------------------------------------------------------
//                          Drive Controller Settings
// ---------------------------------------------------------------------------------------

#define steeringPin 4            // connect this pin to steering servo for MSE (R/C mode)
#define drivePin 3               // connect this pin to ESC for forward/reverse drive (R/C mode)
#define L2Throttle               // comment this to use Joystick throttle (instead of L2 throttle)

// ---------------------------------------------------------------------------------------
//                          Sound Control Settings
// ---------------------------------------------------------------------------------------

#define MP3TxPin 2               // connect this pin to the MP3 Trigger
#define MP3RxPin 8               // This pin isn't used by the sparkfun MP3 trigger, but is used by the MDFly
//#define Sparkfun               // Use the sparkfun MP3 Trigger
#define MDFly                    // Use the MDFly MP3 Player
//#define RogueRMP3              // Use the Rogue RMP3 Player


// ---------------------------------------------------------------------------------------
//                          Libraries
// ---------------------------------------------------------------------------------------
#include <PS3BT.h>
#include <Servo.h>


#ifdef RogueRMP3
#include <RogueMP3.h>
#include <SoftwareSerial.h>
SoftwareSerial rmp3_s = SoftwareSerial(MP3TxPin, MP3RxPin);
RogueMP3 MP3(rmp3_s);
#endif


#ifdef Sparkfun
#include <MP3Trigger.h>
#include <SoftwareSerial.h>
SoftwareSerial swSerial = SoftwareSerial(MP3RxPin, MP3TxPin);
MP3Trigger MP3;
#endif

#ifdef MDFly
#include <serMP3.h>
serMP3 MP3(MP3TxPin, MP3RxPin);
#endif

// ---------------------------------------------------------------------------------------
//                          Variables
// ---------------------------------------------------------------------------------------

long previousMillis = millis();
long currentMillis = millis();
int serialLatency = 25;          // This is a delay factor in ms to prevent queueing of the Serial data.
                                 // 25ms seems approprate for HardwareSerial, values of 50ms or larger
                                 // are needed for Softare Emulation

///////Setup for USB and Bluetooth Devices////////////////////////////
USB Usb;
BTD Btd(&Usb);                   // You have to create the Bluetooth Dongle instance like so
PS3BT *PS3Nav = new PS3BT(&Btd);
PS3BT *PS3Nav2 = new PS3BT(&Btd);
//Used for PS3 Fault Detection
uint32_t msgLagTime = 0;
uint32_t lastMsgTime = 0;
uint32_t currentTime = 0;
uint32_t lastLoopTime = 0;
int badPS3Data = 0;

boolean firstMessage = true;
String output = "";

boolean isDriveMotorStopped = true;

boolean isPS3NavigatonInitialized = false;
boolean isSecondaryPS3NavigatonInitialized = false;

byte vol = 0;                   // 0 = full volume, 255 off for MP3Trigger, the MDFly use a 0-31 vol range
boolean isStickEnabled = true;

byte action = 0;
unsigned long DriveMillis = 0;

Servo steeringSignal;
Servo driveSignal;
int steeringValue, driveValue;   //will hold steering/drive values (-100 to 100)
int prevSteeringValue, prevDriveValue; //will hold steering/drive speed values (-100 to 100)


// =======================================================================================
//                          Main Program
// =======================================================================================

void setup()
{
  //Debug Serial for use with USB Debugging
  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
  if (Usb.Init() == -1)
  {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nBluetooth Library Started"));
  output.reserve(200); // Reserve 200 bytes for the output string

  //Setup for PS3
  PS3Nav->attachOnInit(onInitPS3); // onInit() is called upon a new connection - you can call the function whatever you like
  PS3Nav2->attachOnInit(onInitPS3Nav2);


#ifdef RogueRMP3
  rmp3_s.begin(9600);
  MP3.sync();
  MP3.stop();
  MP3.setVolume(vol);
  Serial.print(F("\r\nRogue rMP3 Initialized"));
#endif


#ifdef Sparkfun
  //Setup for SoftwareSerial/MP3 Trigger
  MP3.setup(&swSerial);
  swSerial.begin(MP3Trigger::serialRate());
  MP3.setVolume(vol);
  Serial.print(F("\r\nSparkfun MP3 Trigger Initialized"));
#endif

#ifdef MDFly
  MP3.begin(vol);
  Serial.print(F("\r\nMDFly MP3 Initialized"));
#endif

  Serial.print(F("\r\nMSE Drive Running"));
  steeringSignal.attach(steeringPin);
  driveSignal.attach(drivePin);
}

boolean readUSB()
{
  //The more devices we have connected to the USB or BlueTooth, the more often Usb.Task need to be called to eliminate latency.
  Usb.Task();
  if (PS3Nav->PS3NavigationConnected ) Usb.Task();
  if (PS3Nav2->PS3NavigationConnected ) Usb.Task();
  if ( criticalFaultDetect() )
  {
    //We have a fault condition that we want to ensure that we do NOT process any controller data!!!
    flushAndroidTerminal();
    return false;
  }
  return true;
}

void loop()
{
  //Useful to enable with serial console when having controller issues.
#ifdef TEST_CONROLLER
  testPS3Controller();
#endif

  //LOOP through functions from highest to lowest priority.

  if ( !readUSB() )
  {
    //We have a fault condition that we want to ensure that we do NOT process any controller data!!!
    return;
  }
  Drive();

  if ( !readUSB() )
  {
    //We have a fault condition that we want to ensure that we do NOT process any controller data!!!
    return;
  }

  toggleSettings();
  soundControl();
  flushAndroidTerminal();
}

void onInitPS3()
{
  String btAddress = getLastConnectedBtMAC();
  PS3Nav->setLedOn(LED1);
  isPS3NavigatonInitialized = true;
  badPS3Data = 0;
#ifdef SHADOW_DEBUG
  output += "\r\nBT Address of Last connected Device when Primary PS3 Connected: ";
  output += btAddress;
  if (btAddress == PS3MoveNavigatonPrimaryMAC)
  {
    output += "\r\nWe have our primary controller connected.\r\n";
  }
  else
  {
    output += "\r\nWe have a controller connected, but it is not designated as \"primary\".\r\n";
  }
#endif
}

void onInitPS3Nav2()
{
  String btAddress = getLastConnectedBtMAC();
  PS3Nav2->setLedOn(LED1);
  isSecondaryPS3NavigatonInitialized = true;
  badPS3Data = 0;
  if (btAddress == PS3MoveNavigatonPrimaryMAC) swapPS3NavControllers();
#ifdef SHADOW_DEBUG
  output += "\r\nBT Address of Last connected Device when Secondary PS3 Connected: ";
  output += btAddress;
  if (btAddress == PS3MoveNavigatonPrimaryMAC)
  {
    output += "\r\nWe have our primary controller connecting out of order.  Swapping locations\r\n";
  }
  else
  {
    output += "\r\nWe have a secondary controller connected.\r\n";
  }
#endif
}

String getLastConnectedBtMAC()
{
  String btAddress = "";
  for (int8_t i = 5; i > 0; i--)
  {
    if (Btd.disc_bdaddr[i] < 0x10)
    {
      btAddress += "0";
    }
    btAddress += String(Btd.disc_bdaddr[i], HEX);
    btAddress += (":");
  }
  btAddress += String(Btd.disc_bdaddr[0], HEX);
  btAddress.toUpperCase();
  return btAddress;
}

void swapPS3NavControllers()
{
  PS3BT* temp = PS3Nav;
  PS3Nav = PS3Nav2;
  PS3Nav2 = temp;
  //Correct the status for Initialization
  boolean tempStatus = isPS3NavigatonInitialized;
  isPS3NavigatonInitialized = isSecondaryPS3NavigatonInitialized;
  isSecondaryPS3NavigatonInitialized = tempStatus;
  //Must relink the correct onInit calls
  PS3Nav->attachOnInit(onInitPS3);
  PS3Nav2->attachOnInit(onInitPS3Nav2);
}

void flushAndroidTerminal()
{
  if (output != "")
  {
    if (Serial) Serial.println(output);
    output = ""; // Reset output string
  }
}

// =======================================================================================
// //////////////////////////Process PS3 Controller Fault Detection///////////////////////
// =======================================================================================
boolean criticalFaultDetect()
{
  if (PS3Nav->PS3NavigationConnected || PS3Nav->PS3Connected)
  {
    lastMsgTime = PS3Nav->getLastMessageTime();
    currentTime = millis();
    if ( currentTime >= lastMsgTime)
    {
      msgLagTime = currentTime - lastMsgTime;
    } else
    {
#ifdef SHADOW_DEBUG
      output += "Waiting for PS3Nav Controller Data\r\n";
#endif
      badPS3Data++;
      msgLagTime = 0;
    }

    if (msgLagTime > 100 && !isDriveMotorStopped)
    {
#ifdef SHADOW_DEBUG
      output += "It has been 100ms since we heard from the PS3 Controller\r\n";
      output += "Shut downing motors, and watching for a new PS3 message\r\n";
#endif

      driveSignal.write(driveNeutral);
      steeringSignal.write(steeringNeutral);
      isDriveMotorStopped = true;
      return true;
    }
    if ( msgLagTime > 30000 )
    {
#ifdef SHADOW_DEBUG
      output += "It has been 30s since we heard from the PS3 Controller\r\n";
      output += "msgLagTime:";
      output += msgLagTime;
      output += "  lastMsgTime:";
      output += lastMsgTime;
      output += "  millis:";
      output += millis();
      output += "\r\nDisconnecting the controller.\r\n";
#endif
      PS3Nav->disconnect();
    }

    //Check PS3 Signal Data
    if (!PS3Nav->getStatus(Plugged) && !PS3Nav->getStatus(Unplugged))
    {
      // We don't have good data from the controller.
      //Wait 10ms, Update USB, and try again
      delay(10);
      Usb.Task();
      if (!PS3Nav->getStatus(Plugged) && !PS3Nav->getStatus(Unplugged))
      {
        badPS3Data++;
#ifdef SHADOW_DEBUG
        output += "\r\nInvalid data from PS3 Controller.";
#endif
        return true;
      }
    }
    else if (badPS3Data > 0)
    {
      //output += "\r\nPS3 Controller  - Recovered from noisy connection after: ";
      //output += badPS3Data;
      badPS3Data = 0;
    }
    if ( badPS3Data > 10 )
    {
#ifdef SHADOW_DEBUG
      output += "Too much bad data coming fromo the PS3 Controller\r\n";
      output += "Disconnecting the controller.\r\n";
#endif
      PS3Nav->disconnect();
    }
  }
  else if (!isDriveMotorStopped)
  {
#ifdef SHADOW_DEBUG
    output += "No Connected Controllers were found\r\n";
    output += "Shuting downing motors, and watching for a new PS3 message\r\n";
#endif

    driveSignal.write(driveNeutral);
    steeringSignal.write(steeringNeutral);
    isDriveMotorStopped = true;
    return true;
  }
  return false;
}
// =======================================================================================
// //////////////////////////END of PS3 Controller Fault Detection///////////////////////
// =======================================================================================


boolean ps3Drive(PS3BT* myPS3 = PS3Nav)
{
  if (isPS3NavigatonInitialized) {
    // Additional fault control.  Do NOT send additional commands if no controllers have initialized.
    if (!isStickEnabled) {
      #ifdef SHADOW_VERBOSE
      if ( abs(myPS3->getAnalogHat(LeftHatY) - 128) > joystickDeadZoneRange)
      {
        output += "Drive Stick is disabled\r\n";
      }
      #endif

      driveSignal.write(driveNeutral);
      steeringSignal.write(steeringNeutral);
      isDriveMotorStopped = true;
      
    } else if (!myPS3->PS3NavigationConnected) {
      driveSignal.write(driveNeutral);
      steeringSignal.write(steeringNeutral);
      isDriveMotorStopped = true;

    } else {
      int stickX = myPS3->getAnalogHat(LeftHatX);
      #ifdef L2Throttle
      int stickY = myPS3->getAnalogButton(L2);
      #else
      int stickY = myPS3->getAnalogHat(LeftHatY);
      #endif

      #ifdef L2Throttle
      // map the steering direction
      if (((stickX <= 128 - joystickDeadZoneRange) || (stickX >= 128 + joystickDeadZoneRange))) {
        #ifdef reverseSteering
          steeringValue = map(stickX, 0, 255, steeringRightEndpoint, steeringLeftEndpoint);
        #else
          steeringValue = map(stickX, 0, 255, steeringLeftEndpoint, steeringRightEndpoint);
        #endif
      } else {
        steeringValue = steeringNeutral;
        driveValue = driveNeutral;
      }

      // map the drive direction
      if (myPS3->getButtonPress(L1) && myPS3->getAnalogButton(L2)) {
        // These values must cross 90 (as that is stopped)
        // The closer these values are the more speed control you get
        driveValue = map(stickY, 0, 255, 90, maxReverseSpeed);
      } else if (myPS3->getAnalogButton(L2)) {
        // These values must cross 90 (as that is stopped)
        // The closer these values are the more speed control you get
        driveValue = map(stickY, 0, 255, 90, maxForwardSpeed);
      }
      #else
      if (((stickX <= 128 - joystickDeadZoneRange) || (stickX >= 128 + joystickDeadZoneRange)) ||
          ((stickY <= 128 - joystickDeadZoneRange) || (stickY >= 128 + joystickDeadZoneRange))) {
            #ifdef reverseSteering
              steeringValue = map(stickX, 0, 255, steeringRightEndpoint, steeringLeftEndpoint);
            #else
              steeringValue = map(stickX, 0, 255, steeringLeftEndpoint, steeringRightEndpoint);
            #endif
            // These values must cross 90 (as that is stopped)
            // The closer these values are the more speed control you get
            driveValue = map(stickY, 0, 255, maxForwardSpeed, maxReverseSpeed);
      } else {
        // stop all movement
        steeringValue = steeringNeutral;
        driveValue = driveNeutral;
      }
      #endif

      driveSignal.write(driveValue);
      steeringSignal.write(steeringValue);

      return true; //we sent a drive command
    }
  }
  return false;
}


void ps3ToggleSettings(PS3BT* myPS3 = PS3Nav)
{
  if (myPS3->getButtonPress(PS) && myPS3->getButtonClick(L3))
  {
    //Quick Shutdown of PS3 Controller
    output += "\r\nDisconnecting the controller.\r\n";
    myPS3->disconnect();
  }


  //// enable / disable right stick & play sound
  if (myPS3->getButtonPress(PS) && myPS3->getButtonClick(CROSS))
  {
#ifdef SHADOW_DEBUG
    output += "Disiabling the DriveStick\r\n";
#endif
    isStickEnabled = false;
    //        MP3.play(52);
  }
  if (myPS3->getButtonPress(PS) && myPS3->getButtonClick(CIRCLE))
  {
#ifdef SHADOW_DEBUG
    output += "Enabling the DriveStick\r\n";
#endif
    isStickEnabled = true;
    ///        MP3.play(53);
  }
}

void processSoundCommand(char soundCommand)
{
  switch (soundCommand)
  {
    case '-':
#ifdef SHADOW_DEBUG
      output += "Volume Down\r\n";
#endif
      if (vol > 0)
      {

#ifdef RogueRMP3
        vol = vol-10;
        if (vol < 0 ) { vol = 0; }
        MP3.setVolume(vol);
#endif

#ifdef MDFly
        vol--;
        MP3.setVol(vol);
#endif

#ifdef Sparkfun
        vol = vol-5;
        if (vol < 0 ) { vol = 0 ;}
        MP3.setVolume(vol);
#endif

      }
      break;
    case '+':
#ifdef SHADOW_DEBUG
      output += "Volume Up\r\n";
#endif

#ifdef RogueRMP3
        if (vol < 255) {
          vol = vol+10;
          if (vol > 255 ) { vol = 255 ;}
          MP3.setVolume(vol);
        }
#endif

#ifdef MDFly
      // The TDB380 and 381 use a 0-31 vol range
      if (vol < 31) {
          vol++;
          MP3.setVol(vol);
      }
#endif

#ifdef Sparkfun
        // The MP3Trigger use a 0-255 vol range
        if (vol < 255) {
          vol = vol+5;
          if (vol > 255 ) { vol = 255 };
          MP3.setVolume(vol);
        }
#endif

        break;

      case '1':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Scream\r\n";
#endif

#ifdef MDFly
        MP3.play(1);
#endif

#ifdef Sparkfun
        MP3.trigger(1);
#endif

#ifdef RogueRMP3
        MP3.playFile("/MSE01.mp3");
#endif


        break;
      case '2':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Doo Doo.\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/MSE02.mp3");
#endif

#ifdef MDFly
        MP3.play(2);
#endif

#ifdef Sparkfun
        MP3.trigger(2);
#endif

        break;
      case '3':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Scramble\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/MSE03.mp3");
#endif

#ifdef MDFly
        MP3.play(3);
#endif

#ifdef Sparkfun
        MP3.trigger(3);
#endif

        break;
        case '4':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Scramble\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/Mouse Droid Song.mp3");
#endif

#ifdef MDFly
        MP3.play(4);
#endif

#ifdef Sparkfun
        MP3.trigger(4);
#endif

        break;
      case '5':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Mouse Sound.\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/MSE04.mp3");
#endif

#ifdef MDFly
        MP3.play(5);
#endif

#ifdef Sparkfun
        MP3.trigger(5);
#endif

        break;
      case '6':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Crank Sound.\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/MSE05.mp3");
#endif

#ifdef MDFly
        MP3.play(6);
#endif

#ifdef Sparkfun
        MP3.trigger(6);
#endif

        break;
      case '7':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Splat.\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/MSE06.mp3");
#endif

#ifdef MDFly
        MP3.play(7);
#endif

#ifdef Sparkfun
        MP3.trigger(7);
#endif

        break;
      case '8':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Electrical.\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/March.mp3");
#endif

#ifdef MDFly
        MP3.play(8);
#endif

#ifdef Sparkfun
        MP3.trigger(15);
#endif

        break;
      case '9':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play March L1+L.\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/MSE10.mp3");
#endif

#ifdef MDFly
        MP3.play(9);
#endif

#ifdef Sparkfun
        MP3.trigger(18);
#endif

        break;
      case '0':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Cantina L1+R\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/MSE11.mp3");
#endif

#ifdef MDFly
        MP3.play(10);
#endif

#ifdef Sparkfun
        MP3.trigger(19);
#endif

        break;
      case 'A':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Jabba Flow L1+O\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/Cantina.mp3");
#endif

#ifdef MDFly
        MP3.play(11);
#endif

#ifdef Sparkfun
        MP3.trigger(20);
#endif

        break;
      case 'B':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play SWG Music L1+X\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/March.mp3");
#endif

#ifdef MDFly
        MP3.play(12);
#endif

#ifdef Sparkfun
        MP3.trigger(21);
#endif

        break;
      case 'C':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play MSE Loop L1+L3\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/StarWars.mp3");
#endif

#ifdef MDFly
        MP3.play(13);
#endif

#ifdef Sparkfun
        MP3.trigger(22);
#endif

        break;
      case 'D':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Meco StarWars L1+PS\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/Mouse Droid Song.mp3");
#endif

#ifdef MDFly
        MP3.play(14);
#endif

#ifdef Sparkfun
        MP3.trigger(23);
#endif

        break;
      case 'E':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Meco StarWars L1+PS\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/meco-sw-disco.mp3");
#endif

#ifdef MDFly
        MP3.play(15);
#endif

#ifdef Sparkfun
        MP3.trigger(23);
#endif

        break;
      case 'F':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Meco StarWars L1+PS\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/the saga begins - wierd al yankovic.mp3");
#endif

#ifdef MDFly
        MP3.play(16);
#endif

#ifdef Sparkfun
        MP3.trigger(23);
#endif

        break;
      case 'R':
#ifdef SHADOW_DEBUG
        output += "Sound Button ";
        output += soundCommand;
        output += " - Play Random Mouse Sound. L3\r\n";
#endif

#ifdef RogueRMP3
        MP3.playFile("/WALLE.mp3");
#endif

#ifdef MDFly
        MP3.play(random(1, 15));
#endif

#ifdef Sparkfun
        MP3.trigger(random(1, 15));
#endif

        break;
      default:
#ifdef SHADOW_DEBUG
        output += "Invalid Sound Command\r\n";
#endif

        output += "Invalid Sound Command\r\n";

#ifdef RogueRMP3
        //Add a valid track here?
        MP3.playFile("/WALLE.mp3");
#endif

#ifdef MDFly
        MP3.play(60);
#endif

#ifdef Sparkfun
        MP3.trigger(60);
#endif
      }
  }

  void ps3soundControl(PS3BT* myPS3 = PS3Nav, int controllerNumber = 1)
  {
    if (!(myPS3->getButtonPress(L1) || myPS3->getButtonPress(L2) || myPS3->getButtonPress(PS))) {
      if (myPS3->getButtonClick(UP))          processSoundCommand('1');
      else if (myPS3->getButtonClick(RIGHT))  processSoundCommand('2');
      else if (myPS3->getButtonClick(DOWN))   processSoundCommand('3');
      else if (myPS3->getButtonClick(LEFT))   processSoundCommand('4');
      else if (myPS3->getButtonClick(CROSS))  processSoundCommand('5');
      else if (myPS3->getButtonClick(CIRCLE)) processSoundCommand('6');
      else if (myPS3->getButtonClick(L3))     processSoundCommand('R');
    }
    else if (myPS3->getButtonPress(L2)) {
      if (myPS3->getButtonClick(UP))          processSoundCommand('7');
      else if (myPS3->getButtonClick(RIGHT))  processSoundCommand('8');
      else if (myPS3->getButtonClick(DOWN))   processSoundCommand('9');
      else if (myPS3->getButtonClick(LEFT))   processSoundCommand('0');
      else if (myPS3->getButtonClick(CIRCLE)) processSoundCommand('A');
      else if (myPS3->getButtonClick(CROSS))  processSoundCommand('B');
      else if (myPS3->getButtonClick(L3))     processSoundCommand('R');
    }
    else if (myPS3->getButtonPress(L1)) {
      if (myPS3->getButtonClick(UP))          processSoundCommand('+');
      else if (myPS3->getButtonClick(DOWN))   processSoundCommand('-');
      else if (myPS3->getButtonClick(LEFT))   processSoundCommand('C');
      else if (myPS3->getButtonClick(RIGHT))  processSoundCommand('D');
      else if (myPS3->getButtonClick(CIRCLE)) processSoundCommand('E');
      else if (myPS3->getButtonClick(CROSS))  processSoundCommand('F');
      else if (myPS3->getButtonClick(L3))     processSoundCommand('R');
      else if (myPS3->getButtonClick(PS))     processSoundCommand('D');
    }
  }

  void Drive()
  {
    //Flood control prevention
    if ((millis() - previousMillis) < serialLatency) return;
    if (PS3Nav->PS3NavigationConnected) ps3Drive(PS3Nav);
    //TODO:  Drive control must be mutually exclusive - for safety
    //Future: I'm not ready to test that before FanExpo
    //if (PS3Nav2->PS3NavigationConnected) ps3Drive(PS3Nav2);
  }

  void toggleSettings()
  {
    if (PS3Nav->PS3NavigationConnected) ps3ToggleSettings(PS3Nav);
    if (PS3Nav2->PS3NavigationConnected) ps3ToggleSettings(PS3Nav2);
  }

  void soundControl()
  {
    if (PS3Nav->PS3NavigationConnected) ps3soundControl(PS3Nav, 1);
    if (PS3Nav2->PS3NavigationConnected) ps3soundControl(PS3Nav2, 2);
  }

#ifdef TEST_CONROLLER
  void testPS3Controller(PS3BT* myPS3 = PS3Nav)
  {
    if (myPS3->PS3Connected || myPS3->PS3NavigationConnected) {
      if (myPS3->getButtonPress(PS) && (myPS3->getAnalogHat(LeftHatX) > 137 || myPS3->getAnalogHat(LeftHatX) < 117 || myPS3->getAnalogHat(LeftHatY) > 137 || myPS3->getAnalogHat(LeftHatY) < 117 || myPS3->getAnalogHat(RightHatX) > 137 || myPS3->getAnalogHat(RightHatX) < 117 || myPS3->getAnalogHat(RightHatY) > 137 || myPS3->getAnalogHat(RightHatY) < 117)) {
        output += "LeftHatX: ";
        output += myPS3->getAnalogHat(LeftHatX);
        output += "\tLeftHatY: ";
        output += myPS3->getAnalogHat(LeftHatY);
        if (myPS3->PS3Connected) { // The Navigation controller only have one joystick
          output += "\tRightHatX: ";
          output += myPS3->getAnalogHat(RightHatX);
          output += "\tRightHatY: ";
          output += myPS3->getAnalogHat(RightHatY);
        }
      }
      //Analog button values can be read from almost all buttons
      if (myPS3->getButtonPress(PS) && (myPS3->getAnalogButton(L2) || myPS3->getAnalogButton(R2)))
      {
        if (output != "")
          output += "\r\n";
        output += "L2: ";
        output += myPS3->getAnalogButton(L2);
        if (myPS3->PS3Connected) {
          output += "\tR2: ";
          output += myPS3->getAnalogButton(R2);
        }
      }
      if (myPS3->getButtonClick(L2)) {
        output += " - L2";
        //myPS3->disconnect();
      }
      if (myPS3->getButtonClick(R2)) {
        output += " - R2";
        //myPS3->disconnect();
      }
      if (output != "") {
        Serial.println(output);
        output = ""; // Reset output string
      }
      if (myPS3->getButtonClick(PS)) {
        output += " - PS";
        //myPS3->disconnect();
      }
      else {
        if (myPS3->getButtonClick(TRIANGLE))
          output += " - Traingle";
        if (myPS3->getButtonClick(CIRCLE))
          output += " - Circle";
        if (myPS3->getButtonClick(CROSS))
          output += " - Cross";
        if (myPS3->getButtonClick(SQUARE))
          output += " - Square";

        if (myPS3->getButtonClick(UP)) {
          output += " - Up";
          if (myPS3->PS3Connected) {
            myPS3->setLedOff();
            myPS3->setLedOn(LED4);
          }
        }
        if (myPS3->getButtonClick(RIGHT)) {
          output += " - Right";
          if (myPS3->PS3Connected) {
            myPS3->setLedOff();
            myPS3->setLedOn(LED1);
          }
        }
        if (myPS3->getButtonClick(DOWN)) {
          output += " - Down";
          if (myPS3->PS3Connected) {
            myPS3->setLedOff();
            myPS3->setLedOn(LED2);
          }
        }
        if (myPS3->getButtonClick(LEFT)) {
          output += " - Left";
          if (myPS3->PS3Connected) {
            myPS3->setLedOff();
            myPS3->setLedOn(LED3);
          }
        }

        if (myPS3->getButtonClick(L1))
          output += " - L1";
        if (myPS3->getButtonClick(L3))
          output += " - L3";
        if (myPS3->getButtonClick(R1))
          output += " - R1";
        if (myPS3->getButtonClick(R3))
          output += " - R3";

        if (myPS3->getButtonClick(SELECT)) {
          output += " - Select";
          myPS3->printStatusString();
        }
        if (myPS3->getButtonClick(START)) {
          output += " - Start";
        }
      }
    }
  }
#endif
