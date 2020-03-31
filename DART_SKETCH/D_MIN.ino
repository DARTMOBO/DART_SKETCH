// MIDI IN


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
        reset_mempos();
       load_preset_base();
       load_preset(page);        // load preset from eerom memory after upload     
    for(int led = 0; led < 8; led++)    {   
   // bit_status[4][led]=0; bit_status[5][led]=0;
    bit_status[0][led]=0; bit_status[1][led]=0;   
    bit_status[2][led]=0; bit_status[3][led]=0; 
    
    } // all leds and toggles off
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
     

//-------------------------------------------------------------------------------------------
       else  if ( (action==0)&&(note==255) ){ // do nothing
      action=2;  }
//-------------------------------------------------------------------------------------------
else  if ( action==1 && note==255 ) { //  wait for the note (databyte)
      note=incomingByte;
      if (openeditor==0) 
      for( byte ledA = 0; ledA < max_modifiers*2; ledA++)        {       
          if (valuetable[ledA]== note ) bit_write(3,ledA,bit_read(2,ledA)); }
    }
//-------------------------------------------------------------------------------------------
else if ( (action==1)&&(note!=255) ){ // ...and then the velocity
    velocity=incomingByte;
    if (type < 144 ) velocity = 0 ;
//-------------------------------------------------------------------------------------------
   
    if ( openeditor == 1 ) 
    
     eeprom_write(); // procedura che contiene le istruzioni per mettere in memoria il dato ricevuto.
    
//------------------------------------------------------------------------------------------------------------------------------------------------




     
 else {  // normali procedure di feedback midi

  
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
       if ( lightable[ledD] > 0 ) {
        #if (shifter_active == 1 && stratos == 0)
       
        shifter.setPin(lightable[ledD]-1,1); shifterwrite=1;    
        // il led viene acceso 
        // da notare il "-1" , che serve per allineare la numerazione dell'editor, che parte da 1
      #endif
      
      if (valuetable[general_mempos] != 0 ) {   // nomobo
         ledControl(ledD, 1);
     
        }
       }
        
       #if (DMX_active == 1  && stratos == 0)
    DmxSimple.write(dmxtable[ledD], velocity);
      #endif
      }
      
     // if (cycletimer < 3 ) 
      {   // midi feedback su toggle
                              
                            if (modetable[ledD] >1 && modetable[ledD] < 11)  // se il pulsante e' in toggle e togglegroups
                            { lastbutton[ledD]=0;   bit_write(4,ledD,0);  } 
                            
                            else 
                            
                            if (modetable[ledD] == 1)                        // se il pulsante e' button mode semplice
                            {  bit_write(4,ledD,1); 
                            // lastbutton[ledD] = 0;
                            } 
                            } } // nb: lastbutton viene usato in modo diverso per pulsanti o pot! 


                            
 
if (valuetable[ledD+max_modifiers]==note && bit_read(3,ledD+max_modifiers) == 1)  {  // seconda pagina
      bit_write(1, lightable[ledD]+max_modifiers, 1);
      old_pitch = 126;
      
      if (page!=0) {                                            // agisci solo se sei su seconda pagina
       if ( lightable[ledD] > 0 ) {
        #if (shifter_active == 1 && stratos == 0)
        shifter.setPin(lightable[ledD]-1,1); shifterwrite=1; 
      #endif
       if (valuetable[general_mempos] != 0 ) {
         ledControl(ledD, 1);
        // digitalWrite(4, 1);
        }
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
    if ( lightable[ledE] > 0 ) {
    #if (shifter_active == 1 && stratos == 0)
    shifter.setPin(lightable[ledE]-1,0); shifterwrite=1;
   #endif
    if (valuetable[general_mempos] != 0 ) {
        ledControl(ledE, 0);
        }
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
     if ( lightable[ledE] > 0 ) {
      #if (shifter_active == 1 && stratos == 0) 
    shifter.setPin(lightable[ledE]-1,0); shifterwrite=1; 
       #endif
        if (valuetable[general_mempos] != 0 ) {
        ledControl(ledE, 0);
        }
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////










void reset_mempos ()
{
  mouse_mempos =0;
  
  }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
