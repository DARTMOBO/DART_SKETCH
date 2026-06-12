/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */
#if (Scene == 1)
void scene_morph_encsc(byte enc_chan);
#endif

void encoder(byte numero) {
  // NUMERO corrisponde alla chan/memoryposition
  // data_LB[] contiene la lettura dell encoder 64 = fermo, 63 = -, 65 = + 
  // data_MODE[] mi dice se si tratta di encoder spinner (mode 21 e 22 su editor) o di encoder generico (mode 19 su editor)
  // data_DM[general_mempos] vedi se gli spinner sono attivati e quali 0-nessuno 1-top 2-top+side 3 top+side+generico
  // data_DM[numero] // la data_DM definisce la modalita - 0 e 1 endless - 2 pot - 3 scale
  // data_QW[numero] settaggio del touch stop
  // data_MI: speed e inversione - il valore va da 0 a 64 // 32 sarebe lo zero.
  // data_MA: usato per trasferimento scale
  // data_LT[numero] // usato come contatore per la modalità 63-65 che oltre allo scatto avanti/indietro fornisce anche la velocità
  // data_TY[] oltre a specificare se si tratta di note o cc - può fare accedere l'encoder alla modalità qwerty 
  // data_DM[mouse_mempos] emulazione mouse attiva
  // data_VA[mouse_mempos+page] // scelta dell encoder che emula mousewheel

  if (data_LB[numero] != 64) { 
    // ============================================================
    // CTRL-F: ENCSC_TYPE244_MARKER
    // NUOVA REGOLA: ENCSC/SCENE-MORPH via TYPE marker (pseudomidi 244)
    // - page-aware: data_TY[] contiene PAGE1 e PAGE2, quindi usiamo +page
    // - quando il marker è attivo, NON eseguiamo logica encoder normale
    // - consumiamo l'impulso (63/65) riportando data_LB a 64
    // ============================================================
    #if (Scene == 1)
    if (data_TY[numero + page] == 244) {
      scene_morph_encsc(numero); // funzione in D_scene.ino
      data_LB[numero] = 64;
      return;
    }
    #endif

    byte tocco;  
    cycletimer = 0;

    if (data_MODE[numero] > 20) { // spinner (21/22)
      numero2h = boolean(data_MODE[numero] - 21);   
      tocco = data_LB[touch_mempos[numero2h]]; // touch associato allo spinner 1/2

      encoder_block[numero2h] = constrain(encoder_block[numero2h] + (data_LB[numero] - 64), 20, 235);

      if (V_touch_regulator[numero2h] == 1 && data_LT[touch_mempos[numero2h]] == 1) { 
        midiSendRaw(data_TY[touch_mempos[numero2h] + page] + 1,
                data_VA[touch_mempos[numero2h] + page],
                127); // invio del virtual touch 

        #if (shifter_active == 1 && stratos == 0)
        shifter.setPin(4 + numero2h, HIGH);
        shifterwrite = 1;
        #endif
      }

      V_touch_regulator[numero2h] = 0;
    } else {
      // CTRL-F: GENERIC_ENCODER_TOUCH_IMMUNE
      // Encoder generici (mode 19): immuni ai touch sensor.
      // Non leggiamo touch_mempos[] e non usiamo touch-stop per loro.
      numero2h = 0;
      tocco = 0;
    }

    //-------------------------------------------------------------------------------------------------------------------------------
    if (data_DM[general_mempos] >= numero2h) {
      if (data_DM[numero] > 1) { // 2 pot - 3 scale
        // Touch-stop SOLO per spinner (tocco = stato touch). Per generici tocco=0 => sempre "touch non premuto".
        if (data_QW[numero] == 0) { 
          encoder_pot_mode(numero); 
        } else {
          if (tocco == 0) {
            encoder_pot_mode(numero); // prima era data_LB[touch_mempos[numero2h]]==0
          }
        }
      } else { // data_DM 0/1 endless
        // CTRL-F: ENCODER_SIGNED_SPEED_REFACTOR
        const int8_t speed = (int8_t)data_MI[numero] - 32; // range -32..+32
        const bool inverted = (speed < 0);

        if (data_DM[numero] == 0) { // endless 63/65 (accelerato)
          if (data_TY[numero + page] <= 191) {
            // CTRL-F: FIX_INV_ACCEL_63_65
            // Niente map32() qui: l'inversione è gestita da dir + speedAbs (evita doppia inversione).
            const int8_t dir = inverted ? -1 : 1;
            const uint8_t speedAbs = (uint8_t)(inverted ? -speed : speed);
            const uint8_t lbAccel = data_LB[numero];

            #if (stratos == 0)
            if (data_LT[numero] == 16)
            #endif
            #if (stratos == 1)
            if (data_LT[numero] == 250)
            #endif
            {
              midiSendTyped(data_TY[numero + page] + boolean(data_QW[numero]) * tocco,
                     data_VA[numero + page],
                     constrain(
                       (int)lbAccel + ((int)(lbAccel - 64) * (int)speedAbs * (int)dir),
                       0, 127),
                     0);

              data_LT[numero] = 0;
              encled[0] = encled[0] + (-(data_LB[numero] - 64)) * (int)speed;
              data_LB[numero] = 64;
            } else {
              data_LT[numero]++;
            }
          } else {
            qwerty_encoderr(numero);
            data_LB[numero] = 64;
          }
        } else { // data_DM == 1 (endless 0..127)
          if ((data_QW[numero] ^ tocco) != 3) {
            // CTRL-F: FIX_INV_ENDLESS_DMX1
            const int8_t dir = inverted ? -1 : 1;

            midiSendTyped(data_TY[numero + (page)] + boolean(data_QW[numero]) * tocco,
                   data_VA[numero + (page)],
                   constrain((-(data_LB[numero] - 64) * dir) * 127, 0, 127),
                   2);

            encled[0] = encled[0] + (-(data_LB[numero] - 64)) * (int)speed;
          }
        }

        #if (shifter_active == 1 && stratos == 0)
        if ((data_QW[numero] ^ tocco) != 3) {
          led_enc_exe();
        }
        #endif

        #if (Matrix_Pads > 0)
        if ((data_QW[numero] ^ tocco) != 3) {
          led_enc_exe_matrix();
        }
        #endif
      }

      //-------------------------------------------------------------------------------------------------------------------------------
      #if defined (__AVR_ATmega32U4__)  
      { // attivazione mousewheel
        #if (stratos == 1)
        if (data_VA[mouse_mempos + page] == numero)
        #endif

        #if (stratos == 0)
        if (remapper(data_VA[mouse_mempos + page] - 1) == numero)
        #endif
        {
          #if (hid_mouse == 1)
          if (mouse_wheel_speed_counter == 0) {
            Mouse.move(0, 0, (-(data_LB[numero] - 64)) * (constrain(data_MI[numero] - 32, -1, 1)));
          }
          #endif

          mouse_wheel_speed_counter++;
          if (mouse_wheel_speed_counter == map32(abs(data_MI[numero] - 32), 0, 32, 32, 1)) {
            mouse_wheel_speed_counter = 0;
          }
        }
      }
      #endif

      if (data_DM[numero] > 0) { // in modalità relativa con accelerazione (63-65) non devo resettare data_LB...
        data_LB[numero] = 64;
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void encoder_pot_mode(byte numero) {
  int valuepot;
  encoder_pot_calcolo(numero, data_MI[numero]); // canale e velocità/speed

  if (data_MODE[numero] > 19) {
    valuepot = encodervaluepot[numero2h];
  } else { // spin1 / spin2
    valuepot = data_LT[numero] * 4; // encoder generico
  }

  // ------------------------------------------------
  // fin qui abbiamo calcolato la posizione (0..1023 circa)
  // ------------------------------------------------

  // OUT 0..127 (unificato)
  byte out = (byte)constrain(valuepot / 8, 0, 127);

  if (data_TY[numero + page] > 223) { // pitch bend
    midiSendFiltered(data_TY[numero + page],
           (valuepot - ((valuepot / 8) * 8)) * 16,
           valuepot / 8, 1);
  } else if (data_TY[numero + page] > 159) { // CC/PC/AT (sotto 160 sono NOTE)
    // --- ANCHOR: PC_TO_CC_IN_POT_EMU ---
    byte t = data_TY[numero + page]; // status (0x90..)
    byte group = (t - 144) / 16; // 0=NOTE,1=AT,2=CC,3=PC,...
    #if (Scene == 1)
    if (group == 3) {
      // PC (0xC0..0xCF) usato come marcatore "CC-SCENE":
      // trasformo in CC (0xB0..0xBF) mantenendo lo stesso canale
      t = t - 16;
    }
    #endif
    // ============================================================
    // CTRL-F: ENCSUBJECT_TO_CONVOY
    // Se questo encoder/spinner è uno scene-subject:
    // - NON inviare diretto
    // - aggiorna il convoy (verità unica) e committa
    // - return per evitare doppio invio (convoy + button)
    // ============================================================
    #if (Scene == 1)
    {
      byte si = convoy_find_subjectIndex(numero); // numero = memoryposition dell'encoder
      if (si != 255) {
        convoy_set(si, out);
        convoy_commit();
        return;
      }
    }
    #endif
    // comportamento normale (non subject): invio diretto
    midiSendTyped(t, data_VA[numero + page], out, 1);
  } else { // NOTE => modalità SCALE (solo spinner)
    if (data_MODE[numero] > 19) {
      at = encodervaluepot[numero2h] / 8;

      if (encodervaluepot_buffer[numero2h] != at) {
        if (scala[numero2h + ((boolean(page)) * 2)] != 0) {
          scala_learn = scala[numero2h + ((boolean(page)) * 2)];
        }

        {
          if (scala_learn != 0) {
            if (encoder_block[numero2h] < 50 || encoder_block[numero2h] > 78) {
              encoder_block[numero2h] = 220;

              if (bitRead(scala_learn, (at) - ((at / 12) * 12)) == 1) {
                midiSendTyped(data_TY[numero + page], encodervaluepot_buffer[numero2h], 0, 0);
                midiSendTyped(data_TY[numero + page], at, 127, 1);
                encodervaluepot_buffer[numero2h] = at;
              } else {
                while (bitRead(scala_learn, (at) - ((at / 12) * 12)) == 0) {
                  encoder_pot_calcolo(numero, constrain(data_MI[numero], 31, 33));
                  at = encodervaluepot[numero2h] / 8;

                  if (at <= 0 || at >= 127) {
                    mouse_wheel_speed_counter = 0;
                    break;
                  }
                }

                {
                  midiSendTyped(data_TY[numero + page], encodervaluepot_buffer[numero2h], 0, 0);
                  midiSendTyped(data_TY[numero + page], at, 127, 1);
                  encodervaluepot_buffer[numero2h] = at;
                }
              }
            }
          }
        }
      }
    }
  }

  // --- LED / feedback come prima ---
  #if (shifter_active == 1 && stratos == 0)
  ledrestore(page);
  #endif

  encled[0] = abs((valuepot / 4) - 255);

  #if (shifter_active == 1 && stratos == 0)
  led_enc_exe();
  #endif

  #if (Matrix_Pads > 0)
  led_enc_exe_matrix();
  #endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void encoder_pot_calcolo(byte chan_, byte moltiplicatore) {
  // serve per calcolare la posizione del pot virtuale - encoder in modalità pot
  // la posizione del pot viene memorizzata su encodervaluepot[] per gli spinner e su data_LT[] per gli encoders generici
  
  moltiplicatore = 64 - moltiplicatore; // commentare questa riga per invertire l'avanzamento del pot emulato
  
  if (data_MODE[chan_] > 19) { // se si tratta di uno spinner - useremo encodervaluepot[] per tenere in memoria la posizione del POT VIRTUALE
                               // 19 sarebbe l'identificativo per l'encoder 
                               // 21 e 22 per gli spinner
    if (mouse_wheel_speed_counter == 0) { 
      /////////////------------- /////////////-------------/////////////-------------/////////////-------------/////////////-------------
      if (data_DM[chan_] == 2) {
        // contiene il valore-posizione attuale dello spinner 1 o 2       
        encodervaluepot[numero2h] = constrain(
          encodervaluepot[numero2h] - 
          (-(data_LB[chan_] - 64)) * // -1 o 1
          (moltiplicatore - 32), // moltiplicatore sarebbe mivalue[numero] che va da 0 a 64
          0, 1023
        );

        // Serial.println(moltiplicatore-32);
        // Serial.println(encodervaluepot[numero2h]);
        // Serial.println(encoder_block[numero2h]);
        // Serial.println("--");
        // spegazione: si aggiunge il valore speed, positio o negativo a seconda del verso.
      } else {
        // modalità virtualpot RAMP - senza limiti superiore e inferiore.      
        encodervaluepot[numero2h] = encodervaluepot[numero2h] - (-(data_LB[chan_] - 64)) * (moltiplicatore - 32);
        if (encodervaluepot[numero2h] < 0) {
          encodervaluepot[numero2h] = 1023;
        } else if (encodervaluepot[numero2h] > 1023) {
          encodervaluepot[numero2h] = 0;
        }
      }
      /////////////-------------/////////////-------------/////////////-------------/////////////-------------/////////////-------------
    }

    if (data_TY[chan_ + page] > 159) { // se type non è un NOTE
      mouse_wheel_speed_counter = 0; // se sono CC o altro mouse_wheel_speed_counter rimane sempre zero - nessun rallentamento.
    } else {
      mouse_wheel_speed_counter++;
      // if (mouse_wheel_speed_counter > (data_MI[chan_]-32)) {mouse_wheel_speed_counter =0;} // in questo modo si ha un rallentamento della mousewheel aumnetando 
      // zero = veloce
      // 32 = rallentato 
      // data_MI qui usata come speed / rallentatore.
      // if (mouse_wheel_speed_counter > constrain(abs(data_MI[chan_]-32),1,32)) {mouse_wheel_speed_counter =0;}
      // if (mouse_wheel_speed_counter > abs(data_MI[chan_]-32)) {mouse_wheel_speed_counter =0;}
      if (mouse_wheel_speed_counter > map32(abs(data_MI[chan_] - 32), 0, 32, 32, 0)) {
        mouse_wheel_speed_counter = 0;
      }
    }
  } else { // se si tratta di encoder generico - useremo data_LT[] per tenere in memoria la posizione del POT VIRTUALE
    if (data_DM[chan_] == 2) {
      // data_LT contiene il valore-posizione attuale dell'encoder generico
      data_LT[chan_] = constrain( 
        data_LT[chan_] - 
        (-(data_LB[chan_] - 64)) *
        (moltiplicatore - 32),
        0, 255
      );
    } else {
      // RAMP - data_LT contiene il valore-posizione attuale dell'encoder generico 
      // - qui in modalità illimitata - quando si va oltre 255 si torna a zero.
      data_LT[chan_] = data_LT[chan_] - (data_LB[chan_] - 64) * (-(moltiplicatore - 32));
    }
  }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// =====================================================================
// TOP SPINNER (interrupt) — lockout leggero, senza majority
// CTRL-F: TOPSPIN_LOCKOUT_ISR
// Note:
// - Qui NON usiamo majority burst: in ISR sarebbe pesante (delayMicroseconds) e può creare jitter.
// - Usiamo solo LOCKOUT temporale (time-gate) + updateEncoder(), che già scarta transizioni illegali.
// - ENC_TOP_LOCKOUT_US permette un timing dedicato per top spinner (ottico vs meccanico).
// =====================================================================
#if (ENABLE_ENC_LOCKOUT == 1)
extern uint32_t enc_lock_last_us[60];   // dichiarato in D_INS.ino (array globale per memoryposition 0..59)
#endif

void lettura_enc_principale() { // legge l'encoder principale (TOP SPINNER) via interrupt
  const byte chan_enc = spinner_mempos[0];

  // Lettura immediata dei 2 bit (A/B).
  // NB: qui manteniamo la polarità originale del top spinner (nessun '!').
  byte msb = (byte)digitalRead(3); // MSB = most significant bit
  byte lsb = (byte)digitalRead(2); // LSB = least significant bit
  byte encoded = (byte)((msb << 1) | lsb); // 0..3

#if (ENABLE_ENC_LOCKOUT == 1)
  // ---------------- LOCKOUT (time-gate) ----------------
  // Se arrivano interrupt troppo ravvicinati (bounce meccanico / raffiche ottico),
  // ignoriamo la generazione di step per un attimo.
  // IMPORTANTISSIMO: aggiorniamo comunque data_MA[chan_enc] (fase quadrature) per non creare salti alla ripresa.
  uint32_t now = micros();
  if ((uint32_t)(now - enc_lock_last_us[chan_enc]) < (uint32_t)ENC_TOP_LOCKOUT_US) {
    MSB[0] = msb;
    LSB[0] = lsb;
    data_MA[chan_enc] = encoded;   // 'previous state' usato da updateEncoder()
    data_LB[chan_enc] = 64;      // nessuno step
    return;
  }
#endif

  // Aggiorna i bit globali come faceva il codice originale, poi decodifica.
  MSB[0] = msb;
  LSB[0] = lsb;
  updateEncoder(chan_enc);

#if (ENABLE_ENC_LOCKOUT == 1)
  // Armiamo il lockout SOLO se updateEncoder() ha prodotto uno step valido.
  // (Se data_LB resta 64, vuol dire: nessun movimento reale.)
  if (data_LB[chan_enc] != 64) {
    enc_lock_last_us[chan_enc] = now;
  }
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void qwerty_encoderr(byte numero) {
  if (data_QW[numero] == 0 && data_LB[numero] != 64) {
    // data_VA[chan + (page)] // encoder usato per emettere messaggi qwerty
    
    #if (hid_keys == 1)
    Keyboard.press(constrain((data_LB[numero] - 63) + data_VA[numero + page], 41, 126)); 
    // Serial.println(data_LB[numero]);
    // delay(10); 
    // 41-126 PER EVITARE di triggerare tasti modificatori
    Keyboard.release(constrain((data_LB[numero] - 63) + data_VA[numero + page], 41, 126));
    #endif
  }
  
  data_QW[numero]++;
  // int qwerty_encoder = data_MI[numero];
  // qwerty_encoder = abs(qwerty_encoder -32);
  // if (data_QW[numero] > qwerty_encoder ) data_QW[numero]= 0;
  if (data_QW[numero] > abs(data_MI[numero] - 32)) {
    data_QW[numero] = 0; 
  }
  
  // attualmente si può usare speed per diminuide la velocitò di emisione dei caratteri qwerty
  // si dovrebbe fare in modo che speed si possa usare in negativo - sarebbe più intuitivo
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void updateEncoder(byte numero) {
  boolean numero2_ = 1;
  if (numero == spinner_mempos[0]) {
    numero2_ = 0;
  }
  
  byte encoded = (MSB[numero2_] << 1) | LSB[numero2_]; // converting the 2 pin value to single number
  byte sum = (data_MA[numero] << 2) | encoded; // adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) { 
    data_LB[numero]++;
  }
  
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) { 
    data_LB[numero]--; 
  }
  
  data_MA[numero] = encoded; // store this value for next time
}
//-----------
