/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */

// MODIFIERS 

// ============================================================
// CTRL-F: VELO_PAD_SQUEEZE_HELPER
// data_MA[] per MODE 27 (velocity pads):
//   0       = velocity originale, nessuno schiacciamento
//   1..126  = schiacciamento progressivo verso 127
//   127     = toggle netto; ON forzato a 127, OFF resta 0
//
// Formula parsimoniosa: non aggiunge un offset fisso, ma riempie
// progressivamente la distanza tra la velocity letta e il soffitto 127.
// Cosi' una botta gia' forte cambia poco, una botta debole viene aiutata di piu'.
// ============================================================
byte velo_pad_squeeze(byte v) {
  byte squeeze = data_MA[chan];

  if (v == 0 || squeeze == 0) return v;

  // Toggle velocity-pad: il sensore serve solo per superare la soglia,
  // ma il messaggio MIDI deve essere netto come un pulsante digitale.
  if (squeeze == 127) return 127;

  return v + (byte)(((uint16_t)(127 - v) * squeeze) / 126);
}


#if (VELO_PAD_RELEASE_SCANS > 0)
// CTRL-F: VELO_PAD_RELEASE_FILTER
// Solo velo==1; condiviso dai rami staged e legacy.
// data_LB: 0 = premuto; 1..data_LB_debounce = riarmo storico (boot = 1);
//          128+n = rilascio candidato, dopo n letture consecutive sopra soglia.
// Il bit alto distingue il candidato dal riarmo: nessun OFF al boot a riposo.
// La pressione azzera data_LB nel chiamante; qui annulliamo anche nella fascia
// intermedia lower..upper_val. Il rilascio confermato riabilita il prossimo ON.
static bool velo_pad_release_ready(
) {
  byte state = data_LB[chan];

  if (valore <= upper_val) {
    if (state & 0x80) data_LB[chan] = 0;
    return false;
  }

  if (state == 0) state = 0x80;
  if (state & 0x80) {
    if (++state == (0x80 + VELO_PAD_RELEASE_SCANS)) {
      data_LB[chan] = data_LB_debounce;
      return true;
    }
  } else if (state < data_LB_debounce) {
    ++state; // riarmo iniziale: nessun evento di rilascio
  }
  data_LB[chan] = state;
  return false;
}
#endif

//---------------------------------------------------------------------------------------------------------------
#if (STAGED_BUTTON_READ_TEST == 1)
// -----------------------------------------------------------------
// STAGED BUTTON SYSTEM ENABLED:
// - keep the public API name 'push_buttons()' so the rest of the sketch
//   compiles unchanged
// - but route everything through staged LETTURA+EFFETTO
// - the legacy implementation (big body) is NOT compiled (flash saved)
// -----------------------------------------------------------------
void push_buttons_lettura_stage(byte velo);
void push_buttons_effetto_stage(byte velo);

void push_buttons(byte velo) {
  push_buttons_lettura_stage(velo);
  push_buttons_effetto_stage(velo);
}
#else
void push_buttons(byte velo) {
  

// gestione pulsanti
  // la variabile "velo" ha valore 0 o 1 - se impstata su 1 attiva la lettura della velocity
  // data_LB[] gestisce il debouncing del pulsante 
  //          - si fa riferimento a data_LB_debounce che è una variabile fissa 
   
  //            in partica serve come temporizzatore per evitare messaggi ripetuti in caso di pulsanti scadenti che non fanno bene contatto
  //
  // lower_val e upper_val - sono due variabili fisse che indicano i valori sotto e soprai quali si considera che un pulsante sia premuto
  // 

  {
    // if (valore < lower_val - (velo *min)    )                         ///// button pushed        // con questa formula più è alto min , meno il pad diventa sensibile
    int lower = lower_val + (velo * (data_MI[chan] * 3));
    if (valore < lower) {
      if (data_LB[chan] == data_LB_debounce) {
        /*
        Serial.print(data_MODE[19]); Serial.println(" - data_MODE");
        Serial.print(data_DM[19]); Serial.println(" - data_DM - modalità endless / pot");
        Serial.print(data_QW[19]); Serial.println(" - touchstop");
        Serial.print(data_MI[19]-32); Serial.println(" - speed");
        */

        /*
        ============================================================
        CTRL-F: PAGESWITCH_DIAG_PRINT_ON_BUTTON  (DISABLED)
        Diagnostica takeover/pageswitch (contatori + dump arm bank5)
        usata solo per inchiodare il problema del takeover armato al boot.
        Ora rimossa/commentata.
        ============================================================

        for (byte i = 0; i < max_modifiers; i++) {
          Serial.println(bit_read(5, i + 0));
        }

        static byte ps_diag_printed = 0;
        if (!ps_diag_printed) {
          ps_diag_printed = 1;
          Serial.print(F("[PS] calls="));    Serial.print(ps_calls);
          Serial.print(F(" changes="));      Serial.print(ps_changes);
          Serial.print(F(" arm="));          Serial.println(ps_arm);
          Serial.print(F("[PS] first page="));      Serial.print(ps_first_page);
          Serial.print(F(" first pagestate="));     Serial.print(ps_first_pagestate);
          Serial.print(F(" first reason="));        Serial.println(ps_first_reason);
          Serial.print(F("[PS] setup page="));       Serial.print(ps_setup_page);
          Serial.print(F(" setup pagestate="));     Serial.println(ps_setup_pagestate);
        }
        */
        if (data_MODE[chan] >= 3 && data_MODE[chan] != 27) {
          offgroup(chan, 1);      // da 3 in poi ci sono i toggle groups e radio groups
        }
      
        if (bit_read(4, page + chan) == 0) { // 4 = toggletable // something happens only if the button is off in the toggletable
          #if (ENABLE_AUTODETECT == 1)
          if (eeprom_preset_active == 0) {
            data_DM[chan]++;    // autodetect_dmx
          }
          #endif
        
          //  Serial.println(mouse_mempos);
          #if defined (__AVR_ATmega32U4__)  
          HOT_keys(chan, 1);    
          #endif  
          
          #if (Scale == 1)
          if (data_TY[chan + page] < 160) {
            scale_learn(data_VA[chan + page]);   // sotto 160 ci sono note on e off 
          }
          #endif
             
#if (shifter_active == 1)
          ledControl(chan, 1); // void ledControl (byte chann, byte stat)   // stat significa status 1 = acceso 0 = spento
          ledrestore(page); // perchè?? non è pesante?
          #endif


          if (data_MODE[chan] > 6) {
            bit_write(4, chan + page, true);   // r-groups // ricordare che: 4 = togletable
          } else if (data_MODE[chan] >= 2) {
            bit_write(4, chan + page, !bit_read(4, page + chan));
          }     

          if (velo == 0) {
            outnucleo(1, chan);
          } else {
         
            //  int velopush =  constrain(analogRead_1024(plexer),60,255);  //Serial.println (velopush);//  delay(2);
            int velopush = constrain(analogRead_1024(plexer), 160, lower);
            midiSendTyped(data_TY[chan + page], data_VA[chan + page], velo_pad_squeeze((byte)map32(velopush, 160, lower, 127, 1)), 1);
            //  Serial.println(velopush);
          }
          // outnucleo (1,chan);
        } else { /// se il pulsante è acceso nella toggletable
          if (data_MODE[chan] < 7 || data_MODE[chan] == 27) {
            #if defined (__AVR_ATmega32U4__)  
            HOT_keys(chan, 0);    
            #endif  
             
#if (shifter_active == 1)
            ledControl(chan, 0);
            ledrestore(page);
            #endif


            bit_write(4, chan + page, !bit_read(4, page + chan)); 
            outnucleo(0, chan);
          }
        }

        #if (shifter_active == 1)
        cycletimer = 0;   
        #endif
        shifterwrite = 1;
      }
      if (data_LB[chan] > 0) {
        data_LB[chan] = 0;
      }
    }

    // --------------------------------------------------------------------------
    #if (VELO_PAD_RELEASE_SCANS > 0)
    if (velo == 1 ? velo_pad_release_ready() : valore > upper_val) {
      if (velo == 1 || data_LB[chan] == 0) {
#else
    if (valore > upper_val) { ///// button released
      if (data_LB[chan] == 0) {
    #endif
        //  if (page == 0) 
        // Serial.println("---- released");
        //  Serial.println(valore);
        {
          if ((data_MODE[chan] >= 2 && data_MODE[chan] < 11) || (data_MODE[chan] == 27 && data_MA[chan] == 127)) { // toggle per i velo pads
            // se il pulsante = toggle o t-group o r-group
            if (data_MODE[chan] < 7 || data_MODE[chan] == 27) { // 7 8 9 10 sono RADIO group
              //                                                    
            }   
          } else {
            // if ( data_MODE[chan] < 5 ) // se il pulsante NON e' in toggle o in uno dei gruppi toggle
            {  
              //  if (bit_read(4,page+chan) == 1) 
              {  
                #if defined (__AVR_ATmega32U4__)  
                HOT_keys(chan, 0);
                #endif
            
#if (shifter_active == 1)
                if (bit_read(4, page + chan) == 0) {
                  ledControl(chan, 0);  
                }
                #endif


              }
              bit_write(4, chan + page, 0);
              outnucleo(0, chan);
            }
          }
        }

        shifterwrite = 1;
      }
      #if (VELO_PAD_RELEASE_SCANS > 0)
      if (velo != 1) // il filtro ha gia' aggiornato il contatore dei velocity pad
      #endif
      if (data_LB[chan] < data_LB_debounce) {
        data_LB[chan]++;
      }
    }
    //  data_LB[chan] = valore / 4;
  }  // PUSH BUTTON SECTION END
}
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTRL-F: STAGED_BUTTON_READ_TEST
// Sperimentazione: separazione della lettura pulsanti (LETTURA) dall'effetto.
// - NON tocca push_buttons() storica.
// - Compilata solo se STAGED_BUTTON_READ_TEST == 1 (vedi DART_config.h)
// - Usata SOLO nello slot MODE 34 (USER4) tramite ain_nucleo().
//
// Obiettivo: avere una logica di lettura riutilizzabile (pressed/released/down)
// senza portarsi dietro MIDI/DMX/LED/toggle ecc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (STAGED_BUTTON_READ_TEST == 1)

// Flag validi SOLO per il canale corrente ("consultabilita' locale").
static byte PB_DOWN_NOW     = 0;   // 1 = pulsante giu' (stato istantaneo, non latch)
static byte PB_PRESSED_NOW  = 0;   // 1 = fronte di pressione (debounced) in questo giro
static byte PB_RELEASED_NOW = 0;   // 1 = fronte di rilascio (debounced) in questo giro
static byte PB_VELO_NOW     = 0;   // 1..127 (solo se velo==1), 0 = non valido
// Accessor API (read-only):
// These functions expose the staged button state to ANY part of the sketch
// (including user functions), without exposing the internal variables.
byte pb_pressed()  { return PB_PRESSED_NOW; }
byte pb_released() { return PB_RELEASED_NOW; }
byte pb_down()     { return PB_DOWN_NOW; }
byte pb_velo()     { return PB_VELO_NOW; }


// ============================================================
// CTRL-F: PUSH_BUTTONS_LETTURA_STAGE
// Stage 1 - LETTURA: decide solo pressed/released/down.
// - Riusa lo stesso schema di debounce di push_buttons(): data_LB[]
// - NON manda MIDI/DMX/LED, NON tocca toggletable.
// ============================================================
void push_buttons_lettura_stage(byte velo) {

  // reset flags (validi solo per questo chan)
  PB_DOWN_NOW     = 0;
  PB_PRESSED_NOW  = 0;
  PB_RELEASED_NOW = 0;
  PB_VELO_NOW     = 0;

  // soglia "pressed" (stessa formula della push_buttons storica)
  int lower = lower_val + (velo * (data_MI[chan] * 3));

  // stato istantaneo (non debounced): utile per eventuali user-effects
  if (valore < lower) {
    PB_DOWN_NOW = 1;
  }

  // ------------------------------
  // PRESS edge (debounced)
  // ------------------------------
  if (valore < lower) {
    if (data_LB[chan] == data_LB_debounce) {
      PB_PRESSED_NOW = 1;

      // se e' un velo-pad, calcoliamo un valore indicativo (come nella push_buttons).
      // qui NON inviamo nulla: e' solo un numero pronto per chi vuole usarlo.
      if (velo == 1) {
        // Safety guard: keep legacy behaviour, but avoid invalid ranges.
        int lowerSafe = lower;
        if (lowerSafe <= 160) lowerSafe = 161;
        int velopush = constrain(analogRead_1024(plexer), 160, lowerSafe);
        PB_VELO_NOW = velo_pad_squeeze((byte)map32(velopush, 160, lowerSafe, 127, 1));
      }
    }
    // comportamento identico: se data_LB > 0 torna a 0 quando il pulsante e' giu'
    if (data_LB[chan] > 0) {
      data_LB[chan] = 0;
    }
  }

  // ------------------------------
  // RELEASE edge (debounced)
  // ------------------------------
  #if (VELO_PAD_RELEASE_SCANS > 0)
  if (velo == 1) {
    PB_RELEASED_NOW = velo_pad_release_ready();
    return;
  }
  #endif

  if (valore > upper_val) {
    if (data_LB[chan] == 0) {
      PB_RELEASED_NOW = 1;
    }
    // incrementa fino a debounce (identico alla storica)
    if (data_LB[chan] < data_LB_debounce) {
      data_LB[chan]++;
    }
  }
}


// ============================================================
// CTRL-F: PUSH_BUTTONS_EFFETTO_SERIAL
// Stage 2 - EFFETTO minimo (solo diagnostica su Serial).
// - Qui NON tocchiamo tabelle, LED, DMX, MIDI...
// - Serve solo per testare la bontà della lettura.
// ============================================================
/*
void push_buttons_effetto_serial(byte velo) {

  // niente da dire? uscita rapida (risparmio CPU)
  if (PB_PRESSED_NOW == 0 && PB_RELEASED_NOW == 0) return;

  // NB: su Leonardo/32U4 Serial e' di solito disponibile; se non e' aperto,
  // le print non fanno danni ma possono perdere tempo.
  if (PB_PRESSED_NOW) {
    Serial.print(F("[PB] PRESS  ch="));  Serial.print(chan);
    Serial.print(F(" page="));          Serial.print(page);
    Serial.print(F(" mode="));          Serial.print(data_MODE[chan]);
    Serial.print(F(" val="));           Serial.print(valore);
    if (velo == 1) {
      Serial.print(F(" velo="));        Serial.print(PB_VELO_NOW);
    }
    Serial.println();
  }

  if (PB_RELEASED_NOW) {
    Serial.print(F("[PB] RELEASE ch=")); Serial.print(chan);
    Serial.print(F(" page="));           Serial.print(page);
    Serial.print(F(" mode="));           Serial.print(data_MODE[chan]);
    Serial.print(F(" val="));            Serial.print(valore);
    Serial.println();
  }
}*/
  // =========================================================
  // STAGED PUSH BUTTON EFFECT (legacy behavior)
  // Uses flags from push_buttons_lettura_stage():
  //   PB_PRESSED_NOW / PB_RELEASED_NOW / PB_VELO_NOW
  // =========================================================
  void push_buttons_effetto_stage(byte velo) {
    // nothing happened -> exit quickly
    if (PB_PRESSED_NOW == 0 && PB_RELEASED_NOW == 0) return;

    // ----------------------------
    // PRESS effects
    // ----------------------------
    if (PB_PRESSED_NOW) {

        /*
        Serial.print(data_MODE[19]); Serial.println(" - data_MODE");
        Serial.print(data_DM[19]); Serial.println(" - data_DM - modalità endless / pot");
        Serial.print(data_QW[19]); Serial.println(" - touchstop");
        Serial.print(data_MI[19]-32); Serial.println(" - speed");
        */

        /*
        ============================================================
        CTRL-F: PAGESWITCH_DIAG_PRINT_ON_BUTTON  (DISABLED)
        Diagnostica takeover/pageswitch (contatori + dump arm bank5)
        usata solo per inchiodare il problema del takeover armato al boot.
        Ora rimossa/commentata.
        ============================================================

        for (byte i = 0; i < max_modifiers; i++) {
          Serial.println(bit_read(5, i + 0));
        }

        static byte ps_diag_printed = 0;
        if (!ps_diag_printed) {
          ps_diag_printed = 1;
          Serial.print(F("[PS] calls="));    Serial.print(ps_calls);
          Serial.print(F(" changes="));      Serial.print(ps_changes);
          Serial.print(F(" arm="));          Serial.println(ps_arm);
          Serial.print(F("[PS] first page="));      Serial.print(ps_first_page);
          Serial.print(F(" first pagestate="));     Serial.print(ps_first_pagestate);
          Serial.print(F(" first reason="));        Serial.println(ps_first_reason);
          Serial.print(F("[PS] setup page="));       Serial.print(ps_setup_page);
          Serial.print(F(" setup pagestate="));     Serial.println(ps_setup_pagestate);
        }
        */
        if (data_MODE[chan] >= 3 && data_MODE[chan] != 27) {
          offgroup(chan, 1);      // da 3 in poi ci sono i toggle groups e radio groups
        }
      
        if (bit_read(4, page + chan) == 0) { // 4 = toggletable // something happens only if the button is off in the toggletable
          #if (ENABLE_AUTODETECT == 1)
          if (eeprom_preset_active == 0) {
            data_DM[chan]++;    // autodetect_dmx
          }
          #endif
        
          //  Serial.println(mouse_mempos);
          #if defined (__AVR_ATmega32U4__)  
          HOT_keys(chan, 1);    
          #endif  
          
          #if (Scale == 1)
          if (data_TY[chan + page] < 160) {
            scale_learn(data_VA[chan + page]);   // sotto 160 ci sono note on e off 
          }
          #endif
             
#if (shifter_active == 1)
          ledControl(chan, 1); // void ledControl (byte chann, byte stat)   // stat significa status 1 = acceso 0 = spento
          ledrestore(page); // perchè?? non è pesante?
          #endif


          if (data_MODE[chan] > 6) {
            bit_write(4, chan + page, true);   // r-groups // ricordare che: 4 = togletable
          } else if (data_MODE[chan] >= 2) {
            bit_write(4, chan + page, !bit_read(4, page + chan));
          }     

          if (velo == 0) {
            outnucleo(1, chan);
          } else {
         
                        // velocity: reuse staged reading (PB_VELO_NOW) to match legacy mapping
            midiSendTyped(data_TY[chan + page], data_VA[chan + page], PB_VELO_NOW, 1);
//  Serial.println(velopush);
          }
          // outnucleo (1,chan);
        } else { /// se il pulsante è acceso nella toggletable
          if (data_MODE[chan] < 7 || data_MODE[chan] == 27) {
            #if defined (__AVR_ATmega32U4__)  
            HOT_keys(chan, 0);    
            #endif  
             
#if (shifter_active == 1)
            ledControl(chan, 0);
            ledrestore(page);
            #endif


            bit_write(4, chan + page, !bit_read(4, page + chan)); 
            outnucleo(0, chan);
          }
        }

        #if (shifter_active == 1)
        cycletimer = 0;   
        #endif
        shifterwrite = 1;
      
    }

    // ----------------------------
    // RELEASE effects
    // ----------------------------
    if (PB_RELEASED_NOW) {

        //  if (page == 0) 
        // Serial.println("---- released");
        //  Serial.println(valore);
        {
          if ((data_MODE[chan] >= 2 && data_MODE[chan] < 11) || (data_MODE[chan] == 27 && data_MA[chan] == 127)) { // toggle per i velo pads
            // se il pulsante = toggle o t-group o r-group
            if (data_MODE[chan] < 7 || data_MODE[chan] == 27) { // 7 8 9 10 sono RADIO group
              //                                                    
            }   
          } else {
            // if ( data_MODE[chan] < 5 ) // se il pulsante NON e' in toggle o in uno dei gruppi toggle
            {  
              //  if (bit_read(4,page+chan) == 1) 
              {  
                #if defined (__AVR_ATmega32U4__)  
                HOT_keys(chan, 0);
                #endif
            
#if (shifter_active == 1)
                if (bit_read(4, page + chan) == 0) {
                  ledControl(chan, 0);  
                }
                #endif


              }
              bit_write(4, chan + page, 0);
              outnucleo(0, chan);
            }
          }
        }

        shifterwrite = 1;
      
    }
  }


#endif // STAGED_BUTTON_READ_TEST

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ============================================================
// CTRL-F: CONVOY_PROTOS_POTS
byte convoy_find_subjectIndex(byte mem_chan);

void convoy_set(byte subjectIndex, byte value);
void convoy_commit();
// ============================================================

// CTRL-F: POT_TAKEOVER_WINDOW
// Finestra di aggancio in unità "raw" di pot_confronto (= abs(data_LB*4 - valore))
// 12 = circa 3 step (perché data_LB lavora a scaglioni da 4)
#define POT_TAKEOVER_WINDOW 24


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void pots() {
// CTRL-F: POT_DIFF_UNLOCK_WINDOW
// fuori finestra: scarto alto (anti-disturbo)
// dentro finestra: scarto basso (hi-precision)
#if (ENABLE_POT_HIPREC_WINDOW == 1)
const byte diff_unlock = 20;
const byte diff_window = 4;
#else
 const byte diff_unlock = 5; // quasi niente scarto se non usiamo finestra precision
#endif

#if (ENABLE_POT_HIPREC_WINDOW == 1)
  byte diff = (data_QW[chan] > 0) ? diff_window : diff_unlock;
#else
  byte diff = diff_unlock; // finestra OFF: sempre scarto grosso
#endif

  // CTRL-F: POTS_LB_INDEX
  // page vale 0 o 60 (max_modifiers). Con /60 otteniamo 0 oppure 1.
  // Usiamo 64 come stride per NON pestare i primi 0..63.
  #if (ENABLE_POT_TAKEOVER == 1)
  byte lb = chan + (byte)((page / max_modifiers) * 64);
#else
  byte lb = chan; // legacy: no split
  #endif

  /*
  if (data_QW[chan] > 0 // && data_MODE[chan] < 16
  ) diff = 3; // #mod_finestra
  */
  {
    int pot_confronto = abs((data_LB[lb] * 4) - valore);

    // CTRL-F: POTS_TAKEOVER_GATE
    byte allowWrite = 1; // 1=può scrivere, 0=quarantena takeover


    #if (ENABLE_POT_TAKEOVER == 1)
    // Special curves keep their physical cursor and bypass takeover.
    if ((data_MODE[chan] < 12 || data_MODE[chan] > 15) && bit_read(5, chan + page) == 1) { // ARMED su questa pagina
      if (pot_confronto <= POT_TAKEOVER_WINDOW) {
        // ============================================================
        // CTRL-F: TAKEOVER_SILENT_CATCH
        // CATCH SILENZIOSO:
         
        // qui avviene l'aggancio. In questa situazione NON vogliamo che
        // il pot "spari" subito il suo valore fisico (es. 0) sovrascrivendo
        // l'automazione (scattino iniziale).
        //
        // Quindi:
        // - disarmiamo (caught)
        // - sincronizziamo data_LB[lb] alla posizione fisica corrente
        // - blocchiamo l'invio per QUESTO giro (allowWrite=0)
        // ============================================================
        bit_write(5, chan + page, 0);  // CAUGHT: sblocca
        data_LB[lb] = valore / 4;   // sync base to physical now
        allowWrite = 0;                // non inviare nello stesso ciclo del catch
      } else {
        allowWrite = 0;                // ancora ARMED: non deve scrivere né inviare
      }
    }
    #endif // ENABLE_POT_TAKEOVER

    if (allowWrite && pot_confronto > diff) {
      // if ( pot_confronto > diff   ) // scaglioni da 4 - qundi 4 8 12 16 etc etc // #mod_finestra
      // the potentiometer has been moved
      {

        if (data_MODE[chan] == 37) { // il MODE relativo al qwerty pot sarà a parte ed esclusivo
          if (eeprom_preset_active != 0) {
            // Compare zones at the same four-step resolution as the saved cursor.
            // Exact thresholds belong to the centre. No extra persistent state.
            byte position = valore / 4;
            byte oldZone = (data_LB[lb] > upper_val / 4) ? 2 : ((data_LB[lb] < 31) ? 1 : 0);
            byte newZone = (position > upper_val / 4) ? 2 : ((position < 31) ? 1 : 0);
            if (oldZone != newZone) {
              byte key = (oldZone == 2) ? data_QW[chan] : data_MI[chan];
              if (oldZone && key) qwerty_out(0, key, 0);
              key = (newZone == 2) ? data_QW[chan] : data_MI[chan];
              if (newZone && key) qwerty_out(1, key, 0);
            }
            data_LB[lb] = position;
          }
        } else { // pot working in MIDI mode ---------------------------------------------------------------------------------------
          data_LB[lb] = valore / 4;

          ///  ----------------------------------------------------------
          if (data_MODE[chan] == 11) {
            if ((data_TY[chan + (page)]) < 224) {
              valore = map32(valore, 63, 960, data_MI[chan], data_MA[chan]);
            } // pot normale 
            // NOTA: il constrain da 0 a 127 viene fatto in seguito sulla variabile potout
          } else if (data_MODE[chan] == 12) { // CTRL-F: HYPERCURVE_INPUT_CLAMP_FIX
            valore = map32(constrain(valore, 63, 256), 63, 256, data_MI[chan], data_MA[chan]); // hypercurve 1 (clamp input)
#if (shifter_active == 1)
            shifter.setAll(LOW); // non ricordo a che serve questo spegnimento
            #endif
          } else if (data_MODE[chan] == 13) {
            valore = map32(constrain(valore, 768, 960), 768, 960, data_MI[chan], data_MA[chan]); // hypercurve 2 (clamp input)
#if (shifter_active == 1)
            shifter.setAll(LOW); 
            #endif
          } else if (data_MODE[chan] == 14) { // hypercurve center
            if (valore < 448) {
              valore = map32(valore, 63, 448, 0, 64);
            } else if (valore > 576) {
              valore = map32(valore, 576, 960, 64, 127);
            } else {
              valore = 64;
            }
          } else if (data_MODE[chan] == 15) { // hypercurve center 2 
            if (valore < 340) {
              valore = map32(valore, 63, 340, 0, 64);
            } else if (valore > 684) {
              valore = map32(valore, 684, 960, 64, 127);
            } else {
              valore = 64;
            }
          }

          ///  ----------------------------------------------------------
          //---------------------------------------------------------
          potOut = constrain(valore, 0, 127); // serve per inviare midi!!! - in realtà constrain è una sorta di misura di sicurezza - per non avere valori essessivi - ma perchè dovrebbero esserci??

          // CTRL-F: POTS_USER_RANGE_CENTER_OUT
          byte potSend = potOut;  // default: invio normale (0..127)

         if (data_MODE[chan] > 13) { // solo centercurve 14/15
  int a = data_MI[chan];
  int b = data_MA[chan];
  int lo = (a < b) ? a : b;
  int hi = (a < b) ? b : a;

  int v = map32(potOut, 0, 127, a, b);     // mantiene l'inversione se a>b
  potSend = (byte)constrain(v, lo, hi);    // chiusura sul range utente
}


          #if (shifter_active == 1)    
          encled[0] = abs(15 - ((potOut) / 8)) * 16; // spiegazione? boh --- mi pare di capire che con questa formula arrivo a una escursione 0 - 240 - ma quantizzata
          #endif

  
          ///  ----------------------------------------------------------
          switch ((data_TY[chan + (page)] - 144) / 16) { ////////// qui viene inviato il segnale midi definitivo
            case 0:
              midiSendFiltered(data_TY[chan + (page)] + 32, data_VA[chan + (page)], potSend, 1);
              break; // if (chan < 8) midiSendFiltered(176, chan,  valore/8, 0) ; break;// note
            case 1:
              midiSendFiltered(data_TY[chan + (page)], data_VA[chan + (page)], potSend, 1);
              break; // poly AT
            case 2:
              midiSendFiltered(data_TY[chan + (page)], data_VA[chan + (page)], potSend, 1);
              // Serial.println(encled);
              break; // cc
            case 4:
              midiSendFiltered(data_TY[chan + (page)], potSend, 0, 1);
              break; // channel AT
            case 5: {
              valore = constrain(map32(valore, 24, 1000, 0, 1024), 0, 1023); // PB - pitch bend e la preparazione encled per l'effetto visivo
              // valore = constrain(map32(valore,24,1000, 0,1024),0,1023);
              midiSendFiltered(data_TY[chan + (page)], (valore - ((valore / 8) * 8)) * 16, valore / 8, 1);
              encled[0] = abs(15 - ((valore) / 64)) * 16;
            }
            break; // PB
  // 2348 
  #if (case_6 == 1 )
      #endif
          }

          ///  ----------------------------------------------------------
          cycletimer = 0; // effetti led
        
#if (shifter_active == 1)
          if (data_LT[chan] > 32) {
            led_enc_exe();
          }
      
          #if (blinker == 1)
          else {
            if (data_LT[chan] > 0) { // 0= no efetti - 1=effetti - 2=blinker
              // CTRL-F: BLINKER_OFF_IS_RANGE_MIN
              // Blinker "stile Traktor": per POT/HYPER (mode 11/12/13) è acceso quando il valore NON è sul minimo del range scelto.
              // Nota: il minimo del range è min(data_MI, data_MA). Questo funziona anche se l'utente inverte (min > max).
              byte blinkOff = (byte)((data_MI[chan] < data_MA[chan]) ? data_MI[chan] : data_MA[chan]);
              if (((potOut != blinkOff) && (data_MODE[chan] < 14)) || ((potOut != 64) && (data_MODE[chan] > 13))) { // hypercurve o normal / centercurve
                bit_write(1, (data_LT[chan] - 1) + page, 1);  
              } else {     
                bit_write(1, (data_LT[chan] - 1) + page, 0);
              }
            }
          }
          #endif // (blinker == 1)
       
          #endif  
       

#if (DMX_active == 1)
          // if (data_DM[chan] == 100) 
          if (eeprom_preset_active == 1) {
             
             // DMX a 8-bit vero: usa il valore raw (0..1023) e lo porta a 0..255
byte dmxOut = (byte)(valore >> 2);   // 1023 >> 2 = 255
DmxSimple.write(data_DM[chan], dmxOut);
         //   DmxSimple.write(data_DM[chan], potOut * 2);
         
          }
          // Serial.println("a");
          #endif

          // CTRL-F: WINDOW_REFRESH_ONLY_ON_BIG_MOVE
// Ricarica la finestra SOLO su movimento "grosso".
// Evita che un jitter > diff_window (3) tenga la finestra aperta per secondi.
#if (ENABLE_POT_HIPREC_WINDOW == 1)
if (pot_confronto > diff_unlock) {
  data_QW[chan] = 160; // #mod_finestra
}  
#endif
        }  // -------------- fine della condizione generale pot mosso
      }
 
      // {if (data_VA[chan + page] == 63) digitalWrite(8,HIGH);} // sperimentale - volevo vedere con un led la finestra temporale che si apre
    }
       
   // CTRL-F: POTS_WINDOW_CLOSE_USES_UNLOCK
// La finestra NON deve rimanere aperta per colpa di micro-instabilità.
// Quindi la chiusura usa il "metro grosso" (diff_unlock), non il 7 fisso.
#if (ENABLE_POT_HIPREC_WINDOW == 1)

if (data_QW[chan] > 0 && (data_MODE[chan] < 16 || data_MODE[chan] == 38)) {
 // if (pot_confronto <= diff_unlock) {     // <-- criterio di "fermo" grossolano
     if (pot_confronto <= diff_window) {   // <-- criterio di "fermo" fine  // CTRL-F: POTS_WINDOW_CLOSE_SMART 
                                           // Se la finestra è aperta e stiamo ancora "muovendo piano" (oltre diff_window), 
                                           // NON la facciamo scendere: così i movimenti lentissimi restano precisi.
    data_QW[chan]--;
  }
}
#endif

    #if (blinker == 1) 
    {
#if (shifter_active == 1)
      // Guard: LED 0 means no LED; values >32 are visual effects, not shifter outputs.
      if (data_LT[chan] > 0 && data_LT[chan] < 33 &&
          bit_read(1, (data_LT[chan] - 1) + page) == 1) { // led = acceso nel banco di memoria
        if (data_TY[general_mempos] == 0) {
          shifter.setPin((data_LT[chan] - 1), 0);
          shifterwrite = 1;
        } // a zero spegni led
        if (data_TY[general_mempos] == 40) {
          shifter.setPin((data_LT[chan] - 1), 1);
          shifterwrite = 1;
        } // a 40 accendi led
      }
      if (data_TY[general_mempos] == 80) {
        data_TY[general_mempos] = 0; // a 80 resetta il counter led lampeggiante
      }
      #endif
    }

    #endif //(blinker ==1)
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void user_item1() {
  #if (hid_keys == 1) 
#if (STAGED_BUTTON_READ_TEST == 1)

  push_buttons_lettura_stage(0);

  byte action = data_VA[chan + page];

  if (pb_pressed()) {

    switch (action) {

      case 0:
        // MIDI NOTE ON
        // data_MI = note number
        // data_MA = velocity
        // data_DM = MIDI channel, 1-16
        {
          byte midi_channel = constrain(data_DM[chan], 1, 16);
          midiSendRaw(143 + midi_channel, data_MI[chan], data_MA[chan]);
        }
        break;

      case 1:
        // HID: letter A
        Keyboard.press('a');
        Keyboard.release('a');
        break;

      case 2:
        // HID: letter B
        Keyboard.press('b');
        Keyboard.release('b');
        break;

      case 3:
        // HID: CTRL + S
        Keyboard.press(128);
        Keyboard.press('s');
        Keyboard.releaseAll();
        break;

      case 4:
        // HID: CTRL + C
        Keyboard.press(128);
        Keyboard.press('c');
        Keyboard.releaseAll();
        break;

      case 5:
        // HID: CTRL + V
        Keyboard.press(128);
        Keyboard.press('v');
        Keyboard.releaseAll();
        break;

      case 6:
        // HID: CTRL + Z
        Keyboard.press(128);
        Keyboard.press('z');
        Keyboard.releaseAll();
        break;

      case 7:
        // HID: ESC
        Keyboard.press(177);
        Keyboard.release(177);
        break;
    }
  }

  if (pb_released()) {

    switch (action) {

      case 0:
        // MIDI NOTE OFF
        {
          byte midi_channel = constrain(data_DM[chan], 1, 16);
          midiSendRaw(127 + midi_channel, data_MI[chan], 0);
        }
        break;
    }
  }
#endif
#endif
}

void user_item2() {
  // vuota
}

void user_item3() {
  // vuota
}

void user_item4() {
  // vuota
}

//-----------------------------------------------------
 
void reset() {
  // ENC.RESET changes a generic CC encoder position, not a button or spinner.
  // Consume the press first so an invalid target cannot retrigger while held.
#if (STAGED_BUTTON_READ_TEST == 1)
  push_buttons_lettura_stage(0);
  if (!pb_pressed()) return;
#else
  if (valore > upper_val && data_LB[chan] == 0) data_LB[chan] = 1;
  if (valore >= lower_val || data_LB[chan] != 1) return;
  data_LB[chan] = 0;
#endif

  // Zero is unassigned. Check before remapping/subtracting or indexing arrays.
  byte target = data_MI[chan];
  if (target == 0 || target > max_modifiers) return;
  target = remapper(target - 1);
  if (target >= max_modifiers || mouse_wheel_owns(target) || data_MODE[target] != 19 ||
      data_DM[target] < 2 || data_DM[target] > 4) return;
  // Keep old SPIN=4 presets working as RAMP; the editor now saves RAMP as 3.
  const byte type = data_TY[target + page];
  if ((type & 0xF0) != 0xB0) return;

  midiSendTyped(type, data_VA[target + page], data_MA[chan], 1);
  data_LT[target] = data_MA[chan] * 2;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void offset_modifier() {
  // MODE 30: temporary MIDI-channel shift; no new persistent state.
  byte on;
#if (STAGED_BUTTON_READ_TEST == 1)
  push_buttons_lettura_stage(0);
  if (pb_pressed()) on = 1;
  else if (pb_released()) on = 0;
  else return;
#else
  if (valore < lower_val && data_LB[chan] > 0) {
    data_LB[chan] = 0;
    on = 1;
  } else if (valore > upper_val && data_LB[chan] == 0) {
    data_LB[chan] = 1;
    on = 0;
  } else return;
#endif

  byte target = data_LT[chan];
  byte amount = data_DM[chan];
  if (target == 0) {
    offset_modifier_ = on ? amount : 0;
    return;
  }
  // Only the 56 physical input positions are valid targeted destinations.
  // Invalid legacy presets must not address the other page or adjacent RAM.
  if (target > 56) return;
  target = remapper(target - 1);
   
  target += page;
  byte status = data_TY[target];
  if (status >= 0xF0) return;  
  if (!on) amount = -amount;
  data_TY[target] = (status & 0xF0) | ((status + amount) & 0x0F);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void virtual_touch_end(byte numero) {
  if (!(control_present & (numero ? CONTROL_TOUCH2 : CONTROL_TOUCH1))) return;
  byte t = touch_mempos[numero];
  // Inactive/physical touches must not consume another touch's decay threshold.
  // The shared scan clock is retained: no new timer or persistent state.
  if (V_touch_regulator[numero] != 0 || data_LT[t] != 1 ||
      cycletimer < data_MI[t]) return;

  midiSendRaw(data_TY[t + page] + 1, data_VA[t + page], 0);
  V_touch_regulator[numero] = 1;
  touch_led_update(numero, 0);

  byte s = spinner_mempos[numero];
  byte type = data_TY[s + page];
  byte group = type & 0xF0;
  if (data_QW[t] == 1 && (control_present & (numero ? CONTROL_SPINNER2 : CONTROL_SPINNER1)) &&
      !mouse_wheel_owns(s) && data_DM[s] == 2 && (group == 176 || group == 224)) {
    byte value = data_DM[t];
    encodervaluepot[numero] = (int)value * 8;
    // PB's centre is exactly 8192: LSB=0, MSB=64, not 64/64.
    midiSendFiltered(type, group == 224 ? 0 : data_VA[s + page], value, 0);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
 
void restore_end() {
  if (cycletimer == 66) {
#if (shifter_active == 1)
    ledrestore(page);
    shifterwrite = 1;
    #endif

    
    cycletimer = 67;
  }
}

//---------------------------------------------------------------------------------------------------------

void HOT_keys(byte canale, byte pressione) {
  // se un modificatore e' impostato su messaggio qwerty (>0), il valore di data_MI 
  // (non utile per il note off) viene utilizzato per selezionare i modificatori qwerty 

  #if defined (__AVR_ATmega32U4__)  
  // Serial.println(data_MI[canale]);

  #if (hid_keys == 1)
  // MODE 27 uses data_MI as sensitivity, never as HID modifiers.
  // Keep the shared bytes intact for preset transport and their other uses.
  if (data_MODE[canale] == 27 || data_QW[canale] == 0) return;
  if (data_MI[canale] > 0) { 
    if (pressione == 1) {
      switch (data_MI[canale]) {
        case 1:
          Keyboard.press(128); // CTRL left
          break;
        case 2:
          Keyboard.press(129); // shift left 
          break;
        case 3: 
          Keyboard.press(130); // alt left 
          break;
        case 4: 
          Keyboard.press(128);
          Keyboard.press(129); // ctrl+shift left 
          break;
        case 5:
          Keyboard.press(128);
          Keyboard.press(130); // ctrl+alt left 
          break;
        case 6:
          Keyboard.press(128);
          Keyboard.press(129);
          Keyboard.press(130); // ctrl+alt left 
          break;
      }
    } else {
      switch (data_MI[canale]) {
        case 1:
          Keyboard.release(128); // CTRL left
          break;
        case 2:
          Keyboard.release(129); // shift left 
          break;
        case 3:
          Keyboard.release(130); // alt left 
          break;
        case 4: 
          Keyboard.release(128);
          Keyboard.release(129); // ctrl+shift left 
          break;
        case 5:
          Keyboard.release(128);
          Keyboard.release(130); // ctrl+alt left 
          break;
        case 6:
          Keyboard.release(128);
          Keyboard.release(129);
          Keyboard.release(130); // ctrl+alt left 
          break;
      }
    }
  }
  #endif
  #endif
}

void pageswitch() { //----------------------------------------------------- PAGE SWITCH
  /*
  // ============================================================
  // CTRL-F: PAGESWITCH_DIAG_HOOK_TOP  (DISABLED)
  // Diagnostica usata per capire perché il takeover veniva armato al boot.
  // ============================================================
  ps_calls++;
  if (ps_first_page == 255) { ps_first_page = page; ps_first_pagestate = pagestate; }
  */

  // 2 casi:
  // A) pagestate==0 e page>0  -> torna alla page 0   (midiSendRaw con MIN)
  // B) pagestate==1 e page==0 -> va a max_modifiers (midiSendRaw con MAX)
  //
  // Tutto il resto è IDENTICO nei due rami, quindi lo condividiamo
  // per spremere flash.

  byte targetPage;
  byte outValue;   // data_MI o data_MA del page_mempos
  byte ledsMode;   // 0 oppure 1 (per page_leds_)

  if (pagestate == 0 && page > 0) {
    targetPage = 0;
    outValue = data_MI[page_mempos];
    ledsMode = 0;
  } else if (pagestate == 1 && page == 0) {
    targetPage = max_modifiers;
    outValue = data_MA[page_mempos];
    ledsMode = 1;
  } else {
    /*
    // CTRL-F: PAGESWITCH_DIAG_NOCHANGE  (DISABLED)
    if (ps_first_reason == 255) ps_first_reason = 0;
    */
    return; // nessun cambio pagina richiesto
  }

  // Applica target
  byte oldPage = page;
  page = targetPage;

  /*
  // ============================================================
  // CTRL-F: PAGESWITCH_DIAG_CHANGE  (DISABLED)
  // ============================================================
  ps_changes++;
  if (ps_first_reason == 255) {
    if (page == max_modifiers) ps_first_reason = 1;
    else if (page == 0)        ps_first_reason = 2;
    else                       ps_first_reason = 0;
  }
  */


  // CTRL-F: TAKEOVER_INIT_PAGE2_ON_FIRST_ENTRY
  #if (ENABLE_POT_TAKEOVER == 1)
  // Prima entrata in Page2: inizializza i target data_LB della Page2 copiando Page1
  // (evita target=0 irraggiungibili che bloccherebbero il takeover)
  if (page == max_modifiers) {
    if ((potPageInitMask & 2) == 0) {
      for (byte i = 0; i < max_modifiers; i++) {
        data_LB[64 + i] = data_LB[i];
      }
      potPageInitMask |= 2;
    }
  }
  #endif
  
  // CTRL-F: TAKEOVER_ARM_ON_PAGESWITCH
  /*
  // CTRL-F: PAGESWITCH_DIAG_ARM  (DISABLED)
  ps_arm++;
  */
  #if (ENABLE_POT_TAKEOVER == 1)
  // Appena entri in una pagina: ARMED su tutti i pot (chan 0..59) di quella pagina.
  // pots() li sbloccherà (CAUGHT) solo quando agganciano il loro target.
  for (byte i = 0; i < max_modifiers; i++) {
    bit_write(5, i + page, 1);
  }
  #endif

  // Reset "modifiers" (come nel tuo)
  offset_modifier_ = 0;

  // IMPORTANT DESIGN NOTE (la tua): page switch = full state reload
  reset_mempos();       // come in chiusura editor (241)
  load_preset_base();   // riallinea data_TY/data_VA ecc (base)
  load_preset(page);    // poi carica la pagina corrente

  #if (Scale == 1)
  // Riallinea scale spinner (come nel tuo)
  update_scala(1); // secondo spinner
  update_scala(0); // primo spinner
  #endif

   
#if (shifter_active == 1)
  shifter.setAll(LOW);
  shifterwrite = 1;
  ledrestore(page);
  #endif

  // Restore matrix (se presente)

  // Notifica pagina (come nel tuo: data_TY/data_VA + min/data_MA)
  if (control_present & CONTROL_PAGE) {
    midiSendRaw(data_TY[page_mempos], data_VA[page_mempos], outValue);
  }

  // LED pagina (0 oppure 1)
  page_leds_(ledsMode);

  // Flag (come nel tuo)
  shifterwrite = 1;

  // Reset Xen (come nel tuo)
  higher_Xen[0] = 40;
  higher_Xen[1] = 40;
  lower_Xen[0] = 100;
  lower_Xen[1] = 100;
}

void page_leds_(byte pagina) {
  #if (page_LEDs == 1)
  if (!(control_present & CONTROL_PAGE)) return;
  #endif
  if (pagina == 0) { 
    #if (page_LEDs == 1) // indicatori led dedicati al page switch
    if (data_VA[general_mempos] == 0 && data_LT[page_mempos] > 0) { // nomobo setup disattivo 
      #if (shifter_active == 1)
      shifter.setPin((data_DM[page_mempos] - 1), 1); 
      #endif
      bit_write(1, (data_DM[page_mempos] - 1) + page, 1);
      
      #if (shifter_active == 1)
      shifter.setPin((data_LT[page_mempos] - 1), 0); 
      #endif
      bit_write(1, (data_LT[page_mempos] - 1) + page, 0);
    } else {
      // shifter.setPin((data_MI[page_mempos]-1), 1); 
      digitalWrite(data_DM[page_mempos] - 1, 1);
      bit_write(1, (data_DM[page_mempos] - 1) + page, 1);
      // shifter.setPin((data_MA[page_mempos]-1), 0); 
      digitalWrite(data_LT[page_mempos] - 1, 0);
      bit_write(1, (data_LT[page_mempos] - 1) + page, 0);
    }
    #endif
  } else {
    #if (page_LEDs == 1)
    if (data_VA[general_mempos] == 0 && data_LT[page_mempos] > 0) {
      #if (shifter_active == 1)
      shifter.setPin((data_DM[page_mempos] - 1), 0); 
      #endif
      bit_write(1, (data_DM[page_mempos] - 1) + page, 0);
      
      #if (shifter_active == 1)
      shifter.setPin((data_LT[page_mempos] - 1), 1); 
      #endif
      bit_write(1, (data_LT[page_mempos] - 1) + page, 1);
    } else {
      // shifter.setPin((data_MI[page_mempos]-1), 1); 
      digitalWrite(data_DM[page_mempos] - 1, 0);
      bit_write(1, (data_DM[page_mempos] - 1) + page, 0);
      // shifter.setPin((data_MA[page_mempos]-1), 0); 
      digitalWrite(data_LT[page_mempos] - 1, 1);
      bit_write(1, (data_LT[page_mempos] - 1) + page, 1);
    }
    #endif
  }
}
 
  
