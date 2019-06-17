void setup()
{
 // Mouse.begin();

  #if (DMX_active == 1  && stratos == 0)
  DmxSimple.usePin(13);
  DmxSimple.maxChannel(64);
  #endif
  
   #if (stratos == 0)
   for(byte bit = 4; bit < 7; bit++) pinMode(bit, OUTPUT);  // set the three select pins to output  // 4051
   #endif
   
  // Serial.println("stratos" );
   
   for(byte bit = 0; bit < 64; bit++) lastbutton[bit]=1;


  #if (stratos == 0 ) 
    pinMode(2, INPUT);                             // ------------ encoder input settings
    pinMode(3, INPUT); 
    // pinMode(12, OUTPUT); 
    digitalWrite(2, HIGH); //turn pullup resistor on // encoder
    digitalWrite(3, HIGH); //turn pullup resistor on // encoder
    
    pinMode(0, INPUT);                             // ------------ midi over DIN settings
    digitalWrite(0, HIGH);
    #endif

 #if (pullups_active == 1)
 
  
  #if defined (__AVR_ATmega32U4__)
 #if (stratos == 0)
  // dartmobo pullups
  
{ digitalWrite(18, HIGH);
  digitalWrite(19, HIGH);
  digitalWrite(20, HIGH);
  digitalWrite(21, HIGH);
  digitalWrite(22, HIGH);
  digitalWrite(23, HIGH);
}

#endif
#if (stratos == 1)
// stratos pullups
{
    digitalWrite(0, HIGH);
    digitalWrite(1, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH); 
      
     digitalWrite(20, HIGH); // enc 2 // in analogici
    digitalWrite(21, HIGH);  
       digitalWrite(2, HIGH); // enc 1
    digitalWrite(3, HIGH); 

  
    pinMode(14, OUTPUT);    
    pinMode(15, OUTPUT); 
       pinMode(16, OUTPUT); 

    //  digitalWrite(8, HIGH);
   // digitalWrite(9, HIGH);   
}
#endif
  
  
  #endif

  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
  digitalWrite(14, HIGH);
  digitalWrite(15, HIGH);
  digitalWrite(16, HIGH);
  digitalWrite(17, HIGH);
  digitalWrite(18, HIGH);
  digitalWrite(19, HIGH);
  #endif
  
 #endif

   
    #if defined (__AVR_ATmega32U4__)
    #if (stratos == 0)  
    Serial1.begin(31250); // midi specific baud rate
    
    #endif
    
    #endif
    
    #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
    Serial.begin(31250); // midi specific baud rate
    #endif
    

    
   attachInterrupt(0, lettura_enc_principale, CHANGE); 
   attachInterrupt(1, lettura_enc_principale, CHANGE);

 //#if (stratos == 1)
 //attachInterrupt(2, lettura_enc_principale2, CHANGE); 
 //attachInterrupt(3, lettura_enc_principale2, CHANGE);
 // #endif

   
//   ------------------------------------------- setting lower values fot touch sensors

/*
 #if (capacitivesensor_active == 1)
     
     
   for ( byte i = 0; i < Xenlimit; i++) 
   {
   touch_sensors();
   delay(5);
   }
    #endif
   
   lower_Xen[0] = averageXen[0];
   lower_Xen[1] = averageXen[1];
*/
   
  //  -----------------------------------------

 // aux = 0;
 //  maxvalue[general_mempos] = 10;

// page = 0;

#if (stratos == 0)
    load_preset_base();
    load_preset(0);
    #endif

   #if (stratos == 1)
  aux_preset();
  #endif


  
  /////////////////////////////////////////////////////////////////////

#if (stratos == 0 )
if (eeprom_preset_active == 1 && page_mempos > 0) // 1 = c'è un preset nella eeprom // eeprom_preset_active
{
  setPlexer((page_mempos)-((page_mempos/8)*8)); 
  lastbutton[page_mempos] = map(analogRead((page_mempos/8)), 0 ,1024, 0, 2); // read page switch state.

  
  if (lastbutton[page_mempos] == 1  )  {page = 0;  // pagestate=0; 
  load_preset(0); //ledrestore(); // levetta a destra caricamento preset eeprom pagina 1
 }
 else 
 {page= max_modifiers;   //pagestate=1; 
 load_preset(1);  //ledrestore2(); // levetta a sinistra preset 2
 } 

}

else 
 page = 0;
#endif



/*
delay(5);
readingsXen[0] = (cs_4_2.capacitiveSensorRaw(limit_touch)/2);
readingsXen[1] = (cs_4_2.capacitiveSensorRaw(limit_touch)/2);
readingsXen[2] = (cs_4_2.capacitiveSensorRaw(limit_touch)/2);
delay(50);
readingsXen[0] = (cs_4_2.capacitiveSensorRaw(limit_touch)/2);
delay(50);
readingsXen[1] = (cs_4_2.capacitiveSensorRaw(limit_touch)/2);
delay(50);
readingsXen[2] = (cs_4_2.capacitiveSensorRaw(limit_touch)/2);
*/
/*
for (byte i = 0 ; i < 200; i++)
{

 lettura = (cs_4_2.capacitiveSensorRaw(limit_touch)/2);
  delay(2);
  }
  readingsXen[0] = (cs_4_2.capacitiveSensorRaw(limit_touch)/2);
   delay(2);
readingsXen[1] = (cs_4_2.capacitiveSensorRaw(limit_touch)/2);
 delay(2);
readingsXen[2] = (cs_4_2.capacitiveSensorRaw(limit_touch)/2);

*/
#if (stratos == 1)
 page = 0; // provvisorio
 #endif


 if (maxvalue[general_mempos] == 0 ){  // se i pads sono attivi bisogna togliere la pullup
 
 #if (stratos == 0)
  #if defined (__AVR_ATmega32U4__) 
  digitalWrite(23, LOW);
  #endif
  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
  digitalWrite(19, LOW);
  #endif
 #endif

}  

if (valuetable[general_mempos] != 0) {
 digitalWrite(12, LOW);
  digitalWrite(11, LOW);
   digitalWrite(10, LOW);
}

// if (minvalue[general_mempos] > 0) digitalWrite(9, HIGH); // extraplex

 openeditor = 0;
 note = 255;  // out of range (0-127) value

 lastbutton[encoder_mempos[0]] = 64;
 
 }
