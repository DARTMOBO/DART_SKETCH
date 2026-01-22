
 /*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */
#if Scene
void scene_morph_encsc(byte enc_chan);
#endif

void encoder(byte numero) 
{
  //  NUMERO  corrisponde alla chan/memoryposition
  // lastbutton[] contiene la lettura dell encoder 64 = fermo, 63 = -, 65 = + 
  // modetable[] mi dice se si tratta di encoder spinner  (mode 21 e 22 su editor) o di encoder generico (mode 19 su editor)
  // dmxtable[general_mempos]  vedi se gli spinner sono attivati e quali 0-nessuno 1-top 2-top+side 3 top+side+generico
  // dmxtable[numero]   // la dmxtable definisce la modalita  - 0 e 1 endless - 2 pot    -  3  scale
  // qwertyvalue[numero] settaggio del touch stop
  // minvalue: speed e inversione - il valore va da 0 a 64  // 32 sarebe lo zero.
  // maxvalue: usato per trasferimento scale
  // lightable[numero] // usato come contatore per la modalità 63-65 che oltre allo scatto avanti/indietro fornisce anche la velocità
  // typetable[] oltre a specificare se si tratta di note o cc - può fare accedere l'encoder alla modalità qwerty 
  // dmxtable[mouse_mempos] emulazione mouse attiva
  // valuetable[mouse_mempos+page] // scelta dell encoder che emula mousewheel

  if (lastbutton[numero] != 64)
  { 

    // ============================================================
    // CTRL-F: ENCSC_TYPE244_MARKER
    // NUOVA REGOLA: ENCSC/SCENE-MORPH via TYPE marker (pseudomidi 244)
    // - page-aware: typetable[] contiene PAGE1 e PAGE2, quindi usiamo +page
    // - quando il marker è attivo, NON eseguiamo logica encoder normale
    // - consumiamo l’impulso (63/65) riportando lastbutton a 64
    // ============================================================
   #if Scene
    if (typetable[numero + page] == 244)
    {
      scene_morph_encsc(numero);   // funzione in D_scene.ino
      lastbutton[numero] = 64;
      return;
    }
#endif

    
    byte tocco;  
    cycletimer = 0;

    if (modetable[numero] > 20) // spinner (21/22)
    {
      numero2h = boolean(modetable[numero] - 21);   
      tocco = lastbutton[touch_mempos[numero2h]];   // touch associato allo spinner 1/2

      encoder_block[numero2h] = constrain(encoder_block[numero2h] + (lastbutton[numero] - 64), 20, 235);

      if (V_touch_regulator[numero2h] == 1 && lightable[touch_mempos[numero2h]] == 1)
      { 
        midiOut(typetable[touch_mempos[numero2h] + page] + 1,
                valuetable[touch_mempos[numero2h] + page],
                127); // invio del virtual touch 

        #if (shifter_active == 1 && stratos == 0)
        shifter.setPin(4 + numero2h, HIGH); (shifterwrite = 1);
        #endif
      }

      V_touch_regulator[numero2h] = 0;
    } 
    else 
    {
      // CTRL-F: GENERIC_ENCODER_TOUCH_IMMUNE
      // Encoder generici (mode 19): immuni ai touch sensor.
      // Non leggiamo touch_mempos[] e non usiamo touch-stop per loro.
      numero2h = 0;
      tocco = 0;
    }

    //-------------------------------------------------------------------------------------------------------------------------------

    if (dmxtable[general_mempos] >= numero2h)
    {
      if (dmxtable[numero] > 1)  // 2 pot - 3 scale
      {  
        // Touch-stop SOLO per spinner (tocco = stato touch). Per generici tocco=0 => sempre "touch non premuto".
        if (qwertyvalue[numero] == 0) 
        { 
          encoder_pot_mode(numero); 
        }
        else 
        {
          if (tocco == 0) encoder_pot_mode(numero); // prima era lastbutton[touch_mempos[numero2h]]==0
        }
      }
      else // dmxtable 0/1 endless
      {
        // CTRL-F: ENCODER_SIGNED_SPEED_REFACTOR
        const int8_t speed = (int8_t)minvalue[numero] - 32;   // range -32..+32
        const bool inverted = (speed < 0);

        if (dmxtable[numero] == 0)      // endless 63/65 (accelerato)
        {
          if (typetable[numero + page] <= 191)
          {
            // CTRL-F: FIX_INV_ACCEL_63_65
            // Niente map32() qui: l'inversione è gestita da dir + speedAbs (evita doppia inversione).
            const int8_t  dir      = inverted ? -1 : 1;
            const uint8_t speedAbs = (uint8_t)(inverted ? -speed : speed);
            const uint8_t lbAccel  = lastbutton[numero];

            #if (stratos == 0)
            if (lightable[numero] == 16)
            #endif
            #if (stratos == 1)
            if (lightable[numero] == 250)
            #endif
            {
              button(typetable[numero + page] + boolean(qwertyvalue[numero]) * tocco,
                     valuetable[numero + page],
                     constrain(
                       (int)lbAccel + ((int)(lbAccel - 64) * (int)speedAbs * (int)dir),
                       0, 127),
                     0);

              lightable[numero] = 0;
              encled[0] = encled[0] + (-(lastbutton[numero] - 64)) * (int)speed;
              lastbutton[numero] = 64;
            }
            else
            {
              lightable[numero]++;
            }
          }
          else
          {
            qwerty_encoderr(numero);
            lastbutton[numero] = 64;
          }
        }
        else // dmxtable == 1  (endless 0..127)
        {
          if ( (qwertyvalue[numero] ^ tocco) != 3 )
          {
            // CTRL-F: FIX_INV_ENDLESS_DMX1
            const int8_t dir = inverted ? -1 : 1;

            button(typetable[numero + (page)] + boolean(qwertyvalue[numero]) * tocco,
                   valuetable[numero + (page)],
                   constrain((-(lastbutton[numero] - 64) * dir) * 127, 0, 127),
                   2);

            encled[0] = encled[0] + (-(lastbutton[numero] - 64)) * (int)speed;
          }
        }

        #if (shifter_active == 1 && stratos == 0)
        if ( (qwertyvalue[numero] ^ tocco) != 3 )
          led_enc_exe();
        #endif

        #if (Matrix_Pads > 0)
        if ( (qwertyvalue[numero] ^ tocco) != 3 )
          led_enc_exe_matrix();
        #endif
      }

      //-------------------------------------------------------------------------------------------------------------------------------

      #if defined (__AVR_ATmega32U4__)  
      {  // attivazione mousewheel
        #if (stratos == 1)
        if (valuetable[mouse_mempos + page] == numero)
        #endif

        #if (stratos == 0)
        if (remapper(valuetable[mouse_mempos + page] - 1) == numero)
        #endif
        {
          #if (hid_mouse == 1)
          if (mouse_wheel_speed_counter == 0)
            Mouse.move(0, 0, (-(lastbutton[numero] - 64)) * (constrain(minvalue[numero] - 32, -1, 1)));
          #endif

          mouse_wheel_speed_counter++;
          if (mouse_wheel_speed_counter == map32(abs(minvalue[numero] - 32), 0, 32, 32, 1))
          {
            mouse_wheel_speed_counter = 0;
          }
        }
      }
      #endif

      if (dmxtable[numero] > 0) // in modalità relativa con accelerazione (63-65) non devo resettare lastbutton...
        lastbutton[numero] = 64;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void encoder_pot_mode (byte numero)
{
  int valuepot;
  encoder_pot_calcolo(numero, minvalue[numero]);  // canale e velocità/speed

  if (modetable[numero] > 19) {
    valuepot = encodervaluepot[numero2h];
  } // spin1 / spin2
  else {
    valuepot = lightable[numero] * 4;             // encoder generico
  }

  // ------------------------------------------------
  // fin qui abbiamo calcolato la posizione (0..1023 circa)
  // ------------------------------------------------

  // OUT 0..127 (unificato)
  byte out = (byte)constrain(valuepot / 8, 0, 127);

  if (typetable[numero + page] > 223)  // pitch bend
  {
    noteOn(typetable[numero + page],
           (valuepot - ((valuepot / 8) * 8)) * 16,
           valuepot / 8, 1);
  }
  else if (typetable[numero + page] > 159) // CC/PC/AT (sotto 160 sono NOTE)
  {
    // --- ANCHOR: PC_TO_CC_IN_POT_EMU ---
    byte t = typetable[numero + page];          // status (0x90..)
    byte group = (t - 144) / 16;                // 0=NOTE,1=AT,2=CC,3=PC,...
#if Scene
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
#if Scene
    {
      byte si = convoy_find_subjectIndex(numero);  // numero = memoryposition dell'encoder
      if (si != 255) {
        convoy_set(si, out);
        convoy_commit();
        return;
      }
    }
#endif
    // comportamento normale (non subject): invio diretto
    button(t, valuetable[numero + page], out, 1);
  }
  else
  {
    // NOTE => modalità SCALE (solo spinner)
    if (modetable[numero] > 19)
    {
      at = encodervaluepot[numero2h] / 8;

      if (encodervaluepot_buffer[numero2h] != at)
      {
        if (scala[numero2h + ((boolean(page)) * 2)] != 0)
          scala_learn = scala[numero2h + ((boolean(page)) * 2)];

        {
          if (scala_learn != 0)
          {
            if (encoder_block[numero2h] < 50 || encoder_block[numero2h] > 78)
            {
              encoder_block[numero2h] = 220;

              if (bitRead(scala_learn, (at) - ((at / 12) * 12)) == 1)
              {
                button(typetable[numero + page], encodervaluepot_buffer[numero2h], 0, 0);
                button(typetable[numero + page], at, 127, 1);
                encodervaluepot_buffer[numero2h] = at;
              }
              else
              {
                while (bitRead(scala_learn, (at) - ((at / 12) * 12)) == 0)
                {
                  encoder_pot_calcolo(numero, constrain(minvalue[numero], 31, 33));
                  at = encodervaluepot[numero2h] / 8;

                  if (at <= 0 || at >= 127) { mouse_wheel_speed_counter = 0; break; }
                }

                {
                  button(typetable[numero + page], encodervaluepot_buffer[numero2h], 0, 0);
                  button(typetable[numero + page], at, 127, 1);
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

void encoder_pot_calcolo(byte chan_, byte moltiplicatore)   
 {
  // serve per calcolare la posizione del pot virtuale - encoder in modalità pot
  // la posizione del pot viene memorizzata su encodervaluepot[] per gli spinner e su lightable[] per gli encoders generici
  
  moltiplicatore = 64 - moltiplicatore;  // commentare questa riga per invertire l'avanzamento del pot emulato
  
   if (modetable[chan_] > 19 )        // se si tratta di uno spinner - useremo encodervaluepot[] per tenere in memoria la posizione del POT VIRTUALE
                                      // 19 sarebbe l'identificativo per l'encoder 
                                      // 21 e 22 per gli spinner

{

 if (mouse_wheel_speed_counter == 0) // 
  { /////////////------------- /////////////-------------/////////////-------------/////////////-------------/////////////-------------
if (dmxtable[chan_] == 2) 
{
 encodervaluepot[numero2h]                                                // contiene il valore-posizione attuale dello spinner 1 o 2       
= (constrain(
  encodervaluepot[numero2h] - 
(-(lastbutton[chan_]-64))          // -1 o 1
*
(moltiplicatore-32)                // moltiplicatore sarebbe mivalue[numero] che va da 0 a 64
,
0, 1023));                         // 

 // Serial.println(moltiplicatore-32);
 // Serial.println(encodervaluepot[numero2h]);
 // Serial.println(encoder_block[numero2h]);
 // Serial.println("--");
// spegazione: si aggiunge il valore speed, positio o negativo a seconda del verso.
}
else
{
  encodervaluepot[numero2h]                                               // modalità virtualpot RAMP - senza limiti superiore e inferiore.      
= encodervaluepot[numero2h] - (-(lastbutton[chan_]-64))*(moltiplicatore-32);
if (encodervaluepot[numero2h] <0 ) encodervaluepot[numero2h] = 1023;
 else if (encodervaluepot[numero2h] >1023 ) encodervaluepot[numero2h] = 0;
  }
} /////////////-------------/////////////-------------/////////////-------------/////////////-------------/////////////-------------

if (typetable[chan_+page] > 159 ) // se type non è un NOTE
{mouse_wheel_speed_counter = 0 ;} // se sono CC o altro mouse_wheel_speed_counter rimane sempre zero - nessun rallentamento.
else 

mouse_wheel_speed_counter++;
//if (mouse_wheel_speed_counter > (minvalue[chan_]-32)) {mouse_wheel_speed_counter =0;} // in questo modo si ha un rallentamento della mousewheel aumnetando 
                                                                                        // zero = veloce
                                                                                        // 32 = rallentato 
                                                                                        // minvalue qui usata come speed / rallentatore.

  // if (mouse_wheel_speed_counter > constrain(abs(minvalue[chan_]-32),1,32)  ) {mouse_wheel_speed_counter =0;}
   //  if (mouse_wheel_speed_counter > abs(minvalue[chan_]-32)  ) {mouse_wheel_speed_counter =0;}
if (mouse_wheel_speed_counter > map32(abs(minvalue[chan_]-32),0,32,32,0)  ) {mouse_wheel_speed_counter =0;}

 }
 
 else // se si tratta di encoder generico - useremo lightable[] per tenere in memoria la posizione del POT VIRTUALE
 {

if (dmxtable[chan_] == 2) 
{
  lightable[chan_]                                                                       
                                                                        // lightable  contiene il valore-posizione attuale dell'encoder generico
= (constrain( 
  lightable[chan_]  
  - (-(lastbutton[chan_]-64))
  *
  (moltiplicatore-32)
  ,
 0, 255));  
}

else
{
  lightable[chan_]                                                     // RAMP - lightable contiene il valore-posizione attuale dell'encoder generico 
                                                                       //- qui in modalità illimitata - quando si va oltre 255 si torna a zero.
=  lightable[chan_]  - (lastbutton[chan_]-64)*(-(moltiplicatore-32));     
}


  }
 
 }






 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void lettura_enc_principale()   // legge l'encoder principale , collegato ad pins con capacità interrupt.
{
   MSB[0] = digitalRead(3); //MSB = most significant bit
   LSB[0] = digitalRead(2); //LSB = least significant bit
   updateEncoder(encoder_mempos[0]);
 //  lastbutton[encoder_mempos[1]]=64;
 //  updateEncoder_main();
}

 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void qwerty_encoderr (byte numero)
{
  
  
        if (qwertyvalue[numero] == 0 && lastbutton[numero] != 64 )
        {
        // valuetable[chan + (page)]                                  // encoder usato per emettere messaggi  qwerty
        
         #if (hid_keys == 1)
        Keyboard.press(constrain((lastbutton[numero]-63)+valuetable[numero+page], 41,126)); 
      //Serial.println(lastbutton[numero]);
        // delay(10); 
      //41-126 PER EVITARE di triggerare tasti modificatori
        Keyboard.release(constrain((lastbutton[numero]-63)+valuetable[numero+page],41,126));
        #endif
    
      }
        qwertyvalue[numero]++;
      //  int qwerty_encoder = minvalue[numero];
      //  qwerty_encoder = abs(qwerty_encoder -32);
       //  if (qwertyvalue[numero] > qwerty_encoder ) qwertyvalue[numero]= 0;
       if (qwertyvalue[numero] > abs(minvalue[numero]-32) ) qwertyvalue[numero]= 0; 
       
       // attualmente si può usare speed per diminuide la velocitò di emisione dei caratteri qwerty
       // si dovrebbe fare in modo che speed si possa usare in negativo - sarebbe più intuitivo
       } 

 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void updateEncoder(byte numero)
{
  boolean numero2_ =1;
  if (numero == encoder_mempos[0]) numero2_=0; 
 
  
    byte encoded = (MSB[numero2_] << 1) |LSB[numero2_]; //converting the 2 pin value to single number
    byte sum  = (maxvalue[numero] << 2) | encoded; //adding it to the previous encoded value

    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) { 
    lastbutton[numero]++;
    }
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) { 
    lastbutton[numero]--; 
    }
    maxvalue[numero]= encoded; //store this value for next time
}
