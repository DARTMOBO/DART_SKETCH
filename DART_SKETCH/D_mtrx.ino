 /*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */
 
 
 // matrix-----------------------------------------------------------------------------

#if (Matrix_Pads > 0 && stratos == 0)
void single_h(byte number_of_unit,byte sprite, byte invert,byte send_) { 
  
              // quale pad, quale simbolo, inversione immagine
  
  byte sprite_pos = remapper(sprite-1); 
  
              // in quale posizione di memoria è memorizzato il simbolo? 
              // fare il remapper della numerazione di "led" (cioè lightable-1)
if (sprite >46 )
{
if (invert == 0 )
{
lc.setRow(number_of_unit,6, valuetable[sprite_pos+page],send_);
lc.setRow(number_of_unit,1, maxvalue[sprite_pos],send_       );
lc.setRow(number_of_unit,5, minvalue[sprite_pos],send_       );
lc.setRow(number_of_unit,3, modetable[sprite_pos],send_      );

lc.setRow(number_of_unit,7, dmxtable[sprite_pos],send_       );
lc.setRow(number_of_unit,2, qwertyvalue[sprite_pos],send_    );
lc.setRow(number_of_unit,4, typetable[sprite_pos+page],send_ );
lc.setRow(number_of_unit,0, lightable[sprite_pos],send_      );
}

else 

{
lc.setRow(number_of_unit,6, ~valuetable[sprite_pos+page],send_);
lc.setRow(number_of_unit,1, ~maxvalue[sprite_pos],send_);
lc.setRow(number_of_unit,5, ~minvalue[sprite_pos],send_);
lc.setRow(number_of_unit,3, ~modetable[sprite_pos],send_);

lc.setRow(number_of_unit,7, ~dmxtable[sprite_pos],send_);
lc.setRow(number_of_unit,2, ~qwertyvalue[sprite_pos],send_);
lc.setRow(number_of_unit,4, ~typetable[sprite_pos+page],send_);
lc.setRow(number_of_unit,0, ~lightable[sprite_pos],send_);
  }
}

 // for(int i=0;i<8;i++) lc.sendRow(number_of_unit,i);
  
}
#endif


 #if (Matrix_Pads == 2)

void matrix_restore (boolean numero)
{

 lc.restore_shutdown();
 
 /* for(int unit = 0; unit < 8; unit++)  // aggiorno l'array STATUS che contiene la situazione di tutte le matrici
      {
      single_h(matrix_remap[remapper(unit)],                             // quale matrice                                                     
               lightable[remapper(unit)+16],                           // quale sprite 
               bit_read(4,remapper(unit)+16+(numero*max_modifiers)),     // positivo - negativo
               1);  // send - no

             // AIN();
            // loop();
      }
      */
      byte sendd;
      for(int unit = 0; unit < 12; unit++)  // aggiorno l'array STATUS che contiene la situazione di tutte le matrici
      {
/*
 * spiegazione per futura memoria: 
 * ho riscontrato pi volte difficoltà nel'aggiornamento veloce di più matrici insieme
 * i motivi al momento non sono chiari , un misto di problemi elettrici di trasmissione con interferenze e problemi di programmazione boh...
 * dato che devo varie volte ripristinare tutta la pagina con tutti gli sprite e devo farlo velocemente per non interferire col funzionamento del controller
 * 
 * dunque ho notato che le matrici che si aggiornano male sono le ultime della catena (catena da 12 unità) 
 * quindi soluzione brutta ma funzionante: aggiorno le ultime quattro singolarmente
 *      perchè singolarmente il simbolino viene fuori in modo ottimale
 * tutte le altre matrici le aggiorno prima in memoria e poi inviando in massa i dati con lc.sendrow()
 * 
 * altra cosa molto molto strana è che se aggiorno le matrici singolarmente (con i simbolini perfettamente visualizzati)
 * e poi mando un lc.sendrow() totale... non ho nessun glitch... strano
 * 
  */
        
        if( matrix_remap[remapper(unit)] > 7 ) sendd = 1; else sendd =0;
      single_h(matrix_remap[remapper(unit)],                             // quale matrice                                                     
               lightable[remapper(unit)+16],                           // quale sprite 
               bit_read(4,remapper(unit)+16+(numero*max_modifiers)),     // positivo - negativo
               sendd);  // send - no
;
      }

  lc.sendRow2(0); // rendi visibile tutto // 
  lc.sendRow2(1);
  }

  #endif



  
  #if (Matrix_Pads == 1)

void matrix_restore (boolean numero)
{

 lc.restore_shutdown();
 

      byte sendd;
     
    
       for(int unit = 0; unit < 12; unit++)  // aggiorno l'array STATUS che contiene la situazione di tutte le matrici
      {
     
        
        if( matrix_remap[remapper(unit)] < 8 ) 
        { //sendd = 1; else sendd =0;
      single_h(matrix_remap[remapper(unit)],                             // quale matrice                                                     
               lightable[remapper(unit)],                           // quale sprite 
               bit_read(4,remapper(unit)+(numero*max_modifiers)),     // positivo - negativo
               0);  // send - no
        }
      }
          lc.sendRow2(0); // rendi visibile tutto // 
  lc.sendRow2(1);
  
        
        
      for(int unit = 0; unit < 12; unit++)  // aggiorno l'array STATUS che contiene la situazione di tutte le matrici
      {
     
        
        if( matrix_remap[remapper(unit)] > 7 ) 
        { //sendd = 1; else sendd =0;
      single_h(matrix_remap[remapper(unit)],                             // quale matrice                                                     
               lightable[remapper(unit)],                           // quale sprite 
               bit_read(4,remapper(unit)+(numero*max_modifiers)),     // positivo - negativo
               1);  // send - no
        }

      }
      

sendd = 1;
       single_h(matrix_remap[remapper(sendd)],                             // quale matrice                                                     
               lightable[remapper(sendd)],                           // quale sprite 
               bit_read(4,remapper(sendd)+(numero*max_modifiers)),     // positivo - negativo
               1);  // send - no

 //  lc.sendRow2(0); // rendi visibile tutto // 
 //   lc.sendRow2(1);
  }

  #endif


  #if (Matrix_Pads > 0  )

void led_enc_exe_matrix () // effettino luminoso quando si girano i pot e gli encoder
{
   byte pad_;
   byte A[]={0,0,0};
   byte encled_matrix;

   
 
if (qwertyvalue[general_mempos] > 0) // verifico che nelle impostazioni GENERAL siano attivati gli effetti // 0 = no / 1 = pots / 2 = spinners / 3 = buttons
  {
                                                                                      // se encled supera il valore massimo o minimo
                                                                                      // per esempio girando un encoder...
                                                                                      // l'effetto luminoso deve ricominciare ni nuovo dal basso o dall'alto
   if (encled[0] <0) { encled[0] = 255 ;}
   else
   if (encled[0] >255) {encled[0] = 0;} 

 //_________________________________________________________________________
 // byte adattamento_encled = encled[0] /44;
 if (cycletimer <  10) { //shifter.setPin(encledtable[constrain(encled/16,0,15)] , 1);  
    //  encled[0] = map(adattamento_encled,0,23,23,0);

    // encled_matrix = encled[0] /10; // ottengo escursione da 24 punti
    // encled_matrix = map(encled[0],0,23,23,0); // mi serviva invertire sopra e sotto.
    // AA  encled[0] = map((encled[0]),0,240,0,23); // riassumo le righe sopra in una sola
    
     // Serial.println(encled_matrix);
     // Serial.println("--");

encled_matrix = map((encled[0]),0,240,0,7);
     // nella precedente versione era encled che veniva convertito in un range 0-23
     // adesso riverso tutto su una variabile temporanea per evitare che encled...

     //  Serial.println(encled_matrix);
      
      pad_ = encled_matrix;
    // AA pad_ = encled[0];
     
     byte pad_2 = pad_/8;   // encled, in modalita matrix, è di 24 punti - diviso otto ottengo un numero da 1 a 3 che mi dice in quale matrice fare l'effetto

   if (pad_ >7) pad_=pad_ -8;
   if (pad_ >7) pad_=pad_ -8;
                                 // con queste due righe divido un numero 0-24 in modo da ottenere la posizione dell effetto da 0 a 8
   
 bitWrite(A[pad_2], pad_   ,true);

 // Serial.print(encled); Serial.print(" - "); Serial.print(pad_);  Serial.print(" - "); Serial.println(pad_2*4);
  
  for (byte i = 0 ; i < 8; i++)
  {
    if ( bitRead(A[pad_2],i) == true) { 
   lc.Parola_diretta(pad_2*4, order_row[i],1 ); // parola diretta settato su 1 inverte lo status dei led - basandosi su STATUS - ved ledcontrol.h
  //   lc.Parola_diretta(pad_2*4, order_row[i+1],1 );  // stessa cosa sulla riga di sotto.
    }
    }


    //----------------

    pad_ = encled[1];
   //   Serial.print(encled[1]);
      pad_2 = encled[1]/8;
      
   if (pad_ >7) pad_=pad_ -8;
   if (pad_ >7) pad_=pad_ -8;
 
   A[pad_2]= 0;
   
      // if (encled[0]/8 != pad_2)
     /// Serial.print(encled[0]); Serial.print(" - "); Serial.print(encled[1]);  Serial.print(" - "); Serial.println(pad_2*4);
      //if (abs(encled[0]-encled[1]) >0)
      
      if (encled_matrix != encled[1])
{
 bitWrite(A[pad_2], pad_   ,true);

     for (byte i = 0 ; i < 8; i++)
  {
    if ( bitRead(A[pad_2],i) == true) { 
   lc.Parola_diretta(pad_2*4, order_row[i],0 ); 
    // lc.Parola_diretta(pad_2*4, order_row[i+1],0 ); 
    }
    }
 }
 
 // encled[1] = encled[0];
    encled[1] = encled_matrix;

  
 } 
                                                                                       // accendo effettivamente il led secondo il valore "encled"
                                                                                       // che farà riferimento alla tabella "encledtable"
                                                                                       // encled è 0-255 // diviso per 16 da luogo a 16 posizioni
                                                                                       // (che può cambiare da modello a modello - kombat / one etc etc)


                                                                                       
 
 // byte ripristino_led = (encled/32) -1;                                                            // adesso vado a spegnere i led che ho acceso prima
 // shifter.setPin((encledtable[ripristino_led] ), bit_read(1,encledtable[ripristino_led]+(page)));  
 //lc.setRow(encled/85,order_row[(encled/11)-(8*(encled/85))], 255);
 // byte pad_ = (encled/85);

 //pad_ = ((encled+46)/85);
 // byte row_ = (encled/11)-(8*(pad_));
 // lc.setRow(pad_*4,order_row[row_], 0);
 
 // lc.setRow(pad_*4+1,row_, 0);
 // lc.setRow(pad_*4+2,row_, 0);
  //lc.setRow(pad_*4+3,row_, 0);
                                                                                                  // se un led era acceso nella tabella bitstatus, rimarrà acceso
 // ripristino_led = (encled/32) +1;                                                                 // spengo anche nella direzione opposta
 // if (encled < 240 ) {}
 //shifter.setPin((encledtable[ripristino_led] ), bit_read(1,encledtable[ripristino_led]+(page)));
 //lc.setRow(0,order_row[ripristino_led], 0);
 
  // byte pad_ = (encled/85);
  //byte row_ = order_row[(encled/11-2)-(8*(pad_))];
  //lc.setRow(pad_*4,row_, 0);
  //lc.setRow(pad_*4+1,row_, 0);
 // lc.setRow(pad_*4+2,row_, 0);
 // lc.setRow(pad_*4+3,row_, 0);

  // shifterwrite=1; // on viene ripetuto ad ogni ciclo, ma solo quando vene mosso l'encoder
}
  }
  
  #endif
