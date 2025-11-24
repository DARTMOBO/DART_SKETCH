 /*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */
 
void noteOn(byte cmd, byte pitch, byte velocity, byte filter) {
  #if (MIDI_OUT_block == 0)  // blocco di tutto il midi out
  
if (filter ==1)
{  if (cmd != old_cmd || pitch != old_pitch || velocity != old_velocity){ // il messaggio midi inviato non deve essere uguale al precedente

midiOut(cmd,pitch,velocity);
  
old_cmd = cmd; 
old_pitch =  pitch ;  
old_velocity = velocity;

} }

else if (filter ==2)    // modalita utilizzata solo da encoders settati in endless 0-127
/*
 * mettendo filter su 2 si può fare in modo che il segnale in uscita di encoders non di alta qualità sia più pulito
 * (devo vedere 0 o 127 puliti che escono fuori a sconda della direzione di rotazione)
 * tale filtro è anche utilissimo per evitare segnali indesiderati sul top spinner, causati sa spostamenti microscopici a ruota ferma
 * 
 */
{

  if (cmd == old_cmd && pitch == old_pitch && velocity == old_velocity) // se il messaggio è uguale al precedente
                                                                         /*
                                                                          * allora aumenta out_filter
                                                                          * quando arriva oltre 100 torna a zero // soglia altissima
                                                                          * quando out_filter supera 2 siglifica che già tre messaggi sono stati uguali
                                                                          * allora il messaggi vengono inviati fuori
                                                                          */
  {
    if (out_filter < 100) out_filter++;
  // if (out_filter > 0)  Serial.println(out_filter);
  } else out_filter = 0;

if (out_filter > 2 || pitch != old_pitch
) { midiOut(cmd,pitch,velocity);}
//if ( cmd != old_cmd) 
//Serial.println(cmd);
  
old_cmd = cmd; 
old_pitch =  pitch ;  
old_velocity = velocity; 
                             }

  
else {midiOut(cmd,pitch,velocity); // se filter = 0 allora mandiamo fuori normalmente il messaggio midi
//old_cmd = cmd; 
//old_pitch =  pitch ;  
//old_velocity = velocity;

}
#endif
}

void midiOut(byte cmd, byte pitch, byte velocity) {
   #if (MIDI_OUT_block == 0)

// velocity = constrain(velocity,0,127)
  #if defined (__AVR_ATmega32U4__)  
 #if (stratos == 0 )
 Serial1.write(cmd+shifter_modifier_);
  Serial1.write(pitch);
  Serial1.write(velocity);
#endif

  midiEventPacket_t event = {((cmd+shifter_modifier_-144)/16)+9, cmd+shifter_modifier_, pitch, velocity};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
  #endif

   #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
 Serial.write(cmd+shifter_modifier_);
  Serial.write(pitch);
  Serial.write(velocity);
  #endif
#endif
}

void button (byte cmd, byte pitch, byte velocity, byte filterr)
{
  #if (MIDI_OUT_block == 0)
  
switch ((cmd-144)  /16)
{
  #if (note_off == 1)
  case -1 :  noteOn(cmd,pitch,velocity,filterr); break; // note
  #endif
   case 0 :  noteOn(cmd,pitch,velocity,filterr); break; // note
 case 1 :    noteOn(cmd,pitch,velocity,filterr); break; // poly AT
 case 2 :    noteOn(cmd,pitch,velocity,filterr); break; // cc
 case 3 :    noteOn(cmd,velocity,0,filterr); // pc
 case 4 :    noteOn(cmd,velocity,0,filterr);  break; // channel AT
 case 5 :  { noteOn(cmd,velocity,velocity,filterr); }
}
#endif
}


void outnucleo (byte onoff, byte chan_)
{
  
switch (onoff)
{
 case 1:                                                                      // se premuto
 if (eeprom_preset_active == 1)
 {
 if (qwertyvalue[chan_] == 0   )                                              // se non è specificato nessun tasto qwerty
 {button(typetable[chan_+page],valuetable[chan_+page],maxvalue[chan_],1);
  #if (DMX_active == 1  && stratos == 0)
 DmxSimple.write(dmxtable[chan_], maxvalue[chan_]*2);
 #endif
 }}
 else button(typetable[chan_+page],valuetable[chan_+page],maxvalue[chan_],1); // se siamo in modalità autodetect
 
 
     #if defined (__AVR_ATmega32U4__)  
      if (eeprom_preset_active == 1) // se esiste un preset in memoria
      qwerty_out(1,qwertyvalue[chan_],0);  
  #endif
     break;
     
#if (note_off == 0)   // ci sono due case = 0 dipende dal define NOTE_OFF
 case 0: 
   if (eeprom_preset_active == 1) {    // normale funz
   if (qwertyvalue[chan_] == 0 ) 
   {
    byte off_velocity = (modetable[chan_ + page] == 27) ? 0 : minvalue[chan_];
    button(typetable[chan_+page],valuetable[chan_+page],off_velocity,1);
    #if (DMX_active == 1  && stratos == 0)
   DmxSimple.write(dmxtable[chan_], minvalue[chan_]*2);
   #endif
   }}
   else button(typetable[chan_+page],valuetable[chan_+page],minvalue[chan_],1);   // autodetect
   
     
      #if defined (__AVR_ATmega32U4__)  
      if (eeprom_preset_active == 1)
      qwerty_out(0,qwertyvalue[chan_],0);
     
    #endif
     break;
#endif

     #if (note_off == 1)
case 0: 
byte note_off_;
if (valuetable[chan_+page] < 160) note_off_ = 16;

if (eeprom_preset_active == 1) {
   if (qwertyvalue[chan_] == 0 ) 
   { 
     // byte off_velocity = (modetable[chan_ + page] == 27) ? 0 : minvalue[chan_];
    button(typetable[chan_+page]-note_off_,valuetable[chan_+page],0,1);
    #if (DMX_active == 1  && stratos == 0)
   DmxSimple.write(dmxtable[chan_], minvalue[chan_]*2);
   #endif
   }}
   else button(typetable[chan_+page]-note_off_,valuetable[chan_+page],0,1);
   
     
      #if defined (__AVR_ATmega32U4__)  
      if (eeprom_preset_active == 1)
      qwerty_out(0,qwertyvalue[chan_],0);
     
    #endif
     break;
     #endif
  }
  
  }

  void qwerty_out(byte onoff, byte chan_, byte add)
  {
    switch (onoff)
{
 case 1:   
  {
      #if defined (__AVR_ATmega32U4__)  
   #if (hid_keys == 1)  
   if (chan_ < 25 && chan_ > 0) Keyboard.press(pgm_read_byte(qwertymod+chan_+add));
  else if (chan_ > 31 ) Keyboard.press(chan_+add); // normale tabella ascii // 
  else if (chan_ != 31 ) Mouse.press(chan_-24+add); // 25 26 27 28 29 30 - 
   #endif
      #endif
      }
 break;
 case 0:
 {
       #if defined (__AVR_ATmega32U4__)  
       //  else if (chan_ < 25 ) Keyboard.release(qwertymod[chan_]); 
    #if (hid_keys == 1)    
    if (chan_ < 25 && chan_ > 0 ) Keyboard.release(pgm_read_byte(qwertymod+chan_+add));
    else if (chan_ > 31 ) Keyboard.release(chan_+add);
 else  if (chan_ != 31 ) Mouse.release(chan_-24+add); 
  #endif
    
    #endif  
      }
 break;
}
    }
