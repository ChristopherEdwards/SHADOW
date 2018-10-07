/*
|| RogueMP3 Spectrum Analyzer Example Sketch.
||
|| Put the included HelloRogue.mp3 onto your SD card that goes into the Rogue MP3 player.
||
|| More notes at the bottom.
*/

#include <RogueMP3.h>
#include <SoftwareSerial.h>

SoftwareSerial rogueSerial = SoftwareSerial(6, 7);

RogueMP3 mp3Player = RogueMP3(rogueSerial);

const int bandCount = 5;
unsigned int bandFrequencies[] = { 120, 200, 500, 2000, 8000 };


void setup()
{
  int syncResponse = 0;

  rogueSerial.begin(9600);
  Serial.begin(9600);
  Serial.println("Sketch Starting.");

  syncResponse = mp3Player.begin();

  if (syncResponse <= 0)
  {
    Serial.println("MP3 module not connected. Check serial connections.");
    for (;;);
  }
  else
  {
    if (mp3Player.getModuleType() != rMP3)
    {
      Serial.println("This sketch will only work on a rMP3.");
      for (;;);
    }
    
    if (mp3Player.version() < 10002)
    {
      Serial.println("Spectrum analyzer output requires a minimum");
      Serial.println("rMP3 firmware version of 100.02. See sketch for details.");
      for (;;);
    }
  }

  // Otherwise, we're ready!
  Serial.println("MP3 player synchronized.");

  // You can change "/HelloRogue.mp3" here to a file you have on your SD card.  
  if (mp3Player.playFile("/HelloRogue.mp3"))
  {
    mp3Player.setSpectrumAnalyzerBands(bandFrequencies, bandCount);
    Serial.println("Track started");
  }
  else
  {
    if (mp3Player.lastErrorCode == ERROR_CARD_NOT_INSERTED)
    {
      Serial.println("Card not inserted. Insert card, then press reset button.");
      for (;;);
    }
    else if (mp3Player.lastErrorCode == ERROR_FILE_DOES_NOT_EXIST)
    {
      Serial.println("The file is not on the inserted SD card.");
      Serial.println("Copy the MP3 file you want on the SD card and");
      Serial.println("press the reset button.");
      for (;;);
    }
    else
    {
      Serial.print("Uh oh. Something went wrong. Error code = ");
      Serial.println(mp3Player.lastErrorCode, HEX);
      for (;;);
    }
  }
}

void loop()
{
  byte specValues[bandCount];
  
  if (mp3Player.isPlaying())
  {
    mp3Player.getSpectrumAnalyzerValues(specValues);

    for (int i = 0; i < bandCount; i++)
    {
      if (specValues[i] < 10)
        Serial.print('0');
      Serial.print(specValues[i], DEC);
      Serial.print(' ');
    }
    Serial.println();
    
    delay(200);
  }
}




/*
||
|| @author         Brett Hagman <bhagman@wiring.org.co>
|| @url            http://wiring.org.co/
|| @url            http://roguerobotics.com/
||
|| @description
|| | RogueMP3 Spectrum Analyzer Example
|| | This sketch demonstrates the usage of the spectrum analyzer data
|| | output from a Rogue Robotics MP3 module (rMP3).
|| | The minimum rMP3 firmware version that gives the spectrum analyzer data
|| | is V100.02 (or V100.02-b002).
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

