/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */

// MIDI IN

byte matrix_vert1;
byte matrix_vert2;
  
#if (USE_REFACTORED_MIDIFEEDBACK == 0)
// ============================================================
// CTRL-F: POT_MIDIFEEDBACK_TAKEOVER
// Pot MIDI feedback -> aggiorna il TARGET software del pot (data_LB per pagina)
// e ARMA il takeover (bit_status bank 5), così il pot fisico non sovrascrive subito.
//
 
// Minimale: conversione value(0..127) -> raw target approx (63..960).
// ============================================================
static inline void pot_midifeedback_takeover_update(byte inType, byte inCC, byte inValue) {
  #if (ENABLE_POT_TAKEOVER == 1)
  // Solo Control Change (176..191). Program Change puro non arriva ancora qui.
  if (inType < 176 || inType > 191) return;

  // Cerchiamo match su entrambe le pagine: pageOffset = 0 (P1) e max_modifiers (P2)
  for (byte pageOffset = 0; pageOffset <= max_modifiers; pageOffset += max_modifiers) {
    for (byte c = 0; c < max_modifiers; c++) {
      byte mode = data_MODE[c];
      if (mode != 11) continue; // special curves do not use takeover

      byte t = data_TY[c + pageOffset];            // tipo configurato in quella pagina
       
      if (t >= 192 && t <= 207) t = t - 16;

      if (t != inType) continue;                     // canale MIDI deve combaciare
      if (data_VA[c + pageOffset] != inCC) continue; // CC number deve combaciare

      // Aggiorna il target software (data_LB per pagina) in dominio 'raw' (0..1023 circa)
      int rawTarget = map32((int)inValue, 0, 127, 63, 960);
      byte lb = c + (byte)((pageOffset / max_modifiers) * 64);
      data_LB[lb] = (byte)(rawTarget >> 2);       // /4, coerente con pots()

      // Arma takeover: finché il pot fisico non aggancia, non scrive né invia.
      bit_write(5, c + pageOffset, 1);
    }
  }
  #endif
}

#endif // legacy takeover scan

#if (USE_REFACTORED_MIDIFEEDBACK == 0)
// per midi feedback veloce dedicato a leds (35) o dmx (36)
#if (FAST_FEEDBACK == 1)
// CTRL-F: FAST_FEEDBACK_HELPER 
static inline void fast_feedback_apply(byte ch, byte vel)
{
  // --- LED ONLY (35) ---
  if (data_MODE[ch] == 35) {
    if (data_LT[ch] == 0) return;

    byte on = (vel != 0);

    if (data_VA[general_mempos] != 0) {
#if (shifter_active == 1)
      ledControl(ch, on);
      #endif
    } else {
#if (shifter_active == 1)
      shifter.setPin(data_LT[ch] - 1, on);
      shifterwrite = 1;
      #endif
    }
    return;
  }

  // --- DMX ONLY (36) ---
  if (data_MODE[ch] == 36) {
#if (DMX_active == 1)
    // stessa scala che usi già: velocity * 2
    byte out = (vel == 0) ? 0 : (byte)(vel * 2);
    DmxSimple.write(data_DM[ch], out);
    #endif
    return;
  }
}
#endif // FAST_FEEDBACK

#endif // legacy fast helper

#if (USE_REFACTORED_MIDIFEEDBACK == 1)

// ============================================================
// CTRL-F: MIDIFEEDBACK_REFACTORED_CORE
// One complete MIDI message -> one dispatcher -> one scan per enabled page.
// No temporary TYPE/VALUE match bit-banks are used by this implementation.
// ============================================================

static byte midi_feedback_mode_for_page(byte ch, byte pageOffset)
{
  #if (ENABLE_DUAL_PAGE_FEEDBACK == 1)
  if (pageOffset != page) return data_MODE_OTHER[ch];
  #endif
  return data_MODE[ch];
}

static void midi_feedback_store_visual(byte ch, byte pageOffset, byte on)
{
  if (data_LT[ch] == 0) return;

    bit_write(1, (data_LT[ch] - 1) + pageOffset, on);
}

static void midi_feedback_draw_active(byte ch, byte on)
{
  if (data_LT[ch] == 0) return;

  if (data_VA[general_mempos] != 0) { // NOMOBO
#if (shifter_active == 1)
    ledControl(ch, on);
    #endif
  } else {
#if (shifter_active == 1)
    shifter.setPin(data_LT[ch] - 1, on);
    shifterwrite = 1;
    #endif


  }
}

// Page-specific group clearing. Unlike legacy offgroup(), it never mixes the
// toggle state of PAGE 1 and PAGE 2 while updating MIDI feedback.
static void midi_feedback_clear_group(byte ch, byte pageOffset, byte mode, byte activePage)
{
  for (byte i = 0; i < 56; i++) {
    if (i == ch) continue;
    if (midi_feedback_mode_for_page(i, pageOffset) != mode) continue;
    if (bit_read(4, i + pageOffset) == 0) continue;

    bit_write(4, i + pageOffset, 0);
    midi_feedback_store_visual(i, pageOffset, 0);
    if (activePage) midi_feedback_draw_active(i, 0);
#if (DMX_active == 1)
    // Only the loaded page owns data_DM/data_MI. Match its ordinary OFF value.
    if (activePage && data_DM[i] > 0) DmxSimple.write(data_DM[i], data_MI[i] * 2);
    #endif
  }
}

static void midi_feedback_takeover_matched(byte ch, byte pageOffset, byte mode,
                                           byte inType, byte inValue)
{
  #if (ENABLE_POT_TAKEOVER == 1)
  if (mode != 11) return; // special curves do not use takeover
  if (inType < 176 || inType > 191) return;

  int rawTarget = map32((int)inValue, 0, 127, 63, 960);
  byte lb = ch + ((pageOffset == 0) ? 0 : 64);
  data_LB[lb] = (byte)(rawTarget >> 2);
  bit_write(5, ch + pageOffset, 1);
#else
  (void)ch; (void)pageOffset; (void)mode; (void)inType; (void)inValue;
  #endif
}

#if (FAST_FEEDBACK == 1)
static void midi_feedback_fast_active(byte ch, byte mode, byte vel)
{
  if (mode == 35) {
    if (data_LT[ch] == 0) return;
    byte on = (vel != 0);

    if (data_VA[general_mempos] != 0) {
#if (shifter_active == 1)
      ledControl(ch, on);
      #endif
    } else {
#if (shifter_active == 1)
      shifter.setPin(data_LT[ch] - 1, on);
      shifterwrite = 1;
      #endif
    }
    return;
  }

  if (mode == 36) {
#if (DMX_active == 1)
    DmxSimple.write(data_DM[ch], (vel == 0) ? 0 : (byte)(vel * 2));
    #endif
  }
}
#endif

static void midi_feedback_apply_match(byte ch, byte pageOffset, byte mode,
                                      byte vel, byte activePage, byte inType)
{
  byte on = (vel != 0);

  if (mode >= 2 && mode < 11) {
    if (inType < 144) on = 0; // An explicit Note Off always wins over MAX=0.
    else {
      byte minimum = data_MI[ch];
      byte maximum = data_MA[ch];
      #if (ENABLE_DUAL_PAGE_FEEDBACK == 1 && ENABLE_EEPROM == 1)
      if (!activePage) {
        // Read only a matched hidden-page toggle; no extra parameter cache.
        int base = ch + (pageOffset ? 512 : 0);
        minimum = EEPROM.read(base + 256);
        maximum = EEPROM.read(base + 320);
      }
      #endif
      if (vel == minimum) on = 0; // Equal endpoints are deterministically OFF.
      else if (vel == maximum) on = 1;
      // Other values keep the existing zero/nonzero feedback convention.
    }
  }

  #if (FAST_FEEDBACK == 1)
  if (mode == 35 || mode == 36) {
    // data_LT/data_DM exist only for the loaded page: physical fast feedback is
    // therefore intentionally limited to the active page.
    if (activePage) midi_feedback_fast_active(ch, mode, vel);
    return;
  }
  #endif

  // data_DM[] belongs to the active page, so never use it for hidden-page data.
  if (activePage && mode < 16) {
#if (DMX_active == 1)
    DmxSimple.write(data_DM[ch], (byte)(vel * 2));
    #endif
  }

  if (mode < 11 || mode == 27) { // buttons + velo-pads
    if (on && mode >= 3 && mode < 11) {
      midi_feedback_clear_group(ch, pageOffset, mode, activePage);
    }

    midi_feedback_store_visual(ch, pageOffset, on);
    if (on) old_pitch = 126;
    if (activePage) midi_feedback_draw_active(ch, on);

    if (mode >= 1 && mode < 11) {
      bit_write(4, ch + pageOffset, on);
    }
  }
}

static void midi_feedback_scan_page(byte inType, byte matchType,
                                    byte inNote, byte inValue,
                                    byte pageOffset)
{
  byte activePage = (pageOffset == page);

  for (byte ch = 0; ch < max_modifiers; ch++) {
    byte item = ch + pageOffset;
    if (data_VA[item] != inNote) continue;

    byte mode = midi_feedback_mode_for_page(ch, pageOffset);
    byte configuredType = data_TY[item];

    // Integrate takeover into this same scan. PC values 192..207 may represent
     
    #if (ENABLE_POT_TAKEOVER == 1)
    byte takeoverType = configuredType;
    if (takeoverType >= 192 && takeoverType <= 207) takeoverType -= 16;
    if (takeoverType == inType) {
      midi_feedback_takeover_matched(ch, pageOffset, mode, inType, inValue);
    }
    #endif

    // General feedback keeps the legacy direct TYPE match. Note Off has already
    // been normalized to the corresponding Note On in matchType.
    if (configuredType != matchType) continue;
    midi_feedback_apply_match(ch, pageOffset, mode, inValue, activePage, inType);
  }
}

static void midi_feedback_process(byte inType, byte inNote, byte inValue)
{
  byte matchType = (inType < 144) ? (byte)(inType + 16) : inType;

  if (inValue != 0) {
    #if (Scale == 1)
    if (inType < 160) scale_learn(inNote);
    #endif

    #if (MIDI_thru == 1)
    if (inType < 160) midiSendFiltered(inType, inNote, 127, 0);
    else midiSendFiltered(inType, inNote, inValue, 0);
    #endif
  } else {
    #if (MIDI_thru == 1)
    midiSendFiltered(inType, inNote, 0, 0);
    #endif
  }

  #if (ENABLE_DUAL_PAGE_FEEDBACK == 1)
  midi_feedback_scan_page(inType, matchType, inNote, inValue, 0);
  midi_feedback_scan_page(inType, matchType, inNote, inValue, max_modifiers);
#else
  midi_feedback_scan_page(inType, matchType, inNote, inValue, page);
  #endif
}

#endif // USE_REFACTORED_MIDIFEEDBACK


void midifeedback() {
  {
    // read the incoming byte:
    if (incomingByte == 241) {  
      #if (ENABLE_EEPROM == 0)
      // EEPROM disabilitata: ignoriamo il toggle upload preset (241)
      // Così openeditor resta sempre 0 e il controller lavora in AUTODETECT.
      return;
      #endif
// this message opens the editor upload mode
      openeditor = !openeditor; // apri o chiudi la ricezione del preset
      if (openeditor == 1) { // se entri in modalità ricezione porta a zero il counter
        editorcounter = 0;
      } else {    
        // ============================================================
        // CTRL-F: EEPROM_EMU_COMMIT_ON_UPLOAD_CLOSE
        // Fine upload preset (241 di chiusura): su SAMD scriviamo la mirror RAM su flash UNA SOLA VOLTA.
        // Su AVR la EEPROM è reale: nessuna commit necessaria.
        // ============================================================
        #if (ENABLE_EEPROM == 1) && defined(ARDUINO_ARCH_SAMD)
          EEPROM.commit();
        #endif
        reset_mempos();       
        load_preset_base();
        // load_preset(0);
        load_preset(page);        // load preset from eerom memory after upload   

        #if (Scale == 1) 
        update_scala(1);  
        update_scala(0); 
        #endif
       
        if (control_present & CONTROL_SPINNER1) data_LB[spinner_mempos[0]] = 0; // 
        for(int led = 0; led < 8; led++) { // reset di tutti i led e tutti i banchi toggle   
          // bit_status[4][led]=0; bit_status[5][led]=0;
          bit_status[0][led] = 0;
          bit_status[1][led] = 0;   
          bit_status[2][led] = 0;
          bit_status[3][led] = 0; 
        } // all leds and toggles off

        
#if (shifter_active == 1)
        shifter.setAll(0);
        shifterwrite = 0; // spegni fisicamente tutti i leds
        #endif
        

#if (Page_switch == 1)
        page_leds_(page);
        #endif
      }
    } else /////////////////////////// se il messaggio non è un 241
    if (incomingByte >= 128 && incomingByte <= 191 || incomingByte == 224) { // a partire da note off fino a control change 
                                                                             // il resto dei messaggio non viene ricevuto
                                                                             // 224 fa eccezione - è un messaggio pithbend che serve per gli sprites
      action = 1;
      type = incomingByte;
      #if (USE_REFACTORED_MIDIFEEDBACK == 0)
      if (openeditor == 0) {
        byte note_off_case; // Note Off behaves as Note On velocity zero.
        if (incomingByte < 144) note_off_case = 16;
        else note_off_case = 0;

        for (byte ledA = 0; ledA < max_modifiers * 2; ledA++) {
          bit_write(2, ledA, (type + note_off_case == data_TY[ledA]));
        }
      }
      #endif
    }
    //-------------------------------------------------------------------------------------------
    else if ((action == 0) && (note == 255)) { // do nothing
      action = 2;
    }
    //-------------------------------------------------------------------------------------------
    else if (action == 1 && note == 255) { //  wait for the note (databyte)
      note = incomingByte;
      #if (USE_REFACTORED_MIDIFEEDBACK == 0)
      if (openeditor == 0) {
        for (byte ledA = 0; ledA < max_modifiers * 2; ledA++) {
          if (data_VA[ledA] == note) bit_write(3, ledA, bit_read(2, ledA));
        }
      }
      #endif
    }
    //-------------------------------------------------------------------------------------------
    else if ((action == 1) && (note != 255)) { // ...and then the velocity
      velocity = incomingByte;
      if (type < 144) {
        velocity = 0;
      }
      //-------------------------------------------------------------------------------------------
      if (openeditor == 1) {
        if (type < 208) { // fino a tutti i control-change la triade midi viene mandata alla procedura  eeprom_write()
                          // arrivati qui abbiamo ricevuto un messaggio midi completo e vado a caricare i dati sulla eeprom 
                          // se arriva un 224 invece...
          #if (ENABLE_EEPROM == 1)
          eeprom_write(); // scrittura su EEPROM (upload preset)
          #endif

        } else {  // basterà mandare un segnale pitch-bend // il contenuto dei due databyte forma il byte verticale per completare la matrice. // 224
          matrix_vert1 = note;
          matrix_vert2 = velocity;
        }
      }
      //------------------------------------------------------------------------------------------------------------------------------------------------
      else {  // normali procedure di feedback midi // se openeditor == 0

        #if (USE_REFACTORED_MIDIFEEDBACK == 1)
        midi_feedback_process(type, note, velocity);
#else
        // CTRL-F: POT_MIDIFEEDBACK_CALL
        pot_midifeedback_takeover_update(type, note, velocity);

        if (velocity != 0) { // ricevuto segnale ACCESO
          #if (Scale == 1)
          if (type < 160) {
            scale_learn(note); 
          }
          #endif

          #if (MIDI_thru == 1)
          if (type < 160) {
            midiSendFiltered(type, note, 127, 0);
          } else {
            midiSendFiltered(type, note, velocity, 0);   // le note vengono sempre sparate fuori al massimo.
          }
          #endif

          // bit tables:
          // 1 - ledstatus 1 e 2
          // 2 - feedback_bit1
          // 3 - feedback_bit2
          // 4 - bit_toggle 1 e 2
          
          for(int ledD = 0; ledD < max_modifiers; ledD++) { // elaborazione led feedback
            
            if (data_VA[ledD] == note && bit_read(3, ledD) == 1) {
              #if (FAST_FEEDBACK == 1)
              if (data_MODE[ledD] == 35 || data_MODE[ledD] == 36) { // fast feedback per LED-ONLY e DMX-ONLY
                fast_feedback_apply(ledD, velocity);
                continue; // salta logiche pesanti (toggle/offgroup/matrix ecc.)
              }
              #endif
              if (data_MODE[ledD] < 16) {    
#if (DMX_active == 1)
                DmxSimple.write(data_DM[ledD], velocity * 2);
                #endif
              }

              if (data_MODE[ledD] < 11 || data_MODE[ledD] == 27) { // buttons + velo-pads, prima pagina
                bit_write(1, data_LT[ledD] - 1, 1); // il led relativo al pulsante (ricevuto) viene memorizzato come acceso
                old_pitch = 126; // old_pitch viene usato nel filtro anti-doppioni interno alla void NoteOn 
                                 // (che invia i messagi midi definitivamente)

                //----------------------------------
                if (page == 0) { // accendi un led  SOLO SE la pagina = 0
                  if (data_MODE[ledD] >= 3) {
                    offgroup(ledD, 0); // offgroup : canale, midiout (1 = manda midi) 
                                       // se il pulsante è in un gruppo toggle, spengo gli altri pulsanti nel gruppo
                  }

                  if (data_LT[ledD] > 0) { // se al pulsante è effettivamente associato un led. //  0 = nessun effetto visivo
                    if (data_VA[general_mempos] != 0) { // nomobo mode  
#if (shifter_active == 1)
                      ledControl(ledD, 1); 
                      #endif
                    } else {
#if (shifter_active == 1)
                      shifter.setPin(data_LT[ledD] - 1, 1); 
                      shifterwrite = 1; // il led viene acceso // da notare il "-1" , che serve per allineare la numerazione dell'editor, che parte da 1                          
                      #endif


                    }                                   
                  }
                }

                { // midi feedback su toggle
                  if (data_MODE[ledD] > 1 && data_MODE[ledD] < 11) { // se il pulsante e' in toggle e togglegroups
                    // { data_LB[ledD]=0;   bit_write(4,ledD,0);  } // 4 = toggletable
                    { //data_LB[ledD]=0;  
                      bit_write(4, ledD, 1); 
                    }
                  } else if (data_MODE[ledD] == 1) { // se il pulsante e' button mode semplice
                    bit_write(4, ledD, 1); // 4 = toggletable
                    // data_LB[ledD] = 0;
                  } 
                } 
              } // nb: data_LB viene usato in modo diverso per pulsanti o pot! 
            }

            // -------------------------------------------------- 
            if (data_VA[ledD + max_modifiers] == note && bit_read(3, ledD + max_modifiers) == 1) {

                       #if (FAST_FEEDBACK == 1)
                       if (data_MODE[ledD] == 35 || data_MODE[ledD] == 36) { // fast feedback per modalità LED-ONLY e DMX-ONLY
                fast_feedback_apply(ledD, velocity);
                continue;
              }
                       #endif
              
              if (data_MODE[ledD] < 16) {
#if (DMX_active == 1)
                DmxSimple.write(data_DM[ledD], velocity * 2); 
                #endif
              }

              if (data_MODE[ledD] < 11 || data_MODE[ledD] == 27) { // feedback buttons + velo-pads
                // seconda pagina
                bit_write(1, data_LT[ledD] + max_modifiers - 1, 1); // il led relativo al pulsante (ricevuto) viene memorizzato come acceso
                old_pitch = 126; // old_pitch viene usato nel filtro anti-doppioni interno alla void NoteOn 
                                 // (che invia i messagi midi definitivamente)

                //----------------------------------
                if (page != 0) { // agisci solo se sei su seconda pagina
                  if (data_MODE[ledD] >= 3) {
                    offgroup(ledD, 0); // offgroup : canale, midiout (1 = manda midi) 
                                       // se il pulsante è in un gruppo toggle, spengo gli altri pulsanti nel gruppo
                  }

                  if (data_LT[ledD] > 0) {
                    if (data_VA[general_mempos] != 0) { // se sei in modalità NOMOBO
#if (shifter_active == 1)
                      ledControl(ledD, 1);
                      #endif
                    } else {
#if (shifter_active == 1)
                      shifter.setPin(data_LT[ledD] - 1, 1); 
                      shifterwrite = 1; 
                      #endif


                    }
                  }
                   
                  // DmxSimple.write(data_DM[ledD], velocity); 
                  // #endif
                } 

                {                       
                  if (data_MODE[ledD] > 1 && data_MODE[ledD] < 11) { 
                    bit_write(4, ledD + max_modifiers, 1); 
                  } else if (data_MODE[ledD] == 1) {
                    bit_write(4, ledD + max_modifiers, 1);
                  } 
                }
              }
            }
          }  
        } 

        ///////////////////////
        //////////////////////  ---------------------------------------------------------------------------------------------------------------
        /////////////////////
        if (velocity == 0) {
          #if (MIDI_thru == 1)
          midiSendFiltered(type, note, velocity, 0);
          #endif
  
          for(int ledE = 0; ledE < max_modifiers; ledE++) { // shifter.setPin(led, ledstatus2[led]);   // elaborazione led feedback
            if (data_VA[ledE] == note && bit_read(3, ledE) == 1) { // il feedback visivo funziona solo per i pulsanti, non per i pot e altro

                          #if (FAST_FEEDBACK == 1)
                          if (data_MODE[ledE] == 35 || data_MODE[ledE] == 36) { // fast feedback per LED-ONLY e DMX-ONLY
                fast_feedback_apply(ledE, 0);
                continue;
              }
                          #endif
              
              if (data_MODE[ledE] < 16) {
#if (DMX_active == 1)
                DmxSimple.write(data_DM[ledE], 0);
                #endif
              }

              if (data_MODE[ledE] < 11 || data_MODE[ledE] == 27) { // buttons + velo-pads, prima pagina
                bit_write(1, data_LT[ledE] - 1, 0); // ledstatus // memorizzo il ledstgatus come spento 
                bit_write(3, ledE, 0); // feedback2

                if (page == 0) {
                  if (data_LT[ledE] > 0) {
                    if (data_VA[general_mempos] != 0) { // nomobo
#if (shifter_active == 1)
                      ledControl(ledE, 0);
                      #endif
                    } else {
#if (shifter_active == 1)
                      shifter.setPin(data_LT[ledE] - 1, 0);
                      shifterwrite = 1;
                      #endif


                    }
                  }

                   
                  // DmxSimple.write(data_DM[ledE], 0);
                  // #endif
                }

                // gestione toggletable
                if (data_MODE[ledE] > 1 && data_MODE[ledE] < 11) { // se il pulsante è in toggle o togglegroups
                  // data_LB[ledE]=1; 
                  // nota: nella void push_button, data_LB viene portato a zero quando un pulsante e' PREMUTO
                  // - viene abilitato a mandare midiON solo se data_LB e' 1
                  bit_write(4, ledE, 0); // 4 = toggletable
                                         // nota: quando la toggletable (4) e' = 1 , 
                                         // la pressione del pulsante genera midiout (ved void pushbuttons)
                } else if (data_MODE[ledE] == 1) { // se il pulsante è in modalità normale
                  bit_write(4, ledE, 0);
                } 
              }
            }

            if (data_VA[ledE + max_modifiers] == note && bit_read(3, ledE + max_modifiers) == 1) { // feedback solo per pulsanti
              if (data_MODE[ledE] < 16) {
                 
                // DmxSimple.write(data_DM[ledE], 0);
                // #endif
              }

              if (data_MODE[ledE] < 11 || data_MODE[ledE] == 27) { // buttons + velo-pads, seconda pagina
                bit_write(1, data_LT[ledE] + max_modifiers - 1, 0); 
                bit_write(3, ledE + max_modifiers, 0);  

                if (page != 0) { // spegni le luci solo se sei sulla seconda pagina  
                  if (data_LT[ledE] > 0) { // se il pulsante ha un led associato // 0 = nessun led
                    if (data_VA[general_mempos] != 0) {
#if (shifter_active == 1)
                      ledControl(ledE, 0);
                      #endif
                    } else {
#if (shifter_active == 1)
                      shifter.setPin(data_LT[ledE] - 1, 0);
                      shifterwrite = 1; 
                      #endif


                    }
                  }
                   
                  // DmxSimple.write(data_DM[ledE], 0);
                  // #endif
                } 

                if (data_MODE[ledE] > 1 && data_MODE[ledE] < 11) {
                  data_LB[ledE] = 1;
                  bit_write(4, ledE + max_modifiers, 0);
                } else if (data_MODE[ledE] == 1) {
                  bit_write(4, ledE + max_modifiers, 0);
                } 
              }    
            }   
          }   
        }  
        #endif
      }   
      note = 255; // out of range (0,127) value
      action = 0;  
    } else {
      // nothing
    }   
  }  
}

    
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void reset_mempos() {
  control_present = 0; // Publish absence before replacing references or tables.
  #if defined(__AVR_ATmega32U4__) && (hid_mouse == 1)
  mouse_hid_wheel_counter = 0;
  #endif
  mouse_mempos = page_mempos = 0;
  spinner_mempos[0] = spinner_mempos[1] = 0;
  touch_mempos[0] = touch_mempos[1] = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// CTRL-F: EEPROM_WRITE_OLD_VERSION
// ============================================================
// VERSIONE ORIGINALE - disattivata (commentata), tenuta come riferimento/rollback.
// Sostituita dalla versione sotto (CTRL-F: EEPROM_WRITE_NEW_VERSION), che condensa
// il pattern "if (memoryposition<64) ... else ... +512-64 ..." (ripetuto identico
// in ogni case) in un helper unico: eepromWriteField(). Stesso comportamento,
// misurato ~202 byte di flash risparmiati (avr-gcc -Os -mmcu=atmega32u4).
// Per tornare alla vecchia versione: ri-commenta il blocco NEW e scommenta questo.
// ============================================================


// ============================================================ fine versione originale commentata


// CTRL-F: EEPROM_WRITE_NEW_VERSION
// ============================================================
// VERSIONE NUOVA - helper + eeprom_write() condensata.
// eepromWriteField(offset, val) sostituisce il pattern ripetuto:
//   if (memoryposition < 64) EEPROM.write(memoryposition + offset, val);
//   else                     EEPROM.write(memoryposition + 512 - 64 + offset, val);
// che nell'originale compariva identico (a parte l'offset) in tutti e 7 i case,
// col commento "ATTENZIONE: c'è un -64 che serve per compensare...".
// Nessun array dati toccato, nessuna EEPROM read toccata, nessuna logica di
// remapper/soglie velocity/marker-scena cambiata: cambia solo COME viene scritto
// il byte finale, non COSA o QUANDO.
// ============================================================
static inline void eepromWriteField(int offset, byte val) {
  if (memoryposition < 64) {
    EEPROM.write(memoryposition + offset, val);
  } else {
    EEPROM.write(memoryposition + 512 - 64 + offset, val);
  }
}

void eeprom_write() {
#if (ENABLE_EEPROM == 0)
  // EEPROM disabilitata: upload preset non supportato.
  return;
#endif

  {
    if (type < 160) {
      editorcounter = 0;


#if (shifter_active == 1)
      if (data_VA[general_mempos] == 0) {
        shifter.setAll(HIGH);
        shifter.write();     // flash di luce - dartmobo
        delay(5);
        shifter.setAll(LOW);
        shifter.write();
      }
      #endif

    }

    //  ________________________________________________________________________________________________________________
    // i messaggi vengono posizionati in eeprom in tale ordine:
    // 0-65 = type                512 - 2nd page
    // 64-128 = value             576 - 2nd page
    // 128-192 = toggle mode      640 - 2nd page
    // 192-256 = DMX              704 - 2nd page
    // 256-320 = min              768 - 2nd page
    // 320-384 = max              832 - 2nd page
    // 384-448 = nel korova viene memorizzatoo il keyboard 896
    // 448-512   led              960 - 2nd page

    switch (editorcounter) {
      case 0: ////////////////////////////////////////////////////////////////////////////////////  NOTE - MEMORYPOSITION
        if (velocity < 56) {
          memoryposition = remapper(velocity);  // riposiziono in memoria gli elementi in modo che la numerazione sia regolare sulla pcb
        } else if (velocity < 64) {
          memoryposition = velocity;
        } else if (velocity < 120) {
          memoryposition = remapper(velocity);
        } else {
          memoryposition = velocity;
        }


        if (matrix_vert1 > 0) {
          bitWrite(note, 7, bitRead(matrix_vert1, 0));
        }

        eepromWriteField(64, note); // value - data_VA[] - pitch della nota
        break;

      case 1: //////////////////////////////////////////////////////////////////////////////////////////// MIN - MAX
        if (matrix_vert1 > 0) {
          bitWrite(note, 7, bitRead(matrix_vert1, 1));
          bitWrite(velocity, 7, bitRead(matrix_vert1, 2));
        }

        eepromWriteField(320, note);     // max // data_MA[]
        eepromWriteField(256, velocity); // min // data_MI[]
        break;

      case 2: /////////////////////////////////////////////////////////////////////////////////////////  MODE - DMX
        if (matrix_vert2 > 0) {
          bitWrite(velocity, 7, bitRead(matrix_vert2, 0));
        }

        eepromWriteField(128, note);     // MODE // data_MODE[]
        eepromWriteField(192, velocity); // dmx  // data_DM[]
        break;

      case 3: { //////////////////////////////////////////////////////////////////////////////////////// keyboard - miditype
        if (matrix_vert2 > 0) {
          bitWrite(note, 7, bitRead(matrix_vert2, 1));
        }


        byte typeByte = (velocity == 6) ? 244 : (byte)(type - 176 + (velocity * 16) + 144);
        eepromWriteField(0, typeByte);  // TYPE // data_TY[]
        eepromWriteField(384, note);    // qwerty // data_QW[]
        break;
      }

      case 4: ////////////////////////////////////////////////////////////////////////////////////////  LED
        eepromWriteField(448, note); // LED // data_LT[] - solo prima pagina, come nell'originale
        break;

      case 5: ///////////////////////////////////////////////////////////////////////////////////////////////
        if (matrix_vert1 > 0) {
          bitWrite(note, 7, bitRead(matrix_vert1, 3));
        }
        if (matrix_vert2 > 0) {
          bitWrite(velocity, 7, bitRead(matrix_vert2, 2));
        }

        eepromWriteField(128, note);    // User_byte_1 - data_MODE[]
        eepromWriteField(0, velocity);  // User_byte_2 - data_TY[]
        break;

      case 6: ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (matrix_vert2 > 0) {
          bitWrite(note, 7, bitRead(matrix_vert2, 3));
        }

        eepromWriteField(448, note); // User_byte_3 - data_LT[]

        matrix_vert1 = 0;
        matrix_vert2 = 0;
        break;
    }

    editorcounter++;
  }
}
// ============================================================ fine versione nuova
