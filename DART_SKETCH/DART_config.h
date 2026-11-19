#ifndef DART_CONFIG_H
#define DART_CONFIG_H

// Select 1 for STANDARD, 2 for CUSTOM, or 3 for KOROVA/ONE.
// Both controllers use profile 3. Load the matching controller preset in DART_EDIT.
#define DART_PROFILE 1
#if DART_PROFILE != 1 && DART_PROFILE != 2 && DART_PROFILE != 3
#error "Select 1 (STANDARD), 2 (CUSTOM) or 3 (KOROVA/ONE)."
#endif

// [PROFILE_DEFAULTS]  (STANDARD - basic)
// -----------------------------------------------------------------------------

#define ENABLE_AUTODETECT           1   // 1 = enable AUTODETECT mode (when no valid preset is found in EEPROM)

#define ENABLE_EEPROM               1    

//-------------------------
                               
#define shifter_active              1    

#define LED_pattern                 2  // LED wiring table: 2 = factory wiring, 0 = alternative wiring

//-------------------------

#define DMX_active                  0   // 1 = enabled ; 0 = disabled .  DMX is enabled/disabled from this single flag.

#define DART_DMX_MAX_CHANNELS       127 // info ctrl-f #DMX_LIMITS_DART // Nota DART: l'editor puo' impostare indirizzi solo fino a 127,

#define FAST_FEEDBACK               1   // 1 = enable fast MIDI->LED/DMX 'ONLY' modes (data_MODE 35/36); 0 = disable /*===== explanation CTRL-F: FAST_FEEDBACK =====


#define STAGED_BUTTON_READ_TEST     1   // 1 = enable / 0 = disable

// CTRL-F: VELO_PAD_RELEASE_FILTER
// Solo push_buttons(1): conferma OFF dopo N scansioni consecutive sopra upper_val.
// 0 = comportamento storico; 1..127 = scansioni (NON millisecondi).
// Riusa data_LB[]: nessun array o timer aggiuntivo. ON e velocity restano immediati.
#define VELO_PAD_RELEASE_SCANS      3
#if (VELO_PAD_RELEASE_SCANS < 0) || (VELO_PAD_RELEASE_SCANS > 127)
  #error "VELO_PAD_RELEASE_SCANS must be between 0 and 127"
#endif


// touch sensor section - explanation ANCHOR: DART_TOUCH_SENSOR_OVERVIEW  
#define touch_invert                0   // touch_invert: 1 inverts logic (active-LOW becomes active-HIGH internally).
#define touch_pullup                0   // touch_pullup: 1 enables INPUT_PULLUP (for buttons to GND / open-drain outputs).
#define touch_led_onboard           0   // LED indicator on pin 8 (SAFE only when Touch_sensors_enable == 2 / external touch on pins 7&9)
#define TOUCH_AVERAGE_ENABLE        0   // 1 = legacy 3-sample averaging; 0 = raw touch readings (decay already provides good stability)
#define Touch_sensors_enable        1   // [TOUCH_FLAGS]
                                        // 0 = OFF
                                        // 1 = internal CapacitiveSensor
                                        // 2 = external digital pins 7&9
                                        // 3 = external digital pins 7&8
                                        // For external digital touch inputs (Touch_sensors_enable = 2 or 3):


#define Page_switch                 1   // 1 = enabled // 0 = disabled // page_switch
#define page_LEDs                   0   // 1 = page LEDs active

#define Distance_sensor             0   // disable distance sensor functions if not used, to save flash memory
#define Seq_                        0   // disable seq control function
#define Piezo_pads                  0   // 1 = enable piezo_pads() on A5 when GENERAL settings say "pads present"; 0 = completely disable piezo pads logic
#define Extraplexer                 0   // 1= abilita Extraplex - possibile leggere altri 8 input extra utilizzando il pin 9 di Arduino (se si usano le funzioni touch interne, per usare Extraplex si deve rinunciare al secondo touch sensor)
#define Scale                       0   // 1= abilita funzione Scale - permette di registrare e suonare una scala di note tramite spinners o beam.


#define hid_keys                    1   // 1 = enabled

// mouse emulation section - //  explanation ANCHOR: DART_MOUSE_AND_BOOSTAX_EXPLANATION
#define hid_mouse                   1   // 1 = enabled
#define ENABLE_BOOSTAX              1   // 1 = prograssive mouse movement accelerator active ; 0 = excluded //  ANCHOR: DART_MOUSE_AND_BOOSTAX_EXPLANATION
#define mouse_block                 0   // 1 = enabled // 0 = disabled // mouse messages are stopped after 2 seconds of repeated activity
#define arrows_block                0   // 1 = enabled // 0 = disabled // arrow key messages are stopped after 2 seconds of repeated activity


#define blinker                     1   // blink effect on a selected led depending on pot position

//---------------------------

#define note_off                    0   // 1 = enabled // 0 = disabled // note OFF messages on button release - if NOTE Type has been selected
#define pullups_active              1   // 1 = enabled // 0 = disabled // pullup resistors -


#define MIDI_thru                   0   // 1 = MIDI Thru active


#define MIDI_IN_block                0   // 1 = MIDI IN blocked
#define MIDI_OUT_block               0   // 1 = MIDI out blocked

#define MIDI_OUT_SAFE_OFFSET         1   // 1= attiva // sistema di sicurezza per OFFSET // offset sarebbe il nuovo nome per la vecchia funzione SHIFTER , un surrogato di PAGE-switch


// ============================================================
// ENCODER INPUT CLEANING / STABILITY FILTERS
//
// Detailed explanation:
// See DART_encoder_filtering.md or d_cng.ino
//
// Why these options exist:
//
// Mechanical and optical encoders can sometimes generate:
// - double steps
// - wrong-direction steps
// - noisy transitions
//
// DART provides four  tools:
//
// MAJORITY
// Reads the encoder several times and keeps the most frequent result.
// Useful when the signal itself is noisy.
//
// ADAPTIVE
// CPU-saving version of Majority.
// Skips the full filtering process when the encoder appears still.
//
// LOCKOUT
// After a valid step, ignores new steps for a short time.
// Very effective against double clicks and bounce.
//
// WINDOW
// Executes Majority only for a short time after movement is detected.
// Reduces CPU usage while keeping good protection during motion.
//
// Structure:
//
// SIGNAL FILTERING
//
//                 MAJORITY
//                 /      \
//          ADAPTIVE      WINDOW
//
//
// TEMPORAL FILTERING
//
//               LOCKOUT
//
// 
// These tools are optional.
// For many encoders the default settings are already sufficient.
//
// Disable everything:
// ENABLE_ENC_MAJORITY = 0
// ENABLE_ENC_LOCKOUT  = 0
//
// ============================================================


// ---------- MAJORITY FILTER ----------
//
// Reads the encoder multiple times and keeps the most common state.
// Higher values = stronger cleaning.
// Lower values = faster response.
//

#define ENABLE_ENC_MAJORITY         0   // 1 = Majority ON, 0 = single raw reading
#define ENC_MAJ_SAMPLES            16   // number of readings used by Majority (9..21 typical)
#define ENC_MAJ_USDELAY            20   // microseconds between readings (10..60 typical)


// ---------- ADAPTIVE MAJORITY ----------
//
// Optional CPU optimization.
//
// If the encoder appears still, the full Majority burst is skipped.
// This saves processing time but does NOT increase filtering quality.
//

#define ENABLE_ENC_MAJ_ADAPTIVE     0   // 1 = Adaptive ON
#define ENC_MAJ_PRECHECK            3   // number of quick pre-readings (2..5 typical)
#define ENC_MAJ_PRE_USDELAY         0   // microseconds between pre-readings


// ---------- LOCKOUT FILTER ----------
//
// After a valid encoder step,
// ignore new steps for a short time.
//
// Very useful against:
// - double steps
// - opposite-direction glitches
// - contact bounce
//

#define ENABLE_ENC_LOCKOUT          1   // 1 = Lockout ON
#define ENC_LOCKOUT_US            800   // lockout duration in microseconds (400..1500 typical)


// ---------- TOP SPINNER LOCKOUT ----------
//
// Dedicated lockout for the main spinner interrupt routine.
//
// Normally it uses the same value as ENC_LOCKOUT_US.
// Advanced users may assign a different value.
//
// Optical spinners often work well with:
// 20..100 us
//
// Mechanical spinners usually need:
// 300..1200 us
//

#define ENC_TOP_LOCKOUT_US  ENC_LOCKOUT_US


// ---------- WINDOW MODE ----------
//
// Executes Majority only for a short period after movement.
//
// Useful only when ENABLE_ENC_MAJORITY = 1.
//
// Reduces CPU load while keeping filtering active during motion.
//

#if (ENABLE_ENC_MAJORITY == 1)

    #define ENABLE_ENC_MAJ_WINDOW   0   // 1 = Majority only during movement window
    #define ENC_MAJ_WINDOW_US   25000   // window duration in microseconds (15k..40k typical)

#endif


#define top_spinner                 1   // 1 = enabled // 0 = disabled // TOP SPINNER
#define Side_spinner                1   // 1 = enabled // 0 = disabled // SIDE SPINNER

#define MIDI_OUT_CLEANER             0   // CTRL-F: MIDI_OUT_CLEANER_FLAG  // 1 = abilita il "cleaner" software per encoders capricciosi (filtra msg uguali che cambiano solo databyte2)


// ........................................pots e analogread
#define Fast_analogread             1   // hi-speed analog read function - lower definition = 0...255
#define Dummy_read                  1   // lettura stabilizzata per analogread - si fanno due letture e si scarta la prima - per stabilizzare il S&H
#define Dummy_read_digi             1   // stabilized digital reading  -  explanation ANCHOR: DUMMY_READ_DIGI_HISTORY
#define Dummy_pullups               0   // 1 = abilita ON/OFF pullup durante Dummy_read, 0 = lascia pullup sempre come sono
#define ENABLE_POT_HIPREC_WINDOW    1   // 1=usa finestra (diff dinamico + timer), 0=solo diff_unlock fisso

#define POT_EMA_ENABLE             1    //[POT_EMA_SMOOTHING]  (dev) - "condensatore digitale" su letture analogiche (0..255)
#define POT_EMA_SHIFT              3    // [POT_EMA_SMOOTHING]  (dev) - "condensatore digitale" su letture analogiche (0..255)
#define POT_EMA_INPUT_COUNT       48    // 6 x 4051 analogici; Extraplexer riservato ai pulsanti
#define ENABLE_POT_TAKEOVER        1    // 1 = enable pot takeover (Page1/Page2 + ARMED/CAUGHT gate)
                                        // 0 = legacy behavior (no takeover, no data_LB split)


// matrix pads section-------------------- Complete documentation for this section will be released alongside the code. Therefore, the use of Spirtes and LED matrices is not yet covered."

#define monitor_temporizzato        0   // ogni 300ms viene monitorizzata la variabile valore - possibile modificare per monitorare altre cose


// -----------------------------------------------------------------------------
//   END OF STANDARd PROFILE - 1
// -----------------------------------------------------------------------------


#if (DART_PROFILE == 3)  // === KOROVA/ONE (profile 3) ===

    #include "DART_profile_undefs.h"

    // === KOROVA/ONE (profile 3) ===
    #define ENABLE_POT_TAKEOVER         0   // 1 = enable pot takeover (Page1/Page2 + ARMED/CAUGHT gate)
    #define ENABLE_AUTODETECT           0   // 1 = enable AUTODETECT mode (when no valid preset is found in EEPROM)


    #define shifter_active              1    
    #define LED_pattern                 2  // LED wiring table: 2 = factory wiring, 0 = alternative wiring
    #define DMX_active                  0   // 1 = enabled ; 0 = disabled .  DMX is enabled/disabled from this single flag.
    #define DART_DMX_MAX_CHANNELS       127 // info ctrl-f #DMX_LIMITS_DART // Nota DART: l'editor puo' impostare indirizzi solo fino a 127,
    #define touch_invert                0   // touch_invert: 1 inverts logic (active-LOW becomes active-HIGH internally).
    #define touch_pullup                0   // touch_pullup: 1 enables INPUT_PULLUP (for buttons to GND / open-drain outputs).
    #define touch_led_onboard           0   // LED indicator on pin 8 (SAFE only when Touch_sensors_enable == 2 / external touch on pins 7&9)
    #define Touch_sensors_enable        1   // [TOUCH_FLAGS]
    #define top_spinner                 1   // 1 = enabled // 0 = disabled // TOP SPINNER
    #define Side_spinner                1   // 1 = enabled // 0 = disabled // SIDE SPINNER
    #define Page_switch                 1   // 1 = enabled // 0 = disabled // page_switch
    #define Distance_sensor             0   // disable distance sensor functions if not used, to save flash memory
    #define Seq_                        1   // disable seq control function
    #define Piezo_pads                  0   // 1 = enable piezo_pads() on A5 when GENERAL settings say "pads present"; 0 = completely disable piezo pads logic
    #define Extraplexer                 1   // 1= abilita Extraplex - possibile leggere altri 8 input extra utilizzando il pin 9 di Arduino (se si usano le funzioni touch interne, per usare Extraplex si deve rinunciare al secondo touch sensor)
    #define Scale                       0   // 1= abilita funzione Scale - permette di registrare e suonare una scala di note tramite spinners o beam.
    #define hid_keys                    1   // 1 = enabled
    #define hid_mouse                   1   // 1 = enabled
    #define ENABLE_BOOSTAX              1   // 1 = progressive mouse movement accelerator active; 0 = excluded

    #define blinker                     1   // blink effect on a selected led depending on pot position
    #define note_off                    0   // 1 = enabled // 0 = disabled // note OFF messages on button release - if NOTE Type has been selected
    #define pullups_active              1   // 1 = enabled // 0 = disabled // pullup resistors -
    #define page_LEDs                   0   // 1 = page LEDs active
    #define MIDI_thru                   0   // 1 = MIDI Thru active
    #define mouse_block                 0   // 1 = enabled // 0 = disabled // mouse messages are stopped after 2 seconds of repeated activity
    #define arrows_block                0   // 1 = enabled // 0 = disabled // arrow key messages are stopped after 2 seconds of repeated activity
    #define encoders_                   1   // 1 = enabled
    #define MIDI_IN_block                0   // 1 = MIDI IN blocked
    #define MIDI_OUT_block               0   // 1 = MIDI out blocked
    #define MIDI_OUT_CLEANER             0   // 1 = abilita il "cleaner" software (filtra msg uguali che cambiano solo databyte2)
    #define FAST_FEEDBACK               1   // 1 = funzioni midi feedback veloce LED-only e DMX-only attive
    // [ENC_FILTER_OVERRIDES] (majority / adaptive / lockout)
    #define ENABLE_ENC_MAJORITY         0   // 1 = majority burst ON, 0 = lettura singola
    #define ENC_MAJ_SAMPLES             16  // letture nel cluster (es. 9..21)
    #define ENC_MAJ_USDELAY             20  // us tra letture (es. 10..60)
    #define ENABLE_ENC_MAJ_ADAPTIVE     0   // 1 = pre-check adattivo ON
    #define ENC_MAJ_PRECHECK            3   // pre-letture veloci (2..5)
    #define ENC_MAJ_PRE_USDELAY         0   // us tra pre-letture (0..5)
    #define ENABLE_ENC_LOCKOUT          1   // 1 = lockout ON
    #define ENC_LOCKOUT_US              800 // us lockout generale (es. 400..1500)
    #define ENC_TOP_LOCKOUT_US          ENC_LOCKOUT_US  // lockout TOP (puoi separarlo)
    #define Fast_analogread             1   // hi-speed analog read function - lower definition = 0...255
    #define Dummy_read                  1   // lettura stabilizzata per analogread - si fanno due letture e si scarta la prima - per stabilizzare il S&H
    #define Dummy_pullups               0
    #define ENABLE_POT_HIPREC_WINDOW    1
    #define POT_EMA_ENABLE             1    // EMA configurabile per profilo
    #define POT_EMA_SHIFT              3    // smoothing: y += (x-y) >> shift
    #define POT_EMA_INPUT_COUNT       48    // ingressi analogici reali: 6 x 4051
    #define monitor_temporizzato        0   // ogni 300ms viene monitorizzata la variabile valore - possibile modificare per monitorare altre cose


#else

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
    #define ENABLE_ENC_MAJORITY         0   // 1 = majority burst ON, 0 = lettura singola
    #define ENC_MAJ_SAMPLES             16  // letture nel cluster (es. 9..21)
    #define ENC_MAJ_USDELAY             20  // us tra letture (es. 10..60)
    #define ENABLE_ENC_MAJ_ADAPTIVE     0   // 1 = pre-check adattivo ON
    #define ENC_MAJ_PRECHECK            3   // pre-letture veloci (2..5)
    #define ENC_MAJ_PRE_USDELAY         0   // us tra pre-letture (0..5)
    #define ENABLE_ENC_LOCKOUT          0   // 1 = lockout ON
    #define ENC_LOCKOUT_US              800 // us lockout generale (es. 400..1500)
    #define ENC_TOP_LOCKOUT_US          ENC_LOCKOUT_US  // lockout TOP (puoi separarlo)

#endif

// -----------------------------------------------------------------------------
#if (DART_PROFILE == 2)
// CUSTOM starts with every STANDARD setting above.
// To change only this profile, override the settings you need here.
// Example: uncomment both lines to disable USB keyboard output.
// #undef hid_keys
// #define hid_keys 0
#endif

// CTRL-F: MIDIFEEDBACK_REFACTOR_FLAGS
// User-selectable MIDI feedback architecture. These flags are intentionally
// independent from DART_PROFILE and from the selected microcontroller.
//
// USE_REFACTORED_MIDIFEEDBACK
//   1 = new single-dispatcher implementation (active in this backup)
//   0 = legacy implementation kept as compile-time rollback
//
// ENABLE_DUAL_PAGE_FEEDBACK
//   1 = scan and update both pages; adds data_MODE_OTHER[60] (60 bytes RAM)
//   0 = scan only the active page; smaller and simpler
// -----------------------------------------------------------------------------
#ifndef USE_REFACTORED_MIDIFEEDBACK
  #define USE_REFACTORED_MIDIFEEDBACK 1
#endif

#ifndef ENABLE_DUAL_PAGE_FEEDBACK
  #define ENABLE_DUAL_PAGE_FEEDBACK 1
#endif

#define case_6 0


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


////////////////////////////////////////////////////////////////////// porting works area

// (M0 port) Need fixed-width integer types even before Arduino.h is included
#include <stdint.h>

#if defined(ARDUINO)
#include <Arduino.h>
#endif


// ===== CTRL-F: M0_ADC_PINMAP =====
// DART usa spesso indici numerici (0..5) per i 6 ingressi analogici collegati ai 4051.
// Su AVR (Leonardo) analogRead(0..5) corrisponde storicamente ai canali A0..A5.
// Su SAMD (Metro M0) questa equivalenza NON è garantita: usiamo una tabella che mappa 0..5 -> A0..A5.
#if defined(ARDUINO_ARCH_SAMD)
  static const uint8_t DART_ADC_PIN[6] = { A0, A1, A2, A3, A4, A5 };
  #define DART_ADC_PIN_FROM_PLEXER(i) (DART_ADC_PIN[(uint8_t)(i)])
#else
  #define DART_ADC_PIN_FROM_PLEXER(i) ((uint8_t)(i))
#endif

// ===== CTRL-F: M0_USB_MIDI_SWITCHES =====
// Switch M0/SAMD only: all these defines are used only inside
// #if defined(ARDUINO_ARCH_SAMD) structural blocks.
//
// DART_M0_MIDI_SEND_MODE
// 0 = invio USB-MIDI packet a 4 byte (vecchia tecnica)
// 1 = invio raw a 3 byte (status, data1, data2)
//     allineato allo sketch di test M0 che ha eliminato i messaggi fantasma.
#ifndef DART_M0_MIDI_SEND_MODE
  #define DART_M0_MIDI_SEND_MODE 1
#endif

// DART_M0_USB_MIDI_FEEDBACK
// 0 = non leggere il MIDI IN USB sulla M0
// 1 = leggi i byte ricevuti da usb_midi e passali a midifeedback()
#ifndef DART_M0_USB_MIDI_FEEDBACK
  #define DART_M0_USB_MIDI_FEEDBACK 1
#endif

// DART_M0_MIDI_HEARTBEAT
// Heartbeat di debug disattivato di default nel DART vero, per allinearlo
// allo sketch di test stabile ed evitare traffico MIDI inutile durante i test.
#ifndef DART_M0_MIDI_HEARTBEAT
  #define DART_M0_MIDI_HEARTBEAT 0
#endif

////////////////////////////////////////////////////////////////////// porting works area - end


#endif // DART_CONFIG_H
