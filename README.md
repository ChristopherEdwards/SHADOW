Introducing S.H.A.D.O.W. (Small Handheld Arduino Droid Operating Wand) MSE Edition
A MSE droid control system based on DanF's Padawan, and Darren Blum's S.H.A.D.O.W. that uses a PS3 Move Navigation Controller

This is a fork of Darren Blum's S.H.A.D.O.W. system. It is specifically modified to support MSE (RC Car) Based Droids.

It is a slimmed down version of the original code base to only support what is used on these types of droids and makes
opinionated decisions (like only supporting the Sparkfun MP3 Trigger). If you are already familiar with S.H.A.D.O.W.
then this fork will feel very familiar.

This sketch works on an Arduino MEGA ADK, or Arduino MEGA or Arduino UNO with a USB Shield

Pin Assignment:
* Steering Servo: 4
* ESC: 3
* MP3 Trigger: 2

Sound Mapping:
Note: when using L2 Throttle the L2 + and L1 + keystrokes still work, though they may not be very useful
* D-pad Up - Sound 001
* D-pad Right - Sound 002
* D-pad Down - Sound 003
* D-pad Left - Random Sound 001-015
* Circle/L2 + D-pad Up - Sound 004
* Cross/L2 + D-pad Right - Sound 005
* Not Available/L2 + D-pad Down - Sound 006
* Not Available/L2 + D-pad Left - Sound 015
* L1 + D-pad Up - Volume Up
* L1 + D-pad Right - Sound 019
* L1 + D-pad Down - Volume Down
* L1 + D-pad Left - Sound 018

Additional information for the original S.H.A.D.O.W. Sketch can be found in this thread on Asromech.net:
http://astromech.net/forums/showthread.php?19298-S-H-A-D-O-W-Padawan-based-mini-PS3-Controller

Thank you to:
* Darren Blum - aka KnightShade
* MSE Droid Builders Facebook page
