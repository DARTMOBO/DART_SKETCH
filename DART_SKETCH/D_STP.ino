/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */

void setup() {
  { // setup per matrix pads
  }

  { // DMX setup
#if (DMX_active == 1)
    DmxSimple.usePin(13);
    DmxSimple.maxChannel(64);
    #endif  
  }

  
  { // USB-MIDI init (SAMD / Metro M0)
    #if defined(ARDUINO_ARCH_SAMD)
      DART_MIDI_Init_SAMD();
    #endif
  }

  { // setup 4051 chips
    for (byte bit = 4; bit < 7; bit++) {
      pinMode(bit, OUTPUT); // set the three select pins to output // 4051
    }
  }


   #if (ENABLE_POT_TAKEOVER == 1)
  for (byte bit = 0; bit < 128; bit++) {
    data_LB[bit] = 1; // ?? 
  }
#else
  for (byte bit = 0; bit < 64; bit++) {
    data_LB[bit] = 1; // ?? 
    }
#endif // ENABLE_POT_TAKEOVER


  { // encoder e midi-DIN inputs - su dartmobo
    pinMode(2, INPUT); // ------------ encoder input settings
    pinMode(3, INPUT);
    // pinMode(12, OUTPUT);

    #if defined(ARDUINO_ARCH_SAMD)
      // SAMD: la pullup si abilita con INPUT_PULLUP
      pinMode(2, INPUT_PULLUP);
      pinMode(3, INPUT_PULLUP);
#else
      // AVR: pullup storica via digitalWrite(HIGH) su pin in INPUT
      digitalWrite(2, HIGH); // turn pullup resistor on // encoder
      digitalWrite(3, HIGH); // turn pullup resistor on // encoder
    #endif
    
    pinMode(0, INPUT); // ------------ midi over DIN settings
    #if defined(ARDUINO_ARCH_SAMD)
      pinMode(0, INPUT_PULLUP);
#else
      digitalWrite(0, HIGH);
    #endif
  }

  { // gestione pullups
    #if (pullups_active == 1)
#if defined (__AVR_ATmega32U4__) || defined(ARDUINO_ARCH_SAMD) // gestione pullups
      { 
        { 
          #if defined(ARDUINO_ARCH_SAMD)
            // SAMD (Metro M0): abilita pullup sui 6 pin ADC reali usati dai 4051
            for (byte i = 0; i < 6; i++) {
              pinMode(DART_ADC_PIN_FROM_PLEXER(i), INPUT_PULLUP);
            }
#else
            // AVR (Leonardo): pullup storica sui canali A0..A5 (numeri 18..23)
            digitalWrite(18, HIGH); // analog in 0
            digitalWrite(19, HIGH); // analog in 1
            digitalWrite(20, HIGH); // analog in 2
            digitalWrite(21, HIGH); // analog in 3
            digitalWrite(22, HIGH); // analog in 4
            digitalWrite(23, HIGH); // analog in 5
          #endif
        }

        
      }
      #endif // defined (__AVR_ATmega32U4__)
      
      #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) // gestione pullups
      {
        digitalWrite(14, HIGH);
        digitalWrite(15, HIGH);
        digitalWrite(16, HIGH);
        digitalWrite(17, HIGH);
    
        digitalWrite(18, HIGH);
        digitalWrite(19, HIGH);
       
      }
      #endif // defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
    #endif // (pullups_active == 1)
  }

  { // serial begin
    #if defined (__AVR_ATmega32U4__)
      Serial1.begin(31250);  
    #endif // defined (__AVR_ATmega32U4__)
    #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
    Serial.begin(31250); // midi specific baud rate
    #endif // defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
  }
 
  // ============================================================
  // CTRL-F: EEPROM_EMU_BEGIN_SAMD
  // EEPROM emulata (SAMD): carica la mirror RAM dalla flash PRIMA di leggere il preset.
  // Su AVR non esiste EEPROM.begin(), quindi chiamata SOLO su SAMD.
  // ============================================================
  #if (ENABLE_EEPROM == 1) && defined(ARDUINO_ARCH_SAMD)
    EEPROM.begin();
  #endif

  load_preset_base();
  load_preset(0);

  #if (Page_switch == 1)
  {
    if (eeprom_preset_active == 1 && (control_present & CONTROL_PAGE)) { // 1 = c'è un preset nella eeprom // eeprom_preset_active viene settato da setup_mempos 
                                                        // che a sua volta è richiamata da load_preset
      setPlexer((page_mempos) - ((page_mempos / 8) * 8)); 
    
     // data_LB[page_mempos] = map32(analogRead_1024((page_mempos / 8)), 0, 1024, 0, 2); // read page switch state. // se valore alto (valore alto è normale, se non viene portato in basso dal cortocircuito di un pulsante)   
    
     // In setup usiamo analogRead classico per evitare dipendenze da fastADC_init.
     data_LB[page_mempos] = map32(analogRead((page_mempos / 8)), 0, 1024, 0, 2);         // read page switch state. // se valore alto (valore alto è normale, se non viene portato in basso dal cortocircuito di un pulsante) 


      if (data_LB[page_mempos] > 0) {
        page = 0; // pagestate=0; 
        pagestate = 0;
        load_preset(0); // ledrestore(); // levetta a destra caricamento preset eeprom pagina 1

        #if (Page_switch == 1)
        page_leds_(0);
        #endif // (Page_switch == 1)

        #if (Scale == 1)
        update_scala(1);  
        update_scala(0);   
        #endif // (Scale == 1)
      } else {
        page = max_modifiers; // 
        pagestate=1; 
        
        load_preset(1); // ledrestore2(); // levetta a sinistra preset 2

        #if (Page_switch == 1)
        page_leds_(1);
        #endif // (Page_switch == 1)

        #if (Scale == 1)
        update_scala(1);  
        update_scala(0);  
        #endif // (Scale == 1)
      } 
    } else {
      page = 0;
      // update_scala(1);  
      // update_scala(0); 
    }
    }
#else
    // Page_switch disattivo: ignora completamente l’eventuale lever nel preset.
    // Forza sempre PAGE 1.
    page = 0;
    // pagestate qui è irrilevante se Page_switch==0 (pageswitch() non gira),
    // ma lo lasciamo “tranquillo”.
    pagestate = 0;
    load_preset(0);  // opzionale ma coerente: riallinea sempre la pagina 1
  #endif // (Page_switch == 1)
 
  
//#if (ENABLE_POT_TAKEOVER == 1)
  //   takeover_init = 0;
  // #endif // (ENABLE_POT_TAKEOVER == 1)


// ============================================================
// CTRL-F: TAKEOVER_BOOT_FREE_STARTPAGE
// Boot-friendly: sulla pagina di AVVIO i pot devono essere subito "live".
// Disarmo takeover (bank5) SOLO per la pagina corrente.
 
// ============================================================
#if (ENABLE_POT_TAKEOVER == 1)
for (byte i = 0; i < max_modifiers; i++) {
  bit_write(5, i + 0, 0);              // Page1 NOT ARMED
  bit_write(5, i + max_modifiers, 0);  // Page2 NOT ARMED
  // opzionale "rassicurante":
  // data_LB[i] = 128;
  // data_LB[i + 64] = 128;  // SOLO se il tuo lb usa stride 64 (come in pots)
}
#endif
/*
    #if (ENABLE_POT_TAKEOVER == 1)
  for (byte bit = 0; bit < 128; bit++) {
    data_LB[bit] = 128; // ?? 
  }
    #endif // ENABLE_POT_TAKEOVER
*/


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (data_MA[general_mempos] == 0) { // se i pads sono attivi bisogna togliere la pullup
      #if defined (__AVR_ATmega32U4__) 
      digitalWrite(23, LOW);
      #endif // defined (__AVR_ATmega32U4__)

      #if defined(ARDUINO_ARCH_SAMD)
      // Su SAMD la pullup si gestisce via pinMode().
      // Qui disattiviamo la pullup sul 6o input analogico (equivalente ad A5).
      pinMode(A5, INPUT);
      #endif // defined(ARDUINO_ARCH_SAMD)

      #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__) 
      digitalWrite(19, LOW);
      #endif // defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
  }  

  if (data_VA[general_mempos] != 0) { // 0 = nomobo
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
    digitalWrite(10, LOW);
  }

  // [TOUCH_PULLUP_SETUP]
  // External touch inputs: optionally enable internal pullups (for buttons to GND / open-drain).
  // Applies ONLY when Touch_sensors_enable is 2 (pins 7&9) or 3 (pins 7&8).
  if (Touch_sensors_enable == 2 || Touch_sensors_enable == 3) {
    const byte tPin0 = 7;
    const byte tPin1 = (Touch_sensors_enable == 2) ? 9 : 8;

    if (touch_pullup) {
      pinMode(tPin0, INPUT_PULLUP);
      pinMode(tPin1, INPUT_PULLUP);
    } else {
      pinMode(tPin0, INPUT);
      pinMode(tPin1, INPUT);
    }
  }


  openeditor = 0;
  note = 255; // out of range (0-127) value
 
  if (data_DM[general_mempos] > 0) {

#if defined(ARDUINO_ARCH_SAMD)
    attachInterrupt(digitalPinToInterrupt(2), lettura_enc_principale, CHANGE);
attachInterrupt(digitalPinToInterrupt(3), lettura_enc_principale, CHANGE);
#else

    attachInterrupt(0, lettura_enc_principale, CHANGE); 
    attachInterrupt(1, lettura_enc_principale, CHANGE);
    #endif
    
  }

  #if (Fast_analogread == 1)
  fastADC_init();
  #endif // (Fast_analogread == 1)


  /*
  // ============================================================
  // CTRL-F: PAGESWITCH_DIAG_SNAPSHOT_END_SETUP (DISABLED)
  // Snapshot usato per diagnostica takeover/pageswitch al boot.
  // ============================================================
  ps_setup_page = page;
  ps_setup_pagestate = pagestate;
  */
}

// =========================================================================
// AVR-only fast ADC
// Su MCU non-AVR (es. SAMD / Metro M0) questi registri NON esistono.
// Qui mettiamo un fallback che compila e usa analogRead() standard.
// =========================================================================

#if defined(__AVR__)

// "Antidoto" alla fastADC_init: rimette l'ADC come si aspetta analogRead()
void standardADC_init() {
  // Riferimento AVcc, risultato right-adjust (ADLAR = 0)
  ADMUX = _BV(REFS0); // REFS0=1, REFS1=0, ADLAR=0, MUX[4:0]=0

  // ADC abilitato, prescaler = 128 (modalità lenta e precisa di Arduino)
  ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

  // Free-running e trigger speciali disattivati
  ADCSRB = 0;
}

// Chiamala in setup()
void fastADC_init() {
  // Riferimento: AVcc, risultato left-adjust (ADLAR=1 → leggiamo solo ADCH)
  ADMUX = _BV(REFS0) | _BV(ADLAR);

  // Prescaler = 32 → ADPS2=1, ADPS1=0, ADPS0=1
  ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS0);
}

uint8_t analogReadFast8(uint8_t analogPin) {
  // 1) Mappa il pin (A0, A1...) al canale ADC vero
  uint8_t ch;
  #ifdef analogPinToChannel
    ch = analogPinToChannel(analogPin);
#else
    ch = analogPin; // fallback se la macro non esiste
  #endif // analogPinToChannel

  // 2) Gestione canali alti (A6..A11) su Leonardo → MUX5 in ADCSRB
  #if defined(MUX5)
  if (ch & 0x20) { // se il bit 5 è alto → canale 8..15
    ADCSRB |= _BV(MUX5);
    ch &= 0x1F; // tieni solo i 5 bit bassi
  } else {
    ADCSRB &= ~_BV(MUX5);
  }
  #endif // defined(MUX5)

  // 3) Imposta il canale mantenendo riferimento e ADLAR
  uint8_t refbits = ADMUX & 0b11100000; // REFS1:0 + ADLAR
  ADMUX = refbits | (ch & 0x1F); // metti il canale nei bit MUX

  // 4) Avvia conversione
  ADCSRA |= _BV(ADSC);

  // 5) Aspetta che finisca (~26 µs con prescaler 32)
  while (ADCSRA & _BV(ADSC));

  // 6) Leggi il risultato 8-bit (0..255) dal registro alto
  return ADCH;
}

#else

// Fallback non-AVR: niente registri ADC. Manteniamo la firma per non rompere il codice.
void standardADC_init() {}
void fastADC_init() {}
uint8_t analogReadFast8(uint8_t analogPin) {
  // analogRead su SAMD è 10 bit (0..1023) di default: comprimiamo a 8 bit.
  return (uint8_t)(analogRead(analogPin) >> 2);
}

#endif // defined(__AVR__)

#if (Fast_analogread == 1)
// Lettura veloce "compatibile 10 bit":
// usa analogReadFast8() e scala il risultato a 0..1020 (~1024 step)
int analogRead_1024(uint8_t analogPin) {
  uint8_t v8 = analogReadFast8(analogPin); // 0..255
  // moltiplica per 4 → 0, 4, 8, ... 1020
  return (int)v8 << 2; // equivalente a v8 * 4
}
#endif // (Fast_analogread == 1)

#if (Fast_analogread == 0)
int analogRead_1024(uint8_t analogPin) {
  // dummy read: scarta la prima conversione (assestamento S/H)
  // (void)analogRead(analogPin);

  // lettura "buona"
  return analogRead(analogPin);
}
#endif // (Fast_analogread == 0)
