
  void midifeedback ()
  {

  
  {
    // read the incoming byte:
    

      if (incomingByte==241){  // this message opens the editor upload mode
      openeditor= !openeditor;
      if (openeditor == 1) 
      {
        editorcounter=0;

      } else {    
       load_preset_base();
       load_preset(page);        // load preset from eerom memory after upload     
    for(int led = 0; led < 8; led++)    {   
    bit_status[4][led]=0; bit_status[5][led]=0;
    bit_status[0][led]=0; bit_status[1][led]=0;    } // all leds and toggles off
    #if (shifter_active == 1 && stratos == 0)
    shifter.setAll(0); shifterwrite =0;
    #endif
     }
     }
     
    else ///////////////////////////     

    if (   incomingByte>= 128 && incomingByte<= 191)
    {   action=1;
        type=incomingByte;
        if (openeditor==0){ 
           byte note_off_case; // se ricevo un note off devo farlo comportare come un note on velocity zero.
           if (incomingByte < 144 ) note_off_case =16; else  note_off_case =0;
           for(byte ledA = 0; ledA < max_modifiers*2; ledA++)   bit_write(2,ledA,(type+note_off_case==(typetable[ledA]))); 
           }
     }
     
///////////////////////////////////////////////////////////////////////////////////////////////////////////
   /*       else if (incomingByte>= 160 && incomingByte<= 175){ // l'eftertouch serve per i LED!!!  32, come valore fuori range, per avere effetto ZERO.
      action=1;
      type=incomingByte;
      }*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////  
       else  if ( (action==0)&&(note==255) ){ // do nothing
      action=2;  }
///////////////////////////////////////////////////////////////////////////////////////////////////////////
    else  if ( action==1 && note==255 ) { //  wait for the note (databyte)
      note=incomingByte;
      if (openeditor==0) 
      for( byte ledA = 0; ledA < max_modifiers*2; ledA++)        {       
          if (valuetable[ledA]== note ) bit_write(3,ledA,bit_read(2,ledA)); }
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////
    else if ( (action==1)&&(note!=255) ){ // ...and then the velocity
    velocity=incomingByte;
    if (type < 144 ) velocity = 0 ;
///////////////////////////////////////////////////////////////////////////////////////////////////////////    
    if ( openeditor == 1 ) 
     eeprom_write(); // procedura che contiene le istruzioni per mettere in memoria il dato ricevuto.
     //-----------------------------------------------------------------------
     
 else {

  
 if (velocity!=0 )                                            // ricevuto segnale ACCESO
 {
   
  if (type < 160) scale_learn(note); 
  
  #if (MIDI_thru == 1)
   if (type < 160)   noteOn(type,note,127,0); else   noteOn(type,note,velocity,0);   // le note vengono sempre sparate fuori al massimo.
  #endif
 
for(int ledD = 0; ledD < max_modifiers; ledD++) {                                     // elaborazione led feedback
    
if (valuetable[ledD]==note && bit_read(3,ledD) == 1   )  {    // prima pagina
      bit_write(1, lightable[ledD], 1);                       // il led relativo al pulsante (ricevuto) viene memorizzato come acceso
      old_pitch = 126;
        if (modetable[ledD] >= 3) offgroup(ledD,1);
      
      if (page==0) {                                          // agisci solo se la pagina = 0
        #if (shifter_active == 1 && stratos == 0)
        shifter.setPin(lightable[ledD]-1,1); shifterwrite=1;    // il led viene acceso 
      #endif
      
      if (valuetable[general_mempos] != 0 ) {
         ledControl(ledD, 1);
      //    Serial.println(remapper(ledD)); //
        //  Serial.println(remapper(ledD));
        //  Serial.println(lightable[ledD]);
         //  Serial.println(lightable[ledD+1]);
          //  Serial.println(lightable[ledD-1]);
       // digitalWrite(5, 1);
        }
       #if (DMX_active == 1  && stratos == 0)
    DmxSimple.write(dmxtable[ledD], velocity);
      #endif
      }
     // if (cycletimer < 3 ) 
      {   // midi feedback su toggle
                              
                            if (modetable[ledD] >1 && modetable[ledD] < 11)  // se il pulsante ÃƒÂ¨ in toggle e togglegroups
                            { lastbutton[ledD]=0;   bit_write(4,ledD,0);  } 
                            
                            else 
                            
                            if (modetable[ledD] == 1)                        // se il pulsante ÃƒÂ¨ button mode semplice
                            {  bit_write(4,ledD,1); 
                            // lastbutton[ledD] = 0;
                            } 
                            } } // nb: lastbutton viene usato in modo diverso per pulsanti o pot! 
 
if (valuetable[ledD+max_modifiers]==note && bit_read(3,ledD+max_modifiers) == 1)  {  // seconda pagina
      bit_write(1, lightable[ledD]+max_modifiers, 1);
      old_pitch = 126;
      
      if (page!=0) {                                            // agisci solo se sei su seconda pagina
        #if (shifter_active == 1 && stratos == 0)
        shifter.setPin(lightable[ledD]-1,1); shifterwrite=1; 
      #endif
       if (valuetable[general_mempos] != 0 ) {
         ledControl(ledD, 1);
        // digitalWrite(4, 1);
        }
       #if (DMX_active == 1  && stratos == 0)
    DmxSimple.write(dmxtable[ledD], velocity); 
      #endif
      
      } 
    //  if (cycletimer < 3 ) 
      {   //  delay for midi feedback-loops - avoids prblems with midi loops and doubled messages                         
      if (modetable[ledD] >1 && modetable[ledD] < 11)  { lastbutton[ledD]=0;  bit_write(4,ledD+max_modifiers,0); }  
       else if  (modetable[ledD] == 1) {  bit_write(4,ledD+max_modifiers,1); } 
      } }
  }  
 } 
 
 
if (velocity==0 ){

  #if (MIDI_thru == 1)
  noteOn(type,note,velocity,0);
  #endif
  
      for(int ledE = 0; ledE < max_modifiers; ledE++) { // shifter.setPin(led, ledstatus2[led]);   // elaborazione led feedback
      
if (valuetable[ledE]==note && bit_read(3,ledE) ==1    )    {   
   bit_write(1, lightable[ledE], 0); // ledstatus
   bit_write(3,ledE,0);              // feedback2
   
   if (page==0) {
    #if (shifter_active == 1 && stratos == 0)
    shifter.setPin(lightable[ledE]-1,0); shifterwrite=1;
   #endif
    if (valuetable[general_mempos] != 0 ) {
        ledControl(ledE, 0);
        }
    #if (DMX_active == 1  && stratos == 0)
   DmxSimple.write(dmxtable[ledE], 0);
   #endif
   }
                  // toggletable
      if (modetable[ledE] >1 && modetable[ledE] < 11){ lastbutton[ledE]=1; 
      // nota: nella void push_button, lastbutton viene portato a zero quando un pulsante ÃƒÂ¨ PREMUTO
      // - viene autorizzato a mandare midiON solo se lastbutton ÃƒÂ¨ 1
      bit_write(4,ledE,0); 
      // nota: quando la toggletable (4) ÃƒÂ¨ == 1 , la presione del pulsante genera midiout (ved void pushbuttons)
      } 
        else if  (modetable[ledE] == 1) {  bit_write(4,ledE,0); } 
      } 
  
if (valuetable[ledE+max_modifiers]==note && bit_read(3,ledE+max_modifiers) ==1    )   {
   bit_write(1, lightable[ledE]+max_modifiers, 0); 
   bit_write(3,ledE+max_modifiers,0);  
   
 
   if (page!=0) {
      #if (shifter_active == 1 && stratos == 0) 
    shifter.setPin(lightable[ledE]-1,0); shifterwrite=1; 
       #endif
        if (valuetable[general_mempos] != 0 ) {
        ledControl(ledE, 0);
        }
        #if (DMX_active == 1  && stratos == 0)
         DmxSimple.write(dmxtable[ledE], 0);
         #endif
    } 

   
    
      if (modetable[ledE] >1 && modetable[ledE] < 11) { lastbutton[ledE]=1;   bit_write(4,ledE+max_modifiers,0); }
        else if  (modetable[ledE] == 1) {  bit_write(4,ledE+max_modifiers,0); } 
      }    
      }      
    }  
 }   
      note=255; // out of range (0,127) value
      action=0;  
    }
    else{
      // nothing
    }   
  }  
  } 
  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void load_preset_base() {       //carica le variabili che devono restare in memoria indipendentemnte per le due pagine, per gestire il feedback. 
   
  for (int i = 0; i <max_modifiers; i++) {
  typetable[i] = EEPROM.read(i);
  valuetable[i] = EEPROM.read(i+64);
  typetable[i+max_modifiers] = EEPROM.read(i+(512));
  valuetable[i+max_modifiers] = EEPROM.read(i+64+(512));
  }
 
}

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

     void load_preset(boolean numero) // numero is the page to load  - 0 1 // carica le variabili dalla EEPROM alla memoria esectiva, secondo la pagnica in cui ci si trova.
  {
    #if (shifter_active == 1 && stratos == 0)
     if (valuetable[general_mempos] == 0 ) {shifter.setAll(HIGH); shifter.write();} // flash di luci quando viene caricato il preset
    #endif
// if (numero > 0) numero = 1;

eeprom_preset_active = 0;

   for (byte i = 0; i <max_modifiers; i++) {
    
    modetable[i]= EEPROM.read(i+128+(numero*512));
    setup_mempos(i);                                // dopo aver caricato la modetable facciamo il settaggio macchina
    
  dmxtable[i]= EEPROM.read(i+192+(numero*512));
  if (i == mouse_mempos) {
    minvalue[i] = remapper(EEPROM.read(i+256+(numero*512))-1); 
    maxvalue[i] = remapper(EEPROM.read(i+320+(numero*512))-1);
    
    if (dmxtable[i] != 0) // quando vengono attivate le funzioni MOUSE, XY del joystick vengono impostate su BLIND INPUT - in tal modo si evitano conflitti
    {modetable[minvalue[mouse_mempos]] = 19;
    modetable[maxvalue[mouse_mempos]] = 19;}
  }
  else {
    minvalue[i]= EEPROM.read(i+256+(numero*512)); // if (dmxtable[mouse_mempos] == 2)
    maxvalue[i] =EEPROM.read(i+320+(numero*512));
    if (modetable[i] == 21) lastEncoded[0] = maxvalue[i];
    if (modetable[i] == 22) lastEncoded[1] = maxvalue[i];
  }
  
  // quando da editor si specifica la posizione su ircuito degli assi x e y - viene usato il remap da numerazione semplificata a numerazione normale
 //  if (i == mouse_mempos) maxvalue[i] = remapper(EEPROM.read(i+320+(numero*512))-1); else  maxvalue[i] =EEPROM.read(i+320+(numero*512));
 qwertyvalue[i] =EEPROM.read(i+384+(numero*512));
 

 }

    delay(5);
  #if (shifter_active == 1 && stratos == 0)
   if (valuetable[general_mempos] == 0 ) {shifter.setAll(LOW); shifter.write();}
  #endif
  
  
       if ( eeprom_preset_active == 0) // aux viene impostato su 1 dalla void setup_mempos richamata sopra - alla riga 194 - se ÃƒÂ¨ presente un general mempos.
       { aux_preset();
 
   
       }else  {   
        for (byte i = 0; i <max_modifiers; i++)  
       lightable[i] = EEPROM.read(i+448);
       
       }                        
  }
  
  void update_scala(byte quale_spinner) // quale_spinner è 0 top 1 side
  
  // richiamato da setup_mempos() e da switchpage()
  {
    // scala[4] è un array che cotiene 4 scale... scalaSpinner0-page1, scalaSpinner1-page1, ScalaSpinner0-page2, scalaSpinner1-page2
    // per caricare una scala dalla eeprom di servono 12 BIT - per le 12 note di un'ottava
    // i primi 7 BIT sono dentro valuetable
    // gli altri 5 BIT sono dentro lastencoded[2]
    // lastEncoded[] viene ricaricata ogni volta che si carica un preset - quindi è sempre "attuale" rispetto alla pagina.
  
  for (byte i = 0; i< 7; i++) {
bitWrite(scala[quale_spinner+(page/max_modifiers)*2],i,  bitRead(valuetable[encoder_mempos[quale_spinner]+max_modifiers] ,i)   );

//bitWrite(scala[numero+2],i,  bitRead(valuetable[encoder_mempos[numero]+max_modifiers] ,i)   );
}
for (byte i = 0; i< 5; i++) {
bitWrite(scala[quale_spinner+(page/max_modifiers)*2],i+7,  bitRead(lastEncoded[quale_spinner] , i)    );
// bitWrite(scala[numero+2],i+7,  bitRead(lastEncoded[numero] , i)    );

/*
 // old version 
  for (byte i = 0; i< 7; i++) {
bitWrite(scala[numero],i,  bitRead(valuetable[encoder_mempos[numero]] ,i)   );
bitWrite(scala[numero+2],i,  bitRead(valuetable[encoder_mempos[numero]+max_modifiers] ,i)   );
}
for (byte i = 0; i< 5; i++) {
bitWrite(scala[numero],i+7,  bitRead(maxvalue[encoder_mempos[numero]] , i)    );
bitWrite(scala[numero+2],i+7,  bitRead(maxvalue[encoder_mempos[numero]+max_modifiers] , i)    );
*/

}

  }

  void setup_mempos (byte i)  // richiamato da load_preset
  {
   
     if ( modetable[i] == 18) distance_mempos = i;
  //____________________________________________________________________________
  if ( modetable[i] == 21) {encoder_mempos[0] = i;update_scala(0);}
  if ( modetable[i] == 22) {encoder_mempos[1] = i;update_scala(1);}
  //______________________________________________________________________________
  if ( modetable[i] == 23) touch_mempos[0] = i;
  if ( modetable[i] == 24) touch_mempos[1] = i;
  if ( modetable[i] == 25) mouse_mempos = i;
  if ( modetable[i] == 20) PADS_mempos = i;
  if ( modetable[i] == 17) page_mempos = i; 
  if ( modetable[i] == 26) {general_mempos = i; eeprom_preset_active = 1;}
  if ( dmxtable[general_mempos] >  1) { modetable[45] = 0; modetable[37] =0;} // disattiva lo scanning sugli input dedicati al secondo encoder
  
  }

 #if (stratos == 1)
 void aux_preset() // preset di base caricato all'avvio soltanto se il preset sulla eeprom non ÃƒÆ’Ã‚Â¨ valido. 
 {

   for (byte i = 0; i <max_modifiers; i++) {
    modetable[i]=1;

    { /// stratos
      valuetable[i+page]=    60+i; 
    valuetable[i]=    60+i;
    }
    
    
    minvalue[i]=0;
    maxvalue[i]=127;
    dmxtable[i]=i;
    qwertyvalue[i]=0;
    typetable[i]=144; /// 144 note 176 cc
    typetable[i+page]=144;
    lightable[i]=remapper(i);
    }

    // nota: i modificatori normali vanno da 0 a 55 nel normale ciclo AIN e nelle tabelle array
    // rimangono solo 4 spazi liberi in teoria
    // ma bisogna considerare che : se l'extraplex ÃƒÂ¨ disattivato, tutti gli spazi da 48 a 55 sono disponibili
    // se si attiva il secondo encoder, gli slot di memoria relativi ai pin 46 e 48 (numerazione ciclo AIN) sono disponibili
    // se si attiva l'extraplex il secondo touch non ha bisogno di settaggio
    // nel default preset l'extraplex non viene attivato
    
   page_mempos=53; 
   mouse_mempos = 54;
   encoder_mempos[1] = 55;
   encoder_mempos[0] = 56;
   
    touch_mempos[0] = 57;        
    touch_mempos[1] = 58;
       
    general_mempos = 59;
    
  
    
     
   
    
    dmxtable[mouse_mempos]= 0; // disabilita il mouse
    
   
    dmxtable[encoder_mempos[0]] = 1; // encoder mode
    minvalue[encoder_mempos[0]] = 36;  // speed - il centro è 32
    qwertyvalue[encoder_mempos[0]] = 0; // touchstop
    modetable[encoder_mempos[0]]= 21;  
    lastbutton[encoder_mempos[0]] = 64;
    typetable[encoder_mempos[0]] = 176;

#if (stratos == 1)
    modetable[14] = 19;
    typetable[14] = 176;
    dmxtable[14] = 1;
    minvalue[14] = 32;
    lastbutton[14] = 64;
    
    modetable[16] = 19;
    typetable[16] = 176;
    dmxtable[16] = 1;
    minvalue[16] = 1;
    lastbutton[16] = 64;
    #endif
    
    
    
    lightable[touch_mempos[0]] = 0; // touch mode 0 normal 1 virtual 2 testmode
    lightable[touch_mempos[1]] = 0;
   
    minvalue [touch_mempos[0]] = 70; // stratos touch sensitivity
    typetable[touch_mempos[0]] = 176; // message type stratos

    qwertyvalue[general_mempos]=0; // no efx
    maxvalue[general_mempos] = 34; // no pads
    minvalue[general_mempos] = 0; // no extraplex
    dmxtable[general_mempos] = 4; // 0 no enc 1 top enc 2 top&side enc 3 generic encoders. 
   
   
 }
 #endif

 #if (stratos == 0) // dart one
 void aux_preset() // preset di base caricato all'avvio soltanto se il preset sulla eeprom non Ã¨ valido. 
 {

   for (byte i = 0; i <max_modifiers; i++) {
    modetable[i]=1;
    valuetable[i+page]=    60+
    (i/8)*8; // di default, tutte le note emesse sono uguali. in questo modo, se non si montano multiplexers,  ogni in triggera otto note, che essendo tutte uguali verranno filtrate dal midiout in una solo nota.
    valuetable[i]=    60+
    (i/8)*8;
    minvalue[i]=0;
    maxvalue[i]=127;
    dmxtable[i]=0;
    qwertyvalue[i]=0;
    typetable[i]=144;
    typetable[i+page]=144;
    lightable[i]=remapper(i);
    }

 
    // nota: i modificatori normali vanno da 0 a 55 nel normale ciclo AIN e nelle tabelle array
    // rimangono solo 4 spazi liberi in teoria
    // ma bisogna considerare che : se l'extraplex è disattivato, tutti gli spazi da 48 a 55 sono disponibili
    // se si attiva il secondo encoder, gli slot di memoria relativi ai pin 46 e 48 (numerazione ciclo AIN) sono disponibili
    // se si attiva l'extraplex il secondo touch non ha bisogno di settaggio

    // nel default preset l'extraplex non viene attivato
   page_mempos=53; 
   mouse_mempos = 54;
   encoder_mempos[0] = 55;
   encoder_mempos[1] = 56;
   
    touch_mempos[0] = 57;        
    touch_mempos[1] = 58;
       
    general_mempos = 59;
    
  
    
     
   
    
    dmxtable[mouse_mempos]= 0; // disabilita il mouse
    
   
    dmxtable[encoder_mempos[0]] = 1; // encoder mode
    minvalue[encoder_mempos[0]] = 10;  // speed 
    lastbutton[encoder_mempos[0]] = 64;
    typetable[encoder_mempos[0]] = 176;
    typetable[encoder_mempos[1]] = 176;
    
  
    lightable[touch_mempos[0]] = 0; // no virtual touch
    lightable[touch_mempos[1]] = 0;
    typetable[touch_mempos[0]] = 144;
    typetable[touch_mempos[1]] = 145;
   // qwertyvalue[touch_mempos[0]]=0;
   // qwertyvalue[touch_mempos[1]]=0;
    minvalue[touch_mempos[0]] = 70;
    minvalue[touch_mempos[1]] = 70;
    
    qwertyvalue[general_mempos]=0; // no efx
    maxvalue[general_mempos] = 34; // no pads
    minvalue[general_mempos] = 0; // no extraplex
    dmxtable[general_mempos] = 2; // no 2nd encoder
    valuetable[general_mempos] = 0; // shifters & multiplexers active.
   
 }
 #endif

 void eeprom_write()
 {
  {   
     switch (editorcounter) 
     {      
     case 0 :  //////////////////////////  NOTE - MEMORYPOSITION

      
      if ( velocity < 56 ) memoryposition = remapper(velocity); 
      else if ( velocity < 64 ) memoryposition = velocity;
      else if ( velocity < 120 )memoryposition = remapper(velocity); 
      else  memoryposition = velocity;
    
       if (memoryposition < 64) {

       // i messaggi vengono posizionati in eeprom in tale ordine:
       // 0-65 = type   512
       // 64-128 = value  576
       // 128-192 = toggle mode 640
       // 192-256 = DMX  704
       // 256-320 = min 768
       // 320-384 = max 832
       // 384-448 = nel korova viene memorizzatoo il keyboard 896
       // 448-512   -    960 
       
 
        EEPROM.write(memoryposition+64,note); // value
       }
       else{
        EEPROM.write(memoryposition+512+64-64,note);  // value 2nd
         }
         
       
     break;
     
     
     case 1 :  /////////////////////////////////// MIN - MAX
     if (memoryposition < 64) {
  
         EEPROM.write(memoryposition+320,note);  // max
        EEPROM.write(memoryposition+256,velocity);     // min
 } else{
          EEPROM.write(memoryposition+512+320-64,note);  // max 2nd
        EEPROM.write(memoryposition+512+256-64,velocity);     // min 2nd
   }
   
 
    
  
     break;
  
  
     case 2 : ///////////////////////////////////////////////////  MODE - DMX
     if (memoryposition < 64) { 
     
  EEPROM.write(memoryposition+128,note); // MODE
/*
 if (note == 26) // se arriva un general_mempos, mi assicuro che sia l'unico in memoria;

{ // byte anti_doubles;
  for (byte i =0; i< max_modifiers; i++)
{ if (i != memoryposition+128){ // anti_doubles == EEPROM.read(i+128);
// if (anti_doubles == 26) EEPROM.write(i+128,19);
if (EEPROM.read(i+128) == note) EEPROM.write(i+128,19);}  }
   }*/
  
// EEPROM.write(memoryposition+128+512-64,note);
  EEPROM.write(memoryposition+192,velocity); // dmx 

 
 } else{
   
 
   
   
  
   EEPROM.write(memoryposition+128+512-64,note); // MODE 2ND

  EEPROM.write(memoryposition+192+512-64,velocity); // dmx 2nd

   }
     break;
   
   
     case 3 : ///////////////////////////// keyboard - miditype
     if (memoryposition < 64) {
// EEPROM.write(memoryposition+448,note);
  EEPROM.write(memoryposition,type-176+(velocity*16)+144); // TYPE // velocity = miditype proveniente dall'editor numerato da 0 a 6 - viene moltiplicato per 16 e sistemato da 144 in poi a seconda del canale
 // type contiene ovviamente anche l'informazione del canale
     EEPROM.write(memoryposition+384,note); //  =  memorizzato a partire dalla posizione 384
 } else{


       EEPROM.write(memoryposition+512-64,type-176+(velocity*16)+144);  // type 2nd
       
   EEPROM.write(memoryposition+384+512-64,note); //  trying not to cross memory limit - qwertyvalue stored 64 memory slots before
 }
 
     break;
     
     case 4 : ///////////////////////////////                                   LED - /
            //  EEPROM.write(memoryposition+448,note);
       if (memoryposition < 64) {
      
     
    
EEPROM.write(memoryposition+448,note);
 
 } else{

 }
 
 
     break;
 
   }
     

     editorcounter++;
     if (editorcounter > 4){ editorcounter = 0;

     #if (shifter_active == 1 && stratos == 0)
   if (valuetable[general_mempos] == 0 )
   {shifter.setAll(HIGH); shifter.write();
     delay(5);
     shifter.setAll(LOW); shifter.write();}
     
     #endif
  }
     }
 }
