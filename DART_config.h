#ifndef DART_CONFIG_H
#define DART_CONFIG_H
#ifndef DART_PROFILE
#define DART_PROFILE 2
#endif


// Global compile-time configuration for DART firmware
//
// === PROFILI (livello "utente") ===
// L'utente finale dovrebbe cambiare SOLO la riga qui (idealmente nel file di apertura .ino/.inp):
//   #define DART_PROFILE 1   // 1=BASIC  2=FULL  3=KOROVA 4=LIME 5=KOMBAT 6=STRATOS
//
// Se DART_PROFILE non e' definito altrove, di default usiamo STANDARD.
// NOTA: i blocchi CUSTOM/KOROVA qui sotto sono precompilati copiando i valori attuali (identici allo STANDARD).
//       Sarai tu a modificare le differenze quando decidi cosa deve cambiare per ogni profilo.



// -----------------------------------------------------------------------------
// [PROFILE_DEFAULTS]  (STANDARD - basic)
// -----------------------------------------------------------------------------
#define ENABLE_POT_TAKEOVER     1       // 1 = enable pot takeover (Page1/Page2 + ARMED/CAUGHT gate)
                                        // 0 = legacy behavior (no takeover, no lastbutton split)

#define ENABLE_AUTODETECT        0      // 1 = enable AUTODETECT mode (when no valid preset is found in EEPROM)
                                        // 0 = disable AUTODETECT code paths to save flash memory;
                                  
#define shifter_active           1      // 1 = enabled // 0 = disabled // SHIFT REGISTERS_ // if enabled, Matrix_pads must be disabled
#define LED_pattern              2      // 0 = dart one // 1 = kombat personal // 2 nb boards - Led animation pattern used by buttons and pots

    
//-------------------------
#define DMX_active               0      // 1 = enabled ; 0 = disabled .  DMX is enabled/disabled from this single flag.

#define FAST_FEEDBACK           1      // 1 = enable fast MIDI->LED/DMX 'ONLY' modes (modetable 35/36); 0 = disable
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
                                        
#define top_spinner              0      // 1 = enabled // 0 = disabled // TOP SPINNER
#define Side_spinner             0      // 1 = enabled // 0 = disabled // SIDE SPINNER
#define Page_switch              1      // 1 = enabled // 0 = disabled // page_switch
#define Distance_sensor          0      // disable distance sensor functions if not used, to save flash memory
#define Seq_                     0      // disable seq control function 
#define Piezo_pads               0      // 1 = enable piezo_pads() on A5 when GENERAL settings say "pads present"; 0 = completely disable piezo pads logic
#define Extraplexer              1      // 1= abilita Extraplex - possibile leggere altri 8 input extra utilizzando il pin 9 di Arduino (se si usano le funzioni touch interne, per usare Extraplex si deve rinunciare al secondo touch sensor) 
#define Scale                    0      // 1= abilita funzione Scale - permette di registrare e suonare una scala di note tramite spinners o beam. 
#define Scene                    0      // 1 = scene/morph attivi (richiede D_scene.ino) // CTRL-F: Scene_FLAG
                                        // 0 = scene/morph OFF (puoi rimuovere D_scene.ino)
                                  
#define hid_keys                 0      // 1 = enabled
#define hid_mouse                1      // 1 = enabled

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


#define MIDI_OUT_CLEANER         0      // CTRL-F: MIDI_OUT_CLEANER_FLAG  // 1 = abilita il "cleaner" software per encoders capricciosi (filtra msg uguali che cambiano solo databyte2)




// ==========================
// ENCODER INPUT CLEANER (majority + lockout)
// Obiettivo: rendere gli encoder piu' "presentabili" (meno rimbalzi / colpi contrari)
// Funziona sia per DART classico (4051) sia per STRATOS (in futuro useremo lo stesso blocco).
// Spegni tutto mettendo ENABLE_ENC_MAJORITY = 0 e/o ENABLE_ENC_LOCKOUT = 0.
// ==========================
#define ENABLE_ENC_MAJORITY       0      // 1 = majority burst ON, 0 = lettura singola (come prima)
#define ENC_MAJ_SAMPLES           16     // numero letture nel cluster (es. 9..21)
#define ENC_MAJ_USDELAY           20     // microsecondi tra una lettura e l'altra (es. 10..60) - ho visto che a 300 è un problema girare veloce su un 24 scatti - 100 ancora funziona bene

// ===== Majority ADAPTIVE (facoltativo) =====
// Se ON: prima fa poche "pre-letture" velocissime.
// - se sono tutte uguali -> ritorna subito (risparmia tempo quando l'encoder e' fermo)
// - se non sono uguali -> fa il cluster completo (robustezza come prima)
// Se OFF: fa sempre il cluster completo (massima pulizia, ma piu' lento).
#define ENABLE_ENC_MAJ_ADAPTIVE   0      // 1 = adattivo ON, 0 = sempre cluster completo
#define ENC_MAJ_PRECHECK          3      // quante pre-letture (2..5 tipico). 3 = buon compromesso
#define ENC_MAJ_PRE_USDELAY       0      // microsecondi tra pre-letture (0..5). 0 = massimo speed

#define ENABLE_ENC_LOCKOUT        0      // 1 = lockout ON (anti doppi/contrari), 0 = OFF
#define ENC_LOCKOUT_US            800    // durata lockout in microsecondi (es. 400..1500)

// Lockout dedicato TOP SPINNER (interrupt)
// - Utile per tagliare "raffiche" rare su ottico in posizioni ambigue, e bounce su encoder meccanico.
// - Se vuoi lo stesso valore del lockout generale: lascia ENC_TOP_LOCKOUT_US = ENC_LOCKOUT_US.
// - Puoi anche fare tuning separato (es. ottico 20..50, meccanico 300..1200).
#define ENC_TOP_LOCKOUT_US        ENC_LOCKOUT_US

// ===== Majority WINDOW (solo quando l'encoder si muove) =====
// Importante: ha senso SOLO se ENABLE_ENC_MAJORITY == 1.
// Se ENABLE_ENC_MAJORITY == 0, la window viene forzata a 0 (zero codice in flash).
#if (ENABLE_ENC_MAJORITY == 1)
  #define ENABLE_ENC_MAJ_WINDOW     0       // 1 = majority solo dentro finestra, 0 = majority sempre
  #define ENC_MAJ_WINDOW_US         25000   // durata finestra in microsecondi (15k..40k tipico)
#else
  #define ENABLE_ENC_MAJ_WINDOW     0
  #define ENC_MAJ_WINDOW_US         0
#endif





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

#if (DART_PROFILE == 2)   // === FULL (profilo 2) ===

  // === CUSTOM (profilo 2) ===
  #undef ENABLE_POT_TAKEOVER
  #undef ENABLE_AUTODETECT
  #undef shifter_active
  #undef LED_pattern
  #undef DMX_active
  #undef DART_DMX_MAX_CHANNELS
  #undef touch_invert
  #undef touch_pullup
  #undef touch_led_onboard
  #undef Touch_sensors_enable
  #undef top_spinner
  #undef Side_spinner
  #undef Page_switch
  #undef Distance_sensor
  #undef Seq_
  #undef Piezo_pads
  #undef Extraplexer
  #undef Scale
  #undef Scene
  #undef hid_keys
  #undef hid_mouse
  #undef stratos
  #undef blinker
  #undef note_off
  #undef pullups_active
  #undef page_LEDs
  #undef MIDI_thru
  #undef mouse_block
  #undef arrows_block
  #undef encoders_
  #undef MIDI_IN_block
  #undef MIDI_OUT_block
  #undef MIDI_OUT_CLEANER
  #undef FAST_FEEDBACK
  // [ENC_FILTER_OVERRIDES] (majority / adaptive / lockout)
  #undef ENABLE_ENC_MAJORITY
  #undef ENC_MAJ_SAMPLES
  #undef ENC_MAJ_USDELAY
  #undef ENABLE_ENC_MAJ_ADAPTIVE
  #undef ENC_MAJ_PRECHECK
  #undef ENC_MAJ_PRE_USDELAY
  #undef ENABLE_ENC_LOCKOUT
  #undef ENC_LOCKOUT_US
  #undef ENC_TOP_LOCKOUT_US
  #undef Fast_analogread
  #undef Dummy_read
  #undef Matrix_Pads
  #undef MATRIX_CROSS_FX
  #undef MATRIXFX_MODE
  #undef led_eco
  #undef monitor_temporizzato
  #undef Velo_pads_debug

  // === FULL (profilo 2) ===

  #define ENABLE_POT_TAKEOVER     1       // 1 = enable pot takeover (Page1/Page2 + ARMED/CAUGHT gate)
  #define ENABLE_AUTODETECT        0      // 1 = enable AUTODETECT mode (when no valid preset is found in EEPROM)
  #define shifter_active           1      // 1 = enabled // 0 = disabled // SHIFT REGISTERS_ // if enabled, Matrix_pads must be disabled
  #define LED_pattern              2      // 0 = dart one // 1 = kombat personal // 2 nb boards - Led animation pattern used by buttons and pots
  #define DMX_active               0      // 1 = enabled ; 0 = disabled .  DMX is enabled/disabled from this single flag.
  #define DART_DMX_MAX_CHANNELS 127       // info ctrl-f #DMX_LIMITS_DART // Nota DART: l'editor puo' impostare indirizzi solo fino a 127,
  #define touch_invert             0      // touch_invert: 1 inverts logic (active-LOW becomes active-HIGH internally).
  #define touch_pullup             0      // touch_pullup: 1 enables INPUT_PULLUP (for buttons to GND / open-drain outputs).
  #define touch_led_onboard        0      // LED indicator on pin 8 (SAFE only when Touch_sensors_enable == 2 / external touch on pins 7&9)
  #define Touch_sensors_enable     1      // [TOUCH_FLAGS]
  #define top_spinner              1      // 1 = enabled // 0 = disabled // TOP SPINNER
  #define Side_spinner             1      // 1 = enabled // 0 = disabled // SIDE SPINNER
  #define Page_switch              1      // 1 = enabled // 0 = disabled // page_switch
  #define Distance_sensor          0      // disable distance sensor functions if not used, to save flash memory
  #define Seq_                     0      // disable seq control function
  #define Piezo_pads               0      // 1 = enable piezo_pads() on A5 when GENERAL settings say "pads present"; 0 = completely disable piezo pads logic
  #define Extraplexer              1      // 1= abilita Extraplex - possibile leggere altri 8 input extra utilizzando il pin 9 di Arduino (se si usano le funzioni touch interne, per usare Extraplex si deve rinunciare al secondo touch sensor)
  #define Scale                    0      // 1= abilita funzione Scale - permette di registrare e suonare una scala di note tramite spinners o beam.
  #define Scene                    0      // 1 = scene/morph attivi (richiede D_scene.ino) // CTRL-F: Scene_FLAG
  #define hid_keys                 1      // 1 = enabled
  #define hid_mouse                1      // 1 = enabled
  #define stratos                  0      // 1 = enabled // 0 = disabled // Stratos sketch version.
  #define blinker                  1      // blink effect on a selected led depending on pot position
  #define note_off                 0      // 1 = enabled // 0 = disabled // note OFF messages on button release - if NOTE Type has been selected
  #define pullups_active           1      // 1 = enabled // 0 = disabled // pullup resistors -
  #define page_LEDs                0      // 1 = page LEDs active
  #define MIDI_thru                0      // 1 = MIDI Thru active
  #define mouse_block              1      // 1 = enabled // 0 = disabled // mouse messages are stopped after 2 seconds of repeated activity
  #define arrows_block             0      // 1 = enabled // 0 = disabled // arrow key messages are stopped after 2 seconds of repeated activity
  #define encoders_                1      // 1 = enabled
  #define MIDI_IN_block            0      // 1 = MIDI IN blocked
  #define MIDI_OUT_block           0      // 1 = MIDI out blocked
  #define MIDI_OUT_CLEANER         1      // 1 = abilita il "cleaner" software (filtra msg uguali che cambiano solo databyte2)
  #define FAST_FEEDBACK            1      // 1 = funzioni midi feedback veloce LED-only e DMX-only attive
  // [ENC_FILTER_OVERRIDES] (majority / adaptive / lockout)
  #define ENABLE_ENC_MAJORITY       0      // 1 = majority burst ON, 0 = lettura singola
  #define ENC_MAJ_SAMPLES           16     // letture nel cluster (es. 9..21)
  #define ENC_MAJ_USDELAY           20     // us tra letture (es. 10..60)
  #define ENABLE_ENC_MAJ_ADAPTIVE   0      // 1 = pre-check adattivo ON
  #define ENC_MAJ_PRECHECK          3      // pre-letture veloci (2..5)
  #define ENC_MAJ_PRE_USDELAY       0      // us tra pre-letture (0..5)
  #define ENABLE_ENC_LOCKOUT        0      // 1 = lockout ON
  #define ENC_LOCKOUT_US            800    // us lockout generale (es. 400..1500)
  #define ENC_TOP_LOCKOUT_US        ENC_LOCKOUT_US  // lockout TOP (puoi separarlo)
  #define Fast_analogread          1      // hi-speed analog read function - lower definition = 0...255
  #define Dummy_read               0      // lettura stabilizzata per analogread - si fanno due letture e si scarta la prima - per stabilizzare il S&H
  #define Matrix_Pads              0      // 1 = enabled // 2 = pads on 17-32 circuitposition (old)// 0 = disabled // max7219 chips
  #define MATRIX_CROSS_FX          0      // 1 = effetto croce attivo, 0 = disattivato
  #define MATRIXFX_MODE            1      // Modalita' effetto "croce" sui pad matrice // 0 = fullwhite (classico) ; 1 = XOR con lo sfondo (status[96])
  #define led_eco                  0      // used in dart_lime, and max7219 based models, to save memory with low impact led efx
  #define monitor_temporizzato     0      // ogni 300ms viene monitorizzata la variabile valore - possibile modificare per monitorare altre cose
  #define Velo_pads_debug          0      // visualizzazione di una serie di frames per analizzare l'impulso di un pad basato su FSR, per strutturare il velocity sensing

#elif (DART_PROFILE == 3)  // === KOROVA (profilo 3) ===

  #undef ENABLE_POT_TAKEOVER
  #undef ENABLE_AUTODETECT
  #undef shifter_active
  #undef LED_pattern
  #undef DMX_active
  #undef DART_DMX_MAX_CHANNELS
  #undef touch_invert
  #undef touch_pullup
  #undef touch_led_onboard
  #undef Touch_sensors_enable
  #undef top_spinner
  #undef Side_spinner
  #undef Page_switch
  #undef Distance_sensor
  #undef Seq_
  #undef Piezo_pads
  #undef Extraplexer
  #undef Scale
  #undef Scene
  #undef hid_keys
  #undef hid_mouse
  #undef stratos
  #undef blinker
  #undef note_off
  #undef pullups_active
  #undef page_LEDs
  #undef MIDI_thru
  #undef mouse_block
  #undef arrows_block
  #undef encoders_
  #undef MIDI_IN_block
  #undef MIDI_OUT_block
  #undef MIDI_OUT_CLEANER
  #undef FAST_FEEDBACK
  // [ENC_FILTER_OVERRIDES] (majority / adaptive / lockout)
  #undef ENABLE_ENC_MAJORITY
  #undef ENC_MAJ_SAMPLES
  #undef ENC_MAJ_USDELAY
  #undef ENABLE_ENC_MAJ_ADAPTIVE
  #undef ENC_MAJ_PRECHECK
  #undef ENC_MAJ_PRE_USDELAY
  #undef ENABLE_ENC_LOCKOUT
  #undef ENC_LOCKOUT_US
  #undef ENC_TOP_LOCKOUT_US
  #undef Fast_analogread
  #undef Dummy_read
  #undef Matrix_Pads
  #undef MATRIX_CROSS_FX
  #undef MATRIXFX_MODE
  #undef led_eco
  #undef monitor_temporizzato
  #undef Velo_pads_debug

   // === KOROVA (profilo 3) ===

  #define ENABLE_POT_TAKEOVER      1      // 1 = enable pot takeover (Page1/Page2 + ARMED/CAUGHT gate)
  #define ENABLE_AUTODETECT        0      // 1 = enable AUTODETECT mode (when no valid preset is found in EEPROM)
  #define shifter_active           1      // 1 = enabled // 0 = disabled // SHIFT REGISTERS_ // if enabled, Matrix_pads must be disabled
  #define LED_pattern              2      // 0 = dart one // 1 = kombat personal // 2 nb boards - Led animation pattern used by buttons and pots
  #define DMX_active               0      // 1 = enabled ; 0 = disabled .  DMX is enabled/disabled from this single flag.
  #define DART_DMX_MAX_CHANNELS 127       // info ctrl-f #DMX_LIMITS_DART // Nota DART: l'editor puo' impostare indirizzi solo fino a 127,
  #define touch_invert             0      // touch_invert: 1 inverts logic (active-LOW becomes active-HIGH internally).
  #define touch_pullup             0      // touch_pullup: 1 enables INPUT_PULLUP (for buttons to GND / open-drain outputs).
  #define touch_led_onboard        0      // LED indicator on pin 8 (SAFE only when Touch_sensors_enable == 2 / external touch on pins 7&9)
  #define Touch_sensors_enable     1      // [TOUCH_FLAGS]
  #define top_spinner              1      // 1 = enabled // 0 = disabled // TOP SPINNER
  #define Side_spinner             1      // 1 = enabled // 0 = disabled // SIDE SPINNER
  #define Page_switch              1      // 1 = enabled // 0 = disabled // page_switch
  #define Distance_sensor          0      // disable distance sensor functions if not used, to save flash memory
  #define Seq_                     1      // disable seq control function
  #define Piezo_pads               0      // 1 = enable piezo_pads() on A5 when GENERAL settings say "pads present"; 0 = completely disable piezo pads logic
  #define Extraplexer              1      // 1= abilita Extraplex - possibile leggere altri 8 input extra utilizzando il pin 9 di Arduino (se si usano le funzioni touch interne, per usare Extraplex si deve rinunciare al secondo touch sensor)
  #define Scale                    1     // 1= abilita funzione Scale - permette di registrare e suonare una scala di note tramite spinners o beam.
  #define Scene                    1      // 1 = scene/morph attivi (richiede D_scene.ino) // CTRL-F: Scene_FLAG
  #define hid_keys                 1      // 1 = enabled
  #define hid_mouse                1      // 1 = enabled
  #define stratos                  0      // 1 = enabled // 0 = disabled // Stratos sketch version.
  #define blinker                  1      // blink effect on a selected led depending on pot position
  #define note_off                 0      // 1 = enabled // 0 = disabled // note OFF messages on button release - if NOTE Type has been selected
  #define pullups_active           1      // 1 = enabled // 0 = disabled // pullup resistors -
  #define page_LEDs                0      // 1 = page LEDs active
  #define MIDI_thru                0      // 1 = MIDI Thru active
  #define mouse_block              0      // 1 = enabled // 0 = disabled // mouse messages are stopped after 2 seconds of repeated activity
  #define arrows_block             0      // 1 = enabled // 0 = disabled // arrow key messages are stopped after 2 seconds of repeated activity
  #define encoders_                1      // 1 = enabled
  #define MIDI_IN_block            0      // 1 = MIDI IN blocked
  #define MIDI_OUT_block           0      // 1 = MIDI out blocked
  #define MIDI_OUT_CLEANER         0      // 1 = abilita il "cleaner" software (filtra msg uguali che cambiano solo databyte2)
  #define FAST_FEEDBACK            1      // 1 = funzioni midi feedback veloce LED-only e DMX-only attive
  // [ENC_FILTER_OVERRIDES] (majority / adaptive / lockout)
  #define ENABLE_ENC_MAJORITY       0      // 1 = majority burst ON, 0 = lettura singola
  #define ENC_MAJ_SAMPLES           16     // letture nel cluster (es. 9..21)
  #define ENC_MAJ_USDELAY           20     // us tra letture (es. 10..60)
  #define ENABLE_ENC_MAJ_ADAPTIVE   0      // 1 = pre-check adattivo ON
  #define ENC_MAJ_PRECHECK          3      // pre-letture veloci (2..5)
  #define ENC_MAJ_PRE_USDELAY       0      // us tra pre-letture (0..5)
  #define ENABLE_ENC_LOCKOUT        1      // 1 = lockout ON
  #define ENC_LOCKOUT_US            800    // us lockout generale (es. 400..1500)
  #define ENC_TOP_LOCKOUT_US        ENC_LOCKOUT_US  // lockout TOP (puoi separarlo)
  #define Fast_analogread          1      // hi-speed analog read function - lower definition = 0...255
  #define Dummy_read               1      // lettura stabilizzata per analogread - si fanno due letture e si scarta la prima - per stabilizzare il S&H
  #define Matrix_Pads              0      // 1 = enabled // 2 = pads on 17-32 circuitposition (old)// 0 = disabled // max7219 chips
  #define MATRIX_CROSS_FX          0      // 1 = effetto croce attivo, 0 = disattivato
  #define MATRIXFX_MODE            1      // Modalita' effetto "croce" sui pad matrice // 0 = fullwhite (classico) ; 1 = XOR con lo sfondo (status[96])
  #define led_eco                  0      // used in dart_lime, and max7219 based models, to save memory with low impact led efx
  #define monitor_temporizzato     0      // ogni 300ms viene monitorizzata la variabile valore - possibile modificare per monitorare altre cose
  #define Velo_pads_debug          0      // visualizzazione di una serie di frames per analizzare l'impulso di un pad basato su FSR, per strutturare il velocity sensing

#elif (DART_PROFILE == 4)    // === LIME ===

  #undef ENABLE_POT_TAKEOVER
  #undef ENABLE_AUTODETECT
  #undef shifter_active
  #undef LED_pattern
  #undef DMX_active
  #undef DART_DMX_MAX_CHANNELS
  #undef touch_invert
  #undef touch_pullup
  #undef touch_led_onboard
  #undef Touch_sensors_enable
  #undef top_spinner
  #undef Side_spinner
  #undef Page_switch
  #undef Distance_sensor
  #undef Seq_
  #undef Piezo_pads
  #undef Extraplexer
  #undef Scale
  #undef Scene
  #undef hid_keys
  #undef hid_mouse
  #undef stratos
  #undef blinker
  #undef note_off
  #undef pullups_active
  #undef page_LEDs
  #undef MIDI_thru
  #undef mouse_block
  #undef arrows_block
  #undef encoders_
  #undef MIDI_IN_block
  #undef MIDI_OUT_block
  #undef MIDI_OUT_CLEANER
  #undef FAST_FEEDBACK
  // [ENC_FILTER_OVERRIDES] (majority / adaptive / lockout)
  #undef ENABLE_ENC_MAJORITY
  #undef ENC_MAJ_SAMPLES
  #undef ENC_MAJ_USDELAY
  #undef ENABLE_ENC_MAJ_ADAPTIVE
  #undef ENC_MAJ_PRECHECK
  #undef ENC_MAJ_PRE_USDELAY
  #undef ENABLE_ENC_LOCKOUT
  #undef ENC_LOCKOUT_US
  #undef ENC_TOP_LOCKOUT_US
  #undef Fast_analogread
  #undef Dummy_read
  #undef Matrix_Pads
  #undef MATRIX_CROSS_FX
  #undef MATRIXFX_MODE
  #undef led_eco
  #undef monitor_temporizzato
  #undef Velo_pads_debug
  
   // === 4 - LIME ===
   
  #define ENABLE_POT_TAKEOVER      1      // 1 = enable pot takeover (Page1/Page2 + ARMED/CAUGHT gate)
  #define ENABLE_AUTODETECT        0      // 1 = enable AUTODETECT mode (when no valid preset is found in EEPROM)
  #define shifter_active           0      // 1 = enabled // 0 = disabled // SHIFT REGISTERS_ // if enabled, Matrix_pads must be disabled
  #define LED_pattern              0      // 0 = dart one // 1 = kombat personal // 2 nb boards - Led animation pattern used by buttons and pots
  #define DMX_active               0      // 1 = enabled ; 0 = disabled .  DMX is enabled/disabled from this single flag.
  #define DART_DMX_MAX_CHANNELS 127       // info ctrl-f #DMX_LIMITS_DART // Nota DART: l'editor puo' impostare indirizzi solo fino a 127,
  #define touch_invert             0      // touch_invert: 1 inverts logic (active-LOW becomes active-HIGH internally).
  #define touch_pullup             0      // touch_pullup: 1 enables INPUT_PULLUP (for buttons to GND / open-drain outputs).
  #define touch_led_onboard        1      // LED indicator on pin 8 (SAFE only when Touch_sensors_enable == 2 / external touch on pins 7&9)
  #define Touch_sensors_enable     2      // [TOUCH_FLAGS] // 0 = OFF// 1 = internal // 2 = external pins 7&9 // 3 = external pins 7&8
  #define top_spinner              1      // 1 = enabled // 0 = disabled // TOP SPINNER
  #define Side_spinner             1      // 1 = enabled // 0 = disabled // SIDE SPINNER
  #define Page_switch              1      // 1 = enabled // 0 = disabled // page_switch
  #define Distance_sensor          0      // disable distance sensor functions if not used, to save flash memory
  #define Seq_                     1      // disable seq control function
  #define Piezo_pads               0      // 1 = enable piezo_pads() on A5 when GENERAL settings say "pads present"; 0 = completely disable piezo pads logic
  #define Extraplexer              0      // 1= abilita Extraplex - possibile leggere altri 8 input extra utilizzando il pin 9 di Arduino (se si usano le funzioni touch interne, per usare Extraplex si deve rinunciare al secondo touch sensor)
  #define Scale                    0      // 1= abilita funzione Scale - permette di registrare e suonare una scala di note tramite spinners o beam.
  #define Scene                    0      // 1 = scene/morph attivi (richiede D_scene.ino) // CTRL-F: Scene_FLAG
  #define hid_keys                 1      // 1 = enabled
  #define hid_mouse                1      // 1 = enabled
  #define stratos                  0      // 1 = enabled // 0 = disabled // Stratos sketch version.
  #define blinker                  1      // blink effect on a selected led depending on pot position
  #define note_off                 0      // 1 = enabled // 0 = disabled // note OFF messages on button release - if NOTE Type has been selected
  #define pullups_active           1      // 1 = enabled // 0 = disabled // pullup resistors -
  #define page_LEDs                0      // 1 = page LEDs active
  #define MIDI_thru                0      // 1 = MIDI Thru active
  #define mouse_block              0      // 1 = enabled // 0 = disabled // mouse messages are stopped after 2 seconds of repeated activity
  #define arrows_block             0      // 1 = enabled // 0 = disabled // arrow key messages are stopped after 2 seconds of repeated activity
  #define encoders_                1      // 1 = enabled
  #define MIDI_IN_block            0      // 1 = MIDI IN blocked
  #define MIDI_OUT_block           0      // 1 = MIDI out blocked
  #define MIDI_OUT_CLEANER         1      // 1 = abilita il "cleaner" software (filtra msg uguali che cambiano solo databyte2)
  #define FAST_FEEDBACK            0 
  // [ENC_FILTER_OVERRIDES] (majority / adaptive / lockout)
  #define ENABLE_ENC_MAJORITY       1      // 1 = majority burst ON, 0 = lettura singola
  #define ENC_MAJ_SAMPLES           4     // letture nel cluster (es. 9..21)
  #define ENC_MAJ_USDELAY           0     // us tra letture (es. 10..60) - 500 completamente ingolfato , utile per testare dispositivo attivo - 20 consigliato gpt - 100 ancora troppo - IO LO METTO A 16/10
  #define ENABLE_ENC_MAJ_ADAPTIVE   0      // 1 = pre-check adattivo ON
  #define ENC_MAJ_PRECHECK          3      // pre-letture veloci (2..5)
  #define ENC_MAJ_PRE_USDELAY       0      // us tra pre-letture (0..5)
  #define ENABLE_ENC_LOCKOUT        1      // 1 = lockout ON
  #define ENC_LOCKOUT_US            50UL    // us lockout generale (es. 400..1500) -10000 freno fortissimo, per rendersi conto che il dispositivo funziona - 800 = valore consigliato gpt - 
  #define ENC_TOP_LOCKOUT_US        ENC_LOCKOUT_US  // lockout TOP (puoi separarlo)
  #define Fast_analogread          1      // hi-speed analog read function - lower definition = 0...255
  #define Dummy_read               1      // lettura stabilizzata per analogread - si fanno due letture e si scarta la prima - per stabilizzare il S&H
  #define Matrix_Pads              1      // 1 = enabled // 2 = pads on 17-32 circuitposition (old)// 0 = disabled // max7219 chips
  #define MATRIX_CROSS_FX          1      // 1 = effetto croce attivo, 0 = disattivato
  #define MATRIXFX_MODE            1      // Modalita' effetto "croce" sui pad matrice // 0 = fullwhite (classico) ; 1 = XOR con lo sfondo (status[96])
  #define led_eco                  0      // used in dart_lime, and max7219 based models, to save memory with low impact led efx
  #define monitor_temporizzato     0      // ogni 300ms viene monitorizzata la variabile valore - possibile modificare per monitorare altre cose
  #define Velo_pads_debug          0      // visualizzazione di una serie di frames per analizzare l'impulso di un pad basato su FSR, per strutturare il velocity sensing


#elif (DART_PROFILE == 5)   // === KOMBAT (profilo 5) ===
  
  #undef ENABLE_POT_TAKEOVER
  #undef ENABLE_AUTODETECT
  #undef shifter_active
  #undef LED_pattern
  #undef DMX_active
  #undef DART_DMX_MAX_CHANNELS
  #undef touch_invert
  #undef touch_pullup
  #undef touch_led_onboard
  #undef Touch_sensors_enable
  #undef top_spinner
  #undef Side_spinner
  #undef Page_switch
  #undef Distance_sensor
  #undef Seq_
  #undef Piezo_pads
  #undef Extraplexer
  #undef Scale
  #undef Scene
  #undef hid_keys
  #undef hid_mouse
  #undef stratos
  #undef blinker
  #undef note_off
  #undef pullups_active
  #undef page_LEDs
  #undef MIDI_thru
  #undef mouse_block
  #undef arrows_block
  #undef encoders_
  #undef MIDI_IN_block
  #undef MIDI_OUT_block
  #undef MIDI_OUT_CLEANER
  #undef FAST_FEEDBACK
  // [ENC_FILTER_OVERRIDES] (majority / adaptive / lockout)
  #undef ENABLE_ENC_MAJORITY
  #undef ENC_MAJ_SAMPLES
  #undef ENC_MAJ_USDELAY
  #undef ENABLE_ENC_MAJ_ADAPTIVE
  #undef ENC_MAJ_PRECHECK
  #undef ENC_MAJ_PRE_USDELAY
  #undef ENABLE_ENC_LOCKOUT
  #undef ENC_LOCKOUT_US
  #undef ENC_TOP_LOCKOUT_US
  #undef Fast_analogread
  #undef Dummy_read
  #undef Matrix_Pads
  #undef MATRIX_CROSS_FX
  #undef MATRIXFX_MODE
  #undef led_eco
  #undef monitor_temporizzato
  #undef Velo_pads_debug

  // === KOMBAT (profilo 5) ===
  
  #define ENABLE_POT_TAKEOVER      1      // 1 = enable pot takeover (Page1/Page2 + ARMED/CAUGHT gate)
  #define ENABLE_AUTODETECT        0      // 1 = enable AUTODETECT mode (when no valid preset is found in EEPROM)
  #define shifter_active           1      // 1 = enabled // 0 = disabled // SHIFT REGISTERS_ // if enabled, Matrix_pads must be disabled
  #define LED_pattern              5      // 0 = dart one // 1 = Kombat pers. // 2 = nb boards - // 5 = Kombat // Led animation pattern used by buttons and pots
  #define DMX_active               0      // 1 = enabled ; 0 = disabled .  DMX is enabled/disabled from this single flag.
  #define DART_DMX_MAX_CHANNELS 127       // info ctrl-f #DMX_LIMITS_DART // Nota DART: l'editor puo' impostare indirizzi solo fino a 127,
  #define touch_invert             0      // touch_invert: 1 inverts logic (active-LOW becomes active-HIGH internally).
  #define touch_pullup             0      // touch_pullup: 1 enables INPUT_PULLUP (for buttons to GND / open-drain outputs).
  #define touch_led_onboard        0      // LED indicator on pin 8 (SAFE only when Touch_sensors_enable == 2 / external touch on pins 7&9)
  #define Touch_sensors_enable     1      // [TOUCH_FLAGS] // 0 = OFF// 1 = internal // 2 = external pins 7&9 // 3 = external pins 7&8
  #define top_spinner              1      // 1 = enabled // 0 = disabled // TOP SPINNER
  #define Side_spinner             1      // 1 = enabled // 0 = disabled // SIDE SPINNER
  #define Page_switch              1      // 1 = enabled // 0 = disabled // page_switch
  #define Distance_sensor          1      // disable distance sensor functions if not used, to save flash memory
  #define Seq_                     1      // disable seq control function
  #define Piezo_pads               0      // 1 = enable piezo_pads() on A5 when GENERAL settings say "pads present"; 0 = completely disable piezo pads logic
  #define Extraplexer              0      // 1= abilita Extraplex - possibile leggere altri 8 input extra utilizzando il pin 9 di Arduino (se si usano le funzioni touch interne, per usare Extraplex si deve rinunciare al secondo touch sensor)
  #define Scale                    0      // 1= abilita funzione Scale - permette di registrare e suonare una scala di note tramite spinners o beam.
  #define Scene                    1      // 1 = scene/morph attivi (richiede D_scene.ino) // CTRL-F: Scene_FLAG
  #define hid_keys                 0      // 1 = enabled
  #define hid_mouse                1      // 1 = enabled
  #define stratos                  0      // 1 = enabled // 0 = disabled // Stratos sketch version.
  #define blinker                  1      // blink effect on a selected led depending on pot position
  #define note_off                 0      // 1 = enabled // 0 = disabled // note OFF messages on button release - if NOTE Type has been selected
  #define pullups_active           1      // 1 = enabled // 0 = disabled // pullup resistors -
  #define page_LEDs                0      // 1 = page LEDs active
  #define MIDI_thru                0      // 1 = MIDI Thru active
  #define mouse_block              1      // 1 = enabled // 0 = disabled // mouse messages are stopped after 2 seconds of repeated activity
  #define arrows_block             0      // 1 = enabled // 0 = disabled // arrow key messages are stopped after 2 seconds of repeated activity
  #define encoders_                1      // 1 = enabled
  #define MIDI_IN_block            0      // 1 = MIDI IN blocked
  #define MIDI_OUT_block           0      // 1 = MIDI out blocked
  #define MIDI_OUT_CLEANER         0      // 1 = abilita il "cleaner" software (filtra msg uguali che cambiano solo databyte2)
  #define FAST_FEEDBACK            0      // 1 = funzioni midi feedback veloce LED-only e DMX-only attive
  // [ENC_FILTER_OVERRIDES] (majority / adaptive / lockout)
  #define ENABLE_ENC_MAJORITY       1      // 1 = majority burst ON, 0 = lettura singola
  #define ENC_MAJ_SAMPLES           16     // letture nel cluster (es. 9..21)
  #define ENC_MAJ_USDELAY           50UL     // us tra letture (es. 10..60)
  #define ENABLE_ENC_MAJ_ADAPTIVE   0      // 1 = pre-check adattivo ON
  #define ENC_MAJ_PRECHECK          5      // pre-letture veloci (2..5)
  #define ENC_MAJ_PRE_USDELAY       0      // us tra pre-letture (0..5)
  #define ENABLE_ENC_LOCKOUT        0      // 1 = lockout ON
  #define ENC_LOCKOUT_US            800    // us lockout generale (es. 400..1500)
  #define ENC_TOP_LOCKOUT_US        ENC_LOCKOUT_US  // lockout TOP (puoi separarlo)
  #define Fast_analogread          1      // hi-speed analog read function - lower definition = 0...255
  #define Dummy_read               1      // lettura stabilizzata per analogread - si fanno due letture e si scarta la prima - per stabilizzare il S&H
  #define Matrix_Pads              0      // 1 = enabled // 2 = pads on 17-32 circuitposition (old)// 0 = disabled // max7219 chips
  #define MATRIX_CROSS_FX          0      // 1 = effetto croce attivo, 0 = disattivato
  #define MATRIXFX_MODE            0      // Modalita' effetto "croce" sui pad matrice // 0 = fullwhite (classico) ; 1 = XOR con lo sfondo (status[96])
  #define led_eco                  0      // used in dart_lime, and max7219 based models, to save memory with low impact led efx
  #define monitor_temporizzato     0      // ogni 300ms viene monitorizzata la variabile valore - possibile modificare per monitorare altre cose
  #define Velo_pads_debug          0      // visualizzazione di una serie di frames per analizzare l'impulso di un pad basato su FSR, per strutturare il velocity sensing


#elif (DART_PROFILE == 6)  // === STRATOS ===
  
  #undef ENABLE_POT_TAKEOVER
  #undef ENABLE_AUTODETECT
  #undef shifter_active
  #undef LED_pattern
  #undef DMX_active
  #undef DART_DMX_MAX_CHANNELS
  #undef touch_invert
  #undef touch_pullup
  #undef touch_led_onboard
  #undef Touch_sensors_enable
  #undef top_spinner
  #undef Side_spinner
  #undef Page_switch
  #undef Distance_sensor
  #undef Seq_
  #undef Piezo_pads
  #undef Extraplexer
  #undef Scale
  #undef Scene
  #undef hid_keys
  #undef hid_mouse
  #undef stratos
  #undef blinker
  #undef note_off
  #undef pullups_active
  #undef page_LEDs
  #undef MIDI_thru
  #undef mouse_block
  #undef arrows_block
  #undef encoders_
  #undef MIDI_IN_block
  #undef MIDI_OUT_block
  #undef MIDI_OUT_CLEANER
  #undef FAST_FEEDBACK
  // [ENC_FILTER_OVERRIDES] (majority / adaptive / lockout)
  #undef ENABLE_ENC_MAJORITY
  #undef ENC_MAJ_SAMPLES
  #undef ENC_MAJ_USDELAY
  #undef ENABLE_ENC_MAJ_ADAPTIVE
  #undef ENC_MAJ_PRECHECK
  #undef ENC_MAJ_PRE_USDELAY
  #undef ENABLE_ENC_LOCKOUT
  #undef ENC_LOCKOUT_US
  #undef ENC_TOP_LOCKOUT_US
  #undef Fast_analogread
  #undef Dummy_read
  #undef Matrix_Pads
  #undef MATRIX_CROSS_FX
  #undef MATRIXFX_MODE
  #undef led_eco
  #undef monitor_temporizzato
  #undef Velo_pads_debug
    
    // === 6- STRATOS ===

  #define ENABLE_POT_TAKEOVER      1      // 1 = enable pot takeover (Page1/Page2 + ARMED/CAUGHT gate)
  #define ENABLE_AUTODETECT        0      // 1 = enable AUTODETECT mode (when no valid preset is found in EEPROM)
  #define shifter_active           0      // 1 = enabled // 0 = disabled // SHIFT REGISTERS_ // if enabled, Matrix_pads must be disabled
  #define LED_pattern              5      // 0 = dart one // 1 = Kombat pers. // 2 = nb boards - // 5 = Kombat // Led animation pattern used by buttons and pots
  #define DMX_active               1      // 1 = enabled ; 0 = disabled .  DMX is enabled/disabled from this single flag.
  #define DART_DMX_MAX_CHANNELS 127       // info ctrl-f #DMX_LIMITS_DART // Nota DART: l'editor puo' impostare indirizzi solo fino a 127,
  #define touch_invert             0      // touch_invert: 1 inverts logic (active-LOW becomes active-HIGH internally).
  #define touch_pullup             0      // touch_pullup: 1 enables INPUT_PULLUP (for buttons to GND / open-drain outputs).
  #define touch_led_onboard        1      // LED indicator on pin 8 (SAFE only when Touch_sensors_enable == 2 / external touch on pins 7&9)
  #define Touch_sensors_enable     0      // [TOUCH_FLAGS] // 0 = OFF// 1 = internal // 2 = external pins 7&9 // 3 = external pins 7&8
  #define top_spinner              1      // 1 = enabled // 0 = disabled // TOP SPINNER
  #define Side_spinner             0      // 1 = enabled // 0 = disabled // SIDE SPINNER
  #define Page_switch              1      // 1 = enabled // 0 = disabled // page_switch
  #define Distance_sensor          0      // disable distance sensor functions if not used, to save flash memory
  #define Seq_                     1      // disable seq control function
  #define Piezo_pads               0      // 1 = enable piezo_pads() on A5 when GENERAL settings say "pads present"; 0 = completely disable piezo pads logic
  #define Extraplexer              0      // 1= abilita Extraplex - possibile leggere altri 8 input extra utilizzando il pin 9 di Arduino (se si usano le funzioni touch interne, per usare Extraplex si deve rinunciare al secondo touch sensor)
  #define Scale                    1      // 1= abilita funzione Scale - permette di registrare e suonare una scala di note tramite spinners o beam.
  #define Scene                    0      // 1 = scene/morph attivi (richiede D_scene.ino) // CTRL-F: Scene_FLAG
  #define hid_keys                 1      // 1 = enabled
  #define hid_mouse                1      // 1 = enabled
  #define stratos                  1      // 1 = enabled // 0 = disabled // Stratos sketch version.
  #define blinker                  1      // blink effect on a selected led depending on pot position
  #define note_off                 0      // 1 = enabled // 0 = disabled // note OFF messages on button release - if NOTE Type has been selected
  #define pullups_active           1      // 1 = enabled // 0 = disabled // pullup resistors -
  #define page_LEDs                0      // 1 = page LEDs active
  #define MIDI_thru                0      // 1 = MIDI Thru active
  #define mouse_block              1      // 1 = enabled // 0 = disabled // mouse messages are stopped after 2 seconds of repeated activity
  #define arrows_block             0      // 1 = enabled // 0 = disabled // arrow key messages are stopped after 2 seconds of repeated activity
  #define encoders_                1      // 1 = enabled
  #define MIDI_IN_block            0      // 1 = MIDI IN blocked
  #define MIDI_OUT_block           0      // 1 = MIDI out blocked
  #define MIDI_OUT_CLEANER         1      // 1 = abilita il "cleaner" software (filtra msg uguali che cambiano solo databyte2)
  #define FAST_FEEDBACK            0
  // [ENC_FILTER_OVERRIDES] (majority / adaptive / lockout)
  #define ENABLE_ENC_MAJORITY       1      // 1 = majority burst ON, 0 = lettura singola
  #define ENC_MAJ_SAMPLES           16     // letture nel cluster (es. 9..21)
  #define ENC_MAJ_USDELAY           20     // us tra letture (es. 10..60)
  #define ENABLE_ENC_MAJ_ADAPTIVE   0      // 1 = pre-check adattivo ON
  #define ENC_MAJ_PRECHECK          3      // pre-letture veloci (2..5)
  #define ENC_MAJ_PRE_USDELAY       0      // us tra pre-letture (0..5)
  #define ENABLE_ENC_LOCKOUT        1      // 1 = lockout ON
  #define ENC_LOCKOUT_US            800    // us lockout generale (es. 400..1500)
  #define ENC_TOP_LOCKOUT_US        ENC_LOCKOUT_US  // lockout TOP (puoi separarlo)
  #define Fast_analogread          0      // hi-speed analog read function - lower definition = 0...255
  #define Dummy_read               0      // lettura stabilizzata per analogread - si fanno due letture e si scarta la prima - per stabilizzare il S&H
  #define Matrix_Pads              0      // 1 = enabled // 2 = pads on 17-32 circuitposition (old)// 0 = disabled // max7219 chips
  #define MATRIX_CROSS_FX          0      // 1 = effetto croce attivo, 0 = disattivato
  #define MATRIXFX_MODE            0      // Modalita' effetto "croce" sui pad matrice // 0 = fullwhite (classico) ; 1 = XOR con lo sfondo (status[96])
  #define led_eco                  0      // used in dart_lime, and max7219 based models, to save memory with low impact led efx
  #define monitor_temporizzato     0      // ogni 300ms viene monitorizzata la variabile valore - possibile modificare per monitorare altre cose
  #define Velo_pads_debug          0      // visualizzazione di una serie di frames per analizzare l'impulso di un pad basato su FSR, per strutturare il velocity sensing


  

#else  // === STANDARD (profilo 1) ===

  // === STANDARD (profilo 1) ===
  // Qui puoi personalizzare i filtri encoder SENZA toccare i blocchi globali sopra.
  // Se non vuoi differenze, lascia questi valori uguali agli altri profili.
  // [ENC_FILTER_OVERRIDES] (majority / adaptive / lockout)
  #undef ENABLE_ENC_MAJORITY
  #undef ENC_MAJ_SAMPLES
  #undef ENC_MAJ_USDELAY
  #undef ENABLE_ENC_MAJ_ADAPTIVE
  #undef ENC_MAJ_PRECHECK
  #undef ENC_MAJ_PRE_USDELAY
  #undef ENABLE_ENC_LOCKOUT
  #undef ENC_LOCKOUT_US
  #undef ENC_TOP_LOCKOUT_US
  // [ENC_FILTER_OVERRIDES] (majority / adaptive / lockout)
  #define ENABLE_ENC_MAJORITY       0      // 1 = majority burst ON, 0 = lettura singola
  #define ENC_MAJ_SAMPLES           16     // letture nel cluster (es. 9..21)
  #define ENC_MAJ_USDELAY           20     // us tra letture (es. 10..60)
  #define ENABLE_ENC_MAJ_ADAPTIVE   0      // 1 = pre-check adattivo ON
  #define ENC_MAJ_PRECHECK          3      // pre-letture veloci (2..5)
  #define ENC_MAJ_PRE_USDELAY       0      // us tra pre-letture (0..5)
  #define ENABLE_ENC_LOCKOUT        0      // 1 = lockout ON
  #define ENC_LOCKOUT_US            800    // us lockout generale (es. 400..1500)
  #define ENC_TOP_LOCKOUT_US        ENC_LOCKOUT_US  // lockout TOP (puoi separarlo)

#endif


/* =====================================================================
   FORCE_UNO_SAFE_DEFINES
   Se compiliamo per MCU senza USB/HID (es. Arduino UNO / ATmega328P),
   forziamo a OFF le feature che richiedono 32U4 (Leonardo/Micro).
   ===================================================================== */

#if !defined(__AVR_ATmega32U4__)
  // forza off tutto ciò che è USB/HID specifico 32U4
  #undef hid_mouse
  #define hid_mouse 0
  #undef hid_keys
  #define hid_keys 0
  #undef midi_usb
  #define midi_usb 0
#endif


#endif // DART_CONFIG_H
