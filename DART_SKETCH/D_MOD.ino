
// MODIFIERS 


//---------------------------------------------------------------------------------------------------------------
  void push_buttons(byte velo)
  {
      // gestione pulsanti
  // la variabile velo ha valore 0 o 1 - se impstata su 1 attiva la lettura della velocity
  // lastbutton[] gestisce il debouncing del pulsante 
  //          - si fa riferimento a lastbutton_debounce che è una variabile fissa 
  //            con valore diverso secondo il circuito del controller (normale o stratos - 10 o 40)
  //            in partica serve come temporizzatore per evitare messaggi ripetuti in caso di pulsanti scadenti che non fanno bene contatto
  //
  // lower_val e upper_val - sono due variabili fisse che indicano i valori sotto e soprai quali si considera che un pulsante sia premuto
  // 
    {


    if (valore < lower_val    )                         ///// button pushed       
        {
     
      if (lastbutton[chan] == lastbutton_debounce)
      {
       /*
       Serial.print(modetable[19]); Serial.println(" - modetable");
        Serial.print(dmxtable[19]); Serial.println(" - dmxtable - modalità endless / pot");
        Serial.print(qwertyvalue[19]); Serial.println(" - touchstop");
        Serial.print(minvalue[19]-32); Serial.println(" - speed");
*/


        
      if (modetable[chan] >= 3) offgroup(chan,1);      // da 3 in poi ci sono i toggle groups e radio groups
      
            if (bit_read(4,page+chan)==0)              // 4 = toggletable // something happens only if the button is off in the toggletable
            { 
               if ( eeprom_preset_active == 0 ) dmxtable[chan]++;    // ????
               
        
     //  Serial.println(mouse_mempos);
            #if defined (__AVR_ATmega32U4__)  
            HOT_keys(chan,1);    
            #endif  
             
             
            
         
            if (typetable[chan+page] < 160) scale_learn(valuetable[chan+page]);   // sotto 160 ci sono note on e off 
             
            #if (shifter_active == 1 && stratos == 0)    
            ledControl(chan, 1);
            ledrestore(page);
            #endif
                
            #if (Matrix_Pads == 1 )
            single_h(matrix_remap[chan],lightable[chan],1,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo, send)
          
          
            #endif

             #if (Matrix_Pads == 2 )
            single_h(matrix_remap[chan-16],lightable[chan],1,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo, send)
          
          
            #endif
    
            
          
       
          if (modetable[chan] > 6) bit_write(4,chan+page,true);   // r-groups // ricordare che: 4 = togletable
           else if (modetable[chan] >= 2   )  bit_write(4,chan+page,!bit_read(4,page+chan));     

          if ( velo == 0 )    outnucleo (1,chan);
          else {
         
           int velopush =  constrain(analogRead(plexer),60,255);  //Serial.println (velopush);//  delay(2);
            button(typetable[chan+page],valuetable[chan+page],map(velopush,60,255,127,10),1);
       
               }

         // outnucleo (1,chan);
          
            } 
            
            else     /// se il pulsante è acceso nella toggletable
            
            if (modetable[chan] <7   )
            {
               #if defined (__AVR_ATmega32U4__)  
            HOT_keys(chan,0);    
            #endif  
             
               #if (shifter_active == 1 && stratos == 0)    
            ledControl(chan, 0);
            ledrestore(page);
            #endif
          

           #if (Matrix_Pads == 1 )
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
      {
        if (modetable[chan] >= 2  && modetable[chan] < 11) // se il pulsante = toggle o t-group o r-group
        {
         if( modetable[chan] < 7  )  {  //                                                      7 8 9 10 sono RADIO group
                 
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

void pots ()
{
 
  {
    
    if (abs((lastbutton[chan] * 4)  - valore) > 10 
       )                                                     // the potentiometer has been moved
       
   if (qwertyvalue[chan] > 0 && eeprom_preset_active != 0) {         // pot working in qwerty mode - solo se non siamo in autodetect e qwerty ha un valore
   {
    if (valore > upper_val ) {
      
      if ( lastbutton[chan]*4 < upper_val) { 
      qwerty_out(1,qwertyvalue[chan],0); 
    //  Keyboard.press(pgm_read_byte(qwertymod+qwertyvalue[chan]));
     // if (qwertyvalue[chan] > 31 ) Keyboard.press(qwertyvalue[chan]); // normale tabella ascii // 
    }
    
    }
    else 
    if (valore < lower_val) { if ( lastbutton[chan]*4 > lower_val) { // if (maxvalue[chan] == 127) 
    qwerty_out(1,minvalue[chan],0); //Keyboard.press(pgm_read_byte(qwertymod+qwertyvalue[chan]+1)); 
    } } 
    else
    {
      if ( lastbutton[chan]*4 > upper_val) {
        qwerty_out(0,qwertyvalue[chan],0); 
      //  Keyboard.release(pgm_read_byte(qwertymod+qwertyvalue[chan]));
       //  if (qwertyvalue[chan] > 31 ) Keyboard.release(qwertyvalue[chan]);
      }
      else 
      if (lastbutton[chan]*4 < lower_val) { // if (maxvalue[chan] == 127)  
      qwerty_out(0,minvalue[chan],0); //Keyboard.release(pgm_read_byte(qwertymod+qwertyvalue[chan]+1));
      }
      }
      
      lastbutton[chan] = valore / 4 ;
   }
   }
   
   else                                                       // pot working in MIDI mode ---------------------------------------------------------------------------------------

   {

 lastbutton[chan] = valore / 4 ;
 


 ///  ----------------------------------------------------------
   

  #if (fader_type == 0)    // ci sono potrenziometri a slitta che hanno una zona vuota sopra e sotto...
      if (modetable[chan] ==11 ) {if ((typetable[chan + (page)]) < 224) valore = map(valore, 63, 960, minvalue[chan], maxvalue[chan]) ;} // pot normale 
                                                                                // NOTA: il constrain da 0 a 127 viene fatto in seguito sulla variabile potout
  #endif

  #if (fader_type == 1)
   if (modetable[chan] ==11 ) {if ((typetable[chan + (page)]) < 224) valore = map(valore, 0, 1024, minvalue[chan], maxvalue[chan]) ;}
  #endif
      
      else if (modetable[chan] == 12) { valore = map(valore, 63, 256, minvalue[chan], maxvalue[chan]) ;                             // hypercurve 1
      #if (shifter_active == 1 && stratos == 0)
      shifter.setAll(LOW); 
      #endif
      }
      
      else if (modetable[chan] == 13) { valore = map(valore, 768, 960, minvalue[chan], maxvalue[chan]) ;                            // hypercurve 2
      #if (shifter_active == 1 && stratos == 0) 
      shifter.setAll(LOW); 
      #endif
      }
    
     else if (modetable[chan] == 14){                    // hypercurve center
       
     if (valore < 448) { valore = map(valore, 63, 448, 
     0, 64) ; } 
     else 
     if (valore > 576) { { valore = map(valore, 576, 
     960, 64, 127) ; } }
     else valore= 64;   }  
  
     else if (modetable[chan] == 15){                     // hypercurve center 2 
       
     if (valore < 340) { valore = map(valore, 63, 340, 0, 64) ; }  
     else 
     if (valore > 684) { { valore = map(valore, 684, 960, 64, 127) ; } }
     else valore= 64;    }


    ///  ----------------------------------------------------------
 

  //---------------------------------------------------------
       potOut = constrain(valore,0,127);   // serve per inviare midi!!!
      
       #if (shifter_active == 1 )    
       encled[0] = abs( 15 - ((potOut) / 8)) * 16 ;   // spiegazione? boh --- mi pare di capire che con questa formula arrivo a una escursione 0 - 240
       #endif

      #if (Matrix_Pads > 0 )    
       
       if (potOut > 84 )
    //   encled[0] = abs( 15 - ((potOut) / 8)) * 16 ;  // stessa cosa che per shifter - voglio ottenere un uguale trattamento dei dati - dell effetto visivo si occupera led_enc_exe_matrix
     encled[0] = map((potOut - 84) *6, 0,240,240,0); 
       else if (potOut > 42 )
       encled[0] = map((potOut - 42) *6, 0,240,240,0); 
       else
       encled[0] = map(potOut *6,0,240,240,0); 
       // Serial.println(encled[0]);
      // encled[0]= valore/4; 
      #endif
  
    ///  ----------------------------------------------------------

   
      
      switch ( (typetable[chan + (page)] - 144)  / 16)                       ////////// qui viene inviato il segnale midi definitivo
      {
        case 0 :   noteOn(typetable[chan + (page)]+32, valuetable[chan + (page)],  potOut, 1) ; break; // if (chan < 8) noteOn(176, chan,  valore/8, 0) ; break;// note
        case 1 :   noteOn(typetable[chan + (page)], valuetable[chan + (page)],  potOut, 1) ; break; // poly AT
        case 2 :   noteOn(typetable[chan + (page)], valuetable[chan + (page)],  potOut, 1) ;
                   //Serial.println(encled);
        break; // cc
        case 3 :   //noteOn(176, chan,  valore/8, 0) ; break; // pc
        case 4 :   noteOn(typetable[chan + (page)], potOut,  0, 1) ; break; // channel AT
        case 5 :  {
           valore =constrain(map(valore,24,1000, 0,1024),0,1023);                                           // PB - pitch bend e la preparazione encled per l'effetto visivo
         // valore = constrain(map(valore,24,1000, 0,1024),0,1023);
            noteOn(typetable[chan + (page)], (valore - ((valore / 8) * 8)) * 16,  valore / 8 , 1) ;
             encled[0] = abs( 15 - ((valore) / 64)) * 16 ;
          }
          break; // PB
      }

      ///  ----------------------------------------------------------

      
                            
        cycletimer = 0;                                    // effetti led
        
 #if (shifter_active == 1 && stratos == 0)
       if (lightable[chan] == 1) {led_enc_exe();}
       #if (blinker ==1)
       else
        {
        if  ( lightable[chan] > 1)
   { 
       if (potOut > 0 && modetable[chan] < 14 || 
      potOut != 64 && modetable[chan] > 13 ) 
     { 
                bit_write(1,(lightable[chan]-1)+page,1);  
     }
       else 
     {     
               bit_write(1,(lightable[chan]-1)+page,0);
       
     }
         
   }
        }
        #endif
       
  #endif
       

       #if (Matrix_Pads > 0  )
       if (lightable[chan] >0 ) led_enc_exe_matrix();
        //  else
        #if (blinker ==1 )
        
        {
        if  ( lightable[chan] > 1)
   { 
       if (potOut > 0 && modetable[chan] < 14 || 
      potOut != 64 && modetable[chan] > 13 ) 
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
  #endif
        
       #endif

       #if (DMX_active == 1  && stratos == 0)
  //if  (dmxtable[chan] == 100 ) 
   if (eeprom_preset_active == 1 )  DmxSimple.write(dmxtable[chan], potOut*2); 
 //  Serial.println("a");
#endif
   

     

    }  // -------------- fina della condizione generale pot mosso
    #if (blinker ==1) 
{
   #if (shifter_active == 1 && stratos == 0)
  if (bit_read(1,(lightable[chan]-1)+page) == 1) {
  if (typetable[general_mempos] == 0 ) {  shifter.setPin((lightable[chan]-1), 0); shifterwrite= 1;}
  if (typetable[general_mempos] == 40 ) {shifter.setPin((lightable[chan]-1), 1); shifterwrite= 1;}
  }
  if (typetable[general_mempos] == 80) typetable[general_mempos] = 0;
  #endif
  }

     #if (Matrix_Pads > 0  && touch_led_onboard == 1)
     {
        if (bit_read(1,(lightable[chan]-1)+page) == 1) {
  if (typetable[general_mempos] == 0 ) {  //shifter.setPin((lightable[chan]-1), 0); shifterwrite= 1;
    digitalWrite(8,HIGH);
  }
  if (typetable[general_mempos] == 40 ) { // shifter.setPin((lightable[chan]-1), 1); shifterwrite= 1;
    digitalWrite(8,LOW);
  }
  }
  if (typetable[general_mempos] == 80) typetable[general_mempos] = 0;
      }
     #endif
#endif

  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


void user_item1 ()
{
  #if (hid_keys == 1)  
  char ctrlKey = KEY_LEFT_CTRL; 
  #endif
 
  if (valore < lower_val                            ///// button pushed
        && lastbutton[chan] == 1    )
      {  
if (valuetable[chan + page] == 66) {


  #if (hid_keys ==1)
  delay(400);
Keyboard.press(ctrlKey);
  Keyboard.press('s');
  delay(100);
  Keyboard.releaseAll();
  #endif
  
  
  }
}
  }

  void user_item2 ()
  {}

    void user_item3 ()
  {}

    void user_item4 ()
  {}


//-----------------------------------------------------
 
  void reset() 
{
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void pageswitch(){ //----------------------------------------------------- PAGE SWITCH
  
  if (lastbutton[page_mempos] > 0  ) {     
  
  if (pagestate==1 ) { 
    page = 0;
//     load_preset_base();
    load_preset(page);
   
    update_scala(1); // secondo spinner
    update_scala(0); // primo spinner
    
    #if (shifter_active == 1 && stratos == 0)
    shifter.setAll(LOW);
    
    shifterwrite=1;
    ledrestore(page);
    #endif

    #if (Matrix_Pads > 0 )
           // single_h(matrix_remap[chan-16],lightable[chan]-1,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo)
            matrix_restore(page);
            // Serial.println(chan-16);
            #endif
  //  incomingByte = boolean(page);
 
 /// Serial.println(minvalue[mouse_mempos]);
  // Serial.println(maxvalue[mouse_mempos]);
  //  Serial.println("----");

midiOut(typetable[page_mempos],valuetable[page_mempos],minvalue[page_mempos]);

 #if (page_LEDs == 1)   // indicatori led dedicati al page switch
                if (valuetable[general_mempos] == 0 && lightable[page_mempos]>0) {  // nomobo setup disattivo 
                shifter.setPin((dmxtable[page_mempos]-1), 1); 
                bit_write(1,(dmxtable[page_mempos]-1)+page,1);
                shifter.setPin((lightable[page_mempos]-1), 0); 
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

 
    shifterwrite=1;
    pagestate=0;
    
  
  higher_Xen[0]= 40;
  higher_Xen[1]=40;
    lower_Xen[0]= 100;
  lower_Xen[1]=100;


    }
  } 
  
  
  if (lastbutton[page_mempos]==0  ){ // seconda
 
 if (pagestate==0) {
      page = max_modifiers;
 
 

    load_preset(page);
    
    update_scala(1); // secondo spinner
    update_scala(0); // primo spinner
 
    
      
      #if (shifter_active == 1 && stratos == 0 )
      shifter.setAll(LOW);
      shifterwrite=1;
      ledrestore(page);
      #endif

      #if (Matrix_Pads > 0  )
       //single_h(matrix_remap[chan-16],lightable[chan]-1,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo)
      matrix_restore(page);
            // Serial.println(chan-16);
      #endif
      
   // incomingByte = boolean(page);
 //   Serial.println(minvalue[mouse_mempos]);
 //   Serial.println(maxvalue[mouse_mempos]);
 //   Serial.println("----");
    
 midiOut(typetable[page_mempos],valuetable[page_mempos],maxvalue[page_mempos]);

 #if (page_LEDs == 1)
if (valuetable[general_mempos] == 0 && lightable[page_mempos]>0) {
 shifter.setPin((dmxtable[page_mempos]-1), 0); 
 bit_write(1,(dmxtable[page_mempos]-1)+page,0);
 shifter.setPin((lightable[page_mempos]-1), 1); 
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

    shifterwrite=1;
    
   pagestate=1;
 
    //    lower_Xen[0] = max(averageXen[0], readingsXen[indexXen]);
 
 //    lower_Xen[0] = averageXen[0];
 //     lower_Xen[1] = averageXen[1];

   higher_Xen[0]= 40;
  higher_Xen[1]=40;
  lower_Xen[0]= 100;
  lower_Xen[1]=100;
// test3();
  }
}
 }
 */
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pageswitch(){ //----------------------------------------------------- PAGE SWITCH
  
 // if (lastbutton[page_mempos] > 0  ) 
  {     
  
  if (pagestate==0 && page > 0) { 

    
  
    page = 0;
  shifter_modifier_=0;
    load_preset(page);
   
    update_scala(1); // secondo spinner
    update_scala(0); // primo spinner
    
    #if (shifter_active == 1 && stratos == 0)
    shifter.setAll(LOW);
    
    shifterwrite=1;
    ledrestore(page);
    #endif

    #if (Matrix_Pads > 0 )
           // single_h(matrix_remap[chan-16],lightable[chan]-1,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo)
            digitalWrite(8,LOW); // se il led di segnalazione sta blinkando - cambiando page si potrebbe bloccare
            matrix_restore(page);
            // Serial.println(chan-16);
            #endif
  //  incomingByte = boolean(page);
 
 /// Serial.println(minvalue[mouse_mempos]);
  // Serial.println(maxvalue[mouse_mempos]);
  //  Serial.println("----");

midiOut(typetable[page_mempos],valuetable[page_mempos],minvalue[page_mempos]);




page_leds_(0);
 
    shifterwrite=1;
   // pagestate=0;
    
  
  higher_Xen[0]= 40;
  higher_Xen[1]=40;
    lower_Xen[0]= 100;
  lower_Xen[1]=100;


    }
  } 
  
  
 // if (lastbutton[page_mempos]==0  )
  { // seconda
 
 if (pagestate==1 && page == 0) {
      page = max_modifiers;
 shifter_modifier_=0;
    //    Serial.println(page);
   //  Serial.println(valore);
    // Serial.println(lastbutton[page_mempos]);
    //  Serial.println("--");
     // delay(50);
 
 

    load_preset(page);
    
    update_scala(1); // secondo spinner
    update_scala(0); // primo spinner
 
    
      
      #if (shifter_active == 1 && stratos == 0 )
      shifter.setAll(LOW);
      shifterwrite=1;
      ledrestore(page);
      #endif

      #if (Matrix_Pads > 0  )
       //single_h(matrix_remap[chan-16],lightable[chan]-1,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo)
        digitalWrite(8,LOW);
      matrix_restore(page);
            // Serial.println(chan-16);
      #endif
      
   // incomingByte = boolean(page);
 //   Serial.println(minvalue[mouse_mempos]);
 //   Serial.println(maxvalue[mouse_mempos]);
 //   Serial.println("----");
    
 midiOut(typetable[page_mempos],valuetable[page_mempos],maxvalue[page_mempos]);


page_leds_(1);

    shifterwrite=1;
    
  // pagestate=1;
 
    //    lower_Xen[0] = max(averageXen[0], readingsXen[indexXen]);
 
 //    lower_Xen[0] = averageXen[0];
 //     lower_Xen[1] = averageXen[1];

   higher_Xen[0]= 40;
  higher_Xen[1]=40;
  lower_Xen[0]= 100;
  lower_Xen[1]=100;
// test3();
  }
}
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

lightable[distance_mempos] 
 // beamValue 
  = constrain( map(valore, minbeam, maxbeam, maxvalue[chan], minvalue[chan]) , minvalue[chan] , maxvalue[chan])  ;

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
    at = map(lightable[distance_mempos] , 0, 127, minvalue[chan], maxvalue[chan] );
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

}
//////////////////////////////////////////////

 void beam_scale_send ()
 {
  #if (shifter_active == 1 && stratos == 0)
            beamefx();
  #endif
        button(typetable[distance_mempos+page],beam_scala_buffer,0 ,0);
        button(typetable[distance_mempos+page],at,127 ,1);  
        beam_scala_buffer= at;
        if (valore< minbeam || valore > maxbeam)  button(typetable[distance_mempos+page],beam_scala_buffer,0 ,0);
       // cycletimer = 0;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 #if ( stratos == 0)
 void PADS () { 

  // i pads vengono attivati da GENERAL SETTINGS, il messaggio MIDI emesso viene descritto nell'ITEM PADS
  // attivando i pads viene disattivata la resistenza pullup sul sesto input analogico A5
  // 
  setPlexer(padNum*2); 

// padVal = analogRead(5);
padVal = analogRead(5); 
                              // la lettura viene ripetuta due volte per lasciare il tempo di scaricare corrente residua al pad
                              // questa è solo una congettura... sto ancora facendo prove e test

// if (padNum == 0) Serial.println(padVal);

if (padVal > 2 && padDecay[padNum] == 0 ) {
for(byte pad = 0; pad < 6; pad++)
  {
 // setPlexer(padNum*2);
 padVal2 = analogRead(5);
  if (padVal2 > padVal) padVal = padVal2;
}
padDecay[padNum] =14 //+ (padVal / 40)
;
//midiOut(typetable[PADS_mempos+(page)],valuetable[PADS_mempos+(page)]+(padNum*2),(map(padVal,0,512,0,127)));
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (capacitivesensor_active == 2) // variante della void touch_sensors dedicata all'utilizzo di ic touch esterni
void touch_sensors(byte T_numero) {
  
     if (touch_mempos[T_numero] >0)  // SE ESISTE UN ITEM CON SETTAGGIO TOUCH ALLORA SI PROCEDE ALLA LETTURA
     {                  
      averageXen[T_numero] = digitalRead(7+(T_numero*2));
   
////

  if (lightable[touch_mempos[T_numero]] == 0)  touch_execute(T_numero); // lightable : 0-capacitive, 1-virtual, 2-monitoring.
  else if  (lightable[touch_mempos[T_numero]] == 2) { noteOn(176, T_numero,   averageXen[T_numero] ,0);  delay(20); }          // monitoring dei valori del touch, si possono osservare da editor.

////
     }
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (capacitivesensor_active == 3) // variante della void touch_sensors dedicata all'utilizzo di ic touch esterni
void touch_sensors(byte T_numero) {
  
     if (touch_mempos[T_numero] >0)  // SE ESISTE UN ITEM CON SETTAGGIO TOUCH ALLORA SI PROCEDE ALLA LETTURA
     {                  
      averageXen[T_numero] = digitalRead(7+(T_numero));
   
////

  if (lightable[touch_mempos[T_numero]] == 0)  touch_execute(T_numero); // lightable : 0-capacitive, 1-virtual, 2-monitoring.
  else if  (lightable[touch_mempos[T_numero]] == 2) { noteOn(176, T_numero,   averageXen[T_numero] ,0);  delay(20); }          // monitoring dei valori del touch, si possono osservare da editor.

////
     }
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (capacitivesensor_active == 4) // variante della void touch_sensors dedicata all'utilizzo di ic touch esterni
void touch_sensors(byte T_numero) {
  
     if (touch_mempos[T_numero] >0)  // SE ESISTE UN ITEM CON SETTAGGIO TOUCH ALLORA SI PROCEDE ALLA LETTURA
     {                  
      averageXen[T_numero] = !(digitalRead(7+(T_numero*2)));
   
////

  if (lightable[touch_mempos[T_numero]] == 0)  touch_execute(T_numero); // lightable : 0-capacitive, 1-virtual, 2-monitoring.
  else if  (lightable[touch_mempos[T_numero]] == 2) { noteOn(176, T_numero,   averageXen[T_numero] ,0);  delay(20); }          // monitoring dei valori del touch, si possono osservare da editor.

////
     }
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (capacitivesensor_active == 1)
void touch_sensors(byte T_numero) {
  // in breve in questa void viene caricato un valore sulla variabile averageXen[T_numero] che poi verrà usato nella void touch_execute 
  // per diventare un messaggio midi
  
 //  if (decaysensor[T_numero] > 0 ) decaysensor[T_numero]--;
 

     if (touch_mempos[T_numero] >0)  // SE ESISTE UN ITEM CON SETTAGGIO TOUCH ALLORA SI PROCEDE ALLA LETTURA
     {                  
       #if touch_version == 1
       readingsXen[T_numero][indexXen] = (cs_4_2[T_numero].capacitiveSensorRaw(72)); // SI FANNO DIVERSE LETTURE - INDEXXEN VA DA 0 A 3 vedi void loop
       #endif
       #if touch_version == 2
       readingsXen[T_numero][indexXen]  = (cs_4_2[T_numero].capacitiveSensorRaw(8000)/64);
       #endif
 

                     
  averageXen[T_numero] = ((readingsXen[T_numero][0] + readingsXen[T_numero][1] + readingsXen[T_numero][2])  / 3) ;  // media delle letture 

   
///////////////////////////////////////////////////////////////////////////////////////////

  if (lightable[touch_mempos[T_numero]] == 0)  touch_execute(T_numero); // lightable : 0-capacitive, 1-virtual, 2-monitoring.
  else if  (lightable[touch_mempos[T_numero]] == 2) { noteOn(176, T_numero,   averageXen[T_numero] ,0);  delay(20); }          // monitoring dei valori del touch, si possono osservare da editor.

///////////////////////////////////////////////////////////////////////////////////////////
     }
   

{
if (averageXen[T_numero] > higher_Xen[T_numero])  higher_Xen[T_numero] = averageXen[T_numero] ;                                       
if (averageXen[T_numero] < lower_Xen[T_numero] && averageXen[T_numero] == readingsXen[T_numero][indexXen] )  lower_Xen[T_numero] = averageXen[T_numero] ;    
 // limite superiore e inferiore si adattano ale circostanze ambientali, in tel modo la soglia decisa via editor è sempre relativa al range


}
              
// sistema di autolimiting : 
// in base alla lettura touch piu' bassa viene stabilito il limite di velocita'  delle letture (interno alla libreria capacitivetouch)
// questo sistema rende retrocompatibile l'algoritmo di sensing nche con i circuiti vecchi, che eramo molto piÃ¹ sensibili e necessitavano di piÃ¹ letture
// piu' letture richiedono piu' tempo, rendendo il touchsensor piu' soggetto a interferenze da parte del DMX o di allungamenti e accorciamenti del loop principale
// dovuti all'uso del side spinner per esempio...

}
#endif

//...............................................................................................................................

#if (capacitivesensor_active == 1)
void touch_execute (byte numero_ex) 
{

   byte zero = 1;
  byte uno = 0;
  {        
         ///////////////////////////////////////////////////////////////////////////////////////////////// rilascio del touch                                 
  if (    averageXen[numero_ex]<
  (higher_Xen[numero_ex]) 
  - map( minvalue[touch_mempos[numero_ex]],0,112,0,higher_Xen[numero_ex]-lower_Xen[numero_ex]) 
  // metto 112 e non 127 - per creare piu separazione tra spazio accensione e spazio spegnimento
  )

  {
     if (decaysensor[numero_ex] > 0 ) decaysensor[numero_ex]--;
   //   if (numero_ex == 0) {Serial.println(decaysensor[numero_ex]); delay(50);}
     
 if ( decaysensor[numero_ex] == 0 ///  
  && lastbutton[touch_mempos[numero_ex]]==uno) /// evitare ripetizione messaggio 
 
 {
  //delay(500); 
 { noteOn(typetable[touch_mempos[numero_ex]+(page)], valuetable[touch_mempos[numero_ex]+(page)], 0,1); 
 // decaysensor[numero] = 0;
 //scala_learn =0; // resetta la scala che viene memorizzata tramite learn ogni volta che si tocca la wheel
//scala_reset = 1;




 if (typetable[encoder_mempos[numero_ex]+page] < 160) // sotto 160 sono note on e note off
 // se encoder impostato su note allora manda il note off ( al rilascio)
 // serve per scale mode, per non lsciare note accese
 {
 button(typetable[encoder_mempos[numero_ex]+page],encodervaluepot_buffer[numero_ex],0 ,1);  
 }


 
 
 if (qwertyvalue[touch_mempos[numero_ex]] == 1)  // touch reset attivo
 { 
       // se impostato su cc o altro - manda il touch reset
       // button(typetable[encoder_mempos[numero]+(page)],valuetable[encoder_mempos[numero]+(page)],dmxtable[touch_mempos[numero]],1); 
       // touch reset normale

int smooth = (encodervaluepot[numero_ex]/8)-dmxtable[touch_mempos[numero_ex]]; // ritorno in modo graduale alla posizione originale
   for (byte i = 8; i > 0; i--)
   {
    button(typetable[encoder_mempos[numero_ex]+(page)],valuetable[encoder_mempos[numero_ex]+(page)],dmxtable[touch_mempos[numero_ex]]+(smooth/8)*(i-1),1);
    delay(15);
   }
      encodervaluepot[numero_ex] = dmxtable[touch_mempos[numero_ex]]*8;
 }
 
 

lastbutton[touch_mempos[numero_ex]] = zero; 
// encled=0; 
encoder_block[numero_ex] = 64;

bit_write(1,maxvalue[touch_mempos[numero_ex]]-1+page,0); // spengo il led nella tabella di memoria


#if (shifter_active == 1 && stratos == 0)
// shifter.setAll(LOW);  
if (page==0) ledrestore(0); else ledrestore(1);  shifterwrite=1; 
#endif
#if ( stratos == 1)
digitalWrite(16,LOW);
#endif


}

} 
  }

//////////////////////////////////////////////////////////////////////////////// touch premuto

if (    averageXen[numero_ex]> 
// (minvalue[touch_mempos[numero]]) + 5
 ((higher_Xen[numero_ex]))  // piu' abbasso questo valore piu' aumenta la sensibilita'
                         // sottraggo  (minvalue[touch_mempos[numero]]) - piu' aumenta piu' e' sensibile
  - map( minvalue[touch_mempos[numero_ex]],0,127,0,higher_Xen[numero_ex]-lower_Xen[numero_ex]) // la sottrazione e' proporzionale ad higher_xen
 //- minvalue[touch_mempos[numero]]
  )
  
 {

#if ( stratos == 1)
   decaysensor[numero_ex] = lightable[general_mempos]*10;
   #endif

   #if ( stratos == 0)
   decaysensor[numero_ex] = lightable[general_mempos]*2;
   #endif
   
 if (lastbutton[touch_mempos[numero_ex]]==zero // && decaysensor[numero] <= 0
 )

{
/*
  Serial.println(scala[0]);
  Serial.println(scala[1]);
  Serial.println(scala[2]);
  Serial.println(scala[3]);
   Serial.println("-");
   */
/*
//  update_scala(0);
 for (byte i = 0; i< 12; i++) {
Serial.print(bitRead(scala[0],i));}
Serial.println(" - scala[]");

//bitRead(valuetable[encoder_mempos[quale_spinner]+page] ,i)
//Serial.print(valuetable[encoder_mempos[0]],BIN); Serial.println(" - valuetable[]");

 for (byte i = 0; i< 8; i++) {
Serial.print(bitRead(valuetable[encoder_mempos[0]],i));}
Serial.println(" - valuetable[]");


 for (byte i = 0; i< 8; i++) {
//Serial.print(maxvalue[encoder_mempos[0]],i));}
Serial.print(bitRead(maxvalue[encoder_mempos[0]],i));}
Serial.println(" - maxvalue[]");

Serial.println(" -//////////////- ");
*/
  
{ //decaysensor[numero] = decaysensor_value; 
scala_reset = 1;
mouse_wheel_speed_counter=0;
noteOn(typetable[touch_mempos[numero_ex]+(page)], valuetable[touch_mempos[numero_ex]+(page)], 127,1);  // MIDIUSB.flush();  
  
lastbutton[touch_mempos[numero_ex]] = uno;  //shifter.setAll(LOW); 
encoder_block[numero_ex] = 64;


bit_write(1,maxvalue[touch_mempos[numero_ex]]-1+page,1);
#if (shifter_active == 1 && stratos == 0)
 shifter.setPin(maxvalue[touch_mempos[numero_ex]]-1,HIGH);  // accendo il led del touch , nella tabella di memoria
 #endif
#if ( stratos == 1)


 digitalWrite(16,HIGH);
  #endif 
// bit_write(1,4+numero+page,1);
//  if (page==0) ledrestore(0); else ledrestore(1);  shifterwrite=1;
shifterwrite=1;};// opencalibration[1]=HIGH;

   }

  
   }
   } 
 
 }
 #endif

//...............................................................................................................................

#if (capacitivesensor_active > 1 )
void touch_execute (byte numero_ex) 
{

  byte zero = 1;
  byte uno = 0;
  {        


                 //////////////////////////////////////////////////////////////// touch rilasciato
                 
  if (    averageXen[numero_ex] == 0 ) 
  
 

 //
 { 
 if (decaysensor[numero_ex] > 0 ) decaysensor[numero_ex]--;

   if (lastbutton[touch_mempos[numero_ex]]== uno && decaysensor[numero_ex] == 0) /// evitare ripetizione messaggio 
  {
 { noteOn(typetable[touch_mempos[numero_ex]+(page)], valuetable[touch_mempos[numero_ex]+(page)], 0,1); 
//decaysensor[numero] = 0;
 
//scala_learn =0; // resetta la scala che viene memorizzata tramite learn ogni volta che si tocca la wheel
//scala_reset = 1;
//Serial.println(typetable[encoder_mempos[numero]+page]);
// Serial.println(valuetable[touch_mempos[numero]+(page)]);

 if (typetable[encoder_mempos[numero_ex]+page] < 160) // sotto 160 sono note on e note off
 // se encoder impostato su note allora manda il note off ( al rilascio)
 // serve per scale mode, per non lsciare note accese
 
 {
 button(typetable[encoder_mempos[numero_ex]+page],encodervaluepot_buffer[numero_ex],0 ,1);  
 
 
 }
 
 if (qwertyvalue[touch_mempos[numero_ex]] == 1)  // touch reset attivo
 { 
       // se impostato su cc o altro - manda il touch reset
       // button(typetable[encoder_mempos[numero]+(page)],valuetable[encoder_mempos[numero]+(page)],dmxtable[touch_mempos[numero]],1); 
       // touch reset normale

int smooth = (encodervaluepot[numero_ex]/8)-dmxtable[touch_mempos[numero_ex]]; // ritorno in modo graduale alla posizione originale
   for (byte i = 8; i > 0; i--)
   {
    button(typetable[encoder_mempos[numero_ex]+(page)],valuetable[encoder_mempos[numero_ex]+(page)],dmxtable[touch_mempos[numero_ex]]+(smooth/8)*(i-1),1);
    delay(15);
   }
      encodervaluepot[numero_ex] = dmxtable[touch_mempos[numero_ex]]*8;
 }
 
 

lastbutton[touch_mempos[numero_ex]] = zero; 
// encled=0; 
encoder_block[numero_ex] = 64;

bit_write(1,maxvalue[touch_mempos[numero_ex]]-1+page,0); // spengo il led nella tabella di memoria


#if (shifter_active == 1 && stratos == 0)
// shifter.setAll(LOW);  
if (page==0) ledrestore(0); else ledrestore(1);  shifterwrite=1; 
#endif
#if ( stratos == 1)
digitalWrite(16,LOW);
#endif

#if (touch_led_onboard == 1 && capacitivesensor_active < 3)
digitalWrite(8,LOW);
#endif

}

 }
} 

//////////////////////////////////////////////////////////////////////////// touch premuto
if (    averageXen[numero_ex] == 1 

  )
  
 {
  
  decaysensor[numero_ex] = lightable[general_mempos]*2;
  
 if (lastbutton[touch_mempos[numero_ex]]==zero  )

{
{ //decaysensor[numero] = decaysensor_value; 
scala_reset = 1;
mouse_wheel_speed_counter=0;
noteOn(typetable[touch_mempos[numero_ex]+(page)], valuetable[touch_mempos[numero_ex]+(page)], 127,1);  
  
lastbutton[touch_mempos[numero_ex]] = uno;  //shifter.setAll(LOW); 
encoder_block[numero_ex] = 64;


bit_write(1,maxvalue[touch_mempos[numero_ex]]-1+page,1);
#if (shifter_active == 1 && stratos == 0)
 shifter.setPin(maxvalue[touch_mempos[numero_ex]]-1,HIGH);  // accendo il led del touch , nella tabella di memoria
 #endif
#if ( stratos == 1)


 digitalWrite(16,HIGH);
  #endif 

#if (touch_led_onboard == 1 && capacitivesensor_active < 3)
  digitalWrite(8,HIGH);
#endif
  
// bit_write(1,4+numero+page,1);
//  if (page==0) ledrestore(0); else ledrestore(1);  shifterwrite=1;
shifterwrite=1;};// opencalibration[1]=HIGH;

   }

 //  decaysensor[numero] = decaysensor_value;
   }
   } 
 
 }
#endif

//----------------------------------------------------------------------------------------------------------------------------------
 
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
        matrix_restore (page);
      //  Serial.println("matrix-restore");
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



  
