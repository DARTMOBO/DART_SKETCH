
 
 #if (stratos == 1)
 void AIN_stratos()
 

 { 
for( channel = 0; channel < 18; channel++)    
{ 
  chan = channel;
   #if (main_encoder == 1)
   if (lastbutton[encoder_mempos[0]] == 64 || dmxtable[general_mempos] == 0
   ) // 64 = no encoder action - the MAIN spinner has priority over any other action.
  #endif
{
     virtual_touch_end(0);
   

 
 // if (dmxtable[general_mempos] == 2)
 

/*
 {  
 senseEncoder_2nd(); 
 updateEncoder(1);
 encoder(1);
}
*/


 
   if (channel == 1) {  digitalWrite(14, LOW); digitalWrite(15, HIGH); digitalWrite(16, HIGH); 
      valore = (!(digitalRead(5)))*1000;   ain_nucleo();  }
      
      else  
      if (channel == 2) {  if (lastbutton[touch_mempos[0]] == 1 ) 
      digitalWrite(16, LOW); 
      valore = (!(digitalRead(4)))*1000;   ain_nucleo();}
      
        else if (channel == 3) { valore = (!(digitalRead(6)))*1000;   ain_nucleo(); }
   else if (channel == 4)  { valore = (!(digitalRead(7)))*1000;   ain_nucleo(); }
   else if (channel == 5) { valore = (!(digitalRead(19)))*1000;   ain_nucleo(); }
   else if (channel == 6) { valore = (!(digitalRead(18)))*1000;   ain_nucleo(); 
   }


   
    else if (channel == 7) {  digitalWrite(15, LOW);digitalWrite(14, HIGH);
    valore = (!(digitalRead(5)))*1000;   ain_nucleo(); }
    else if (channel == 8) { valore = (!(digitalRead(6)))*1000;   ain_nucleo(); }
    else if (channel == 9) { valore = (!(digitalRead(7)))*1000;   ain_nucleo();   }
    else if (channel == 10) { valore = (!(digitalRead(19)))*1000;   ain_nucleo();  }
    else if (channel == 11) { valore = (!(digitalRead(18)))*1000;   ain_nucleo(); 
    }

       else if (channel == 13) { MSB[1] = (!(digitalRead(21)));   }
       else if (channel == 14) { LSB[1] = (!(digitalRead(20)));//   delay(30);
     updateEncoder(channel); 
  
   //   if (lastbutton[14] != 64) { do_ ++; if (do_ > 3) do_ = 0;  }
  
      //  if (do_ == 0)
        encoder(channel); 
      //  else    lastbutton[14] = 64  ;
       } // primo encoder extra
       
     //  else if (channel == 15) { MSB[1] = (!(digitalRead(0)));   }
     //  else if (channel == 16) { LSB[1] = (!(digitalRead(1)));   updateEncoder(channel); encoder(channel);} // primo encoder extra

//Serial.println(bitRead(channel,0));
 // delay(200); 
  // else if (channel == 16) { 
 
 // encoder(channel);}


  }   
 #if (main_encoder == 1)
  else {
  encoder(encoder_mempos[0]); // midiout 1st encoder  
  //    Serial.println("encoder_active");         
  }
 #endif
  
   } // end PLEXER
   } // end ain

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


 
  #if (stratos == 0)
  void AIN()
 { 
for( channel = 0; channel < 8; channel++)    /// per ognuno degli 8 channels del multiplexer vado poi a leggere tutti gli ingressi analogici (cioè l'uscita di ogni plexer)
{ 

      #if (LED_rings == 1)
   LED_rings_ ();
   shifter.write();
   #endif
   
   #if (main_encoder == 1) 
 if (lastbutton[encoder_mempos[0]] == 64 || dmxtable[general_mempos] == 0) // 64 = no encoder action - the MAIN spinner has priority over any other action.
#endif
{
    virtual_touch_end(0);
    
  if (maxvalue[general_mempos] == 0 ){ PADS();}  


 
  if (dmxtable[general_mempos] >1)
{  
  senseEncoder_2nd();  // carica MSB LSB [1]
  // updateEncoder(encoder_mempos[1]);  // carica lastbutton[encoder_mempos[1]
  //encoder(encoder_mempos[1]); // esegui encoder secondario.
}

 ///////////////////////////////////////////////////////////////////   
 
  if (valuetable[general_mempos] !=1 ) // nomobo setup
 setPlexer(channel); // all 4051's are set on the channel to be read
 
 for(plexer = 0; plexer < 
  5+boolean(maxvalue[general_mempos]) // se si attivano i pads (mettendo maxvalue = 0) l'analogico A5 non viene letto
  ; plexer++) //  plexer 0,1,2,3,4 - the 5th plexer is read at higher speed (pads and 2nd encoder)
  
  {
  // 
    
   chan = (plexer * 8) + channel  ;                                  // 46 - 48 - second encoder
 
   
 
 //Serial.println(chan);
//delay (100);
 
  if (eeprom_preset_active == 0) // se non trovo un preset nella eeprom 
  {
    valore = analogRead(plexer);
   // valore = analogRead(plexer);
    
    if (valore < upper_val   /// se premo un pulsante - valore scende
     && dmxtable[chan] < 3
    )  
   {  
    detect_plexer(); 

   if (valore > 448 && valore < 576   ) // se si ha un valore intermedio
  {
//    if (qwertyvalue[chan] == (valore /32) ) // se la lettura precedente = molto simile alla attuale (risoluzione 0-1024 downgradata a 0-32
    // questo passaggio serve per evitare che del rumore o imperfezionti temporanee dela lettura possano fare scattare la POT mode
    {
    modetable[chan] = 11;
    typetable[chan]= 176; 
    }
   //  test1();
    } 
    
  
      
  }
 qwertyvalue[chan] = valore /32; // registra la lettura per un futuro confronto in un range 0-255
  }
  else
  {
   
     if (modetable[chan] < 11) //  valore = analogRead(plexer); 
    //valore = !boolean(digitalRead(plexer))*1024; // digital_read is faster
     #if defined (__AVR_ATmega32U4__)
     valore = digitalRead(plexer+18)*1020; //
     #endif
     #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
     valore = digitalRead(plexer+14)*1020; //
     #endif
     
    else if (modetable[chan] < 19)     valore = analogRead(plexer); // pots 

   #if (encoders_generic == 1)
    else if (modetable[chan] == 19)                                 // encoders
    {
       #if defined (__AVR_ATmega32U4__)
      MSB[1]=   digitalRead(plexer+18); //
      LSB[1]=   digitalRead(plexer+19); //
    //  Serial.println(MSB[1]);
    //  Serial.println(LSB[1]);
    //  Serial.println("-");
    //  delay(100);
     #endif
     #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
     MSB[1] = digitalRead(plexer+14); //
     LSB[1]=   digitalRead(plexer+15);
     #endif
      
      }
      #endif
  
    }

// if (chan == remapper(33)) {Serial.println(valore); Serial.println(modetable[chan]);  Serial.println("-");  }
// delay(2);
 
     ain_nucleo(); 
     
  
  
  }
   
  /////////////////////////////
 if (minvalue[general_mempos] > 0) 
  { 
    #if (pullups_active == 1)
    digitalWrite(9, HIGH);
    #endif
    
       #if defined (__AVR_ATmega32U4__)  
    valore = analogRead(9); //if ( valuetable[general_mempos] >= 7) 
    #endif

  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
  valore = 1000*(!digitalRead(9)); 
  #endif
  
  chan = channel + 48;// else chan = 48;
  ain_nucleo();  }

  
  
  else virtual_touch_end(1); // se non c'ÃƒÂ¨ l'extraplex c'ÃƒÂ¨ il touch2 // col suo virtualtouch
  
  
  }   
  #if (main_encoder == 1)
  else {
   encoder(encoder_mempos[0]);                           
  }
  #endif

  


   } // end PLEXER
   } // end ain

   #endif

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  
  

  
    #if (stratos == 0) 
  void detect_plexer()
  {
     {
if (channel >4) scala_learn = -1; else scala_learn = 1; 
// mi serviva un variabile int non usata nel default preset... 
// scala_learn qui mi serve per leggere un input plexer diverso dall'attuale
// solo nel caso in cui channel > 4 devo andare a leggere un input " in avanti" altrimenti rischierei di leggere 
// l'input di un'altro plexer

      if (valuetable[chan] == valuetable[chan+scala_learn]) // solo se non e' ancora stata effettuata la diversificazione
   
     { //----------------------
      if ((valore/128) <20 && qwertyvalue[chan+scala_learn]>30) 
      // se il valore attuale e' diverso da un valore precedente all'interno dello stesso gruppo-plexer
        { // i valori di valuetable dentro il gruppo-plexer vanno resi tutti diversi
         { setPlexer(channel+scala_learn);

          if (  analogRead(plexer)> upper_val)  /// valori molto diversi
      //   {     setPlexer(channel+(scala_learn*2)); // controlla ancora
      //    if ( analogRead(plexer) > upper_val )         
      //    diversifica_valuetable (); 
      //    }
         
          diversifica_valuetable (); 
          
          setPlexer(channel);  // torna al plexer normale
          }              }
      }  // --------------------------

     
      }

     
}


 
  
    
void diversifica_valuetable ()
{
   for (byte i = 0; i <8; i++)
          { valuetable[i+(plexer*8)] = i+(plexer*8)   +60
          ;
            }
}
#endif

///////////////////////////////////////////////////////

   void ain_nucleo()
{

{
 // if (chan == 2) Serial.println(valore); // monitoraggio velostat.
  }

  
   if ( modetable[chan] == 0) {}
  else if ( modetable[chan] > 0 && modetable[chan]< 11) {push_buttons(); }
  else if (modetable[chan] < 17)  pots();  // pots + hypercurves - 11, 12,13,14,15,
   else if (modetable[chan] == 17)   { if (valore < 512) lastbutton[page_mempos] =0; else lastbutton[page_mempos] =1;} // page switch
 
   else if (modetable[chan] == 18) {                  // beam

      #if defined (__AVR_ATmega32U4__)
    digitalWrite(18+plexer, LOW);   // tolgo la pullup
    valore = analogRead(plexer);  beam(); 
    digitalWrite(18+plexer, HIGH); // rimetto la pullup
      #endif

       #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
    digitalWrite(14+plexer, LOW);  valore = analogRead(plexer); beam(); digitalWrite(14+plexer, HIGH);
       #endif

      
    }
    #if (encoders_generic == 1)
    else if (modetable[chan] == 19) {updateEncoder(chan); 
  //  Serial.println(lastbutton[chan]); delay(100);
    encoder(chan); }
    #endif
    

    #if defined (__AVR_ATmega32U4__)  
    if (mouse_mempos > 0 ) mouse_control () ;
    #endif
}



   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void scale_learn(byte pitch)
{


   if (lastbutton[touch_mempos[0]] == 0 || lastbutton[touch_mempos[1]] == 0)  // se il touch ÃƒÆ’Ã‚Â¨ premuto/toccato
  {
  if (dmxtable[encoder_mempos[0]] == 3)  encodervaluepot[0] = pitch * 8;
   
   if (scala_reset == 1)                    // azzera la scala
   for (byte i =0; i < scala_lenght; i++){
    scala_array[i] = 0;
    scala_learn= 0;
     scala_reset =0;  } 

     
    scala_array[scala_counter] = (pitch - (pitch/12)*12)+1; // 0 == nessuna nota,
  scala_counter++;
  if (scala_counter >= scala_lenght) scala_counter = 0;
  scala_learn =0;
  for (byte i =0; i< scala_lenght; i++)
  {
  // if (scala_array[i] != 0) 
   if (scala_array[i] >0 ) // 0 = nessuna nota da caricare nella scala
   bitWrite(scala_learn, scala_array[i]-1, 1);
    }
    
}
  }

   void offgroup (byte canale, byte midiout)
   {    for (int i = 0; i <56; i++) { // se i pulsanti sono nello stesso gruppo e sono accesi, spegnere lucetta e memorizzare status toggle

  if (modetable[i] == modetable[canale] && i!=canale) {
  
 if (
 bit_read(4,i) == 1
 || 
 bit_read(4,i+max_modifiers) == 1
 )  
  
{  if (midiout ==1) outnucleo(0,i);
 }  
 #if (shifter_active == 1 && stratos == 0)
  shifter.setPin(lightable[i]-1, 0); 
  #endif 
  
  bit_write(4,i+page,0);
 bit_write(1,lightable[i]+page-1,0);
  } 
  }
}

#if (stratos == 0 )
void setPlexer( byte channel)
{
   // set the selector pins HIGH and LOW to match the binary value of channel
   for(byte bit = 0; bit < 3; bit++)
   {
      byte pin = bit+4; // the pin wired to the multiplexer select bit
      byte isBitSet =  bitRead(channel, bit); // true if given bit set in channel
      digitalWrite(pin, isBitSet);
   }
}
#endif
 
