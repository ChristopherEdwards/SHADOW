Introducing S.H.A.D.O.W. (Small Handheld Arduino Droid Operating Wand)
A droid control system based on DanF's Padawan that uses a PS3 Move Navigation Controller


As I've been working with my R2, I've found that it is advantages to have a small controller that is easily concealed.

My droid initially used a PS2 controller connected to a C6C - so the controller isn't too large... and in cooler weather, I could easily conceal it within a Jacket pocket. 
During the summer however, I learned that it was much easier for those intently looking to spot the droid handler. This led me down a path toward looking at alternate controllers.

I had also wanted to have some more automation for the dome than the C6C offers.
This had me looking at PADAWAN - as aside from the C6C it used many of the same electronics that I already had in R2 - I just needed to add an Arduino into the mix.

As I started down that path - I started to look at different controller options - wondering what could interact with an Arduino.
I had heard that some were using Wii Nunchucks to control their droid - and that sounded interesting. 
As I researched wireless Wii Nunchucks - the range of the typical wireless nunchuck seemed to be limited to about 3 meters, which was too short for a droid.

In my musings, I wandered across the PS3 Move Navigation Controller:

Being essentially the left half of a PS3 controller - it was considerably smaller than what I was using. 
Its wireless was bluetooth (Class 2) which has a range of about 33 feet. 
I found a USB Host Shield from Circuits@home.com
Which had a set of drivers that would interface with wireless PS3 controllers (both the DualShock as well as the Move Navigation)

It seemed like all of the key elements existed to travel down this path - and I've been able to do so succesfully.
My droid has been successfully running this way for over 6 months now.

This project is to bring other people in on the fun, and to continue to improve the codebase.

Additional information can be found in this thread on Asromech.net:
http://astromech.net/forums/showthread.php?19298-S-H-A-D-O-W-Padawan-based-mini-PS3-Controller

-Darren Blum - aka KnightShade

