/*
||
|| @author         Brett Hagman <bhagman@wiring.org.co>
|| @url            http://wiring.org.co/
|| @url            http://roguerobotics.com/
||
|| @description
|| | Rogue Firmware Update - Serial Passthrough Sketch
|| | Use this sketch on your Arduino or Wiring compatible board,
|| | connected to a Rogue MP3 (uMP3 or rMP3) or Rogue SD (uMMC) module.
|| |
|| | When the board LED turns on, then you can put the Rogue MP3 or SD
|| | module into Update Mode (uMMC and uMP3 - short UPD jumper and reset,
|| | rMP3 - short pins 2 & 3 on JP6). Then use the Rogue Updater to upload
|| | the firmware.
|| |
|| | You can use this sketch for testing serial communications as well.
|| |
|| | See http://www.roguerobotics.com/faq/updateFirmware for updating firmware.
|| #
||
|| @license Please see LICENSE.txt.
||
*/

#if WIRING
 #include <NewSoftSerial.h>
 #define SoftwareSerialType NewSoftSerial
#elif ARDUINO >= 100
 #include <SoftwareSerial.h>
 #define SoftwareSerialType SoftwareSerial
 #define WLED 13
#endif

SoftwareSerialType rogueSerial = SoftwareSerialType(6, 7);


void setup()
{
  rogueSerial.begin(9600);
  Serial.begin(9600);

  pinMode(WLED, OUTPUT);
  digitalWrite(WLED, LOW);

  delay(500);  // wait for things to settle

  // This is to clear out the pipes
  for (int i = 0; i < 128; i++)
    rogueSerial.print((char)0x1b);

  rogueSerial.flush();

  digitalWrite(WLED, HIGH);
}

void loop()
{
  if (rogueSerial.available())
  {
    Serial.print((char)rogueSerial.read());
    digitalWrite(WLED, LOW);
  }
  if (Serial.available())
  {
    rogueSerial.print((char)Serial.read());
    digitalWrite(WLED, HIGH);
  }
}

