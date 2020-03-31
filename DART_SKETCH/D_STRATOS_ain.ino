
 #if (stratos == 1)
 void AIN_stratos()
 

 { 
for( channel = 0; channel < 18; channel++)    
{ 
  chan = channel;
   #if (main_encoder == 1)
   if (lastbutton[encoder_mempos[0]] == 64 || dmxtable[general_mempos] == 0
   ) // 64 = no encoder action - the MAIN spinner has priority over any other action.
  #endif
{
     virtual_touch_end(0);
   

 
 // if (dmxtable[general_mempos] == 2)
 

/*
 {  
 senseEncoder_2nd(); 
 updateEncoder(1);
 encoder(1);
}
*/


 
   if (channel == 1) {  digitalWrite(14, LOW); digitalWrite(15, HIGH); digitalWrite(16, HIGH); 
      valore = (!(digitalRead(5)))*1000;   ain_nucleo();  }
      
      else  
      if (channel == 2) {  if (lastbutton[touch_mempos[0]] == 1 ) 
      digitalWrite(16, LOW); 
      valore = (!(digitalRead(4)))*1000;   ain_nucleo();}
      
        else if (channel == 3) { valore = (!(digitalRead(6)))*1000;   ain_nucleo(); }
   else if (channel == 4)  { valore = (!(digitalRead(7)))*1000;   ain_nucleo(); }
   else if (channel == 5) { valore = (!(digitalRead(19)))*1000;   ain_nucleo(); }
   else if (channel == 6) { valore = (!(digitalRead(18)))*1000;   ain_nucleo(); 
   }


   
    else if (channel == 7) {  digitalWrite(15, LOW);digitalWrite(14, HIGH);
    valore = (!(digitalRead(5)))*1000;   ain_nucleo(); }
    else if (channel == 8) { valore = (!(digitalRead(6)))*1000;   ain_nucleo(); }
    else if (channel == 9) { valore = (!(digitalRead(7)))*1000;   ain_nucleo();   }
    else if (channel == 10) { valore = (!(digitalRead(19)))*1000;   ain_nucleo();  }
    else if (channel == 11) { valore = (!(digitalRead(18)))*1000;   ain_nucleo(); 
    }

       else if (channel == 13) { MSB[1] = (!(digitalRead(21)));   }
       else if (channel == 14) { LSB[1] = (!(digitalRead(20)));//   delay(30);
     updateEncoder(channel); 
  
   //   if (lastbutton[14] != 64) { do_ ++; if (do_ > 3) do_ = 0;  }
  
      //  if (do_ == 0)
        encoder(channel); 
      //  else    lastbutton[14] = 64  ;
       } // primo encoder extra
       
     //  else if (channel == 15) { MSB[1] = (!(digitalRead(0)));   }
     //  else if (channel == 16) { LSB[1] = (!(digitalRead(1)));   updateEncoder(channel); encoder(channel);} // primo encoder extra

//Serial.println(bitRead(channel,0));
 // delay(200); 
  // else if (channel == 16) { 
 
 // encoder(channel);}


  }   
 #if (main_encoder == 1)
  else {
  encoder(encoder_mempos[0]); // midiout 1st encoder  
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
       dmxtable[mouse_mempos] = 0; // no mouse emulation
   
 }
 #endif
   
