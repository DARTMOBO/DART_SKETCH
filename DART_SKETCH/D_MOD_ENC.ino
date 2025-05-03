void encoder(byte numero) 
{
  //  NUMERO  corrisponde alla chan/memoryposition
 // lastbutton[] contiene la lettura dell encoder 64 = fermo, 63 = -, 65 = + 
 // modetable[] mi dice se si tratta di encoder spinner  21 22 o di encoder generico 19
 // dmxtable[general_mempos]  vedi se gli spinner sono attivati e quali 0-nessuno 1-top 2-top+side 3 top+side+generico
 // dmxtable[numero]   // la dmxtable definisce la modalita  - 0 e 1 endless - 2 pot    -  3  scale
 // qwertyvalue[numero] settaggio del touch stop
 // minvalue: speed e inversione - il valore va da 0 a 64  // 32 sarebe lo zero.
 // maxvalue: usato per trasferimento scale
 // lightable[numero] // usato come contatore per la modalità 63-65 che oltre allo scatto avanti/indietro fornisce anche la velocità
 // typetable[] oltre a specificare se si tratta di note o cc - può fare accedere l'encoder alla modalità qwerty 
 // - per controllare zooming o altro in vari software
 // - ved più avanti spegazioni o ved qwerty_encoderr (si mette typetable dc PC in su e valuetable va a determinare qualche carattere ascii triggerare)
 // dmxtable[mouse_mempos] emulazione mouse attiva
 //  valuetable[mouse_mempos+page] // scelta dell encoder che emula mousewheel
 

  if (lastbutton[numero] != 64)
    { 
   //     Serial.print("--  ");
   //     Serial.println(numero+1);
      byte tocco;  
     cycletimer = 0;
     
     // Serial.println(modetable[numero]);delay(100);
    
    if (modetable[numero]>20) // no encoder generico -  solo se modetable è 21 o 22 devi eseguire i comendi in parentesi 
                              //- nota: in encoder() entreranno solo "numero" 19, 21 e 22 - quindi l'encoder generico è escluso da correlazioni col touch
       {
         numero2h = boolean(modetable[numero]-21);   
        // VADO A DETERMINARE SE SI TRATTA DI SPINNER 1 O 2 - gli spinner sono 21 e 22 in elenco - quindi ottengo 0 spinner1 e 1 spinner2 - 
        // gli encoder generici sono sul numero 19 - risultato 0 cioe touch1 --  ved riga 42
       //  Serial.println(modetable[numero]);delay(100);
                                                       
        tocco = lastbutton[touch_mempos[numero2h]];   //   A QUALE TOUCH SENSOR DEVO FARE RIFERIMENTO
        encoder_block[numero2h] = constrain(encoder_block[numero2h]+(lastbutton[numero] -64), 20,235);
        
        if (V_touch_regulator[numero2h] == 1 &&  lightable[touch_mempos[numero2h]] == 1){ 
  
  // spiegazione : switchstate[touch]/ V_touch_regulator serve come regolatore anti-doppioni, 
  // lightable[touch] serve come attivatore-disattivatore da editor, 
  // quando non si desidera la funzione virtual-touch
  
 midiOut(typetable[touch_mempos[numero2h]+page]+1,valuetable[touch_mempos[numero2h]+page],127);// invio del virtual touch 
 #if (shifter_active == 1 && stratos == 0)
  shifter.setPin(4+numero2h,HIGH); (shifterwrite =1);
  #endif
 }
 
V_touch_regulator[numero2h] = 0; // lastbutton encoder 1 e 2 mempos si possono usare ...
        } else {numero2h = 0; // tocco = 0;
          tocco = lastbutton[touch_mempos[numero2h]];
        }

 //-------------------------------------------------------------------------------------------------------------------------------
 
 
  if (dmxtable[general_mempos] >= numero2h  ) // vedi se gli spinner sono attivati e quali 0-nessuno 1-top 2-top+side 3 top+side+generico
 
  {
   if (dmxtable[numero] > 1  )  // la dmxtable definisce la modalita  - 0 e 1 endless -            2 pot    -  3  scale
 {  
  // Serial.println(lastbutton[touch_mempos[numero2]]);delay(100); // modetable[numero]
  //   Serial.println(modetable[numero]);delay(100);
  if (qwertyvalue[numero]==0) {encoder_pot_mode(numero); }// vedi se touchstop attivato. - no
  else {if (lastbutton[touch_mempos[numero2h]]  == 0)    encoder_pot_mode(numero);} // else encled = 254;          // touch stop  attivato
 }
 else                                                                                           // poly at, cc, pc, at 

 //-------------------------------------------------------------------------------------------------------------------------------
 // dmx : 0,1  =endless; 2= pot
 // qwerty: touchstop - 0 spento; 1=acceso; (non esiste touchstop virtuale)
 // min: speed e inversione - il valore va da 0 a 64 - 32 sarebe lo zero.
 // max: usato per trasferimento scale
 //-------------------------------------------------------------------------------------------------------------------------------

 { 
    if (minvalue[numero] >= 32) // normal or inverted?
 { 
    
    if (dmxtable[numero]==0)      //   endless mode <63 >65
   {
    if (typetable[numero+page] <= 191 )

    {
                                        // spiegazione : c'è la possibilià di inviare qwerty usando l'encoder, una lettera in una direzione e una in un'altra
                                        // utile per controllare lo zoom,  per esempio in cubase
                                        // in carattere ascii da inviare è determinato da valuetable
                                        // vengono inviate quando siamo in modalità 63-65 
                                        // e solo quando il messaggio midi selezionato è maggiore di CC - quindi da PC in poi - da 192 in poi
   
     
   
    // spiegazione:
    // boolean(qwertyvalue[encoder_mempos[numero]]) // se il touchstop = attivato ,// quertyvalue viene caricata a ogni cambio PAGE
    // lastbutton[touch_mempos[numero]] //  se il touch = premuto o no // sostituita con byte tocco, che per gli encoder semplici è 0.
    // quindi col touchstop attivato, i messaggi endless cambiano canale al tocco.

   
                                       #if (stratos == 0 )
                                       if (lightable[numero] ==  16) 
                                       #endif
                                       #if (stratos == 1 )
                                       if (lightable[numero] ==  250) 
                                       #endif
                                       // perche 16 - è arbitrario - è un congruo tempo di attesa
                                                                     // per accumulare un allontanamento dallo zero centrale
                                                                     // ricordiamo che la modalità 63-65 vuole restituire anche un valore di velocità
                                                                     // cioò non riferisce solo uno scatto in basso o in alto ma anche di quanto ci siamo mossi
                                                                     // per stratos , che è velocissimo nelle letture i cilci di accumulo aumentano a 250
                                       {
                                       button(
                                       typetable[numero+page]
                                       +boolean(qwertyvalue[numero])*tocco, // QWERTYVALUE[numero] mi dice se è attivo il touchstop
                                       
                                       valuetable[numero+page], 

                                       constrain(
                                       lastbutton[numero]
                                       + ((lastbutton[numero]-64)*(minvalue[numero]-32)), // minvalue cioe speed , viene usato come moltiplicatore
                                                                                          // quindi l'allontanamento dal centro 64 aumenta 
                                                                                          // il valore ottimale di minvalue parrebbe essere al massimo 8
                                       0,127)
                                       
                                       ,0) ; 
                                    //   Serial.println(lastbutton[numero]
                                    //   + ((lastbutton[numero]-64)*(minvalue[numero]-32)));
                                       
                                       lightable[numero] = 0;
                                       encled[0] = encled[0] + (-(lastbutton[numero]-64))*(minvalue[numero]-32) ;
                                       lastbutton[numero] = 64;
                                       }
                                       else 
                                       lightable[numero] ++;
    
      }  
      
      else // qwerty encoder
      {   
        qwerty_encoderr(numero);
        lastbutton[numero] = 64;
    
      }
    }
    else // ---------------- modalità 0-127
    {
    if ( (qwertyvalue[numero] ^ tocco) != 3 )  // sistema per gestione touch stop
    {        
      button(typetable[numero+page]+boolean(qwertyvalue[numero])*tocco,
      valuetable[numero+page], 
      constrain((lastbutton[numero]-63)*64, 0,127),2);  

       //    Serial.println(encled[0]);
        encled[0] = encled[0] + (-(lastbutton[numero]-64)) *(minvalue[numero]-32) ;     
      //     Serial.println(encled[0]);
        //   Serial.println((lastbutton[numero]-64)*(minvalue[numero]-32));
        //Serial.println(lastbutton[numero]-64);
        // Serial.println("--");

         // encled[0] = encled[0] + (lastbutton[numero]-64)*(minvalue[numero]-32) ; // spiegazione : lastbutton è 63 o 65, quindi lastbutton -64 mi da +1 o -1
                                                                            // minvalue sarebbe un fattore di velocità-speed anche questo positivo o negativo
                                                                            // quindi io aggiungo o tolgo qualcosa al precedente encled[0], 
                                                                            // in base al verso di rotazione
    }   

    }
           
  
    
 } 


  else
{  // - minvalue sotto 32 inverte il funzionamento ---------------------------------------------------------------------------------------------
    
  if (dmxtable[numero]==0)     // --------modalita  endless<63 >65
  {
    if (typetable[numero+page] <= 191 ) {

                                         #if (stratos == 0 )
                                       if (lightable[numero] ==  16) 
                                       #endif
                                       #if (stratos == 1 )
                                       if (lightable[numero] ==  250) 
                                       #endif
                                       
                                       {button(typetable[numero+page]+boolean(qwertyvalue[numero])*tocco,
   
                                       valuetable[numero+page], 
  
                                     //  map(lastbutton[numero],0,127,127,0)
                                      constrain(
                                       map(lastbutton[numero],0,127,127,0)
                                       + ((lastbutton[numero]-64)*(minvalue[numero]-32)), // minvalue cioe speed , viene usato come moltiplicatore
                                                                                          // quindi l'allontanamento dal centro 64 aumenta 
                                                                                          // il valore ottimale di minvalue parrebbe essere al massimo 8
                                       0,127)
                                       
                                       ,0) ; 
                                       
                                       lightable[numero] = 0;
                                       encled[0] = encled[0] + (-(lastbutton[numero]-64))*(minvalue[numero]-32); 
                                       lastbutton[numero] = 64;
                                       }
                                       else 
                                       lightable[numero] ++;
    
      }  
      else
      {
          qwerty_encoderr(numero);
          lastbutton[numero] = 64;
        }
  }
   
  else {  // ---------------- modalità 0-127
     if ( (qwertyvalue[numero] ^ tocco) != 3 )
     {
   button(typetable[numero+(page)]+boolean(qwertyvalue[numero])*tocco,
   valuetable[numero+(page)], 
   constrain(-(lastbutton[numero]-64)*127, 0,127),2);   

   encled[0] = encled[0] + (-(lastbutton[numero]-64))*(minvalue[numero]-32); 
  // lastbutton[numero] = 64;
  // Serial.println(encled[0]);
   
  // Serial.println(minvalue[numero]);
  // Serial.println("--");
  }
   }
   
 
}
   
    #if (shifter_active == 1 && stratos == 0)
     if ( (qwertyvalue[numero] ^ tocco) != 3 ) 
     led_enc_exe();
    #endif

     //Serial.println(encled[0]);
     // Serial.println("-");
    
       #if (Matrix_Pads > 0 )  
       if ( (qwertyvalue[numero] ^ tocco) != 3 )  
       led_enc_exe_matrix();    
       #endif

     
 }
//-------------------------------------------------------------------------------------------------------------------------------

   #if defined (__AVR_ATmega32U4__)  
   
    // Serial.println(valuetable[mouse_mempos+page]);
    // Serial.println(numero);
    // Serial.println(remapper(valuetable[mouse_mempos+page]-1));
    // Serial.println("-");


//if (dmxtable[mouse_mempos] > 0)
{  // attivazione mousewheel
 // Serial.println("sono qui");
   //Serial.println(numero);
   // Serial.println(valuetable[mouse_mempos+page]);
     #if (stratos == 1)
   if (  //valuetable[mouse_mempos+page] > 0 && 
   valuetable[mouse_mempos+page] == numero) // scelta dell encoder che emula mousewheel
     #endif

      #if (stratos == 0)
   if (  //valuetable[mouse_mempos+page] > 0 && 
   remapper(valuetable[mouse_mempos+page]-1) == numero) 
     #endif
   
   { // nuova regola - per attivare la mousewheel, usare la memoryposition dell'encoder scelto
    // l'encoder scelto funziona come mouse wheel 

 // Serial.println("sono qui");
 
   #if (hid_mouse ==1)
   if (mouse_wheel_speed_counter == 0)  //  mouse_wheel_speed_counter un rallentatore
   Mouse.move(0,0,(-(lastbutton[numero]-64))*(constrain(minvalue[numero]-32,-1,1) ) ); // l'encoder funziona come mouse wheel
   #endif
   
   
   mouse_wheel_speed_counter++;
 //  if (mouse_wheel_speed_counter ==  constrain (map(minvalue[numero+page],0,8,32,1),1,32)) 
   if (mouse_wheel_speed_counter ==  map(abs(minvalue[numero]-32),0,32,32,1)       )   // sarebbe il valore SPEED del encoder
   {
   mouse_wheel_speed_counter=0;
   }
   } 
  }
#endif


  
    
    }
    
   
 //  led_enc_exe();
   // if (typetable[numero+page] > 191 ) 
   if (dmxtable[numero]>0) // in mdalita relativa con accelerazione (63-65) non devo resettare lastbutton...
   lastbutton[numero] = 64; // 
} 

}

//////////////////////////////////////////////////////////////////////////////////////////////////

  void encoder_pot_mode (byte numero) {

  int valuepot;
  encoder_pot_calcolo(numero,minvalue[numero]);  // canale e velocità/speed
  
  if (modetable[numero] > 19){   valuepot = encodervaluepot[numero2h]; 
 ///  Serial.println(valuepot);
  } // nel caso di spin1 e spin2
  else valuepot = lightable[numero]*4;                                  // nel caso di enc generico

  // ------------------------------------------------ // fin qui abbiamo calcolato la posizione 
  
  if (typetable[numero+page] > 223 )  // pitch bend 
  {
   // encoder_pot_calcolo(numero2);
    
     noteOn(     typetable[numero + (page)], 
     (valuepot - ((valuepot / 8) * 8)) * 16,  
     valuepot / 8 , 1) ;
    }
    
  else 
  if (typetable[numero+page] > 159 )   // pot mode normale - anche ramp - sotto il 160 sono note on e off
  { //encoder_pot_calcolo(numero2);
  button(typetable[numero+page],valuetable[numero+page],valuepot/8 ,1);
  // Serial.println(valuepot);
  } 

  
  
  else // se typetable = NOTE - viene attivata la modalita scale  ////////////////////////////// SCALE ///////////////////////////////////
  {
  if 
  ( modetable[numero] > 19 ) // modetable serve a far funzionre scale solo per gli spinner
  {
    at = encodervaluepot[numero2h]/8; // at è la posizione 0-127 sulla tastiera di note ideale // numero2 viene deciso da encoder() - ok
    
    if ( encodervaluepot_buffer[numero2h] != at) // se diverso dal precedente sgnale inviato - evitare doppioni e note ripetute
    {
      
  if (scala[numero2h+((boolean(page))*2)] != 0) 
  // siamo in POT mode, se si imposte NOTE come type, allora si suona la scala fissa
                  scala_learn = scala[numero2h+((boolean(page))*2)]; // carico la scala fissa che mi interessa, dall'array scala[4]
                 // scala[4] è un array che cotiene 4 scale... scalaSpinner0-page1, scalaSpinner1-page1, ScalaSpinner0-page2, scalaSpinner1-page2
  
  {
     if (scala_learn != 0 ) // ci sono delle note?
     {
    //  Serial.println(valuepot);
     if (encoder_block[numero2h] <50 || encoder_block[numero2h] > 78) 
     { 
        // encoder block serve per non fare suonare note per sbaglio
        // mentre si registra una scala toccando la wheel
        // quindi la wheel inizia a suonare solo dopo aver superato un certo spazio di rotazione...
    
       encoder_block[numero2h] = 220; 
      // quando la scala comincia a essere suonata 
      // encoder_block viene portato fuori range
      // in modo che la scala sia suonabile senza interruzioni.
      // encoder_block[] viene inizializzato a 64 e riportato a 64 ad ogni tocco touch sensor
      
      if  ( bitRead(scala_learn, (at)-((at/12)*12)) == 1)  // scala_learn // 1 = trovo una nota
      {                                                                                // se trovi una nota, suonala 
        button(typetable[numero+page],encodervaluepot_buffer[numero2h],0 ,0); // spegni nota precedente
      //  button(typetable[numero+page],encodervaluepot[numero2h]/8,127 ,1);    // suona nota trovata
          button(typetable[numero+page],at,127 ,1);                             // suona nota trovata
        encodervaluepot_buffer[numero2h]= at;                                 // ricordati la nota suonata per spegnerla dopo
      }
       else                                                                // altrimenti vai avanti in direzione della prossima,
                                                                           // avanti o indietro secondo il senso di rotazione                                                                                        
       {
        while ( bitRead(scala_learn, (at)-((at/12)*12)) == 0)              // finchè trovi zero vai avanti
        {
        //encoder_pot_calcolo(numero,33);
        encoder_pot_calcolo(numero,constrain(minvalue[numero],31,33));
     
       //   encoder_pot_calcolo(numero,32+(lastbutton[numero]-64));
       //   Serial.println((lastbutton[numero]-64));
                                         //ricordiamo  void encoder_pot_calcolo(byte chan_, byte moltiplicatore)   
                                         // il moltiplicatore è 33 perchè risulterà poi 1 (-32) ricordiamo range 0-64 con centro 32
                                        
                                        // quindi vado avanti con moltiplicatore 1
                                        // sto virtualmente spostando la rotella in avanti o indietro
      at = encodervaluepot[numero2h]/8;
      
        if (at <= 0 || at >= 127) { mouse_wheel_speed_counter = 0 ; break;} // ?? 
      
       }
        //  if  ( bitRead(scala_learn, (at)-((at/12)*12)) == 1)  // inutile conferma
         {     
        button(typetable[numero+page],encodervaluepot_buffer[numero2h],0 ,0); // spegni precedente
        button(typetable[numero+page],at,127 ,1);  
        encodervaluepot_buffer[numero2h]= at;}
        }
     
    }
    }////
  }
    
     } 
  }
  }

  ///////////////  fine modalità scale

  
  //mouse_wheel_speed_counter++;
  //if (mouse_wheel_speed_counter > abs(minvalue[numero]-32)) mouse_wheel_speed_counter =0; // minvalue qui usata come speed / rallentatore.
  // if (mouse_wheel_speed_counter > 4 ) mouse_wheel_speed_counter =0;  // rallentatore a quota fissa 


#if (shifter_active == 1 && stratos == 0)
 ledrestore(page);
  // encled = abs((encodervaluepot[numero2]/4)-255);
  #endif
  encled[0] = abs((valuepot/4)-255); // posizione effetto luminoso encoder - valuepot è 1024
  
  
 
 #if (shifter_active == 1 && stratos == 0)
  led_enc_exe();
 #endif

        #if (Matrix_Pads > 0 )
     //  if (lightable[chan] > 0) 
       led_enc_exe_matrix();
       #endif
  }

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void encoder_pot_calcolo(byte chan_, byte moltiplicatore)   
 {
  // serve per calcolare la posizione del pot virtuale - encoder in modalità pot
  // la posizione del pot viene memorizzata su encodervaluepot[] per gli spinner e su lightable[] per gli encoders generici
 
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
if (mouse_wheel_speed_counter > map(abs(minvalue[chan_]-32),0,32,32,0)  ) {mouse_wheel_speed_counter =0;}

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
   MSB[0] = digitalRead(2); //MSB = most significant bit
   LSB[0] = digitalRead(3); //LSB = least significant bit
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
