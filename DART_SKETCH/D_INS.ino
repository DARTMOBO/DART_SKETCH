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
   
  
 
   
  
#if (top_spinner == 1) 
     if (lastbutton[encoder_mempos[0]] == 64 || dmxtable[general_mempos] == 0)  //  [encoder_mempos[0] è l0'idirizzo di memoria per lo spinner principale, quello che dave avere maggiore risoluzione e priorità di esecuzione su tutto.
    
    
      // 64 = no encoder action - the MAIN spinner has priority over any other action.
     
#endif


{ // parentesi utile per folding del codice - finisce a riga 171
  

    virtual_touch_end(0); // dentro ain normale
    restore_end(); // ?? non ricordo cosa faceve questa void - aggiunegre commento urgentemente
    
    
#if Piezo_pads
  if (maxvalue[general_mempos] == 0 ){ piezo_pads();} 
#endif


  //  if (dmxtable[general_mempos] >1)
  //  {senseEncoder_2nd();}  // carica MSB LSB [1] // // gli input pin su cui viene letto il secondo encoder sono 33 e 41



 ///////////////////////////////////////////////////////////////////   
 
  if (valuetable[general_mempos] !=1 ) // nomobo setup
 setPlexer(channel); // all 4051's are set on the channel to be read // qui vengono effettivamente guidati i 4051 della dartmobo, per essere settati di volta in volta sul canale successivo . channel, ricordiamo , va da 1 a 8
  //delay(1); prova per vedere se si stabilizzano le letture dopo il cambio di canale-  a volte vedo dei rinculi a V
  //delayMicroseconds(400);
   // if (dmxtable[general_mempos] >1)
   // {senseEncoder_2nd();}  // carica MSB LSB [1] // // gli input pin su cui viene letto il secondo encoder sono 33 e 41

 //valore = analogRead_1024(0); // dummy read generale

 for(plexer = 0; plexer < 
  //5+boolean(maxvalue[general_mempos]) // se si attivano i pads (mettendo maxvalue = 0) l'analogico A5 non viene letto // OPZIONE 1
  6 // OPZIONE 2
  ; plexer++) //  plexer 0,1,2,3,4 - the 5th plexer is read at higher speed (pads and 2nd encoder)
  
  {
  
    
   chan = (plexer * 8) + channel  ;        // chan sarà il mio punto di riferimento per tutti e 56 (48+ EXTRA PLEXER) gli input della mia DARTMOBO. 
                        
 

 ///////////////////////////////////////////////////////////////////////////////////// autodetect
  #if (ENABLE_AUTODETECT == 1)
  if (eeprom_preset_active == 0) // se NON trovo un preset nella eeprom 
  {

// CTRL-F: AUTODETECT_DOC_SIMPLE
// AUTODETECT (attivo solo quando NON esiste un preset valido)
// In questa modalità il firmware parte da aux_preset(): tutti gli input sono "button" (modetable=1).
// Qui dentro, AIN() osserva i valori analogici e fa due cose:
//
// 1) Se vede "attività" (valore sotto una soglia), chiama detect_plexer():
//    - serve a diversificare valuetable[] nel gruppo di 8 canali, evitando note duplicate.
//
// 2) Se il valore è in una fascia "intermedia", promuove l'input a POT:
//    - modetable[chan] = 11  (POT)
//    - typetable[chan] = 176 (CC)
//
// Nota LED (anti "blinker party"):
// Quando un canale passa a POT, possono attivarsi gli effetti LED tipici dei pot.
// Per evitare lampeggi confusionali durante AUTODETECT, si può forzare lightable[chan]=0
// nel punto in cui viene impostato modetable=11.

    
    valore = analogRead_1024(plexer);
    
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
    lightable[chan] = 0;
    }
   //  test1();
    }    
  }
 qwertyvalue[chan] = valore /32;                                // registra la lettura per un futuro confronto in un range 0-255 // spiegare meglio questo meccanismo - spiegazione non chiara. 
  }
////////////////////////////////////////////////////////////////////////////////////////////////// fine autodetect
  
  else     // trovo un preset nella eeprom - allora la macchina funziona in base al preset che ha in memoria,ad ogni input corrisponde un tipo di "traduzione" esatta
  #endif
  {

     #if (Side_spinner == 1)
   if (channel == 5 && dmxtable[general_mempos] >1) 
   Side_spinner_read();
  
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

   #if (encoders_ == 1)
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
 
    #if (Dummy_read == 1)
        #if (pullups_active == 1)
        digitalWrite(18+plexer, LOW);
        #endif
        valore = analogRead_1024(plexer);
        delayMicroseconds(5); 
        valore = analogRead_1024(plexer);
        #if (pullups_active == 1)
        digitalWrite(18+plexer, HIGH);
        #endif
 #endif
  #if (Dummy_read == 0)
      valore = analogRead_1024(plexer);
 #endif

      
    break;

  case 2: // Encoder (digitalRead x2)
    #if defined (__AVR_ATmega32U4__)
   //  valore = analogRead_1024(plexer);
  //   valore = analogRead_1024(plexer+1);
      MSB[1] = digitalRead(plexer + 18);
      LSB[1] = digitalRead(plexer + 19);
      
    #elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
      MSB[1] = digitalRead(plexer + 14);
      LSB[1] = digitalRead(plexer + 15);
    #endif
    updateEncoder(chan);
    break;

  // ila CASE 3 è vuoto, serve per saltare la lettura dove non serve

  case 4:
        #if (pullups_active == 1)
        digitalWrite(18+plexer, LOW);
        #endif
     
      //  delayMicroseconds(5); 
        valore = analogRead_1024(plexer);
        
        #if (pullups_active == 1)
        digitalWrite(18+plexer, HIGH);
        #endif
  
  break;
  
}


// if (chan == 9)  aindbg_valueOnly(valore);  // monitoraggio di un determinato chan
  
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

  #if (Extraplexer ==1) 

  Extraplexer_read();
 
  #endif          
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
   #if (hid_mouse == 1)
  #if defined(__AVR_ATmega32U4__)
  if (mouse_mempos > 0) mouse_control();
  #endif
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

#if (Seq_ == 1)
    case 16:  // SEQ
      seq();
      break;
#endif

    case 17:  // page switch
      #if (Page_switch == 1)
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
      } else { // page active = 0 - lever
        pagestate = (valore < 512) ? 1 : 0;
      }
      #endif
      break;

#if (Distance_sensor == 1)
    case 18:  // distance sens.
      #if defined (__AVR_ATmega32U4__)
      digitalWrite(18 + plexer, LOW);
      valore = analogRead_1024(plexer);
      beam();
      digitalWrite(18 + plexer, HIGH);
      #elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
      digitalWrite(14 + plexer, LOW);
      valore = analogRead_1024(plexer);
      beam();
      digitalWrite(14 + plexer, HIGH);
      #endif
      break;
#endif

    case 19:  // encoder
      #if (encoders_ == 1)
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
      
      case 37:  // qwerty_pot
      pots();
      break;

   #if Scene
    case 38:  // scene_control
      scene_control_pot();
      break;

    case 39:  // scene_record_button
      scene_record_button();
      break;
#endif

  // case 40:  // scene_control
   //  encoder(chan);
     // break;


    default:  // modalità non gestita
      break;
  }
}

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  #if (stratos == 0) && (ENABLE_AUTODETECT == 1)
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

          if (  analogRead_1024(plexer)> upper_val)  /// valori molto diversi
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

#endif // stratos==0 && ENABLE_AUTODETECT


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

 #if (monitor_temporizzato == 1)
// [AINDBG_VALUE_ONLY]
// Stampa SOLO "valore" ogni 300ms. Da chiamare in ain() dopo la lettura.

void aindbg_valueOnly(int valore_ora)
{
  static unsigned long tLast = 0;
  unsigned long now = millis();
  if ((now - tLast) < 300) return;
  tLast = now;

  Serial.println(valore_ora);
}
 #endif 

 
