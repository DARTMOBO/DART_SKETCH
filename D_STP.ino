

void setup()
{

  { // setup per matrix pads
#if (Matrix_Pads > 0  && stratos == 0)
  {

  set_unit(0);
  set_unit(1);
  set_unit(2);
  set_unit(3);
  
  set_unit(4);
  set_unit(5);
  set_unit(6);
  set_unit(7);
  
  set_unit(8);
  set_unit(9);
  set_unit(10);
  set_unit(11);

   set_unit(12);
   set_unit(13);
}
#endif
  }

  { // DMX setup
  #if (DMX_active == 1  && stratos == 0)
  DmxSimple.usePin(13);
  DmxSimple.maxChannel(64);
  #endif
  }

  { // setup 4051 chips
   #if (stratos == 0)
   for(byte bit = 4; bit < 7; bit++) pinMode(bit, OUTPUT);  // set the three select pins to output  // 4051
   #endif
  }
   
 
   for(byte bit = 0; bit < 64; bit++) lastbutton[bit]=1;    // ?? 

   { // encoder e midi-DIN inputs - su dartmobo
   #if (stratos == 0 ) 
    pinMode(2, INPUT);                             // ------------ encoder input settings
    pinMode(3, INPUT); 
    // pinMode(12, OUTPUT); 
    digitalWrite(2, HIGH); //turn pullup resistor on // encoder
    digitalWrite(3, HIGH); //turn pullup resistor on // encoder
    
    pinMode(0, INPUT);                             // ------------ midi over DIN settings
    digitalWrite(0, HIGH);
    #endif
}

{ // gestione pullups
#if (pullups_active == 1)
   
      #if defined (__AVR_ATmega32U4__) // gestione pullups
{ 
            #if (stratos == 0) // dartmobo pullups  
{ 
  digitalWrite(18, HIGH); // analog in 0
  digitalWrite(19, HIGH); // analog in 1
  digitalWrite(20, HIGH); // analog in 2
  digitalWrite(21, HIGH); // analog in 3
  digitalWrite(22, HIGH); // analog in 4
  digitalWrite(23, HIGH); // analog in 5
}
            #endif

            #if (stratos == 1) // stratos pullups 
{
   pinMode(0, INPUT);
    digitalWrite(0, HIGH);
    pinMode(1, INPUT);
    digitalWrite(1, HIGH);
    pinMode(4, INPUT);
    digitalWrite(4, HIGH);
    pinMode(5, INPUT);
    digitalWrite(5, HIGH);
    pinMode(6, INPUT);
    digitalWrite(6, HIGH);
    pinMode(7, INPUT);
    digitalWrite(7, HIGH); 
    
       digitalWrite(18, HIGH);
        digitalWrite(19, HIGH);

        pinMode(20, INPUT);
     digitalWrite(20, HIGH); // enc 2 // in analogici
     pinMode(21, INPUT);
    digitalWrite(21, HIGH);  
    pinMode(2, INPUT);
       digitalWrite(2, HIGH); // enc 1
       pinMode(3, INPUT);
    digitalWrite(3, HIGH); 

  
    pinMode(14, OUTPUT);    
    pinMode(15, OUTPUT); 
       pinMode(16, OUTPUT);  // led touch

}
            #endif
}
      #endif

      #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)  // gestione pullups
  {
  digitalWrite(14, HIGH);
  digitalWrite(15, HIGH);
  digitalWrite(16, HIGH);
  digitalWrite(17, HIGH);
  digitalWrite(18, HIGH);
  digitalWrite(19, HIGH);
  }
      #endif

#endif
}

   { // serial begin
   #if defined (__AVR_ATmega32U4__)
    #if (stratos == 0)  
    Serial1.begin(31250); // midi over DIN connectors - specific baud rate - su stratos non serve
    #endif  
   #endif
    
    #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
    Serial.begin(31250); // midi specific baud rate
    #endif
   }
 
       load_preset_base();
       load_preset(0);

#if (stratos == 0 ) // se trovo un preset lo carico e controllo lo stato di PAGE
{
 if (eeprom_preset_active == 1 && page_mempos > 0) // 1 = c'è un preset nella eeprom // eeprom_preset_active viene settato da setup_mempos 
                                                   // che a sua volta è richiamata da load_preset
 {
  setPlexer((page_mempos)-((page_mempos/8)*8)); 
  lastbutton[page_mempos] = map(analogRead((page_mempos/8)), 0 ,1024, 0, 2); // read page switch state.

  if (lastbutton[page_mempos] == 0  )  {page = 0;  // pagestate=0; 
  pagestate = 1;
  load_preset(0); //ledrestore(); // levetta a destra caricamento preset eeprom pagina 1
    page_leds_(0);
    
     update_scala(1);  
     update_scala(0);   
 
        
 }
 else 
 {page= max_modifiers;   pagestate=0; 
 load_preset(1);  //ledrestore2(); // levetta a sinistra preset 2

  page_leds_(1);
     update_scala(1);  
     update_scala(0);  

 
 } 
 }
 else {
 page = 0;
    //  update_scala(1);  
    //  update_scala(0); 
 }
   
    

}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (stratos == 1) // se trovo un preset lo carico e controllo lo stato di PAGE
{
 //page = 0; // provvisorio

   digitalWrite(15, LOW);digitalWrite(14, HIGH);    
     //valore = analogRead(18);
     if (eeprom_preset_active == 1 && page_mempos > 0)
     { 
     lastbutton[page_mempos] = map(analogRead(18), 0 ,1024, 0, 2);

      if (lastbutton[page_mempos] == 1  ) 
      {
        page = 0;  // pagestate=0; 
  load_preset(0); //ledrestore(); // levetta a destra caricamento preset eeprom pagina 1
    
     update_scala(1);  
     update_scala(0);   
 
        
 }
      else 
      {
  page= max_modifiers;   //pagestate=1; 
 load_preset(1);  //ledrestore2(); // levetta a sinistra preset 2
 
     update_scala(1);  
     update_scala(0);  

 
 }
  
     }
     
}
 #endif


 if (maxvalue[general_mempos] == 0 )
 {  // se i pads sono attivi bisogna togliere la pullup
 
 #if (stratos == 0)
  #if defined (__AVR_ATmega32U4__) 
  digitalWrite(23, LOW);
  #endif
  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
  digitalWrite(19, LOW);
  #endif
 #endif

}  


if (valuetable[general_mempos] != 0) { // 0 = nomobo
 digitalWrite(12, LOW);
  digitalWrite(11, LOW);
   digitalWrite(10, LOW);
}

 openeditor = 0;
 note = 255;  // out of range (0-127) value
 
if (dmxtable[general_mempos] >0) 
{
   attachInterrupt(0, lettura_enc_principale, CHANGE); 
   attachInterrupt(1, lettura_enc_principale, CHANGE);
}
 
 }
