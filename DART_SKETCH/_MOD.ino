void mouse_control () 
{
   #if defined (__AVR_ATmega32U4__)  
  if (mouse_mempos != 0)
  { /// tutta la sezione mouse
  if (chan == minvalue[mouse_mempos] )   {// sezione mouse  // asse X
  
  mousex = 119 + (((valore+1)/64)) ;  
 if (mousex >133 ) {if ( qwertyvalue[mouse_mempos] < 255 ) qwertyvalue[mouse_mempos]++;} else  {qwertyvalue[mouse_mempos]= 0;}

#if (mouse_block == 1)
if (qwertyvalue[mouse_mempos] < 230)
#endif

{
  if  (dmxtable[mouse_mempos] == 1){  // 1 = attivazione mouse  
   Mouse.move( (mousex-127)*
 ((boolean(lightable[mouse_mempos])*2  // inversione di direzione
 ) -1), 0, 0); 
 
 }
 }

 
 if (dmxtable[mouse_mempos] == 2) // 2 = attivazione freccette - destra sinistra
 {
 if (valore>upper_val) { if (lastbutton[minvalue[mouse_mempos]] != 2) { Keyboard.press(216); lastbutton[minvalue[mouse_mempos]]=2;  }}
 else
 if (valore<lower_val) { if (lastbutton[minvalue[mouse_mempos]] != 0) { Keyboard.press(215); lastbutton[minvalue[mouse_mempos]]=0;  }}
 else { if (lastbutton[minvalue[mouse_mempos]] != 1) { Keyboard.release(215); Keyboard.release(216);  lastbutton[minvalue[mouse_mempos]]=1;}
 } 
 #if (arrows_block == 1)
 if (qwertyvalue[mouse_mempos] > 230) { Keyboard.release(215); Keyboard.release(216); }
 #endif
 }
  
 
 }
 
   else if (chan == maxvalue[mouse_mempos])  { // asse Y
  
   mousex = 119 + (((valore+1)/64)) ;
   if (mousex >133 ) {if ( mousey < 255 ) mousey++;} else  {mousey= 0;}
 
  #if (mouse_block == 1)
  if (mousey < 230)
  #endif
 {
 if  (dmxtable[mouse_mempos] == 1) { 
    
 Mouse.move( 0 , (mousex-127)*((boolean(lightable[mouse_mempos])*2) -1), 0); }
 }
  
 if (dmxtable[mouse_mempos] == 2) // 2 = attivazione freccette - sopra sotto
 {
 if (valore>upper_val) { if (lastbutton[maxvalue[mouse_mempos]] != 2) { Keyboard.press(218); lastbutton[maxvalue[mouse_mempos]]=2;  }}
 else
 if (valore<lower_val) { if (lastbutton[maxvalue[mouse_mempos]] != 0) { Keyboard.press(217); lastbutton[maxvalue[mouse_mempos]]=0;  }}
 else { if (lastbutton[maxvalue[mouse_mempos]] != 1) { Keyboard.release(218); Keyboard.release(217);  lastbutton[maxvalue[mouse_mempos]]=1;}
 } 
  #if (arrows_block == 1)
  if (mousey > 230){ Keyboard.release(218); Keyboard.release(217);  }
  #endif
  
 }
                    
}
}
#endif
}


void beam()  /////////////////////////////////////////// BEAM SENSOR
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
    if ( beam_scala_buffer != at) // se ÃƒÆ’Ã‚Â¨ diverso dal precedente sgnale inviato - evitare doppioni e note ripetute
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

void pageswitch(){ //----------------------------------------------------- PAGE SWITCH
  
  if (lastbutton[page_mempos] > 0  ) {     
  
  if (pagestate==1 ) { 
    page = 0;
//     load_preset_base();
    load_preset(page);
    update_scala(0); // primo spinner
    update_scala(1); // secondo spinner
    #if (shifter_active == 1 && stratos == 0)
    shifter.setAll(LOW);
    
    shifterwrite=1;
    ledrestore(page);
    #endif
  //  incomingByte = boolean(page);

midiOut(typetable[page_mempos],valuetable[page_mempos],minvalue[page_mempos]);

 #if (page_LEDs == 1)
if (valuetable[general_mempos] == 0 && lightable[page_mempos]>0) {
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
    
   // lower_Xen[0] = max(averageXen[0], readingsXen[indexXen]); // min - serve a dare un ritocco in basso alla soglia del touchsensor.
 
   //lower_Xen[0] = averageXen[0];
  //  lower_Xen[1] = averageXen[1];
  higher_Xen[0]= 40;
  higher_Xen[1]=40;
    lower_Xen[0]= 100;
  lower_Xen[1]=100;
//test3();

    }
  } 
  
  
  if (lastbutton[page_mempos]==0  ){ // seconda
 
 if (pagestate==0) {
      page = max_modifiers;
 
 

    load_preset(page);
    update_scala(0); // primo spinner
    update_scala(1); // secondo spinner
    #if (shifter_active == 1 && stratos == 0 )
    shifter.setAll(LOW);
    
   shifterwrite=1;
    ledrestore(page);
    #endif
   // incomingByte = boolean(page);
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

 #if (capacitivesensor_active > 0)
 
/*void touch_sensors() {
  
  if (decaysensor[0] > 0 ) decaysensor[0]--;
  if (decaysensor[1] > 0 ) decaysensor[1]--;

 #if (capacitivesensor_active ==1 ) // primo touch sempre in lettura 
 #if touch_version == 1
 readingsXen[indexXen]  = (cs_4_2[0].capacitiveSensorRaw(72));
 #endif
  #if touch_version == 2

 readingsXen[indexXen]  = (cs_4_2[0].capacitiveSensorRaw(8000)/64);
  #endif
  #endif

 #if (capacitivesensor_active ==1 && stratos == 0)                   // lettura secondo touch
 // if (dmxtable[general_mempos] >1)                                   // solo se il secondo spinner è in lettura.
 if (touch_mempos[1] >0)                                               // solo se il preset contiene i settings del secondo touch
   #if touch_version == 1
   readingsXen2[indexXen] = (cs_4_2[1].capacitiveSensorRaw(72));
   #endif
   #if touch_version == 2
   //  readingsXen2[indexXen] = (cs_4_3.capacitiveSensorRaw(4096)/32);
   readingsXen2[indexXen] = (cs_4_3[1].capacitiveSensorRaw(8000)/64);
  #endif
  #endif

  indexXen++; if (indexXen == 3) indexXen = 0;
                     
  averageXen[0] = ((readingsXen[0] + readingsXen[1] + readingsXen[2])  / 3) ;  

  #if ( stratos == 0 )
  averageXen[1] = ((readingsXen2[0] + readingsXen2[1] + readingsXen2[2])  / 3) ; 
  #endif
  
   
///////////////////////////////////////////////////////////////////////////////////////////
  if (lightable[touch_mempos[0]] == 0)  touch_execute(0); // lightable : 0-capacitive, 1-virtual, 2-monitoring.
  else if  (lightable[touch_mempos[0]] == 2) { noteOn(176, 127,   averageXen[0] ,0);  delay(20); } // monitoring
////////////////////////////////////////////////////////////////////////////////////////////////
  if (minvalue[general_mempos] == 0) { // extraplex disattivato
  if ( lightable[touch_mempos[1]] == 0) touch_execute(1);   
  else if  (lightable[touch_mempos[1]] == 2) { noteOn(177, 127,   averageXen[1] ,0);  delay(20); }  // monitoring

  }

{
if (averageXen[0] > higher_Xen[0])  higher_Xen[0] = averageXen[0] ;
if (averageXen[0] < lower_Xen[0] && averageXen[0] == readingsXen[indexXen] )  lower_Xen[0] = averageXen[0] ;

#if (stratos == 0)
if (averageXen[1] > higher_Xen[1])  higher_Xen[1] = averageXen[1] ;
if (averageXen[1] < lower_Xen[1] && averageXen[1] == readingsXen2[indexXen])  lower_Xen[1] = averageXen[1] ;
#endif
/// limit_touch = constrain( map ( lower_Xen[0],0,43,50,255) ,0,255)   ;
}
              
// sistema di autolimiting : 
// in base alla lettura touch piÃ¹ bassa viene stabilito il limite di velocitÃ  delle letture (interno alla libreria capacitivetouch)
// questo sistema rende retrocompatibile l'algoritmo di sensing nche con i circuiti vecchi, che eramo molto piÃ¹ sensibili e necessitavano di piÃ¹ letture
// piÃ¹ letture richiedono piÃ¹ tempo, rendendo il touchsensor piÃ¹ soggetto a interferenze da parte del DMX o di allungamenti e accorciamenti del loop principale
// dovuti all'uso del side spinner per esempio...

}
*/
void touch_sensors(byte T_numero) {
  
  if (decaysensor[T_numero] > 0 ) decaysensor[T_numero]--;
 // if (decaysensor[1] > 0 ) decaysensor[1]--;

 #if (capacitivesensor_active ==1 ) // primo touch sempre in lettura 
 if (touch_mempos[T_numero] >0)                    
     #if touch_version == 1
     readingsXen[T_numero][indexXen] = (cs_4_2[T_numero].capacitiveSensorRaw(72));
     #endif
     #if touch_version == 2
     readingsXen[T_numero][indexXen]  = (cs_4_2[T_numero].capacitiveSensorRaw(8000)/64);
     #endif
 #endif

 /*#if (capacitivesensor_active ==1 && stratos == 0)                   // lettura secondo touch

 if (touch_mempos[1] >0)                                               // solo se il preset contiene i settings del secondo touch
   #if touch_version == 1
   readingsXen2[indexXen] = (cs_4_2[1].capacitiveSensorRaw(72));
   #endif
   #if touch_version == 2
   //  readingsXen2[indexXen] = (cs_4_3.capacitiveSensorRaw(4096)/32);
   readingsXen2[indexXen] = (cs_4_3[1].capacitiveSensorRaw(8000)/64);
  #endif
  #endif
  */

  
                     
  averageXen[T_numero] = ((readingsXen[T_numero][0] + readingsXen[T_numero][1] + readingsXen[T_numero][2])  / 3) ;  

  /* #if ( stratos == 0 )
  averageXen[1] = ((readingsXen2[0] + readingsXen2[1] + readingsXen2[2])  / 3) ; 
  #endif
  */
  
   
///////////////////////////////////////////////////////////////////////////////////////////
  if (lightable[touch_mempos[T_numero]] == 0)  touch_execute(T_numero); // lightable : 0-capacitive, 1-virtual, 2-monitoring.
  else if  (lightable[touch_mempos[T_numero]] == 2) { noteOn(176, 127,   averageXen[T_numero] ,0);  delay(20); }          // monitoring dei valori del touch, si possono osservare da editor.
////////////////////////////////////////////////////////////////////////////////////////////////
 /* if (minvalue[general_mempos] == 0) { // extraplex disattivato
  if ( lightable[touch_mempos[1]] == 0) touch_execute(1);   
  else if  (lightable[touch_mempos[1]] == 2) { noteOn(177, 127,   averageXen[1] ,0);  delay(20); }  // monitoring
  }
  */

{
if (averageXen[T_numero] > higher_Xen[T_numero])  higher_Xen[T_numero] = averageXen[T_numero] ;                                       
if (averageXen[T_numero] < lower_Xen[T_numero] && averageXen[T_numero] == readingsXen[T_numero][indexXen] )  lower_Xen[T_numero] = averageXen[T_numero] ;    
 // limite superiore e inferiore si adattano ale circostanze ambientali, in tel modo la soglia decisa via editor è sempre relativa al range


/*#if (stratos == 0)
if (averageXen[1] > higher_Xen[1])  higher_Xen[1] = averageXen[1] ;
if (averageXen[1] < lower_Xen[1] && averageXen[1] == readingsXen2[indexXen])  lower_Xen[1] = averageXen[1] ;
#endif
*/

/// limit_touch = constrain( map ( lower_Xen[0],0,43,50,255) ,0,255)   ;
}
              
// sistema di autolimiting : 
// in base alla lettura touch piÃ¹ bassa viene stabilito il limite di velocitÃ  delle letture (interno alla libreria capacitivetouch)
// questo sistema rende retrocompatibile l'algoritmo di sensing nche con i circuiti vecchi, che eramo molto piÃ¹ sensibili e necessitavano di piÃ¹ letture
// piÃ¹ letture richiedono piÃ¹ tempo, rendendo il touchsensor piÃ¹ soggetto a interferenze da parte del DMX o di allungamenti e accorciamenti del loop principale
// dovuti all'uso del side spinner per esempio...

}

#endif

#if (capacitivesensor_active > 0)
 
void touch_execute (byte numero) 
{
   {                                                     
 if (    averageXen[numero]<

 // (minvalue[touch_mempos[numero]])

 (higher_Xen[numero]) 
  - map( minvalue[touch_mempos[numero]],0,112,0,higher_Xen[numero]-lower_Xen[numero])  // metto 112 e non 127 - per creare piÃ¹ separazione tra spazio accensione e spazio spegnimento
//- minvalue[touch_mempos[numero]]-5

 && decaysensor[numero] <= 0 
 && lastbutton[touch_mempos[numero]]==0) { 
 { noteOn(typetable[touch_mempos[numero]+(page)], valuetable[touch_mempos[numero]+(page)], 0,1); // MIDIUSB.flush(); 
decaysensor[numero] = 0;
 //scala_learn =0; // resetta la scala che viene memorizzata tramite learn ogni volta che si tocca la wheel
//scala_reset = 1;

 if (typetable[encoder_mempos[numero]+page] < 160) 
 // se encoder impostato su note allora manda il note off ( al rilascio)
 // serve per scale mode, per non lsciare note accese
 {
 button(typetable[encoder_mempos[numero]+page],encodervaluepot_buffer[numero],0 ,1);  
 }
 
 if (qwertyvalue[touch_mempos[numero]] == 1)  // touch reset 
 { 
       // se impostato su cc o altro - manda il touch reset
       // button(typetable[encoder_mempos[numero]+(page)],valuetable[encoder_mempos[numero]+(page)],dmxtable[touch_mempos[numero]],1); 
       // touch reset normale

int smooth = (encodervaluepot[numero]/8)-dmxtable[touch_mempos[numero]]; // ritorno in modo graduale alla posizione originale
   for (byte i = 8; i > 0; i--)
   {
    button(typetable[encoder_mempos[numero]+(page)],valuetable[encoder_mempos[numero]+(page)],dmxtable[touch_mempos[numero]]+(smooth/8)*(i-1),1);
     delay(15);
    }
      encodervaluepot[numero] = dmxtable[touch_mempos[numero]]*8;
 }
 
 

lastbutton[touch_mempos[numero]] = 1; encled=0; 
encoder_block[numero] = 64;

bit_write(1,maxvalue[touch_mempos[numero]]-1+page,0); // spengo il led nella tabella di memoria


#if (shifter_active == 1 && stratos == 0)
// shifter.setAll(LOW);  
if (page==0) ledrestore(0); else ledrestore(1);  shifterwrite=1; 
#endif
#if ( stratos == 1)
digitalWrite(16,LOW);
#endif


// opencalibration[1]=LOW;
}

} 


if (    averageXen[numero]> 
// (minvalue[touch_mempos[numero]]) + 5
 ((higher_Xen[numero]))  // piÃƒÂ¹ abbasso questo valore piÃƒÂ¹ aumenta la sensibilitÃƒÂ 
                         // sottraggo  (minvalue[touch_mempos[numero]]) - piÃƒÂ¹ aumenta piÃƒÂ¹ ÃƒÂ¨ sensibile
  - map( minvalue[touch_mempos[numero]],0,127,0,higher_Xen[numero]-lower_Xen[numero]) // la sottrazione ÃƒÂ¨ proporzionale ad higher_xen
 //- minvalue[touch_mempos[numero]]
  )
 {
 if (lastbutton[touch_mempos[numero]]==1 && decaysensor[numero] <= 0 )

{
{ //decaysensor[numero] = decaysensor_value; 
scala_reset = 1;
mouse_wheel_speed_counter=0;
noteOn(typetable[touch_mempos[numero]+(page)], valuetable[touch_mempos[numero]+(page)], 127,1);  // MIDIUSB.flush();  
  
lastbutton[touch_mempos[numero]] = 0;  //shifter.setAll(LOW); 
encoder_block[numero] = 64;

bit_write(1,maxvalue[touch_mempos[numero]]-1+page,1);
#if (shifter_active == 1 && stratos == 0)
 shifter.setPin(maxvalue[touch_mempos[numero]]-1,HIGH);  // accendo il led del touch , nella tabella di memoria
 #endif
#if ( stratos == 1)


 digitalWrite(16,HIGH);
  #endif 
// bit_write(1,4+numero+page,1);
//  if (page==0) ledrestore(0); else ledrestore(1);  shifterwrite=1;
shifterwrite=1;};// opencalibration[1]=HIGH;

   }

   decaysensor[numero] = decaysensor_value;
   }
   } 
 
 }

 #endif
 


 //----------------------------------------------------------------------------------------------------------------------------------
 void virtual_touch_end(byte numero)
{
  
      if (cycletimer == 10) 
      { 
      encled=0;
      
      // utilizzo inomingbyte (boolean(page) - ÃƒÆ’Ã‚Â¨ na variabile normalmente utilizzata per il midi-in, per on creare nuove variabili la riutilizzo
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
              // se la modetable del touch ÃƒÆ’Ã‚Â¨ in toggle, avremo un virtual-touch-reset sul valore definito da DMXtable del touch.
                                }
     V_touch_regulator[numero]= 1; 
     
     }
     #if (shifter_active == 1 && stratos == 0)
        ledrestore(page);  shifterwrite=1;
        #endif
        
 
    // se l'encoder non gira piÃƒÆ’Ã‚Â¹ bisogna fare ledrestore
      }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------



void encoder(byte numero) // 
// adesso il numero accettato corrisponderà alla chan/memoryposition

{

  if (lastbutton[numero] != 64)// MODIFICO // if (lastbutton[encoder_mempos[numero]] != 64)
    { 
     
      byte tocco;  
     cycletimer = 0;
     
    // MODIFICO // encoder_block[numero] = encoder_block[numero]+(lastbutton[encoder_mempos[numero]] -64);
    if (modetable[numero]>20) // no encoder generico -  solo se modetable è 21 o 22 devi eseguire i comendi in parentesi 
                              //- nota: in encoder() entreranno solo "numero" 19, 21 e 22 - quindi l'encoder generico è escluso da correlazioni col touch
       {
        numero2 = modetable[numero]-21;
        tocco = lastbutton[touch_mempos[numero2]];
        encoder_block[numero2] = encoder_block[numero2]+(lastbutton[numero] -64);
        
        if (V_touch_regulator[numero2] == 1 &&  lightable[touch_mempos[numero2]] == 1){ 
  
  // spiegazione : switchstate[touch]/ V_touch_regulator serve come regolatore anti-doppioni, 
  // lightable[touch] serve come attivatore-disattivatore da editor, 
  // quando non si desidera la funzione virtual-touch
  
 midiOut(typetable[touch_mempos[numero2]+page]+1,valuetable[touch_mempos[numero2]+page],127);// invio del virtual touch 
 #if (shifter_active == 1 && stratos == 0)
  shifter.setPin(4+numero2,HIGH); (shifterwrite =1);
  #endif
 }
V_touch_regulator[numero2] = 0; // lastbutton encoder 1 e 2 mempos si possono usare ...
        } else {numero2 = 2; tocco = 0;}



 
      

 // contoencoder = encoder_mempos[numero] ;
 //contoencoder = numero; 
 //-------------------------------------------------------------------------------------------------------------------------------
 
 
  if (dmxtable[general_mempos] >= numero2  ) // vedi se gli spinner sono attivati e quali 0-nessuno 1-top 2-top+side 3 top+side+generico
  //if (dmxtable[general_mempos] == 100 )
  {
   if (dmxtable[numero] > 1  )  // la dmxtable definisce la modalita  - 0 e 1 endless - 2 e 3 pot e scale
 {  
  if (qwertyvalue[numero]==0) {encoder_pot_mode(numero); }// vedi se touchstop attivato. - no
  else if (lastbutton[touch_mempos[numero2]]  ==0)    encoder_pot_mode(numero); // else encled = 254;          // touch stop  attivato
 }
 else                                                                                           // poly at, cc, pc, at 

 //-------------------------------------------------------------------------------------------------------------------------------
 // dmx : 0,1  =endless; 2= pot
 // qwerty: touchstop - 0 spento; 1=acceso; (non esiste touchstop virtuale)
 // min: speed e inversione - il valore va da 0 a 64 - 32 sarebe lo zero.
 // max: usato per trasferimento scale
 //-------------------------------------------------------------------------------------------------------------------------------

 { 
    if (minvalue[numero] >= 32) { // normall or inverted?
    
    if (dmxtable[numero]==0)      // modalita  endless<63 >65
   
    
    {   button(typetable[numero+page]+boolean(qwertyvalue[numero])*tocco, 
    // spiegazione:
    // boolean(qwertyvalue[encoder_mempos[numero]]) // se il touchstop = attivato ,// questyvalue viene caricata a ogni cambio PAGE
    // lastbutton[touch_mempos[numero]] //  se il touch = premuto o no // sostituita con byte tocco, che per gli encoder semplici è 0.
    // quindi col touchstop attivato, i messaggi endless cambiano canale al tocco.
    valuetable[numero+page], 
      // MODIFICO//  lastbutton[encoder_mempos[numero]],0); 
       lastbutton[numero],0); 

        
        }  
        
    else // ---------------- modalità 0-127
    {button(typetable[numero+page]+boolean(qwertyvalue[numero])*tocco,
    valuetable[numero+page], 
          constrain((lastbutton[numero]-63)*64, 0,127),0);   }   
           
    encled = encled + (lastbutton[numero]-64)*(minvalue[numero]-32) ; } // endless mode 0-127
   
  else{  // - minvalue sotto 32 inverte il funzionamento ----------------------------------------------------
    
  if (dmxtable[numero]==0)     // ----------------------modalita  endless<63 >65
  {   button(typetable[numero+page]+boolean(qwertyvalue[numero])*tocco,
  valuetable[numero+page], 
      -(lastbutton[numero]-64)+64, 0);  } 
  else {  // ---------------- modalità 0-127
   button(typetable[numero+(page)]+boolean(qwertyvalue[numero])*tocco,
   valuetable[numero+(page)], 
   constrain(-(lastbutton[numero]-64)*127, 0,127),0);}
   
   encled = encled + (lastbutton[numero]-64)*(minvalue[numero]-32); }
#if (shifter_active == 1 && stratos == 0)
    if (modetable[numero]>20) led_enc_exe();
   #endif
 }
//-------------------------------------------------------------------------------------------------------------------------------

   #if defined (__AVR_ATmega32U4__)  
   if (  //valuetable[mouse_mempos+page] > 0 && 
   valuetable[mouse_mempos+page] == numero+1) { // nuova regola - per attivare la mousewheel, usare la memoryposition dell'encoder scelto
    // l'encoder scelto funziona come mouse wheel 
    //- valuetable[mouse_mempos]   0 = spento, 1 = mousewheel su main encoder, 2 = mousewheel su encoder secondario. 
   if (mouse_wheel_speed_counter == 0)  //  mouse_wheel_speed_counter ÃƒÆ’Ã‚Â¨ un rallentatore
   Mouse.move(0,0,(lastbutton[numero]-64)*(constrain(minvalue[numero]-32,-1,1) ) ); // l'encoder funziona come mouse wheel
   
   mouse_wheel_speed_counter++;
 //  if (mouse_wheel_speed_counter ==  constrain (map(minvalue[numero+page],0,8,32,1),1,32)) 
   if (mouse_wheel_speed_counter ==  map(abs(minvalue[numero]-32),0,32,32,1)       ) 
   {
   mouse_wheel_speed_counter=0;
   }
   } 
#endif
   
    }
    
   
 //  led_enc_exe();
lastbutton[numero] = 64; // 
} 
}

 
  void encoder_pot_mode (byte numero) {

  int valuepot;
  encoder_pot_calcolo(numero,minvalue[numero]);  // canale e velocità
  
  if (modetable[numero] > 19){   valuepot = encodervaluepot[numero2]; }
  else valuepot = lightable[numero]*4;
  
  if (typetable[numero+page] > 223 )  // pitch bend 
  {
   // encoder_pot_calcolo(numero2);
    
     noteOn(     typetable[numero + (page)], 
     (valuepot - ((valuepot / 8) * 8)) * 16,  
     valuepot / 8 , 1) ;
     
    }
    
  else 
  if (typetable[numero+page] > 159 )   // pot normali - sotto il 160 sono note on e off
  { //encoder_pot_calcolo(numero2);
  button(typetable[numero+page],valuetable[numero+page],valuepot/8 ,1);} 
  
  else                                 // se typetable = NOTE - viene attivata la modalita scale
  if (mouse_wheel_speed_counter == 0 && modetable[numero] > 19) 
  // mouse_wheel_speed_counter - viene usato qui come rallentatore, 
  // modetable serve a far funzionre scale solo per gli spinner
  {
   // encoder_pot_calcolo(numero2);
    at = encodervaluepot[numero2]/8; // at è la posizione 0-127 sulla tastiera di note ideale // numero2 viene deciso da encoder() - ok
    
    if ( encodervaluepot_buffer[numero2] != at) // se diverso dal precedente sgnale inviato - evitare doppioni e note ripetute
    {
      
  if (dmxtable[numero] == 2) 
  // siamo in POT mode, se si imposte NOTE come type, allora si suona la scala fissa
  // se dmxtable = 3 allora si  usa la scala_learn
  scala_learn = scala[numero2+((page/max_modifiers)*2)]; // carico la scala fissa che mi interessa, dall'array scala[4]
  // scala[4] è un array che cotiene 4 scale... scalaSpinner0-page1, scalaSpinner1-page1, ScalaSpinner0-page2, scalaSpinner1-page2
  
  
     if (scala_learn != 0 ) // ci sono delle note?
     if (encoder_block[numero2] <50 || encoder_block[numero2] > 78) 
     // encoder block serve per non fare suonare note per sbaglio
     // mentre si registra una scala toccando la wheel
     // quindi la wheel inizia a suonare solo dopo aver superato un certo spazio di rotazione...
     { 
      encoder_block[numero2] = 128; 
      // quando la scala comincia a essere suonata 
      // encoder_block viene portato fuori range
      // in modo che la scala sia suonabile senza interruzioni.
      
      if  ( bitRead(scala_learn, (at)-((at/12)*12)) == 1)  // scala_learn
      {                                                                                // se trovi una nota, suonala 
        button(typetable[numero+page],encodervaluepot_buffer[numero2],0 ,0);
        button(typetable[numero+page],encodervaluepot[numero2]/8,127 ,1);  
        encodervaluepot_buffer[numero2]= at;
      }
       else                                                                // altrimenti vai avanti in direzione della prossima,
                                                                           // avanti o indietro secondo il senso di rotazione                                                                                        
       {
        while ( bitRead(scala_learn, (at)-((at/12)*12)) == 0)              // finchè trovi il vuoto vai avanti
        {
        encoder_pot_calcolo(numero,1);
        at = encodervaluepot[numero2]/8;
        if (at <= 0 || at >= 127) break;
       }
         if  ( bitRead(scala_learn, (at)-((at/12)*12)) == 1) 
         {     
        button(typetable[numero+page],encodervaluepot_buffer[numero2],0 ,0);
        button(typetable[numero+page],encodervaluepot[numero2]/8,127 ,1);  
        encodervaluepot_buffer[numero2]= at;}
        }
    }
     } 
  }

  ///////////////

  
  mouse_wheel_speed_counter++;
  if (mouse_wheel_speed_counter > abs(minvalue[numero]-32)) mouse_wheel_speed_counter =0;

#if (shifter_active == 1 && stratos == 0)
 ledrestore(page);
  // encled = abs((encodervaluepot[numero2]/4)-255);
  encled = abs((valuepot/4)-255);
  #endif
  
 
 #if (shifter_active == 1 && stratos == 0)
 led_enc_exe();
 #endif
  }


 void encoder_pot_calcolo(byte chan_, byte moltiplicatore)
 {
// moltiplicatore = (minvalue[chan_]-32);
// if (minvalue[chan_] == 32 )  moltiplicatore= 1;
 
   if (modetable[chan_] > 19 ) 
   
{
  

 encodervaluepot[numero2]                                                               // contiene il valore-posizione attuale dello spinner 1 o 2
        
= (constrain(encodervaluepot[numero2] - (lastbutton[chan_]-64)*(moltiplicatore-32),
0, 1023));  
// spegazione: si aggiunge il valore speed, positio o negativo a seconda del verso.

 }
 else
 {
  lightable[chan_]                                                                       // contiene il valore-posizione attuale dell'encoder generico
= (constrain( lightable[chan_]  - (lastbutton[chan_]-64)*(moltiplicatore-32),
 0, 255));  

  }
 // if (modetable[numero] > 19 ) encodervaluepot[numero] = calcolo; else lightable[numero] = calcolo/8;
  
 }

 # if(stratos == 0)
void senseEncoder_2nd ()
{

 // Serial.println(MSB[1]);
switch (channel) { //// 2nd enc scanning
case 1 : { // MSB2 = map(getValue(5,5), 0 ,1024, 0, 2);//MSB = map(getValue(1,5), 0 ,1024, 0, 2);
setPlexer(5); //analogRead(multiplexer);

// MSB[1] = map(analogRead(5), 0 ,1024, 0, 2);
MSB[1] = digitalRead(A5);
LSB[1] = digitalRead(A4);
updateEncoder(encoder_mempos[1]); 
encoder(encoder_mempos[1]);

}; 
break;
case 6 : {  // QUANDO CHANNEL Si strova su 6 vuol dire che il setplexer() è ancora messo su 5, che è l'input da leggere
MSB[1] = digitalRead(A5);
LSB[1] = digitalRead(A4);
updateEncoder(encoder_mempos[1]); 
encoder(encoder_mempos[1]);
};
break;

}

}
#endif





void lettura_enc_principale()   // legge l'encoder principale , collegato ad pins con capacità interrupt.
{
   MSB[0] = digitalRead(2); //MSB = most significant bit
   LSB[0] = digitalRead(3); //LSB = least significant bit
   updateEncoder(encoder_mempos[0]);
}
/*
#if (stratos == 1) 
void lettura_enc_principale2()
{
   MSB[1] = digitalRead(0); //MSB = most significant bit
   LSB[1] = digitalRead(1); //LSB = least significant bit
   updateEncoder(16);
}
#endif
*/
  
void updateEncoder(byte numero)

{
  byte numero2;
  if (numero == encoder_mempos[0]) numero2=0; else numero2 = 1;
  
    byte encoded = (MSB[numero2] << 1) |LSB[numero2]; //converting the 2 pin value to single number
    byte sum  = (maxvalue[numero] << 2) | encoded; //adding it to the previous encoded value

    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) { 
    lastbutton[numero]++;
   // test2();
    }
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) { 
    lastbutton[numero]--;
    
    }

    maxvalue[numero]= encoded; //store this value for next time
}

//---------------------------------------------------------------------------------------------------------------
  void push_buttons()
  {
    {


    if (valore < lower_val                            ///// button pushed
        && lastbutton[chan] == 1    
        ) {
        
        
      lastbutton[chan] = 0;
      if (modetable[chan] >= 3) offgroup(chan,1);      // da 3 in poi ci sono i toggle groups
      
            if (bit_read(4,page+chan)==0)              // something happens only if the button is off in the toggletable
            { 
               if ( eeprom_preset_active == 0 ) {dmxtable[chan]++;  
               
        
        }
           #if defined (__AVR_ATmega32U4__)  
            HOT_keys(chan,1);    
            #endif  
            outnucleo (1,chan);
           // lower_Xen[0] = 200;
            if (typetable[chan+page] < 160) scale_learn(valuetable[chan+page]);   
            #if (shifter_active == 1 && stratos == 0)    
            ledControl(chan, 1);
            ledrestore(page);
            #endif
          //  test3();     
            }
      cycletimer = 0;
      shifterwrite = 1;
    }

    
    if (valore > upper_val && lastbutton[chan] == 0)   ///// button released
    {
      lastbutton[chan] = 1;

     //  if (page == 0) 
      {
        if (modetable[chan] >= 2) // se il pulsante = toggle o t-group
        { 
             if (bit_read(4,page+chan) == 1) { // se il pulsante ÃƒÆ’Ã‚Â¨ in toggle ed ÃƒÆ’Ã‚Â¨ acceso
             #if defined (__AVR_ATmega32U4__)  
             HOT_keys(chan,0);
             #endif
             outnucleo (0,chan);
             #if (shifter_active == 1 && stratos == 0)
             ledControl(chan, 0);
              #endif
           
             }      
          bit_write(4,chan+page,!bit_read(4,page+chan));        
        }  
             else {  // se il pulsante NON ÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¨ in toggle o in uno dei gruppi
         
        //  if (bit_read(4,page+chan) == 1) 
          {  
            #if defined (__AVR_ATmega32U4__)  
              HOT_keys(chan,0);
              #endif
             outnucleo (0,chan);
             #if (shifter_active == 1 && stratos == 0)
              if (bit_read(4,page+chan) == 0)  ledControl(chan,0);
              #endif
              }
              bit_write(4,chan+page,0);
             
             }
                    }

    /*  if (page != 0) {
        if (modetable[chan] >= 2) {
          if ( bit_read(4,max_modifiers+chan) == 1 ) {   
          HOT_keys(chan,0);
          outnucleo (0);
          ledControl(0); 
          }
          bit_write(4,max_modifiers+chan,!bit_read(4,max_modifiers+chan));   }
        else {
        HOT_keys(chan,0);
        ledControl(0);     }
      }*/
      shifterwrite = 1;
    }

   //  lastbutton[chan] = valore / 4;

  }  ////////////////////////////// PUSH BUTTON SECTION END
  }



//---------------------------------------------------------------------------------------------------------
 void HOT_keys (byte canale, byte pressione) 
 // se un modificatore e' impostato su messaggio qwerty (>0), il valore di minvalue 
 // (non utile per il note off) viene utilizzato per selezionare i modificatori qwerty 

{
     #if defined (__AVR_ATmega32U4__)  

 //  Serial.println(minvalue[canale]);
  
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
}


void pots ()
{
 
  {
    
    if (abs((lastbutton[chan] * 4)  - valore) > 10 
       )                                                     // the potentiometer has been moved
       
   if (qwertyvalue[chan] > 0 && eeprom_preset_active != 0) {                             // pot working in qwerty mode - slo se non siamo in autodetect e qwerty ha un valore
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
   else                                                       // pot working in MIDI mode

   
   {

 lastbutton[chan] = valore / 4 ;
 
      if (modetable[chan] ==11) if ((typetable[chan + (page)]) < 224) valore = map(valore, 63, 960, minvalue[chan], maxvalue[chan]) ;  
      else if (modetable[chan  ] == 12) { valore = map(valore, 63, 256, minvalue[chan], maxvalue[chan]) ;                             // hypercurve 1
      #if (shifter_active == 1 && stratos == 0)
      shifter.setAll(LOW); 
      #endif
      }
      else if (modetable[chan  ] == 13) { valore = map(valore, 768, 960, minvalue[chan], maxvalue[chan]) ;                            // hypercurve 2
      #if (shifter_active == 1 && stratos == 0) 
      shifter.setAll(LOW); 
      #endif
      }
    
     else if (modetable[chan] == 14){ 
       
     if (valore < 448) { valore = map(valore, 63, 448, 0, 64) ; }  // hypercurve center
     else 
     if (valore > 576) { { valore = map(valore, 576, 960, 64, 127) ; } }
     else valore= 64;   }  
 
     else if (modetable[chan] == 15){ 
       
     if (valore < 340) { valore = map(valore, 63, 340, 0, 64) ; }  // hypercurve center
     else 
     if (valore > 684) { { valore = map(valore, 684, 960, 64, 127) ; } }
     else valore= 64;    }
     potOut = constrain(valore,0,127);

     
      encled = abs( 15 - ((potOut) / 8)) * 16 ;
      switch ( (typetable[chan + (page)] - 144)  / 16)
      {
        case 0 :   noteOn(typetable[chan + (page)]+32, valuetable[chan + (page)],  potOut, 1) ; break; // if (chan < 8) noteOn(176, chan,  valore/8, 0) ; break;// note
        case 1 :   noteOn(typetable[chan + (page)], valuetable[chan + (page)],  potOut, 1) ; break; // poly AT
        case 2 :   noteOn(typetable[chan + (page)], valuetable[chan + (page)],  potOut, 1) ;
 
        break; // cc
        case 3 :   //noteOn(176, chan,  valore/8, 0) ; break; // pc
        case 4 :   noteOn(typetable[chan + (page)], potOut,  0, 1) ; break; // channel AT
        case 5 :  {
           valore =constrain(map(valore,24,1000, 0,1024),0,1023);                                           // PB
         // valore = constrain(map(valore,24,1000, 0,1024),0,1023);
            noteOn(typetable[chan + (page)], (valore - ((valore / 8) * 8)) * 16,  valore / 8 , 1) ;
             encled = abs( 15 - ((valore) / 64)) * 16 ;
          }
          break; // PB
      }

      

        cycletimer = 0;

       #if (shifter_active == 1 && stratos == 0)
       if (lightable[chan] > 0) led_enc_exe();
       #endif

       #if (DMX_active == 1  && stratos == 0)
  //if  (dmxtable[chan] == 100 ) 
   if (eeprom_preset_active == 1 )  DmxSimple.write(dmxtable[chan], potOut*2); 
 //  Serial.println("a");
#endif
   

     

    }/////////////////////////////////////////////////



  }
}
