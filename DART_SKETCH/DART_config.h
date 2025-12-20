#ifndef DART_CONFIG_H



#define DART_CONFIG_H



// Global compile-time configuration for DART firmware
// NOTE: for now this file centralizes only the DMX_active flag.

// DART feature flags migrated from DART_SKETCH.ino (lines 21-53)
// Changing them here affects the whole firmware.
#define shifter_active    1         // 1 = enabled // 0 = disabled // SHIFT REGISTERS_ // if enabled, Matrix_pads must be disabled
#define stratos  0                   // 1 = enabled // 0 = disabled // Stratos sketch version.
#define LED_pattern 2                // 0 = dart one // 1 = kombat personal // 2 nb boards - Led animation pattern used by buttons and pots

#define DMX_active      0            // 1 = enabled ; 0 = disabled .  DMX is enabled/disabled from this single flag.
#define DART_DMX_MAX_CHANNELS 127   // info ctrl-f #DMX_LIMITS_DART //  Nota DART: l’editor può impostare indirizzi solo fino a 127,

#define capacitivesensor_active 2    // 1 = enabled // 0 = disabled  EXTERNAL TOUCH IC (inverted) on pin 7 - 9--- CAPACITIVE SENSORS_
#define touch_version 1              // 1 = 680k //  2 = 10m //     resistor settings for touch sensing circuit
#define touch_led_onboard 1          // 1 = led output  on arduino pin 8 // 0 = disabled

#define Matrix_Pads      0           // 1 = enabled // 2 = pads on 17-32 circuitposition (old)// 0 = disabled // max7219 chips
#define MATRIX_CROSS_FX  1           // 1 = effetto croce attivo, 0 = disattivato
#define MATRIXFX_MODE 1              // Modalità effetto "croce" sui pad matrice // 0 = fullwhite (classico) ; 1 = XOR con lo sfondo (status[96])

#define hid_keys 1                   // 1 = enabled
#define hid_mouse 1                  // 1 = enabled
#define top_spinner 1                // 1 = enabled // 0 = disabled // TOP SPINNER
#define side_spinner 1               // 1 = enabled // 0 = disabled // SIDE SPINNER
#define note_off 0                   // 1 = enabled // 0 = disabled // note OFF messages on button release -  if NOTE Type has been selected
#define pullups_active 1             // 1 = enabled // 0 = disabled // pullup resistors
#define page_active 1                // 1 = enabled // 0 = disabled //  page_switch
#define page_LEDs 0                  // 1 = page LEDs active
#define MIDI_thru 0                  // 1 = MIDI Thru active
#define mouse_block 1                // 1 = enabled // 0 = disabled // mouse messages are stopped after 2 seconds of repeated activity
#define arrows_block 0               // 1 = enabled // 0 = disabled // arrow key messages are stopped after 2 seconds of repeated activity
#define encoders_generic 1           // 1 = enabled
#define MIDI_IN_block 0              // 1 = MIDI IN blocked
#define MIDI_OUT_block 0             // 1 = MIDI out blocked
#define LED_rings 0                  // 1 = LED rings active - experimental - leave it to 0
#define blinker 1                    // blink effect on a selected led depending on pot position
#define Fast_analogread 1            // 
#define led_eco 0                    // 
#define PIEZO_PADS_ACTIVE  0         // 1 = enable piezo_pads() on A5 when GENERAL settings say "pads present"; 0 = completely disable piezo pads logic

#define PAD_VELO_DEBUG  0            // 



#endif // DART_CONFIG_H
