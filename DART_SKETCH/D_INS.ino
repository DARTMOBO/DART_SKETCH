
  #if (stratos == 0)
  void AIN()
 { 
for( channel = 0; channel < 8; channel++)    /// per ognuno degli 8 channels del multiplexer vado poi a leggere tutti gli ingressi analogici (cioè l'uscita di ogni plexer)
{ 
   
  
   #if (LED_rings == 1)
   LED_rings_ ();
   shifter.write();
   #endif 
   
  
#if (top_spinner == 1) 
     if (lastbutton[encoder_mempos[0]] == 64 || dmxtable[general_mempos] == 0) 
    
      // 64 = no encoder action - the MAIN spinner has priority over any other action.
     
#endif


{
  

    virtual_touch_end(0); // dentro ain normale
    restore_end();
    
  if (maxvalue[general_mempos] == 0 ){ PADS();}  


  //  if (dmxtable[general_mempos] >1)
  //  {senseEncoder_2nd();}  // carica MSB LSB [1] // // gli input pin su cui viene letto il secondo encoder sono 33 e 41



 ///////////////////////////////////////////////////////////////////   
 
  if (valuetable[general_mempos] !=1 ) // nomobo setup
 setPlexer(channel); // all 4051's are set on the channel to be read
  
   // if (dmxtable[general_mempos] >1)
   // {senseEncoder_2nd();}  // carica MSB LSB [1] // // gli input pin su cui viene letto il secondo encoder sono 33 e 41


 for(plexer = 0; plexer < 
  //5+boolean(maxvalue[general_mempos]) // se si attivano i pads (mettendo maxvalue = 0) l'analogico A5 non viene letto // OPZIONE 1
  6 // OPZIONE 2
  ; plexer++) //  plexer 0,1,2,3,4 - the 5th plexer is read at higher speed (pads and 2nd encoder)
  
  {
  
    
   chan = (plexer * 8) + channel  ;                              
 

 ///////////////////////////////////////////////////////////////////////////////////// autodetect
  if (eeprom_preset_active == 0) // se NON trovo un preset nella eeprom 
  {
    valore = analogRead(plexer);
    
    if (valore < upper_val   /// se premo un pulsante - valore scende
     && dmxtable[chan] < 3
    )  
   {  
    detect_plexer(); 

   if (valore > 448 && valore < 576   ) // se si ha un valore intermedio
  {
    {
    modetable[chan] = 11;
    typetable[chan]= 176; 
    }
   //  test1();
    }    
  }
 qwertyvalue[chan] = valore /32;                                // registra la lettura per un futuro confronto in un range 0-255
  }
////////////////////////////////////////////////////////////////////////////////////////////////// fine autodetect
  
  else      // trovo un preset nella eeprom
  {

     #if (side_spinner == 1)
   if (channel == 5 && dmxtable[general_mempos] >1) { // gestione del SIDE SPINNER 
      #if defined (__AVR_ATmega32U4__)
      MSB[1]=   digitalRead(22);
      LSB[1]=   digitalRead(23);
      #endif
      #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
      MSB[1]=   digitalRead(18);
      LSB[1]=   digitalRead(19);
      #endif
      
      updateEncoder(encoder_mempos[1]); 
      encoder(encoder_mempos[1]);
                       }
 #endif

                       
                       
     if (modetable[chan] < 11 || modetable[chan] == 29 || modetable[chan] == 30 || modetable[chan] == 27) // per tutti i pulsanti si usa digitalread, che legge in modo più rapido
         #if defined (__AVR_ATmega32U4__)
         valore = digitalRead(plexer+18)*1020; //
         #endif
         #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
         valore = digitalRead(plexer+14)*1020; //
         #endif
     
     else if (modetable[chan] < 19 // || modetable[chan] == 27
     )     valore = analogRead(plexer); // si usa analogread per i pots 

   #if (encoders_generic == 1)
      else if (modetable[chan] == 19)                                 // encoders 
      {
      #if defined (__AVR_ATmega32U4__)
      MSB[1]=   digitalRead(plexer+18); //
      LSB[1]=   digitalRead(plexer+19); //
      #endif
   
      #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
      MSB[1] = digitalRead(plexer+14); //
      LSB[1]=   digitalRead(plexer+15);
      #endif
      }
   #endif

   
  
    }

  // if ((mousex-127) != 0 ) 
//  if (chan == 39)
 //  {  Serial.print(valore); 
 //  Serial.println("  /  ");//   Serial.println(mousex-127);
 //  delay(50);
 // }
 
     ain_nucleo(); 
     
  
  
  }
   
  //  __________________________________________________________
  
 if (minvalue[general_mempos] > 0)  // vedi se il plexer EXTRA è attivato
            { 
            #if (pullups_active == 1)
            digitalWrite(9, HIGH);
            #endif
    
            #if defined (__AVR_ATmega32U4__)  
            valore = analogRead(9); 
            #endif

            #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
            valore = 1000*(!digitalRead(9)); 
            #endif
  
            chan = channel + 48;
            ain_nucleo(); 
            }
 // ______________________________________________________
  
  
 // else virtual_touch_end(1); // se non c'e' l'extraplex, c'e' il touch2 // col suo virtualtouch
  virtual_touch_end(1);
  
  }   

  
  #if (top_spinner == 1)
  else {
   
   encoder(encoder_mempos[0]);                           
  }
  #endif
  

  


   } // end PLEXER
   } // end ain
   #endif

////////////////////////////////////////////////////////////////////////////////////////////////////////

   void ain_nucleo()
{    
    #if defined (__AVR_ATmega32U4__)  
    if (mouse_mempos > 0 ) mouse_control () ;
    #endif
  
  if ( modetable[chan] == 0) {}
  else if ( modetable[chan] > 0 && modetable[chan]< 11) {push_buttons(0); }
  else if (modetable[chan] < 16)  pots();  // pots + hypercurves - 11, 12,13,14,15,
  else if  (modetable[chan] == 16) seq();
  else if (modetable[chan] == 17)   { if (valore < 512) lastbutton[page_mempos] =0; else lastbutton[page_mempos] =1;} // page switch

   else if (modetable[chan] == 18) {                  // beam
      #if defined (__AVR_ATmega32U4__)
    digitalWrite(18+plexer, LOW);   // tolgo la pullup
    valore = analogRead(plexer);  beam(); 
    digitalWrite(18+plexer, HIGH); // rimetto la pullup
      #endif
       #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
    digitalWrite(14+plexer, LOW);  
    valore = analogRead(plexer); beam(); 
    digitalWrite(14+plexer, HIGH);
       #endif     
    }

    
    #if (encoders_generic == 1)
    else if (modetable[chan] == 19) {updateEncoder(chan); 
    encoder(chan); }
    #endif
    
      else if (modetable[chan] == 27) push_buttons(1); // push buttons con velocity
      else if (modetable[chan] == 29) reset();
      else if (modetable[chan] == 30) shifter_modifier();
      else if (modetable[chan] == 31) user_item1();
      else if (modetable[chan] == 32) user_item2();
      else if (modetable[chan] == 33) user_item3();
      else if (modetable[chan] == 34) user_item4();
  

  
    
}
   
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

 ////////////////////////////////////////////////////////////////////////////////////////////////////
     
void diversifica_valuetable ()
{
   for (byte i = 0; i <8; i++)
          { valuetable[i+(plexer*8)] = i+(plexer*8)   +60
          ;
            }
}
#endif

//////////////////////////////////////////////////////////////////////
void scale_learn(byte pitch)
{


   if (lastbutton[touch_mempos[0]] == 0 || lastbutton[touch_mempos[1]] == 0)  // se touch premuto/toccato
  {
  if (dmxtable[encoder_mempos[0]] >1)  encodervaluepot[0] = pitch * 8;
   
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

//////////////////////////////////////////////////////////////////////
 void update_scala(byte quale_spinner) // quale_spinner è 0 top 1 side
 // richiamato da setup_mempos() e da switchpage()
  {
    // scala[4] è un array che cotiene 4 scale... scalaSpinner0-page1, scalaSpinner1-page1, ScalaSpinner0-page2, scalaSpinner1-page2
    // per caricare una scala dalla eeprom di servono 12 BIT - per le 12 note di un'ottava
    // i primi 7 BIT sono dentro valuetable
    // gli altri 5 BIT sono dentro lastencoded[2]
    // lastEncoded[] viene ricaricata ogni volta che si carica un preset - quindi è sempre "attuale" rispetto alla pagina.

  
//scala[0] = 14;
   
  for (byte i = 0; i< 7; i++) {
 bitWrite(scala[quale_spinner+(page/max_modifiers)*2],i,  bitRead(valuetable[encoder_mempos[quale_spinner]+page] ,i)   );
// bitWrite(scala[0],i,  bitRead(valuetable[encoder_mempos[quale_spinner]+page] ,i)   );


}
  for (byte i = 0; i< 5; i++) {

 bitWrite(scala[quale_spinner+(page/max_modifiers)*2],i+7,  bitRead(EEPROM.read(encoder_mempos[quale_spinner]+320+(page/max_modifiers*512)), i)    );
// bitWrite(scala[0],i+7,  bitRead(EEPROM.read(encoder_mempos[quale_spinner]+320+(page/max_modifiers*512)), i)    );

}

 







/*


for (byte i = 0; i< 12; i++) {
Serial.print(bitRead(scala[0],i));}
Serial.println(" - scala[]");
*/


  }

//////////////////////////////////////////////////////////////////////
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
 
