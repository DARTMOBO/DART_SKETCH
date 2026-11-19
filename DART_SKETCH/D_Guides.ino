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

The editor value is converted into an internal base speed:

baseSpeed = data_DM[mouse_mempos] / 2

The editor scale is:

0  = pointer X/Y disabled; wheel assignment remains independent
16 = half speed   (internal speed 8)
32 = normal speed (internal speed 16)
64 = double speed (internal speed 32)

This preserves the original maximum internal speed while distributing
the whole editor range linearly. The mouse slot uses data_DM not as DMX output,
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

After all MODEs have loaded, active X/Y inputs become BLIND so their original
BUTTON/POT behavior cannot also send MIDI. Invalid array indices are ignored;
preset input 0 means unassigned and maps to the internal sentinel 255. Actual
internal index zero remains valid. With HID mouse excluded, MIDI stays usable.
Only HID on ATmega32U4 activates the mouse routes. This preserves board guards.

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

6. POT TAKEOVER

#define ENABLE_POT_TAKEOVER

Takeover is not an analog filter in the traditional sense.

Instead, it solves the synchronization problem that occurs whenever the software value and the physical potentiometer position no longer match.

This situation is common in modern MIDI setups. A parameter may change because of:

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
