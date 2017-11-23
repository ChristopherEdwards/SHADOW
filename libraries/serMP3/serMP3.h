#ifndef serMP3_h
#define serMP3_h

#include <inttypes.h>
//#include <Stream.h>

/******************************************************************************
* Definitions
******************************************************************************/
#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

class serMP3 // : public Stream
{
private:
  	// per object data
 	 uint8_t _transmitBitMask;
 	 volatile uint8_t *_transmitPortRegister;
 	 uint8_t _bsyBitMask;
 	 volatile uint8_t *_bsyPortRegister;

 	 // static data
 	 static serMP3 *active_object;

 	 // private methods
  	 void tx_pin_write(uint8_t pin_state);

  	// private static method for timing
	static inline void tunedDelay(uint16_t delay);

	public:
  	// public methods
 	serMP3(uint8_t transmitPin, uint8_t bsyPin);
	void send(uint8_t b);
	void begin(uint8_t vol);
	void play(uint8_t file);
	void stop();
	void pause();
	void playRand();
	void cd(uint8_t folder);
	void setVol(uint8_t vol);
	void volUP();
	void volDN();
	bool checkBsy();
	
};

// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round

#endif
