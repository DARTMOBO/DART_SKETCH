#ifndef DART_CONFIG_H
#define DART_CONFIG_H

// Global compile-time configuration for DART firmware
//
// === PROFILI (livello "utente") ===
// L'utente finale dovrebbe cambiare SOLO la riga qui sotto (idealmente nel file di apertura .ino/.inp):
//   #define DART_PROFILE 1   // 1=STANDARD  2=CUSTOM  3=KOROVA
//
// Se DART_PROFILE non e' definito altrove, di default usiamo STANDARD.
// NOTA: i blocchi CUSTOM/KOROVA qui sotto sono precompilati copiando i valori attuali (identici allo STANDARD).
//       Sarai tu a modificare le differenze quando decidi cosa deve cambiare per ogni profilo.

#ifndef DART_PROFILE
  #define DART_PROFILE 1
#endif

// -----------------------------------------------------------------------------
// [PROFILE_DEFAULTS]  (STANDARD)
// -----------------------------------------------------------------------------
#define ENABLE_POT_TAKEOVER     1       // 1 = enable pot takeover (Page1/Page2 + ARMED/CAUGHT gate)
                                        // 0 = legacy behavior (no takeover, no lastbutton split)

#define ENABLE_AUTODETECT        0      // 1 = enable AUTODETECT mode (when no valid preset is found in EEPROM)
                                        // 0 = disable AUTODETECT code paths to save flash memory;
                                      
#define shifter_active           1      // 1 = enabled // 0 = disabled // SHIFT REGISTERS_ // if enabled, Matrix_pads must be disabled
#define LED_pattern              2      // 0 = dart one // 1 = kombat personal // 2 nb boards - Led animation pattern used by buttons and pots

//-------------------------
#define DMX_active               0      // 1 = enabled ; 0 = disabled .  DMX is enabled/disabled from this single flag.
#define DART_DMX_MAX_CHANNELS 127       // info ctrl-f #DMX_LIMITS_DART // Nota DART: l'editor puo' impostare indirizzi solo fino a 127,


#define touch_invert             0      // touch_invert: 1 inverts logic (active-LOW becomes active-HIGH internally).
#define touch_pullup             0      // touch_pullup: 1 enables INPUT_PULLUP (for buttons to GND / open-drain outputs).
#define touch_led_onboard        0      // LED indicator on pin 8 (SAFE only when Touch_sensors_enable == 2 / external touch on pins 7&9)
#define Touch_sensors_enable     0      // [TOUCH_FLAGS]
                                        // 0 = OFF
                                        // 1 = internal CapacitiveSensor
                                        // 2 = external digital pins 7&9
                                        // 3 = external digital pins 7&8
                                        // For external digital touch inputs (Touch_sensors_enable = 2 or 3):
                                        
#define top_spinner              1      // 1 = enabled // 0 = disabled // TOP SPINNER
#define Side_spinner             0      // 1 = enabled // 0 = disabled // SIDE SPINNER
#define Page_switch              0      // 1 = enabled // 0 = disabled // page_switch
#define Distance_sensor          0      // disable distance sensor functions if not used, to save flash memory
#define Seq_                     0      // disable seq control function 
#define Piezo_pads               0      // 1 = enable piezo_pads() on A5 when GENERAL settings say "pads present"; 0 = completely disable piezo pads logic
#define Extraplexer              0      // 1= abilita Extraplex - possibile leggere altri 8 input extra utilizzando il pin 9 di Arduino (se si usano le funzioni touch interne, per usare Extraplex si deve rinunciare al secondo touch sensor) 
#define Scale                    0      // 1= abilita funzione Scale - permette di registrare e suonare una scala di note tramite spinners o beam. 
#define Scene                    0      // 1 = scene/morph attivi (richiede D_scene.ino) // CTRL-F: Scene_FLAG
                                        // 0 = scene/morph OFF (puoi rimuovere D_scene.ino)
                                  
#define hid_keys                 1      // 1 = enabled
#define hid_mouse                0      // 1 = enabled

#define stratos                  0      // 1 = enabled // 0 = disabled // Stratos sketch version.
#define blinker                  1      // blink effect on a selected led depending on pot position

//---------------------------

#define note_off                 0      // 1 = enabled // 0 = disabled // note OFF messages on button release - if NOTE Type has been selected
#define pullups_active           1      // 1 = enabled // 0 = disabled // pullup resistors - 

#define page_LEDs                0      // 1 = page LEDs active
#define MIDI_thru                0      // 1 = MIDI Thru active
#define mouse_block              1      // 1 = enabled // 0 = disabled // mouse messages are stopped after 2 seconds of repeated activity
#define arrows_block             0      // 1 = enabled // 0 = disabled // arrow key messages are stopped after 2 seconds of repeated activity
#define encoders_                1      // 1 = enabled
#define MIDI_IN_block            0      // 1 = MIDI IN blocked
#define MIDI_OUT_block           0      // 1 = MIDI out blocked


#define Fast_analogread          1      // hi-speed analog read function - lower definition = 0...255
#define Dummy_read               0      // lettura stabilizzata per analogread - si fanno due letture e si scarta la prima - per stabilizzare il S&H


#define Matrix_Pads              0      // 1 = enabled // 2 = pads on 17-32 circuitposition (old)// 0 = disabled // max7219 chips
#define MATRIX_CROSS_FX          0      // 1 = effetto croce attivo, 0 = disattivato
#define MATRIXFX_MODE            1      // Modalita' effetto "croce" sui pad matrice // 0 = fullwhite (classico) ; 1 = XOR con lo sfondo (status[96])
#define led_eco                  0      // used in dart_lime, and max7219 based models, to save memory with low impact led efx 


#define monitor_temporizzato     0      // ogni 300ms viene monitorizzata la variabile valore - possibile modificare per monitorare altre cose
#define Velo_pads_debug          0      // visualizzazione di una serie di frames per analizzare l'impulso di un pad basato su FSR, per strutturare il velocity sensing






// -----------------------------------------------------------------------------
// [PROFILE_OVERRIDES]
// -----------------------------------------------------------------------------
// IMPORTANT: per ora CUSTOM e KOROVA copiano pari-pari lo STANDARD.
//            Quando decidi le differenze, modifica SOLO il blocco del profilo.

#if (DART_PROFILE == 2)

  // === CUSTOM (profilo 2) ===
  #undef ENABLE_POT_TAKEOVER
  #define ENABLE_POT_TAKEOVER 1

  #undef ENABLE_AUTODETECT
  #define ENABLE_AUTODETECT 0

  #undef shifter_active
  #define shifter_active 1

  #undef LED_pattern
  #define LED_pattern 2

  #undef DMX_active
  #define DMX_active 0

  #undef DART_DMX_MAX_CHANNELS
  #define DART_DMX_MAX_CHANNELS 127

  #undef Touch_sensors_enable
  #define Touch_sensors_enable 1

  #undef touch_invert
  #define touch_invert 0

  #undef touch_pullup
  #define touch_pullup 0

  #undef touch_led_onboard
  #define touch_led_onboard 0

  #undef hid_keys
  #define hid_keys 1

  #undef hid_mouse
  #define hid_mouse 1

  #undef top_spinner
  #define top_spinner 1

  #undef Side_spinner
  #define Side_spinner 1

  #undef note_off
  #define note_off 0

  #undef pullups_active
  #define pullups_active 1

  #undef Page_switch
  #define Page_switch 1

  #undef page_LEDs
  #define page_LEDs 0

  #undef MIDI_thru
  #define MIDI_thru 0

  #undef mouse_block
  #define mouse_block 1

  #undef arrows_block
  #define arrows_block 0

  #undef encoders_
  #define encoders_ 1

  #undef MIDI_IN_block
  #define MIDI_IN_block 0

  #undef MIDI_OUT_block
  #define MIDI_OUT_block 0

  #undef Distance_sensor
  #define Distance_sensor 0

  #undef blinker
  #define blinker 1

  #undef Fast_analogread
  #define Fast_analogread 1

  #undef led_eco
  #define led_eco 0

  #undef Piezo_pads
  #define Piezo_pads 0

  #undef Velo_pads_debug
  #define Velo_pads_debug 0

  #undef Matrix_Pads
  #define Matrix_Pads 0

  #undef MATRIX_CROSS_FX
  #define MATRIX_CROSS_FX 0

  #undef MATRIXFX_MODE
  #define MATRIXFX_MODE 1

  #undef Dummy_read
  #define Dummy_read 0

  #undef monitor_temporizzato
  #define monitor_temporizzato 0

  #undef stratos
  #define stratos 0

#elif (DART_PROFILE == 3)

  // === KOROVA (profilo 3) ===
  #undef ENABLE_POT_TAKEOVER
  #define ENABLE_POT_TAKEOVER 1

  #undef ENABLE_AUTODETECT
  #define ENABLE_AUTODETECT 0

  #undef shifter_active
  #define shifter_active 1

  #undef LED_pattern
  #define LED_pattern 2

  #undef DMX_active
  #define DMX_active 0

  #undef DART_DMX_MAX_CHANNELS
  #define DART_DMX_MAX_CHANNELS 127

  #undef Touch_sensors_enable
  #define Touch_sensors_enable 1

  #undef touch_invert
  #define touch_invert 0

  #undef touch_pullup
  #define touch_pullup 0

  #undef touch_led_onboard
  #define touch_led_onboard 0

  #undef hid_keys
  #define hid_keys 1

  #undef hid_mouse
  #define hid_mouse 1

  #undef top_spinner
  #define top_spinner 1

  #undef Side_spinner
  #define Side_spinner 1

  #undef note_off
  #define note_off 0

  #undef pullups_active
  #define pullups_active 1

  #undef Page_switch
  #define Page_switch 1

  #undef page_LEDs
  #define page_LEDs 0

  #undef MIDI_thru
  #define MIDI_thru 0

  #undef mouse_block
  #define mouse_block 1

  #undef arrows_block
  #define arrows_block 0

  #undef encoders_
  #define encoders_ 1

  #undef MIDI_IN_block
  #define MIDI_IN_block 0

  #undef MIDI_OUT_block
  #define MIDI_OUT_block 0

  #undef Distance_sensor
  #define Distance_sensor 0

  #undef blinker
  #define blinker 1

  #undef Fast_analogread
  #define Fast_analogread 1

  #undef led_eco
  #define led_eco 0

  #undef Piezo_pads
  #define Piezo_pads 0

  #undef Velo_pads_debug
  #define Velo_pads_debug 0

  #undef Matrix_Pads
  #define Matrix_Pads 0

  #undef MATRIX_CROSS_FX
  #define MATRIX_CROSS_FX 0

  #undef MATRIXFX_MODE
  #define MATRIXFX_MODE 1

  #undef Dummy_read
  #define Dummy_read 0

  #undef monitor_temporizzato
  #define monitor_temporizzato 0

  #undef stratos
  #define stratos 0

#endif

#endif // DART_CONFIG_H
