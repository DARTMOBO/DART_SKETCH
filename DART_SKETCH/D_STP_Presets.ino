/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */

void load_preset_base() {
  control_present = 0; // Do not let the ISR consume partially replaced tables.
#if (ENABLE_EEPROM == 0)
  // EEPROM disabilitata: niente preset da caricare.
  // Lasciamo le tabelle come sono (verranno inizializzate da aux_preset()).
  return;
#endif
 // carica le variabili che devono restare in memoria indipendentemnte per le due pagine, per gestire il feedback. 
  for (int i = 0; i < max_modifiers; i++) {
    data_TY[i] = EEPROM.read(i); // note cc pc at + channel
    data_VA[i] = EEPROM.read(i + 64); // 0-127 che nota è
    data_TY[i + max_modifiers] = EEPROM.read(i + (512));
    data_VA[i + max_modifiers] = EEPROM.read(i + 64 + (512));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void load_preset(boolean numero) {
  reset_mempos();
#if (ENABLE_EEPROM == 0)
   
  // Forziamo AUTODETECT: eeprom_preset_active=0 + preset base.
  eeprom_preset_active = 0;
  aux_preset();
  return;
#endif

  // numero is the page to load - 0 1 
  // carica le variabili dalla EEPROM alla memoria esecutiva, 
  /// secondo la pagina in cui ci si trova.
     
#if (shifter_active == 1)
  if (data_VA[general_mempos] == 0) {
    shifter.setAll(HIGH);
    shifter.write();
  } // flash di luci quando viene caricato il preset
  #endif
  // if (numero > 0) numero = 1;

  eeprom_preset_active = 0;

  for (byte i = 0; i < max_modifiers; i++) {
    #if (VELO_PAD_RELEASE_SCANS > 0)
    const byte previousMode = data_MODE[i];
    #endif
    data_MODE[i] = EEPROM.read(i + 128 + (numero * 512));
    #if (VELO_PAD_RELEASE_SCANS > 0)
    // Il marker di rilascio appartiene solo a MODE 27: pulisci ai cambi di tipo.
    // Conserva invece pressione/conteggio se entrambe le pagine usano velocity pad.
    if (data_MODE[i] != previousMode &&
        (data_MODE[i] == 27 || (previousMode == 27 && (data_LB[i] & 0x80)))) {
      data_LB[i] = 1;
    }
    #endif

    // CTRL-F: LOAD_DATA_MODE_OTHER
    // Keep the inactive page mode table coherent without changing the meaning
    // of data_MODE[] everywhere else in the sketch.
    #if (USE_REFACTORED_MIDIFEEDBACK == 1) && (ENABLE_DUAL_PAGE_FEEDBACK == 1)
    data_MODE_OTHER[i] = EEPROM.read(i + 128 + ((numero ? 0 : 1) * 512));
    #endif

    setup_mempos(i); // dopo aver caricato la data_MODE facciamo il settaggio macchina
    
    data_DM[i] = EEPROM.read(i + 192 + (numero * 512));
  
    if (i == mouse_mempos && data_MODE[i] == 25) {  
      data_MI[i] = remapper(EEPROM.read(i + 256 + (numero * 512)) - 1); 
      data_MA[i] = remapper(EEPROM.read(i + 320 + (numero * 512)) - 1);   
      
      // Reserve valid axes only after every MODE has been loaded.
    } else {
      data_MI[i] = EEPROM.read(i + 256 + (numero * 512)); // if (data_DM[mouse_mempos] == 2)
      
      if (data_MODE[i] == 19 || data_MODE[i] == 21 || data_MODE[i] == 22) { // con speed = 0 l'encoder non emette nulla, adesso speed 0 è = 1
        if (data_MI[i] == 32) {
          data_MI[i]++;
        }
      }
      
      data_MA[i] = EEPROM.read(i + 320 + (numero * 512));
      // if (data_MODE[i] == 21) lastEncoded[0] = data_MA[i];
      // if (data_MODE[i] == 22) lastEncoded[1] = data_MA[i];
    }

    /*
    if (data_MODE[i] == 21) {
      // spinner_mempos[0] = i; 
      update_scala(0);
    }
    if (data_MODE[i] == 22) {
      // spinner_mempos[1] = i;  
      update_scala(1);
    }
    */

    // quando da editor si specifica la posizione su circuito degli assi x e y - viene usato il remap da numerazione semplificata a numerazione normale
    // if (i == mouse_mempos) data_MA[i] = remapper(EEPROM.read(i+320+(numero*512))-1); else data_MA[i] =EEPROM.read(i+320+(numero*512));
    data_QW[i] = EEPROM.read(i + 384 + (numero * 512));
  }


  // Absent spinners are disabled by presence bits, without borrowing a slot.

  // if (mouse
  delay(5);
  
#if (shifter_active == 1)
  if (data_VA[general_mempos] == 0) {
    shifter.setAll(LOW);
    shifter.write();
  }
  #endif
  
  if (eeprom_preset_active == 0) { // nessun preset valido trovato
    aux_preset();

    // CTRL-F: AUTODETECT_DISABLED_FALLBACK
    // Se AUTODETECT è disattivato, dopo aux_preset() forziamo la modalità normale:
    // - niente branch di autodetect in AIN()
    // - la macchina resta comunque usabile col preset di base.
    #if (ENABLE_AUTODETECT == 0)
    eeprom_preset_active = 1;
    #endif
  } else {
    for (byte i = 0; i < max_modifiers; i++) {
      data_LT[i] = EEPROM.read(i + 448 + (numero * 512));
    }
    // MOUSE owns its axes regardless of their order in EEPROM. Zero in the
    // preset maps to 255 (unassigned); never use that sentinel as an array index.
    if (data_MODE[mouse_mempos] == 25) {
      data_QW[mouse_mempos] = 255; // RAM-only cache: remapped wheel input.
      #if defined(__AVR_ATmega32U4__) && (hid_mouse == 1)
      byte wheel = data_VA[mouse_mempos + (numero ? max_modifiers : 0)];
      if (wheel != 0) {
        wheel = remapper(wheel - 1);
        if (wheel < max_modifiers &&
            (data_MODE[wheel] == 19 || data_MODE[wheel] == 21 || data_MODE[wheel] == 22))
          data_QW[mouse_mempos] = wheel;
      }
      if (data_DM[mouse_mempos] != 0) {
        byte x = data_MI[mouse_mempos], y = data_MA[mouse_mempos];
        if (x < max_modifiers) data_MODE[x] = 0;
        if (y < max_modifiers) data_MODE[y] = 0;
      }
      #endif
    }
    rebuild_control_presence();
  }

  // ------------------------------------------------------------
   
  // La facciamo una sola volta, quando viene caricata la pagina 0
  // ------------------------------------------------------------
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Final validation happens after MOUSE may have made an axis BLIND.
// One atomic publication after all settings are ready; no MODE scan in loop().
void rebuild_control_presence() {
  byte present = 0;
  if (data_MODE[page_mempos] == 17) present |= CONTROL_PAGE;
  if (data_MODE[mouse_mempos] == 25) present |= CONTROL_MOUSE;
  if (data_MODE[touch_mempos[0]] == 23) present |= CONTROL_TOUCH1;
  if (data_MODE[touch_mempos[1]] == 24) present |= CONTROL_TOUCH2;
  if (data_MODE[spinner_mempos[0]] == 21) present |= CONTROL_SPINNER1;
  if (data_MODE[spinner_mempos[1]] == 22) present |= CONTROL_SPINNER2;
  control_present = present;
}

void setup_mempos(byte i) { // richiamato da load_preset
  if (data_MODE[i] == 18) {
    distance_sensor_mempos = i;
  }
  
  //____________________________________________________________________________
  if (data_MODE[i] == 21) {
    spinner_mempos[0] = i; // update_scala(0);
  }
  
  if (data_MODE[i] == 22) {
    spinner_mempos[1] = i; // update_scala(1);
  }
  
  //______________________________________________________________________________
  if (data_MODE[i] == 23) {
    touch_mempos[0] = i;
  }
  
  if (data_MODE[i] == 24) {
    touch_mempos[1] = i;
  }
  
  if (data_MODE[i] == 25) {
    mouse_mempos = i;
  }
  
  #if (Piezo_pads == 1)
  if (data_MODE[i] == 20) {
    PADS_mempos = i;
  }
  #endif
  
  if (data_MODE[i] == 17) {
    page_mempos = i; 
  }
  
  if (data_MODE[i] == 26) {
    general_mempos = i;
    eeprom_preset_active = 1;
  }
  
  if (data_MODE[i] == 30 || data_MODE[i] == 16) {
    data_LB[i] = 1;
  }

 
  #if (ENABLE_POT_TAKEOVER == 1)
  if (data_MODE[i] >10 && data_MODE[i] < 16) {
   // if (takeover_init == 1) data_LB[i+page] = 128;
 //   if (takeover_init == 1) data_LB[i + (byte)((page / max_modifiers) * 64)] = 128;
  }
   #endif
  
  
  // if (data_DM[general_mempos] > 1) { data_MODE[45] = 0; data_MODE[37] =0;} // disattiva lo scanning sugli input dedicati al secondo encoder
  // se il secondo encoder è attivo
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void aux_preset() { // preset di base caricato all'avvio soltanto se il preset sulla eeprom non valido.
  control_present = 0; 
  for (byte i = 0; i < max_modifiers; i++) {
    data_MODE[i] = 1;
    #if (USE_REFACTORED_MIDIFEEDBACK == 1) && (ENABLE_DUAL_PAGE_FEEDBACK == 1)
    data_MODE_OTHER[i] = 1;
    #endif

    // ==== [AUX_PRESET_UNIQUE_NOTES_SAMD] CTRL-F: AUX_PRESET_UNIQUE_NOTES_SAMD ====
    // Allineamento con lo sketch di test M0: ogni item ha un data1 diverso, cosi'
    // nel monitor MIDI si capisce subito quale memory position sta trasmettendo.
    // Scriviamo esplicitamente sia PAGE 1 sia PAGE 2, evitando dipendenze dal
    // valore runtime di 'page' durante l'avvio.
    #if defined(ARDUINO_ARCH_SAMD)
    data_VA[i] = 36 + i;
    data_VA[i + max_modifiers] = 36 + i;
    data_TY[i] = 144;
    data_TY[i + max_modifiers] = 144;
#else
    data_VA[i + page] = 60 + (i / 8) * 8; // di default, tutte le note emesse sono uguali. in questo modo, se non si montano multiplexers, ogni in triggera otto note, che essendo tutte uguali verranno filtrate dal midiout in una solo nota.
    data_VA[i] = 60 + (i / 8) * 8;
    data_TY[i] = 144;
    data_TY[i + page] = 144;
    #endif

    data_MI[i] = 0;
    data_MA[i] = 127;
    data_DM[i] = 0;
    data_QW[i] = 0;

    #if (ENABLE_AUTODETECT == 1)
    if (eeprom_preset_active == 0) {
      data_LT[i] = 0; // in autodetect: niente LED associati => niente blinker confuso
    } else {
      data_LT[i] = remapper(i); // in modalità normale: comportamento storico
    }
#else
    data_LT[i] = remapper(i);
    #endif
  }

  // nota: i modificatori normali vanno da 0 a 55 nel normale ciclo AIN e nelle tabelle array
  // rimangono solo 4 spazi liberi in teoria
  // ma bisogna considerare che: se l'extraplex è disattivato, tutti gli spazi da 48 a 55 sono disponibili
  // se si attiva il secondo encoder, gli slot di memoria relativi ai pin 46 e 48 (numerazione ciclo AIN) sono disponibili
  // se si attiva l'extraplex il secondo touch non ha bisogno di settaggio

  // nel default preset l'extraplex non viene attivato
  page_mempos = 53; 
  mouse_mempos = 54;
  spinner_mempos[0] = 55;
  spinner_mempos[1] = 56;
  touch_mempos[0] = 57;        
  touch_mempos[1] = 58;
  general_mempos = 59;
    
  data_DM[mouse_mempos] = 0; // disabilita il mouse
    
  data_DM[spinner_mempos[0]] = 1; // encoder mode
  data_MI[spinner_mempos[0]] = 10; // speed 
  data_LB[spinner_mempos[0]] = 64;
  data_TY[spinner_mempos[0]] = 176;
  data_TY[spinner_mempos[1]] = 176;
    
  data_LT[touch_mempos[0]] = 0; // no virtual touch
  data_LT[touch_mempos[1]] = 0;
  data_TY[touch_mempos[0]] = 144;
  data_TY[touch_mempos[1]] = 145;
  // data_QW[touch_mempos[0]]=0;
  // data_QW[touch_mempos[1]]=0;
  data_MI[touch_mempos[0]] = 70;
  data_MI[touch_mempos[1]] = 70;
    
  data_QW[general_mempos] = 0; // no efx
  data_MA[general_mempos] = 34; // no pads
  data_MI[general_mempos] = 0; // no extraplex
  data_DM[general_mempos] = 2; // no 2nd encoder
  data_DM[mouse_mempos] = 0; // no mouse emulation
  data_QW[mouse_mempos] = 255; // no wheel in the implicit fallback preset
  data_VA[general_mempos] = 0; // shifters & multiplexers active.
  // The fallback preset deliberately defines these slots without matching MODEs.
  control_present = CONTROL_PAGE | CONTROL_MOUSE | CONTROL_TOUCH1 | CONTROL_TOUCH2 |
                    CONTROL_SPINNER1 | CONTROL_SPINNER2;
}
