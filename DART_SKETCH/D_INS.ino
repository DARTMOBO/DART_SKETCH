 /*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */
 
  #if (stratos == 0)
  void AIN()
 { 
for( channel = 0; channel < 8; channel++)    /// per ognuno degli 8 channels del multiplexer vado poi a leggere tutti gli ingressi analogici (cioè l'uscita di ogni plexer)
{ 
   
  
   #if (LED_rings == 1) // per ora non ci interessa. questa è una void sperimentare per led rings ntorno agli encoders.. da fer in futuro.
   LED_rings_ ();
   shifter.write();
   #endif 
   
  
#if (top_spinner == 1) 
     if (lastbutton[encoder_mempos[0]] == 64 || dmxtable[general_mempos] == 0)  //  [encoder_mempos[0] è l0'idirizzo di memoria per lo spinner principale, quello che dave avere maggiore risoluzione e priorità di esecuzione su tutto.
    
    
      // 64 = no encoder action - the MAIN spinner has priority over any other action.
     
#endif


{ // parentesi utile per folding del codice - finisce a riga 171
  

    virtual_touch_end(0); // dentro ain normale
    restore_end(); // ?? non ricordo cosa faceve questa void - aggiunegre commento urgentemente
    
    
  if (maxvalue[general_mempos] == 0 ){ PADS();}  


  //  if (dmxtable[general_mempos] >1)
  //  {senseEncoder_2nd();}  // carica MSB LSB [1] // // gli input pin su cui viene letto il secondo encoder sono 33 e 41



 ///////////////////////////////////////////////////////////////////   
 
  if (valuetable[general_mempos] !=1 ) // nomobo setup
 setPlexer(channel); // all 4051's are set on the channel to be read // qui vengono effettivamente guidati i 4051 della dartmobo, per essere settati di volta in volta sul canale successivo . channel, ricordiamo , va da 1 a 8
  
  
   // if (dmxtable[general_mempos] >1)
   // {senseEncoder_2nd();}  // carica MSB LSB [1] // // gli input pin su cui viene letto il secondo encoder sono 33 e 41


 for(plexer = 0; plexer < 
  //5+boolean(maxvalue[general_mempos]) // se si attivano i pads (mettendo maxvalue = 0) l'analogico A5 non viene letto // OPZIONE 1
  6 // OPZIONE 2
  ; plexer++) //  plexer 0,1,2,3,4 - the 5th plexer is read at higher speed (pads and 2nd encoder)
  
  {
  
    
   chan = (plexer * 8) + channel  ;        // chan sarà il mio punto di riferimento per tutti e 56 (48+ EXTRA PLEXER) gli input della mia DARTMOBO. 
                        
 

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
 qwertyvalue[chan] = valore /32;                                // registra la lettura per un futuro confronto in un range 0-255 // spiegare meglio questo meccanismo - spiegazione non chiara. 
  }
////////////////////////////////////////////////////////////////////////////////////////////////// fine autodetect
  
  else     // trovo un preset nella eeprom - allora la macchina funziona in base al preset che ha in memoria,ad ogni input corrisponde un tipo di "traduzione" esatta
  {

     #if (side_spinner == 1)
   if (channel == 5 && dmxtable[general_mempos] >1) { // gestione del SIDE SPINNER    // dmxtable[general_mempos] >1 significa che un side spinner è stato "istituito" via editor, nel mio preset. 
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

// ===========================
// MODALITÀ DI INPUT (modetable[chan])
// ===========================
//  0–10   → Pulsanti digitali (lettura rapida con digitalRead)
//           Usati per trigger veloci o pulsanti on/off semplici.
// 11–15   → Potenziometri analogici (lettura con analogRead)
//           Controlli continui tipo fader, slider, rotativi lenti.
// 16–18   → Potenziometri con curve speciali (hypercurve, log/exp?)
//           Per controlli con risposta non lineare.
// 19      → Encoder rotativo (lettura MSB/LSB su 2 pin digitali)
//           Tipico per knob infiniti o jogwheel laterali.
// 27      → Modalità speciale non documentata (da verificare).
// 29–30   → Funzioni speciali (es. reset hardware, toggle di sistema).
//           Usati raramente, attivati da preset o in condizioni particolari.
//
// NOTE: Altri valori potrebbero essere validi se gestiti altrove (es. modetable > 30)
// ===========================
                
       /*                
     if (modetable[chan] < 11 || modetable[chan] == 29 || modetable[chan] == 30 || modetable[chan] == 27) // per tutti i pulsanti si usa digitalread, che legge in modo più rapido
         #if defined (__AVR_ATmega32U4__)
       //  valore = digitalRead(plexer+18)*1020; // valore = digitalRead(pin) << 10;
            valore = digitalRead(plexer+18) << 10;

         #endif
         #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
        //  valore = digitalRead(plexer+14)*1020; //
        valore = digitalRead(plexer+14) << 10;
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
      updateEncoder(chan); 
      }
   #endif
*/
   byte readmode = modetable_readmode[modetable[chan]];

switch (readmode) {
  case 0: // Pulsanti → digitalRead
    #if defined (__AVR_ATmega32U4__)
    //  valore = digitalRead(plexer + 18) * 1020;
        valore = digitalRead(plexer+18) << 10;
    #elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
  //    valore = digitalRead(plexer + 14) * 1020;
          valore = digitalRead(plexer+14) << 10;
    #endif
    break;

  case 1: // Potenziometri / analogici → analogRead
    valore = analogRead(plexer);
    break;

  case 2: // Encoder (digitalRead x2)
    #if defined (__AVR_ATmega32U4__)
      MSB[1] = digitalRead(plexer + 18);
      LSB[1] = digitalRead(plexer + 19);
    #elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
      MSB[1] = digitalRead(plexer + 14);
      LSB[1] = digitalRead(plexer + 15);
    #endif
    updateEncoder(chan);
    break;

  
}

  
    }

  // if ((mousex-127) != 0 ) 
//  if (chan == 39)
 //  {  Serial.print(valore); 
 //  Serial.println("  /  ");//   Serial.println(mousex-127);
 //  delay(50);
 // }
 
     ain_nucleo(); // vai a eseguire effettivamente la traduzione da segnale elettrico a segnale MIDI, secondo tutte le modalità disponibili. 
     
  
  
  }
   
  //  __________________________________________________________
  
 if (minvalue[general_mempos] > 0)  // vedi se il plexer EXTRA è attivato // 1= extra plexer attivato - gli input della dartmobo passano da 48 a 56! adesso il pin che prima veniva dedicato al touch sensor 2 viene dedicato alla letture dell'extra plexer - nota: usant touch ic's esterni è possibile avere entrambe le cose. 
           { 
            #if (pullups_active == 1)
            digitalWrite(9, HIGH);
            #endif
    
            #if defined (__AVR_ATmega32U4__)  
            valore = analogRead(9); 
            #endif

            #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
          //  valore = 1000*(!digitalRead(9)); 
           valore =  (!digitalRead(9)) << 10;
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
   
   encoder(encoder_mempos[0]);         // esecuzione con out MIDI dello spinner principale                  
  }
  #endif
  

  


   } // end PLEXER
   } // end ain
   #endif



   void ain_nucleo()
{
  #if defined(__AVR_ATmega32U4__)
  if (mouse_mempos > 0) mouse_control();
  #endif

  uint8_t mode = modetable[chan];

  switch (mode) {

    case 0:   // Blind Input
      break;

    case 1 ... 10:  // Button, Toggle, Toggle Group 1–4, Radio Group 1–4
      push_buttons(0);
      break;

    case 11 ... 15:  // POT, Hypercurve 1–2, Center-curve, Center-curve2
      pots();
      break;

    case 16:  // SEQ
      seq();
      break;

    case 17:  // page switch
      #if (page_active == 1)
      if (qwertyvalue[page_mempos] == 1) {
        if (valore < 512) {
          if (lastbutton[page_mempos] == lastbutton_debounce) {
            pagestate = !pagestate;
          }
          lastbutton[page_mempos] = 0;
        } else {
          if (lastbutton[page_mempos] < lastbutton_debounce) {
            lastbutton[page_mempos]++;
          }
        }
      } else {
        pagestate = (valore < 512) ? 0 : 1;
      }
      #endif
      break;

    case 18:  // distance sens.
      #if defined (__AVR_ATmega32U4__)
      digitalWrite(18 + plexer, LOW);
      valore = analogRead(plexer);
      beam();
      digitalWrite(18 + plexer, HIGH);
      #elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
      digitalWrite(14 + plexer, LOW);
      valore = analogRead(plexer);
      beam();
      digitalWrite(14 + plexer, HIGH);
      #endif
      break;

    case 19:  // encoder
      #if (encoders_generic == 1)
      encoder(chan);
      #endif
      break;

    case 20:  // PIEZO-PADS
      // (non implementato qui)
      break;

    case 21:  // SPINNER 1
      // (non implementato qui)
      break;

    case 22:  // SPINNER 2
      // (non implementato qui)
      break;

    case 23:  // touch 1
      // (non implementato qui)
      break;

    case 24:  // touch 2
      // (non implementato qui)
      break;

    case 25:  // MOUSE
      // handled at top via mouse_mempos
      break;

    case 26:  // GENERAL
      // (non implementato qui)
      break;

    case 27:  // VELO-PADS
      push_buttons(1);
      break;

    case 28:  // SPRITE
      // (non implementato qui)
      break;

    case 29:  // enc.RESET
      reset();
      break;

    case 30:  // SHIFTER
      shifter_modifier();
      break;

    case 31:  // USER 1
      user_item1();
      break;

    case 32:  // USER 2
      user_item2();
      break;

    case 33:  // USER 3
      user_item3();
      break;

    case 34:  // USER 4
      user_item4();
      break;

    default:  // modalità non gestita
      break;
  }
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
 
