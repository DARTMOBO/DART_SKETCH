
// ensure this library description is only included once
#ifndef CapacitiveSensor_h
#define CapacitiveSensor_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// library interface description
class CapacitiveSensor
{
  // user-accessible "public" interface
  public:
  // methods
	CapacitiveSensor(uint8_t sendPin, uint8_t receivePin);
	int capacitiveSensorRaw(uint16_t limit);
	// long capacitiveSensor(uint8_t samples);
	
	// void reset_CS_AutoCal();
	// void set_CS_AutocaL_Millis(unsigned long autoCal_millis);
  // library-accessible "private" interface
  private:
  // variables
	// int error;
	//unsigned long  leastTotal;
//	unsigned int   loopTimingFactor;
	//  unsigned byte  CS_Timeout_Millis = 30;
	// unsigned int     CS_AutocaL_Millis;
	//unsigned long  lastCal;
	 volatile int  total;
	uint8_t sBit;   // send pin's ports and bitmask
	volatile uint8_t *sReg;
	volatile uint8_t *sOut;
	uint8_t rBit;    // receive pin's ports and bitmask 
	volatile uint8_t *rReg;
	volatile uint8_t *rIn;
	volatile uint8_t *rOut;
  // methods
//	int SenseOneCycle(void);
};

#endif
