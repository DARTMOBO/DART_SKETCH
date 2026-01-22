
 /*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */



// [TOUCH_SENSORS_EXTERNAL_UNIFIED]
// External digital touch read (modules/buttons): pinset chosen by Touch_sensors_enable (2 or 3)
// - Touch_sensors_enable = 2  -> T0=7, T1=9
// - Touch_sensors_enable = 3  -> T0=7, T1=8
// Logic inversion controlled by touch_invert (0=normal, 1=invert).
#if (Touch_sensors_enable == 2 || Touch_sensors_enable == 3)

void touch_sensors(byte T_numero)
{
  // SE ESISTE UN ITEM CON SETTAGGIO TOUCH ALLORA SI PROCEDE ALLA LETTURA
  if (touch_mempos[T_numero] > 0)
  {
    const byte pin0 = 7;
    const byte pin1 = (Touch_sensors_enable == 2) ? 9 : 8;

    byte pin = (T_numero == 0) ? pin0 : pin1;

    byte raw = digitalRead(pin);     // HIGH/LOW
    if (touch_invert) raw = !raw;    // invert if needed

    averageXen[T_numero] = raw;

    // Manteniamo la logica originale: touch_execute quando lightable è in modalità capacitive (0)
    if (lightable[touch_mempos[T_numero]] == 0)  touch_execute(T_numero);
    // else if (lightable[touch_mempos[T_numero]] == 2) { ... monitoring ... }  // lasciato come in origine (commentato)
  }
}

#endif



#if (Touch_sensors_enable == 1)
void touch_sensors(byte T_numero) {
  // in breve in questa void viene caricato un valore sulla variabile averageXen[T_numero] che poi verrà usato nella void touch_execute 
  // per diventare un messaggio midi
  
 //  if (decaysensor[T_numero] > 0 ) decaysensor[T_numero]--;
 

     if (touch_mempos[T_numero] >0)  // SE ESISTE UN ITEM CON SETTAGGIO TOUCH ALLORA SI PROCEDE ALLA LETTURA
     {                  
      
       readingsXen[T_numero][indexXen] = (cs_4_2[T_numero].capacitiveSensorRaw(72)); // SI FANNO DIVERSE LETTURE - INDEXXEN VA DA 0 A 3 vedi void loop
      
     
 

                     
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

#if (Touch_sensors_enable == 1)
void touch_execute (byte numero_ex) 
{

   byte zero = 1;
  byte uno = 0;
  {        
         ///////////////////////////////////////////////////////////////////////////////////////////////// rilascio del touch                                 
  if (    averageXen[numero_ex]<
  (higher_Xen[numero_ex]) 
  - map32( minvalue[touch_mempos[numero_ex]],0,112,0,higher_Xen[numero_ex]-lower_Xen[numero_ex]) 
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
  - map32( minvalue[touch_mempos[numero_ex]],0,127,0,higher_Xen[numero_ex]-lower_Xen[numero_ex]) // la sottrazione e' proporzionale ad higher_xen
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

#if (Touch_sensors_enable > 1 )
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



 
   
#if (shifter_active == 1 && stratos == 0)
if (maxvalue[touch_mempos[numero_ex]] > 0 )
 bit_write(1,maxvalue[touch_mempos[numero_ex]]-1+page,0);           // spengo il led nella tabella di memoria
 
   if (page==0) ledrestore(0); else ledrestore(1);  
   shifterwrite=1; 
   
#endif

#if (stratos == 1)
digitalWrite(16,LOW);
#endif

#if (touch_led_onboard == 1 && Touch_sensors_enable  == 2)
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




#if (shifter_active == 1 && stratos == 0)
if (maxvalue[touch_mempos[numero_ex]] > 0 )
bit_write(1,maxvalue[touch_mempos[numero_ex]]-1+page,1);  // accendo il led nella tabella di memoria

 
 shifter.setPin(maxvalue[touch_mempos[numero_ex]]-1,HIGH);  // accendo il led del touch , nella tabella di memoria
 #endif
 
#if ( stratos == 1)
 digitalWrite(16,HIGH);
  #endif 

#if (touch_led_onboard == 1 && Touch_sensors_enable  == 2)
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
 
