/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */
 
#if (stratos == 1)
#if (ENABLE_ENC_LOCKOUT == 1)
extern uint32_t enc_lock_last_us[60];
#endif


// ============================================================
// STRATOS (stratos==1)
//
// In precedenza qui c'erano funzioni "majority" e "lockout" dedicate a STRATOS.
// Ora le eliminiamo per usare quelle GENERALI del progetto (D_INS.ino), così:
// - il comportamento degli encoder è coerente tra STRATOS e DART classico
// - il tuning (samples/delay/lockout) è centralizzato via #define
//
// Nota polarità STRATOS:
// - in STRATOS i segnali AB sono storicamente invertiti (!digitalRead).
// - per questo, nelle letture majority usiamo la variante generale:
//     enc_majority_state_from_pins_inv()
// ============================================================

void AIN_stratos() { 
  for (channel = 0; channel < 20; channel++) { 
    chan = channel; // chan viene usata da void ain_nucleo
    // delay(1);
  
    #if (top_spinner == 1)
    if (data_LB[spinner_mempos[0]] == 64 || data_DM[general_mempos] == 0) { // 64 = no encoder action - the MAIN spinner has priority over any other action.
    #endif
      virtual_touch_end(0);

      if (channel == 1) {
        digitalWrite(14, LOW);
        digitalWrite(15, HIGH); // SERIE DIODI 1
        digitalWrite(16, HIGH); // accendi il LED
        valore = ((digitalRead(5))) * 1000;  
        ain_nucleo(); // b5  
        
      } else if (channel == 2) {
        if (data_LB[touch_mempos[0]] == 1) {
          digitalWrite(16, LOW); // spegni il LED - dipende dal touch
        }
        digitalWrite(14, LOW);
        digitalWrite(15, HIGH);
        valore = ((digitalRead(4))) * 1000;   
        ain_nucleo(); // b6 - ENCODER2 BUTTON
      } 
      
      else if (channel == 3) {
        valore = ((digitalRead(6))) * 1000;   
        ain_nucleo();
      } // b4
      
      else if (channel == 4) {
        valore = ((digitalRead(7))) * 1000;   
        ain_nucleo(); 
      } // b3
      
      else if (channel == 5) {
        // valore = analogRead(19);
        valore = ((digitalRead(19))) * 1000; 
        // Serial.println(digitalRead(7));
        // Serial.println(valore);
        // delay(50);
        // valore = ((digitalRead(19)))*1000;
        ain_nucleo(); 
      } // b2
      
      else if (channel == 6) {
        // valore = (!(digitalRead(18)))*1000; 
        digitalWrite(14, LOW);
        digitalWrite(15, HIGH);   
        valore = analogRead_1024(18);
        ain_nucleo();
      } // b1 - ENCODER1 BUTTON
      
      else if (channel == 7) {
        digitalWrite(15, LOW);
        digitalWrite(14, HIGH); // SERIE DIODI 2
        valore = (!(digitalRead(5))) * 1000;   
        ain_nucleo(); // pb7
      }   
      
      else if (channel == 8) {
        valore = ((digitalRead(6))) * 1000;   
        ain_nucleo(); 
      } // pb8
      
      else if (channel == 9) {
        valore = ((digitalRead(7))) * 1000;  
        ain_nucleo();   
      } // pb10
      
      else if (channel == 10) {
        valore = analogRead_1024(19);
        ain_nucleo();   
      } // pb9
      
      else if (channel == 11) {
    //    digitalWrite(15, LOW);
      //  digitalWrite(14, HIGH);       // cambio di canale lettura - diodi
        valore = analogRead_1024(18);
        ain_nucleo();  
      } // page
      
      else if (channel == 12) {
        valore = analogRead_1024(10);
        ain_nucleo();//  
      } 
      
      else if (channel == 13) {
        // vuoto
      } 
      
     else if (channel == 14) {
        // ===== STRATOS_USE_GENERAL_ENCODER_FILTER =====
        // Qui NON usiamo più funzioni/variabili STRATOS dedicate.
        // Usiamo il filtro *generale* (majority + lockout) come nel DART classico.
        //
        // Importante:
        // - Se ENABLE_ENC_MAJORITY = 0 → lettura STRATOS tradizionale (digitalRead invertito).
        // - Se ENABLE_ENC_LOCKOUT  = 0 → nessun lockout.
        //
        // Encoder STRATOS su pin diretti (polarità invertita): MSB=20, LSB=21
        const byte chan_enc = 14;   // in STRATOS: channel == memoryposition

        #if (ENABLE_ENC_LOCKOUT == 1)
          uint32_t now = micros();
          if ((uint32_t)(now - enc_lock_last_us[chan_enc]) < (uint32_t)ENC_LOCKOUT_US) {
            // Durante lockout: NON generiamo step.
            // Però aggiorniamo la fase quadrature (data_MA) per non creare salti.
            // >>> QUI applichiamo il LOCKOUT generale: niente step, ma aggiorna fase <<<
            byte s_lock = (byte)(((byte)(!digitalRead(20)) << 1) | (byte)(!digitalRead(21)));
            MSB[1] = (byte)(s_lock >> 1);
            LSB[1] = (byte)(s_lock & 1);
            data_MA[chan_enc] = s_lock;
            data_LB[chan_enc] = 64;
          } else
        #endif
                  {
            // RAW sempre (pin STRATOS encoder #1: 20/21, logica invertita)
            byte s_raw = (byte)(((byte)(!digitalRead(20)) << 1) | (byte)(!digitalRead(21)));

         #if (ENABLE_ENC_MAJORITY == 1) && (ENABLE_ENC_MAJ_WINDOW == 1)

              bool maj_window_open = false;

              if (s_raw != data_MA[chan_enc]) {
                uint32_t now_w = micros();
                enc_maj_window_until_us = now_w + (uint32_t)ENC_MAJ_WINDOW_US;
                maj_window_open = true;
              }
              else if (enc_maj_window_until_us != 0) {
                uint32_t now_w = micros();
                if ((int32_t)(enc_maj_window_until_us - now_w) > 0) {
                  maj_window_open = true;
                } else {
                  enc_maj_window_until_us = 0;
                }
              }
            #endif

            #if (ENABLE_ENC_MAJORITY == 1)
              // >>> Majority solo dentro finestra (se abilitata) <<<
            #if (ENABLE_ENC_MAJORITY == 1) && (ENABLE_ENC_MAJ_WINDOW == 1)
                byte s = maj_window_open
                  ? enc_majority_state_from_pins_inv(20, 21, (byte)ENC_MAJ_SAMPLES, (unsigned int)ENC_MAJ_USDELAY)
                  : s_raw;
              #else
                byte s = enc_majority_state_from_pins_inv(20, 21, (byte)ENC_MAJ_SAMPLES, (unsigned int)ENC_MAJ_USDELAY);
              #endif
            #else
              byte s = s_raw;
            #endif

            MSB[1] = (byte)(s >> 1);
            LSB[1] = (byte)(s & 1);


            updateEncoder(channel);
            encoder(channel);

            #if (ENABLE_ENC_LOCKOUT == 1)
              if (data_LB[chan_enc] != 64) enc_lock_last_us[chan_enc] = now;  // usa lo stesso timestamp
            #endif
          }
      }

      else if (channel == 15) {
        // vuoto
      } 
      
   else if (channel == 16) {
        // ===== STRATOS_USE_GENERAL_ENCODER_FILTER_2 =====
        // Secondo encoder STRATOS: stesso identico filtro generale (majority + lockout).
        //
        // Importante:
        // - Se ENABLE_ENC_MAJORITY = 0 → lettura STRATOS tradizionale (digitalRead invertito).
        // - Se ENABLE_ENC_LOCKOUT  = 0 → nessun lockout.
        //
        // Encoder STRATOS su pin diretti (polarità invertita): MSB=1, LSB=0
        const byte chan_enc = 16;   // in STRATOS: channel == memoryposition

        #if (ENABLE_ENC_LOCKOUT == 1)
          uint32_t now = micros();
          if ((uint32_t)(now - enc_lock_last_us[chan_enc]) < (uint32_t)ENC_LOCKOUT_US) {
            // >>> QUI applichiamo il LOCKOUT generale: niente step, ma aggiorna fase <<<
            byte s_lock = (byte)(((byte)(!digitalRead(1)) << 1) | (byte)(!digitalRead(0)));
            MSB[1] = (byte)(s_lock >> 1);
            LSB[1] = (byte)(s_lock & 1);
            data_MA[chan_enc] = s_lock;
            data_LB[chan_enc] = 64;
          } else
        #endif
                    {
            // RAW sempre (pin STRATOS encoder #2: 1/0, logica invertita)
            byte s_raw = (byte)(((byte)(!digitalRead(1)) << 1) | (byte)(!digitalRead(0)));

     #if (ENABLE_ENC_MAJORITY == 1) && (ENABLE_ENC_MAJ_WINDOW == 1)
              bool maj_window_open = false;

              if (s_raw != data_MA[chan_enc]) {
                uint32_t now_w = micros();
                enc_maj_window_until_us = now_w + (uint32_t)ENC_MAJ_WINDOW_US;
                maj_window_open = true;
              }
              else if (enc_maj_window_until_us != 0) {
                uint32_t now_w = micros();
                if ((int32_t)(enc_maj_window_until_us - now_w) > 0) {
                  maj_window_open = true;
                } else {
                  enc_maj_window_until_us = 0;
                }
              }
            #endif

            #if (ENABLE_ENC_MAJORITY == 1)
              #if (ENABLE_ENC_MAJORITY == 1) && (ENABLE_ENC_MAJ_WINDOW == 1)
                byte s = maj_window_open
                  ? enc_majority_state_from_pins_inv(1, 0, (byte)ENC_MAJ_SAMPLES, (unsigned int)ENC_MAJ_USDELAY)
                  : s_raw;
              #else
                byte s = enc_majority_state_from_pins_inv(1, 0, (byte)ENC_MAJ_SAMPLES, (unsigned int)ENC_MAJ_USDELAY);
              #endif
            #else
              byte s = s_raw;
            #endif

            MSB[1] = (byte)(s >> 1);
            LSB[1] = (byte)(s & 1);

            updateEncoder(channel);
            encoder(channel);

            #if (ENABLE_ENC_LOCKOUT == 1)
              if (data_LB[chan_enc] != 64) enc_lock_last_us[chan_enc] = now;  // usa lo stesso timestamp
            #endif
          }
      }

    #if (top_spinner == 1)
    } else {
      encoder(spinner_mempos[0]); // midiout 1st encoder  
      // Serial.println("encoder_active");         
    }
    #endif
  } // end PLEXER
} // end ain
#endif

#if (stratos == 1)
void aux_preset() { // preset di base caricato all'avvio soltanto se il preset sulla eeprom non e' valido. 
  for (byte i = 0; i < max_modifiers; i++) {
    data_MODE[i] = 1; // pushbuttons

    { /// stratos
      data_VA[i + page] = 60 + i; 
      data_VA[i] = 60 + i;
    }
    
    data_MI[i] = 0;
    data_MA[i] = 127;
    data_DM[i] = i;
    data_QW[i] = 0;
    data_TY[i] = 144; /// 144 = note ; 176 = cc
    data_TY[i + page] = 144;
    data_LT[i] = remapper(i);
  }

  data_MODE[12] = 11; 
  data_TY[12] = 176;
  data_MI[12] = 0;
  data_MA[12] = 127;
    
  page_mempos = 53; 
  mouse_mempos = 54;
  spinner_mempos[1] = 55;
  spinner_mempos[0] = 56;
  
  touch_mempos[0] = 57;        
  touch_mempos[1] = 58;
       
  general_mempos = 59;
    
  data_DM[mouse_mempos] = 0; // disabilita il mouse
    
  data_DM[spinner_mempos[0]] = 1; // encoder mode
  data_MI[spinner_mempos[0]] = 36; // speed - il centro è 32
  data_QW[spinner_mempos[0]] = 0; // touchstop
  data_MODE[spinner_mempos[0]] = 21;  
  data_LB[spinner_mempos[0]] = 64;
  data_TY[spinner_mempos[0]] = 176;

  #if (stratos == 1)
  data_MODE[14] = 19; // 19 = encoder - ved ain_nucleo
  data_TY[14] = 176;
  data_DM[14] = 1;
  data_MI[14] = 32;
  data_LB[14] = 64;
    
  data_MODE[16] = 19; // 19 = encoder 
  data_TY[16] = 176;
  data_DM[16] = 1;
  data_MI[16] = 1;
  data_LB[16] = 64;
  #endif
    
  data_LT[touch_mempos[0]] = 0; // touch mode 0 normal 1 virtual 2 testmode
  data_LT[touch_mempos[1]] = 0;
   
  data_MI[touch_mempos[0]] = 70; // stratos touch sensitivity
  data_TY[touch_mempos[0]] = 176; // message type stratos

  data_QW[general_mempos] = 0; // no efx
  data_MA[general_mempos] = 34; // no pads
  data_MI[general_mempos] = 0; // no extraplex
  data_DM[general_mempos] = 4; // 0 no enc 1 top enc 2 top&side enc 3 generic encoders. 
  data_DM[mouse_mempos] = 0; // no mouse emulation
}
#endif
