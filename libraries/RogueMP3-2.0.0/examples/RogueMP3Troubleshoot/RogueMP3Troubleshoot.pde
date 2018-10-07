/*
|| RogueMP3 Troubleshoot Sketch.
||
|| More notes at the bottom.
*/

#include <RogueMP3.h>
#include <RogueSD.h>
#include <SoftwareSerial.h>
#include "HelloRogueMP3.h"

SoftwareSerial rogueSerial = SoftwareSerial(6, 7);

RogueMP3 mp3Player = RogueMP3(rogueSerial);
RogueSD fileControl = RogueSD(rogueSerial);

const char *MP3Filename = "/HelloRogue32kbps.mp3";
#define BLOCKSIZE 256

void setup()
{
  int syncResponse = 0;
  char ch = 0;

  rogueSerial.begin(9600);
  Serial.begin(9600);
  Serial.println("Sketch Starting.");

  fileControl.begin();
  syncResponse = mp3Player.begin();

  if (syncResponse < 0)
  {
    Serial.println("No response from module. Check serial connections.");
  }
  else if (syncResponse == 0)
  {
    Serial.println("uMMC connected. RogueMP3 will only work on a Rogue Robotics rMP3 or uMP3.");
  }
  else
  {
    Serial.print("Firmware Version: ");
    Serial.println(mp3Player.version() / 100.0);
    Serial.print("Full firmware version from module: ");
    rogueSerial.print("V\r");

    for (;;)
    {
      while (!rogueSerial.available());
      ch = rogueSerial.read();

      if (ch == '>')
        break;
      Serial.print(ch);
    }

    Serial.println();

    Serial.println("MP3 player synchronized.");

    // MP3 player is synchronized.
    // Now let's see if our test sound is available on the SD card.
    if (fileControl.exists(MP3Filename))
    {
      Serial.print("Playing ");
      Serial.println(MP3Filename);
      mp3Player.playFile(MP3Filename);
    }
    else
    {
      Serial.print(MP3Filename);
      Serial.println(" doesn't exist. Let's write it onto the card...");
      // Create the file
      Serial.print("Size of ");
      Serial.print(MP3Filename);
      Serial.print(": ");
      Serial.println(sizeof(HelloRogue32kbpsmp3), DEC);

      int8_t fh = fileControl.open(MP3Filename, OPEN_WRITE);
      if (fh > 0)
      {
        Serial.println("Writing...");

        unsigned int count = 0;

        while (count < sizeof(HelloRogue32kbpsmp3))
        {
          uint8_t b[BLOCKSIZE];
          int bytesToRead = BLOCKSIZE;
          uint8_t *p = HelloRogue32kbpsmp3 + count;

          if ((sizeof(HelloRogue32kbpsmp3) - count) < BLOCKSIZE)
          {
            bytesToRead = sizeof(HelloRogue32kbpsmp3) - count;
          }

          memcpy_P(b, p, bytesToRead);
          fileControl.write(fh, bytesToRead, (const char *) b);
          count += bytesToRead;
          Serial.println(count, DEC);
        }

        fileControl.close(fh);

        Serial.println("Done. Let's play it!");

        mp3Player.playFile(MP3Filename);
      }
      else
      {
        Serial.print("Error opening file: E");
        Serial.println(fh, fileControl.lastErrorCode);
      }
    }
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
|| | RogueMP3 Troubleshoot sketch.
|| | This sketch tests a few things with the RogueMP3 library.
|| | Additionaly, if the sample sound is not found on the inserted SD card,
|| | it will be uploaded.  NOTE: the sample is low quality compared to the
|| | one that is supplied in the examples folder.
|| |
|| | This sketch also requires the RogueSD library.
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
|| @license Please see LICENSE.txt.
||
*/

