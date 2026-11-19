/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */

// Touch LEDs use existing state; no extra persistent memory.
 
bool touch_led_active(byte n) {
  byte t = touch_mempos[n];
  if (!(control_present & (n ? CONTROL_TOUCH2 : CONTROL_TOUCH1))) return false;
  return data_LT[t] == 1 ? V_touch_regulator[n] == 0
                        : data_LT[t] == 0 && data_LB[t] == 0;
}

void touch_led_update(byte n, byte on) {
#if (shifter_active == 1)
  byte led = data_MA[touch_mempos[n]];
  if (led > 0 && led <= 32) {
    // Two touches may deliberately share one LED: releasing one keeps the other lit.
    byte other = n ^ 1;
    if (!on && data_MA[touch_mempos[other]] == led && touch_led_active(other)) on = 1;
    bit_write(1, led - 1 + page, on);
    shifter.setPin(led - 1, on);
    shifterwrite = 1;
  }
  #endif
  #if (touch_led_onboard == 1 && Touch_sensors_enable == 2)
  // This physical LED has no selectable shift-register address.
  digitalWrite(8, on || touch_led_active(n ^ 1));
  #endif
}

// Physical Touch Reset: eight non-blocking steps, one every 15 ms.
// Reuse the spinner position; the only new persistent state is 4 bytes.
static byte touch_reset_steps[2]; // low nibble: remaining steps; bit 7: page
static uint16_t touch_reset_ms;

void touch_reset_start(byte n) {
  if (!(touch_reset_steps[0] | touch_reset_steps[1]))
    touch_reset_ms = (uint16_t)millis();
  touch_reset_steps[n] = data_QW[touch_mempos[n]] == 1
    ? (byte)(8 | (page ? 128 : 0)) : 0;
}

void touch_reset_tick() {
  if (!(touch_reset_steps[0] | touch_reset_steps[1])) return;
  if (openeditor) {
    touch_reset_steps[0] = touch_reset_steps[1] = 0;
    return;
  }
  uint16_t now = (uint16_t)millis();
  if ((uint16_t)(now - touch_reset_ms) < 15) return;
  touch_reset_ms = now;
  for (byte n = 0; n < 2; ++n) {
    byte steps = touch_reset_steps[n];
    if (!steps) continue;
    const byte required = n ? CONTROL_TOUCH2 | CONTROL_SPINNER2 : CONTROL_TOUCH1 | CONTROL_SPINNER1;
    if ((control_present & required) != required) {
      touch_reset_steps[n] = 0;
      continue;
    }
    byte s = spinner_mempos[n];
    byte t = touch_mempos[n];
    byte type = data_TY[s + page];
    byte group = type & 0xF0;
    // Retouch, a new turn, a page/preset change or incompatible settings cancel.
    if (mouse_wheel_owns(s) || data_LB[t] == 0
        || (data_QW[s] == 0 && encoder_block[n] != 64)
        || !!(steps & 128) != !!page
        || data_DM[s] != 2 || (group != 176 && group != 224)) {
      touch_reset_steps[n] = 0;
      continue;
    }
    steps &= 15;
    int target = (int)data_DM[t] * 8;
    int value = encodervaluepot[n];
    value += (target - value) / steps;
    encodervaluepot[n] = value;
    touch_reset_steps[n] = steps > 1 ? touch_reset_steps[n] - 1 : 0;
    if (group == 224) {
      // PB uses real LSB/MSB: reset value 64 is exactly 8192 (0,64).
      midiSendFiltered(type, (value & 7) << 4, value >> 3, 1);
    } else {
      midiSendFiltered(type, data_VA[s + page], value >> 3, 1);
    }
  }
}


// [TOUCH_SENSORS_EXTERNAL_UNIFIED]
// External digital touch read (modules/buttons): pinset chosen by Touch_sensors_enable (2 or 3)
// - Touch_sensors_enable = 2  -> T0=7, T1=9
// - Touch_sensors_enable = 3  -> T0=7, T1=8
// Logic inversion controlled by touch_invert (0=normal, 1=invert).
#if (Touch_sensors_enable == 2 || Touch_sensors_enable == 3)

void touch_sensors(byte T_numero) { // void dedicata al touch esterno - con uso di chip dedicati, che sgravano il codice
  // SE ESISTE UN ITEM CON SETTAGGIO TOUCH ALLORA SI PROCEDE ALLA LETTURA
  if (control_present & (T_numero ? CONTROL_TOUCH2 : CONTROL_TOUCH1)) {
    const byte pin0 = 7;
    const byte pin1 = (Touch_sensors_enable == 2) ? 9 : 8;

    byte pin = (T_numero == 0) ? pin0 : pin1;

    byte raw = digitalRead(pin); // HIGH/LOW
    if (touch_invert) {
      raw = !raw; // invert if needed
    }

    averageXen[T_numero] = raw;

    // Manteniamo la logica originale: touch_execute quando data_LT è in modalità capacitive (0)
    if (data_LT[touch_mempos[T_numero]] == 0) {
      touch_execute(T_numero);
    }
    // Legacy monitoring mode 2 is retired; the editor only offers physical/virtual.
  }
}

#endif

#if (Touch_sensors_enable == 1)
void touch_sensors(byte T_numero) {
  // ===== TOUCH_SAMD_PHASE1 =====
  // Troncone 1:
  // - su AVR manteniamo il sistema storico con CapacitiveSensor
  // - su SAMD leggiamo SOLO il touch 1 (T_numero == 0) con TX=8 e RX=7
  // - il release-hold resta nel touch_execute() storico

  if (!(control_present & (T_numero ? CONTROL_TOUCH2 : CONTROL_TOUCH1))) return;

  // ------------------------------------------------------------------
  // AVR storico
  // ------------------------------------------------------------------
  #if defined(__AVR_ATmega32U4__)

    readingsXen[T_numero][indexXen] = cs_4_2[T_numero].capacitiveSensorRaw(72);
    averageXen[T_numero] = ((readingsXen[T_numero][0] + readingsXen[T_numero][1] + readingsXen[T_numero][2]) / 3);

    if (data_LT[touch_mempos[T_numero]] == 0) {
      touch_execute(T_numero);
    }

    if (averageXen[T_numero] > higher_Xen[T_numero]) {
      higher_Xen[T_numero] = averageXen[T_numero];
    }

    if (averageXen[T_numero] < lower_Xen[T_numero] && averageXen[T_numero] == readingsXen[T_numero][indexXen]) {
      lower_Xen[T_numero] = averageXen[T_numero];
    }

  #endif


   #if defined(ARDUINO_ARCH_SAMD) && !defined(__AVR_ATmega32U4__)

    // Fase 1 SAMD:
    // - stesso flusso logico del ramo AVR
    // - niente binarizzazione qui
    // - alimentiamo averageXen / higher_Xen / lower_Xen
    //   e lasciamo decidere a touch_execute()

    // Per ora gestiamo solo T_numero == 0 (touch 1: TX=8, RX=7)
    if (T_numero != 0) return;

    const byte TOUCH_TX = 8;
    const byte TOUCH_RX = 7;
    const uint16_t rawLimit = 400;

    // 1) Lettura grezza SAMD
    uint16_t rawCount = touch_raw_samd(TOUCH_TX, TOUCH_RX, rawLimit);

       // ===== TOUCH_SAMD_PREMAP_T1 =====
    // Premappatura SAMD per far "somigliare" il touch al vecchio mondo AVR,
    // senza toccare touch_execute().
    //
    // Finestra osservata nel test:
    // - riposo circa 6..8
    // - tocco circa 30..49
    //
    // Qui allarghiamo la forbice:
    //   7  -> 20
    //   45 -> 230
    //
    // Se serve piu' sensibilita', alziamo il valore minimo di ingresso
    // oppure allarghiamo ancora l'uscita.

    uint16_t rawClamp = rawCount;

    if (rawClamp < 7)  rawClamp = 7;
    if (rawClamp > 45) rawClamp = 45;

    byte rawByte = map(rawClamp, 7, 32, 10, 240);

    // 3) Stesso schema del ramo AVR: buffer circolare + media di 3
    readingsXen[T_numero][indexXen] = rawByte;

    // ===== TOUCH_AVERAGE_SWITCH =====
    // Con TOUCH_AVERAGE_ENABLE == 1:
    //   comportamento storico, media su 3 campioni.
    //
    // Con TOUCH_AVERAGE_ENABLE == 0:
    //   nessuna media, averageXen segue subito il campione corrente.
    //   Utile per testare la massima reattivita' del touch.
    #if (TOUCH_AVERAGE_ENABLE == 1)
      averageXen[T_numero] =
        (readingsXen[T_numero][0] +
         readingsXen[T_numero][1] +
         readingsXen[T_numero][2]) / 3;
#else
      averageXen[T_numero] = readingsXen[T_numero][indexXen];
    #endif

    // 4) Touch logic storica invariata
    if (data_LT[touch_mempos[T_numero]] == 0) {
      touch_execute(T_numero);
    }

    // 5) Auto-apprendimento storico dei limiti
    if (averageXen[T_numero] > higher_Xen[T_numero]) {
      higher_Xen[T_numero] = averageXen[T_numero];
    }

    if (averageXen[T_numero] < lower_Xen[T_numero] &&
        averageXen[T_numero] == readingsXen[T_numero][indexXen]) {
      lower_Xen[T_numero] = averageXen[T_numero];
    }

  #endif
}
#endif

//...............................................................................................................................

#if (Touch_sensors_enable == 1)
void touch_execute(byte numero_ex) {
  byte zero = 1;
  byte uno = 0;
  
  {        
    ///////////////////////////////////////////////////////////////////////////////////////////////// rilascio del touch                                 
    if (averageXen[numero_ex] <
        (higher_Xen[numero_ex]) 
        - map32(data_MI[touch_mempos[numero_ex]], 0, 112, 0, higher_Xen[numero_ex] - lower_Xen[numero_ex])) { // metto 112 e non 127 - per creare piu separazione tra spazio accensione e spazio spegnimento
      if (decaysensor[numero_ex] > 0) {
        decaysensor[numero_ex]--;
      }
      
      // if (numero_ex == 0) {Serial.println(decaysensor[numero_ex]); delay(50);}
      if (decaysensor[numero_ex] == 0 && data_LB[touch_mempos[numero_ex]] == uno) { /// evitare ripetizione messaggio 
        // delay(500); 
        {
          midiSendFiltered(data_TY[touch_mempos[numero_ex] + (page)], data_VA[touch_mempos[numero_ex] + (page)], 0, 1); 
          // decaysensor[numero] = 0;
          // scala_learn =0; // resetta la scala che viene memorizzata tramite learn ogni volta che si tocca la wheel
          // scala_reset = 1;

          if ((control_present & (numero_ex ? CONTROL_SPINNER2 : CONTROL_SPINNER1)) && !mouse_wheel_owns(spinner_mempos[numero_ex]) && data_TY[spinner_mempos[numero_ex] + page] < 160) { // sotto 160 sono note on e note off
            // se encoder impostato su note allora manda il note off ( al rilascio)
            // serve per scale mode, per non lasciare note accese
            midiSendTyped(data_TY[spinner_mempos[numero_ex] + page], encodervaluepot_buffer[numero_ex], 0, 1);  
          }

          touch_reset_start(numero_ex);

          data_LB[touch_mempos[numero_ex]] = zero; 
          // encled=0; 
          encoder_block[numero_ex] = 64;

          touch_led_update(numero_ex, 0);
        }
      } 
    }

    //////////////////////////////////////////////////////////////////////////////// touch premuto
    if (averageXen[numero_ex] > 
        ((higher_Xen[numero_ex])) // piu' abbasso questo valore piu' aumenta la sensibilita'
        - map32(data_MI[touch_mempos[numero_ex]], 0, 127, 0, higher_Xen[numero_ex] - lower_Xen[numero_ex])) { // la sottrazione e' proporzionale ad higher_xen

      decaysensor[numero_ex] = data_LT[general_mempos] * 2;
      
      if (data_LB[touch_mempos[numero_ex]] == zero) { // && decaysensor[numero] <= 0
        /*
        Serial.println(scala[0]);
        Serial.println(scala[1]);
        Serial.println(scala[2]);
        Serial.println(scala[3]);
        Serial.println("-");
        */
        /*
        // update_scala(0);
        for (byte i = 0; i< 12; i++) {
          Serial.print(bitRead(scala[0],i));
        }
        Serial.println(" - scala[]");

        // bitRead(data_VA[spinner_mempos[quale_spinner]+page] ,i)
        // Serial.print(data_VA[spinner_mempos[0]],BIN); Serial.println(" - data_VA[]");

        for (byte i = 0; i< 8; i++) {
          Serial.print(bitRead(data_VA[spinner_mempos[0]],i));
        }
        Serial.println(" - data_VA[]");

        for (byte i = 0; i< 8; i++) {
          // Serial.print(data_MA[spinner_mempos[0]],i));
          Serial.print(bitRead(data_MA[spinner_mempos[0]],i));
        }
        Serial.println(" - data_MA[]");

        Serial.println(" -//////////////- ");
        */
        
        { // decaysensor[numero] = decaysensor_value; 
          scala_reset = 1;
          mouse_wheel_speed_counter = 0;
          midiSendFiltered(data_TY[touch_mempos[numero_ex] + (page)], data_VA[touch_mempos[numero_ex] + (page)], 127, 1); // MIDIUSB.flush();  
          
          data_LB[touch_mempos[numero_ex]] = uno; // shifter.setAll(LOW); 
          encoder_block[numero_ex] = 64;

          touch_led_update(numero_ex, 1);
        } // opencalibration[1]=HIGH;
      }
    }
  } 
}
#endif

//...............................................................................................................................

#if (Touch_sensors_enable > 1)
void touch_execute(byte numero_ex) {
  byte zero = 1;
  byte uno = 0;
  
  {        
    //////////////////////////////////////////////////////////////// touch rilasciato
    if (averageXen[numero_ex] == 0) {
      if (decaysensor[numero_ex] > 0) {
        decaysensor[numero_ex]--;
      }

      if (data_LB[touch_mempos[numero_ex]] == uno && decaysensor[numero_ex] == 0) { /// evitare ripetizione messaggio 
        {
          midiSendFiltered(data_TY[touch_mempos[numero_ex] + (page)], data_VA[touch_mempos[numero_ex] + (page)], 0, 1); 
          // decaysensor[numero] = 0;
          
          // scala_learn =0; // resetta la scala che viene memorizzata tramite learn ogni volta che si tocca la wheel
          // scala_reset = 1;
          // Serial.println(data_TY[spinner_mempos[numero]+page]);
          // Serial.println(data_VA[touch_mempos[numero]+(page)]);

          if ((control_present & (numero_ex ? CONTROL_SPINNER2 : CONTROL_SPINNER1)) && !mouse_wheel_owns(spinner_mempos[numero_ex]) && data_TY[spinner_mempos[numero_ex] + page] < 160) { // sotto 160 sono note on e note off
            // se encoder impostato su note allora manda il note off ( al rilascio)
            // serve per scale mode, per non lasciare note accese
            midiSendTyped(data_TY[spinner_mempos[numero_ex] + page], encodervaluepot_buffer[numero_ex], 0, 1);  
          }

          touch_reset_start(numero_ex);

          data_LB[touch_mempos[numero_ex]] = zero; 
          // encled=0; 
          encoder_block[numero_ex] = 64;
          
          touch_led_update(numero_ex, 0);
        }
      }
    } 

    //////////////////////////////////////////////////////////////////////////// touch premuto
    if (averageXen[numero_ex] == 1) {
      decaysensor[numero_ex] = data_LT[general_mempos] * 2;
      
      if (data_LB[touch_mempos[numero_ex]] == zero) {
        { // decaysensor[numero] = decaysensor_value; 
          scala_reset = 1;
          mouse_wheel_speed_counter = 0;
          midiSendFiltered(data_TY[touch_mempos[numero_ex] + (page)], data_VA[touch_mempos[numero_ex] + (page)], 127, 1);  
          
          data_LB[touch_mempos[numero_ex]] = uno; // shifter.setAll(LOW); 
          encoder_block[numero_ex] = 64;

          touch_led_update(numero_ex, 1);
        } // opencalibration[1]=HIGH;
      }
      // decaysensor[numero] = decaysensor_value;
    }
  } 
}
#endif


// ===== TOUCH_SAMD_RAW_TEST_T1 =====
// Lettura grezza capacitiva per SAMD.
// TX = pin che "spinge" (emettitore)
// RX = pin che "ascolta" (ricevitore)
// Ritorna un conteggio: piu' alto o piu' basso a seconda di come reagisce il circuito.
uint16_t touch_raw_samd(byte txPin, byte rxPin, uint16_t limit)
{
  uint16_t count = 0;

  // 1) scarica tutto
  pinMode(txPin, OUTPUT);
  digitalWrite(txPin, LOW);

  pinMode(rxPin, OUTPUT);
  digitalWrite(rxPin, LOW);

  delayMicroseconds(2);

  // 2) prepara il ricevitore
  pinMode(rxPin, INPUT);

  // 3) innesca la carica
  digitalWrite(txPin, HIGH);

  // 4) misura quanto ci mette RX a vedere HIGH
  while ((digitalRead(rxPin) == LOW) && (count < limit)) {
    count++;
  }

  // 5) rimetti tutto in stato neutro
  digitalWrite(txPin, LOW);
  pinMode(rxPin, OUTPUT);
  digitalWrite(rxPin, LOW);

  return count;
}


//----------------------------------------------------------------------------------------------------------------------------------
