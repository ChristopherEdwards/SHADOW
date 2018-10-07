/*
|| RogueMP3 Play Folder Sketch.
||
|| You will need to have a folder on your SD card with any number
|| of MP3 files.  This sketch will play them from the folder.
||
|| More notes at the bottom.
*/

#include <RogueMP3.h>
#include <RogueSD.h>
#include <SoftwareSerial.h>

SoftwareSerial rogueSerial = SoftwareSerial(6, 7);

RogueMP3 mp3Player = RogueMP3(rogueSerial);
RogueSD fileControl = RogueSD(rogueSerial);

// Change this to the match the name of the folder on your SD card.
const char *folder = "/Halloween";

int MP3Count;
int currentMP3;


void setup()
{
  int syncResponse = 0;

  rogueSerial.begin(9600);
  Serial.begin(9600);
  Serial.println("Sketch Starting.");

  fileControl.begin();
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
  }

  if (!fileControl.status() && fileControl.lastErrorCode == ERROR_CARD_NOT_INSERTED)
  {
    // Wait here until a card is inserted.
    Serial.println("Insert card to continue.");
    while (!fileControl.status())
      delay(100);
  }

  // Now let's count the files in the folder.
  MP3Count = fileControl.fileCount(folder, "*.mp3");

  if (MP3Count >= 0)
  {
    Serial.print(MP3Count, DEC);
    Serial.print(" file");
    if (MP3Count != 1)
      Serial.print('s');
    Serial.print(" found in folder ");
    Serial.println(folder);
    
    currentMP3 = -1;
  }
  else
  {
    // Something went wrong
    if (fileControl.lastErrorCode == ERROR_OPEN_PATH_INVALID)
    {
      Serial.println("Looks like the folder is missing.");
      Serial.print("The sketch is looking for this folder on the SD card: ");
      Serial.println(folder);
      Serial.println("Put some MP3 files in that folder on the SD card, and press the reset button.");
      for (;;);
    }
  }
}

void loop()
{
  // After each file ends, play the next file.
  // If we have reached the end, we will start again at the top.
  char filename[200];

  if (mp3Player.getPlaybackStatus() == 'S')
  {
    // Play the next song.
    currentMP3++;
    
    if (currentMP3 >= MP3Count)
      currentMP3 = 0;
    
    // We need to get the filename from the entry number.
    fileControl.entryToFilename(filename, 200, currentMP3, folder, "*.mp3");
 
    mp3Player.playFile(folder, filename);

    Serial.print("Playing: ");
    Serial.print(folder);
    Serial.print('/');
    Serial.println(filename);
  }
  else
  {
    delay(100);
  }

  if (!fileControl.status())
  {
    if (fileControl.lastErrorCode == ERROR_CARD_NOT_INSERTED)
    {
      Serial.println("SD card removed. Press the reset button to start again.");
    }
    else
    {
      Serial.print("An error occurred: E");
      Serial.println(fileControl.lastErrorCode, HEX);
    }
    for (;;);
  }
}




/*
||
|| @author         Brett Hagman <bhagman@wiring.org.co>
|| @url            http://wiring.org.co/
|| @url            http://roguerobotics.com/
||
|| @description
|| | RogueMP3 Play Folder Sketch
|| | This sketch plays mp3 files found in a folder on the inserted
|| | SD card.
|| |
|| | Free sounds and music can be found here:
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

