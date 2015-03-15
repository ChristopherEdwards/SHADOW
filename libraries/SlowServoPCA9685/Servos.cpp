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

#include "Servos.h"

Servos* Servos::board[Servos::MAX_BOARDS];

Servos::Servos(uint8_t addr)
           : Adafruit_PWMServoDriver(addr) {
  uint8_t idx;
  uint8_t boardAddr = addr & 0xf;
  
  if( boardAddr < MAX_BOARDS ) {
    board[boardAddr] = this;
  }

  initialised = false;

  for(idx=0; idx<MAX_SERVOS; ++idx) {
    servo[idx].setPwmNu(this, idx);
  }
}


void
Servos::delay(ulong milliseconds) {
  ulong startTime = millis();

  while( (millis() - startTime) < milliseconds ) {
    move( millis() );
  }
}

void
Servos::init() {
  begin();
  setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  initialised = true;
}

uint8_t
Servos::maxServos() {
  return MAX_SERVOS;
}

void
Servos::move(ulong pTimeNow) {
  for( uint8_t idx=0; idx < MAX_BOARDS; ++idx ) {
    if( board[idx] != 0 ) {
      board[idx]->moveBoard(pTimeNow);
    }
  }
}

void
Servos::moveBoard(ulong pTimeNow) {
  if ( ! initialised ) {
    init();
  }

  for( uint8_t idx=0; idx< MAX_SERVOS; ++idx ) {
    servo[idx].move(pTimeNow);
  }
}

void
Servos::moveTo(byte pServoNu, ulong pMoveTime, uint pFinishPos) {
  if( pServoNu < MAX_SERVOS ) {
    moveTo(pServoNu, pMoveTime, servo[pServoNu].currentPos(), pFinishPos);
  }
}

void
Servos::moveTo(byte pServoNu, ulong pMoveTime, uint pStartPos, uint pFinishPos) {
  if ( ! initialised ) {
    init();
  }

  if( pServoNu < MAX_SERVOS ) {
    servo[pServoNu].moveTo(pMoveTime, pStartPos, pFinishPos);
  }
}

