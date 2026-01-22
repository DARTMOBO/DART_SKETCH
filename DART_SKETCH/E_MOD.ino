//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pageswitch(){ //----------------------------------------------------- PAGE SWITCH

  // 2 casi:
  // A) pagestate==0 e page>0  -> torna alla page 0   (midiOut con MIN)
  // B) pagestate==1 e page==0 -> va a max_modifiers (midiOut con MAX)
  //
  // Tutto il resto è IDENTICO nei due rami, quindi lo condividiamo
  // per spremere flash.

  byte targetPage;
  byte outValue;   // minvalue o maxvalue del page_mempos
  byte ledsMode;   // 0 oppure 1 (per page_leds_)

  if (pagestate==0 && page > 0) {
    targetPage = 0;
    outValue   = minvalue[page_mempos];
    ledsMode   = 0;
  }
  else if (pagestate==1 && page == 0) {
    targetPage = max_modifiers;
    outValue   = maxvalue[page_mempos];
    ledsMode   = 1;
  }
  else {
    return; // nessun cambio pagina richiesto
  }

  // Applica target
  byte oldPage = page;
  page = targetPage;

  // CTRL-F: TAKEOVER_INIT_PAGE2_ON_FIRST_ENTRY
#if ENABLE_POT_TAKEOVER
  // Prima entrata in Page2: inizializza i target lastbutton della Page2 copiando Page1
  // (evita target=0 irraggiungibili che bloccherebbero il takeover)
  if (page == max_modifiers)
  {
    if ((potPageInitMask & 2) == 0)
    {
      for (byte i = 0; i < max_modifiers; i++)
        lastbutton[64 + i] = lastbutton[i];

      potPageInitMask |= 2;
    }
  }
#endif
  // CTRL-F: TAKEOVER_ARM_ON_PAGESWITCH
#if ENABLE_POT_TAKEOVER
// Appena entri in una pagina: ARMED su tutti i pot (chan 0..59) di quella pagina.
// pots() li sbloccherà (CAUGHT) solo quando agganciano il loro target.
for (byte i = 0; i < max_modifiers; i++)
{
  bit_write(5, i + page, 1);
}
#endif


  // Reset “modifiers” (come nel tuo)
  shifter_modifier_ = 0;

  // IMPORTANT DESIGN NOTE (la tua): page switch = full state reload
  reset_mempos();       // come in chiusura editor (241)
  load_preset_base();   // riallinea typetable/valuetable ecc (base)
  load_preset(page);    // poi carica la pagina corrente

#if (Scale ==1)
  // Riallinea scale spinner (come nel tuo)
  update_scala(1); // secondo spinner
  update_scala(0); // primo spinner
  #endif

  // Restore shifter leds (solo se attivo e non stratos)
  #if (shifter_active == 1 && stratos == 0)
    shifter.setAll(LOW);
    shifterwrite = 1;
    ledrestore(page);
  #endif

  // Restore matrix (se presente)
  #if (Matrix_Pads > 0)
    // se il led di segnalazione sta blinkando - cambiando page si potrebbe bloccare
    digitalWrite(8,LOW);
    matrix_restore(page);
  #endif

  // Notifica pagina (come nel tuo: typetable/valuetable + min/maxvalue)
  midiOut(typetable[page_mempos], valuetable[page_mempos], outValue);

  // LED pagina (0 oppure 1)
  page_leds_(ledsMode);

  // Flag (come nel tuo)
  shifterwrite = 1;

  // Reset Xen (come nel tuo)
  higher_Xen[0] = 40;
  higher_Xen[1] = 40;
  lower_Xen[0]  = 100;
  lower_Xen[1]  = 100;
}




 void page_leds_ (byte pagina)
 {
if (pagina == 0)
{ 
  #if (page_LEDs == 1)   // indicatori led dedicati al page switch
                if (valuetable[general_mempos] == 0 && lightable[page_mempos]>0) {  // nomobo setup disattivo 
                  #if (  shifter_active == 1)
                shifter.setPin((dmxtable[page_mempos]-1), 1); 
                #endif
                bit_write(1,(dmxtable[page_mempos]-1)+page,1);
                #if (  shifter_active == 1)
                shifter.setPin((lightable[page_mempos]-1), 0); 
                #endif
                bit_write(1,(lightable[page_mempos]-1)+page,0);
                }
                else
                {
                // shifter.setPin((minvalue[page_mempos]-1), 1); 
                digitalWrite(dmxtable[page_mempos]-1,1);
                bit_write(1,(dmxtable[page_mempos]-1)+page,1);
                // shifter.setPin((maxvalue[page_mempos]-1), 0); 
                digitalWrite(lightable[page_mempos]-1,0);
                bit_write(1,(lightable[page_mempos]-1)+page,0);
                }
 #endif
 }
else
{
 #if (page_LEDs == 1)
if (valuetable[general_mempos] == 0 && lightable[page_mempos]>0) {
#if (  shifter_active == 1)
 shifter.setPin((dmxtable[page_mempos]-1), 0); 
 #endif
 bit_write(1,(dmxtable[page_mempos]-1)+page,0);
 #if (  shifter_active == 1)
 shifter.setPin((lightable[page_mempos]-1), 1); 
 #endif
 bit_write(1,(lightable[page_mempos]-1)+page,1);
}
else
 {
 // shifter.setPin((minvalue[page_mempos]-1), 1); 
 digitalWrite(dmxtable[page_mempos]-1,0);
 bit_write(1,(dmxtable[page_mempos]-1)+page,0);
 // shifter.setPin((maxvalue[page_mempos]-1), 0); 
 digitalWrite(lightable[page_mempos]-1,1);
 bit_write(1,(lightable[page_mempos]-1)+page,1);
 }
 #endif

}
 }
 
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void beam()  //----------------------------------------------------- BEAM SENSOR
{
 #if (Distance_sensor == 1 ) ////------------------------
lightable[distance_mempos] 
 // beamValue 
  = constrain( map32(valore, minbeam, maxbeam, maxvalue[chan], minvalue[chan]) , minvalue[chan] , maxvalue[chan])  ;

  if ( qwertyvalue[chan] == 0 ) // se impostato su POT mode
  {

    if (valore > minbeam)  { //  a threshold to eliminate sensor's noise
 //Serial.println(valore); 
  //  Serial.println(lightable[distance_mempos]);
   // Serial.println("");
      noteOn(typetable[chan + (page)], valuetable[chan + (page)],lightable[distance_mempos], 1); // MIDIUSB.flush();
       #if (DMX_active == 1  && stratos == 0)
     //  DmxSimple.write(dmxtable[chan], lightable[distance_mempos] *2);
      #endif
      
#if (shifter_active == 1 && stratos == 0)
      beamefx();
      #endif
      

      shifterwrite = 1;
      lastbutton[distance_mempos] = 1;
    }

    else if (lastbutton[distance_mempos] == 1) {
#if (shifter_active == 1 && stratos == 0)
      ledrestore(page);
      
      shifterwrite = 1;
      #endif
      lastbutton[distance_mempos] = 0;
    }

  }

  else     /////////////
   if ( qwertyvalue[chan] == 1 )
  {                                                            // imppostazione note trigger
    if (valore > 300 && lastbutton[chan] == 1)  {
      lastbutton[chan] = 0;

      noteOn((typetable[chan + (page)]), valuetable[chan + (page)], lightable[distance_mempos] , 1 ); // MIDIUSB.flush();
      buttonefx = 0;
      buttonefxu = 0;
      buttonefxd = 8;
    }
    if (valore < 270 && lastbutton[chan] == 0) {
      noteOn((typetable[chan + (page)]), valuetable[chan + (page)], 0, 1 ); //  MIDIUSB.flush();
      lastbutton[chan] = 1;
    }

  }
  else //--------------------------------------------------- beam scale
 {
   if (beam_counter == 0 ){
    at = map32(lightable[distance_mempos] , 0, 127, minvalue[chan], maxvalue[chan] );
    if ( beam_scala_buffer != at) // se diverso dal precedente sgnale inviato - evitare doppioni e note ripetute
    {
  //if (dmxtable[contoencoder] == 2)
 // scala_learn = scala[((page/max_modifiers)*2)]; // se dmxtable ÃƒÆ’Ã‚Â¨ su 3 allora verrÃƒÆ’Ã‚Â  usata la scala_learn
      if  ( bitRead(scala_learn, (at)-((at/12)*12)) == 1)  // scala_learn
      {
       beam_scale_send ();
      }
       else 
       {
        while ( bitRead(scala_learn, (at)-((at/12)*12)) == 0) 
        {
       // encoder_pot_calcolo(numero);
        at++;
        if ( at > 126) break;
       }
          if ( beam_scala_buffer != at) { beam_scale_send ();
    
        }
        }
     } 
 }


       beam_counter++;
 // if (mouse_wheel_speed_counter > abs(minvalue[contoencoder]-32)) mouse_wheel_speed_counter =0;
 if (beam_counter > 5) beam_counter =0;
  // if (cycletimer = 10) { button(typetable[distance_mempos+page],beam_scala_buffer,0 ,0); }
//  if (valore< minbeam || valore > maxbeam) if (beam_scala_buffer != at) { button(typetable[distance_mempos+page],beam_scala_buffer,0 ,0); beam_scala_buffer = at;}

    }
 #endif ///-------------------------------
}
//////////////////////////////////////////////

 void beam_scale_send ()
 {
  #if (Distance_sensor == 1 )
  #if (shifter_active == 1 && stratos == 0)
            beamefx();
  #endif
        button(typetable[distance_mempos+page],beam_scala_buffer,0 ,0);
        button(typetable[distance_mempos+page],at,127 ,1);  
        beam_scala_buffer= at;
        if (valore< minbeam || valore > maxbeam)  button(typetable[distance_mempos+page],beam_scala_buffer,0 ,0);
       // cycletimer = 0;
        #endif
  }




void seq()
{
// come funziona:
// se su editor si assegna un valore solo al numberbox "A" allora seq funziona a scatti - gli altri numberbox sono vuoti, valore zero.
// ogni scatto è una progrssione dal basso in alto da 0 a 127.
// ogni scatto avanza tanto quanto viene specificato su "a"
// -----------------------
// se si assegna un valore anche al numberbox "b" allora seq funzionerà in modo alternato
// la sequenza alternerà il valore A e il valore B, 
// se type viene assegnato ad un messaggio NOTE allora ad ogni scatto verrà "spento" (note off) lo step precedente
// se assegnamo un valore al numberbox C e D, allora la sequenza si estenderà anche a questi valori





  
    
   if (valore < lower_val && lastbutton[chan] == 1)   ///// button pushed
   {     
       lastbutton[chan] = 0;

     //  Serial.println("push");
       // Serial.println(valore);
      if  (maxvalue[chan] > 0) {
switch (qwertyvalue[chan])   // qwertyvalue[chan] usato come contatore 

                             // > 159 sono cc
{
  case 0 :
 // Serial.println(minvalue[chan]);
 

 if (typetable[chan + (page)] < 160 )             // se sono note
 {
  noteOn(typetable[chan + (page)], minvalue[chan],  127, 1) ;  // invio accensione
  
  if (lightable[chan] > 0 ) {                                  // invio spegnimento
    noteOn(typetable[chan + (page)], lightable[chan],  0, 1) ;
    }
  else if (dmxtable[chan] > 0 )
  {
    noteOn(typetable[chan + (page)], dmxtable[chan],  0, 1) ;
    }
    else noteOn(typetable[chan + (page)], maxvalue[chan],  0, 1) ;
  } 
else                                             // se sono cc
noteOn(typetable[chan + (page)], valuetable[chan + (page)],  minvalue[chan], 1) ;  // l?ultimo "1" è relativo al filtro anti doppioni
  
  qwertyvalue[chan]++;
  break;

  case 1 :
 // Serial.println(maxvalue[chan]);
 if (typetable[chan + (page)] < 160 ) 
 {
   noteOn(typetable[chan + (page)], maxvalue[chan],  127, 1) ;

    noteOn(typetable[chan + (page)], minvalue[chan],  0, 1) ;
  } 
 else
 noteOn(typetable[chan + (page)], valuetable[chan + (page)],  maxvalue[chan], 1) ;
 
  if (dmxtable[chan] > 0 ) qwertyvalue[chan]++; else qwertyvalue[chan]= 0;
  break;

   case 2 :
 // Serial.println(dmxtable[chan]);
 if (typetable[chan + (page)] < 160 ) {
   noteOn(typetable[chan + (page)], dmxtable[chan],  127, 1) ;
   noteOn(typetable[chan + (page)],  maxvalue[chan],  0, 1) ;
  }
 else
  noteOn(typetable[chan + (page)], valuetable[chan + (page)],  dmxtable[chan], 1) ;
  
   if (lightable[chan] > 0 ) qwertyvalue[chan]++; else qwertyvalue[chan]= 0;
  break;

     case 3 :
 // Serial.println(lightable[chan]);
  if (typetable[chan + (page)] < 160 )
  {
    noteOn(typetable[chan + (page)], lightable[chan],  127, 1) ;
     noteOn(typetable[chan + (page)], dmxtable[chan],  0, 1) ;
    }
  else
  noteOn(typetable[chan + (page)], valuetable[chan + (page)],  lightable[chan], 1) ;
  qwertyvalue[chan]= 0;
  break;
  }}
  else                                  // progressione a step - minvalue decide la grandezza dello step
  {
    qwertyvalue[chan] = qwertyvalue[chan]+minvalue[chan];
    if (qwertyvalue[chan] > 127 ) qwertyvalue[chan] =0; 
   // Serial.println(qwertyvalue[chan]);
   noteOn(typetable[chan + (page)], valuetable[chan + (page)],  qwertyvalue[chan], 1) ;
    
    
    }


   }

   if (valore > upper_val && lastbutton[chan] == 0)   ///// button released - non succede niente
    {
      lastbutton[chan] = 1;
    //   Serial.println("release");
     //   Serial.println(valore);
    }



        
  }




#if ( stratos == 0)
 #if PIEZO_PADS_ACTIVE
 void piezo_pads () { 

  // i pads vengono attivati da GENERAL SETTINGS, il messaggio MIDI emesso viene descritto nell'ITEM PADS
  // attivando i pads viene disattivata la resistenza pullup sul sesto input analogico A5
  // 
  setPlexer(padNum*2); 

// padVal = analogRead(5);
padVal = analogRead_1024(5); 
                              // la lettura viene ripetuta due volte per lasciare il tempo di scaricare corrente residua al pad
                              // questa è solo una congettura... sto ancora facendo prove e test

// if (padNum == 0) Serial.println(padVal);

if (padVal > 2 && padDecay[padNum] == 0 ) {
for(byte pad = 0; pad < 6; pad++)
  {
 // setPlexer(padNum*2);
 padVal2 = analogRead_1024(5);
  if (padVal2 > padVal) padVal = padVal2;
}
padDecay[padNum] =14 //+ (padVal / 40)
;
//midiOut(typetable[PADS_mempos+(page)],valuetable[PADS_mempos+(page)]+(padNum*2),(map32(padVal,0,512,0,127)));
midiOut(typetable[PADS_mempos+(page)],valuetable[PADS_mempos+(page)]+(padNum*2),constrain(padVal,0,127));

  buttonefx = 0;
    // buttonefxu = 5;
   buttonefxd = constrain((padVal/128), 0,3);
}

  { 
     padNum++;
    if (padNum > 3) padNum = 0 ;   
{

if (padDecay[padNum] >0) { padDecay[padNum] = padDecay[padNum] -1; 
if (padDecay[padNum] == 1)  midiOut(typetable[PADS_mempos+(page)],valuetable[PADS_mempos+(page)]+(padNum*2),0);

}
   } 
   }
}
 #endif

#endif


#if (Extraplexer ==1 )
void Extraplexer_read ()
{
  if (minvalue[general_mempos] > 0)  // vedi se il plexer EXTRA è attivato // 1= extra plexer attivato - gli input della dartmobo passano da 48 a 56! adesso il pin che prima veniva dedicato al touch sensor 2 viene dedicato alla letture dell'extra plexer - nota: usant touch ic's esterni è possibile avere entrambe le cose. 
           { 
            #if (pullups_active == 1)
             digitalWrite(9, HIGH);
            #endif
    
            #if defined (__AVR_ATmega32U4__)  
           //  valore = analogRead_1024(9); 
          //  valore = analogRead(9); 
         
            valore =  (digitalRead(9)) << 10;
            #endif

            #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
          //  valore = 1000*(!digitalRead(9)); 
           valore =  (digitalRead(9)) << 10;
            #endif
  
            chan = channel + 48;
           //     if (modetable[chan] == 1 && valuetable[chan] == 88) {Serial.println (valore); delay(100);}
            ain_nucleo(); 
            }
  }
#endif


#if (Side_spinner == 1)
void Side_spinner_read()
{
   { // gestione del SIDE SPINNER    // dmxtable[general_mempos] >1 significa che un side spinner è stato "istituito" via editor, nel mio preset. 
       #if defined (__AVR_ATmega32U4__)
      MSB[1]=   digitalRead(22);
      LSB[1]=   digitalRead(23);
      #endif
      #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
      MSB[1]=   digitalRead(18);
      LSB[1]=   digitalRead(19);
      #endif
      
      updateEncoder(encoder_mempos[1]); 
      encoder(encoder_mempos[1]);
                       }
  }
#endif
