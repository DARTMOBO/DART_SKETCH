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


//---------------------------------------------------------------------------------------------------------------
  void push_buttons(byte velo)
  {
      // gestione pulsanti
  // la variabile "velo" ha valore 0 o 1 - se impstata su 1 attiva la lettura della velocity
  // lastbutton[] gestisce il debouncing del pulsante 
  //          - si fa riferimento a lastbutton_debounce che è una variabile fissa 
  //            con valore diverso secondo il circuito del controller (normale o stratos - 10 o 40)
  //            in partica serve come temporizzatore per evitare messaggi ripetuti in caso di pulsanti scadenti che non fanno bene contatto
  //
  // lower_val e upper_val - sono due variabili fisse che indicano i valori sotto e soprai quali si considera che un pulsante sia premuto
  // 

  
    {


   // if (valore < lower_val - (velo *min)    )                         ///// button pushed        // con questa formula più è alto min , meno il pad diventa sensibile
   int lower = lower_val + (velo *(minvalue[chan]*3));
    if (valore < lower   )     
        {
     
      if (lastbutton[chan] == lastbutton_debounce)
      {
       /*
       Serial.print(modetable[19]); Serial.println(" - modetable");
        Serial.print(dmxtable[19]); Serial.println(" - dmxtable - modalità endless / pot");
        Serial.print(qwertyvalue[19]); Serial.println(" - touchstop");
        Serial.print(minvalue[19]-32); Serial.println(" - speed");
*/


      //   Serial.println(chan);
        
      if (modetable[chan] >= 3 && modetable[chan] != 27) offgroup(chan,1);      // da 3 in poi ci sono i toggle groups e radio groups
      
            if (bit_read(4,page+chan)==0)              // 4 = toggletable // something happens only if the button is off in the toggletable
            { 
               #if (ENABLE_AUTODETECT == 1)
               if ( eeprom_preset_active == 0 ) dmxtable[chan]++;    // autodetect_dmx
               #endif
               
        
     //  Serial.println(mouse_mempos);
            #if defined (__AVR_ATmega32U4__)  
            HOT_keys(chan,1);    
            #endif  
             
             
            
         #if (Scale ==1 )
            if (typetable[chan+page] < 160) scale_learn(valuetable[chan+page]);   // sotto 160 ci sono note on e off 
         #endif
             
            #if (shifter_active == 1 && stratos == 0)    
            ledControl(chan, 1); // void ledControl (byte chann, byte stat)   // stat significa status 1 = acceso 0 = spento
            ledrestore(page); // perchè?? non è pesante?
            #endif
                
            #if (Matrix_Pads == 1 )
            single_h(matrix_remap[chan],lightable[chan],1,1);  // pad in negativo (sprite invertito)
            ledControl_matrix(chan,1);
            // avvia effetto a croce sulle matrici (vedi matrixbuttonledefx in D_mtrx.ino)
            
     #if (MATRIX_CROSS_FX == 1)
  // CTRL-F: CROSS_TRIGGER_MASK_AAFF
  // canali autorizzati a triggerare la croce (0..15)
  const uint16_t CROSS_MASK = 0xAAFF;

  // evita OOB su matrix_remap[] e filtra i canali
  if (chan < 16 && (CROSS_MASK & (uint16_t)(1U << chan))) {
    buttonefxd = matrix_remap[chan];
    buttonefx  = 1;
    cycletimer = 0;
  }
#endif
 
            #endif

             #if (Matrix_Pads == 2 )
            single_h(matrix_remap[chan-16],lightable[chan],1,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo, send)
          
          
            #endif
    
            
          
       
          if (modetable[chan] > 6) bit_write(4,chan+page,true);   // r-groups // ricordare che: 4 = togletable
           else if (modetable[chan] >= 2   )  bit_write(4,chan+page,!bit_read(4,page+chan));     

          if ( velo == 0 )    outnucleo (1,chan);
          else {
           
              
            #if Velo_pads_debug
            debugPadVelocityFilm();   // <<< qui parte il "filmato" dopo il gate
            #endif
         
         //  int velopush =  constrain(analogRead_1024(plexer),60,255);  //Serial.println (velopush);//  delay(2);
           int velopush =  constrain(analogRead_1024(plexer),160,lower);
            button(typetable[chan+page],valuetable[chan+page],map32(velopush,160,lower,127,1),1);
            
     //  Serial.println(velopush);
               }

         // outnucleo (1,chan);
          
            } 
            
            else     /// se il pulsante è acceso nella toggletable
            
            if (modetable[chan] <7  || modetable[chan] == 27 )
            {
               #if defined (__AVR_ATmega32U4__)  
            HOT_keys(chan,0);    
            #endif  
             
               #if (shifter_active == 1 && stratos == 0)    
            ledControl(chan, 0);
            ledrestore(page);
            #endif
         

           #if (Matrix_Pads == 1 )
            ledControl_matrix(chan,0);
           single_h(matrix_remap[chan],lightable[chan],0,1);
        //   single_h(pgm_read_byte(matrix_remap + chan-16),dmxtable[chan],0); //  utilizzo una lookup table memorizzata su flash con PROGMEM
           #endif

           #if (Matrix_Pads == 2 )
           single_h(matrix_remap[chan-16],lightable[chan],0,1);
        //   single_h(pgm_read_byte(matrix_remap + chan-16),dmxtable[chan],0); //  utilizzo una lookup table memorizzata su flash con PROGMEM
           #endif
             
            bit_write(4,chan+page,!bit_read(4,page+chan)); 

            outnucleo (0,chan);
          
              }

              #if (shifter_active == 1)
      cycletimer = 0;   
              #endif
      shifterwrite = 1;
    }
     if (lastbutton[chan] > 0 ) lastbutton[chan] =0;
        }

    // --------------------------------------------------------------------------
    if (valore > upper_val //&& lastbutton[chan] == 0
    )                                                                                     ///// button released
    {
    
      if (lastbutton[chan] == 0)
   {
     //  if (page == 0) 
      // Serial.println("---- released");
        //  Serial.println(valore);
      {
        if (modetable[chan] >= 2  && modetable[chan] < 11
        ||  modetable[chan] == 27 && maxvalue[chan] > 0 // toggle per i velo pads
        ) // se il pulsante = toggle o t-group o r-group
        
        {
         if( modetable[chan] < 7  ||  modetable[chan] == 27)  {  //                                                      7 8 9 10 sono RADIO group
                 
        }   
           
        }
             else{
        //     if ( modetable[chan] < 5 ) // se il pulsante NON e' in toggle o in uno dei gruppi toggle
             
             {  
         
        //  if (bit_read(4,page+chan) == 1) 
          {  
            #if defined (__AVR_ATmega32U4__)  
              HOT_keys(chan,0);
              #endif
            
             #if (shifter_active == 1 && stratos == 0)
              if (bit_read(4,page+chan) == 0)  
              ledControl(chan,0);  
              #endif

               #if (Matrix_Pads == 1 )
                 ledControl_matrix(chan,0);
               single_h(matrix_remap[chan],lightable[chan],0,1);
             //   bit_write(1,(lightable[chan]-1)+page,0); 
             // single_h(pgm_read_byte(matrix_remap + chan-16),dmxtable[chan],0); //  utilizzo una lookup table memorizzata su flash con PROGMEM
              #endif

              #if (Matrix_Pads == 2 )
               single_h(matrix_remap[chan-16],lightable[chan],0,1);
             //   bit_write(1,(lightable[chan]-1)+page,0); 
             // single_h(pgm_read_byte(matrix_remap + chan-16),dmxtable[chan],0); //  utilizzo una lookup table memorizzata su flash con PROGMEM
              #endif
              
              }
               bit_write(4,chan+page,0);
              outnucleo (0,chan);
             }
            
             }
                    }

   
      shifterwrite = 1;
    }
     if (lastbutton[chan] <  lastbutton_debounce) lastbutton[chan]++;
    }
   //  lastbutton[chan] = valore / 4;
        
  }  // PUSH BUTTON SECTION END
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ============================================================
// CTRL-F: CONVOY_PROTOS_POTS
byte convoy_find_subjectIndex(byte mem_chan);

void convoy_set(byte subjectIndex, byte value);
void convoy_commit();
// ============================================================



// CTRL-F: POT_TAKEOVER_WINDOW
// Finestra di aggancio in unità "raw" di pot_confronto (= abs(lastbutton*4 - valore))
// 12 = circa 3 step (perché lastbutton lavora a scaglioni da 4)
#define POT_TAKEOVER_WINDOW 24

void pots ()
{
  // ============================================================
  // CTRL-F: POTS_SCENE244_EARLY
  // SCENE CONTROL (marker raw=244 / 0xF4 undefined MIDI)
  //
  // Nota pratica:
  // - Il pot takeover (allowWrite=0) può bloccare il blocco "pot mosso",
  //   e quindi impedire l'esecuzione di scene_control_pot() quando il REC
  //   è armato. Il recording, invece, scatta proprio *dentro* scene_control_pot().
  // - Qui bypassiamo takeover, curve e invio MIDI: per gli item scene-control
  //   vogliamo solo la logica max-wins (e l'eventuale recording), basata
  //   sulla lettura fisica del pot.
  //
  // NB: uso chan+page per coerenza con il resto del codice (type page-dependent).
  // Se vuoi il scene-control sempre "page 0", qui basta sostituire con typetable[chan].
  // ============================================================
 
  #if Scene
  if (typetable[chan + page] == 244) { scene_control_pot(); return; }
#endif

 byte diff = 11; // #mod_finestra
 // CTRL-F: POTS_LB_INDEX
// page vale 0 o 60 (max_modifiers). Con /60 otteniamo 0 oppure 1.
// Usiamo 64 come stride per NON pestare i primi 0..63.
#if ENABLE_POT_TAKEOVER
byte lb = chan + (byte)((page / max_modifiers) * 64);
#else
byte lb = chan; // legacy: no split
#endif

/*
 if (qwertyvalue[chan] >0 // && modetable[chan] <16
 ) diff = 3; // #mod_finestra
 */
  {
    int pot_confronto = abs((lastbutton[lb] * 4)  - valore);

    
    // CTRL-F: POTS_TAKEOVER_GATE
byte allowWrite = 1; // 1=può scrivere, 0=quarantena takeover

#if ENABLE_POT_TAKEOVER
if (bit_read(5, chan + page) == 1) // ARMED su questa pagina
{
  if (pot_confronto <= POT_TAKEOVER_WINDOW)
  {
    // ============================================================
    // CTRL-F: TAKEOVER_SILENT_CATCH
    // CATCH SILENZIOSO:
    // Quando il software (scene/morph/midi) raggiunge il pot fisico,
    // qui avviene l'aggancio. In questa situazione NON vogliamo che
    // il pot "spari" subito il suo valore fisico (es. 0) sovrascrivendo
    // l'automazione (scattino iniziale).
    //
    // Quindi:
    // - disarmiamo (caught)
    // - sincronizziamo lastbutton[lb] alla posizione fisica corrente
    // - blocchiamo l'invio per QUESTO giro (allowWrite=0)
    // ============================================================
    bit_write(5, chan + page, 0);  // CAUGHT: sblocca
    lastbutton[lb] = valore / 4;   // sync base to physical now
    allowWrite = 0;                // non inviare nello stesso ciclo del catch
  }
  else
  {
    allowWrite = 0;                // ancora ARMED: non deve scrivere né inviare
  }
}
#endif // ENABLE_POT_TAKEOVER

    if ( allowWrite && pot_confronto > diff )
   // if ( pot_confronto > diff   ) // scaglioni da 4 - qundi 4 8 12 16 etc etc // #mod_finestra
                                                           // the potentiometer has been moved
     {
       {

        if (modetable[chan] == 37) // il MODE relativo al qwerty pot sarà a parte ed esclusivo
       {
   if (qwertyvalue[chan] > 0 && eeprom_preset_active != 0)          // pot working in qwerty mode - solo se non siamo in autodetect e qwerty ha un valore
  {
    if (valore > upper_val ) { // -------------------------------------------------------
      
      if ( lastbutton[lb]*4 < upper_val) { 
     qwerty_out(1,qwertyvalue[chan],0); 
  //  Serial.println("alto ");
    }
    
    }
    else 
    if (valore < 124) { if ( lastbutton[lb]*4 > 124) { // if (maxvalue[chan] == 127) 
    qwerty_out(1,minvalue[chan],0);
  // Serial.println("basso ");
    } }
     
    else //------------------------------------------------------------------------
    
      if ( lastbutton[lb]*4 > upper_val) {
        qwerty_out(0,qwertyvalue[chan],0); 
    //    Serial.println ("!alto ");
   
      }
      else 
      if (lastbutton[lb]*4 < 124) { // if (maxvalue[chan] == 127)  
     qwerty_out(0,minvalue[chan],0); 
   //   Serial.println ("!basso ");
      }
      
      
      lastbutton[lb] = valore / 4 ;
    //  delay(100);
     //   Serial.print ("valore: ");  Serial.println (valore);
  }
       }
   
   else                                                       // pot working in MIDI mode ---------------------------------------------------------------------------------------

   {

 lastbutton[lb] = valore / 4 ;
 


 ///  ----------------------------------------------------------
   

  
      if (modetable[chan] ==11 ) {if ((typetable[chan + (page)]) < 224) valore = map32(valore, 63, 960, minvalue[chan], maxvalue[chan]) ;} // pot normale 
                                                                                // NOTA: il constrain da 0 a 127 viene fatto in seguito sulla variabile potout
 

 
      
      else if (modetable[chan] == 12) { valore = map32(valore, 63, 256, minvalue[chan], maxvalue[chan]) ;                             // hypercurve 1
      #if (shifter_active == 1 && stratos == 0)
      shifter.setAll(LOW); // non ricordo a che serve questo spegnimento
      #endif
      }
      
      else if (modetable[chan] == 13) { valore = map32(valore, 768, 960, minvalue[chan], maxvalue[chan]) ;                            // hypercurve 2
      #if (shifter_active == 1 && stratos == 0) 
      shifter.setAll(LOW); 
      #endif
      }
    
     else if (modetable[chan] == 14){                    // hypercurve center
       
     if (valore < 448) { valore = map32(valore, 63, 448, 
     0, 64) ; } 
     else 
     if (valore > 576) { { valore = map32(valore, 576, 
     960, 64, 127) ; } }
     else valore= 64;   }  
  
     else if (modetable[chan] == 15){                     // hypercurve center 2 
       
     if (valore < 340) { valore = map32(valore, 63, 340, 0, 64) ; }  
     else 
     if (valore > 684) { { valore = map32(valore, 684, 960, 64, 127) ; } }
     else valore= 64;    }


    ///  ----------------------------------------------------------
 

  //---------------------------------------------------------
       potOut = constrain(valore,0,127);   // serve per inviare midi!!! - in realtà constrain è una sorta di misura di sicurezza - per non avere valori essessivi - ma perchè dovrebbero esserci??
      
       #if (shifter_active == 1 )    
       encled[0] = abs( 15 - ((potOut) / 8)) * 16 ;   // spiegazione? boh --- mi pare di capire che con questa formula arrivo a una escursione 0 - 240 - ma quantizzata
       #endif

      #if (Matrix_Pads > 0 )    
       /*
       if (potOut > 84 )
    //   encled[0] = abs( 15 - ((potOut) / 8)) * 16 ;  // stessa cosa che per shifter - voglio ottenere un uguale trattamento dei dati - dell effetto visivo si occupera led_enc_exe_matrix
     encled[0] = map32((potOut - 84) *6, 0,240,240,0); 
       else if (potOut > 42 )
       encled[0] = map32((potOut - 42) *6, 0,240,240,0); 
       else
       encled[0] = map32(potOut *6,0,240,240,0); 
       // Serial.println(encled[0]);
      // encled[0]= valore/4; 
*/

  encled[0] = potOut*2 ;   


      #endif // (Matrix_Pads > 0 )    
  
    ///  ----------------------------------------------------------

   
      
      switch ( (typetable[chan + (page)] - 144)  / 16)                       ////////// qui viene inviato il segnale midi definitivo
      {
        case 0 :   noteOn(typetable[chan + (page)]+32, valuetable[chan + (page)],  potOut, 1) ; break; // if (chan < 8) noteOn(176, chan,  valore/8, 0) ; break;// note
        case 1 :   noteOn(typetable[chan + (page)], valuetable[chan + (page)],  potOut, 1) ; break; // poly AT
        case 2 :   noteOn(typetable[chan + (page)], valuetable[chan + (page)],  potOut, 1) ;
                   //Serial.println(encled);
        break; // cc
#if Scene
          case 3:  // PC  (nel DART: usato come "CC-scene" per pot continui)

    // ============================================================
    // CTRL-F: CONVOY_POTS_CASE3
    // DEVIAZIONE verso CONVOY (sacrificale v1)
    // Se questo pot è uno dei scene-subject, NON inviamo direttamente MIDI:
    // aggiorniamo il convoy e lasciamo che lui invii (anti-spam + snapshot fedele).
    // Se NON è un subject, comportamento originale (PC->CC continuo).
    // ============================================================

    {
      byte mem_chan = chan;                     // stesso indirizzo usato qui sotto
      byte si = convoy_find_subjectIndex(mem_chan);    // 0..15 se è un subject, 255 se no

      if (si != 255)
      {
        convoy_set(si, potOut);
        convoy_commit();                               // per ora commit immediato (brutale)
        break;
      }
    }

    // Fallback: comportamento originale (PC->CC)
    noteOn(typetable[chan + page] - 16,
           valuetable[chan + page],
           potOut,
           1);

    break;
#endif

        case 4 :   noteOn(typetable[chan + (page)], potOut,  0, 1) ; break; // channel AT
        case 5 :  {
           valore =constrain(map32(valore,24,1000, 0,1024),0,1023);                                           // PB - pitch bend e la preparazione encled per l'effetto visivo
         // valore = constrain(map32(valore,24,1000, 0,1024),0,1023);
            noteOn(typetable[chan + (page)], (valore - ((valore / 8) * 8)) * 16,  valore / 8 , 1) ;
             encled[0] = abs( 15 - ((valore) / 64)) * 16 ;
          }
          break; // PB
          #if Scene
        case 6 :  // SCENE CONTROL (marker TYPE=6 -> typetable raw 244)
          // ============================================================
          // CTRL-F: POTS_TYPE6_SCENE_MARKER
          // Questo case è attivo quando in EEPROM abbiamo scritto 244 (0xF4 undefined).
          // Infatti: (244 - 144) / 16 = 6.  Il canale NON conta (marker fisso).
          // Deviazione verso il max-wins scene control (D_scene.ino: scene_control_pot()).
          // ============================================================
          
          scene_control_pot();
          break;
          #endif
      }

      ///  ----------------------------------------------------------

      
                            
        cycletimer = 0;                                    // effetti led
        
 #if (shifter_active == 1 && stratos == 0)
       if (lightable[chan] >32) {led_enc_exe();}
      
       #if (blinker ==1)
       else
        {
        if  ( lightable[chan] > 0)                          // 0= no efetti - 1=effetti - 2=blinker
   { 
       if (potOut > 0 && modetable[chan] < 14 ||   // hypercurve o normal
      potOut != 64 && modetable[chan] > 13 )       // centercurve
     { 
                bit_write(1,(lightable[chan]-1)+page,1);  
     }
       else 
     {     
               bit_write(1,(lightable[chan]-1)+page,0);
       
     }
         
   }
        }
        #endif // (blinker ==1)
       
  #endif //(shifter_active == 1 && stratos == 0)
       

       #if (Matrix_Pads > 0  )    // nel caso del controller matrix - blinker e effetti led sono coesistenti
       if (lightable[chan] >0 ) led_enc_exe_matrix();
        //  else
        #if (blinker ==1 )
        
        {
        if  ( lightable[chan] > 1)                  // 0= no efetti - 1=effetti - 2=blinker+effeti
   { 
       if (potOut > 0 && modetable[chan] < 14 ||   // hypercurve o normal
      potOut != 64 && modetable[chan] > 13 )       // centercurve
     { 
                bit_write(1,(lightable[chan]-1)+page,1);  
     }
       else 
     {     
               bit_write(1,(lightable[chan]-1)+page,0);
                 digitalWrite(8,LOW);
       
     }
         
   }
        }
  #endif  // (blinker ==1 )
        
       #endif // (Matrix_Pads > 0  )


       

       #if (DMX_active == 1  && stratos == 0)
  //if  (dmxtable[chan] == 100 ) 
   if (eeprom_preset_active == 1 )  DmxSimple.write(dmxtable[chan], potOut*2); 
 //  Serial.println("a");
#endif
   

      qwertyvalue[chan] = 80; // #mod_finestra

    }  // -------------- fine della condizione generale pot mosso
    
       }
 
 //   {if (valuetable[chan + page] == 63) digitalWrite(8,HIGH);} // sperimentale - volevo vedere con un led la finestra temporale che si apre
       }
       

         if ( pot_confronto <=  7   )      { if (qwertyvalue[chan]>0 && modetable[chan]<16 ) qwertyvalue[chan]--; 
         // else {if (valuetable[chan + page] == 63) digitalWrite(8,LOW);} // sperimentale - volevo vedere con un led la finestra temporale che si apre
         
         
         }

  
    #if (blinker ==1) 
{
   #if (shifter_active == 1 && stratos == 0)
  if (bit_read(1,(lightable[chan]-1)+page) == 1) {  // led = acceso nel banco di memoria 
  if (typetable[general_mempos] == 0 ) {  shifter.setPin((lightable[chan]-1), 0); shifterwrite= 1;} // a zero spegni led
  if (typetable[general_mempos] == 40 ) {shifter.setPin((lightable[chan]-1), 1); shifterwrite= 1;}   // a 40 accendi led
  }
  if (typetable[general_mempos] == 80) typetable[general_mempos] = 0;                                // a 80 resetta il counter led lampeggiante
  #endif
  }

     #if (Matrix_Pads > 0  && touch_led_onboard == 1 && Touch_sensors_enable == 2)
     {

    //   if  ( lightable[chan] > 1)  Serial.println(bit_read(1,(lightable[chan]-1)+page)); 
    
        if (bit_read(1,(lightable[chan]-1)+page) == 1) {   // led = acceso nel banco di memoria 

          
  if (typetable[general_mempos] == 0 )  { //shifter.setPin((lightable[chan]-1), 0); shifterwrite= 1;
    digitalWrite(8,LOW);                  // a zero spegni led
  }
  if (typetable[general_mempos] == 40 ) { // shifter.setPin((lightable[chan]-1), 1); shifterwrite= 1;
    digitalWrite(8,HIGH);                 // a 40 accendi led
  }
  }
  if (typetable[general_mempos] == 80) typetable[general_mempos] = 0;   // a 80 resetta il counter led lampeggiante
  
      }
     #endif
#endif //(blinker ==1)

  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void user_item1 ()
{

  }

  void user_item2 ()
  {
 

    
    }

    void user_item3 ()
  {}

    void user_item4 ()
  {}


//-----------------------------------------------------
 
  void reset() 
{
  // info: funzione che riporta un encoder ad un determinato punto della sua escursione , se impostato in Pot-Emulation
  
  // Serial.println("reset pressed");
  // delay(100);
   
      if (valore < lower_val && lastbutton[chan] == 1)   ///// button pushed
   {     
   //  Serial.println(minvalue[chan]);
       lastbutton[chan] = 0;
       
        
       #if (stratos == 0)    
       button(typetable[ remapper(minvalue[chan]-1)+page],valuetable[ remapper(minvalue[chan]-1)+page],maxvalue[chan] ,1);
       lightable[remapper(minvalue[chan]-1)] = maxvalue[chan]*2;
        
       #endif 
  
        #if (stratos == 1)
        button(typetable[ (minvalue[chan])+page],valuetable[ (minvalue[chan])+page], maxvalue[chan] ,1);
        lightable[minvalue[chan]] = maxvalue[chan]*2;
        #endif
   }



   
     if (valore > upper_val && lastbutton[chan] == 0)   ///// button released
    {
      lastbutton[chan] = 1;
     // Serial.println("reset pressed");
   
    }

    
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void shifter_modifier() 

{

// funzione simile al pageswitch , tutti i messaggi midi vengono temporaneamente traslati di canale - in tal modo è possibile controllare più parametri
  
     if (valore < lower_val && lastbutton[chan] > 0)   ///// button pushed
     {     
       lastbutton[chan] = 0;
    //   Serial.println("tunz on");
       if (lightable[chan] > 0)
       {
     #if (stratos == 0)  
    //  valuetable[ remapper(lightable[chan]-1)+page] ++;
     typetable[ remapper(lightable[chan]-1)+page] = typetable[ remapper(lightable[chan]-1)+page] + dmxtable[chan] ;
   // Serial.println("tunz");
     #endif
     
     #if (stratos == 1)
    //  valuetable[ (lightable[chan])+page] ++;
        typetable[ (lightable[chan])+page] = typetable[ (lightable[chan])+page] + dmxtable[chan] ;
    //  Serial.println("tunz");
     #endif
       }
       else{shifter_modifier_ = dmxtable[chan];}
        
   }

     if (valore > upper_val && lastbutton[chan] == 0)   ///// button released
    {
      lastbutton[chan] = 1;
   //   Serial.println("tunz off");
      if (lightable[chan] > 0)
      {
            #if (stratos == 0)  
            //valuetable[ remapper(lightable[chan]-1)+page] --;
             typetable[ remapper(lightable[chan]-1)+page] = typetable[ remapper(lightable[chan]-1)+page] - dmxtable[chan];
            #endif
            
       #if (stratos == 1)
       // valuetable[ (lightable[chan])+page] --;
       typetable[ (lightable[chan])+page] = typetable[ (lightable[chan])+page] -dmxtable[chan];
       #endif
      }
      else shifter_modifier_=0;
   
    }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 void virtual_touch_end(byte numero)
{
  
      if (cycletimer == minvalue[touch_mempos[numero]]) // decay vitualtouch
      { 
        cycletimer++;
       // Serial.println("virtual_touch");
    //   encled[0]=0;
      
      // utilizzo inomingbyte (boolean(page) - e' na variabile normalmente utilizzata per il midi-in, per on creare nuove variabili la riutilizzo
      if (V_touch_regulator[numero] == 0 
      && lightable[touch_mempos[numero]] == 1)         // la casella lightable si usa per decidere se attivare o no il touch
      {
      //  shifter.setAll(LOW); 
        midiOut(typetable[touch_mempos[numero]+page]+1,valuetable[touch_mempos[numero]+page],0);        // il virtual touch viene emesso - su un canale maggiorato di 1 rispetto al touch normale
          #if (shifter_active == 1 && stratos == 0)
             shifter.setAll(LOW);  
             #endif
              
              if (qwertyvalue[touch_mempos[numero]] == 1)  // touch reset 
              {encodervaluepot[numero] = dmxtable[touch_mempos[numero]]*8;  // posiziono encodervaluepot sulla giusta posizione da 0a1024
              button(typetable[encoder_mempos[numero]+page],valuetable[encoder_mempos[numero]+page],dmxtable[touch_mempos[numero]],0); 
              // se la modetable del touch E' in toggle, avremo un virtual-touch-reset sul valore definito da DMXtable del touch.
                                }
     V_touch_regulator[numero]= 1; 
     
     }
      
        
 
    // se l'encoder non gira piu' bisogna fare ledrestore
    cycletimer =   minvalue[touch_mempos[numero]]+1;
      }

  // 
}

//----------------------------------------------------------------------------------------------------------------------------------
 
void restore_end()
{
  
   if (cycletimer == 66)
  {
    #if (shifter_active == 1 && stratos == 0)
        ledrestore(page);  shifterwrite=1;
        #endif

        #if (Matrix_Pads > 0 )
         matrix_restore (page); // gli effetti led sono già stabili così , non serve questo restore per adesso, perferisco non averlo per chè va a spegnere i pulsanti che sono ancora premuti
    
        #endif
        cycletimer = 67;
  }
}

//---------------------------------------------------------------------------------------------------------

 void HOT_keys (byte canale, byte pressione) 
{
   // se un modificatore e' impostato su messaggio qwerty (>0), il valore di minvalue 
   // (non utile per il note off) viene utilizzato per selezionare i modificatori qwerty 

     #if defined (__AVR_ATmega32U4__)  

 //  Serial.println(minvalue[canale]);


 #if (hid_keys ==1)
  
   if (minvalue[canale] > 0) { 
    if (pressione == 1) 
       switch (minvalue[canale]) {
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
         Keyboard.press(128);  Keyboard.press(129);// ctrl+shift left 
       break;
       case 5 :
         Keyboard.press(128);  Keyboard.press(130);// ctrl+alt left 
       break;
        case 6 :
         Keyboard.press(128);  Keyboard.press(129); Keyboard.press(130);// ctrl+alt left 
       break;
       }
       else 
       {
       switch (minvalue[canale]) {
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
         Keyboard.release(128);  Keyboard.release(129);// ctrl+shift left 
       break;
       case 5 :
         Keyboard.release(128);  Keyboard.release(130);// ctrl+alt left 
       break;
            case 6 :
         Keyboard.release(128); Keyboard.release(130); Keyboard.release(130);// ctrl+alt left 
       break;
       }
       
       }
}

#endif


#endif
}


  
