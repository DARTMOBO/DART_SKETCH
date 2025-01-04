
 #if (stratos == 1)
 void AIN_stratos()
 { 
    for( channel = 0; channel < 20; channel++)    
{ 
  chan = channel; // chan viene usata da void ain_nucleo
//   delay(1);
  
   #if (top_spinner == 1)
   if (lastbutton[encoder_mempos[0]] == 64 || dmxtable[general_mempos] == 0
   ) // 64 = no encoder action - the MAIN spinner has priority over any other action.
  #endif
{
     virtual_touch_end(0);
   

 

 
   if (channel == 1) {               digitalWrite(14, LOW); digitalWrite(15, HIGH);                      // SERIE DIODI 1
                                     digitalWrite(16, HIGH); // accendi il LED
      valore = ((digitalRead(5)))*1000;  
      ain_nucleo(); //  b5  
      }
      
   else if (channel == 2) {          if (lastbutton[touch_mempos[0]] == 1 )   
                                     digitalWrite(16, LOW);  // spegni il LED - dipende dal touch
                                     digitalWrite(14, LOW); digitalWrite(15, HIGH);
      valore = ((digitalRead(4)))*1000;   
      ain_nucleo(); // b6 - ENCODER2 BUTTON

     }
      
   else if (channel == 3)  { 
    valore = ((digitalRead(6)))*1000;   
    ain_nucleo();
    } // b4
   
    else if (channel == 4)  { 
    valore = ((digitalRead(7)))*1000;   
    ain_nucleo(); 
    } // b3
    else if (channel == 5)  { 
    valore = analogRead(19);
    ain_nucleo(); 
    } // b2
    else if (channel == 6)  {// valore = (!(digitalRead(18)))*1000; 
    digitalWrite(14, LOW); digitalWrite(15, HIGH);   
    valore = analogRead(18);
    ain_nucleo();  } // b1 - ENCODER1 BUTTON
   
   else if (channel == 7) {  digitalWrite(15, LOW);digitalWrite(14, HIGH);                  // SERIE DIODI 2
                             valore = (!(digitalRead(5)))*1000;   
                             ain_nucleo();                           // pb7
                          }   
   else if (channel == 8)  {  
    valore = ((digitalRead(6)))*1000;   
    ain_nucleo(); 
    } //  pb8
   else if (channel == 9)  {  
    valore = ((digitalRead(7)))*1000;  
   ain_nucleo();   
   } // pb10
   else if (channel == 10) {
    valore = analogRead(19);
   ain_nucleo();   
   } // pb9
   else if (channel == 11) { 
     digitalWrite(15, LOW);digitalWrite(14, HIGH);    
     valore = analogRead(18);
   ain_nucleo();  
   } // page

   else if (channel == 12) {  
     valore = analogRead(10);
     ain_nucleo();//  
     } 
    
    else if (channel == 13) {    }
    else if (channel == 14) {
      digitalWrite(15, LOW);digitalWrite(14, HIGH);   
      LSB[1] = (!(digitalRead(21)));  MSB[1] = (!(digitalRead(20)));
    updateEncoder(channel); 
    encoder(channel);   
       } 
    else if (channel == 15) {    }
    else if (channel == 16) {
      digitalWrite(15, LOW);digitalWrite(14, HIGH);   
      LSB[1] = (!(digitalRead(0)));   MSB[1] = (!(digitalRead(1)));
    updateEncoder(channel); 
    encoder(channel);   
       } 
  
       
     
  }   
 #if (top_spinner == 1)
  else {
  encoder(encoder_mempos[0]);           // midiout 1st encoder  
  //    Serial.println("encoder_active");         
  }
 #endif
  
   } // end PLEXER
   } // end ain
 #endif


#if (stratos == 1)
 void aux_preset() // preset di base caricato all'avvio soltanto se il preset sulla eeprom non e' valido. 
 {

   for (byte i = 0; i <max_modifiers; i++) {
    modetable[i]=1; // pushbuttons

    { /// stratos
      valuetable[i+page]=    60+i; 
    valuetable[i]=    60+i;
    }
    
    
    minvalue[i]=0;
    maxvalue[i]=127;
    dmxtable[i]=i;
    qwertyvalue[i]=0;
    typetable[i]=144; /// 144 = note ; 176 = cc
    typetable[i+page]=144;
    lightable[i]=remapper(i);
    }

   modetable[12]=11; 
    typetable[12]=176;
    minvalue[12]=0;
    maxvalue[12]=127;
    
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
    modetable[14] = 19;        // 19 = encoder - ved ain_nucleo
    typetable[14] = 176;
    dmxtable[14] = 1;
    minvalue[14] = 32;
    lastbutton[14] = 64;
    
    modetable[16] = 19;         // 19 = encoder 
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
       dmxtable[mouse_mempos] = 0; // no mouse emulation
   
 }
 #endif
   
