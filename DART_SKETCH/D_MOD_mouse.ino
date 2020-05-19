void mouse_control () 


{
 
  
  #if defined (__AVR_ATmega32U4__)  
  if (mouse_mempos != 0)
  { /// tutta la sezione mouse comincia qui
 

    
 if (chan == (minvalue[mouse_mempos]) )   {         // sezione mouse  // asse X 
 valore = analogRead(plexer);


 
 mousex = 119 + (((valore+1)/64)) ;    
 // spiegazione: valore/64 produce un risultato che va da 0 a 16
 // 119 = 127 - (16/2) dove 16/2 sarebbe il centro, quindi 127 è il centro, 135 il massimo a destra e 111 il minimo a sinistra.

 #if (mouse_block == 1)
 if (mousex >133 )  // il massimo sarebbe 135
 {if ( qwertyvalue[mouse_mempos] < 255 ) qwertyvalue[mouse_mempos]++; } // qwertyvalue[mouse_mempos] è usata come contatore 
 else  {qwertyvalue[mouse_mempos]= 0;} // mouse block praparazione

 if (qwertyvalue[mouse_mempos] < 230)  // superata la sogli di 230 cicli viene interrotta la funzionalità
 #endif


  if  (dmxtable[mouse_mempos] == 1){  // 1 =  mouse  attivato
  if (abs(mousex-127) >1 ) // cerco di evitare spostamenti minimi quando il joystick non tiene benissimo il centro
    Mouse.move( (mousex-127)
  *((boolean(lightable[mouse_mempos])*2  ) -1) // inversione di direzione: questa operazione restituisce "1" oppure "-1"
 , 0, 0); 
 }
//  if ((mousex-127) != 0 ) {Serial.print(valore); Serial.print("  /  "); Serial.println(mousex-127);}
 

 
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
 
   else if (chan == (maxvalue[mouse_mempos]))  { // asse Y
  valore = analogRead(plexer);
   mousex = 119 + (((valore+1)/64)) ; // viene usato lo stesso la variabile mousex anche per l'asse Y
   if (mousex >133 ) {if ( mousey < 255 ) mousey++;} else  {mousey= 0;}
 
  #if (mouse_block == 1)
  if (mousey < 230)
  #endif
 {
 if  (dmxtable[mouse_mempos] == 1) { 
    if (abs(mousex-127) > 1 )
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
