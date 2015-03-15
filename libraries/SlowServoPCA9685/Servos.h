/*
 *  The SlowServoPCS9685 library is Copyright (c) 2013 Graham Short.
 *
 *  This file is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This file is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  A copy of the GNU General Public License is available at
 *  <http://www.gnu.org/licenses/>
 */

#ifndef SLOW_SERVO_SERVOS_H
#define SLOW_SERVO_SERVOS_H

#include "SlowServo.h"

/**********************************************************************
 * 
 * class Servos
 *
 *       Servos - Constructor. Parameter addr specifies which board to address.
 *                Valid values are 0x40, 0x41, 0x42, 0x43 or 0x44.
 *                Create a separate Servos object for each board
 *                and call the move() method for each one.
 *
 *       delay  - Use this instead of the Arduino library delay() method
 *                to keep servos moving while delaying.
 *
 *       maxServos - Returns the number of servos supported by a board (16).
 *
 *       move   - Call this frequently to keep the servos moving
 *
 *       moveTo - Instructs a servo what position to move to and how long
 *                to take to get there. Servo is moved to the start position
 *                immediately (ie. without the need to call move().
 *                Parameters -
 *                  pServoNu   - servo number. 0 to maxServos() -1.
 *                  pMoveTime  - time in milliseconds to take
 *                  pStartPos  - (optional) start postition
 *                  pFinishPos - finish position
 *
 *********************************************************************/
class Servos : public Adafruit_PWMServoDriver {
  static const uint MAX_BOARDS = 5;
  static const uint MAX_SERVOS = 16;

  static Servos *board[MAX_BOARDS];
  SlowServo      servo[MAX_SERVOS];

  boolean initialised;

  void moveBoard(ulong pTimeNow);
  void init();

public:
  Servos(uint8_t addr = 0x40);
  
  static void delay(ulong milliseconds);
  uint8_t     maxServos();
  static void move(ulong pTimeNow);
  void        moveTo(byte pServoNu, ulong pMoveTime, uint pFinishPos);
  void        moveTo(byte pServoNu, ulong pMoveTime, uint pStartPos, uint pFinishPos);
};

#endif
