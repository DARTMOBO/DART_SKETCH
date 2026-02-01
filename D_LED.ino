/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */

#if (shifter_active == 1 && stratos == 0)

void ledrestore (boolean numero) { // riscrive sullo shifter la pagina - numero - con luci accese o spente
  for(int led = 0; led < max_modifiers; led++) { 
    shifter.setPin(led, bit_read(1,led+(numero*max_modifiers)));
    // if (lastbutton[touch_mempos[0]] == 1)  shifter.setPin(4+numero,HIGH); 
    // if (lastbutton[touch_mempos[1]] == 1)  shifter.setPin(5+numero,HIGH); 
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
#if (shifter_active == 1 && stratos == 0)

void ledControl (byte chann, byte stat) { // stat significa status 1 = acceso 0 = spento
  // versione commentata
  
  // Se general_mempos == 0 siamo in modalità "MOBO":
  // DART completo, con tutta la controlboard e lo shifter attaccato.
  if (valuetable[general_mempos] == 0) { // se siamo in modalita MOBO - con tutta la controlboard
  
    // Se sto ACCENDENDO il LED (stat > 0), resetto l'ampiezza dell'effetto "scoppio".
    // Questo fa ripartire l'onda luminosa da raggio 0 attorno al pulsante appena premuto.
    if (stat > 0) 
      buttonefx = 0; // ampiezza dell'effetto (raggio corrente dello scoppio)
  
    // Ciclo su tutti i 16 "slot" logici dello shifter (0..15).
    // encledtable[i] = numero di uscita fisica dello shifter in quella posizione.
    // lightable[chann]-1 = uscita fisica associata al pulsante logico "chann".
    // In pratica: qui cerco in quale "colonna" dello shifter si trova il LED del pulsante.
    for (byte i = 0; i < 16; i++) {
      // Se in questa posizione trovo il LED del canale premuto:
      //  - buttonefxd = i → centro dell'effetto di scoppio (indice 0..15)
      //  - break → smetto di cercare, perché il centro è stato trovato.
      // Se NON trovo corrispondenza in questa iterazione:
      //  - buttonefxd = 60 → valore "fuori range" che significa: nessun centro valido.
      //    (Con buttonefxd = 60, più avanti buttonledefx() NON disegnerà nessun effetto.)
      if (encledtable[i] == (lightable[chann]-1)) {
        buttonefxd = i;
        break;
      } else {
        buttonefxd = 60;  // effetti led // buttonefxd è il centro dell'effetto
      }
    }

    // Comando il LED fisico sullo shifter:
    // (lightable[chann]-1) è la linea reale del 74HC595, "stat" è ON/OFF.
    shifter.setPin((lightable[chann]-1), stat); 

    // Memorizzo lo stato del LED in una bit-table.
    // Questo serve a ricordare lo stato "di fondo" dei LED, così gli effetti temporanei
    // (tipo lo scoppio) possono poi essere cancellati ripristinando la mappa originale.
    bit_write(1,(lightable[chann]-1)+page,stat);  
    // memorizzo lo stato del LED in modo che, anche muovendo i pot, 
    // si possa tornare a visualizzare la corretta configurazione
    // bit table
    // 1 - ledstatus 1 e 2
    // 2 - feedback_bit1
    // 3 - feedback_bit2
    // 4 - bit_toggle 1 e 2
  } else {    // se siamo in modalità no-mobo -  solo arduino
    // Qui NON abbiamo la controlboard collegata.
    // Usiamo direttamente alcuni pin digitali di Arduino come LED "di servizio".
    //Serial.println(lightable[chann]);

    // Solo se il valore di lightable[chann] è uno di questi, facciamo qualcosa:
    // 4,5,6,10,11,12 sono pin digitali di Arduino riservati a LED/uscite locali.
    if (                              // solo se corrisponde a uno di questi valore , fai qualcosa -  sarebbero i pin digitali di arduino
      lightable[chann] == 4 || 
      lightable[chann] == 5 || 
      lightable[chann] == 6 || 
      lightable[chann] == 10 || 
      lightable[chann] == 11 || 
      lightable[chann] == 12 ) {
      //  Serial.println(lightable[chann]);

      // In modalità "solo Arduino" accendiamo/spegniamo direttamente il pin digitale.
      // Qui "stat" è usato come HIGH/LOW (0/1) per digitalWrite.
      digitalWrite(lightable[chann], stat);

      // Anche in questo caso aggiorniamo la bit-table,
      // così la logica di ripristino (ledrestore / status interni) resta coerente.
      bit_write(1,(lightable[chann]-1)+page,stat);

      //  digitalWrite(10, stat);   // vecchio test, lasciato commentato
    }
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

//_____________________________________________________________________________________________________

#if (shifter_active == 1 && stratos == 0)

void led_enc_exe () {
  if (qwertyvalue[general_mempos] > 0) { // verifico che nelle impostazioni GENERAL siano attivati gli effetti // 0 = no / 1 = pots / 2 = spinners / 3 = buttons
    { // se encled supera il valore massimo o minimo
      // per esempio girando un encoder...
      // l'effetto luminoso deve ricominciare ni nuovo dal basso o dall'alto
      
      if (encled[0] < 0) {
        encled[0] = 255; 
        shifter.setPin(encledtable[0], bit_read(1,encledtable[0]+(page)));
        shifter.setPin(encledtable[1], bit_read(1,encledtable[1]+(page)));
      } else if (encled[0] > 255) {
        encled[0] = 0; 
        shifter.setPin(encledtable[15], bit_read(1,encledtable[15]+(page))); 
        shifter.setPin(encledtable[14], bit_read(1,encledtable[14]+(page))); 
      }
    }

    if (cycletimer < 10) {
      shifter.setPin(encledtable[constrain(encled[0]/16,0,15)], 1);  // accendo effettivamente il led secondo il valore "encled"
      // che farà riferimento alla tabella "encledtable"
      // encled è 0-255 // diviso per 16 da luogo a 16 posizioni
      // (che può cambiare da modello a modello - kombat / one etc etc)
    }
    
    byte ripristino_led = (encled[0]/16) - 2; // adesso vado a spegnere i led che ho acceso prima
    shifter.setPin((encledtable[ripristino_led]), bit_read(1,encledtable[ripristino_led]+(page))); // se un led era acceso nella tabella bitstatus (toggle accesi), 
    // rimarrà acceso
    ripristino_led = (encled[0]/16) + 2; // spengo anche nella direzione opposta
    if (encled[0] < 240) {
      shifter.setPin((encledtable[ripristino_led]), bit_read(1,encledtable[ripristino_led]+(page)));
    }

    shifterwrite = 1; // on viene ripetuto ad ogni ciclo, ma solo quando vene mosso l'encoder
  }
}
#endif

//----------------------------------------------------------------------------------------------------------------------------

//_____________________________________________________________________________________________________________________________________________

//_____________________________________________________________________________________________________________________________________________

#if (shifter_active == 1 && stratos == 0)

void buttonledefx() {
  // versione commentata
  //  buttonefx  - ampiezza dell'effetto (raggio dell'onda di scoppio)
  //  buttonefxd - centro dell'effetto  (indice 0..15 del LED centrale nello shifter)
 
  // Primo blocco: espansione dello "scoppio"
  // -------------------------------------------------
  // L'effetto parte solo se:
  //  - il raggio buttonefx è minore di 5 (massimo 5 step di espansione)
  //  - il centro buttonefxd è un valore valido (0..15). Se è 60, l'effetto è disattivato.
  if (buttonefx < 5 && buttonefxd < 16) {
    // L'aggiornamento avviene solo quando cycletimer == 2.
    // cycletimer viene incrementato nel loop principale e qui fa da "metronomo" dei frame.
    if (cycletimer == 2) {
      // Disegno un "anello" dello scoppio:
      // accendo il LED alla distanza buttonefx a SINISTRA e a DESTRA del centro buttonefxd,
      // ma rimanendo confinato alla stessa "riga" da 8 LED.

      //  if (controlfx < 0) controlfx =60;
      //  if (controlfx >= 0 && controlfx < 8)

      // Calcolo la posizione a SINISTRA rispetto al centro:
      //   - buttonefxd - buttonefx: distanza dal centro
      //   - (buttonefxd/8)*8: inizio riga (0 o 8 a seconda di dove sta il centro)
      //   - (buttonefxd/8)*8+8: limite superiore "virtuale" della riga
      // Il constrain impedisce di uscire dalla riga orizzontale.
      byte controlfx  = constrain(
        buttonefxd - buttonefx,
        (buttonefxd / 8) * 8,
        (buttonefxd / 8) * 8 + 8
      );
      // Accendo il LED corrispondente allo slot controlfx, usando la mappa encledtable[]
      shifter.setPin(encledtable[controlfx], HIGH);

      // Calcolo la posizione a DESTRA rispetto al centro:
      //    buttonefxd + buttonefx: spostamento a destra
      //    stesso inizio riga ( (buttonefxd/8)*8 )
      //    fine riga reale: (buttonefxd/8)*8 + 7  → ultimo indice valido della riga
      controlfx  = constrain(
        buttonefxd + buttonefx,
        (buttonefxd / 8) * 8,
        (buttonefxd / 8) * 8 + 7
      );
      shifter.setPin(encledtable[controlfx], HIGH);

      // Segnalo che ci sono modifiche da scrivere sullo shifter (verrà fatto nel loop)
      shifterwrite = 1;
   
      // Ho fatto un "frame" di animazione:
      //  - azzero cycletimer per ricominciare il conteggio
      //  - aumento il raggio buttonefx in modo che al prossimo giro l'onda sia più ampia
      cycletimer = 0; 
      buttonefx++; 

      // Quando il raggio ha raggiunto 5,
      // preparo buttonefxu per la fase successiva (chiusura / ripristino).
      // Commento originale: "buttonefxu è come buttonfx, scala da 5 a zero ogni 30 millisecondi"
      if (buttonefx == 5) {
        buttonefxu = 0;
      }
    }
  }
    
  // Secondo blocco: "chiusura" dell'effetto e ripristino stato base
  // ---------------------------------------------------------------
  // Quando buttonefxu è minore di 5 parte la fase che ristabilisce la mappa normale dei LED.
  if (buttonefxu < 5) {
    // Anche qui si usa cycletimer come cadenza, ma su valore 3 invece che 2:
    // è un altro "tempo" per scandire la chiusura rispetto all'apertura.
    //   //   if(previousMillis2<currentMillis)   ← vecchia logica a millis(), ora sostituita da cycletimer.
    if (cycletimer == 3) {
      //  shifter.setAll(LOW);   // vecchia debug / opzione, ora commentata.

      // Ripristina la pagina LED corrente (page),
      // usando la bit-table di stato (toggle, ledstatus, ecc.).
      // In pratica: cancella lo "scoppio" temporaneo e ridisegna la configurazione reale.
      ledrestore(page);
 
      // Come sopra, segnalo che devo riscrivere lo shifter
      shifterwrite = 1; 

      //  previousMillis2=(currentMillis+30);  // sostituito dal meccanismo cycletimer
      cycletimer = 0;

      // buttonefxu viene decrementato: scende verso valori negativi,
      // e viene usato solo come "contatore" per sapere quando chiudere del tutto.
      buttonefxu = (buttonefxu - 1);
   
      // Quando buttonefxu arriva a 5 (in pratica condizione di "reset completo"):
      //    - spengo tutto lo shifter
      //    - richiamo ledrestore(page) per ridisegnare lo stato LED corretto
      //    - shifterwrite=1 per mandare fuori i dati aggiornati
      if (buttonefxu == 5) {
        shifter.setAll(LOW);  
        ledrestore(page);
        shifterwrite = 1;
      }
    }
  }
}


   
#endif

#if (shifter_active == 1 && stratos == 0 && Distance_sensor == 1)
void beamefx() {
  cycletimer = 0;
  encled[0] = lightable[distance_mempos] * 2;
  led_enc_exe();
}
#endif

//--------------------------------------------------------------------------------------------------------------------------------------

void offgroup (byte canale, byte midiout) { // canale == il chan del pulsante premuto
  for (int i = 0; i < 56; i++) { 
    if (modetable[i] == modetable[canale] && i != canale) { // se i pulsanti sono nello stesso gruppo e sono accesi, spegnere lucetta e memorizzare status toggle
      if (bit_read(4,i) == 1 || bit_read(4,i+max_modifiers) == 1) {
        if (midiout == 1) {
          outnucleo(0,i); // invia messaggio OFF - se il parametro midiout e' 1
        }
        
        #if (shifter_active == 1 && stratos == 0)
        shifter.setPin(lightable[i]-1, 0); // spegni il led
        bit_write(4,i+page,0);
        bit_write(1,lightable[i]+page-1,0); // spegni il led nella lightable
        #endif 

        #if (Matrix_Pads == 1 )
        single_h(matrix_remap[i],lightable[i],0,1);  // 
        bit_write(4,i+page,0);
        bit_write(1,matrix_remap[i]+page-1,0); // spegni il led nella lightable
        #endif

        #if (Matrix_Pads == 2 )
        single_h(matrix_remap[i-16],lightable[i],0,1);  // 
        bit_write(4,i+page,0);
        bit_write(1,matrix_remap[i-16]+page-1,0); // spegni il led nella lightable
        #endif
      }
    }
  }
}
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
