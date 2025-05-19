
///////////////////////////
// DART_SKETCH   v1.87   // 
// Massimiliano Marchese //
// Piero Pappalardo      //
// www.dartmobo.com      //
///////////////////////////

#define shifter_active    0          // 1 = enabled // 0 = disabled // SHIFT REGISTERS_ // if enabled, Matrix_pads must be disabled
#define stratos  0                   // 1 = enabled // 0 = disabled // Stratos sketch version.
#define LED_pattern 6                // 0 = dart one // 1 = kombat // 2 = NB-boards // 3 = Kombat-NB // 4 kombat-NB2 - // 5 KOMBAT-NB3 // 6 kombat personal - Led animation pattern used by buttons and pots
#define capacitivesensor_active 2    // 1 = enabled // 0 = disabled // 2 = EXTERNAL TOUCH IC on pin 7 - 9 // 3 = EXTERNAL TOUCH IC on pin 7 - 8 // 4 = EXTERNAL TOUCH IC (inverted) on pin 7 - 9--- CAPACITIVE SENSORS_
#define touch_version 1              // 1 = 680k //  2 = 10m //     resistor settings for touch sensing circuit
#define touch_led_onboard 1          // 1 = led output  on arduino pin 8 // 0 = disabled
#define DMX_active    0              // 1 = enabled // 0 = disabled // enable-disable also from _DART_Dmx_out.cpp !!!!!!!!!
<<<<<<< HEAD
#define Matrix_Pads 2                // 1 = enabled // 2 = pads on 17-32 circuitposition (old)// 0 = disabled // max7219 chips
=======
#define Matrix_Pads 1                // 1 = enabled // 2 = pads on 17-32 circuitposition (old)// 0 = disabled // max7219 chips
>>>>>>> origin/master
#define hid_keys 1                   // 1 = enabled
#define hid_mouse 1                  // 1 = enabled
#define top_spinner 1                // 1 = enabled // 0 = disabled // TOP SPINNER
#define side_spinner 1               // 1 = enabled // 0 = disabled // SIDE SPINNER
#define note_off 0                   // 1 = enabled // 0 = disabled // send NOTE-OFF messages on button release -  if NOTE Type has been selected
#define pullups_active 1             // 1 = enabled // 0 = disabled // pullup resistors
#define page_active 1                // 1 = enabled // 0 = disabled //  page_switch
#define page_LEDs 0                  // 1 = page LEDs active
#define MIDI_thru 0                  // 1 = MIDI Thru active
#define mouse_block 1                // 1 = enabled // 0 = disabled // mouse messages are stopped after 2 seconds of repeated activity
#define arrows_block 0               // 1 = enabled // 0 = disabled // arrow key messages are stopped after 2 seconds of repeated activity
#define encoders_generic 1           // 1 = enabled 
#define MIDI_IN_block 0              // 1 = MIDI IN blocked
#define MIDI_OUT_block 0             // 1 = MIDI out blocked
#define fader_type 0                 // 0 = normal // 1 =  scratch style fader - experimental - leave it to 0
#define LED_rings 0                  // 1 = LED rings active - experimental - leave it to 0
#define blinker 1
//---------------------------------------------


#if defined (__AVR_ATmega32U4__)  
#include "_DART_MIDI.h"
#include <Mouse.h>
#include <Keyboard.h>
midiEventPacket_t rx;
#endif

#if (capacitivesensor_active == 1)
// pin 9 e 7 sono gli input - 8 è l'emettitore
#include "_DART_Touch_Sensor.h"
#if (stratos == 1 )
CapacitiveSensor   cs_4_2[1] = {CapacitiveSensor(9,8)}; // stratos
#endif
#if (stratos == 0 )
CapacitiveSensor   cs_4_2[2] = {CapacitiveSensor(8,7),CapacitiveSensor(8,9)};
#endif
 
#endif


#if (DMX_active == 1 && stratos == 0)
#include "_DART_DMX_Out.h"
#endif

#include "_DART_EEPROM.h"

#if (shifter_active == 1 && stratos == 0)
#include "_DART_Shifter.h"
#endif

 #if (Matrix_Pads == 1 && stratos == 0)
 #include "LedControl.h"
 #endif

  #if (Matrix_Pads == 2 && stratos == 0)
 #include "LedControl.h"
 #endif


//#if (stratos == 1)
byte out_filter; // usato in void noteon come filtro antiflicker per gli encoders di scarsa qualità
//#endif
byte shifter_modifier_; // usato in midiout per shiftare i segnali su altro canale ottenendo un effetto simile a PAGE 

  byte do_; // attualmente usato come counter per flash lights in modalità matrix durante caricamento preset
///////////////////////////////////////////////////////////////////////
const byte max_modifiers = 60;
const int maxbeam = 520;
const int minbeam = 290;

///////////////////////////////////////////////////////////////////////
#if (LED_pattern == 0) 
   const byte encledtable[16]= { 4,5,6,7, 12,13,14,15, 3,2,1,0, 11,10,9,8,};  // dart one 
 #endif
 
 // const byte encledtable[16]= { 4,5,6,7, 12,13,14,15, 3,2,1,16, 11,10,9,8,};  // dart one - darietto
  
 
//  const byte encledtable[16] = { 15,15,14,14,13,13,12,12,11,11,10,10,9,9,8,8,};  // vector

 // const byte encledtable[16] = { 8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15};  // vector inverted

 #if (LED_pattern == 1) 
  const byte encledtable[16] = { 12,13,14,14,15,0,7,7,1,6,2,2,5,3,4,4};  // kombat
#endif

/*
#if (LED_pattern == 3) 
  const byte encledtable[16] = { 1,4,3,3,2,8,7,7,6,5,12,12,11,10,9,9};  // kombat - nb // 1,4,3,2, 8,7,6,5, 12 ,11,10,9
#endif
*/

#if (LED_pattern == 3) 
 // const byte encledtable[16] = {0,3,2,2,1,7,6,6,5,4,11,11,10,9,8,8};  // kombat - nb // 
    const byte encledtable[16] = {8,8,9,10,11,11,4,5,6,6,7,1,2,2,3,0};
#endif

#if (LED_pattern == 4) 
 // const byte encledtable[16] = {0,3,2,2,1,7,6,6,5,4,11,11,10,9,8,8};  // kombat - nb2 // 
    const byte encledtable[16] = {8,8,10,11,9,9,4,5,6,6,7,1,2,2,3,0};
#endif

#if (LED_pattern == 5) 
 // const byte encledtable[16] = {0,3,2,2,1,7,6,6,5,4,11,11,10,9,8,8};  // kombat - nb3 // 
    const byte encledtable[16] = {11,11,9,8,10,10,4,5,6,6,7,1,2,2,3,0}; // 11 9 8 10
#endif

#if (LED_pattern == 6) 
 // const byte encledtable[16] = {0,3,2,2,1,7,6,6,5,4,11,11,10,9,8,8};  // kombat - nb3 // 
   //   const byte encledtable[16] = {11,11,9,9,8,8,10,10,0,0,1,1,2,2,3,3}; // 11 9 8 10
    const byte encledtable[16] = {11,11,10,10,9,9,8,8,3,3,2,2,1,1,0,0}; // 11 9 8 10
#endif

 #if (LED_pattern == 2) 
   const byte encledtable[16]= { 
   15,14,13,12,
  7,6,5,4,
   8,9,10,11,
   0,1,2,3
   };  // NB-boards
#endif

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
{return  (( pgm_read_byte(input_remap + ((input)-(((input)/8)*8)) ) +(((input)/8)*8))) -1;}


//|||||||||||||||||||||||||||||||||
// const byte select[] = {4,5,6}; // pins connected to the 4051 input select lines

//|||||||||||||||||||||||||||||||||
byte  eeprom_preset_active ; // activate auxiliary preset at startup;
byte channel;
byte plexer;
byte chan;

#if ( fader_type == 0)
const int upper_val = 750;
const byte lower_val = 255;
#endif

#if ( fader_type == 1)
const int upper_val = 1024;
const byte lower_val = 0;
#endif

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
volatile byte encoder_mempos[2];             // da editor si scegli quale sara'  la memoryposition dell'encoder 
                                             
volatile byte touch_mempos[2];
byte V_touch_regulator[2] = {1,1};
byte mouse_mempos ;
byte PADS_mempos;
byte distance_mempos;
byte page_mempos;
 byte general_mempos = 0;
/////////////////////////////////////////////////////////////////////
byte page = max_modifiers ;

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
#endif

#if ( stratos == 0)
byte decaysensor[2]; //= decaysensor_value;
#endif

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
#endif 

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
 * l'ultima matrice non si accendeva perchè ho messo delle resistenze in serie du load e clock
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
 * allo stato attuale mi rimane da capire perchè le ultime 4 matrici sono sempre leggermente più luminose
 * e perchè l'aggiornamento degli sprite funziona bene col midi feedback ma non premendo sui tasti... - adesso sembra superato anche questo problema.
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
 * bisogno chiarire esattamente come vengono associati gli sprite ai pulsanti, erchè nn mi ricordo.
 * 
 * 
 * 
 * 
 */
byte max_units = 12;

LedControl lc=LedControl(10,11,12,max_units); 
// LedControl(int dataPin, int clkPin, int csPin, int numDevices) 

//byte order_line[8] = {5,6,0,1,3,4,7,2};
 // const byte order_row[8] = {6,1,5,3,7,2,4,0};

 void set_unit(byte number_of_unit){ 
  lc.shutdown(number_of_unit,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(number_of_unit,5); // luminosita' da 1 a 15
  /* and clear the display */
  lc.clearDisplay(number_of_unit); 
   }
   
#endif

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
volatile byte lastbutton[64] ; // used to record the previous state of a button - debounce
#if ( stratos == 0)
byte lastbutton_debounce = 5;
#endif

#if ( stratos == 1)
byte lastbutton_debounce = 40;
#endif

volatile byte lightable[65] // ho provato a scendere a 64 - ma si creavano stranissimi problemi di conflitto memoria con buttonled_efx - messaggi midi disordinati etc etc
               //la vecchia lightable era fissa.

 = { //40,
1,40,3,40,2,40,0,40,
5,40,7,40,6,40,4,40,
9,40,11,40,10,40,8,40,
13,40,15,40,14,40,12,40,
17,40,19,40,18,40,16,40,
21,40,23,40,22,40,20,40,
};





 byte  valuetable[max_modifiers*2] ;
 byte  typetable[max_modifiers*2] ;

/*
qualche nota per ricordare a qule cifra typetable corrispondono i vari messaggi midi
questi sono i valori effettivi usati per mandare fuori midi con serial out
soltanto nel trasferimento dei preset da editor a Dart di utilizzano delle conversioni per rimanere nel range 0-127

Note Off          128 + Channel 0-127 Pitch 0-127 Velocity
Note On           144 + Channel 0-127 Pitch 0-127 Velocity
Poly Pressure     160 + Channel 0-127 Pitch 0-127 Pressure
Control Change    176 + Channel 0 Undefined 0-127 MSB
Program Change    192 + Channel 0-127 Program Not used
Channel Pressure  208 + Channel 0-127 Pressure  Not used
*/
 
 byte  minvalue[max_modifiers] ;
 byte  maxvalue[max_modifiers] ;
 byte  modetable[max_modifiers] ; // 
 byte  qwertyvalue[max_modifiers];
 byte  dmxtable[max_modifiers];

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
volatile byte bit_status[4][max_modifiers/4]; // bit_status e' un multiarray che contiene le vecchie tabelle di riferimento per far funzionare il toggle, 
                                              // il feedback e i led
     
 // 1 - ledstatus 1 e 2
 // 2 - feedback_bit1
 // 3 - feedback_bit2
 // 4 - bit_toggle 1 e 2

 byte feedop2 (byte input) { // used in midi feedback bit operations
 return input/8; }

 byte bit_write(byte array_, byte posizione, byte stato)
 {  bitWrite(bit_status[array_-1][feedop2(posizione)],posizione-(feedop2(posizione)*8),stato); }

  byte bit_read( byte array_,byte posizione)
  { return bitRead(bit_status[array_-1][feedop2(posizione)],posizione-(feedop2(posizione)*8));  }
 //  byte scrivi_bool (boolean numero) {  Serial.print(numero);  }
  
///////////////////////////////////////////////////////////////////////////
boolean pagestate = 0;
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



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////7
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////
