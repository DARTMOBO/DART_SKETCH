

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void beam() { //----------------------------------------------------- BEAM SENSOR
  #if (Distance_sensor == 1) ////------------------------
  data_LT[distance_sensor_mempos] 
    // beamValue 
    = constrain(map32(valore, minbeam, maxbeam, data_MA[chan], data_MI[chan]), data_MI[chan], data_MA[chan]);

  if (data_QW[chan] == 0) { // se impostato su POT mode
    if (valore > minbeam) { // a threshold to eliminate sensor's noise
      // Serial.println(valore); 
      // Serial.println(data_LT[distance_sensor_mempos]);
      // Serial.println("");
      midiSendFiltered(data_TY[chan + (page)], data_VA[chan + (page)], data_LT[distance_sensor_mempos], 1); // MIDIUSB.flush();
      
      #if (DMX_active == 1 && stratos == 0)
      // DmxSimple.write(data_DM[chan], data_LT[distance_sensor_mempos] * 2);
      #endif
      
      #if (shifter_active == 1 && stratos == 0)
      beamefx();
      #endif

      shifterwrite = 1;
      data_LB[distance_sensor_mempos] = 1;
    } else if (data_LB[distance_sensor_mempos] == 1) {
      #if (shifter_active == 1 && stratos == 0)
      ledrestore(page);
      shifterwrite = 1;
      #endif
      data_LB[distance_sensor_mempos] = 0;
    }
  } else if (data_QW[chan] == 1) { ///////////// impostazione note trigger
    if (valore > 300 && data_LB[chan] == 1) {
      data_LB[chan] = 0;
      midiSendFiltered((data_TY[chan + (page)]), data_VA[chan + (page)], data_LT[distance_sensor_mempos], 1); // MIDIUSB.flush();
      buttonefx = 0;
      buttonefxu = 0;
      buttonefxd = 8;
    }
    
    if (valore < 270 && data_LB[chan] == 0) {
      midiSendFiltered((data_TY[chan + (page)]), data_VA[chan + (page)], 0, 1); // MIDIUSB.flush();
      data_LB[chan] = 1;
    }
  } else { //--------------------------------------------------- beam scale
    if (beam_counter == 0) {
      at = map32(data_LT[distance_sensor_mempos], 0, 127, data_MI[chan], data_MA[chan]);
      
      if (beam_scala_buffer != at) { // se diverso dal precedente sgnale inviato - evitare doppioni e note ripetute
        // if (data_DM[contoencoder] == 2)
        // scala_learn = scala[((page/max_modifiers)*2)]; // se data_DM è su 3 allora verrà usata la scala_learn
        
        if (bitRead(scala_learn, (at) - ((at / 12) * 12)) == 1) { // scala_learn
          beam_scale_send();
        } else {
          while (bitRead(scala_learn, (at) - ((at / 12) * 12)) == 0) {
            // encoder_pot_calcolo(numero);
            at++;
            if (at > 126) {
              break;
            }
          }
          
          if (beam_scala_buffer != at) {
            beam_scale_send();
          }
        }
      }
    }

    beam_counter++;
    // if (mouse_wheel_speed_counter > abs(data_MI[contoencoder]-32)) mouse_wheel_speed_counter =0;
    if (beam_counter > 5) {
      beam_counter = 0;
    }
    // if (cycletimer = 10) { button(data_TY[distance_sensor_mempos+page],beam_scala_buffer,0 ,0); }
    // if (valore< minbeam || valore > maxbeam) if (beam_scala_buffer != at) { button(data_TY[distance_sensor_mempos+page],beam_scala_buffer,0 ,0); beam_scala_buffer = at;}
  }
  #endif ///-------------------------------
}

//////////////////////////////////////////////

void beam_scale_send() {
  #if (Distance_sensor == 1)
    #if (shifter_active == 1 && stratos == 0)
    beamefx();
    #endif
    
    button(data_TY[distance_sensor_mempos + page], beam_scala_buffer, 0, 0);
    button(data_TY[distance_sensor_mempos + page], at, 127, 1);  
    beam_scala_buffer = at;
    
    if (valore < minbeam || valore > maxbeam) {
      button(data_TY[distance_sensor_mempos + page], beam_scala_buffer, 0, 0);
    }
    // cycletimer = 0;
  #endif
}


void seq() {
  // come funziona:
  // se su editor si assegna un valore solo al numberbox "A" allora seq funziona a scatti - gli altri numberbox sono vuoti, valore zero.
  // ogni scatto è una progrssione dal basso in alto da 0 a 127.
  // ogni scatto avanza tanto quanto viene specificato su "a"
  // -----------------------
  // se si assegna un valore anche al numberbox "b" allora seq funzionerà in modo alternato
  // la sequenza alternerà il valore A e il valore B,
  // se type viene assegnato ad un messaggio NOTE allora ad ogni scatto verrà "spento" (note off) lo step precedente
  // se assegnamo un valore al numberbox C e D, allora la sequenza si estenderà anche a questi valori

#if (STAGED_BUTTON_READ_TEST == 1)

  // Stage 1: lettura debounciata pulsante (API staged) per il chan corrente
  push_buttons_lettura_stage(0);

  // Evento "pressed" pulito (edge debounciato)
  if (pb_pressed()) { ///// button pushed (debounced)

    // Serial.println("push");
    // Serial.println(valore);

    if (data_MA[chan] > 0) {
      switch (data_QW[chan]) { // data_QW[chan] usato come contatore > 159 sono cc
        case 0:
          // Serial.println(data_MI[chan]);
          if (data_TY[chan + (page)] < 160) { // se sono note
            midiSendFiltered(data_TY[chan + (page)], data_MI[chan], 127, 1); // invio accensione

            if (data_LT[chan] > 0) { // invio spegnimento
              midiSendFiltered(data_TY[chan + (page)], data_LT[chan], 0, 1);
            } else if (data_DM[chan] > 0) {
              midiSendFiltered(data_TY[chan + (page)], data_DM[chan], 0, 1);
            } else {
              midiSendFiltered(data_TY[chan + (page)], data_MA[chan], 0, 1);
            }
          } else { // se sono cc
            midiSendFiltered(data_TY[chan + (page)], data_VA[chan + (page)], data_MI[chan], 1); // l'ultimo "1" è relativo al filtro anti doppioni
          }

          data_QW[chan]++;
          break;

        case 1:
          // Serial.println(data_MA[chan]);
          if (data_TY[chan + (page)] < 160) {
            midiSendFiltered(data_TY[chan + (page)], data_MA[chan], 127, 1);
            midiSendFiltered(data_TY[chan + (page)], data_MI[chan], 0, 1);
          } else {
            midiSendFiltered(data_TY[chan + (page)], data_VA[chan + (page)], data_MA[chan], 1);
          }

          if (data_DM[chan] > 0) {
            data_QW[chan]++;
          } else {
            data_QW[chan] = 0;
          }
          break;

        case 2:
          // Serial.println(data_DM[chan]);
          if (data_TY[chan + (page)] < 160) {
            midiSendFiltered(data_TY[chan + (page)], data_DM[chan], 127, 1);
            midiSendFiltered(data_TY[chan + (page)], data_MA[chan], 0, 1);
          } else {
            midiSendFiltered(data_TY[chan + (page)], data_VA[chan + (page)], data_DM[chan], 1);
          }

          if (data_LT[chan] > 0) {
            data_QW[chan]++;
          } else {
            data_QW[chan] = 0;
          }
          break;

        case 3:
          // Serial.println(data_LT[chan]);
          if (data_TY[chan + (page)] < 160) {
            midiSendFiltered(data_TY[chan + (page)], data_LT[chan], 127, 1);
            midiSendFiltered(data_TY[chan + (page)], data_DM[chan], 0, 1);
          } else {
            midiSendFiltered(data_TY[chan + (page)], data_VA[chan + (page)], data_LT[chan], 1);
          }

          data_QW[chan] = 0;
          break;
      }
    } else { // progressione a step - data_MI decide la grandezza dello step
      data_QW[chan] = data_QW[chan] + data_MI[chan];
      if (data_QW[chan] > 127) {
        data_QW[chan] = 0;
      }

      // Serial.println(data_QW[chan]);
      midiSendFiltered(data_TY[chan + (page)], data_VA[chan + (page)], data_QW[chan], 1);
    }
  }

  // In modalità staged NON gestiamo qui "release":
  // lo arming/debounce viene gestito da push_buttons_lettura_stage().
  // Se un domani ti serve un evento release per la SEQ, puoi usare pb_released().

#else

  // ---- LEGACY (identica alla tua versione) ----

  if (valore < lower_val && data_LB[chan] == 1) { ///// button pushed
    data_LB[chan] = 0;

    // Serial.println("push");
    // Serial.println(valore);

    if (data_MA[chan] > 0) {
      switch (data_QW[chan]) { // data_QW[chan] usato come contatore > 159 sono cc
        case 0:
          // Serial.println(data_MI[chan]);
          if (data_TY[chan + (page)] < 160) { // se sono note
            midiSendFiltered(data_TY[chan + (page)], data_MI[chan], 127, 1); // invio accensione

            if (data_LT[chan] > 0) { // invio spegnimento
              midiSendFiltered(data_TY[chan + (page)], data_LT[chan], 0, 1);
            } else if (data_DM[chan] > 0) {
              midiSendFiltered(data_TY[chan + (page)], data_DM[chan], 0, 1);
            } else {
              midiSendFiltered(data_TY[chan + (page)], data_MA[chan], 0, 1);
            }
          } else { // se sono cc
            midiSendFiltered(data_TY[chan + (page)], data_VA[chan + (page)], data_MI[chan], 1); // l'ultimo "1" è relativo al filtro anti doppioni
          }

          data_QW[chan]++;
          break;

        case 1:
          // Serial.println(data_MA[chan]);
          if (data_TY[chan + (page)] < 160) {
            midiSendFiltered(data_TY[chan + (page)], data_MA[chan], 127, 1);
            midiSendFiltered(data_TY[chan + (page)], data_MI[chan], 0, 1);
          } else {
            midiSendFiltered(data_TY[chan + (page)], data_VA[chan + (page)], data_MA[chan], 1);
          }

          if (data_DM[chan] > 0) {
            data_QW[chan]++;
          } else {
            data_QW[chan] = 0;
          }
          break;

        case 2:
          // Serial.println(data_DM[chan]);
          if (data_TY[chan + (page)] < 160) {
            midiSendFiltered(data_TY[chan + (page)], data_DM[chan], 127, 1);
            midiSendFiltered(data_TY[chan + (page)], data_MA[chan], 0, 1);
          } else {
            midiSendFiltered(data_TY[chan + (page)], data_VA[chan + (page)], data_DM[chan], 1);
          }

          if (data_LT[chan] > 0) {
            data_QW[chan]++;
          } else {
            data_QW[chan] = 0;
          }
          break;

        case 3:
          // Serial.println(data_LT[chan]);
          if (data_TY[chan + (page)] < 160) {
            midiSendFiltered(data_TY[chan + (page)], data_LT[chan], 127, 1);
            midiSendFiltered(data_TY[chan + (page)], data_DM[chan], 0, 1);
          } else {
            midiSendFiltered(data_TY[chan + (page)], data_VA[chan + (page)], data_LT[chan], 1);
          }

          data_QW[chan] = 0;
          break;
      }
    } else { // progressione a step - data_MI decide la grandezza dello step
      data_QW[chan] = data_QW[chan] + data_MI[chan];
      if (data_QW[chan] > 127) {
        data_QW[chan] = 0;
      }

      // Serial.println(data_QW[chan]);
      midiSendFiltered(data_TY[chan + (page)], data_VA[chan + (page)], data_QW[chan], 1);
    }
  }

  if (valore > upper_val && data_LB[chan] == 0) { ///// button released - non succede niente
    data_LB[chan] = 1;
    // Serial.println("release");
    // Serial.println(valore);
  }

#endif
}


#if (stratos == 0)
  #if (Piezo_pads == 1)
  void piezo_pads() { 
    // i pads vengono attivati da GENERAL SETTINGS, il messaggio MIDI emesso viene descritto nell'ITEM PADS
    // attivando i pads viene disattivata la resistenza pullup sul sesto input analogico A5
    // 
    setPlexer(padNum * 2); 

    // padVal = analogRead(5);
    padVal = analogRead_1024(5); 
    // la lettura viene ripetuta due volte per lasciare il tempo di scaricare corrente residua al pad
    // questa è solo una congettura... sto ancora facendo prove e test

    // if (padNum == 0) Serial.println(padVal);

    if (padVal > 2 && padDecay[padNum] == 0) {
      for (byte pad = 0; pad < 6; pad++) {
        // setPlexer(padNum*2);
        padVal2 = analogRead_1024(5);
        if (padVal2 > padVal) {
          padVal = padVal2;
        }
      }
      
      padDecay[padNum] = 14 // + (padVal / 40)
      ;
      
      // midiSendRaw(data_TY[PADS_mempos+(page)],data_VA[PADS_mempos+(page)]+(padNum*2),(map32(padVal,0,512,0,127)));
      midiSendRaw(data_TY[PADS_mempos + (page)], data_VA[PADS_mempos + (page)] + (padNum * 2), constrain(padVal, 0, 127));

      buttonefx = 0;
      // buttonefxu = 5;
      buttonefxd = constrain((padVal / 128), 0, 3);
    }

    { 
      padNum++;
      if (padNum > 3) {
        padNum = 0;
      }
      
      {
        if (padDecay[padNum] > 0) { 
          padDecay[padNum] = padDecay[padNum] - 1; 
          if (padDecay[padNum] == 1) {
            midiSendRaw(data_TY[PADS_mempos + (page)], data_VA[PADS_mempos + (page)] + (padNum * 2), 0);
          }
        }
      } 
    }
  }
  #endif
#endif

#if (Extraplexer == 1)
void Extraplexer_read() {
  if (data_MI[general_mempos] > 0) { // vedi se il plexer EXTRA è attivato // 1= extra plexer attivato - gli input della dartmobo passano da 48 a 56! adesso il pin che prima veniva dedicato al touch sensor 2 viene dedicato alla letture dell'extra plexer - nota: usant touch ic's esterni è possibile avere entrambe le cose.

    #if defined(ARDUINO_ARCH_SAMD)
      pinMode(9, INPUT_PULLUP);
      #if (Dummy_read_digi == 1) // stessa tecnica del percorso ain() normale su M0
        valore = digitalRead(9) ? 1023 : 0;
        delayMicroseconds(10);
      #endif
      valore = digitalRead(9) ? 1023 : 0;

  
    #endif

    #if defined (__AVR_ATmega32U4__)
      #if (pullups_active == 1)
        digitalWrite(9, HIGH);
      #endif
      // valore = analogRead_1024(9);
      // valore = analogRead(9);
      valore = (digitalRead(9)) << 10;
    #endif

    #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
      #if (pullups_active == 1)
        digitalWrite(9, HIGH);
      #endif
      // valore = 1000*(!digitalRead(9));
      valore = (digitalRead(9)) << 10;
    #endif

    chan = channel + 48;
    // if (data_MODE[chan] == 1 && data_VA[chan] == 88) {Serial.println (valore); delay(100);}
    ain_nucleo();
  }
}
#endif
