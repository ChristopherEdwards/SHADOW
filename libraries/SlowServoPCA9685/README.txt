This library supports the movement of multiple servos at various speeds at
the same time on Adafruit PCA9685 16-channel PWM/Servo boards.

Two classes are included in the library. Applications should make use of the
Servos class. Applications should not normally need to use the SlowServo class.

The PCA9685 supports multiple boards chained together. One Servos object
should be created for each board, each with different a address (0x40,
0x41 etc).

The Servos::moveTo method is used to setup a servo with a postion to move to
and how long to take.

The Servos::move method performs the task of moving each servo according to
current time. It moves all servos on all boards according to their current
moveTo instruction. It should be called frequently. Use of the standard
delay() method should be avoided as this may lead to jerky movement - use
the Servos::delay method instead.

Further documentation is provided in the Servos.h file.

The library makes extensive use of the millis() Arduino function. Note that
millis() overflows once an Arduino has been running for about 50 days and
this library may lead to unexpected results on an Arduino that is not
rebooted in that period.

The SlowServoPCS9685 library is Copyright (c) 2013 Graham Short.

    SlowServoPCS9685 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlowServoPCS9685 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    A copy of the GNU General Public License is available at
    <http://www.gnu.org/licenses/>

