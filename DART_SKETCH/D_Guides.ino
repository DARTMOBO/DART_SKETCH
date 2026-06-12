

 
 /*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */



/*
DART_SKETCH – quick overview

This comment serves as a quick mental map of the sketch,
to realign in a few minutes without having to re-study it from scratch.
conceived especially for AI analysis.

The idea is:

remember how the “ITEMS” (memory slots) are organized,

understand the main flow (loop ? AIN ? ain_nucleo ? modules),

keep in mind how LEDs, MIDI, editor, EEPROM and hardware variants work.



General architecture: ITEM and shared tables

Item is a fundamental concept,
mainly used by dart_editor, which is an external software to this sketch and
works both online via browser and as a standalone in old versions;
from a computer science point of view we need to know that an item is a group of information, stored as bytes,
that serve, when read and used by this sketch, to regulate HOW an electrical reading of a given input will be translated into a MIDI message.

Each ITEM is indexed by “chan” / “mempos” and uses a series of RAM tables (i.e. common byte arrays):
• data_TY[] – message type (note, CC, PC, aftertouch…) / + other possible uses
• data_VA[] – main value (note pitch, CC number, etc.) / + other possible uses
• data_MA[] – upper limit of the databyte2 of a MIDI message / + other possible uses
• data_MI[] – lower limit of the databyte2 of a MIDI message / + other possible uses
• data_LT[] – indication of the output related to that ITEM, generally one output controls one LED / + other possible uses
• data_QW[] – indication of the ascii character number linked to the current ITEM, important when HID emulation is active / + other possible uses
• data_MODE[] – operating mode - this array has a FIXED function: to direct,
                 in void ain_nucleo(), the reading of an input,
                 stored in the variable "valore" towards a translation void contained in the d_mod.ino or e_mod.ino table

Recap with an example:
data_MODE[chan] tells me
* whether a given input will be read digitally (faster) or analogically, if it is a button it will be read digitally
* to which "interpretation" void to direct the result of the reading, if it is a button everything will be directed to push_button()
the other variables/tables (data_TY[], data_VA[] etc.) are used by push_button() to translate "valore" into a MIDI message according to the rules valid for the nature of the input/ITEM currently being processed.

** Setup_mempos() handling **
There are “service” ITEMs used as containers for machine setting information, NOT used to translate an input into a MIDI message:
• general_mempos – general controller settings slot - fundamental, must always be present in a preset, the editor forces at least one ITEM to be set to contain general info.
• mouse_mempos – slot associated with mouse emulation (if active),
• page_mempos – slot controlling page A/B state
• touch_mempos[] – memory slots associated with touch sensors
• piezo_pads_mempos[] – piezoelectric pad handling
• distance_sensor_mempos[] – distance sensor handling
• spinner_mempos[] – high resolution encoder handling, called Spinner 1 and 2
the content of these “service” index variables is loaded at machine startup so that the program always knows where to read general parameters.

The MIDI editor works on these same 60 ITEMs: it reads from a csv file and writes the tables, sending them to the controller via a stream of MIDI messages, not sysex.
according to a protocol designed to work around problems related to Arduino libraries without sysex support.

the concept of MEMORYPOSITION is very important in this context:
each ITEM, inside a dart_editor preset, is "tagged" with a unique memory position number.
this unique number is then what correctly matches a specific input on the dart_mobo pcb to a specific memory slot indexed via "chan".
note: during preset transfer, a specific remap of memorypositions occurs - see remap()
      this is done to make the progressive numbering of ITEMs in the editor perfectly correspond to the numbering of inputs on the pcb, as the human eye reads them sequentially.
      in fact this visual sequence does not correspond to the inputs of the 74hc4051 multiplexers used on the dart_mobo
----------------------------
.

Main loop: loop()

The main loop (D_loop.ino) coordinates all parts of the sketch:
• update of internal counters (e.g. cycletimer) mainly used for light effects - see buttonledefx();
• management of general blinks (blinker)
• scanning of physical inputs:
- AIN() – classic DART version (stratos == 0), or AIN_stratos() – version for the STRATOS controller (stratos == 1)
      * call to the logical core: ain_nucleo() – translates the read value into behavior (button, pot, encoder, etc.) and MIDI effect.
      * management of possible mouse functions (mouse_control) if enabled by editor
• incoming MIDI management (D_MIN.ino) with related LED update – therefore listening to serial ports via DIN and USB and possible reaction with void midifeedback();
• LED update: with void buttonledefx(); or matrixbuttonledefx();
      - via shifter (D_LED.ino) if shifter_active == 1
      - via MAX7219/matrices (D_mtrx.ino) if Matrix_Pads == 1
• page change management (PAGE A/B)
      note on page: ain_nucleo() (mode 17) does not change the page: it only updates the pagestate;
      pageswitch() is called only once from loop() and, reading pagestate, performs the actual page change (load preset, ledrestore, etc.).


----------------------------------

Input scanning: AIN()

3.1 AIN() – classic DART version (stratos == 0)

It is located in D_INS.ino.

Scans the inputs coming from the 4051 (analog multiplexers).
For each cycle:
• iterates through “channel” (0–7 etc.) to read the plexer outputs
• uses analogRead_1024(plexer) to get a value 0–1024
• calculates “chan”: the logical slot connected to that plexer/pin
• the eventual AUTODETECT block can directly use this reading
to understand if the channel behaves like a button or a pot.
• once reading is finished (the reading is stored in the variable Valore) it passes control to: ain_nucleo().

3.2 AIN_stratos() – STRATOS version (stratos == 1)
It is located in D_STRATOS_ain.ino.
Performs a different scan (up to 20 channels) suitable for the STRATOS pcb layout.
In the STRATOS model, instead of the 4051, a more “rudimentary” multiplexing system based on diodes and a couple of grounding lines is used,
to increase the number of usable inputs on the Arduino Pro Micro
(which is the hardware basis of the smaller DART).

-----------------------------------------------------

Logical core: ain_nucleo()

It is located in D_INS.ino.

Takes:
• chan – which ITEM we are updating
• data_MODE[chan] – what type of control it is (button, toggle, encoder, touch, mouse, etc.)
• the reading result, if performed, loaded into the variable Valore.

It is the great “dispatcher”: for each ITEM it decides which control module to call.

Simplified scheme (mode numbers are for reference):
• mode 0 – empty ITEM (no action)
• modes 1–10 – various button types:
- momentary, toggle, group toggle, radio groups, etc.
- delegates to push_buttons() in D_MOD.ino
• modes 11–15 – potentiometers / sliders:
- analog reading, smoothing, mapping to MIDI
- delegates to the pot function in D_MOD.ino
• mode 17 – page switch:
- uses page_mempos, data_QW[] and pagestate
- manages switching between PAGE A and PAGE B
• mode 18 – distance sensor
• mode 19 – generic encoders:
- delegates to encoder() in D_MOD_ENC.ino

for other data_MODE values, void ain_nucleo() does not call any function because some special devices are handled exceptionally in other areas of the code, some examples:
• modes 21–22 – spinner (top and side)
• modes 23–24 – touch sensor 1/2
• mode 25 – mouse
• mode 26 – general settings slot (general controller parameters)

Before these branches, ain_nucleo may call mouse_control()
if mouse_mempos is valid and mouse emulation is active.


-------------------------------------------------------------------------------

Control modules: D_MOD., D_MOD_ENC., D_MOD_mouse.*

5.1 “General” modules (D_MOD.ino)

Functions dedicated to basic control logic:

• push_buttons(velo)
- manages simple buttons, toggle, group toggle, radio.
- can calculate velocity (for pads) based on press time
and upper_val / lower_val thresholds.
- updates data_VA[] and data_LT[] according to button type.
note: button reading is equipped with optimization systems: see - software debounce (lastbutton_debounce).
note: sensorization and effect functions have been separated, in the latest versions of the sketch,
      to help diy users create custom functions related to button presses.
      see optional staged scanning (STAGED_BUTTON_READ_TEST).
note: we do not have dedicated velocity pad modules, the same module for mechanical buttons can be used in velocity sensitive mode.

• potentiometer/fader functions:
- read the filtered analog value
- apply ranges and limits from data_MI[] / data_MA[]
- update data_VA[] and send related MIDI messages
- update feedback LED state (in data_LT[]).
note: potentiometer reading is equipped with optimization systems that can be turned on or off from config:
      see #define POT_EMA_ENABLE 1,
      high precision time window #define ENABLE_POT_HIPREC_WINDOW 1
note: it is possible to enable #define ENABLE_POT_TAKEOVER 1
note: it is possible to choose a faster analog reading mode, ideal for scan-based systems like dart_sketch - see Fast_analogread: fast 8-bit ADC, rescaled to ~10-bit




5.2 Modules for encoder and spinner (D_MOD_ENC.ino)

Main function: encoder(byte numero)
• interprets MSB/LSB variations and uses data_LB[numero] for direction (- / 0 / +)
• manages:
- generic encoders (mode 19)
- top/side spinner (modes 21/22) - these are called directly from ain() to increase their speed.
• uses tables like encodervaluepot[] for scale / pot emulation mode
• updates:
- the internal value (e.g. position)
- packages MIDI messages (increments, notes, CC, scales)
- prepares specific LED feedback for encoders -
note: the main encoder, called top_spinner, is read very quickly via interrupt, and is usually based on optical encoders.
note: to optimize encoder readings we have various systems that can be activated by choice - see #define ENABLE_ENC_LOCKOUT , #define ENABLE_ENC_MAJORITY ,  #define ENABLE_ENC_MAJ_ADAPTIVE, #define ENABLE_ENC_MAJ_WINDOW.
note: the side spinner is read inside ain() to increase its reading frequency, so it can be considered intermediate precision. generic encoders are read within the normal cycle that uses ain_nucleo().

we have so far covered the main "translation" modules, but there are still others, commented inside their void - see tabs d_mod.ino and e_mod.ino,


5.3 Mouse module (D_MOD_mouse.ino)

Main function: mouse_control()
• available on MCUs like ATmega32U4 (Leonardo/Micro, etc.)
• reads XY pots associated with mouse_mempos
• applies:
- deadzone around the center (to reduce jitter)
- accumulation and thresholds for smooth movements
- configurable speed from editor (data_DM[mouse_mempos])
- optional “Boostax” (if enabled via define) to increase speed
when the joystick remains fully tilted for a while.
• converts the result into real HID mouse movements (Mouse.move()).
All behavior is configurable via the mouse_mempos slot,
similarly to other ITEMs.
note: there is an optional module (see dart_config.ino) called BOOSTAX dedicated to mouse pointer acceleration.








Visual outputs: shifter, MAX7219, status LEDs

6.1 Shifter and MAX7219

The sketch can drive (not simultaneously):
• shift register (e.g. 74HC595) – via _DART_Shifter.*
• LED matrices based on MAX7219 – via DartLedControl.* and LedControl.*

The choice is controlled by some defines:
• shifter_active:
- 1 – activates shift register management
- 0 – excludes the shifter block
• Matrix_Pads:
- 0 – no active MAX7219
- 1 – activates MAX7219 matrices for pads / indicators
- 2 – particular and rare path, kept only for an old
management with force sensors coupled to LED matrices


6.2 LED effects and consistency with button state

LED management is designed to create non-destructive graphic effects:
when the effect ends, the LED “base” returns to clearly representing
the true state of buttons (toggle, groups, radio).

Functions like buttonledefx() and encledefx():
• draw small timed effects (flash, symmetrical expansions, etc.)
around the button or encoder that was used
• do this without permanently erasing the background map:
ledrestore(page) and internal logic take care of restoring the
situation consistent with data_LT[] and toggle states.

LEDs react both:
• to panel actions (press a key, turn an encoder…)
• to incoming MIDI messages from the outside, if mapped as feedback.

6.3 Status LEDs for touch, potentiometers and blinker

Some LEDs are dedicated to signaling:
• touch sensor status (active / virtual / test, etc.)
• blinker effect to have an LED blink when a POT is considered outside a "safe" range


----------------

MIDI IN, MIDI OUT and communication protocol with dart_editor

7.1 MIDI OUT (D_OUT.ino)

Here are all the functions that send messages to the host:

The output functions are called by the control modules (buttons, pots, encoders…). In parallel to the same event, you can also activate:
• eventual DMX outputs (if DMX_active == 1)
• eventual HID emulations (keyboard / mouse) if enabled.

The logical path is therefore unique: an action on the panel reaches an
“output core” that can generate MIDI, DMX and HID in parallel
according to the configuration.



7.2 MIDI IN (D_MIN.ino)

Manages MIDI messages coming from outside (DIN or USB ports):
• updates feedback LEDs (on shifter or matrix) - see midifeedback();
• can update various internal states related to ITEMs - position of a virtual pot (encoder in pot mode) or the unlock takeover position for a real pot.

In practice incoming messages can:
• turn pads on or off
• synchronize the controller state with the host state (software on pc, like traktor or ableton).
note: synchronization occurs independently for pages (PAGE 1 / PAGE 2),
      this is especially important for toggle button status;
      the two pages behave as if they were two independent controllers, always connected




7.3 Communication with the editor

The editor uses a MIDI protocol, not based on sysex but on common MIDI signals (CC, notes, AT),
to read/write presets in the sketch.

A special message opens/closes editor mode:
• openeditor = 1 – the firmware listens for preset data
• openeditor = 0 – return to normal mode

In editor mode:
• received data populates the tables (data_TY, data_VA,
data_MA, data_MI, data_MODE, data_LT, data_QW, etc.)
• presets contain settings for A/B pages (PAGE 1 / PAGE 2) of the 60 ITEMs.



EEPROM, presets and A/B pages

The _DART_EEPROM.h library manages EEPROM reads/writes.

D_STP_Presets.ino contains functions for:
• loading presets from EEPROM at startup
• setting mempos - see setup_mempos() - see "** Setup_mempos() handling **"

Logical structure of EEPROM memory per page:
• there are two operating pages A/B for each of the 60 ITEMs -
    but only what is necessary for the complete operation of the current page
    and the minimum needed for synchronizing the inactive page is kept in RAM - see separation between load_preset_base() and load_preset().
    at each page change we have an EEPROM read access. which adds to the primary access that occurs at startup.


If no valid preset is found in EEPROM at startup:
• eeprom_preset_active remains 0
• AUTODETECT mode is activated (...after aux_preset() has built the emergency RAM preset) 


 how Preset upload via MIDI -> EEPROM writing (operational summary)
   [CNG_ADDENDUM_PRESET_MIDI_EEPROM]  (2026-02-11)

   Where it really happens:
   - Reception / parsing + EEPROM writer: D_MIN.ino
   - Preset reading from EEPROM (boot and refresh): D_STP_Presets.ino

   1) Enter/exit "editor upload" mode
   - A special byte (241 / 0xF1) toggles `openeditor`.
     * openeditor=1  -> received MIDI messages are interpreted as preset data
     * openeditor=0  -> exit upload and reload tables from EEPROM:
                       reset_mempos(); load_preset_base(); load_preset(page);

   2) When writing to EEPROM
   - During openeditor=1, when a "complete" MIDI message arrives (type + note + velocity),
     if `type < 208`, `eeprom_write()` is called which immediately executes EEPROM.write(...).
   - Pitch Bend (type 224) does NOT write immediately: it updates `matrix_vert1/2` which can
     influence some bits during writes (bit7).

   3) Transfer structure (sequence)
   - The editor sends a sequence of messages for the same ITEM (memoryposition).
   - `eeprom_write()` uses `editorcounter` (switch case 0..6) to know which “piece” is arriving.
   - A NOTE (type < 160) is treated as block start: resets `editorcounter = 0`.

   4) EEPROM Layout (concept)
   - Data is stored in contiguous tables (type/value/mode/dmx/min/max/qwerty/light/…).
   - PAGE 2 uses a base offset of +512 compared to PAGE 1.
     (in the writer there are also compensations like “-64” for some memoryposition ranges,
      see comments in code: it is part of historical mapping compatibility).

   Note: this block describes the flow; for exact details (addresses, cases, remap, user bytes)
   refer directly to `eeprom_write()` in D_MIN.ino.
   Note: as already explained, the remapper() function is used to align the progressive numbering of items in the editor with that of the inputs on the 74hc4051 pins on the dart_mobo pcb

   =====================================================================


-----------------------


Hardware variants, NOMOBO and AUTODETECT mode

The sketch is designed to work on multiple controllers using a single code:
• define stratos:
- 0 – classic DART version with 4051 and standard layout
- 1 – STRATOS version, based on Pro Micro with diode multiplexing
• define Matrix_Pads:
- 0 – no MAX7219 matrix
- 1 – active matrices for pads/indicators
- 2 – historical variant for an old management with force sensors
and matrices (rare use, kept for compatibility)


NOMOBO mode:
• when data_VA[general_mempos] is non-zero, the controller enters
a “reduced” NOMOBO mode.
• in this mode, mainly the basic pin set of the Arduino Uno/Leonardo is used,
without relying on all the complexity of shifters, matrices and extra sensors.


AUTODETECT mode:
• if no preset is found in EEPROM at startup
(eeprom_preset_active == 0), the sketch enters AUTODETECT mode.
• in this mode:
- AIN() directly observes the electrical behavior of the inputs:
buttons: readings that drop below the threshold when pressed
potentiometers: “stable” and continuous values across the range
- based on these observations, the sketch:
- chooses an appropriate mode (button / pot)
- sets thresholds, min/max and sensible internal parameters
- automatically assigns “basic” MIDI messages.
• Result: even without having loaded a preset from the editor, a correctly wired controller can start working “immediately” in a reasonable way, as soon as the sketch is uploaded. the outgoing midi messages are limited to NOTE for digital controls and CC for continuous controls. (encoders cannot be "seen" by autodetect)

// End of overview
// This block only serves as a high-level mental map.
// To add new functions it is convenient to:
// - start from ain_nucleo() (new mode),
// - connect the module in D_MOD.* / D_MOD_ENC.* / D_MOD_mouse.*,
// - possibly extend LEDs in D_LED.ino / D_mtrx.ino,
// - and, if necessary, adapt preset/editor in D_STP_Presets.ino and D_MIN.ino.
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
#DMX_LIMITS_DART
------------------------------------------------------------
DART – DMX QUICK REFERENCE (limits & logic overview)
------------------------------------------------------------

1. Why 127 channels?
   The DART editor currently allows DMX addresses only in range 1–127.
   To save RAM on the ATmega32u4, the DMX buffer is intentionally
   reduced from 512 → 127 channels. This is fully adequate for DART's
   creative/utility DMX use.

2. Global DMX size control
   In config.h:
       #define DART_DMX_MAX_CHANNELS 127
   This value defines the actual DMX universe size used by the firmware.
   DMX_SIZE is now tied to this define. To expand (e.g., 256 or 512),
   simply change the number and recompile.

3. Library-level safety
   dmxWrite() only accepts channels 1..DMX_SIZE.
   - Valid channel  = written normally into dmxBuffer[channel-1].
   - Above limit    = ignored safely (no crash, no memory breach).

4. The “maxChannel(64)” confusion
   The setup still calls:
       DmxSimple.maxChannel(64);
   This is *not* a hard cap. It only sets the initial “dmxMax”.
   When any item writes to a higher channel (e.g., 70), the library
   automatically extends dmxMax up to that value, as long as it is
   ≤ DMX_SIZE (127). This is why channels >64 still work.

5. Practical effect
   - Editor assigns channels 1..127 → firmware handles them safely.
   - Anything >127 → ignored without affecting system stability.
   - Reducing buffer size frees ~400 bytes of RAM.

6. Future expansion
   To restore larger universes:
     1) Edit DART_DMX_MAX_CHANNELS in config.h
     2) Recompile
     3) (Optional) update the editor UI to allow >127

------------------------------------------------------------
End of DMX quick reference.
#DMX_LIMITS_DART
*/


/*
 * 
 * 
STRUTTURA EEPROM – SITUAZIONE ATTUALE (DART_SKETCH)
approfondimento

Questo commento riassume come sono organizzate in EEPROM le tabelle
principali (data_TY, data_VA, data_MODE, ecc.) nelle due pagine
di preset. Serve come “mappa fisica” per capire cosa c’è dove, prima
di mettere mano al layout o riciclare la PAGE 2 per altre funzioni.

NOTE GENERALI

MCU tipica: ATmega328 / ATmega32u4 con 1024 byte di EEPROM.

DART divide la EEPROM in 2 blocchi da 512 byte ciascuno:
• BLOCCO 0: indirizzi 0–511 → PAGE 1
• BLOCCO 1: indirizzi 512–1023 → PAGE 2

Ogni blocco contiene lo stesso tipo di dati, organizzati in “fasce”
da 64 byte per ciascuna tabella (type, value, mode, dmx, min, max,
qwerty, led).

max_modifiers = 60 → la parte usata in ogni fascia va da 0 a 59,
i byte rimanenti (60–63) sono di fatto “spazio di manovra”.

TABELLE PRINCIPALI (LOGICA)

Per ogni ITEM (0–59) e per ogni pagina (PAGE 1 / PAGE 2) abbiamo:

data_TY[] → tipo messaggio + canale MIDI (note/cc/pc/at, ecc.)

data_VA[] → valore di base (nota, CC number, ecc.)

data_MODE[] → mode (button, pot, encoder, toggle, ecc.)

data_DM[] → canale/parametro DMX (0 = DMX disattivo)

data_MI[] → minimo o parametro extra (soglia bassa, range, ecc.)

data_MA[] → massimo o parametro extra (soglia alta, range, ecc.)

data_QW[] → extra vari (qwerty-mode, flag interni, ecc.)

data_LT[] → mappa/indirizzo LED + opzioni luce

In RAM, per la PAGE 2, si usano spesso gli offset +max_modifiers:

data_TY[i+max_modifiers] → tipo pagina 2

data_VA[i+max_modifiers] → valore pagina 2
mentre data_MODE/min/max/dmx/qwerty/data_LT restano accessibili
tramite la variabile globale "page" (0 o max_modifiers).

MAPPA EEPROM – BLOCCO 0 (PAGE 1)

Indirizzi 0–511 → contenuto PAGE 1.

Per i cicli con i = 0..59 (max_modifiers):

0–59 : data_TY PAGE 1
data_TY[i] = EEPROM.read( i );

64–123 : data_VA PAGE 1
data_VA[i] = EEPROM.read( i + 64 );

128–187: data_MODE PAGE 1
data_MODE[i] = EEPROM.read( i + 128 + (0*512) );

192–251: data_DM PAGE 1
data_DM[i] = EEPROM.read( i + 192 + (0*512) );

256–315: data_MI PAGE 1
data_MI[i] = EEPROM.read( i + 256 + (0*512) );
(in alcuni casi remapper() viene applicato a questi valori)

320–379: data_MA PAGE 1
data_MA[i] = EEPROM.read( i + 320 + (0*512) );

384–443: data_QW PAGE 1
data_QW[i] = EEPROM.read( i + 384 + (0*512) );

448–507: data_LT PAGE 1
data_LT[i] = EEPROM.read( i + 448 + (0*512) );

60–63, 124–127, 188–191, 252–255, 316–319, 380–383, 444–447, 508–511:
zona non utilizzata in modo strutturale (piccoli “cuscinetti” tra le fasce).

Riassunto blocco 0:
0– 59 data_TY (PAGE 1)
64–123 data_VA (PAGE 1)
128–187 data_MODE
192–251 data_DM
256–315 data_MI
320–379 data_MA
384–443 data_QW
448–507 data_LT
508–511 attualmente non usato in modo critico

MAPPA EEPROM – BLOCCO 1 (PAGE 2)

Indirizzi 512–1023 → contenuto PAGE 2.

La struttura è identica, traslata di +512:

Per i = 0..59:

512–571 : data_TY PAGE 2
data_TY[i+max_modifiers] = EEPROM.read( i + 512 );

576–635 : data_VA PAGE 2
data_VA[i+max_modifiers] = EEPROM.read( i + 64 + 512 );

640–699 : data_MODE PAGE 2
data_MODE[i] = EEPROM.read( i + 128 + (1*512) );

704–763 : data_DM PAGE 2
data_DM[i] = EEPROM.read( i + 192 + (1*512) );

768–827 : data_MI PAGE 2
data_MI[i] = EEPROM.read( i + 256 + (1*512) );
(stesso discorso del remapper)

832–891 : data_MA PAGE 2
data_MA[i] = EEPROM.read( i + 320 + (1*512) );

896–955 : data_QW PAGE 2
data_QW[i] = EEPROM.read( i + 384 + (1*512) );

960–1019: data_LT PAGE 2
data_LT[i] = EEPROM.read( i + 448 + (1*512) );

Anche qui, le posizioni corrispondenti agli indici 60–63 di ogni fascia
restano piccoli spazi di margine, non usati come parte del “rettangolo”
principale di 60 elementi.

Riassunto blocco 1:
512–571 data_TY (PAGE 2)
576–635 data_VA (PAGE 2)
640–699 data_MODE
704–763 data_DM
768–827 data_MI
832–891 data_MA
896–955 data_QW
960–1019 data_LT
1020–1023 (fuori range, non accessibili su 1KB EEPROM)

RUOLO DI load_preset_base() E load_preset()

load_preset_base():
• legge data_TY e data_VA per entrambe le pagine:
- PAGE 1: da 0 / 64
- PAGE 2: da 512 / 576
• serve per inizializzare i “tipi” e i “valori base” di tutti
gli ITEM in RAM, così da avere subito le info minime di entrambe
le pagine (anche se al momento viene usata solo PAGE 1).

load_preset( numero ):
• numero = 0 → PAGE 1
numero = 1 → PAGE 2
• in base a numero, legge:
data_MODE, data_DM, data_MI, data_MA, data_QW, data_LT
dagli offset:
+128, +192, +256, +320, +384, +448
aggiungendo (numero*512) per selezionare il blocco giusto.

• dopo aver caricato data_MODE[i], chiama setup_mempos(i) per:
- individuare distance_sensor_mempos, spinner_mempos, touch_mempos,
general_mempos, ecc.
- se trova un ITEM con mode = 26 (general_mempos), imposta
eeprom_preset_active = 1.
- se NON trova un general_mempos valido, eeprom_preset_active
rimane 0 e viene richiamato aux_preset().

AUTODETECT E AUX_PRESET

eeprom_preset_active:
• NON viene salvata in EEPROM come byte dedicato.
• Viene derivata in setup_mempos():
- se esiste un ITEM con mode 26 (general_mempos), allora
eeprom_preset_active = 1 (preset valido trovato).
- altrimenti rimane 0.

Se eeprom_preset_active rimane 0:
• load_preset() chiama aux_preset():
- preset “di base” costruito in RAM,
- PAGE 1 viene inizializzata con un setup di default,
utile anche per la modalità AUTODETECT.

In questo modo:

la presenza di un general_mempos valido (mode 26) dentro la struttura
della PAGE 1 funge da “firma” del preset.

Se la firma manca, il firmware considera la EEPROM “vuota” o non valida
e parte con un preset standard di emergenza (aux_preset).

NOTA PER FUTURI CAMBI DI LAYOUT

Attualmente TUTTI i 1024 byte della EEPROM sono usati per PAGE 1 e PAGE 2.

PAGE 2 occupa interamente il blocco 512–1023 con lo stesso schema a fasce
di PAGE 1.

Se in futuro si decide di abbandonare PAGE 2 (seconda pagina completa),
il blocco 512–1023 può essere riciclato come:
• banco di SCENE,
• struttura diversa per preset,
• o altre funzioni avanzate,
a patto di aggiornare in modo coerente:

load_preset_base()

load_preset()

il commento di mappa EEPROM presente in D_MIN.ino

ed eventuali funzioni che si aspettano la PAGE 2.


// ------------------------------------------------------------------------------------
// APPROFONDIMENTO: AUTODETECT dentro AIN() (D_INS.ino)
// ------------------------------------------------------------------------------------
// AUTODETECT entra in funzione SOLO quando eeprom_preset_active == 0.
// Questo succede quando load_preset() non trova una "firma" valida del preset
// (general_mempos con data_MODE == 26) e quindi carica un preset di emergenza (aux_preset()).
//
// aux_preset() imposta una base sicura per partire:
// - data_MODE[] = 1  (tutti button)
// - data_TY[] = 144 (NOTE ON di default)
// - range min/max standard
// - (attenzione) data_LT[] viene inizializzato con remapper(i) nella versione classica.
//
// Durante AUTODETECT, AIN() legge l'analogico e fa due azioni principali:
// 1) detect_plexer(): quando vede attività, diversifica data_VA[] nel gruppo di 8 canali
//    per evitare che più input mandino la stessa nota.
// 2) pot-detect: se il valore letto cade in una fascia "intermedia" (indicativa di un pot),
//    promuove il canale a POT (data_MODE=11) e lo sposta su messaggi CC (data_TY=176).
//
// Nota pratica: quando un canale passa a POT, possono attivarsi gli effetti LED tipici dei pot.
// Per evitare lampeggi confusionali durante AUTODETECT si può:
// - forzare data_LT[chan] = 0 quando data_MODE[chan] viene messo a 11
//   (oppure inizializzare data_LT[] a 0 in aux_preset() quando eeprom_preset_active==0).


*/







// #martrix_led_info_efx
//  ────────────────────────────────────────────────────────────────
//  GESTIONE MATRICE LED PER I PAD (Matrix_Pads + single_h)
//  ----------------------------------------------------------------
//  Questa sezione decide COME viene disegnato il pad sulle matrici
//  basate su MAX7219 quando il pulsante passa da spento→acceso o
//  da acceso→spento.
//
//  Nota importante di design:
//  - Layout con SHIFTER e layout con MATRICI sono ALTERNATIVI.
//    Non esiste nessuna configurazione in cui vengono usati insieme.
//    Se usi la matrice, lo shifter non è collegato, e viceversa.
//
//  - Matrix_Pads seleziona se e come usare la matrice:
//      Matrix_Pads = 0 → nessuna matrice per i pad (solo logica interna).
//      Matrix_Pads = 1 → design attuale: usa matrix_remap[chan]
//                        per disegnare i pad sulla matrice attiva.
//      Matrix_Pads = 2 → design vecchio / non più in uso:
//                        combinazione input–matrici storica, lasciata
//                        disponibile ma non usata nei layout attuali.
//
//  - single_h(number_of_unit, sprite, invert, send_):
//      number_of_unit = quale unità / blocco di MAX7219 usare.
//      sprite        = data_LT[chan] → quale “slot” / sprite mostrare.
//      invert        = 0 → sprite NORMALE  (bit non invertiti).
//                      1 → sprite NEGATIVO (bit invertiti con ~NOT).
//      send_         = 1 → invia subito i dati alla matrice.
//
//  Comportamento dinamico premuto / rilasciato:
//  -------------------------------------------
//  • Pad appena PREMUTO  (transizione OFF → ON):
//      - viene chiamata single_h(..., invert = 1, ...)
//      - il pad appare sulla matrice con SPRITE INVERTITO (negativo),
//        evidenziando che il pad è attivo / selezionato.
//
//  • Pad appena RILASCIATO / spento (transizione ON → OFF):
//      - viene chiamata single_h(..., invert = 0, ...)
//      - il pad torna allo SPRITE NORMALE (non invertito).
//
//  In questo layout la matrice è l’unico “display” visivo per i pad;
//  lo shifter NON è collegato quando Matrix_Pads è diverso da 0.
//  ────────────────────────────────────────────────────────────────
// #martrix_led_info_efx - fine



# /*

DART – OWNERSHIP ENCSC (MORPH)
Lock temporizzato (500 ms) per evitare ping‑pong tra 2 spinner
==============================================================

PAROLA CHIAVE PER RICERCA:
CTRL-F: MORPH_OWNER_LOCK

## PROBLEMA OSSERVATO

Quando due ENCSC (spinner) vengono mossi quasi insieme mentre il morph è
attivo, la logica pre-esistente poteva alternare rapidamente l’owner:
A → B → A → B ...

Effetto pratico:

* reset continui dello stato morph
* avanzamento che non progredisce
* possibile “sospensione” del flusso MIDI (meglio della confusione, ma non ideale)

## OBIETTIVO

Stabilizzare l’ownership in modo musicale:

* “last touch wins”
* ma l’owner NON può essere rubato immediatamente dall’altro spinner
* viene introdotto un lock temporale: 500 ms

## COME FUNZIONA (REGOLA)

Se morph è attivo e c’è già un owner:

* gli impulsi provenienti dallo spinner NON-owner vengono ignorati
  finché l’owner ha mosso negli ultimi 500 ms.

In pratica:

* spinner A inizia → A diventa owner
* se muovi B mentre A è ancora “fresco” → B è ignorato
* quando A smette (nessun impulso per 500 ms) → B può prendere ownership

## IMPLEMENTAZIONE (MINIMA)

Dove: file D_scene.ino, funzione:
scene_morph_encsc(byte enc_chan)

1. Definizione costante:
   CTRL-F: MORPH_OWNER_LOCK_MS
   #define MORPH_OWNER_LOCK_MS 500

2. Guardia (gate) all’inizio della funzione:
   CTRL-F: MORPH_OWNER_LOCK_GUARD

   * legge millis()
   * se enc_chan != morph_owner
   * controlla quanto tempo è passato dall’ultimo movimento dell’owner
     usando morph_last_ms[ownerIdx]
   * se < 500 ms → return; (ignora l’impulso del NON-owner)

## CODICE CHIAVE: DOVE METTERE L’ANCORA

Consigliato inserire l’ancora direttamente:

* sopra la #define
* e sopra la guardia

Esempio:
// CTRL-F: MORPH_OWNER_LOCK
#define MORPH_OWNER_LOCK_MS 500

...
// CTRL-F: MORPH_OWNER_LOCK
if (morph_active ... ) { ... return; }

## NOTE IMPORTANTI

* Questa ownership riguarda SOLO gli ENCSC (spinner) usati per il morph.
* Non coinvolge gli encoder generici dello scanning AIN().
* L’approccio è volutamente minimale: niente tabelle nuove, niente stato extra.
* Se in futuro vuoi un feeling più reattivo:

  * riduci MORPH_OWNER_LOCK_MS (es. 250–350)
    Se vuoi più “DJ safe”:
  * aumenta (es. 600–800)

====================================================================
*/

# /*

DART – TAKEOVER POT / SCENE SUBJECT
Aggiornamento della base software dei pot-subject
=================================================

PAROLA CHIAVE PER RICERCA:
CTRL-F: POT_SCENE_BASE_SYNC

## CONTESTO

Nel sistema DART i pot-scene-subject (pot che controllano parametri di scena)
possono essere modificati da due meccanismi interni:

1. MAXWINS (pot analogici: vince il valore più alto)
2. MORPH   (encoder: interpolazione temporale dei valori)

Senza un aggiornamento della "base software" dei pot, questi diventano
"duri": quando l'utente li tocca dopo un morph o un maxwins,
il valore fisico scatta brutalmente verso la posizione precedente.

## OBIETTIVO

Rendere i pot-scene-subject "morbidi" tramite il meccanismo di TAKEOVER,
aggiornando la loro base software quando il valore del subject cambia
PER CAUSE INTERNE (maxwins o morph), senza generare loop o feedback.

## SCELTA ARCHITETTURALE (CRITICA)

L'aggiornamento della base dei pot NON deve passare da:

* convoy_commit()
* sistemi globali di dispatch

Perché:

* convoy è re-entrante
* genera feedback logici interni
* può causare oscillazioni sugli encoder-scene-subject

La base software dei pot viene quindi aggiornata SOLO:

* nel punto in cui il valore finale del subject viene deciso
* localmente
* senza invii MIDI

## IMPLEMENTAZIONE

La stessa identica logica viene applicata in due punti:

A) MAXWINS
B) MORPH

In entrambi i casi:

* il codice è attivo SOLO se ENABLE_POT_TAKEOVER == 1
* vale SOLO per pot (mode 11..15)
* vale SOLO per Page1 (regola fissa nel mondo scene)

## PSEUDOCODICE

if (ENABLE_POT_TAKEOVER)
{
if (subject_mode is POT)
{
// aggiorna base software del pot
data_LB[subject_chan] = final_value << 1;  // coerente con pots()

```
// arma takeover: il pot non scriverà finché non aggancia
bit_write(ARMED_BANK, subject_chan, 1);
```

}
}

## EFFETTO

* Quando MAXWINS o MORPH cambiano un subject:

  * il pot fisico viene riallineato internamente
  * il takeover evita scatti quando l'utente lo tocca

* Nessun invio MIDI extra

* Nessun feedback interno

* Nessuna interferenza con encoder-scene-subject

## NOTE IMPORTANTI

* Questa logica NON va spostata in convoy
* Questa logica NON va eseguita durante lo scanning AIN
* Questa logica è intenzionalmente duplicata (maxwins / morph)
  per mantenere il controllo locale e ridurre effetti collaterali

## ESTENSIONI FUTURE (NON IMPLEMENTATE)

* Ownership degli encoder-scene-subject
* Takeover scena-parametro completo (valutare costo/beneficio)

====================================================================
*/


/* 
 *  ============================================================================

ENCODER INPUT CLEANING
 
ADDENDUM (CNG) — ENCODER DIGITAL FILTERING: MAJORITY + ADAPTIVE + LOCKOUT
Data: 2026-02-09 (Europe/Rome)
Scope: DART Sketch — encoder reliability improvements (scan-based + STRATOS + side + top spinner)

WHY THIS EXISTS
---------------
In the DART ecosystem we can have:
- scan-based encoders (through 4051 chain, read inside ain())
- side spinner (dedicated read function, but same logical encoder pipeline)
- top spinner (read via interrupts, special case)

Goal: reduce bounce / “opposite direction spikes” / noisy states while keeping the system fast.
We achieved it with 3 independent software devices:

  (A) MAJORITY (burst vote on the 2-bit quadrature state)
  (B) ADAPTIVE pre-check (speed optimization for majority)
  (C) LOCKOUT (time-gate after a valid step)

IMPORTANT: these are NOT hierarchical. You can enable/disable them independently.

-------------------------------------------------------------------------------
DEVICE A) MAJORITY (burst vote)
-------------------------------------------------------------------------------
What it does:
- Reads A/B repeatedly (N times), produces a 2-bit state (00/01/10/11) each time.
- Counts occurrences and returns the most frequent state (the “winner”).
- Purpose: stabilize dirty reads before quadrature decoding.

Config knobs (per profile):
- ENABLE_ENC_MAJORITY       (0/1)  : master switch
- ENC_MAJ_SAMPLES           (N)    : number of samples in a burst (typical 9..16)
- ENC_MAJ_USDELAY           (us)   : delay between samples (typical 10..25)
Notes:
- Increasing ENC_MAJ_USDELAY pushes the system from micro-time to milli-time.
  Above ~100us the encoder remains usable but starts degrading when rotating fast.
  Above ~300us it becomes slow and can mis-vote during real movement.
- Increasing ENC_MAJ_SAMPLES increases time cost linearly but does NOT increase RAM.

Time cost rule-of-thumb (digitalRead-based AVR):
  T_burst ≈ N * (cost_digitalRead_AB + ENC_MAJ_USDELAY)
where cost_digitalRead_AB is a few microseconds (2x digitalRead + packing).

Memory:
- RAM: no permanent RAM increase per “N”; only small local counters on stack.
- Flash: presence of majority code costs some Flash, but if ENABLE_ENC_MAJORITY=0
  we guard/compile it out for visual + compile cleanliness.

Polarity:
- DART classic: uses normal digitalRead() polarity.
- STRATOS: historically used inverted reads (!digitalRead). We support that via
  a dedicated wrapper (e.g. enc_majority_state_from_pins_inv()).

-------------------------------------------------------------------------------
DEVICE B) ADAPTIVE (pre-check for majority)
-------------------------------------------------------------------------------
What it does:
- A speed optimization for scan-based systems: when the encoder is stable (not moving),
  we want to avoid running the full burst every time.
- Performs ENC_MAJ_PRECHECK quick reads; if they are all equal, returns immediately.
- If not stable, runs the full burst.

Config knobs (per profile):
- ENABLE_ENC_MAJ_ADAPTIVE   (0/1)  : independent switch
- ENC_MAJ_PRECHECK          (k)    : number of pre-reads (typical 3..5)
- ENC_MAJ_PRE_USDELAY       (us)   : delay between pre-reads (typical 0..5)

Notes:
- Adaptive only has effect if ENABLE_ENC_MAJORITY=1.
- Low PRECHECK values can “exit too early” on borderline states (less cleaning).
  Increasing PRECHECK makes early-exit rarer and “safer”.

Memory:
- No extra RAM. Very small Flash delta.

-------------------------------------------------------------------------------
DEVICE C) LOCKOUT (time-gate after a valid step)
-------------------------------------------------------------------------------
What it does:
- After a valid decoded step (CW or CCW), we ignore new steps for a short time.
- Purpose: kill bounce and “reverse spikes” after a step.
- Implemented with a per-encoder timestamp array:
    enc_lock_last_us[60]  // indexed by MemoryPosition (0..59)

Config knobs (per profile):
- ENABLE_ENC_LOCKOUT        (0/1)  : independent switch
- ENC_LOCKOUT_US            (us)   : lockout duration for scan-based + side + STRATOS
- ENC_TOP_LOCKOUT_US        (us)   : dedicated lockout for TOP spinner (interrupt)

Key implementation detail:
- During lockout we still update the “previous quadrature state”
  (data_MA[chan] in this codebase) so that when lockout ends we do not create
  phase jumps and false steps.

Arm rule:
- Lockout is armed ONLY when updateEncoder() actually produced a step.
  We detect that via data_LB[chan] != 64 (64 == no-step sentinel).

IMPORTANT PITFALL (found and fixed on side spinner):
- encoder(chan) often CONSUMES data_LB[chan] and resets it to 64 after output.
  Therefore: if you arm lockout AFTER calling encoder(chan), you may NEVER arm it.
  Correct sequence is:
      updateEncoder(chan);
      if (step) enc_lock_last_us[chan] = now;   // arm here
      encoder(chan);                            // then output
This is critical for any code path where updateEncoder() and encoder() are called
back-to-back inside the same function (e.g. side spinner read).

-------------------------------------------------------------------------------
WHERE EACH DEVICE IS USED
-------------------------------------------------------------------------------
1) Generic scan-based encoders (4051 / ain()):
- can use MAJORITY (+ optional ADAPTIVE) before updateEncoder()
- can use LOCKOUT around updateEncoder() results

2) STRATOS (AIN_stratos()):
- old STRATOS-specific majority/lockout removed
- uses the same general functions:
    - majority inv wrapper for pin polarity (!digitalRead)
    - lockout uses enc_lock_last_us[chan] and ENC_LOCKOUT_US
- if ENABLE_ENC_MAJORITY=0 -> STRATOS falls back to “traditional” single-read mode

3) Side spinner (Side_spinner_read()):
- now supports LOCKOUT and can optionally use MAJORITY on its A/B pins
- same timing as scan-based: ENC_LOCKOUT_US (generic)
- MUST arm lockout immediately after updateEncoder() and before encoder()

4) Top spinner (interrupt: lettura_enc_principale()):
- NO MAJORITY (do NOT do burst+delay inside ISR)
- YES LOCKOUT (time-gate) using ENC_TOP_LOCKOUT_US
- uses chan_enc = spinner_mempos[0] (top spinner memoryposition), then updateEncoder(chan_enc)
- lockout check is in ISR; phase is still updated during lockout to avoid jumps

-------------------------------------------------------------------------------
TUNING GUIDELINES (starting points)
-------------------------------------------------------------------------------
Majority:
- ENC_MAJ_SAMPLES:   9..16
- ENC_MAJ_USDELAY:   10..25  (micro-time zone)
- Adaptive:
  - ENC_MAJ_PRECHECK: 3..5
  - ENC_MAJ_PRE_USDELAY: 0..5

Lockout:
- ENC_LOCKOUT_US (scan-based/side/stratos): 400..1000 (depends on bounce + feel)
- ENC_TOP_LOCKOUT_US:
  - mechanical “micro touches”: 600..900 (e.g. 800)
  - optical rare “threshold chatter”: 20..50

Sanity check:
- huge lockout (e.g. 8000UL) produces visible “brake”/latency -> proves gate works.

-------------------------------------------------------------------------------
FOR FUTURE REFINEMENTS (notes for later chats)
-------------------------------------------------------------------------------
- If needing more speed: reduce ENC_MAJ_SAMPLES and keep USDELAY low.
- If needing more cleaning (without big delays): increase samples slightly, keep delay micro.
- Consider hardware improvements as next step:
  better encoders, proper RC + Schmitt/trigger, cleaner grounding, shorter cables.
- Top spinner: if more robustness is needed, prefer illegal-transition rejection or
  ISR-minimal + decoding in loop, not majority bursts in ISR.

END ADDENDUM
============================================================================ 
*/

// per installare e sperimentare con adafruit metro m0 express https://adafruit.github.io/arduino-board-index/package_adafruit_index.json - aggiungere url in preferenze poi installare schede adafruit 


/*===== CTRL-F: FAST_FEEDBACK =====

FAST_FEEDBACK is a special shortcut system designed for items that are used only as MIDI-controlled outputs.

Normally, when a MIDI feedback message arrives, DART processes it through several layers of logic:
groups, toggles, modifiers, matrix operations, value management, etc.

For some applications this is unnecessary.

Modes 35 and 36 provide a direct path:

MODE 35 = LED ONLY
MODE 36 = DMX ONLY

These modes are intended for indicators, status LEDs, external light feedback, DMX visualization, and similar uses where the item does not need to behave like a normal control.

Flow:

MIDI IN
  |
  v
midifeedback()
  |
  v
fast_feedback_apply()
  |
  +--> MODE 35 -> LED update
  |
  +--> MODE 36 -> DMX update

The processing stops here.

No button logic.
No toggle logic.
No modifier logic.
No matrix processing.

The item simply receives a MIDI value and immediately updates its visual output.

Advantages:

- lower CPU usage
- faster response
- simpler behaviour
- useful for dedicated indicators

Important:

Modes 35 and 36 are output-oriented modes.

They are normally excluded from the standard input scanning process and are not intended to act as ordinary buttons, pots or encoders.

Typical examples:

- VU meters
- clip indicators
- transport LEDs
- deck status indicators
- DMX visual feedback
- software status mirrors

In short:

FAST_FEEDBACK creates a dedicated high-speed route from MIDI feedback to LED or DMX output, bypassing most of the normal DART processing chain.

*/


/*===== CTRL-F: STAGED_BUTTON_READ_TEST =====

STAGED BUTTON READ SYSTEM
Mini-guide for custom button behaviours

The staged button system separates two different jobs:

1. reading the physical button
2. deciding what the button does

This is useful because the same clean reading system can be reused for many different behaviours:
MIDI notes, toggles, HID keyboard commands, special user modes, DMX actions, etc.

In the old style, button reading and button action were often mixed together.
In the staged system, the firmware first decides the button state, then another part of the code decides the effect.

Basic flow:

physical button
  |
  v
ain_nucleo()
  |
  v
push_buttons_lettura_stage()
  |
  +--> pressed   = button has just been pressed
  +--> released  = button has just been released
  +--> down      = button is currently held
  +--> velocity  = pressure/velocity value, when available
  |
  v
custom action

Important idea:

The staged reading function should only detect the state of the button.
It should not directly send MIDI, HID, DMX, LED changes or other final actions.

Those actions should happen after the reading stage, depending on the selected MODE.


------------------------------------------------------------
EXAMPLE: USER1 button sends the HID letter "a"
------------------------------------------------------------

Goal:

When an item is set to USER1 mode, pressing its button sends the keyboard letter "a".

In this example:

- USER1 is one of the user modes handled by ain_nucleo()
- the staged system detects the press
- the custom code reacts only to the PRESS event
- the letter is sent once, not continuously while the button is held


Pseudo-flow:

button connected to an item
  |
  v
item MODE = USER1
  |
  v
ain_nucleo() detects USER1
  |
  v
push_buttons_lettura_stage(chan)
  |
  v
if button was just pressed:
    Keyboard.press('a')
    Keyboard.release('a')


Example code idea:

case USER1_MODE:
{
    push_buttons_lettura_stage(chan);

    if (pb_pressed(chan))
    {
        Keyboard.press('a');
        Keyboard.release('a');
    }

    break;
}


Why use pb_pressed() and not pb_down()?

pb_pressed(chan) happens only once, at the exact moment the button is pressed.

pb_down(chan) stays true for all the time the button is held.

So:

pb_pressed(chan)
  = good for one-shot actions
  = send one HID letter
  = launch one command
  = trigger one MIDI event

pb_down(chan)
  = good for continuous actions
  = keep something active while holding the button
  = repeat or sustain behaviour


------------------------------------------------------------
PRACTICAL NOTES
------------------------------------------------------------

Use the staged system when you want to create a new custom behaviour without rewriting the button-reading logic.

Do not duplicate debounce code inside every new mode.

Do not read the physical pin again inside the custom action.

Let the staged system do the reading first, then use the clean state result.

Good pattern:

read button once
  |
  v
use pressed / released / down
  |
  v
perform the custom action

Bad pattern:

read button
  |
  v
send action
  |
  v
read same button again somewhere else
  |
  v
send another action

The staged system keeps button behaviour cleaner, more predictable and easier to debug.


------------------------------------------------------------
SHORT SUMMARY
------------------------------------------------------------

STAGED_BUTTON_READ_TEST turns button handling into a two-step process:

1. read and classify the button state
2. apply the selected behaviour

This makes custom modes safer and easier to build.

For a USER mode, the recommended approach is:

- select the USER mode in ain_nucleo()
- call the staged reading function
- react to pb_pressed(), pb_released() or pb_down()
- keep the final action separate from the physical reading


// STAGED_BUTTON_READ_TEST giude - end 
*/




















/*
 ANCHOR: DART_MOUSE_AND_BOOSTAX_EXPLANATION

DART MOUSE SYSTEM — INTERNAL MECHANISM EXPLANATION

The DART mouse system is built around one special memory slot: mouse_mempos.

This slot does not represent a normal physical control by itself.
Instead, it acts as the configuration center for the mouse engine.

The two analog inputs used for mouse movement are stored inside:

data_MI[mouse_mempos]  -> X axis input
data_MA[mouse_mempos]  -> Y axis input

So the mouse system works only when the current scanned input, chan,
matches one of these two stored positions.

If chan equals data_MI[mouse_mempos], the code reads the X axis.
If chan equals data_MA[mouse_mempos], the code reads the Y axis.

The mouse engine is called at the beginning of ain_nucleo(), before the
normal mode switch. This means that mouse movement is treated as a special
parallel function, not as a normal POT or BUTTON behavior.



1. BASIC FLOW

The general sequence is:

1. ain_nucleo() calls mouse_control()
2. mouse_control() checks if mouse_mempos exists
3. mouse_control() checks if mouse speed is active
4. the current analog input is read
5. the analog value is converted into a signed direction value
6. optional axis inversion is applied
7. optional mouse_block safety gate is checked
8. mouse_axis_step() converts the direction into real HID mouse movement
9. Mouse.move() sends the final movement to the computer



2. MOUSE SPEED

The base mouse speed comes from:

data_DM[mouse_mempos]

This value is not used directly.
The code calculates:

baseSpeed = data_DM[mouse_mempos] - 32

So value 32 means zero speed.
Values above 32 produce movement.
Values below or equal to 32 do not produce useful mouse movement.

This is important because the mouse slot uses data_DM not as DMX output,
but as a speed parameter for the HID mouse engine.



3. ANALOG VALUE TO DIRECTION

Each axis reads a 10-bit analog value, normally from 0 to 1023.

The code compresses this value into a small signed range:

d = ((value + 1) >> 5) - 16

In practice, this turns the analog reading into a value around:

-16 ... 0 ... +15

So the joystick center is around 0.
Negative values move in one direction.
Positive values move in the opposite direction.

This reduced range is intentional:
it makes the mouse calculation lighter and faster.



4. DEADZONE

Inside mouse_axis_step(), the first important filter is the deadzone:

if d is between -2 and +2, the axis is considered still.

When the axis is inside the deadzone:

- the movement accumulator is reset
- the Boostax counter for that axis is reset
- no mouse movement is sent

This prevents small analog noise around the center from moving the pointer.



5. AXIS INVERSION

Axis inversion is controlled by:

data_LT[mouse_mempos]

Only the first two bits are used:

bit 0 -> invert Y
bit 1 -> invert X

So data_LT[mouse_mempos] works as a compact two-switch configuration byte.

If bit 1 is active, X direction is flipped.
If bit 0 is active, Y direction is flipped.



6. ACCUMULATOR SYSTEM

The mouse does not send every tiny analog change immediately.

Instead, each axis has its own accumulator:

accum_x
accum_y

The code adds movement energy to the accumulator:

accum += (d * speed_total) / 8

Then it waits until the accumulator reaches at least 16 or -16.

Only then it converts the stored energy into an actual Mouse.move() step.

This gives smoother motion and avoids wasting HID messages on tiny movements.

The accumulator is also useful because it preserves fractions of movement:
small inputs can gradually build up until they become one real mouse step.



7. BOOSTAX

Boostax is an optional progressive acceleration system.

It is enabled or removed at compile time with:

ENABLE_BOOSTAX

Boostax works independently on X and Y:

boost_counter_x
boost_counter_y

This is important:
pushing the joystick hard on X does not automatically boost Y, and vice versa.



8. WHEN BOOSTAX STARTS

Boostax only starts charging when the axis is pushed close to its extreme:

d <= -14 or d >= 14

So Boostax is not active during small or medium movements.
It is reserved for strong, intentional joystick pressure.

If the axis remains strongly pushed, the boost counter increases.

If the axis is no longer strongly pushed, but still outside the deadzone,
the boost counter slowly decreases.

If the axis returns to the deadzone, the counter is fully reset.



9. BOOSTAX TIMING

The current constants are:

boost_delay_max  = 200
boost_step_delay = 12
boost_max        = 35
boost_limit      = 620

Meaning:

- for the first 200 cycles, no boost is added
- after that, boost increases by 1 every 12 cycles
- the maximum boost is 35
- the counter stops growing at 620

So Boostax is not an instant turbo.
It is a delayed acceleration.

The idea is:

short push  -> normal mouse movement
long hard push -> progressively faster movement



10. FINAL SPEED

The final speed used by the mouse axis is:

speed_total = baseSpeed + boost

Without Boostax, speed_total is just the base speed.

With Boostax enabled, the mouse can start slow and then accelerate
when the joystick remains strongly pushed.



11. MOUSE_BLOCK SAFETY SYSTEM

There is also an optional mouse_block system.

If enabled, it acts like a safety brake.

When the mouse remains active continuously for several seconds,
mouse_block stops further mouse output.

The block is released only when the joystick returns to rest for a short time.

When the block activates, the code resets:

- accum_x
- accum_y
- boost_counter_x
- boost_counter_y

This prevents sudden jumps when the mouse becomes active again.



12. FINAL OUTPUT

The final HID output is sent here:

Mouse.move(move, 0, 0)  -> X axis
Mouse.move(0, move, 0)  -> Y axis

The wheel is not handled inside mouse_control().
Mouse wheel behavior is managed elsewhere, mainly in the encoder logic.



13. SHORT PRACTICAL SUMMARY

The DART mouse engine is a lightweight joystick-to-HID translator.

mouse_mempos selects the mouse configuration slot.
data_MI[mouse_mempos] selects the X input.
data_MA[mouse_mempos] selects the Y input.
data_DM[mouse_mempos] controls the base speed.
data_LT[mouse_mempos] controls X/Y inversion.

Each analog axis is reduced to a small signed direction value.
A deadzone removes unwanted jitter.
An accumulator makes movement smoother.
Boostax adds delayed acceleration only when the joystick is pushed hard.
Mouse.move() sends the final result to the computer.

In short:

normal movement is smooth and controlled;
hard continuous movement gradually becomes faster.


 ANCHOR: DART_MOUSE_AND_BOOSTAX_EXPLANATION - end
*/



/* 
 -

ANCHOR: DART_TOUCH_SENSOR_OVERVIEW

DART TOUCH SENSOR SYSTEM

DART supports two touch sensors, normally associated with the two spinner/jog controls.

In the firmware, a touch sensor is treated mainly as an ON/OFF detector. It is not used as a continuous expressive analog control. Its job is to tell the system: “the user is touching this control” or “the user has released it”.

The physical source of this information can change. The touch state may come from an internal capacitive sensing system, from an external digital touch chip, or even from a normal mechanical button. Once the firmware receives the touch state, the following DART logic remains the same.

The two touch sensors are linked to their memory slots through:

touch_mempos[0]
touch_mempos[1]

These slots contain the normal DART settings for the touch item: MIDI type, MIDI number, sensitivity/decay-related values, LED reference, reset options and operating mode.

Touch_sensors_enable selects the hardware strategy:

0 = touch disabled
1 = internal touch sensing
2 = external digital touch inputs on pins 7 and 9
3 = external digital touch inputs on pins 7 and 8

In external mode, DART simply reads HIGH or LOW from the input pins. This means that an external capacitive touch module and a mechanical button can be used in the same logical way.

For buttons to GND, or open-drain/open-collector outputs, the internal pullup can be enabled with:

touch_pullup = 1

If the external signal has the opposite logic, it can be corrected with:

touch_invert = 1

So the external touch mode is not limited to “touch chips”. It is a generic digital touch/button input mode.

The main touch behavior is handled by touch_execute(). When a touch is detected, DART sends a MIDI ON-style message with value 127. When the touch is released, DART sends value 0. The message type and number come from the touch item settings, just like other DART controls.

The touch system includes a decay mechanism. The release is not always sent immediately when the raw touch signal disappears. Instead, decaysensor[] counts down before confirming the OFF state. This avoids nervous ON/OFF flickering, weak-contact interruptions and unstable releases.

For the physical touch system, the decay amount is based on:

data_LT[general_mempos]

On standard DART builds the decay is multiplied by 2.
On STRATOS builds it is multiplied by 10.

This means the GENERAL settings influence how long the physical touch remains active after the raw signal falls below the release threshold.

The touch slot also controls sensitivity. In the internal sensing mode, data_MI[touch_mempos[x]] is used to decide how easily the touch threshold is reached. Higher or lower values change how close the reading must be to the learned upper/lower touch range before the firmware considers the sensor pressed or released.

External digital touch inputs do not need this analog threshold in the same way, because the external circuit already provides a clean HIGH/LOW signal.

DART also includes a separate Virtual Touch system. Virtual Touch is not produced by a physical sensor: it is generated by software when a spinner moves and the touch item is configured for virtual touch behavior.

Virtual Touch uses:

data_LT[touch_mempos[x]] = 1

When the spinner moves, DART can send a virtual touch ON message. When the spinner stops, virtual_touch_end() sends the virtual touch OFF message after its own delay.

This Virtual Touch delay is separate from the physical touch decay. In the current code, Virtual Touch timing is based on:

data_MI[touch_mempos[x]]

and uses cycletimer as its internal timer.

So there are two different decay concepts:

1. Physical touch decay
   Used after a real capacitive/digital/button touch disappears.
   Controlled mainly through data_LT[general_mempos].

2. Virtual Touch decay
   Used after spinner movement stops.
   Controlled through data_MI[touch_mempos[x]].

Touch sensors also interact with spinner behavior. They can be used for touch-stop logic, note-off safety in scale mode, LED feedback, and optional touch reset.

If touch reset is enabled through:

data_QW[touch_mempos[x]] = 1

then releasing the touch can force the related spinner value back toward a stored reference value, taken from:

data_DM[touch_mempos[x]]

In short, the DART touch system is a small but central ON/OFF layer. It does not matter whether the touch comes from a capacitive pad, a digital module or a mechanical button. Once the firmware receives the touch state, it can send MIDI touch messages, control LEDs, protect note behavior, stop or release spinner functions, and optionally reset spinner values.



------------------------------------------

Touch Averaging

DART includes an optional averaging stage that can combine multiple raw touch readings before threshold evaluation.

Historically, averaging was used to reduce noisy touch readings and improve stability. However, later versions of the firmware introduced a configurable decay system that proved to be a simpler and often more effective solution.

Instead of continuously smoothing incoming readings, the decay mechanism keeps the touch active for a short time after the signal disappears. This prevents rapid ON/OFF oscillations caused by weak contact, noise or unstable releases, while requiring less processing and preserving a more immediate response.

For this reason, many modern DART configurations rely primarily on decay and can operate correctly with touch averaging disabled.§

*/





/*
 
ANCHOR: DART_POTENTIOMETER_READING_SYSTEM

DART POTENTIOMETER READING SYSTEM

Reading a potentiometer may seem simple, but in a real controller several factors can reduce accuracy and stability: ADC noise, multiplexer settling time, electrical interference, mechanical vibrations and sudden jumps caused by preset changes.

For this reason, DART includes several optional filtering and stabilization stages. Each stage solves a different problem, and they can be enabled or disabled independently through compile-time defines.

The signal path can be summarized as:

Potentiometer
→ ADC reading
→ optional stabilization
→ optional smoothing
→ movement detection
→ optional high precision window
→ optional takeover management
→ MIDI/DMX output


1. FAST ANALOG READ

#define Fast_analogread

DART can use a custom high-speed ADC routine instead of the standard Arduino analogRead().

The fast version reduces acquisition time significantly, which is particularly useful when scanning many inputs through multiplexers.

The tradeoff is slightly reduced ADC precision. Internally the system works on an 8-bit style reading and later rescales it to the ranges used by the firmware.

In practice this allows faster scan cycles while maintaining more than enough precision for most MIDI controller applications.



2. DUMMY READ

#define Dummy_read

Analog multiplexers such as the CD4051 require a short settling time after changing channel.

Immediately after switching to a new channel, the ADC sample-and-hold capacitor may still contain a small residue from the previous input.

To avoid this effect, DART can perform two consecutive readings:

first reading  -> discarded
second reading -> used

This greatly improves consistency when scanning many analog channels through multiplexers and is especially important when using fast scan rates.



3. DUMMY PULLUPS

#define Dummy_pullups

This option is related to the Dummy Read process.

When enabled, DART temporarily changes pullup states during the stabilization phase before restoring them.

The feature exists mainly for experimentation and hardware compatibility. Most configurations work correctly with this option disabled.



4. POT EMA SMOOTHING

#define POT_EMA_ENABLE
#define POT_EMA_SHIFT

EMA stands for Exponential Moving Average.

This is essentially a software low-pass filter, sometimes described as a "digital capacitor".

Instead of using the raw ADC value directly, DART gradually moves toward the new reading.

Small fluctuations are absorbed while real movements remain responsive.

The amount of smoothing is determined by POT_EMA_SHIFT:

smaller values:
    faster response
    less filtering

larger values:
    slower response
    stronger filtering

This filter is particularly useful on controllers with noisy analog circuitry or long cable runs.



5. HIGH PRECISION WINDOW

#define ENABLE_POT_HIPREC_WINDOW

This system was introduced to improve the balance between stability and precision.

A common problem is that a potentiometer needs a relatively large threshold to avoid jitter when standing still, but a small threshold to allow fine adjustments.

The High Precision Window solves this conflict dynamically.

When a large movement is detected, DART temporarily enters a more sensitive operating window.

Inside this window, smaller changes are accepted and transmitted.

After a period of inactivity, the window closes and the system returns to a more conservative threshold.

The result is:

stable when idle
precise when moving

This feature proved particularly effective on demanding hardware configurations and contributed significantly to the stability of the LIME controller.




6. POT TAKEOVER

#define ENABLE_POT_TAKEOVER

Takeover is not an analog filter in the traditional sense.

Instead, it solves the synchronization problem that occurs whenever the software value and the physical potentiometer position no longer match.

This situation is common in modern MIDI setups. A parameter may change because of:

Page switching
Scene recall
Morph systems
Preset loading
MIDI feedback from software

For example, a DJ or musician may move a filter, EQ or effect parameter directly inside Traktor, Ableton Live or another application. The software updates its internal value, but the physical potentiometer on the controller remains in its previous position.

Without takeover, the next movement of the potentiometer immediately forces the parameter to jump to the hardware position. If the control is assigned to something critical such as a filter cutoff, the result can be a sudden and very noticeable parameter jump during a live performance.

With takeover enabled, DART waits until the physical potentiometer reaches the current software value before allowing it to take control again.

DART implements this using an ARMED / CAUGHT mechanism.

ARMED:
    the software value and the hardware position are different;
    the potentiometer is temporarily ignored.

CAUGHT:
    the potentiometer has reached the current parameter value;
    normal control is restored.

The result is a smooth handover between software and hardware, preventing abrupt jumps and making MIDI feedback workflows practical and safe during live use.



7. DUMMY READ DIGI -   ANCHOR: DUMMY_READ_DIGI_HISTORY

#define Dummy_read_digi

Although not directly related to potentiometers, this option belongs to the same acquisition philosophy.

It was introduced during the SAMD21/M0 porting process.

The original AVR-based firmware scanned inputs slowly enough that CD4051 outputs had time to settle naturally.

The much faster SAMD21 scan cycle could occasionally read a digital signal before the multiplexer output had fully stabilized.

To improve reliability, DART can perform an additional preliminary digitalRead() followed by a short delay and a second read.

This mechanism is mainly related to multiplexer settling time rather than switch debouncing.



FINAL NOTES

These systems address different problems and should not be considered interchangeable.

Dummy Read improves ADC acquisition reliability.

EMA smoothing reduces noise and small fluctuations.

High Precision Window improves the balance between stability and fine control.

Takeover prevents logical jumps after preset-related value changes.

Together they form the modern DART potentiometer handling system, developed through years of testing on real hardware and live performance environments.§
*/


 
