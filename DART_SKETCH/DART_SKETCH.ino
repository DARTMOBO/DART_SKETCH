

  ///////////////////////////
  // DART_SKETCH   v1.88   // 
  // Massimiliano Marchese //
  // www.dartmobo.com      //
  ///////////////////////////








 /* 
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */


#include "DART_config.h"   // central compile-time settings




















// =========================================================
// STAGED BUTTON READ API (debounced button reading)
// - Makes push_buttons_lettura_stage() + accessors visible to ALL tabs.
// - When STAGED_BUTTON_READ_TEST is OFF, stubs compile away to keep compatibility.
// =========================================================
#if (STAGED_BUTTON_READ_TEST == 1)
  void push_buttons_lettura_stage(byte velo);
  byte pb_pressed();
  byte pb_released();
  byte pb_down();
  byte pb_velo();
#else
  // Stubs: allow code to compile even when staged system is disabled.
  // They return "no event" and do nothing.
  inline void push_buttons_lettura_stage(byte velo) { (void)velo; }
  inline byte pb_pressed()  { return 0; }
  inline byte pb_released() { return 0; }
  inline byte pb_down()     { return 0; }
  inline byte pb_velo()     { return 0; }
#endif

 
//---------------------------------------------
// =====================================================================
// CTRL-F: #MAP_HELPERS
// Flash-saving helpers: replace Arduino map32() (long, out-of-TU) with
// an inline equivalent that is bit-identical for all integer inputs.
// NOTE: map32() in Arduino does NOT clamp. This one doesn't either.
// =====================================================================
static inline int32_t map32(int32_t x, int32_t in_min, int32_t in_max,
                           int32_t out_min, int32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Common DART case: 10-bit ADC (0..1023) -> MIDI (0..127), bit-identical
// to map32(x, 0,1023, 0,127) but typically compiles smaller.
static inline uint8_t map7_0_1023_to_0_127(uint16_t x)
{
  return (uint32_t)x * 127u / 1023u;
}

//---------------------------------------------------------------


#if defined (__AVR_ATmega32U4__)  
#include "_DART_MIDI.h"

//---------------------------------------------------------------

  #if (hid_mouse == 1)
#include <Mouse.h>
 #endif // (hid_mouse == 1)


//---------------------------------------------------------------

#if ( hid_keys  == 1)
#include <Keyboard.h>

#endif // (hid_keys == 1)
midiEventPacket_t rx;
#endif // defined (__AVR_ATmega32U4__)
#if defined(ARDUINO_ARCH_SAMD) && !defined(__AVR_ATmega32U4__)
#include "_DART_MIDI_SAMD.h"
#endif


#if (Touch_sensors_enable == 1)

// ===== TOUCH_SAMD_AVR_SPLIT =====
// AVR: usa la libreria storica CapacitiveSensor
// SAMD: NON includere la libreria touch classica; useremo una lettura raw dedicata in E_touch.ino

#if defined(__AVR_ATmega32U4__)
  // pin 9 e 7 sono gli input - 8 è l'emettitore
  #include "_DART_Touch_Sensor.h"

  #if (stratos == 1 )
  CapacitiveSensor cs_4_2[1] = {CapacitiveSensor(9,8)}; // stratos
  #endif

  #if (stratos == 0 )
  CapacitiveSensor cs_4_2[2] = {CapacitiveSensor(8,7), CapacitiveSensor(8,9)};
  #endif
#endif

//#if defined(ARDUINO_ARCH_SAMD) && !defined(__AVR_ATmega32U4__)
  // Stato raw minimale per il touch 1 su SAMD
  //byte touchStateT1 = 0;
//#endif

#endif // (Touch_sensors_enable == 1)




#if (DMX_active == 1 && stratos == 0)
#include "_DART_Dmx_Out.h"
#endif // (DMX_active == 1 && stratos == 0)

#include "_DART_EEPROM.h"

#if (shifter_active == 1 && stratos == 0)
#include "_DART_Shifter.h"
#endif // (shifter_active == 1 && stratos == 0)

//-----------------------------------------------------------------------------------
 #if (Matrix_Pads >0  && stratos == 0)
// =====================================================================
// DART LED MATRIX: scelta libreria (bitbang vs SPI hardware)
// =====================================================================

// 0 = usa la vecchia LedControl bitbang (quella hackerata che già funziona)
// 1 = usa DartLedControl (versione SPI hardware basata su Faster_LedControl)
  const byte order_row[8] = {6,1,5,3,7,2,4,0};
  
#define DART_USE_HW_SPI 1

#if (DART_USE_HW_SPI == 1)
  #include "_DART_LedControl.h"
  typedef DartLedControl DartLedLib;
#else // !DART_USE_HW_SPI
  #include "LedControl.h"
  typedef LedControl DartLedLib;
#endif // DART_USE_HW_SPI

 #endif // (Matrix_Pads >0  && stratos == 0)
//--------------------------------------------------------------------------------------


 unsigned long delay_us;
//byte diff_pot = 19;
//#if (stratos == 1)
#if (MIDI_OUT_CLEANER == 1) // CTRL-F: MIDI_OUT_CLEANER_VAR
byte out_filter; // usato in void noteon come filtro antiflicker per gli encoders di scarsa qualità 
#endif // (MIDI_OUT_CLEANER == 1)
//#endif
byte offset_modifier_; // usato in midiout per shiftare i segnali su altro canale ottenendo un effetto simile a PAGE 

  byte do_; // attualmente usato come counter per flash lights in modalità matrix durante caricamento preset
///////////////////////////////////////////////////////////////////////
const byte max_modifiers = 60;
const int maxbeam = 520;
const int minbeam = 290;

///////////////////////////////////////////////////////////////////////
#if (LED_pattern == 0) 
   const byte encledtable[16]= { 4,5,6,7, 12,13,14,15, 3,2,1,0, 11,10,9,8,};  // dart one 
 #endif // (LED_pattern == 0)

 #if (LED_pattern == 1) 
  const byte encledtable[16] = { 12,13,14,14,15,0,7,7,1,6,2,2,5,3,4,4};  // kombat
#endif // (LED_pattern == 1)

#if (LED_pattern == 2) 
   const byte encledtable[16]= { 15,14,13,12,  7,6,5,4,  8,9,10,11,  0,1,2,3};  // NB-boards
#endif // (LED_pattern == 2)

#if (LED_pattern == 3) 
 // const byte encledtable[16] = {0,3,2,2,1,7,6,6,5,4,11,11,10,9,8,8};  // kombat - nb // 
    const byte encledtable[16] = {8,8,9,10,11,11,4,5,6,6,7,1,2,2,3,0};
#endif // (LED_pattern == 3)

#if (LED_pattern == 4) 
 // const byte encledtable[16] = {0,3,2,2,1,7,6,6,5,4,11,11,10,9,8,8};  // kombat - nb2 // 
    const byte encledtable[16] = {8,8,10,11,9,9,4,5,6,6,7,1,2,2,3,0};
#endif // (LED_pattern == 4)

#if (LED_pattern == 5) 
 // const byte encledtable[16] = {0,3,2,2,1,7,6,6,5,4,11,11,10,9,8,8};  // kombat - nb3 // 
    const byte encledtable[16] = {11,11,9,8,10,10,4,5,6,6,7,1,2,2,3,0}; // 11 9 8 10
#endif // (LED_pattern == 5)

#if (LED_pattern == 6) 
 // const byte encledtable[16] = {0,3,2,2,1,7,6,6,5,4,11,11,10,9,8,8};  // kombat - nb4 // 
    const byte encledtable[16] = {11,11,10,10,9,9,8,8,3,3,2,2,1,1,0,0}; // 11 9 8 10
#endif // (LED_pattern == 6)

 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////



 const PROGMEM  byte qwertymod[33] = { // da 0 a 24 ci sono i modificatori - da 25 a 32 controlli mouse
   
 0,   // niente 
 194, // f1
 195, // f2
 196, // f3
 
 197, // f4
 198, // f5 
 199, // f6
 200, // f7
 
 201, // f8
 202, // f9
 203, // f10
 204, // f11
 
 205, // f12
 215, // right arrow
 216, // left
 217, // down
 
 218, // up
 128, // left crtl
 129, // left shift
 130, // left alt
 
 179, // tab
 176, // return
 177, // esc
 212, // delete
 };


   const byte matrix_remap[] = {1, 6, 2, 5, 7,11, 0, 4, 3,10, 8,9 ,12, 3, 0, 8};

/*
byte matrix_remappp(byte inputt)
{return matrix_remap[inputt];  // 16 dipende dalla posizione dei pulsanti force sensing, in qusto caso sono collegati in posizione 17 a salire
  }
  */



// const PROGMEM byte matrix_remap[] = {1, 6, 2, 5, 7,11, 0, 4, 3,10, 8,9 ,12, 3, 0, 8};

const PROGMEM byte  input_remap[]  = {6,8,4,2,7,1,5,3,};
 
byte remapper(byte input) 
{
  if (input == 255) return 255; 
  return  (( pgm_read_byte(input_remap + ((input)-(((input)/8)*8)) ) +(((input)/8)*8))) -1;}


//|||||||||||||||||||||||||||||||||
// const byte select[] = {4,5,6}; // pins connected to the 4051 input select lines

//|||||||||||||||||||||||||||||||||
byte  eeprom_preset_active ; // activate auxiliary preset at startup;
byte channel;
byte plexer;
byte chan;



// ricordiamo che se premuto il valore scende verso gnd cioè zero 0 - se rilasciato il pulsante va in alto verso 1024 (consiverando valore in un range 0-1024 classico - il range poi con le funzioni fastanalog si restringe a 8bit 0-255
const int upper_val = 800; // ho scelto 800 perché ho visto che alcuno pads velocity sensitive , a riposo non salgono sopra 840 - ricordiamo che a riposo il valore tende all'altro cioè 1024 - 
const int lower_val = 400;
/*
  // per pad velocity molto sensibile - un pad premuto fortissimo si avvicina allo zero - premuto piano scende facilmente sotto i 750 
const int upper_val = 900;
const int lower_val = 750;

 // per pulsanti normali - valori storici del dart... in realtà funziona bene anche coi valori pad-velo che possono essere resi universali
const int upper_val = 750;
const byte lower_val = 255;

*/




int valore; // analog 0-1024 value for 4051 analog readings.
volatile  byte potOut;
//||||||||||||||||||||||||||||||
byte mousex;
byte mousey;
// byte mouserall;
//int mouseyacceleration; // 
//int mousexacceleration; // 
byte mouse_wheel_speed_counter;
/////////////////////////////////////////////////////////////
volatile byte spinner_mempos[2];             // da editor si sceglie quale sara'  la memoryposition dell'encoder 
                                             
volatile byte touch_mempos[2];
byte V_touch_regulator[2] = {1,1};
byte mouse_mempos ;
byte PADS_mempos;
byte distance_sensor_mempos;
byte page_mempos;
 byte general_mempos = 0;
/////////////////////////////////////////////////////////////////////
// CTRL-F: PAGE_BOOT_SELECT
#if (ENABLE_POT_TAKEOVER == 1)
byte page = 0; // takeover: boot in Page1
//byte takeover_init = 1;
#else // !ENABLE_POT_TAKEOVER
byte page = max_modifiers ; // legacy boot
#endif // ENABLE_POT_TAKEOVER

byte cycletimer;   /// conteggio di tempo in base ai cicli - serve per regolare effetti visivi led - e per il timing del virtualtouch
                   // viene aumenato di +1 ad ogni ciclo - fino a un max di 250

boolean openeditor;
byte editorcounter;
byte memoryposition;
byte type;
byte incomingByte;
byte note;
byte velocity;
byte action=2;

volatile byte old_cmd; // midi out doubles-filter, buffer variables 
volatile byte old_pitch;
volatile byte old_velocity;

byte padDecay[4];
byte padNum;
volatile  unsigned int padVal;  // 
volatile  unsigned int padVal2; // 
//////////////////////////////////////////////////////////////////////////////////   touch sensor
 byte limit_touch = 250;

volatile byte  readingsXen[2][3] ;      // the readings from the analog input
// byte readingsXenn = 250;
// volatile byte readingsXen2[3];
// volatile byte readingsXenn2;
//const byte Xenlimit = 6;
// const byte Xendivider = 2;
// volatile byte lettura;

#if ( stratos == 1)
int decaysensor[2]; //= decaysensor_value;
#endif // (stratos == 1)

#if ( stratos == 0)
byte decaysensor[2]; //= decaysensor_value;
#endif // (stratos == 0)

byte indexXen = 0;                  // the index of the current reading
// unsigned int totalXen[2];                  // the running total
byte averageXen[2];                // the average
byte lower_Xen[2] = {200,200};           
byte higher_Xen[2] = {40,40}; 

//////////////////////////////////////////////////////////////////////////////////////


 byte buttonefx = 0; 
 byte buttonefxd = 0;
 byte buttonefxu = 0;
 byte shifterwrite = 1; // led refresh
 int encled[2]; //  used for encoder led animation // encled[1] è utilizzato solo in matrix mode - per effetti luminosi

//////////////////////////////////////////////////////////////////////////////////////
#if (shifter_active == 1 && stratos == 0)
#define SER_Pin 10 //SER_IN
#define RCLK_Pin 11 //L_CLOCK
#define SRCLK_Pin 12 //CLOCK
#define NUM_REGISTERS 4 // how many registers are in the chain
Shifter shifter(SER_Pin, RCLK_Pin, SRCLK_Pin, NUM_REGISTERS);
#endif // (shifter_active == 1 && stratos == 0)

#if (Matrix_Pads > 0 && stratos == 0)

/*
 * per usare la modalità matrix bisogna disattivare gli  shifter!!!! ved setup all'inizo
 * 
 * osservazioni per lavorare su matrix pads:
 * se metto la luminosità a 15 - (al massimo) aumentano i problemi, le ultime 4 matrici si incantano facilmente
 * non so ancora se questa cosa è risolvibile cambiando resistenze di carico vin o se serve un alimentatore più potente
 * - confermato, la luminosità al massimo crea moltissimi problemi
 * - mettendo una resistenza iset da 220k tutto funziona bene, la luminosità può anche essere messa a 5 e non ci sono problemi di unità che si spengono  o altro
 * 
 * l'ingresso del +5 direttamente sulla prima matrice migliora le cose -prima era sulla 9a
 * è una questione proprio di distribuzione elettrica di rumore sul circuito 
 * penso che in effetti questi chip siano stati pensati per essere collegati in serie quindi vogliono la corrente all'inizio dela catena
 * 
 * l'ultima matrice non si accendeva perché ho messo delle resistenze in serie du load e clock
 * non so se queste resistenze abbiano un reale beneficio comunque ho messo resistenze troppo forti 10k 
 * si dovrebbero usare resistenze da 2k secondo quello che leggo sul forum 
 * su https://forum.allaboutcircuits.com/threads/long-chain-of-max7219-matrix-displays-fail.151773/page-2
 * 
 * alcuni consigliano un buffer basato su  74LVC2G17W6-7 per rinforzare il segnale di load e clock...
 * ogni 6 matrici ... boh
 * 
 * sempre da internet leggo che la resistenza di terminazione si collega dalla fine della linea a massa
 * il tipo avrebbe usato usa resistenza da 1k che dice lui tende a scaldarsi - per ora ho provato solo con 10k
 * 
 * 
 * da altre parti ho sentito parlare di un condensatore di disaccoppiamento - sul +5v delle matrici
 * ho anche pensato a un trasformatore dcdc che separa la corrente della sezione matrici, ma mi pare una cazzata
 * 
 * ------------- 
 * 
 * allo stato attuale mi rimane da capire perché le ultime 4 matrici sono sempre leggermente più luminose
 * e perché l'aggiornamento degli sprite funziona bene col midi feedback ma non premendo sui tasti... - adesso sembra superato anche questo problema.
 * 
 * ...................
 * 
 * nuove osservazioni costruttive . se voglio sostituire una matrice di led è un vero inferno, è difficilissimo toglierla
 * si potrebbero creare dei pads per saldare i piedini delle matrici dopo averli piegati
 * 
 * la scheda pcb top v15 ha dei bug : i contatti +5v e iset sono uniti!
 * i pads per saldare i max7219 dovrebbero essere più lunghi e comodi
 * i percorsi di distribuzione del segnale clock e load dovrebbero seguire il percorso del segnale din , dalla prima all'ultima matrice
 * dovrebbero inoltre includere lo spazio per le resistenze da mettere in serie da 1k (per ogni ingresso load e clock di ogni unità) 
 * e anche per le resistenze di terminazione
 * il ground forse dovrebbe essere spanso per tutta la piastra, in modo che eventuali condensatori di disaccoppiamento abbiano un senso 
 * infatti non basta connettere questi semplicemente a terra, anzi è sbagliato collegarli  a massa troppo vicino a dove il max7219 va a massa.
 * 
 * ----------------------------------------------------------
 * 
 * bisogno chiarire esattamente come vengono associati gli sprite ai pulsanti, erché nn mi ricordo.
 * 
 * 
 * 
 * 
 */
byte max_units = 12;

// LedControl lc=LedControl(10,11,12,max_units); // vecchio pinning - si usano i pin digitali uguali a quelli usati per gli shifters nei dart normali

// LedControl lc=LedControl(16,15,12,max_units); // pinning che usa i pin leonardo d16 e d15 che sono sul connettore icsp - per fusare libreria ultraveloce

// config pin matrice
#define MAX_DIN_PIN 16   // ICSP MOSI
#define MAX_CLK_PIN 15   // ICSP SCK
#define MAX_CS_PIN  12   // CS comune a tutte le matrici

#if (DART_USE_HW_SPI == 1)
  // SPI hardware: la libreria ignora DIN/CLK e usa direttamente MOSI/SCK hardware.
  // Qui passi SOLO il CS e il numero di dispositivi.
  DartLedLib lc = DartLedLib(MAX_CS_PIN, max_units);
#else // !DART_USE_HW_SPI
  // Bitbang: come prima, 4 argomenti (DIN, CLK, CS, numDevices)
  DartLedLib lc = DartLedLib(MAX_DIN_PIN, MAX_CLK_PIN, MAX_CS_PIN, max_units);
#endif // DART_USE_HW_SPI





// LedControl(int dataPin, int clkPin, int csPin, int numDevices) 

//byte order_line[8] = {5,6,0,1,3,4,7,2};


 void set_unit(byte number_of_unit){ 
  lc.shutdown(number_of_unit,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(number_of_unit,5); // luminosita' da 1 a 15
  /* and clear the display */
  lc.clearDisplay(number_of_unit); 
   }
   
#endif // (Matrix_Pads > 0 && stratos == 0)

//////////////////////////////////////////////////////////////////////////////////////
//byte contoencoder;
byte numero2h;
// byte lastEncoded[2] ; // adesso queste variabili contendono la scala scritta da editor...
int encodervaluepot[2] ; 
byte encodervaluepot_buffer[2]; // serve per spegnere le note gia'  suonate in encoder - scale mode
volatile byte MSB[2] ; 
volatile byte LSB[2] ;
byte encoder_block[2]= {64,64} ; // serve per bloccare l'attivita'  dell'encoder quando viene toccato ma tenuto fermo - per registrare una scala.
////////////////////////////////////////////////////////////////////////////////  
// CTRL-F: data_LB_SIZE_SWITCH
#if (ENABLE_POT_TAKEOVER == 1)
volatile byte data_LB[128] ; // split: 0..63 legacy + 64..127 Page2 pots // ex lastbutton

#else // !ENABLE_POT_TAKEOVER
volatile byte data_LB[64] ; // legacy size
#endif // ENABLE_POT_TAKEOVER
#if ( stratos == 0)
byte data_LB_debounce = 5;
#endif // (stratos == 0)

#if ( stratos == 1)
byte data_LB_debounce = 40;
#endif // (stratos == 1)

const byte data_MODE_readmode[41] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //  0–10: pulsanti → digitalRead
  1, 1, 1, 1, 1,                   // 11–15: potenziometri → analogRead
  0,                                  // 16 seq
  0,                                   // 17 page
  4,                                // beam
  2,                               // 19: encoder
  0,                               // 20
  3,                               // 21  spinner 1 - inutile leggere due volte - si salta la lettura, perché gli spinner vengono letti a parte
  3,                               // 22  spinner 2  - inutile leggere due volte - si salta la lettura, perché gli spinner vengono letti a parte
  3,                              // 23 touch 1
  3,                                 // 24 touch 2
  3,                                   // 25 mouse
  3,                         // 26: general
  1,                               // 27: velo pads - analogread
  3,                               // 28: non usato - sprite
  0,                                // 29 - enc reset
  0,                            // 30: shifter
  1, 1, 1, 1,                       // 31–34: analogici user
#if (FAST_FEEDBACK == 1)
  3,                              // 35 LED ONLY -> skip lettura (fast MIDI->LED)
  3,                              // 36 DMX ONLY -> skip lettura (fast MIDI->DMX)
#else
  1,                              // 35 LED ONLY (FAST_FEEDBACK=0): torna analogRead legacy
  1,                              // 36 DMX ONLY (FAST_FEEDBACK=0): torna analogRead legacy
#endif
  1,                                // 37 qwerty-pot
  1, // 38 scene_control_pot
  0,  // 39 scene_record_button
  2    // 40 encoder-scene morph
};


volatile byte data_LT[65] // ex lightable // ho provato a scendere a 64 - ma si creavano stranissimi problemi di conflitto memoria con buttonled_efx - messaggi midi disordinati etc etc
               //la vecchia data_LT era fissa.

 = { //40,
1,40,3,40,2,40,0,40,
5,40,7,40,6,40,4,40,
9,40,11,40,10,40,8,40,
13,40,15,40,14,40,12,40,
17,40,19,40,18,40,16,40,
21,40,23,40,22,40,20,40,
};

 byte  data_VA[max_modifiers*2] ; // dove  max_modifiers = 60  // ex valuetable
 byte  data_TY[max_modifiers*2] ; // ex typetable

/*
qualche nota per ricordare a qule cifra data_TY (type) corrispondono i vari messaggi midi
questi sono i valori effettivi usati per mandare fuori midi con serial out
soltanto nel trasferimento dei preset da editor a Dart di utilizzano delle conversioni per rimanere nel range 0-127

Note Off          128 + Channel 0-127 Pitch 0-127 Velocity
Note On           144 + Channel 0-127 Pitch 0-127 Velocity
Poly Pressure     160 + Channel 0-127 Pitch 0-127 Pressure
Control Change    176 + Channel 0 Undefined 0-127 MSB
Program Change    192 + Channel 0-127 Program Not used
Channel Pressure  208 + Channel 0-127 Pressure  Not used
*/
 
 byte  data_MI[max_modifiers] ; // ex minvalue
 byte  data_MA[max_modifiers] ; // ex maxvalue
 byte  data_MODE[max_modifiers] ; //  ex modetable
 byte  data_QW[max_modifiers];  //  EX qwertyvalue
 byte  data_DM[max_modifiers]; // ex dmxtable

	// -----------------------------------------------------------------------------
	// CTRL-F: POT_EMA_ARRAY
	// EMA ("condensatore digitale") per i POT in range 0..255.
	// - Per ora usiamo un array dedicato (60 byte).
	// - In futuro vedremo se e' possibile riusare una tabella esistente.
	// -----------------------------------------------------------------------------
	#if (POT_EMA_ENABLE == 1)
	byte pot_ema_8[max_modifiers];
	#endif

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
// CTRL-F: BITSTATUS_SIZE_SWITCH
#if (ENABLE_POT_TAKEOVER == 1)
volatile byte bit_status[5][max_modifiers/4];
#else // !ENABLE_POT_TAKEOVER
volatile byte bit_status[4][max_modifiers/4];
#endif // ENABLE_POT_TAKEOVER
                                              // il feedback e i led
     
 // 1 - ledstatus 1 e 2
 // 2 - feedback_bit1
 // 3 - feedback_bit2
 // 4 - bit_toggle 1 e 2

 byte feedop2 (byte input) { // used in midi feedback bit operations
 return input/8; }

 // NOTE (SAMD / strict compilers): questa funzione non deve "ritornare" nulla.
 // Su alcune toolchain (es. Metro M0 / GCC con -Werror) un return mancante diventa errore.
 void bit_write(byte array_, byte posizione, byte stato)
 {  bitWrite(bit_status[array_-1][feedop2(posizione)],posizione-(feedop2(posizione)*8),stato); }

  byte bit_read( byte array_,byte posizione)
  { return bitRead(bit_status[array_-1][feedop2(posizione)],posizione-(feedop2(posizione)*8));  }
 //  byte scrivi_bool (boolean numero) {  Serial.print(numero);  }
  
///////////////////////////////////////////////////////////////////////////
boolean pagestate = 0;


/*
// ============================================================
// CTRL-F: PAGESWITCH_DIAG_COUNTERS  (DISABLED)
// Diagnostica pageswitch (boot-safe): contatori leggibili DOPO il boot.
// Usata SOLO per inchiodare l'"assassino" del takeover al boot.
// Ora non serve più: lasciata commentata per riferimento futuro.
// ============================================================
volatile unsigned long ps_calls = 0;
volatile unsigned long ps_changes = 0;
volatile unsigned long ps_arm = 0;
volatile byte ps_first_page = 255;
volatile byte ps_first_pagestate = 255;
volatile byte ps_first_reason = 255;
volatile byte ps_setup_page = 255;
volatile byte ps_setup_pagestate = 255;
*/
// CTRL-F: POTPAGE_INIT_MASK
// bit0=Page1 init OK, bit1=Page2 init done (targets copied once)
// CTRL-F: POTPAGEINITMASK_GUARD
#if (ENABLE_POT_TAKEOVER == 1)
byte potPageInitMask = 1; // bit0 Page1 ok, bit1 Page2 init pending
#endif // ENABLE_POT_TAKEOVER

/////////////////////////////////////////////////////////////////////////// 
// ------------------------------------------------------------- scale play
// void scale_play
byte at; // serve per lo scale player - ved enc_pot_mode
byte beam_scala_buffer;
byte beam_counter;
int scala[4]; // scala[4] è un array che cotiene 4 scale... 
              // scalaSpinner0-page1, scalaSpinner1-page1, ScalaSpinner0-page2, scalaSpinner1-page2
int scala_learn; // viene suonato semre quello che c'è in questa variavie, ma in scala fissa viene caricato il contenuto da scale[4]
byte scala_array[6];
const byte scala_lenght = 6;

byte scala_counter;
byte scala_reset;



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////
