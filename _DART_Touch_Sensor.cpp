

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#include "WConstants.h"
#endif

#include "_DART_Touch_Sensor.h"


CapacitiveSensor::CapacitiveSensor(uint8_t sendPin, uint8_t receivePin)
{
	uint8_t sPort, rPort;

	
	sBit =  digitalPinToBitMask(sendPin);			// get send pin's ports and bitmask
	sPort = digitalPinToPort(sendPin);
	sReg = portModeRegister(sPort);
	sOut = portOutputRegister(sPort);				// get pointer to output register   

	rBit = digitalPinToBitMask(receivePin);			// get receive pin's ports and bitmask 
	rPort = digitalPinToPort(receivePin);
	rReg = portModeRegister(rPort);
	rIn  = portInputRegister(rPort);
   	rOut = portOutputRegister(rPort);
	
	// get pin mapping and port for receive Pin - from digital pin functions in Wiring.c
    noInterrupts();
	*sReg |= sBit;              // set sendpin to OUTPUT 
    interrupts();

}

// Public Methods //////////////////////////////////////////////////////////////
// Functions available in Wiring sketches, this library, and other libraries

int CapacitiveSensor::capacitiveSensorRaw(uint16_t limit)
{
  total = 0;
  //int total2 =0;
   //limit = 255;
  int limit2 = limit*2;
 //  noInterrupts();
	*sOut &= ~sBit;        // set Send Pin Register low

	*rReg &= ~rBit;        // set receivePin to input
	*rOut &= ~rBit;        // set receivePin Register low to make sure pullups are off
	*rReg |= rBit;         // set pin to OUTPUT - pin is now LOW AND OUTPUT
	*rReg &= ~rBit;        // set pin to INPUT 

	*sOut |= sBit;         // set send Pin High 

 // interrupts();
// delay(5);
	
while ( 
	  !(*rIn & rBit)  && 
	(total < limit ) ) {  // while receive pin is LOW AND total is positive value
    total++;	
   }
  /*  *rReg &= ~rBit;        // set receivePin to input
  *rOut &= ~rBit;        // set receivePin Register low to make sure pullups are off
  *rReg |= rBit;         // set pin to OUTPUT - pin is now LOW AND OUTPUT
  *rReg &= ~rBit;        // set pin to INPUT 
  *sOut |= sBit;         // set send Pin High 
   */

  /* for (byte i = 0 ; i < 255; i++){
    if ((*rIn & rBit)) {  total++;  }
    }
    */
 /*   for (byte i = 0 ; i < 255; i++){
    if (digitalRead(8)!=digitalRead(9)) {  total++;  }
    }*/
  // delay(1);
  

	// set receive pin HIGH briefly to charge up fully - because the while loop above will exit when pin is ~ 2.5V 
 //  noInterrupts();
	*rOut  |= rBit;        // set receive pin HIGH - turns on pullup 
	//  *rOut  |= ~rBit;  
	 *rReg |= rBit;         // set pin to OUTPUT - pin is now HIGH AND OUTPUT

	*rReg &= ~rBit;        // set pin to INPUT 
	*rOut  &= ~rBit;       // turn off pullup
 // delay(2);
	*sOut &= ~sBit;        // set send Pin LOW

 
 // interrupts();
 

   /*for (byte i = 0 ; i < 255; i++){
    if ((*rIn & rBit)) {  //total++;  
    }
    }
    */
 
	 while ( 
	 (*rIn & rBit) &&  // se entrambi accesi
	 (total < limit  ) ) {  // while receive pin is HIGH  AND total is less than timeout
		 total++; ;
		}
 
   
  //  Serial.println(total);
  // Serial.println(total2);
 //Serial.println(max(total,total2));
 // Serial.println((total+total2));
  //  Serial.println("..");
return total;
}
