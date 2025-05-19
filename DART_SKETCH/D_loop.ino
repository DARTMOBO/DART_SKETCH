
void loop () {

if (cycletimer < 250 ) cycletimer++;
//if (typetable[general_mempos] 

#if (blinker == 1)  
typetable[general_mempos]++; 
#endif


#if (MIDI_IN_block == 0)

  
    #if defined (__AVR_ATmega32U4__)               // USB MIDI in
  do {                                         
    rx = MidiUSB.read();
    if (rx.header != 0) {
 incomingByte  = rx.byte1; midifeedback();
 incomingByte  = rx.byte2; midifeedback();
 incomingByte  = rx.byte3; midifeedback();
    }
  } while (rx.header != 0);
 
   #if ( stratos == 0) 
                                                  // DIN MIDI in
  if (Serial1.available() > 0) { 
  incomingByte = Serial1.read();
  midifeedback(); 
  } 
  else 
  #endif
  
#endif





  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
 if (Serial.available() > 0) { 
  incomingByte = Serial.read();
  midifeedback();  } 
  else 
  #endif

      
  if (openeditor != 1) 
  #endif 
   
  {

    
  

 
   #if (capacitivesensor_active>0)
    indexXen++; if (indexXen == 3) indexXen = 0;
    touch_sensors(0);
          #if (stratos == 0)
         if (dmxtable[general_mempos] >1)  // attiva il secondo touch solo se è attivo il secondo spinner
        { touch_sensors(1);}
          #endif
    #endif

  #if (stratos == 1)  
  AIN_stratos();
   #endif
   
  #if (stratos == 0)  
  AIN();
   #endif


    #if (shifter_active == 1 && stratos == 0)
  if (qwertyvalue[general_mempos] > 2)    buttonledefx();  
    #endif
 
 

     #if (page_active > 0)  
   if (page_mempos > 0 ) pageswitch();
     #endif
   

 //shifter.setAll(LOW);     
// if (valuetable[general_mempos] == 0)  shifter.write();
     #if (shifter_active == 1 && stratos == 0)
     if (shifterwrite ==1 && valuetable[general_mempos] == 0)  {shifter.write(); shifterwrite=0;
      }
     #endif
 
  }

}
