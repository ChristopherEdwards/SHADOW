/*
CFSoundIII.cpp
@author Darren Blum
*/

#include "CFSoundIII.h"

CFSoundIII::CFSoundIII()
{
}

CFSoundIII::~CFSoundIII()
{
	s->flush();
	s = NULL;
}

void CFSoundIII::setup()
{
	setup(&Serial,2400);
}

void CFSoundIII::setup(HardwareSerial* serial, int baud = 2400)
{
	//The default speed for the CFSound III serial port is 2400.
	//It is possible to change this using a configuration file or code on the SD Card.
	s = serial;
	s->begin(baud);
}

void CFSoundIII::volumeUp()
{
	s->write("$B+\r");
}

void CFSoundIII::volumeDown()
{
	s->write("$B-\r");
}

void CFSoundIII::sendButton(char c)
{
	//Can we be more efficient than this code?
	//  s->write("$B");
	//  s->write(c);
	//  s->write("\r");
	
		
	String buttonCommand = "$B";
    buttonCommand += c;
    buttonCommand += "\r";
	char charBuf[buttonCommand.length()+1];
	buttonCommand.toCharArray(charBuf, buttonCommand.length()+1); 
	s->write(charBuf);
}

void CFSoundIII::setVolume(int level)
{
	//Volume can be set between 0 and 64
	if ( level < 1 )
	{
		level = 0;
	} else if (level > 64)
	{
		level = 64;
	}
	String volumeCommand = "$V";
    volumeCommand += level;
    volumeCommand += "\r";
	char charBuf[volumeCommand.length()+1];
	volumeCommand.toCharArray(charBuf, volumeCommand.length()+1);
	s->write(charBuf);
}


void CFSoundIII::stop()
{
	s->write("$S\r");
}

void CFSoundIII::playfile(const char *filename)
{
	s->write("$P");
	s->write(filename);
	s->write("\r");
}

