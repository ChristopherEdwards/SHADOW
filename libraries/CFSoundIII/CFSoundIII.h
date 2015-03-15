/*
CFSoundIII.h
@author Darren Blum

A very basic library to send button presses to a CFSound III.
This permits an Arduino to simulate the button used for many Astromech Droids.
It doesn't handle messages from the CFSound at this point
*/

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif
  
class CFSoundIII
{
  public:
	CFSoundIII();
	~CFSoundIII();
	void setup();
	void setup(HardwareSerial* serial, int baud);
	void volumeUp();
	void volumeDown();
	void sendButton(char c);	
	void setVolume(int level);	//0-64
    void stop(void);
	void playfile(const char *filename);

	
private:
	HardwareSerial* s;
};