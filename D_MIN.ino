// MIDI IN

byte matrix_vert1;
byte matrix_vert2;
  
void midifeedback ()
  {
  {
    // read the incoming byte:
   
      if (incomingByte==241){  // this message opens the editor upload mode
      openeditor= !openeditor; // apri o chiudi la ricezione del preset
      if (openeditor == 1)  // se entri in modalità ricezione porta a zero il counter
      {
        editorcounter=0;
      } else {    
        reset_mempos();       
       load_preset_base();
       // load_preset(0);
       load_preset(page);        // load preset from eerom memory after upload   
        
       update_scala(1);  
       update_scala(0); 
       lastbutton[encoder_mempos[0]] = 0; // 
    for(int led = 0; led < 8; led++)    { // reset di tutti i led e tutti i banchi toggle   
   // bit_status[4][led]=0; bit_status[5][led]=0;
    bit_status[0][led]=0; bit_status[1][led]=0;   
    bit_status[2][led]=0; bit_status[3][led]=0; 
    
    } // all leds and toggles off
    #if (shifter_active == 1 && stratos == 0)
    shifter.setAll(0); shifterwrite =0; // spegni fisicamente tutti i leds
    #endif
    
    #if (stratos == 1)
    digitalWrite(16, LOW);
    #endif

  #if (stratos == 0)
   
    page_leds_(page);
      
    #endif
    
     }
     }
     
    else ///////////////////////////     se il messaggio non è un 241

    if (   incomingByte>= 128 && incomingByte<= 191 || incomingByte ==  224 ) // a partire da note off fino a control change 
                                                                              // il resto dei messaggio non viene ricevuto
                                                                              // 224 fa eccezione - è un messaggio pithbend che serve per gli sprites
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
   
    if ( openeditor == 1 ){
    if( type < 208) // fino a tutti i control-change la triade midi viene mandata alla procedura  eeprom_write()
                    // arrivati qui abbiamo ricevuto un messaggio midi completo e vado a caricare i dati sulla eeprom 
                    // se arriva un 224 invece...
    
     eeprom_write(); // procedura che contiene le istruzioni per mettere in memoria il dato ricevuto. // 
     
     else{  // basterà mandare un segnale pitch-bend // il contenuto dei due databyte forma il byte verticale per completare la matrice. // 224
      matrix_vert1 = note;
      matrix_vert2 = velocity;
      }
    }
//------------------------------------------------------------------------------------------------------------------------------------------------




     
 else {  // normali procedure di feedback midi // se openeditor == 0

  
 if (velocity!=0 )                                            // ricevuto segnale ACCESO
 {
   
  if (type < 160) scale_learn(note); 
  
  #if (MIDI_thru == 1)
   if (type < 160)   noteOn(type,note,127,0); else noteOn(type,note,velocity,0);   // le note vengono sempre sparate fuori al massimo.
  #endif

 // bit tables:
 // 1 - ledstatus 1 e 2
 // 2 - feedback_bit1
 // 3 - feedback_bit2
 // 4 - bit_toggle 1 e 2
 
for(int ledD = 0; ledD < max_modifiers; ledD++) {                                     // elaborazione led feedback
    
if (valuetable[ledD]==note && bit_read(3,ledD) == 1   )  {

if ( modetable[ledD] < 16){    
       #if (DMX_active == 1  && stratos == 0)
       DmxSimple.write(dmxtable[ledD], velocity*2);
       #endif
       
       }

  
if ( modetable[ledD] < 11) {    // prima pagina
     
      bit_write(1, lightable[ledD]-1, 1);                     // il led relativo al pulsante (ricevuto) viene memorizzato come acceso
      
      old_pitch = 126;                                        //  old_pitch viene usato nel filtro anti-doppioni interno alla void NoteOn 
                                                              //  (che invia i messagi midi definitivamente)
                                                              
    


                                                              
      //----------------------------------
      if (page==0) {                                          // accendi un led  SOLO SE la pagina = 0

      if (modetable[ledD] >= 3) offgroup(ledD,0);             // offgroup : canale, midiout (1 = manda midi) 
                                                              // se il pulsante è in un gruppo toggle, spengo gli altri pulsanti nel gruppo
        
       if ( lightable[ledD] > 0 ) {                           // se al pulsante è effettivamente associato un led. //  0 = nessun effetto visivo
        
 
      
      if (valuetable[general_mempos] != 0 )                      // nomobo mode  
      {
        #if (shifter_active == 1 && stratos == 0)
        ledControl(ledD, 1); 
        #endif
      
      } else {
      #if (shifter_active == 1 && stratos == 0)    
      shifter.setPin(lightable[ledD]-1,1); 
      shifterwrite=1; // il led viene acceso // da notare il "-1" , che serve per allineare la numerazione dell'editor, che parte da 1                          
      #endif

      #if (Matrix_Pads == 1 )
      single_h(matrix_remap[ledD],lightable[ledD],1,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo)
      #endif

       #if (Matrix_Pads == 2 )
      single_h(matrix_remap[ledD-16],lightable[ledD],1,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo)
      #endif
        
        }                                   
       }
        
  
      }
      
                                                  
                            {                                                     // midi feedback su toggle
                              
                            if (modetable[ledD] >1 && modetable[ledD] < 11)       // se il pulsante e' in toggle e togglegroups
                          //  { lastbutton[ledD]=0;   bit_write(4,ledD,0);  }     // 4 = toggletable
                              { //lastbutton[ledD]=0;  
                              bit_write(4,ledD,1); 
                                   }
                            else 
                            
                            if (modetable[ledD] == 1)                           // se il pulsante e' button mode semplice
                            {  bit_write(4,ledD,1);                             // 4 = toggletable
                            // lastbutton[ledD] = 0;
                            } 
                            
                            } 
                            
                            } // nb: lastbutton viene usato in modo diverso per pulsanti o pot! 
}

// -------------------------------------------------- 
                            
 
if (valuetable[ledD+max_modifiers]==note && bit_read(3,ledD+max_modifiers) == 1 
 )                       

{

 if (modetable[ledD+max_modifiers] < 16 ) {
      #if (DMX_active == 1  && stratos == 0)
      DmxSimple.write(dmxtable[ledD], velocity*2); 
      #endif
 }
    
  if (modetable[ledD+max_modifiers] < 11 )
// feedback solo per i pulsanti
{                                                               // seconda pagina
      
      bit_write(1, lightable[ledD]+max_modifiers-1, 1);         // il led relativo al pulsante (ricevuto) viene memorizzato come acceso
      
      old_pitch = 126;                                         //  old_pitch viene usato nel filtro anti-doppioni interno alla void NoteOn 
                                                               //  (che invia i messagi midi definitivamente)

       //----------------------------------
      if (page!=0) {                                            // agisci solo se sei su seconda pagina

      if (modetable[ledD] >= 3) offgroup(ledD,0);             // offgroup : canale, midiout (1 = manda midi) 
                                                              // se il pulsante è in un gruppo toggle, spengo gli altri pulsanti nel gruppo
       
        if ( lightable[ledD] > 0 ) {
          
       
      
       if (valuetable[general_mempos] != 0 ) { // se sei in modalità NOMOBO
        #if (shifter_active == 1 && stratos == 0)
         ledControl(ledD, 1);
         #endif
       
        }else{
          
          #if (shifter_active == 1 && stratos == 0)
        shifter.setPin(lightable[ledD]-1,1); 
        shifterwrite=1; 
        #endif

        #if (Matrix_Pads == 1 )
        single_h(matrix_remap[ledD],lightable[ledD],1,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo)
        #endif 

        #if (Matrix_Pads == 2 )
        single_h(matrix_remap[ledD-16],lightable[ledD],1,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo)
        #endif 
          }
        
       }
    //  #if (DMX_active == 1  && stratos == 0)
    //  DmxSimple.write(dmxtable[ledD], velocity); 
    //  #endif
      
      } 
      
   
      {                       
      if (modetable[ledD] >1 && modetable[ledD] < 11)  { 
       bit_write(4,ledD+max_modifiers,1); 
      }  
       else if  (modetable[ledD] == 1) {  bit_write(4,ledD+max_modifiers,1); } 
      } } }
  }  
 } 
 
 ///////////////////////
 //////////////////////  ---------------------------------------------------------------------------------------------------------------
 /////////////////////
if (velocity==0 ){

  #if (MIDI_thru == 1)
  noteOn(type,note,velocity,0);
  #endif
  
      for(int ledE = 0; ledE < max_modifiers; ledE++) { // shifter.setPin(led, ledstatus2[led]);   // elaborazione led feedback
      
if (valuetable[ledE]==note && bit_read(3,ledE) ==1   
               // il feedback visivo funziona solo per i pulsanti, non per i pot e altro
)   

{
if( modetable[ledE] < 16) {
   #if (DMX_active == 1  && stratos == 0)
   DmxSimple.write(dmxtable[ledE], 0);
   #endif
}
  
  if( modetable[ledE] < 11)
{                                // prima pagina


  
   bit_write(1, lightable[ledE]-1, 0); // ledstatus // memorizzo il ledstgatus come spento 
   bit_write(3,ledE,0);              // feedback2
   
   if (page==0) {
   if ( lightable[ledE] > 0 ) {
    
  
   
    if (valuetable[general_mempos] != 0 )  // nomobo
          { 
          #if (shifter_active == 1 && stratos == 0)
          ledControl(ledE, 0);
          #endif
          }

          else
          {
          #if (shifter_active == 1 && stratos == 0)
          shifter.setPin(lightable[ledE]-1,0); shifterwrite=1;
          #endif

          #if (Matrix_Pads == 1 )
          single_h(matrix_remap[ledE],lightable[ledE],0,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo)
          #endif 

          #if (Matrix_Pads == 2 )
          single_h(matrix_remap[ledE-16],lightable[ledE],0,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo)
          #endif 
          }
        
    }
        
 //   #if (DMX_active == 1  && stratos == 0)
 //  DmxSimple.write(dmxtable[ledE], 0);
 //   #endif
   }
                                                                  // gestione toggletable
      if (modetable[ledE] >1 && modetable[ledE] < 11)            // se il pulsante è in toggle o togglegroups
      { //lastbutton[ledE]=1; 
                                                                 // nota: nella void push_button, lastbutton viene portato a zero quando un pulsante e' PREMUTO
                                                                 // - viene abilitato a mandare midiON solo se lastbutton e' 1
     
      bit_write(4,ledE,0);                                       // 4 = toggletable
                                                                 // nota: quando la toggletable (4) e' = 1 , 
                                                                 // la pressione del pulsante genera midiout (ved void pushbuttons)
      } 
        else if  (modetable[ledE] == 1)                          // se il pulsante è in modalità normale
        {  bit_write(4,ledE,0); } 
        
      } }
  
if (valuetable[ledE+max_modifiers]==note && bit_read(3,ledE+max_modifiers) ==1  
 )   // feedback solo per pulsanti

{
  if (modetable [ledE+max_modifiers] <16)
  {
   //  #if (DMX_active == 1  && stratos == 0)
      //   DmxSimple.write(dmxtable[ledE], 0);
      //   #endif
}
if (modetable [ledE+max_modifiers] <11)
{         //    secoda pagina
  
   bit_write(1, lightable[ledE]+max_modifiers-1, 0); 
   bit_write(3,ledE+max_modifiers,0);  
   
 
   if (page!=0) {                  // spegni le luci solo se sei sulla seconda pagina  
   if ( lightable[ledE] > 0 ) {    // se il pulsante ha un led associato // 0 = nessun led
         
        if (valuetable[general_mempos] != 0 ) 
        {
        #if (shifter_active == 1 && stratos == 0)
        ledControl(ledE, 0);
        #endif
        }
        else
        {
        #if (shifter_active == 1 && stratos == 0) 
        shifter.setPin(lightable[ledE]-1,0); shifterwrite=1; 
        #endif

        #if (Matrix_Pads == 1 )
        single_h(matrix_remap[ledE],lightable[ledE],0,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo)
        #endif 

           #if (Matrix_Pads == 2 )
        single_h(matrix_remap[ledE-16],lightable[ledE],0,1);  // visualizzazione simbolino // (quale pad , quale simbolo, positivo o negativo)
        #endif 
        }
     }
      //  #if (DMX_active == 1  && stratos == 0)
      //   DmxSimple.write(dmxtable[ledE], 0);
      //   #endif
    } 

   
    
      if (modetable[ledE] >1 && modetable[ledE] < 11) { lastbutton[ledE]=1;   bit_write(4,ledE+max_modifiers,0); }
        else if  (modetable[ledE] == 1) {  bit_write(4,ledE+max_modifiers,0); } 
      }    
      }   }   
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
encoder_mempos[0]=0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 void eeprom_write()
 {
  {  
    if (type <160)  { 
      editorcounter = 0;


          #if (stratos == 1)   // flash di luce sul led di Stratos
          digitalWrite(16, HIGH);
          delay(5);
          digitalWrite(16, LOW);
          #endif
          
       #if (shifter_active == 1 && stratos == 0)
   if (valuetable[general_mempos] == 0 )
   {shifter.setAll(HIGH); shifter.write();     // flash di luce - dartmobo
     delay(5);
     shifter.setAll(LOW); shifter.write();
   }
   #endif

 #if (Matrix_Pads > 0  && stratos == 0)      // flash di luce matrix
    if (do_ > 7 ) do_ = 0;
    lc.Parola_diretta(0, order_row[do_],1 );
    delay(5);
    lc.Parola_diretta(0, order_row[do_],0 );
    do_++;
  #endif
   
    }
         
     //  ________________________________________________________________________________________________________________
       // i messaggi vengono posizionati in eeprom in tale ordine:
       // 0-65 = type                512 - 2nd page
       // 64-128 = value             576 - 2nd page
       // 128-192 = toggle mode      640 - 2nd page
       // 192-256 = DMX              704 - 2nd page
       // 256-320 = min              768 - 2nd page
       // 320-384 = max              832 - 2nd page
       // 384-448 = nel korova viene memorizzatoo il keyboard 896
       // 448-512   led              960 - 2nd page
       
       //   pz g matt 15 
       
switch (editorcounter) 
     {      
case 0 : ////////////////////////////////////////////////////////////////////////////////////  NOTE - MEMORYPOSITION  
     #if (stratos == 0 )
      if ( velocity < 56 ) memoryposition = remapper(velocity);  // riposiziono in memoria gli elementi in modo che la numerazione sia regolare sulla pcb
      else if ( velocity < 64 ) memoryposition = velocity;
      else if ( velocity < 120 )memoryposition = remapper(velocity); 
      else  memoryposition = velocity;
      #endif

       #if (stratos == 1 )                               // stratos non usa multiplexers - è più semplice la numerazione diretta
      if ( velocity < 56 ) memoryposition = velocity+1; 
      else if ( velocity < 64 ) memoryposition = velocity;
      else if ( velocity < 120 )memoryposition = velocity+1; 
      else  memoryposition = velocity;
      #endif

      if (matrix_vert1 > 0 )
     { bitWrite(note,7,bitRead(matrix_vert1,0));  }
     
     if (memoryposition < 64) {
      
     
  
   
     EEPROM.write(memoryposition+64,note); // value - valuetable[] - pitch della nota
     }else{
     EEPROM.write(memoryposition+512+64-64,note);  // value 2nd   // ATTENZIONE: c'è un -64 che serve per compensare la numerazione memoryposition 2nd page
     }     
     break;
         
case 1 :  //////////////////////////////////////////////////////////////////////////////////////////// MIN - MAX
    
      if (matrix_vert1 > 0 )
     { 
      bitWrite(note,7,bitRead(matrix_vert1,1));  
      bitWrite(velocity,7,bitRead(matrix_vert1,2));  
     }
     
     if (memoryposition < 64) {
      
    
       
     EEPROM.write(memoryposition+320,note);  // max  // maxvalue[] // soglia minima della escursione del byte2 nel messaggio midi
     EEPROM.write(memoryposition+256,velocity);     // min  // minvalue[] // soglia massima della escursione del byte2 nel messaggio midi
     } else{
     EEPROM.write(memoryposition+512+320-64,note);  // max 2nd
     EEPROM.write(memoryposition+512+256-64,velocity);     // min 2nd
     }  // ATTENZIONE: c'è un -64 che serve per compensare la numerazione memoryposition 2nd page
     break;
  
case 2 : /////////////////////////////////////////////////////////////////////////////////////////  MODE - DMX

        if (matrix_vert2 > 0 )
     { 
       bitWrite(velocity,7,bitRead(matrix_vert2,0));  
     }
     
     if (memoryposition < 64) {   

   
     
    
     EEPROM.write(memoryposition+128,note); // MODE // modetable[] // modalità di funzionamento del modificatore. es: button, pot, toggle, groups, page etc... 
     //bitWrite(velocity,7,1);
     EEPROM.write(memoryposition+192,velocity); // dmx // dmxtable[] // canale dmx - nota: l'escursione dmx è 0-127 ma moltiplicata per 2, quindi 0-255.
     } else{ 
     EEPROM.write(memoryposition+128+512-64,note); // MODE 2ND
     EEPROM.write(memoryposition+192+512-64,velocity); // dmx 2nd
     }  // ATTENZIONE: c'è un -64 che serve per compensare la numerazione memoryposition 2nd page
     break;
     
case 3 : ////////////////////////////////////////////////////////////////////////////////////// keyboard - miditype


        if (matrix_vert2 > 0 )
     { 
       bitWrite(note,7,bitRead(matrix_vert2,1));  
     }
     
     if (memoryposition < 64) {
       
    

     EEPROM.write(memoryposition,type-176+(velocity*16)+144);      // TYPE // typetable[] // 
     // note: velocity = miditype proveniente dall'editor numerato da 0 a 6 - viene moltiplicato per 16 e sistemato da 144 in poi a seconda del canale
     // type contiene ovviamente anche l'informazione del canale midi 0-16
     
     EEPROM.write(memoryposition+384,note); // qwerty // qwertyvalue[] //  =  memorizzato a partire dalla posizione 384
     } else{
     EEPROM.write(memoryposition+512-64,type-176+(velocity*16)+144);  // type 2nd     
     EEPROM.write(memoryposition+384+512-64,note); //  trying not to cross memory limit - qwertyvalue stored 64 memory slots before
     }  // ATTENZIONE: c'è un -64 che serve per compensare la numerazione memoryposition 2nd page
     break;
     
case 4 : ////////////////////////////////////////////////////////////////////////////////////////  LED - /          
     if (memoryposition < 64) { 
      
     
     EEPROM.write(memoryposition+448,note); // LED // lightable[] // il dato viene memorizzato solo sulla prima pagina
     } else{
     EEPROM.write(memoryposition+512-64+448,note);
     }  // ATTENZIONE: c'è un -64 che serve per compensare la numerazione memoryposition 2nd page
     break;
case 5 : ///////////////////////////////////////////////////////////////////////////////////////////////
    

      if (matrix_vert1 > 0 )
     { bitWrite(note,7,bitRead(matrix_vert1,3)); }
        if (matrix_vert2 > 0 )
     { bitWrite(velocity,7,bitRead(matrix_vert2,2));}
     
     if (memoryposition < 64) { 
      
       
    
     EEPROM.write(memoryposition+128,note); // User_byte_1 - modetable[]  
     EEPROM.write(memoryposition,velocity); //  User_byte_2 - typetable[]
     
     } else{
     EEPROM.write(memoryposition+512-64+128,note); // User_byte_5 - modetable[]
     EEPROM.write(memoryposition+512-64,velocity); // User_byte_6 - typetable[]
     }  // ATTENZIONE: c'è un -64 che serve per compensare la numerazione memoryposition 2nd page
     break;
case 6 : ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    
     
         if (matrix_vert2 > 0 )
     { 
       bitWrite(note,7,bitRead(matrix_vert2,3));  
     }
     
     if (memoryposition < 64) { 
        
    
     
     EEPROM.write(memoryposition+448,note);   // User_byte_3 - lightable[]
    // EEPROM.write(memoryposition+448,velocity);  // User_byte_4 - riga verticale
     } else{
     EEPROM.write(memoryposition+512-64+448,note); // User_byte_7 - lightable[]
    // EEPROM.write(memoryposition+512+448,velocity); // User_byte_8 - riga verticale
     }  // ATTENZIONE: c'è un -64 che serve per compensare la numerazione memoryposition 2nd page
     
     matrix_vert1 = 0;
     matrix_vert2 = 0;
     break;

     }
     

     editorcounter++;
     
     }
 }
