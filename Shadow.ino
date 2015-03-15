// =======================================================================================
//                 SHADOW :  Small Handheld Arduino Droid Operating Wand
// =======================================================================================
//                          Last Revised Date: 10/05/14
//                             Written By: KnightShade
//                        Inspired by the PADAWAN by danf
// =======================================================================================
//
//         This program is free software: you can redistribute it and/or modify it .
//         This program is distributed in the hope that it will be useful,
//         but WITHOUT ANY WARRANTY; without even the implied warranty of
//         MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// =======================================================================================
//   Note: You will need a Arduino Mega 1280/2560 to run this sketch,
//   as a normal Arduino (Uno, Duemilanove etc.) doesn't have enough SRAM and FLASH
//
//   This is written to be a UNIVERSAL Sketch - supporting multiple controller options
//      - Single PS3 Move Navigation
//      - Pair of PS3 Move Navigation
//      - Android Phone (Limited Controls)
//      Future Planned Enhancements:
//      - XBox 360 Controller  (Why not, these also uses the USB Host Shield)
//      - PS3 Dual Shock Controller
//      - PS4 Dual Shock Controller
//
//   PS3 Bluetooth library - developed by Kristian Lauszus (kristianl@tkjelectronics.com)
//   For more information visit my blog: http://blog.tkjelectronics.dk/ or
//
//   Holoprojector Support:
//      Legacy Holoprojector Support:  was based on Padawan, using a second Arduino (Teeces)
//          This used a Padawan Dome sketch that was loaded to the Teeces Logics.
//          It leveraged the EasyTransfer libraries by Bill Porter
//          Legacy support will likely be deprecated and removed in time
//      Long Term Holoprojector Support:
//          SHADOW control will be isolated from particular logic hardware.
//          We will migrate to I2C dome commands with PWM support:
//          Holoprojector Servos and LEDs will be driven by:  
//            http://www.adafruit.com/product/815
//            This can drive 6 servos, and 3 LEDs.  PWM will allow for LED brightness "flicker"
//  
//   Sabertooth (Foot Drive):
//         Set Sabertooth 2x32 or 2x25 Dip Switches: 1 and 2 Down, All Others Up
//
//   SyRen 10 Dome Drive:
//         For SyRen packetized Serial Set Switches: 1, 2 and 4 Down, All Others Up
//         NOTE:  Support for SyRen Simple Serial has been removed, due to problems.
//         Please contact DimensionEngineering to get an RMA to flash your firmware
//         Some place a 10K ohm resistor between S1 & GND on the SyRen 10 itself
//
// =======================================================================================
//
// ---------------------------------------------------------------------------------------
//                          User Settings
// ---------------------------------------------------------------------------------------

//Primary Controller bound to Gmyle Class 1 Adapter 
//String PS3MoveNavigatonPrimaryMAC = "04:76:6E:87:B0:F5"; //If using multiple controlers, designate a primary

//Primary Controller bound to Parani UD-100 
String PS3MoveNavigatonPrimaryMAC = "00:07:04:05:EA:DF"; //If using multiple controlers, designate a primary


byte drivespeed1 = 70;   //set these 3 to whatever speeds work for you. 0-stop, 127-full speed.
byte drivespeed2 = 127;  //Recommend beginner: 50 to 75, experienced: 100 to 127, I like 100.

byte turnspeed = 75; //50;     // the higher this number the faster it will spin in place, lower - easier to control.
                         // Recommend beginner: 40 to 50, experienced: 50 $ up, I like 75

byte domespeed = 100;    // If using a speed controller for the dome, sets the top speed
                         // Use a number up to 127 for serial

byte ramping = 6; //3;        // Ramping- the lower this number the longer R2 will take to speedup or slow down,
                         // change this by increments of 1

byte joystickFootDeadZoneRange = 15;  // For controllers that centering problems, use the lowest number with no drift
byte joystickDomeDeadZoneRange = 10;  // For controllers that centering problems, use the lowest number with nfo drift
byte driveDeadBandRange = 10;     // Used to set the Sabertooth DeadZone for foot motors

int invertTurnDirection = -1;   //This may need to be set to 1 for some configurations

//#define TEST_CONROLLER   //Support coming soon
#define SHADOW_DEBUG       //uncomment this for console DEBUG output
#define SHADOW_VERBOSE     //uncomment this for console VERBOSE output

// ---------------------------------------------------------------------------------------
//                          Drive Controller Settings
// ---------------------------------------------------------------------------------------

int motorControllerBaudRate = 9600; // Set the baud rate for the Syren motor controller
                                    // for packetized options are: 2400, 9600, 19200 and 38400
                                    
#define SYREN_ADDR         129      // Serial Address for Dome Syren
#define SABERTOOTH_ADDR    128      // Serial Address for Foot Sabertooth


// ---------------------------------------------------------------------------------------
//                          Sound Settings
// ---------------------------------------------------------------------------------------
//Uncomment one line based on your sound system
#define SOUND_CFSOUNDIII     //Original system tested with SHADOW
//#define SOUND_MP3TRIGGER   //Code Tested by Dave C. and Marty M.
//#define SOUND_ROGUE_RMP3   //Support coming soon
//#define SOUND_RASBERRYPI   //Support coming soon

//#define EXTRA_SOUNDS


// ---------------------------------------------------------------------------------------
//                          Dome Control System
// ---------------------------------------------------------------------------------------
//Uncomment one line based on your Dome Control
#define DOME_I2C_ADAFRUIT       //Current SHADOW configuration used with R-Series Logics
//#define DOME_SERIAL_TEECES    //Original system tested with SHADOW
//#define DOME_I2C_TEECES       //Untested Nov 2014




// ---------------------------------------------------------------------------------------
//                          Utility Arm Settings
// ---------------------------------------------------------------------------------------

//Utility Arm Contribution by Dave C.
//TODO:  Move PINS to upper part of Mega for Shield purposes
const int UTILITY_ARM_BOTTOM_PIN  = 9;
const int UTILITY_ARM_TOP_PIN   = 11;

int utilArmClosedPos = 0;    // variable to store the servo closed position 
int utilArmOpenPos = 140;    // variable to store the servo Opened position 

// Check value, open = true, closed = false
boolean isUtilArmTopOpen = false;    
boolean isUtilArmBottomOpen = false;

int UtilArmBottomPos = 0;
int UtilArmTopPos = 0;

const int UTIL_ARM_TOP = 1;
const int UTIL_ARM_BOTTOM = 2;

// ---------------------------------------------------------------------------------------
//                          LED Settings
// ---------------------------------------------------------------------------------------

//Coin Slot LED Contribution by Dave C.
//TODO:  Move PINS to upper part of Mega for Shield purposes
#define numberOfCoinSlotLEDs 3
int COIN_SLOT_LED_PINS[] = { 2, 3, 4 }; // LED pins to use.
long nextCoinSlotLedFlash[numberOfCoinSlotLEDs]; // Array indicating which LED to flash next.
int coinSlotLedState[numberOfCoinSlotLEDs]; // Array indicating the state of the LED's.


// ---------------------------------------------------------------------------------------
//                          Libraries
// ---------------------------------------------------------------------------------------
#include <PS3BT.h>
#include <SPP.h>
#include <usbhub.h>
// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <Sabertooth.h>
#include <Servo.h>
#include <LedControl.h>

#ifdef DOME_SERIAL_TEECES    
#include <EasyTransfer.h>
#endif

#ifdef DOME_I2C_TEECES    
#include <EasyTransferI2C.h>
#endif

#ifdef DOME_I2C_ADAFRUIT    
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
//#include <Servos.h>  //Attempted to use the "SlowServo library from BHD.... had issues
#endif

//This is the traditional sound controler that has been used with PADAWAN
#ifdef SOUND_MP3TRIGGER
#include <MP3Trigger.h>
MP3Trigger trigger;
#endif

//Custom written Libraryy for the old CFSoundIII to emulate 12 button remote
//CFSoundIII needs a supporting CFSOUND.BAS version running on the CFSoundIII 
#ifdef SOUND_CFSOUNDIII
#include <CFSoundIII.h>
CFSoundIII cfSound;
#endif

//#ifdef  SOUND_ROGUE_RMP3
//TODO:add rMP3 support
//#endif
//#ifdef  SOUND_RASBERRYPI
//TODO:add Raspberry Pi Sound support
//#endif


// ---------------------------------------------------------------------------------------
//                          Variables
// ---------------------------------------------------------------------------------------

long previousDomeMillis = millis();
long previousFootMillis = millis();
long currentMillis = millis();
int serialLatency = 25;   //This is a delay factor in ms to prevent queueing of the Serial data.
                          //25ms seems approprate for HardwareSerial, values of 50ms or larger are needed for Softare Emulation

Sabertooth *ST=new Sabertooth(SABERTOOTH_ADDR, Serial2);
Sabertooth *SyR=new Sabertooth(SYREN_ADDR, Serial2);


#ifdef DOME_SERIAL_TEECES    
    EasyTransfer ET;
#endif

#ifdef DOME_I2C_TEECES    
    EasyTransferI2C ET;
#endif

#if defined(DOME_SERIAL_TEECES) || defined(DOME_I2C_TEECES)
    struct SEND_DATA_STRUCTURE
    {
        //put your variable definitions here for the data you want to send
        //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
        int hpx; // hp movement
        int hpy; // hp movement
        int hpl; // hp light
        int hpa; // hp automation
        int dsp; // 100=no change, 0=random, 1=alarm, 4=whistle, 5=leia, 6=short circut, 10=EQ, 11=alarm2,
    };         // 21=speed1, 22=speed2, 23=speed3, 24=logics+, 25=logics-
    SEND_DATA_STRUCTURE domeData;//give a name to the group of data
#endif 


#ifdef DOME_I2C_ADAFRUIT    
    const int HOLO_FRONT = 1;
    const int HOLO_BACK = 2;
    const int HOLO_TOP = 3;
    
    const int HOLO_DELAY = 20000; //up to 20 second delay
    const int PWM_OFF = 4095;  //setting from Adafruit
    
    const int HOLO_FRONT_RED_PWM_PIN     = 0;
    const int HOLO_FRONT_GREEN_PWM_PIN   = 1;
    const int HOLO_FRONT_BLUE_PWM_PIN    = 2;
    const int HOLO_FRONT_X_PWM_PIN       = 3;
    const int HOLO_FRONT_Y_PWM_PIN       = 4;
    const int HOLO_BACK_RED_PWM_PIN      = 5;
    const int HOLO_BACK_GREEN_PWM_PIN    = 6;
    const int HOLO_BACK_BLUE_PWM_PIN     = 7;
    const int HOLO_BACK_X_PWM_PIN        = 8;
    const int HOLO_BACK_Y_PWM_PIN        = 9;
    const int HOLO_TOP_X_PWM_PIN        = 10;
    const int HOLO_TOP_Y_PWM_PIN        = 11;
    const int HOLO_TOP_RED_PWM_PIN      = 12;
    const int HOLO_TOP_GREEN_PWM_PIN    = 13;
    const int HOLO_TOP_BLUE_PWM_PIN     = 14;


   const int HOLO_SERVO_CTR = 300;

    const int HOLO_FRONT_X_SERVO_MIN = 265; //250; //150;  // Issues with resin holo...
    const int HOLO_FRONT_X_SERVO_MAX = 315; //350; //600;  // Issues with resin holo...
    const int HOLO_FRONT_Y_SERVO_MIN = 250; //200; //150;  // Issues with resin holo...
    const int HOLO_FRONT_Y_SERVO_MAX = 330; //400; //600;  // Issues with resin holo...
    
    const int HOLO_BACK_X_SERVO_MIN = 275; //250; //150;
    const int HOLO_BACK_X_SERVO_MAX = 325; //350; //600; 
    const int HOLO_BACK_Y_SERVO_MIN = 250; //200; //150;
    const int HOLO_BACK_Y_SERVO_MAX = 350; //400; //600; 
    
    const int HOLO_TOP_X_SERVO_MIN = 275; //250; //150;
    const int HOLO_TOP_X_SERVO_MAX = 325; //350; //600; 
    const int HOLO_TOP_Y_SERVO_MIN = 250; //200; //150;
    const int HOLO_TOP_Y_SERVO_MAX = 350; //400; //600; 
    
    const int HOLO_LED_OFF = 0;
    const int HOLO_LED_ON = 1;
    const int HOLO_LED_FLICKER = 2;    
    int holoLightFrontStatus = 0;
    int holoLightBackStatus = 0;
    int holoLightTopStatus = 0;
   
    uint32_t holoFrontRandomTime = 0;
    uint32_t holoBackRandomTime = 0;
    uint32_t holoTopRandomTime = 0;
    Adafruit_PWMServoDriver domePWM = Adafruit_PWMServoDriver();
#endif




///////Setup for USB and Bluetooth Devices////////////////////////////
USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
PS3BT *PS3Nav=new PS3BT(&Btd);
PS3BT *PS3Nav2=new PS3BT(&Btd);
//Used for PS3 Fault Detection
uint32_t msgLagTime = 0;
uint32_t lastMsgTime = 0;
uint32_t currentTime = 0;
uint32_t lastLoopTime = 0;
int badPS3Data = 0;

SPP SerialBT(&Btd,"Astromech:R2","1977"); // Create a BT Serial device(defaults: "Arduino" and the pin to "0000" if not set)
boolean firstMessage = true;
String output = "";

boolean isFootMotorStopped = true;
boolean isDomeMotorStopped = true;

boolean isPS3NavigatonInitialized = false;
boolean isSecondaryPS3NavigatonInitialized = false;

byte vol = 50; // 0 = full volume, 255 off
boolean isStickEnabled = true;
byte isAutomateDomeOn = false;
unsigned long automateMillis = 0;
byte automateDelay = random(5,20);// set this to min and max seconds between sounds
int domeAutomationTurnDirection = 20;
byte action = 0;
unsigned long DriveMillis = 0;

Servo UtilArmTopServo;  // create servo object to control a servo 
Servo UtilArmBottomServo;  // create servo object to control a servo

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

    //The Arduino Mega has three additional serial ports: 
    // - Serial1 on pins 19 (RX) and 18 (TX), 
    // - Serial2 on pins 17 (RX) and 16 (TX), 
    // - Serial3 on pins 15 (RX) and 14 (TX). 

    //Setup for Serial1:: Sound 
    #ifdef SOUND_CFSOUNDIII
      cfSound.setup(&Serial1,2400);    
    #endif
    #ifdef SOUND_MP3TRIGGER
      trigger.setup(&Serial1);
      trigger.setVolume(vol);
    #endif

    //Setup for Serial2:: Motor Controllers - Syren (Dome) and Sabertooth (Feet) 
    Serial2.begin(motorControllerBaudRate);
    SyR->autobaud();
    SyR->setTimeout(300);      //DMB:  How low can we go for safety reasons?  multiples of 100ms

    //Setup for Sabertooth / Foot Motors
    ST->autobaud();          // Send the autobaud command to the Sabertooth controller(s).
    ST->setTimeout(300);      //DMB:  How low can we go for safety reasons?  multiples of 100ms
    ST->setDeadband(driveDeadBandRange);
    ST->stop(); 

    // NOTE: *Not all* Sabertooth controllers need the autobaud command.
    //       It doesn't hurt anything, but V2 controllers use an
    //       EEPROM setting (changeable with the function setBaudRate) to set
    //       the baud rate instead of detecting with autobaud.
    //
    //       If you have a 2x12, 2x25 V2, 2x60 or SyRen 50, you can remove
    //       the autobaud line and save yourself two seconds of startup delay.


    #ifdef DOME_I2C_ADAFRUIT           
        domePWM.begin();
        domePWM.setPWMFreq(50);  // Analog servos run at ~50 Hz updates
    #endif

    #ifdef DOME_SERIAL_TEECES
      //Setup for Serial3:: Dome Communication Link   
      Serial3.begin(57600);//start the library, pass in the data details and the name of the serial port.
      ET.begin(details(domeData), &Serial3);
    #endif
    
    #ifdef DOME_I2C_TEECES    
      Wire.begin();
      ET.begin(details(domeData), &Wire);
    #endif


    //Setup for Utility Arm Servo's    
    UtilArmTopServo.attach(UTILITY_ARM_TOP_PIN);  
    UtilArmBottomServo.attach(UTILITY_ARM_BOTTOM_PIN);
    closeUtilArm(UTIL_ARM_TOP);
    closeUtilArm(UTIL_ARM_BOTTOM);
    
    //Setup for Coin Slot LEDs    
    for(int i = 0; i<numberOfCoinSlotLEDs; i++)
    {
      pinMode(COIN_SLOT_LED_PINS[i],OUTPUT);
      coinSlotLedState[i] = LOW;
      digitalWrite(COIN_SLOT_LED_PINS[i], LOW); // all LEDs off
      nextCoinSlotLedFlash[i] = millis() +random(100, 1000);
    }     
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
    initAndroidTerminal();
    
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
    footMotorDrive();
        
    if ( !readUSB() )
    {
      //We have a fault condition that we want to ensure that we do NOT process any controller data!!!
      return;
    }
    automateDome();
    domeDrive();

    utilityArms();
    holoprojector();
    toggleSettings();
    soundControl();
    flashCoinSlotLEDs();
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
    for(int8_t i = 5; i > 0; i--)
    {
        if (Btd.disc_bdaddr[i]<0x10)
        {
            btAddress +="0";
        }
        btAddress += String(Btd.disc_bdaddr[i], HEX);
        btAddress +=(":");
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


void initAndroidTerminal()
{
    //Setup for Bluetooth Serial Monitoring
    if (SerialBT.connected)
    {
        if (firstMessage)
        {
            firstMessage = false;
            SerialBT.println(F("Hello from S.H.A.D.O.W.")); // Send welcome message
        }
        //TODO:  Process input from the SerialBT
        //if (SerialBT.available())
        //    Serial.write(SerialBT.read());
    }
    else
    {
        firstMessage = true;
    }
}

void flushAndroidTerminal()
{
    if (output != "")
    {
        if (Serial) Serial.println(output);
        if (SerialBT.connected)
            SerialBT.println(output);
            SerialBT.send();
        output = ""; // Reset output string
    }
}


void automateDome()
{
  /////////////automate
    if (isAutomateDomeOn)
    {
        //TODO:  We have other conditions where the dome is moving
        if ( abs(PS3Nav->getAnalogHat(LeftHatY)-128) > joystickFootDeadZoneRange)
        {
            automateMillis = millis();
        }
        currentMillis = millis();
        if (currentMillis - automateMillis > (automateDelay*1000))
        {
            automateMillis = millis();
            action = random(1,5);
            if (action>1)
            {
              //DMB:  Random Sounds, and Dome movement shouldn't be tied 1:1
              //TODO:  Add Sound Automation Routine
              #ifdef SOUND_MP3TRIGGER
              (trigger.play(random(32,52)));
              #endif
            }
            if (action<4)
            {
                  Serial.println("automation of Dome");			
                  rotateDome(domeAutomationTurnDirection,"Automation");
                  //DMB:  The delay statement below is a critial failing (IMO) as this locks out control.
                  //TODO:  Rework Dome Automation! 
                  delay(500);
                  rotateDome(0,"Automation");
                if (domeAutomationTurnDirection>0)
                {
                    domeAutomationTurnDirection = -45;
                }
                else
                {
                    domeAutomationTurnDirection = 45;
                }
            }
            automateDelay = random(5,20);// set this to min and max seconds between sounds
        }
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
        
        if (msgLagTime > 100 && !isFootMotorStopped)
        {
            #ifdef SHADOW_DEBUG
              output += "It has been 100ms since we heard from the PS3 Controller\r\n";
              output += "Shut downing motors, and watching for a new PS3 message\r\n";
            #endif
            ST->stop();
            SyR->stop();
            isFootMotorStopped = true;
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
        if(!PS3Nav->getStatus(Plugged) && !PS3Nav->getStatus(Unplugged))
        {
            // We don't have good data from the controller.
            //Wait 10ms, Update USB, and try again
            delay(10);
            Usb.Task();
            if(!PS3Nav->getStatus(Plugged) && !PS3Nav->getStatus(Unplugged))
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
    else if (!isFootMotorStopped)
    {
        #ifdef SHADOW_DEBUG      
            output += "No Connected Controllers were found\r\n";
            output += "Shuting downing motors, and watching for a new PS3 message\r\n";
        #endif
        ST->stop();
        SyR->stop();
        isFootMotorStopped = true;
        return true;
    }
    return false;
}
// =======================================================================================
// //////////////////////////END of PS3 Controller Fault Detection///////////////////////
// =======================================================================================


boolean ps3FootMotorDrive(PS3BT* myPS3 = PS3Nav)
{
  int footDriveSpeed = 0;
  int stickSpeed = 0;
  int turnnum = 0;

  if (isPS3NavigatonInitialized)
  {
      // Additional fault control.  Do NOT send additional commands to Sabertooth if no controllers have initialized.
      if (!isStickEnabled)
      {
            #ifdef SHADOW_VERBOSE
              if ( abs(myPS3->getAnalogHat(LeftHatY)-128) > joystickFootDeadZoneRange)
              {
                output += "Drive Stick is disabled\r\n";
              }
            #endif
          ST->stop();
          isFootMotorStopped = true;
      } else if (!myPS3->PS3NavigationConnected)
      {
          ST->stop();
          isFootMotorStopped = true;
      } else if ( myPS3->getButtonPress(L1) )
      {
          //TODO:  Does this need to change this when we support dual controller, or covered by improved isStickEnabled
          ST->stop();
          isFootMotorStopped = true;
      } else
      {
          int joystickPosition = myPS3->getAnalogHat(LeftHatY);
          isFootMotorStopped = false;
          if (myPS3->getButtonPress(L2))
          {
            int throttle = 0;
            if (joystickPosition < 127)
            {
                throttle = joystickPosition - myPS3->getAnalogButton(L2);
            } else
            {
                throttle = joystickPosition + myPS3->getAnalogButton(L2);
            }
            stickSpeed = (map(throttle, -255, 510, -drivespeed2, drivespeed2));                
          } else 
          {
            stickSpeed = (map(joystickPosition, 0, 255, -drivespeed1, drivespeed1));
          }          

          if ( abs(joystickPosition-128) < joystickFootDeadZoneRange)
          {
              footDriveSpeed = 0;
          } else if (footDriveSpeed < stickSpeed)
          {
              if (stickSpeed-footDriveSpeed<(ramping+1))
                  footDriveSpeed+=ramping;
              else
                  footDriveSpeed = stickSpeed;
          }
          else if (footDriveSpeed > stickSpeed)
          {
              if (footDriveSpeed-stickSpeed<(ramping+1))
                  footDriveSpeed-=ramping;
              else
                  footDriveSpeed = stickSpeed;  
          }
          
          turnnum = (myPS3->getAnalogHat(LeftHatX));

          //TODO:  Is there a better algorithm here?  
          if ( abs(footDriveSpeed) > 50)
              turnnum = (map(myPS3->getAnalogHat(LeftHatX), 54, 200, -(turnspeed/4), (turnspeed/4)));
          else if (turnnum <= 200 && turnnum >= 54)
              turnnum = (map(myPS3->getAnalogHat(LeftHatX), 54, 200, -(turnspeed/3), (turnspeed/3)));
          else if (turnnum > 200)
              turnnum = (map(myPS3->getAnalogHat(LeftHatX), 201, 255, turnspeed/3, turnspeed));
          else if (turnnum < 54)
              turnnum = (map(myPS3->getAnalogHat(LeftHatX), 0, 53, -turnspeed, -(turnspeed/3)));


          currentMillis = millis();
          if ( (currentMillis - previousFootMillis) > serialLatency  )
          {

          #ifdef SHADOW_VERBOSE      
          if ( footDriveSpeed < -driveDeadBandRange || footDriveSpeed > driveDeadBandRange)
          {
            output += "Driving Droid at footSpeed: ";
            output += footDriveSpeed;
            output += "!  DriveStick is Enabled\r\n";
            output += "Joystick: ";              
            output += myPS3->getAnalogHat(LeftHatX);
            output += "/";              
            output += myPS3->getAnalogHat(LeftHatY);
            output += " turnnum: ";              
            output += turnnum;
            output += "/";              
            output += footDriveSpeed;
            output += " Time of command: ";              
            output += millis();
          }
          #endif


          ST->turn(turnnum * invertTurnDirection);
          ST->drive(footDriveSpeed);
          // The Sabertooth won't act on mixed mode packet serial commands until
          // it has received power levels for BOTH throttle and turning, since it
          // mixes the two together to get diff-drive power levels for both motors.
           previousFootMillis = currentMillis;
          return true; //we sent a foot command   
          }
      }
  }
  return false;
}


int ps3DomeDrive(PS3BT* myPS3 = PS3Nav, int controllerNumber = 1)
{
    int domeRotationSpeed = 0;
    if (controllerNumber==1 && myPS3->getButtonPress(CROSS) && !(myPS3->getButtonPress(L1)) && !(myPS3->getButtonPress(L2)) && !(myPS3->getButtonPress(PS))  )
    {
      domeRotationSpeed = -75;
    } else if (controllerNumber==1 && myPS3->getButtonPress(CIRCLE) && !(myPS3->getButtonPress(L1)) && !(myPS3->getButtonPress(L2)) && !(myPS3->getButtonPress(PS))  )
    {
      domeRotationSpeed = 75;
    } else if ( (controllerNumber==1 && myPS3->getButtonPress(L1)) || ( controllerNumber==2 && !myPS3->getButtonPress(L1) && !myPS3->getButtonPress(L2) )  )
    {
        int joystickPosition = myPS3->getAnalogHat(LeftHatX);
        domeRotationSpeed = (map(joystickPosition, 0, 255, -domespeed, domespeed));
        if ( abs(joystickPosition-128) < joystickDomeDeadZoneRange ) 
          domeRotationSpeed = 0;
    }
    return domeRotationSpeed;
}

void rotateDome(int domeRotationSpeed, String mesg)
{
    //Constantly sending commands to the SyRen (Dome) is causing foot motor delay.
    //Lets reduce that chatter by trying 3 things:
    // 1.) Eliminate a constant stream of "don't spin" messages (isDomeMotorStopped flag)
    // 2.) Add a delay between commands sent to the SyRen (previousDomeMillis timer)
    // 3.) Switch to real UART on the MEGA (Likely the *CORE* issue and solution)
    // 4.) Reduce the timout of the SyRen - just better for safety!
    
    currentMillis = millis();
    if ( (!isDomeMotorStopped || domeRotationSpeed != 0) && ((currentMillis - previousDomeMillis) > (2*serialLatency) )  )
    {
      #ifdef SHADOW_VERBOSE      
        output += "DEBUG:  Dome Rotation called by: ";
        output += mesg;
        if (domeRotationSpeed < 0)
        {
          output += "  Spinning Dome Left at speed: "; 
        } else if (domeRotationSpeed > 0)
        {
          output += "  Spinning Dome Right at speed: "; 
        } else
        {
          output += "  Stopping Dome Spin speed: "; 
        }    
        output += domeRotationSpeed; 
        output += "\r\n";
      #endif
      if (domeRotationSpeed != 0)
      {
        isDomeMotorStopped = false;
      } else
      {
        isDomeMotorStopped = true;
      }
      previousDomeMillis = currentMillis;      
      SyR->motor(domeRotationSpeed);
    }
}


#ifdef DOME_I2C_ADAFRUIT           
boolean adafruitPs3Holoprojector(PS3BT* myPS3 = PS3Nav, int controllerNumber = 1)
{
    ////turn hp light on & off
    if( !(myPS3->getButtonPress(PS)) && myPS3->getButtonClick(L3))
    {
        if (holoLightFrontStatus != HOLO_LED_OFF)
	{
            #ifdef SHADOW_DEBUG      
              output += "Turning Off Holo Light\r\n";
            #endif
            holoLightFrontStatus = HOLO_LED_OFF;
            holoLightOff(HOLO_FRONT_RED_PWM_PIN, HOLO_FRONT_GREEN_PWM_PIN, HOLO_FRONT_BLUE_PWM_PIN);
	} else
	{
            #ifdef SHADOW_DEBUG      
              output += "Turning On Holo Light\r\n";
            #endif
            holoLightFrontStatus = HOLO_LED_ON;
            holoLightOn(HOLO_FRONT_RED_PWM_PIN, HOLO_FRONT_GREEN_PWM_PIN, HOLO_FRONT_BLUE_PWM_PIN);
	}      
        return true;
    }

/////hp movement
    if (myPS3->getButtonPress(PS))
    {
        if(myPS3->getButtonPress(UP))
        {
            #ifdef SHADOW_DEBUG
              output += "Move Holo Up\r\n";
            #endif
            moveHoloServo(HOLO_FRONT_Y_PWM_PIN, HOLO_FRONT_Y_SERVO_MAX);
        }        
        if(myPS3->getButtonPress(DOWN))
        {
            #ifdef SHADOW_DEBUG
              output += "Move Holo Down\r\n";
            #endif
            moveHoloServo(HOLO_FRONT_Y_PWM_PIN, HOLO_FRONT_Y_SERVO_MIN);
        }
        if(myPS3->getButtonPress(LEFT))
        {
            #ifdef SHADOW_DEBUG
              output += "Move Holo Left\r\n";
            #endif
            moveHoloServo(HOLO_FRONT_X_PWM_PIN, HOLO_FRONT_X_SERVO_MAX );
        }
        if(myPS3->getButtonPress(RIGHT))
        {
            #ifdef SHADOW_DEBUG
              output += "Move Holo Right\r\n";
            #endif
            moveHoloServo(HOLO_FRONT_X_PWM_PIN, HOLO_FRONT_X_SERVO_MIN);
        }
        if (!(myPS3->getButtonPress(UP)||myPS3->getButtonPress(DOWN)))
        {
            moveHoloServo(HOLO_FRONT_Y_PWM_PIN, HOLO_SERVO_CTR);
        }
        if (!(myPS3->getButtonPress(RIGHT)||myPS3->getButtonPress(LEFT)))
        {
            moveHoloServo(HOLO_FRONT_X_PWM_PIN, HOLO_SERVO_CTR);
        }
        return true;
    }
    if (myPS3->getButtonPress(L1) &&controllerNumber == 2)
    {
            //TODO:  Analog stick - can we granually control holo?
            if(myPS3->getAnalogHat(LeftHatY) < 50)
            {
                output += "Move Holo Up\r\n";
                moveHoloServo(HOLO_FRONT_Y_PWM_PIN, HOLO_FRONT_Y_SERVO_MAX);
            }        
            if(myPS3->getAnalogHat(LeftHatY) > 200)
            {
                output += "Move Holo Down\r\n";
                moveHoloServo(HOLO_FRONT_Y_PWM_PIN, HOLO_FRONT_Y_SERVO_MIN);
            }
            if(myPS3->getAnalogHat(LeftHatX) < 50)
            {
                output += "Move Holo Left\r\n";
                moveHoloServo(HOLO_FRONT_X_PWM_PIN, HOLO_FRONT_X_SERVO_MAX);
            }
            if(myPS3->getAnalogHat(LeftHatX) > 200)
            {
                output += "Move Holo Right\r\n";
                moveHoloServo(HOLO_FRONT_X_PWM_PIN, HOLO_FRONT_X_SERVO_MIN);
            }
            if (!(myPS3->getAnalogHat(LeftHatY) < 50||myPS3->getAnalogHat(LeftHatY) > 200))
            {
                //output += "Move Holo V-Center\r\n";
                moveHoloServo(HOLO_FRONT_Y_PWM_PIN, HOLO_SERVO_CTR);
            }
            if (!(myPS3->getAnalogHat(LeftHatX) < 50||myPS3->getAnalogHat(LeftHatX) > 200))
            {
                //output += "Move Holo H-Center\r\n";
                moveHoloServo(HOLO_FRONT_X_PWM_PIN, HOLO_SERVO_CTR);
            }
            return true;        
    }
    return false;
}
#endif
  
    
#if defined(DOME_SERIAL_TEECES) || defined(DOME_I2C_TEECES)
boolean teecesPs3Holoprojector(PS3BT* myPS3 = PS3Nav, int controllerNumber = 1)
{
  ////turn hp light on & off
    if( !(myPS3->getButtonPress(PS)) && myPS3->getButtonClick(L3))
    {
        if(domeData.hpl == 1)
        {
            #ifdef SHADOW_DEBUG      
              output += "Turning Off Holo Light\r\n";
            #endif
            domeData.hpl = 0;    //if hp light is on, turn it off
            domeData.dsp = 100;
            ET.sendData();
        }
        else
        {
            #ifdef SHADOW_DEBUG      
              output += "Turning On Holo Light\r\n";
            #endif
            domeData.hpl = 1;    //turn hp light on
            domeData.dsp = 100;
            ET.sendData();
        }
        return true;
    }

/////hp movement
    if (myPS3->getButtonPress(PS))
    {
        if(myPS3->getButtonPress(UP))
        {
            #ifdef SHADOW_DEBUG
              output += "Move Holo Up\r\n";
            #endif
            domeData.hpy = 150;
            domeData.dsp = 100;
            ET.sendData();
        }        
        if(myPS3->getButtonPress(DOWN))
        {
            #ifdef SHADOW_DEBUG
              output += "Move Holo Down\r\n";
            #endif
            domeData.hpy = 30;
            domeData.dsp = 100;
            ET.sendData();
        }
        if(myPS3->getButtonPress(LEFT))
        {
            #ifdef SHADOW_DEBUG
              output += "Move Holo Left\r\n";
            #endif
            domeData.hpx = 30;
            domeData.dsp = 100;
            ET.sendData();
        }
        if(myPS3->getButtonPress(RIGHT))
        {
            #ifdef SHADOW_DEBUG
              output += "Move Holo Right\r\n";
            #endif
            domeData.hpx = 150;
            domeData.dsp = 100;
            ET.sendData();
        }
        if (!(myPS3->getButtonPress(UP)||myPS3->getButtonPress(DOWN)))
        {
            domeData.hpy = 0;
            domeData.dsp = 100;
            ET.sendData();
        }
        if (!(myPS3->getButtonPress(RIGHT)||myPS3->getButtonPress(LEFT)))
        {
            domeData.hpx = 0;
            domeData.dsp = 100;
            ET.sendData();
        }
        return true;
    }
    if (myPS3->getButtonPress(L1) &&controllerNumber == 2)
    {
            //TODO:  Analog stick - can we granually control holo?
            if(myPS3->getAnalogHat(LeftHatY) < 50)
            {
                output += "Move Holo Up\r\n";
                domeData.hpy = 30;
                domeData.dsp = 100;
                ET.sendData();
            }        
            if(myPS3->getAnalogHat(LeftHatY) > 200)
            {
                output += "Move Holo Down\r\n";
                domeData.hpy = 150;
                domeData.dsp = 100;
                ET.sendData();
            }
            if(myPS3->getAnalogHat(LeftHatX) < 50)
            {
                output += "Move Holo Left\r\n";
                domeData.hpx = 30;
                domeData.dsp = 100;
                ET.sendData();
            }
            if(myPS3->getAnalogHat(LeftHatX) > 200)
            {
                output += "Move Holo Right\r\n";
                domeData.hpx = 150;
                domeData.dsp = 100;
                ET.sendData();
            }
            if (!(myPS3->getAnalogHat(LeftHatY) < 50||myPS3->getAnalogHat(LeftHatY) > 200))
            {
                //output += "Move Holo V-Center\r\n";
                domeData.hpy = 0;
                domeData.dsp = 100;
                ET.sendData();
            }
            if (!(myPS3->getAnalogHat(LeftHatX) < 50||myPS3->getAnalogHat(LeftHatX) > 200))
            {
                //output += "Move Holo H-Center\r\n";
                domeData.hpx = 0;
                domeData.dsp = 100;
                ET.sendData();
            }
            return true;        
    }
    return false;
}
#endif

boolean ps3Holoprojector(PS3BT* myPS3 = PS3Nav, int controllerNumber = 1)
{  
      #ifdef DOME_I2C_ADAFRUIT           
          return adafruitPs3Holoprojector(myPS3,controllerNumber);
      #endif
      #if defined(DOME_SERIAL_TEECES) || defined(DOME_I2C_TEECES)
          return teecesPs3Holoprojector(myPS3,controllerNumber);
      #endif
      return false;
}


void ps3utilityArms(PS3BT* myPS3 = PS3Nav, int controllerNumber = 1)
{
  switch (controllerNumber)
    {
      case 1:
        if(myPS3->getButtonPress(L1)&&myPS3->getButtonClick(CROSS))
          {
              #ifdef SHADOW_DEBUG
                output += "Opening/Closing top utility arm\r\n";
              #endif
              
                waveUtilArm(UTIL_ARM_TOP);
          }
          if(myPS3->getButtonPress(L1)&&myPS3->getButtonClick(CIRCLE))
          {
              #ifdef SHADOW_DEBUG
                output += "Opening/Closing bottom utility arm\r\n";
              #endif
              
                waveUtilArm(UTIL_ARM_BOTTOM);
          }
        break;
      case 2:
        if (!(myPS3->getButtonPress(L1)||myPS3->getButtonPress(L2)||myPS3->getButtonPress(PS)))
        {
          if(myPS3->getButtonClick(CROSS))
          {
              #ifdef SHADOW_DEBUG
                output += "Opening/Closing top utility arm\r\n";
              #endif
              
                waveUtilArm(UTIL_ARM_TOP);
          }
          if(myPS3->getButtonClick(CIRCLE))
          {
              #ifdef SHADOW_DEBUG
                output += "Opening/Closing bottom utility arm\r\n";
              #endif
              
                waveUtilArm(UTIL_ARM_BOTTOM);
          }
        }
        break;
    }
}

void utilityArms()
{
  if (PS3Nav->PS3NavigationConnected) ps3utilityArms(PS3Nav,1);
  if (PS3Nav2->PS3NavigationConnected) ps3utilityArms(PS3Nav2,2);
}

void ps3ToggleSettings(PS3BT* myPS3 = PS3Nav)
{
    if(myPS3->getButtonPress(PS)&&myPS3->getButtonClick(L3))
    {
      //Quick Shutdown of PS3 Controller
      output += "\r\nDisconnecting the controller.\r\n";
      myPS3->disconnect();
    }

  
  //// enable / disable right stick & play sound
    if(myPS3->getButtonPress(PS)&&myPS3->getButtonClick(CROSS))
    {
        #ifdef SHADOW_DEBUG
          output += "Disiabling the DriveStick\r\n";
        #endif
        isStickEnabled = false;
//        trigger.play(52);
    }
    if(myPS3->getButtonPress(PS)&&myPS3->getButtonClick(CIRCLE))
    {
        #ifdef SHADOW_DEBUG
          output += "Enabling the DriveStick\r\n";
        #endif
        isStickEnabled = true;
///        trigger.play(53);
    }


////turn hp automation or automate on & off
//    if(myPS3->getButtonPress(L1)&&myPS3->getButtonClick(CIRCLE))
//    {
//        #ifdef SHADOW_DEBUG
//          output += "Enabling the Holo Automation\r\n";
//        #endif
//        //Turn On HP Automation
//        domeData.hpa = 1;
//        domeData.dsp = 100;
//        ET.sendData();
//    }
//    if(myPS3->getButtonPress(L1)&&myPS3->getButtonClick(CROSS))
//    {
//        #ifdef SHADOW_DEBUG
//          output += "Disabling the Holo Automation\r\n";
//        #endif
//        //Turn Off HP Automation
//        domeData.hpa = 0;
//        domeData.dsp = 100;
//        ET.sendData();
//    }

    if(myPS3->getButtonPress(L2)&&myPS3->getButtonClick(CROSS))
    {
        #ifdef SHADOW_DEBUG
          output += "Disabling the Dome Automation\r\n";        
        #endif
        isAutomateDomeOn = false;
        action = 0;
//        trigger.play(53);
    }
    if(myPS3->getButtonPress(L2)&&myPS3->getButtonClick(CIRCLE))
    {
        #ifdef SHADOW_DEBUG
          output += "Enabling the Dome Automation\r\n";
        #endif
        isAutomateDomeOn = true;
//        trigger.play(52);
    }


    /*
    ////Logic display brightness
        if(ps2x.ButtonPressed(PSB_PAD_UP))
        {
            if(ps2x.Button(PSB_L1))
            {
                domeData.dsp = 24;
                ET.sendData();
            }
        }
        if(ps2x.ButtonPressed(PSB_PAD_DOWN))
        {
            if(ps2x.Button(PSB_L1))
            {
                domeData.dsp = 25;
                ET.sendData();
            }
        }
    */

}

void processSoundCommand(char soundCommand)
{
    #ifdef SOUND_CFSOUNDIII
    //cfSound.playfile("happy.wav");  
    //cfSound.setVolume(20);
    switch (soundCommand) 
    {
        case '+':
            #ifdef SHADOW_DEBUG    
              output += "Volume Up\r\n";
            #endif
            cfSound.volumeUp();
        break;
        case '-':
            #ifdef SHADOW_DEBUG
              output += "Volume Down\r\n";
            #endif
            cfSound.volumeDown();
        break;
        case '9': 
            #ifdef SHADOW_DEBUG   
              output += "Sound Random OFF/Bank\r\n";
            #endif
            cfSound.sendButton('9');
        break;
        case '0':    
            #ifdef SHADOW_DEBUG
              output += "Sound Random ON/Bank\r\n";
            #endif    
            cfSound.sendButton('0');
        break;
        case '1':    
        case '2':    
        case '3':    
        case '4':    
        case '5':    
        case '6':    
        case '7':    
        case '8':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += "\r\n";
            #endif
            cfSound.sendButton(soundCommand);
        break;
        default:
            #ifdef SHADOW_DEBUG
              output += "Invalid Sound Command\r\n";
            #endif
        break;
    }
    #endif

    #ifdef SOUND_MP3TRIGGER
    switch (soundCommand) 
    {
        case '+':
            #ifdef SHADOW_DEBUG    
              output += "Volume Up\r\n";
            #endif
            if (vol>0)
            {
                vol--;
                trigger.setVolume(vol);
            }
        break;
        case '-':   
            #ifdef SHADOW_DEBUG 
              output += "Volume Down\r\n";
            #endif
            if (vol<255)
            {
                vol++;
                trigger.setVolume(vol);
            }
        break;
        
        case '1':  
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Sceam\r\n";
          #endif
          //Play Sceam
          trigger.play(1);  
          break;
        case '2':   
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Wolf Whistle.\r\n";
          #endif        
          // Play Wolf Whistle
          trigger.play(4);
          break;
        case '3':    
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Doo Doo\r\n";
          #endif        
          //Play Doo Doo
          trigger.play(3);
          break;
        case '4':    
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Chortle\r\n";
          #endif        
          //Play Chortle
          trigger.play(2);
          break;
        case '5':    
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Random Sentence.\r\n";
          #endif        
          // Play Random Sentence
          trigger.play(random(32,52));
          break;
        case '6':    
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Random Misc.\r\n";
          #endif   
          //Play Random Misc.     
          trigger.play(random(17,25));
          break;
        case '7':    
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Cantina Song.\r\n";
          #endif        
          //Play Cantina Song
          trigger.play(10);
          break;
        case '8':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play Imperial March.\r\n";
            #endif
            //Play Imperial March
            trigger.play(11);
        break;
        case '9':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play Let It Go.\r\n";
            #endif
            //Play Let It Go
            trigger.play(55);
        break;
        case '0':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play Gangdum Style\r\n";
            #endif
            //Play Gangdum Style
            trigger.play(54);
        break;
        case 'A':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play Upset a Droid\r\n";
            #endif
            //Play Upset a Droid
            trigger.play(57);
        break;
        case 'B':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play Summer\r\n";
            #endif
            //Play Summer
            trigger.play(61);
        break;
        case 'C':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play Everything is Awesome\r\n";
            #endif
            //Play Everything is Awesome
            trigger.play(62);
        break;
        case 'D':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play What Does the Fox Say\r\n";
            #endif
            //Play What Does the Fox Say
            trigger.play(63);
        break;
        default:
            #ifdef SHADOW_DEBUG
              output += "Invalid Sound Command\r\n";
            #endif
            trigger.play(60);
  }
  #endif
}

void ps3soundControl(PS3BT* myPS3 = PS3Nav, int controllerNumber = 1)
{

#ifdef EXTRA_SOUNDS
    switch (controllerNumber)
    {
      case 1:
#endif
    	if (!(myPS3->getButtonPress(L1)||myPS3->getButtonPress(L2)||myPS3->getButtonPress(PS)))
	    {
	      if (myPS3->getButtonClick(UP))          processSoundCommand('1');    
	      else if (myPS3->getButtonClick(RIGHT))  processSoundCommand('2');    
	      else if (myPS3->getButtonClick(DOWN))   processSoundCommand('3');    
	      else if (myPS3->getButtonClick(LEFT))   processSoundCommand('4');    
	    } else if (myPS3->getButtonPress(L2))
	    {
	      if (myPS3->getButtonClick(UP))          processSoundCommand('5');    
	      else if (myPS3->getButtonClick(RIGHT))  processSoundCommand('6');    
	      else if (myPS3->getButtonClick(DOWN))   processSoundCommand('7');    
	      else if (myPS3->getButtonClick(LEFT))   processSoundCommand('8');    
	    } else if (myPS3->getButtonPress(L1))
	    {
	      if (myPS3->getButtonClick(UP))          processSoundCommand('+');    
	      else if (myPS3->getButtonClick(DOWN))   processSoundCommand('-');    
	      else if (myPS3->getButtonClick(LEFT))   processSoundCommand('9');    
	      else if (myPS3->getButtonClick(RIGHT))  processSoundCommand('0');    
	    } 
#ifdef EXTRA_SOUNDS
        break;
      case 2:
    	if (!(myPS3->getButtonPress(L1)||myPS3->getButtonPress(L2)||myPS3->getButtonPress(PS)))
	    {
	      if (myPS3->getButtonClick(UP))          processSoundCommand('A');    
	      else if (myPS3->getButtonClick(RIGHT))  processSoundCommand('B');    
	      else if (myPS3->getButtonClick(DOWN))   processSoundCommand('C');    
	      else if (myPS3->getButtonClick(LEFT))   processSoundCommand('D');    
	    } else if (myPS3->getButtonPress(L2))
	    {
	      if (myPS3->getButtonClick(UP))          processSoundCommand('E');    
	      else if (myPS3->getButtonClick(RIGHT))  processSoundCommand('F');    
	      else if (myPS3->getButtonClick(DOWN))   processSoundCommand('G');    
	      else if (myPS3->getButtonClick(LEFT))   processSoundCommand('H');    
	    } else if (myPS3->getButtonPress(L1))
	    {
	      if (myPS3->getButtonClick(UP))          processSoundCommand('I');    
	      else if (myPS3->getButtonClick(DOWN))   processSoundCommand('J');    
	      else if (myPS3->getButtonClick(LEFT))   processSoundCommand('K');    
	      else if (myPS3->getButtonClick(RIGHT))  processSoundCommand('L');    
	    } 
        break;
	}
#endif

}



void footMotorDrive()
{
  //Flood control prevention
  if ((millis() - previousFootMillis) < serialLatency) return;  
  if (PS3Nav->PS3NavigationConnected) ps3FootMotorDrive(PS3Nav);
  //TODO:  Drive control must be mutually exclusive - for safety
  //Future: I'm not ready to test that before FanExpo
  //if (PS3Nav2->PS3NavigationConnected) ps3FootMotorDrive(PS3Nav2);
}  

void domeDrive()
{
  //Flood control prevention
  //This is intentionally set to double the rate of the Foot Motor Latency
  if ((millis() - previousDomeMillis) < (2*serialLatency) ) return;  
  

  int domeRotationSpeed = 0;
  int ps3NavControlSpeed = 0;
  int ps3Nav2ControlSpeed = 0;
  if (PS3Nav->PS3NavigationConnected) ps3NavControlSpeed = ps3DomeDrive(PS3Nav,1);
  if (PS3Nav2->PS3NavigationConnected) ps3Nav2ControlSpeed = ps3DomeDrive(PS3Nav2,2);

  //In a two controller system, give dome priority to the secondary controller.
  //Only allow the "Primary" controller dome control if the Secondary is NOT spinnning it
  
  if ( abs(ps3Nav2ControlSpeed) > 0 )
  {
    domeRotationSpeed = ps3Nav2ControlSpeed;
  } else
  {
    domeRotationSpeed = ps3NavControlSpeed; 
  }
  rotateDome(domeRotationSpeed,"Controller Move");
}  


void moveHoloServo(int pwmPIN, int pulse)
{
    domePWM.setPWM(pwmPIN, 0, pulse);
}

void holoLightFlicker(int pwmPINred, int pwmPINgreen, int pwmPINblue)
{
    int flicker = random(4096);
    domePWM.setPWM(pwmPINred, 0, flicker*0.75);
    domePWM.setPWM(pwmPINgreen, 0, flicker*0.75);
    domePWM.setPWM(pwmPINblue, 0, random(4096));
}

void holoLightOff(int pwmPINred, int pwmPINgreen, int pwmPINblue)
{
    domePWM.setPWM(pwmPINred, 0, PWM_OFF);
    domePWM.setPWM(pwmPINgreen, 0, PWM_OFF);
    domePWM.setPWM(pwmPINblue, 0, PWM_OFF);
}

void holoLightOn(int pwmPINred, int pwmPINgreen, int pwmPINblue)
{
    domePWM.setPWM(pwmPINred, 0, 4094);
    domePWM.setPWM(pwmPINgreen, 0, 4094);
    domePWM.setPWM(pwmPINblue, 0, 4094);
}

void randomHoloMovement(int holoprojector)
{
    currentMillis = millis();

    switch (holoprojector)
    {
      case HOLO_FRONT:   
          if (currentMillis > holoFrontRandomTime)
          {  
              holoFrontRandomTime = currentMillis + random(HOLO_DELAY);
              //TODO:  Determine range of Holoprojector X/Y better
                  //hpY=random(80,120);
                  //hpX=random(80,120); 
              moveHoloServo(HOLO_FRONT_X_PWM_PIN, random(HOLO_FRONT_X_SERVO_MIN,HOLO_FRONT_X_SERVO_MAX));
              moveHoloServo(HOLO_FRONT_Y_PWM_PIN, random(HOLO_FRONT_Y_SERVO_MIN,HOLO_FRONT_Y_SERVO_MAX));
              int ledState = random(1,10);
              switch( ledState )
              {
                  case 0:
                  case 1:
                  case 2:
                  case 3:
                      holoLightFrontStatus = HOLO_LED_OFF;
                      holoLightOff(HOLO_FRONT_RED_PWM_PIN, HOLO_FRONT_GREEN_PWM_PIN, HOLO_FRONT_BLUE_PWM_PIN);
                      break;
                  case 4:
                  case 5:
                  case 6:
                  case 7:
                      holoLightFrontStatus = HOLO_LED_ON;
                      holoLightOn(HOLO_FRONT_RED_PWM_PIN, HOLO_FRONT_GREEN_PWM_PIN, HOLO_FRONT_BLUE_PWM_PIN);
                      break;
                  default:
                      holoLightFrontStatus = HOLO_LED_FLICKER;
                      break;
              }
          }
          if (holoLightFrontStatus == HOLO_LED_FLICKER)
          {
              holoLightFlicker(HOLO_FRONT_RED_PWM_PIN, HOLO_FRONT_GREEN_PWM_PIN, HOLO_FRONT_BLUE_PWM_PIN);
          }        
          break;
      case HOLO_BACK:
            if (currentMillis > holoBackRandomTime)
            {  
                holoBackRandomTime = currentMillis + random(HOLO_DELAY*1.5);
                //TODO:  Determine range of Holoprojector X/Y better
                    //hpY=random(80,120);
                    //hpX=random(80,120); 
                moveHoloServo(HOLO_BACK_X_PWM_PIN, random(HOLO_BACK_X_SERVO_MIN,HOLO_BACK_X_SERVO_MAX));
                moveHoloServo(HOLO_BACK_Y_PWM_PIN, random(HOLO_BACK_Y_SERVO_MIN,HOLO_BACK_Y_SERVO_MAX));
                int ledState = random(1,10);
                switch( ledState )
                {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                        holoLightBackStatus = HOLO_LED_OFF;
                        holoLightOff(HOLO_BACK_RED_PWM_PIN, HOLO_BACK_GREEN_PWM_PIN, HOLO_BACK_BLUE_PWM_PIN);
                        break;
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                        holoLightBackStatus = HOLO_LED_ON;
                        holoLightOn(HOLO_BACK_RED_PWM_PIN, HOLO_BACK_GREEN_PWM_PIN, HOLO_BACK_BLUE_PWM_PIN);
                        break;
                    default:
                        holoLightBackStatus = HOLO_LED_FLICKER;
                        break;
                }
            }
            if (holoLightBackStatus == HOLO_LED_FLICKER)
            {
                holoLightFlicker(HOLO_BACK_RED_PWM_PIN, HOLO_BACK_GREEN_PWM_PIN, HOLO_BACK_BLUE_PWM_PIN);
            }        
            break;
        case HOLO_TOP:  
            if (currentMillis > holoTopRandomTime)
            {  
                holoTopRandomTime = currentMillis + random(HOLO_DELAY*1.5);
                //TODO:  Determine range of Holoprojector X/Y better
                    //hpY=random(80,120);
                    //hpX=random(80,120); 
                moveHoloServo(HOLO_TOP_X_PWM_PIN, random(HOLO_TOP_X_SERVO_MIN,HOLO_TOP_X_SERVO_MAX));
                moveHoloServo(HOLO_TOP_Y_PWM_PIN, random(HOLO_TOP_Y_SERVO_MIN,HOLO_TOP_Y_SERVO_MAX));
                int ledState = random(1,10);
                switch( ledState )
                {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                        holoLightTopStatus = HOLO_LED_OFF;
                        holoLightOff(HOLO_TOP_RED_PWM_PIN, HOLO_TOP_GREEN_PWM_PIN, HOLO_TOP_BLUE_PWM_PIN);
                        break;
                    case 6:
                    case 7:
                    case 8:
                        holoLightTopStatus = HOLO_LED_ON;
                        holoLightOn(HOLO_TOP_RED_PWM_PIN, HOLO_TOP_GREEN_PWM_PIN, HOLO_TOP_BLUE_PWM_PIN);
                        break;
                    default:
                        holoLightTopStatus = HOLO_LED_FLICKER;
                        break;
                }
            }
            if (holoLightTopStatus == HOLO_LED_FLICKER)
            {
                holoLightFlicker(HOLO_TOP_RED_PWM_PIN, HOLO_TOP_GREEN_PWM_PIN, HOLO_TOP_BLUE_PWM_PIN);
            }        
            break;
      }  
}

void holoprojector()
{
   boolean isFrontHoloActivelyControlled = false;
   if (PS3Nav->PS3NavigationConnected) 
   {
     if ( ps3Holoprojector(PS3Nav,1) )
     {
       isFrontHoloActivelyControlled = true;
     }
   }
   if (PS3Nav2->PS3NavigationConnected) 
      {
     if ( ps3Holoprojector(PS3Nav2,2) )
     {
       isFrontHoloActivelyControlled = true;
     }
   }
  if (!isFrontHoloActivelyControlled) randomHoloMovement(HOLO_FRONT);
  randomHoloMovement(HOLO_BACK);
  randomHoloMovement(HOLO_TOP);
}  

void toggleSettings()
{
   if (PS3Nav->PS3NavigationConnected) ps3ToggleSettings(PS3Nav);
   if (PS3Nav2->PS3NavigationConnected) ps3ToggleSettings(PS3Nav2);
}  

void soundControl()
{
   if (PS3Nav->PS3NavigationConnected) ps3soundControl(PS3Nav,1);
   if (PS3Nav2->PS3NavigationConnected) ps3soundControl(PS3Nav2,2);

    // Read from cfsound, send to port USB Serial & BT Serial:
    #ifdef SOUND_CFSOUNDIII
    if (Serial1.available()) 
    {
      int inByte = Serial1.read();
      Serial.write(inByte);         
      if (SerialBT.connected)
      {
        SerialBT.write(inByte);
      }
    }
    #endif
}  


void openUtilArm(int arm, int position = utilArmOpenPos)
{
    //When passed a position - this can "partially" open the arms.
    //Great for more interaction
    moveUtilArm(arm, utilArmOpenPos);
}

void closeUtilArm(int arm)
{
    moveUtilArm(arm, utilArmClosedPos);
}

void waveUtilArm(int arm)
{
    switch (arm)
    {
      case UTIL_ARM_TOP:
        if(isUtilArmTopOpen == false){
          openUtilArm(UTIL_ARM_TOP);
        } else {
          closeUtilArm(UTIL_ARM_TOP);
        }
        break;
      case UTIL_ARM_BOTTOM:  
        if(isUtilArmBottomOpen == false){
          openUtilArm(UTIL_ARM_BOTTOM);
        } else {
          closeUtilArm(UTIL_ARM_BOTTOM);
        }
        break;
    }
}

void moveUtilArm(int arm, int position)
{
    switch (arm)
    {
      case UTIL_ARM_TOP:
        UtilArmTopServo.write(position);
        if ( position == utilArmClosedPos)
        {
          isUtilArmTopOpen = false;
        } else
        {
          isUtilArmTopOpen = true;
        }
        break;
      case UTIL_ARM_BOTTOM:  
        UtilArmBottomServo.write(position);
        if ( position == utilArmClosedPos)
        {
          isUtilArmBottomOpen = false;
        } else
        {
          isUtilArmBottomOpen = true;
        }
        break;
    }
}


void flashCoinSlotLEDs()
{
  for(int i = 0; i<numberOfCoinSlotLEDs; i++)
  {
    if(millis() > nextCoinSlotLedFlash[i])
    {
      if(coinSlotLedState[i] == LOW) coinSlotLedState[i] = HIGH; 
      else coinSlotLedState[i] = LOW;
      digitalWrite(COIN_SLOT_LED_PINS[i],coinSlotLedState[i]);
      nextCoinSlotLedFlash[i] = millis()+random(100, 1000) ; // next toggle random time
    } 
  }
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
            if (SerialBT.connected)
                SerialBT.println(output);
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


