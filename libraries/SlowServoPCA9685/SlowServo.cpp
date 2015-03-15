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

#include "SlowServo.h"

SlowServo::SlowServo() {
  init();
  posNow = 150;
}
  
uint
SlowServo::currentPos() {
  return posNow;
}

void
SlowServo::init() {
  finishTime = 0;
  lastMoveTime = 0;
  finishPos = 0;
}

void
SlowServo::doMove(ulong pTimeNow) {
  pwm->setPWM(pwmNu, 0, posNow);
  lastMoveTime = pTimeNow;
}

void
SlowServo::move(ulong pTimeNow) {
  if( finishTime != 0 ) {
    if( pTimeNow >= finishTime ) {
      posNow = finishPos;
      doMove(pTimeNow);
      init(); // all done - clears finish time so we just exit next time around
    } else if( lastMoveTime != pTimeNow ) {
      ulong   timeSinceLastMove = pTimeNow - lastMoveTime;
      ulong   denominator = finishTime - lastMoveTime;
      float   fractionChange = float(timeSinceLastMove)/float(denominator);

      int     distanceToGo = finishPos - posNow;
      float   distanceToMove = float(distanceToGo) * fractionChange;
      int     distanceToMoveInt = int(distanceToMove);

      if( abs(distanceToMoveInt) > 1 ) {
        posNow = posNow + distanceToMoveInt;
        doMove(pTimeNow);
      }      
    }
  }
}

void
SlowServo::moveTo(ulong pMoveTime, uint pStartPos, uint pFinishPos) {
  ulong timeNow = millis();
  
  finishTime = pMoveTime+timeNow;
  finishPos = pFinishPos;
  posNow = pStartPos;
  doMove(timeNow);
}

void
SlowServo::setPwmNu(Adafruit_PWMServoDriver *pPwm, uint8_t Nu) {
  pwm   = pPwm;
  pwmNu = Nu;
}

