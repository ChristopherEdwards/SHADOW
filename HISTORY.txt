================================================================================
////////////SHADOW :  Small Handheld Arduino Droid Operating Wand///////////////
================================================================================
                          Last Revised Date: 11/14/2014
                             Written By: KnightShade
                     Heavily Influenced by PADAWAN by danf

Revision History:
-------------------------------------------------------------------------------
V5::  11/14/2014 
Alpha Release 4
Updated code for Strong
 - Merged in Fixed from Dave C. and Marty M. (MP3 Trigger)
 - Merged an altered version of Utility Arm Code and Body LED code contributed by Dave C.
 - Added support for multiple Dome communication:  Teeces Serial, Teeces I2C, AdaFruit I2C
 - With AdaFruit I2C:  Control 3 Holoprojectors with Servos and RGB LED (paired with R Series Logics)
 - Misc Fixes (InvertTurn, Fixes for Dome Automation and deadband)
 - Testing with Parani UD-100 Bluetooth w/ external Antenna
 - Extended commands sent to CFSound III (& prep for other sound systems:  rMP3 & Rasberry Pi)

-------------------------------------------------------------------------------
V4::  09/13/2014 
Alpha Release 3
Updated code for Scare-a-con
 - Updated dome sketch to resolve communication issues, and "stalled" transition pattern
 - Ported dome serial to Hardware Serial.  This resolved Dome Communication
 - Tweaked some sound controls (L1 vs PS)

-------------------------------------------------------------------------------
V3::  09/01/2014 
Alpha Release 2
Updated code for FanExpo in Toronto, code base is approaching "stable"
 - Reworked the flow of "ps3FootMotorDrive" - successfully resolved issue #1 from v2
 - Resolve Latency Issues by converting to HardwareSerial resolving issue #2 from v2
 - Added a 10ms gap between SyRen.motor calls
		This was an alternate solution to #2, and ensures serial time for foot motors
 - Chained the Syren and Sabertooth - while removing legacy SimplifiedSyren support.  
 - Reduced the timeouts for the SyRen and Sabertooth to 300ms (quicker shutoff in absense of Commands)
 - Added "Quick Disconnect" for Controller (PS+L3)
 - Updated Holo movement, moving it to L1-"D-pad" due to changes in Joystick control

Other updates pending:
	Further refinement of Controller #2
-------------------------------------------------------------------------------
V2::  08/24/2014
Alpha Release 1
Initial test release, used at the Baldwinsville Min-Con Library event
 - Base functionality all there.
 - Added support for a second PS3 Move Navigation Controller
 - Initial support for SPP (Bluetooth Serial Monitor)
 - Latency for the foot motors has been traced down to a conflict 
		with the dome code flooding the serial channels.
 - Code added to reduce serial chatter after the motors are stopped.

Major Bugs found in this release:
1.)Spinning the dome with the joystick is causing the foot motors to go.
	Placed the SyRen&Sabertooth onto different addresses - this did NOT resolve the issue
2.)	I now suspect that the flooding issue may have been related to SoftwareSerial buffer size.  
		As the MEGA has 3 hardware uarts - move the motors to use Hardware, (possibly chained).
-------------------------------------------------------------------------------
V1::  08/10/2014  
Pre-Alpha Release
Prototype code, to evaluate feasibility
 - Support for Single Move Navigation Controller
 - Code was all inline - minimal deviation from Padawan base code
