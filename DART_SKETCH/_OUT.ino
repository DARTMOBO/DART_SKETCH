void noteOn(byte cmd, byte pitch, byte velocity, byte filter) {
  #if (MIDI_OUT_block == 0)
if (filter ==1){  if (cmd != old_cmd || pitch != old_pitch || velocity != old_velocity){

midiOut(cmd,pitch,velocity);
  
old_cmd = cmd; 
old_pitch =  pitch ;  
old_velocity = velocity;

} }
else {midiOut(cmd,pitch,velocity);
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
 Serial1.write(cmd);
  Serial1.write(pitch);
  Serial1.write(velocity);
#endif

  midiEventPacket_t event = {((cmd-144)/16)+9, cmd, pitch, velocity};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
  #endif

   #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
 Serial.write(cmd);
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
 case 0 :   noteOn(cmd,pitch,velocity,filterr); break; // note
 case 1 :   noteOn(cmd,pitch,velocity,filterr); break; // poly AT
 case 2 :   noteOn(cmd,pitch,velocity,filterr); break; // cc
 case 3 :   noteOn(cmd,velocity,0,filterr); // pc
 case 4 :   noteOn(cmd,velocity,0,filterr);  break; // channel AT
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
   /*   {
    if (qwertyvalue[chan_] < 25 && qwertyvalue[chan_] > 0) Keyboard.press(pgm_read_byte(qwertymod+qwertyvalue[chan_]));
  else if (qwertyvalue[chan_] > 31 ) Keyboard.press(qwertyvalue[chan_]); // normale tabella ascii // 
  else if (qwertyvalue[chan_] != 31 ) Mouse.press(qwertyvalue[chan_]-24); // 25 26 27 28 29 30 - 
      }*/
  #endif
     break;
     
 case 0: 
   if (eeprom_preset_active == 1) {
   if (qwertyvalue[chan_] == 0 ) 
   { button(typetable[chan_+page],valuetable[chan_+page],minvalue[chan_],1);
    #if (DMX_active == 1  && stratos == 0)
   DmxSimple.write(dmxtable[chan_], minvalue[chan_]*2);
   #endif
   }}
   else button(typetable[chan_+page],valuetable[chan_+page],minvalue[chan_],1);
   
     
      #if defined (__AVR_ATmega32U4__)  
      if (eeprom_preset_active == 1)
      qwerty_out(0,qwertyvalue[chan_],0);
     /* {
    //  else if (qwertyvalue[chan_] < 25 ) Keyboard.release(qwertymod[qwertyvalue[chan_]]); 
       if (qwertyvalue[chan_] < 25 && qwertyvalue[chan_] > 0 ) Keyboard.release(pgm_read_byte(qwertymod+qwertyvalue[chan_]));
   else if (qwertyvalue[chan_] > 31 ) Keyboard.release(qwertyvalue[chan_]);
    else  if (qwertyvalue[chan_] != 31 ) Mouse.release(qwertyvalue[chan_]-24);   
      }
      */
    #endif
     break;
  }
  
  }

  void qwerty_out(byte onoff, byte chan_, byte add)
  {
    switch (onoff)
{
 case 1:   
  {
    if (chan_ < 25 && chan_ > 0) Keyboard.press(pgm_read_byte(qwertymod+chan_+add));
  else if (chan_ > 31 ) Keyboard.press(chan_+add); // normale tabella ascii // 
  else if (chan_ != 31 ) Mouse.press(chan_-24+add); // 25 26 27 28 29 30 - 
      }
 break;
 case 0:
 {
    //  else if (chan_ < 25 ) Keyboard.release(qwertymod[chan_]); 
       if (chan_ < 25 && chan_ > 0 ) Keyboard.release(pgm_read_byte(qwertymod+chan_+add));
   else if (chan_ > 31 ) Keyboard.release(chan_+add);
    else  if (chan_ != 31 ) Mouse.release(chan_-24+add);   
      }
 break;
}
    }
