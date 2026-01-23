/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */

void load_preset_base() { // carica le variabili che devono restare in memoria indipendentemnte per le due pagine, per gestire il feedback. 
  for (int i = 0; i < max_modifiers; i++) {
    typetable[i] = EEPROM.read(i); // note cc pc at + channel
    valuetable[i] = EEPROM.read(i + 64); // 0-127 che nota è
    typetable[i + max_modifiers] = EEPROM.read(i + (512));
    valuetable[i + max_modifiers] = EEPROM.read(i + 64 + (512));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void load_preset(boolean numero) {
  // numero is the page to load - 0 1 
  // carica le variabili dalla EEPROM alla memoria esecutiva, 
  /// secondo la pagina in cui ci si trova.
     
  #if (shifter_active == 1 && stratos == 0)
  if (valuetable[general_mempos] == 0) {
    shifter.setAll(HIGH);
    shifter.write();
  } // flash di luci quando viene caricato il preset
  #endif
  // if (numero > 0) numero = 1;

  eeprom_preset_active = 0;

  for (byte i = 0; i < max_modifiers; i++) {
    modetable[i] = EEPROM.read(i + 128 + (numero * 512));    

    #if (Matrix_Pads > 0)
    if (i < 47)
    #endif
    setup_mempos(i); // dopo aver caricato la modetable facciamo il settaggio macchina
    
    dmxtable[i] = EEPROM.read(i + 192 + (numero * 512));
  
    if (i == mouse_mempos && mouse_mempos > 0) {  
      minvalue[i] = remapper(EEPROM.read(i + 256 + (numero * 512)) - 1); 
      maxvalue[i] = remapper(EEPROM.read(i + 320 + (numero * 512)) - 1);   
      
      // quando vengono attivate le funzioni MOUSE, XY del joystick vengono impostate su BLIND INPUT - 
      // in tal modo si evitano conflitti
      // correzione - dato che poi gli input non verrebbero letti (analogread verrebbe saltato da AIN) tolgo questa parte
      if (dmxtable[i] != 0) {
        modetable[minvalue[mouse_mempos]] = 0;
        modetable[maxvalue[mouse_mempos]] = 0;
      } 
    } else {
      minvalue[i] = EEPROM.read(i + 256 + (numero * 512)); // if (dmxtable[mouse_mempos] == 2)
      
      if (modetable[i] == 19 || modetable[i] == 21 || modetable[i] == 22) { // con speed = 0 l'encoder non emette nulla, adesso speed 0 è = 1
        if (minvalue[i] == 32) {
          minvalue[i]++;
        }
      }
      
      maxvalue[i] = EEPROM.read(i + 320 + (numero * 512));
      // if (modetable[i] == 21) lastEncoded[0] = maxvalue[i];
      // if (modetable[i] == 22) lastEncoded[1] = maxvalue[i];
    }

    /*
    if (modetable[i] == 21) {
      // encoder_mempos[0] = i; 
      update_scala(0);
    }
    if (modetable[i] == 22) {
      // encoder_mempos[1] = i;  
      update_scala(1);
    }
    */

    // quando da editor si specifica la posizione su circuito degli assi x e y - viene usato il remap da numerazione semplificata a numerazione normale
    // if (i == mouse_mempos) maxvalue[i] = remapper(EEPROM.read(i+320+(numero*512))-1); else maxvalue[i] =EEPROM.read(i+320+(numero*512));
    qwertyvalue[i] = EEPROM.read(i + 384 + (numero * 512));
  }
 
  if (encoder_mempos[0] == 0) {
    // 181 risolto il problema tra b6 (input in posizione reale = 0) e lo spinner1 (quando non viene dichiarato nel preset.
    // da adesso , quando encoder_mempos[0] = 0 il controller va a verificare che non ci sia un conflitto con un item in memoryposition 0 
    // se si va a posizionare lo spinner1 altrove in memoria, dove c'è spazio inutilizzato.
    if (modetable[0] != 21) {
      for (byte i = 0; i < max_modifiers; i++) {
        if (modetable[i] == 0) {
          encoder_mempos[0] = i;
          break;
        }
      }
    }
  }

  // if (mouse
  delay(5);
  
  #if (shifter_active == 1 && stratos == 0)
  if (valuetable[general_mempos] == 0) {
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
      lightable[i] = EEPROM.read(i + 448 + (numero * 512));
    }
  }

  // ------------------------------------------------------------
  // SCENE: ricostruzione lista pot-soggetto dopo load preset
  // La facciamo una sola volta, quando viene caricata la pagina 0
  // ------------------------------------------------------------
  #if Scene
  if (numero == 0) {
    scene_build_subject_list();
    scene_eeprom_load();
  }
  #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup_mempos(byte i) { // richiamato da load_preset
  if (modetable[i] == 18) {
    distance_mempos = i;
  }
  
  //____________________________________________________________________________
  if (modetable[i] == 21) {
    encoder_mempos[0] = i; // update_scala(0);
  }
  
  if (modetable[i] == 22) {
    encoder_mempos[1] = i; // update_scala(1);
  }
  
  //______________________________________________________________________________
  if (modetable[i] == 23) {
    touch_mempos[0] = i;
  }
  
  if (modetable[i] == 24) {
    touch_mempos[1] = i;
  }
  
  if (modetable[i] == 25) {
    mouse_mempos = i;
  }
  
  if (modetable[i] == 20) {
    PADS_mempos = i;
  }
  
  if (modetable[i] == 17) {
    page_mempos = i; 
  }
  
  if (modetable[i] == 26) {
    general_mempos = i;
    eeprom_preset_active = 1;
  }
  
  if (modetable[i] == 30 || modetable[i] == 16) {
    lastbutton[i] = 1;
  }
  
  // if (dmxtable[general_mempos] > 1) { modetable[45] = 0; modetable[37] =0;} // disattiva lo scanning sugli input dedicati al secondo encoder
  // se il secondo encoder è attivo
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (stratos == 0) // dart one
void aux_preset() { // preset di base caricato all'avvio soltanto se il preset sulla eeprom non valido. 
  for (byte i = 0; i < max_modifiers; i++) {
    modetable[i] = 1;
    valuetable[i + page] = 60 + (i / 8) * 8; // di default, tutte le note emesse sono uguali. in questo modo, se non si montano multiplexers, ogni in triggera otto note, che essendo tutte uguali verranno filtrate dal midiout in una solo nota.
    valuetable[i] = 60 + (i / 8) * 8;
    minvalue[i] = 0;
    maxvalue[i] = 127;
    dmxtable[i] = 0;
    qwertyvalue[i] = 0;
    typetable[i] = 144;
    typetable[i + page] = 144;

    #if (ENABLE_AUTODETECT == 1)
    if (eeprom_preset_active == 0) {
      lightable[i] = 0; // in autodetect: niente LED associati => niente blinker confuso
    } else {
      lightable[i] = remapper(i); // in modalità normale: comportamento storico
    }
    #else
    lightable[i] = remapper(i);
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
  encoder_mempos[0] = 55;
  encoder_mempos[1] = 56;
  touch_mempos[0] = 57;        
  touch_mempos[1] = 58;
  general_mempos = 59;
    
  dmxtable[mouse_mempos] = 0; // disabilita il mouse
    
  dmxtable[encoder_mempos[0]] = 1; // encoder mode
  minvalue[encoder_mempos[0]] = 10; // speed 
  lastbutton[encoder_mempos[0]] = 64;
  typetable[encoder_mempos[0]] = 176;
  typetable[encoder_mempos[1]] = 176;
    
  lightable[touch_mempos[0]] = 0; // no virtual touch
  lightable[touch_mempos[1]] = 0;
  typetable[touch_mempos[0]] = 144;
  typetable[touch_mempos[1]] = 145;
  // qwertyvalue[touch_mempos[0]]=0;
  // qwertyvalue[touch_mempos[1]]=0;
  minvalue[touch_mempos[0]] = 70;
  minvalue[touch_mempos[1]] = 70;
    
  qwertyvalue[general_mempos] = 0; // no efx
  maxvalue[general_mempos] = 34; // no pads
  minvalue[general_mempos] = 0; // no extraplex
  dmxtable[general_mempos] = 2; // no 2nd encoder
  dmxtable[mouse_mempos] = 0; // no mouse emulation
  valuetable[general_mempos] = 0; // shifters & multiplexers active.
}
#endif
