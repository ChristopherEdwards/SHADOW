#include <Arduino.h>
#include <serMP3.h>

uint8_t _bsyPin;

  serMP3 *serMP3::active_object = 0;

inline void serMP3::tunedDelay(uint16_t delay) { 
  uint8_t tmp=0;

  asm volatile("sbiw    %0, 0x01 \n\t"
    "ldi %1, 0xFF \n\t"
    "cpi %A0, 0xFF \n\t"
    "cpc %B0, %1 \n\t"
    "brne .-10 \n\t"
    : "+r" (delay), "+a" (tmp)
    : "0" (delay)
    );
}

void serMP3::tx_pin_write(uint8_t pin_state)
{
  if (pin_state == LOW)
    *_transmitPortRegister &= ~_transmitBitMask;
  else
    *_transmitPortRegister |= _transmitBitMask;
}

// Constructor
//
serMP3::serMP3(uint8_t transmitPin,uint8_t bsyPin)
{
 	pinMode(transmitPin, OUTPUT);
 	digitalWrite(transmitPin, HIGH);
	_transmitBitMask = digitalPinToBitMask(transmitPin);
	uint8_t port = digitalPinToPort(transmitPin);
  	_transmitPortRegister = portOutputRegister(port);
  	
  	pinMode(bsyPin, INPUT_PULLUP);
  	_bsyBitMask = digitalPinToBitMask(bsyPin);
	port = digitalPinToPort(bsyPin);
	_bsyPortRegister = portOutputRegister(port);
	_bsyPin = bsyPin;
  	
}

//
// Public methods
//
void serMP3::begin(uint8_t vol){

	delay(3);
	setVol(vol);
	delay(5);
	stop();
}

void serMP3::send(uint8_t b)
{
  byte mask;
  uint8_t oldSREG = SREG;
  cli();  // turn off interrupts for a clean txmit

  // Write the start bit
  tx_pin_write( LOW);
  tunedDelay(476);

  // Write each of the 8 bits

    for (mask = 0x01; mask; mask <<= 1)
    {
      if (b & mask) // choose bit
        tx_pin_write(HIGH); // send 1
      else
        tx_pin_write(LOW); // send 0
    
      tunedDelay(471);
    }

    tx_pin_write(HIGH); // restore pin to natural state

  SREG = oldSREG; // turn interrupts back on
  tunedDelay(471);
}

void serMP3::play(uint8_t file){
	if(!checkBsy()){
		stop();
	}
	
	if (file > 0 && file < 200){
		send(file);	
	}
}
void serMP3::stop(){
	send(239);
}

void serMP3::pause(){
	send(234);
}

void serMP3::playRand(){
	
	if(!checkBsy()){
		stop();
	}
	
	send(uint8_t(0));
}

void serMP3::cd(uint8_t folder){
	
	if(!checkBsy()){
		stop();
	}
	
	if (folder > 0 && folder < 16){
		folder = folder + 240;
		send(folder);
	}
	while(checkBsy()== 0){
		
	}
}

void serMP3::setVol(uint8_t vol){
	if (vol >= 0 && vol < 32){
	vol = vol + 200;
		send(vol);
	}
}

void serMP3::volUP(){
	send(232);
}

void serMP3::volDN(){
	send(233);
}

bool serMP3::checkBsy(){
	bool bsy = digitalRead(_bsyPin);
	
	return  bsy;
} 
	