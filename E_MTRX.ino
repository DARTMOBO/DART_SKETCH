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

/* ============================================================
   #DART_MATRIXFX_CROSSMODE
   Selettore modalità effetto "croce" sui PAD matrice
   ----------------------------------------------------
   MATRIXFX_MODE 0 : effetto FULLWHITE (comportamento classico)
   MATRIXFX_MODE 1 : effetto XOR con lo sfondo memorizzato in status[]
                     (usa xorRowOverlayFromStatus per ogni riga)
   Per info: cerca questa ancora nel codice.
   ============================================================ */





// Effetti non distruttivi per i PAD su matrici:
// - matrixFx_drawFullWhiteUnit: accende TUTTA la matrice (8x8) a piena luce
//   usando setRowOverlay() → non modifica lo sfondo memorizzato in status[]
// - matrixFx_restoreUnit: ripristina lo sfondo memorizzato in status[]
//   usando restoreRowFromStatus(), riga per riga.


void matrixFx_drawFullWhiteUnit(byte number_of_unit) {
  for (byte row = 0; row < 8; row++) {
    // 0xFF = 8 LED accesi sulla riga
    // In base a MATRIXFX_MODE usiamo:
    // 0 → setRowOverlay (fullwhite classico)
    // 1 → xorRowOverlayFromStatus (inversione rispetto allo sfondo status[])
#if (MATRIXFX_MODE == 1)
    lc.xorRowOverlayFromStatus(number_of_unit, row, 0xFF);
#else
    lc.setRowOverlay(number_of_unit, row, 0xFF);
#endif
  }
}


void matrixFx_restoreUnit(byte number_of_unit) {
  for (byte row = 0; row < 8; row++) {
    // restoreRowFromStatus legge status[] e rimanda allo SPI la riga originale
    lc.restoreRowFromStatus(number_of_unit, row);
  }
}

// CTRL-F: SINGLE_H_REFAC_MASK
void single_h(byte number_of_unit, byte sprite, byte invert, byte send_) {

  // mantiene la tua regola: disegna solo se sprite > 46
  if (sprite > 46) {

    // remap della numerazione di "led" (cioè lightable-1)
    byte sprite_pos = remapper(sprite - 1);

    // invert: 0 => mask=0x00 (valore invariato)
    // invert: 1 => mask=0xFF (equivale a ~valore)
    byte m = invert ? 0xFF : 0x00;

    lc.setRow(number_of_unit, 6, valuetable[sprite_pos + page] ^ m, send_);
    lc.setRow(number_of_unit, 1, maxvalue[sprite_pos]          ^ m, send_);
    lc.setRow(number_of_unit, 5, minvalue[sprite_pos]          ^ m, send_);
    lc.setRow(number_of_unit, 3, modetable[sprite_pos]         ^ m, send_);

    lc.setRow(number_of_unit, 7, dmxtable[sprite_pos]          ^ m, send_);
    lc.setRow(number_of_unit, 2, qwertyvalue[sprite_pos]       ^ m, send_);
    lc.setRow(number_of_unit, 4, typetable[sprite_pos + page]  ^ m, send_);
    lc.setRow(number_of_unit, 0, lightable[sprite_pos]         ^ m, send_);
  }

  // (la tua riga commentata resta inutile, la lasciamo fuori)
  // for(int i=0;i<8;i++) lc.sendRow(number_of_unit,i);
}


#endif


 #if (Matrix_Pads == 2)

void matrix_restore (boolean numero)
{

 #if (led_eco == 1)
 lc.restore_shutdown();
  #endif
  
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
        
        if( matrix_remap[remapper(unit)] > 7 ) sendd = 1; else sendd =0; // da chiarire
        
      single_h(matrix_remap[remapper(unit)],                             // quale matrice                                                     
               lightable[remapper(unit)+16],                             // quale sprite 
               bit_read(1,remapper(unit)+16+(numero*max_modifiers)),     // positivo - negativo
               sendd);                                                   // send - no
;

      }

  lc.sendRow2(0); // rendi visibile tutto // 
  lc.sendRow2(1);
  }

  #endif



  
  #if (Matrix_Pads == 1)

// CTRL-F: MATRIX_RESTORE_PADS1_TEST_NO_EXTRA
void matrix_restore (boolean numero)
{
  #if (led_eco == 1)
    lc.restore_shutdown();
  #endif

  // offset per bit_read (evita moltiplicazioni ripetute)
  byte off = numero ? max_modifiers : 0;

  // 1) Prima passata: matrici 0..7 (send = 0)
  for (int unit = 0; unit < 12; unit++)
  {
    byte r  = remapper(unit);        // indice memoria DART
    byte mu = matrix_remap[r];       // indice matrice MAX7219

    if (mu < 8)
    {
      single_h(mu,                  // quale matrice
               lightable[r],        // quale sprite
               bit_read(1, mu + off),
               0);                  // send = 0
    }
  }

  // rende visibile il primo blocco
  lc.sendRow2(0);
  lc.sendRow2(1);

  // 2) Seconda passata: matrici 8..15 (send = 1)
  for (int unit = 0; unit < 12; unit++)
  {
    byte r  = remapper(unit);
    byte mu = matrix_remap[r];

    if (mu > 7)
    {
      single_h(mu,
               lightable[r],
               bit_read(1, mu + off),
               1);                  // send = 1
    }
  }
}

  #endif

  //---------------------------------------------------------------------------------------------------


  #if (Matrix_Pads > 0  )


 #if (led_eco == 1  )

/*
========================  PROMEMORIA SHUTDOWN MATRIX (DART)  ========================

QUADRO GENERALE
-------------------------------------------------------------------------------
Con #define led_eco == 1 l'effetto LED "vecchio" usa Parola_diretta() per accendere
righe sulle matrici. INSIEME all'effetto grafico, Parola_diretta introduce UNO
SPEGNIMENTO HARDWARE delle matrici "pari" tramite OP_SHUTDOWN del MAX7219.

RISULTATO VISIVO:
- quando muovi pot/encoder: alcune matrici diventano NERE (shutdown hardware)
- a pot fermi: dopo cycletimer==66 tutte le matrici vengono riaccese e ripopolate
  dallo status[]

-------------------------------------------------------------------------------
DOVE AVVIENE LO SPEGNIMENTO (VERO SHUTDOWN HARDWARE)
-------------------------------------------------------------------------------
File: LedControl.cpp
Funzione: LedControl::Parola_diretta(byte matrice_partenza, byte row, byte onoff)

Nel blocco FINALE della funzione, quando onoff == 1, viene inviato questo comando:

    spidata[1 + (matrice_partenza*2 + 2)] = 12;   // OP_SHUTDOWN
    spidata[(matrice_partenza*2) + 2]     = 0;    // dato = 0  --> SHUTDOWN

EFFETTO:
- La matrice "vicina" alla matrice attiva viene messa in SHUTDOWN hardware
- Questo succede A OGNI CHIAMATA con onoff==1

Quindi OGNI riga accesa via Parola_diretta(...,1) produce anche:
--> spegnimento completo della matrice accoppiata

-------------------------------------------------------------------------------
DOVE AVVIENE IL RESTORE (RIACCENSIONE)
-------------------------------------------------------------------------------
File: D_MOD.ino
Funzione: restore_end()

Quando cycletimer == 66:

    restore_end() -> matrix_restore(page)

File: D_mtrx.ino
Funzione: matrix_restore(page)

    lc.restore_shutdown();

File: LedControl.cpp
Funzione: LedControl::restore_shutdown()

Questa funzione manda OP_SHUTDOWN = 1 a TUTTE le matrici:

    spidata[i*4+3] = 12;   // OP_SHUTDOWN
    spidata[i*4+2] = 1;    // dato = 1 --> NORMAL OPERATION

EFFETTO:
- tutte le matrici escono dallo shutdown
- subito dopo matrix_restore() riscrive lo stato completo da status[]

-------------------------------------------------------------------------------
ALTRO IMPORTANTE
-------------------------------------------------------------------------------
- La funzione ufficiale:

      lc.shutdown(addr, true/false)

  NON viene mai usata con true (shutdown) nello sketch.

- L'UNICO attore che mette in shutdown hardware le matrici è:

      Parola_diretta(..., onoff==1)

  tramite scrittura diretta del registro OP_SHUTDOWN nello SPI buffer.

-------------------------------------------------------------------------------
CONSEGUENZE PRATICHE
-------------------------------------------------------------------------------
- During effetto (led_eco == 1): alcune matrici sono disabilitate via hardware
- A pot fermi: vengono riattivate *dopo* restore_end()
- restore_shutdown() oggi spesso "riaccende" matrici che sono già accese
- Il sistema funziona ma è LOGICAMENTE COMPLESSO

-------------------------------------------------------------------------------
PER LA VERSIONE NUOVA (led_eco == 0)
-------------------------------------------------------------------------------
RACCOMANDAZIONE:
- NON usare shutdown hardware per effetti grafici
- usare solo:
      setRow / status[] / ripristino manuale
- mantenere restore_shutdown() solo come sicurezza/inizializzazione

================================================================================
*/


 
void led_enc_exe_matrix () // effettino luminoso quando si girano i pot e gli encoder // versione ECONOMICA con paroladiretta - molto essenziale
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
    //  encled[0] = map32(adattamento_encled,0,23,23,0);

    // encled_matrix = encled[0] /10; // ottengo escursione da 24 punti
    // encled_matrix = map32(encled[0],0,23,23,0); // mi serviva invertire sopra e sotto.
    // AA  encled[0] = map32((encled[0]),0,240,0,23); // riassumo le righe sopra in una sola
    
     // Serial.println(encled_matrix);
     // Serial.println("--");

encled_matrix = map32((encled[0]),0,240,0,7);
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
  
  #endif



// ---------------------------------------------------------------------------------------------------------------------
#if (Matrix_Pads > 0 && led_eco == 0)


/*
void led_enc_exe_matrix() {

     byte encled_matrix;
     byte encled_matrix_riga;

   
 
if (qwertyvalue[general_mempos] > 0) // verifico che nelle impostazioni GENERAL siano attivati gli effetti // 0 = no / 1 = pots / 2 = spinners / 3 = buttons
  {
                                                                                      // se encled supera il valore massimo o minimo
                                                                                      // per esempio girando un encoder...
                                                                                      // l'effetto luminoso deve ricominciare ni nuovo dal basso o dall'alto
   if (encled[0] <0) { encled[0] = 255 ;}
   else
   if (encled[0] >255) {encled[0] = 0;}

encled_matrix = map32(encled[0], 0,255,22,0);

if ( encled_matrix > 15 ){
  // riga orizzontale sulle ultime quattro  matrici - 
  encled_matrix_riga = order_row(encled_matrix -15);
  // accendi riga orizzontale in corrispondenza di   encled_matrix_riga
  }
else 
    if ( encled_matrix > 7   ){
       // riga orizzontale sulle  quattro matrici intermedie 
      }
    else {
        // riga orizzontale sulle prime quattro matrici
      }
  }

}
*/



/*
 * led_enc_exe_matrix()
 * -----------------------------------------------------------------------------
 * SCOPO GENERALE
 * -----------------------------------------------------------------------------
 * Gestisce l'effetto grafico "barra mobile" sulle 12 matrici di led collegate
 * alla DART. La barra è composta da 2 righe orizzontali sovrapposte, replicate
 * su un quartetto di matrici (1–4, 5–8, 9–12) e si sposta lungo un percorso
 * di 23 step logici (0..22) calcolati a partire da encled[0].
 *
 * ARCHITETTURA LOGICA
 * -----------------------------------------------------------------------------
 * - encled[0]  : valore "grezzo" 0..255 usato come sorgente per la posizione.
 * - encled[1]  : copia dell'indice mappato (0..22) usato come debug / monitor.
 * - encled_matrix : posizione logica corrente della barra (0..22).
 * - last_encled_matrix : posizione logica precedente della barra (0..22) usata
 *                        per sapere DOVE spegnere/ripristinare lo sfondo.
 *
 * - Le 12 matrici sono divise in 3 "quartetti":
 *      stage 0 : matrici  0..3  (prime 4, fisicamente 1–4)
 *      stage 1 : matrici  4..7  (centrali, 5–8)
 *      stage 2 : matrici  8..11 (ultime 4, 9–12)
 *
 * - order_row[] : tabella di mapping da indice logico (0..7) a riga fisica
 *   0..7 del MAX7219. Serve per definire l'ordine di percorrenza delle righe
 *   (ad esempio: zig-zag, dal centro all'esterno, ecc.).
 *
 * MODELLO GRAFICO
 * -----------------------------------------------------------------------------
 * - status[] nella DartLedControl conserva SEMPRE lo "sfondo" (stato base
 *   delle matrici: sprite, testo, layout).
 *
 * - La funzione NON modifica mai status[], ma usa:
 *      - xorRowOverlayFromStatus() : legge lo sfondo, calcola una riga XOR
 *                                    (negativo) e la manda ai LED.
 *      - restoreRowFromStatus()    : rimanda ai LED la riga pulita dallo
 *                                    sfondo (annullando l'effetto).
 *
 * - In questo modo:
 *      - lo sfondo resta intatto in status[],
 *      - la barra è un overlay temporaneo in XOR,
 *      - quando si sposta, le righe "abbandonate" vengono ripristinate
 *        dallo sfondo invece di essere azzerate brutalmente.
 */

// FLASH_SAVER_LED_ENC_HELPER
// Calcola: quale quartetto (start_dev) e le 2 righe fisiche (row1,row2)
// a partire da un indice logico 0..22 (encled_matrix).
// È lo stesso identico calcolo che prima era duplicato per "current" e "prev".
static void _encMatrixBandParams(byte encled_matrix, byte &start_dev, byte &row1, byte &row2)
{
  // quartetto: 0..7 -> dev 0..3 | 8..15 -> dev 4..7 | 16..22 -> dev 8..11
  byte base;
  if (encled_matrix > 15) { start_dev = 8; base = 16; }
  else if (encled_matrix > 7) { start_dev = 4; base = 8; }
  else { start_dev = 0; base = 0; }

  byte idx = encled_matrix - base;   // 0..7 (stage 2 arriva solo a 6)
  byte idx2;

  // coppia “a barra” su 2 righe (come nel tuo codice)
  if (idx == 0) idx2 = 1;
  else if (idx < 7) idx2 = idx + 1;
  else idx2 = 6; // idx==7

  row1 = order_row[idx];
  row2 = order_row[idx2];
}


void led_enc_exe_matrix() {

  byte encled_matrix;
  byte encled_matrix_riga;   // (non usata qui, ma la lascio per compatibilità)

  // 255 = invalido all’avvio (nessuna banda precedente)
  static byte last_encled_matrix = 255;

  // effetti LED attivi?
  if (qwertyvalue[general_mempos] == 0) return;

  // Nota: encled[0] è byte -> questi check sono “difensivi” (li lasciamo)
  if (encled[0] < 0) encled[0] = 255;
  else if (encled[0] > 255) encled[0] = 0;

  // Mapping 0..255 -> step 0..22 (come nel tuo)
  uint16_t tmp = (uint16_t)encled[0] * 23;
  byte step = tmp / 255;
  if (step > 22) step = 22;

  encled_matrix = 22 - step;
  encled[1] = encled_matrix;

  // se non cambia lo step logico, non ridisegniamo
  if (encled_matrix == last_encled_matrix) return;

  // --- banda corrente ---
  byte start_dev, row_addr, row_addr2;
  _encMatrixBandParams(encled_matrix, start_dev, row_addr, row_addr2);

  // --- banda precedente (se valida) ---
  bool has_prev = (last_encled_matrix <= 22);
  byte prev_start_dev = 0, prev_row_addr = 0, prev_row_addr2 = 0;

  if (has_prev) {
    _encMatrixBandParams(last_encled_matrix, prev_start_dev, prev_row_addr, prev_row_addr2);

    // stesso quartetto? allora alcune righe vecchie coincidono con le nuove
    bool same_quartet = (prev_start_dev == start_dev);

    for (byte m = prev_start_dev; m < (byte)(prev_start_dev + 4); m++) {

      bool row1_is_still_used =
        same_quartet && (
          (prev_row_addr == row_addr) ||
          (prev_row_addr == row_addr2)
        );

      if (!row1_is_still_used) {
        lc.restoreRowFromStatus(m, prev_row_addr);
      }

      bool row2_is_still_used =
        same_quartet && (
          (prev_row_addr2 == row_addr) ||
          (prev_row_addr2 == row_addr2)
        );

      if (!row2_is_still_used) {
        lc.restoreRowFromStatus(m, prev_row_addr2);
      }
    }
  }

  // --- overlay XOR della banda corrente (non tocca status[]) ---
  for (byte m = start_dev; m < (byte)(start_dev + 4); m++) {
    lc.xorRowOverlayFromStatus(m, row_addr,  0xFF);
    lc.xorRowOverlayFromStatus(m, row_addr2, 0xFF);
  }

  last_encled_matrix = encled_matrix;
}



// Effetto "a scoppio" a CROCE per i layout basati su matrici (MAX7219).
// ---------------------------------------------------------------------
// Riutilizza le variabili globali già presenti:
//
//   buttonefx  = stato / step dell'effetto (0 = spento, 1/2/3 = frame animazione)
//   buttonefxd = unità centrale (1..12) da cui parte lo scoppio
//
// Nota: in questo layout le matrici sostituiscono lo shifter, quindi
//       le stesse variabili non vengono usate contemporaneamente da
//       buttonledefx() (che è solo per shifter_active == 1).
//
// Sequenza:
//   - alla pressione del pad: in push_buttons() viene impostato
//         buttonefxd = matrix_remap[chan];
//         buttonefx  = 1;
//         cycletimer = 0;
//   - matrixbuttonledefx() usa cycletimer come metronomo:
//         step 1 → croce distanza 1 (vicini immediati)
//         step 2 → spegne la croce 1 e accende la croce distanza 2
//         step 3 → spegne la croce 2 e chiude l'effetto.
//
// L'overlay usa le funzioni non distruttive:
//   matrixFx_drawFullWhiteUnit()  → bianco pieno sull'unità
//   matrixFx_restoreUnit()        → ripristina lo sfondo da status[]
//
// Niente viene acceso "fuori schermo": la griglia logica è 3x4 unità:
//
//       1  2  3  4
//       5  6  7  8
//       9 10 11 12
//
// e i controlli di riga/colonna impediscono di uscire da questi limiti.

// Applica la croce per una data distanza dal centro (1 = vicini, 2 = più lontani).
// Se draw == true → overlay bianco; se draw == false → ripristino da status[].
static void matrixFx_applyCross(byte centerUnit, byte distance, bool draw)
{
  // centerUnit è l'indice della matrice 0..11 (stesso schema usato da LedControl/DartLedControl)
  // distance = 1 → croce vicina; distance = 2 → croce più ampia
  if (centerUnit > 11) return;
  if (distance == 0) return;

  // Ricostruiamo la posizione logica sullo "schermo" 3x4:
  // 0  1  2  3
  // 4  5  6  7
  // 8  9 10 11
  byte row = centerUnit / 4; // 0..2
  byte col = centerUnit % 4; // 0..3

  // sopra
  if (row >= distance) {
    byte u = centerUnit - 4 * distance;
    if (draw) matrixFx_drawFullWhiteUnit(u); else matrixFx_restoreUnit(u);
  }

  // sotto
  if ((byte)(row + distance) <= 2) {
    byte u = centerUnit + 4 * distance;
    if (draw) matrixFx_drawFullWhiteUnit(u); else matrixFx_restoreUnit(u);
  }

  // sinistra
  if (col >= distance) {
    byte u = centerUnit - distance;
    if (draw) matrixFx_drawFullWhiteUnit(u); else matrixFx_restoreUnit(u);
  }

  // destra
  if ((byte)(col + distance) <= 3) {
    byte u = centerUnit + distance;
    if (draw) matrixFx_drawFullWhiteUnit(u); else matrixFx_restoreUnit(u);
  }
}







// Animazione temporizzata a croce per i PAD su matrici.
// Viene richiamata dal loop principale quando Matrix_Pads > 0.
void matrixbuttonledefx()
{
  // Se non abbiamo un centro valido, nessun effetto è attivo.
  // Indici validi: 0..11 (0 = prima matrice nella catena MAX7219)
  if (buttonefxd > 11) return;
 byte timing = 50;
  // STEP 1: croce vicina (distanza 1)
  if (buttonefx == 1 && cycletimer == timing)
  {
    matrixFx_applyCross(buttonefxd, 1, true);  // overlay bianco sui vicini
    cycletimer = 0;
    buttonefx  = 2;                            // passa al frame successivo
  }

  // STEP 2: spegni croce 1, accendi croce distanza 2
  else if (buttonefx == 2 && cycletimer == timing)
  {
    matrixFx_applyCross(buttonefxd, 1, false); // ripristina vicini
    matrixFx_applyCross(buttonefxd, 2, true);  // overlay croce più ampia
    cycletimer = 0;
    buttonefx  = 3;
  }

  // STEP 3: spegni croce 2 e chiudi l'effetto
  else if (buttonefx == 3 && cycletimer == timing)
  {
    matrixFx_applyCross(buttonefxd, 2, false); // ripristina croce 2
    cycletimer = 0;
    buttonefx  = 0;
    buttonefxd = 0;                            // nessun centro attivo
  }
}




//--
//--
//--



#endif // Matrix_Pads > 0 && led_eco == 0



   #if (shifter_active == 0 && stratos == 0 && Matrix_Pads == 1)
   
  void ledControl_matrix (byte chann, byte stat)  {


      bit_write(1, matrix_remap[chann]+page,stat);  
     
    }
   #endif
         

  
