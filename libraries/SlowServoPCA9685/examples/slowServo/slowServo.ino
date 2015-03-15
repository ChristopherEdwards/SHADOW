
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <Servos.h>

Servos servos(0x40);

// -------------------------------------------
//
// Example program to produce a "Mexican Wave"
// using 5 servos. After performing a wave
// the built-in LED is switched on for 5 
// seconds (some servos will still be moving).
// The wave then restarts.
//
// -------------------------------------------

const int SERVO_0 = 0;
const int SERVO_1 = 1;
const int SERVO_2 = 2;
const int SERVO_3 = 3;
const int SERVO_4 = 4;
const int SERVO_5 = 5;

const int SERVO_START_POS = 150;
const int SERVO_END_POS   = 600;
const int SERVO_TIME      = 2500;
const int DELAY_TIME      = SERVO_TIME / 5;

ulong programStartTime;
int   currentProgram;


// -------------------------------------------------------------
// This "program" progam0 instructs the servos to move
// to their bottom position as fast as possible, then waits one
// second before moving onto the wave sequence (program1 & 2)
// -------------------------------------------------------------
void program0() {
   static int state = 0;
   ulong      timeNow = millis();
   uint8_t    idx;
  
   switch ( state ) {
   case 0:
     programStartTime = timeNow;
     // move all servos to start
     for(idx=0; idx<servos.maxServos(); ++idx) {
       servos.moveTo(idx, 0, SERVO_START_POS, SERVO_START_POS);
     }
     state = 1;
     break;
   case 1:
     // wait 1 sec before moving on to next program
     if( (timeNow - programStartTime) >= 1000 ) {
       state = 0;
       programStartTime = millis();
       currentProgram = 1;  // move on to next program
     }
     break;
   }
}

// -------------------------------------------------------------
// progam1 moves the servos in an overlapping
// sequence to their top postion.
// -------------------------------------------------------------
void program1() {
   static int state = 0;
   ulong      timeNow = millis();
   
   switch ( state ) {
   case 0:
     programStartTime = timeNow;
     servos.moveTo(SERVO_0, SERVO_TIME, SERVO_END_POS);
     state = 1;
     break;
   case 1:
     if( (timeNow - programStartTime) >= (DELAY_TIME) ) {
       servos.moveTo(SERVO_1, SERVO_TIME, SERVO_END_POS);
       state = 2;
     }
     break;
   case 2:
     if( (timeNow - programStartTime) >= (DELAY_TIME*2) ) {
       servos.moveTo(SERVO_2, SERVO_TIME, SERVO_END_POS);
       state = 3;
     }
     break;
   case 3:
     if( (timeNow - programStartTime) >= (DELAY_TIME*3) ) {
       servos.moveTo(SERVO_3, SERVO_TIME, SERVO_END_POS);
       state = 4;
     }
     break;
   case 4:
     if( (timeNow - programStartTime) >= (DELAY_TIME*4) ) {
       servos.moveTo(SERVO_4, SERVO_TIME, SERVO_END_POS);
       state = 5;
     }
       break;
   case 5:
     if( (timeNow - programStartTime) >= (DELAY_TIME*5) ) {
       servos.moveTo(SERVO_5, SERVO_TIME, SERVO_END_POS);
       state = 0;     // start at beginning next time
       programStartTime = millis();
       currentProgram = 2;  // move on to next program
     }
     break;
   }     
}

// -------------------------------------------------------------
// progam2 moves the servos in an overlapping
// sequence to their bottom position.
// -------------------------------------------------------------
void program2() {
   static int state = 0;
   ulong      timeNow = millis();
   
   switch ( state ) {
   case 0:
     programStartTime = timeNow;
     servos.moveTo(SERVO_0, SERVO_TIME, SERVO_END_POS, SERVO_START_POS);
     state = 1;
     break;
   case 1:
     if( (timeNow - programStartTime) >= (DELAY_TIME) ) {
       servos.moveTo(SERVO_1, SERVO_TIME, SERVO_END_POS, SERVO_START_POS);
       state = 2;
     }
     break;
   case 2:
     if( (timeNow - programStartTime) >= (DELAY_TIME*2) ) {
       servos.moveTo(SERVO_2, SERVO_TIME, SERVO_END_POS, SERVO_START_POS);
       state = SERVO_3;
     }
     break;
   case 3:
     if( (timeNow - programStartTime) >= (DELAY_TIME*3) ) {
       servos.moveTo(SERVO_3, SERVO_TIME, SERVO_END_POS, SERVO_START_POS);
       state = 4;
     }
     break;
   case 4:
     if( (timeNow - programStartTime) >= (DELAY_TIME*4) ) {
       servos.moveTo(SERVO_4, SERVO_TIME, SERVO_END_POS, SERVO_START_POS);
       state = 5;
     }
       break;
   case 5:
     if( (timeNow - programStartTime) >= (DELAY_TIME*5) ) {
       servos.moveTo(SERVO_5, SERVO_TIME, SERVO_END_POS, SERVO_START_POS);
       state = 0;     // start at beginning next time
       programStartTime = millis();
       currentProgram = 3;  // move on to next program
     }
     break;
   }     
}

// -------------------------------------------------------------
// program3 switches on the led for 5 seconds
// -------------------------------------------------------------
void program3() {
   static int state = 0;
   ulong timeNow = millis();
   uint8_t idx;
  
   switch ( state ) {
   case 0:
     digitalWrite(13, HIGH);   // set the LED on

     state = 1;
     break;
   case 1:
     // wait 5 sec before moving on to next program
     Servos::delay(5000);

     digitalWrite(13, LOW);    // set the LED off

     state = 0;
     programStartTime = millis();
     currentProgram = 4;  // move on to next program
     break;
   }
}

// -------------------------------------

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(13, OUTPUT);     
  digitalWrite(13, LOW);    // set the LED off
  
  Serial.begin(9600);
  Serial.println("Slow Servo test");

  currentProgram = 0;
}

void loop() {
  Servos::move(millis());  // this performs the actual moves
  
  switch( currentProgram ) {
  case 0:
    program0();
    break;
  case 1:
    program1();
    break;
  case 2:
    program2();
    break;
  case 3:
    program3();
    break;
  default:
    programStartTime = millis();
    currentProgram = 1; // skip program0 (initialisation)
    break;
  }
}


