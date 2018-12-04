#ifndef Shifter_h
#define Shifter_h

// Include the standard types
#include <Arduino.h>

// Define the Shifter class
class Shifter
{
  public:
    // Constructor
    Shifter(byte SER_Pin, byte RCLK_Pin, byte SRCLK_Pin, byte Number_of_Registers);
   	void write();
   	void setPin(int index, boolean val);
   	void setAll(boolean val);
   	//void clear();



  private:
	byte _SER_Pin;
  	byte _RCLK_Pin;
  	byte _SRCLK_Pin;
  	byte _Number_of_Registers;
  	byte _shiftRegisters[25];
};

#endif //Shifter_h
