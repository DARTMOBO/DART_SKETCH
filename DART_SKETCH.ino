

////////////////////////////
//
// DART Sketch - 
// Massimiliano Marchese
// Piero pappalardo
// www.dartmobo.com
//
////////////////////////////

#define touch_version 1 // 1 = 680k - 2 = 10m
#define main_encoder 1 // disabilita l'encoder principale
#define capacitivesensor_active  1 // 1 = capacitivesensor attivato
#define shifter_active  1
#define DMX_active  1 // 1 = attiva // 0 == disattiva // disattivare anche da dmxsimple .cpp per liberare tutta la memoria
#define pullups_active 0 // 
#define stratos 0 // 

#if defined (__AVR_ATmega32U4__)  
  #include "MIDIUSB.h"
 #include <Mouse.h>
  #include <Keyboard.h>

 midiEventPacket_t rx;
#endif

 #if (capacitivesensor_active == 1)
 #include "CapacitiveSensor.h"

#if (stratos == 1 )
CapacitiveSensor   cs_4_2 = CapacitiveSensor(8,9); // stratos

#endif

#if (stratos == 0 )
  CapacitiveSensor   cs_4_2 = CapacitiveSensor(8,7);
  CapacitiveSensor   cs_4_3 = CapacitiveSensor(8,9);  // one - stratos
 #endif
 
#endif



 // #if defined DMX_active 
 #if (DMX_active == 1 && stratos == 0)

#include "DmxSimple.h"
#endif


#include "EEPROM.h"

#if (shifter_active == 1 && stratos == 0)
#include "Shifter.h"
#endif


  byte do_; // serve per diminuire la velocità i lettura encoder, daltando tot cicli.
///////////////////////////////////////////////////////////////////////
const byte max_modifiers = 60;
const int maxbeam = 520;
const int minbeam = 290;

///////////////////////////////////////////////////////////////////////

   const byte encledtable[16]= { 4,5,6,7, 12,13,14,15, 3,2,1,0, 11,10,9,8,};  // dart one 
 
 // const byte encledtable[16]= { 4,5,6,7, 12,13,14,15, 3,2,1,16, 11,10,9,8,};  // dart one - darietto
  
// const byte *const encledtable_MEM[16] PROGMEM = { 4,5,6,7, 12,13,14,15, 3,2,1,0, 11,10,9,8,};  // dart one


 
//  const byte encledtable[16] = { 15,15,14,14,13,13,12,12,11,11,10,10,9,9,8,8,};  // vector

 // const byte encledtable[16] = { 8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15};  // vector inverted


// const byte encledtable[16] = { 12,13,14,14,15,0,7,7,1,6,2,2,5,3,4,4};  // kombat

//const byte  encledtable2[16]  = {11,10,9,8,60,60,60,60,8,9,10,11,0,1,2,3,};

//const byte  encledtable3[16]  = {32,32,32,32,4,5,6,7,12,13,14,15,32,32,32,32};

 //  const byte qwertymod[2];


 const byte *const qwertymod[33] PROGMEM = { // da 0 a 24 ci sono i modificatori - da 25 a 32 controlli mouse
   
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
 
 178, // backspace
 176, // return
 177, // esc
 212, // delete
 };



// const byte input_remap[] = {6,8,4,2,7,1,5,3,};
// byte remapper(byte input) {return  ((input_remap[((input)-(((input)/8)*8))]+(((input)/8)*8))) -1;}



 const byte *const input_remap[] PROGMEM = {6,8,4,2,7,1,5,3,};
byte remapper(byte input) 
{return  (( pgm_read_byte(input_remap + ((input)-(((input)/8)*8)) ) +(((input)/8)*8))) -1;}


//|||||||||||||||||||||||||||||||||
// const byte select[] = {4,5,6}; // pins connected to the 4051 input select lines

//|||||||||||||||||||||||||||||||||
byte  eeprom_preset_active ; // activate auxiliary preset at startup;
byte channel;
byte plexer;
byte chan;
const int upper_val = 750;
const byte lower_val = 255;
int valore; // analog 0-1024 value for 4051 analog readings.
volatile byte potOut;
//||||||||||||||||||||||||||||||
byte mousex;
byte mousey;
// byte mouserall;
//int mouseyacceleration; // 
//int mousexacceleration; // 
byte mouse_wheel_speed_counter;
/////////////////////////////////////////////////////////////
volatile byte encoder_mempos[2];             // da editor si scegli quale sarÃƒÆ’Ã‚Â  la memoryposition dell'encoder - la cosa rende piÃƒÆ’Ã‚Â¹ semplice creare un nuovo layout editor - masta mettere mode su encoder1
volatile byte touch_mempos[2];
byte V_touch_regulator[2] = {1,1};
byte mouse_mempos = 58;
byte PADS_mempos;
byte distance_mempos;
byte page_mempos;
 byte general_mempos = 0;
/////////////////////////////////////////////////////////////////////
byte page = max_modifiers ;

byte cycletimer;   /// conteggio di tempo in base ai cicli - serve per regolare effetti visivi led - e per il timing del virtualtouch

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

volatile byte  readingsXen[3] ;      // the readings from the analog input
// byte readingsXenn = 250;
volatile byte readingsXen2[3];
// volatile byte readingsXenn2;
//const byte Xenlimit = 6;
// const byte Xendivider = 2;
// volatile byte lettura;

#if (stratos == 0 )
const byte decaysensor_value = 2;
#endif
#if (stratos == 1 )
const byte decaysensor_value = 100;
#endif
byte decaysensor[2]; //= decaysensor_value;
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
 int encled; //  used for encoder led animation

//////////////////////////////////////////////////////////////////////////////////////
#if (shifter_active == 1 && stratos == 0)
#define SER_Pin 10 //SER_IN
#define RCLK_Pin 11 //L_CLOCK
#define SRCLK_Pin 12 //CLOCK
#define NUM_REGISTERS 4 // how many registers are in the chain
Shifter shifter(SER_Pin, RCLK_Pin, SRCLK_Pin, NUM_REGISTERS);
#endif 

//////////////////////////////////////////////////////////////////////////////////////
//byte contoencoder;
byte numero2;
byte lastEncoded[2] ; // adesso queste variabili contendono la scala scritta da editor...
int encodervaluepot[2] ; 
byte encodervaluepot_buffer[2]; // serve per spegnere le note giÃƒÂ  suonate in encoder - scale mode
volatile byte MSB[2] ; 
volatile byte LSB[2] ;
byte encoder_block[2]= {64,64} ; // serve per bloccare l'attivitÃƒÂ  dell'encoder quando viene toccato ma tenuto fermo - per registrare una scala.
////////////////////////////////////////////////////////////////////////////////  
volatile byte lastbutton[64] ; // used to record the previous state of a button - debounce
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



volatile byte bit_status[4][max_modifiers/4]; // bit_status ÃƒÆ’Ã‚Â¨ un multiarray che contiene le vecchie tabelle di riferimento per far funzionare il toggle, il feedback e i led

 // 1 - ledstatus 1 e 2
 // 2 - feedback_bit1
 // 3 - feedback_bit2
 // 4 - bit_toggle 1 e 2


 byte  valuetable[max_modifiers*2] ;
 byte  typetable[max_modifiers*2] ;
 byte  minvalue[max_modifiers] ;
 byte  maxvalue[max_modifiers] ;
 byte  modetable[max_modifiers] ; // 
 byte  qwertyvalue[max_modifiers];
 byte  dmxtable[max_modifiers];

 byte feedop2 (byte input) { // used in midi feedback bit operations
 return input/8; }

 byte bit_write(byte array_, byte posizione, byte stato)
 {  bitWrite(bit_status[array_-1][feedop2(posizione)],posizione-(feedop2(posizione)*8),stato); }

  byte bit_read( byte array_,byte posizione)
  { return bitRead(bit_status[array_-1][feedop2(posizione)],posizione-(feedop2(posizione)*8));  }
 //  byte scrivi_bool (boolean numero) {  Serial.print(numero);  }
  
///////////////////////////////////////////////////////////////////////////
byte pagestate = 0;
/////////////////////////////////////////////////////////////////////////// 
// ------------------------------------------------------------- scale play
// void scale_play
byte at; // serve per lo scale player
byte beam_scala_buffer;
byte beam_counter;
int scala[4]; // scala[4] è un array che cotiene 4 scale... 
              // scalaSpinner0-page1, scalaSpinner1-page1, ScalaSpinner0-page2, scalaSpinner1-page2
int scala_learn; // viene suonato semre quello che c'è in questa variavie, ma in scala fissa viene caricato il contenuto da scale[4]
byte scala_array[6];
const byte scala_lenght = 6;

byte scala_counter;
byte scala_reset;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void loop () {

   

if (cycletimer < 250 ) cycletimer++;

 
    #if defined (__AVR_ATmega32U4__)               // USB MIDI in
  do {                                         
    rx = MidiUSB.read();
    if (rx.header != 0) {
 incomingByte  = rx.byte1; midifeedback();
 incomingByte  = rx.byte2; midifeedback();
 incomingByte  = rx.byte3; midifeedback();
    }
  } while (rx.header != 0);
 
   #if ( stratos == 0) 
                                                  // DIN MIDI in
  if (Serial1.available() > 0) { 
  incomingByte = Serial1.read();
  midifeedback(); 
  } 
  else 
  #endif
  
#endif





  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
 if (Serial.available() > 0) { 
  incomingByte = Serial.read();
  midifeedback();  } 
  else 
  #endif

      
  if (openeditor != 1)   
   
  {

    
  
 
   #if (capacitivesensor_active > 0)
     touch_sensors();
    #endif

  #if (stratos == 1)  
  AIN_stratos();
   #endif
  #if (stratos == 0)  
  AIN();
   #endif


    #if (shifter_active == 1 && stratos == 0)
  if (qwertyvalue[general_mempos] > 2)    buttonledefx();  
    #endif
 
 
 
  
      
  pageswitch();
    
   
      
     #if (shifter_active == 1 && stratos == 0)
     if (shifterwrite ==1)  {shifter.write(); shifterwrite=0;}
     #endif
 
  }

}



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////7
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
void test1() // startup test code
{
 Serial.println(valore/32); //
 Serial.println(mouse_mempos);
 Serial.println("   ");
 // delay (1);

}
*/




/*
void test2() // touchsensors test code
{


}

void test3() // on button pressure test code
{



 // Serial.println("-----   ");
  
  
//Serial.print("-encoder_mempos-");Serial.println(encoder_mempos[0]);
//Serial.print("eeprom-dmx-1--");Serial.println(EEPROM.read(58+192));
//Serial.print("eeprom-dmx-2--");Serial.println(EEPROM.read(58+192+512));

}
*/
