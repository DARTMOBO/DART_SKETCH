

void encoder(byte numero) // 

//  NUMERO  corrisponde alla chan/memoryposition

{

  if (lastbutton[numero] != 64)
    { 
     
      byte tocco;  
     cycletimer = 0;
     
    
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

 //-------------------------------------------------------------------------------------------------------------------------------
 
 
  if (dmxtable[general_mempos] >= numero2  ) // vedi se gli spinner sono attivati e quali 0-nessuno 1-top 2-top+side 3 top+side+generico
 
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
    if (minvalue[numero] >= 32) { // normal or inverted?
    
    if (dmxtable[numero]==0)      //   endless mode <63 >65
   
    
    {  button(typetable[numero+page]+boolean(qwertyvalue[numero])*tocco, 
    // spiegazione:
    // boolean(qwertyvalue[encoder_mempos[numero]]) // se il touchstop = attivato ,// questyvalue viene caricata a ogni cambio PAGE
    // lastbutton[touch_mempos[numero]] //  se il touch = premuto o no // sostituita con byte tocco, che per gli encoder semplici è 0.
    // quindi col touchstop attivato, i messaggi endless cambiano canale al tocco.
    valuetable[numero+page], 
  
    lastbutton[numero],0); 
 
      }  
        
    else // ---------------- modalità 0-127
    {button(typetable[numero+page]+boolean(qwertyvalue[numero])*tocco,
    valuetable[numero+page], 
          constrain((lastbutton[numero]-63)*64, 0,127),0);   }   
           
    encled[0] = encled[0] + (lastbutton[numero]-64)*(minvalue[numero]-32) ; } // endless mode 0-127
   
  else{  // - minvalue sotto 32 inverte il funzionamento ----------------------------------------------------
    
  if (dmxtable[numero]==0)     // ----------------------modalita  endless<63 >65
  {   button(typetable[numero+page]+boolean(qwertyvalue[numero])*tocco,
  valuetable[numero+page], 
      -(lastbutton[numero]-64)+64, 0);  } 
  else {  // ---------------- modalità 0-127
   button(typetable[numero+(page)]+boolean(qwertyvalue[numero])*tocco,
   valuetable[numero+(page)], 
   constrain(-(lastbutton[numero]-64)*127, 0,127),0);}
   
   encled[0] = encled[0] + (lastbutton[numero]-64)*(minvalue[numero]-32); }
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

//////////////////////////////////////////////////////////////////////////////////////////////////


 
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
  encled[0] = abs((valuepot/4)-255);
  #endif
  
 
 #if (shifter_active == 1 && stratos == 0)
 led_enc_exe();
 #endif
  }


///////////////////////////////////////////////////////////////////////////////////////////////////////////7

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

 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void lettura_enc_principale()   // legge l'encoder principale , collegato ad pins con capacità interrupt.
{
   MSB[0] = digitalRead(2); //MSB = most significant bit
   LSB[0] = digitalRead(3); //LSB = least significant bit
   updateEncoder(encoder_mempos[0]);
 //  lastbutton[encoder_mempos[1]]=64;
 //  updateEncoder_main();
}


  
void updateEncoder(byte numero)
{
  boolean numero2 =1;
  if (numero == encoder_mempos[0]) numero2=0; 
 
  
    byte encoded = (MSB[numero2] << 1) |LSB[numero2]; //converting the 2 pin value to single number
    byte sum  = (maxvalue[numero] << 2) | encoded; //adding it to the previous encoded value

    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) { 
    lastbutton[numero]++;
    }
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) { 
    lastbutton[numero]--; 
    }
    maxvalue[numero]= encoded; //store this value for next time
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





 
