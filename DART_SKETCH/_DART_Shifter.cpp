// Include the standard types
#include <Arduino.h>
#include "_DART_Shifter.h"


// Constructor
Shifter::Shifter(byte SER_Pin, byte RCLK_Pin, byte SRCLK_Pin, byte Number_of_Registers){	
	_SER_Pin = SER_Pin;
	_RCLK_Pin = RCLK_Pin;
	_SRCLK_Pin = SRCLK_Pin;
	
	_Number_of_Registers = Number_of_Registers;
	
	
	pinMode(_SER_Pin, OUTPUT);
	pinMode(_RCLK_Pin, OUTPUT);
	pinMode(_SRCLK_Pin, OUTPUT);
	
	
	// clear(); //reset all register pins
	write();
}

void Shifter::write(){
	//Set and display registers
	//Only call AFTER all values are set how you would like (slow otherwise)
 // if (valuetable[general_mempos] == 0)
{
  digitalWrite(_RCLK_Pin, LOW);
  
  //iterate through the registers
  for(int i = _Number_of_Registers - 1; i >=  0; i--){
    
    //iterate through the bits in each registers
    for(int j = 8 - 1; j >=  0; j--){
      
      digitalWrite(_SRCLK_Pin, LOW);   
      
      int val = _shiftRegisters[i] & (1 << j);
      
      digitalWrite(_SER_Pin, val);
      digitalWrite(_SRCLK_Pin, HIGH);
    
    }
   
  }
  
  digitalWrite(_RCLK_Pin, HIGH);

}
}
	
void Shifter::setPin(int index, boolean val){
	int byteIndex = index/8;
	int bitIndex = index % 8;
	
	byte current = _shiftRegisters[byteIndex];
	
	current &= ~(1 << bitIndex); //clear the bit
	current |= val << bitIndex; //set the bit
	
	_shiftRegisters[byteIndex] = current; //set the value
}

void Shifter::setAll(boolean val){
//set all register pins to LOW  
  for(int i = _Number_of_Registers * 8 - 1; i >=  0; i--){
     setPin(i, val);
  }
}

/*
void Shifter::clear(){
//set all register pins to LOW  
  for(int i = _Number_of_Registers * 8 - 1; i >=  0; i--){
     setPin(i, LOW);
  }
}
*/
