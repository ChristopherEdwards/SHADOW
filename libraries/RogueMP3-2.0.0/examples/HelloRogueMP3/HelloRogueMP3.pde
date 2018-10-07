/*
|| RogueMP3 Hello Example Sketch.
||
|| Put the included HelloRogue.mp3 onto your SD card that goes into the Rogue MP3 player.
||
|| More notes at the bottom.
*/

#include <RogueMP3.h>
#include <SoftwareSerial.h>

SoftwareSerial rogueSerial = SoftwareSerial(6, 7);

RogueMP3 mp3Player = RogueMP3(rogueSerial);

void setup()
{
  rogueSerial.begin(9600);
  Serial.begin(9600);
  Serial.println("Sketch Starting.");

  if (mp3Player.sync())
  {
    Serial.println("MP3 player synchronized.");

    // You can change "/HelloRogue.mp3" here to a file you have on your SD card.
    mp3Player.playFile("/HelloRogue.mp3");
  }
  else
  {
    Serial.println("Please check the connections.");
  }
}

void loop()
{
}




/*
||
|| @author         Brett Hagman <bhagman@wiring.org.co>
|| @url            http://wiring.org.co/
|| @url            http://roguerobotics.com/
||
|| @description
|| | HelloRogueMP3
|| | Rogue Robotics MP3 Module Library Example (HelloRogueMP3)
|| |
|| | For a sample sound, you can download an example here:
|| | https://googledrive.com/host/0BxcFNQX9fJt9ckNKTVpDY3NJN0E/HelloRogue.mp3
|| |
|| | More free sounds and music can be found here:
|| | https://googledrive.com/host/0BxcFNQX9fJt9ckNKTVpDY3NJN0E/
|| |
|| | The RogueMP3 Wiring and Arduino Library works with the following
|| | Rogue Robotics modules:
|| |   - uMP3 (Industrial MP3 Playback Module)
|| |   - rMP3 (Commercial MP3 Playback Module)
|| |
|| | Requires:
|| | uMP3 firmware > 111.01
|| | rMP3 firmware > 100.00
|| |
|| | See http://www.roguerobotics.com/faq/updateFirmware for updating firmware.
|| #
||
|| @notes
|| | If you don't see "MP3 player synchronized" on the Serial Monitor,
|| | then check to make sure the Rogue MP3 module (uMP3/rMP3) is connected to pins
|| | 6 and 7 on your board.  Also, make sure the MP3 module is running at 9600 bps.
|| #
||
|| @license Please see LICENSE.txt.
||
*/

