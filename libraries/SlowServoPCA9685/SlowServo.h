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

#ifndef SLOW_SERVO_H
#define SLOW_SERVO_H

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

typedef unsigned long ulong;
typedef unsigned int  uint;

/* class for handling a single servo
*/
class SlowServo {
  Adafruit_PWMServoDriver *pwm;
  uint8_t pwmNu;
  ulong   finishTime;
  ulong   lastMoveTime;
  uint    finishPos;
  uint    posNow;

  void doMove(ulong pTimeNow);  
  void init();
public:
  SlowServo();

  uint currentPos();
  void move(ulong pTimeNow);
  void moveTo(ulong pMoveTime, uint pStartPos, uint pFinishPos);
  void setPwmNu(Adafruit_PWMServoDriver *pPwm, uint8_t Nu);
};

#endif
