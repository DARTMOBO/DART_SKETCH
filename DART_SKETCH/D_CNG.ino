

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


// extraplex e fastanalog : al momento attuale ho fissato digitalread per tutto l'extraplex - la lettura fastanalog non funziona sull'extraplex non so perchè - pin 9 non corrisponde e non trovo soluzione
//                          in futuro sarebbe carino avere lettura differenziata per MODE tra digitalread e analog-normale

// dmx libreria si può finalmente sospendere da un falg unico in config.h
// messo piezo pads sotto una flag apposita in config.h
// tolto messaggi serialprint di test dalla funzione mouse

// finalmente dmx si può attivare con un singolo flag , ho spostato tutti i define in config.h


/*
DART_SKETCH – panoramica rapida

Questo commento serve ad avere una mappa mentale veloce dello sketch,
per potersi riallineare in pochi minuti senza doverlo ristudiare da zero.

L’idea è:

ricordare come sono organizzati gli “ITEM” (slot di memoria),

capire il flusso principale (loop → AIN → ain_nucleo → moduli),

tenere a mente come funzionano LED, MIDI, editor, EEPROM e varianti hardware.

Architettura generale: ITEM e tabelle condivise

Il cuore del firmware è una tabella di 60 ITEM (slot logici).
Ogni ITEM rappresenta:
• un ingresso fisico (pulsante, pot, pad, encoder, sensore, joystick…)
• oppure una funzione “virtuale” (touch, mouse, impostazioni generali, ecc.).

Ogni ITEM è indicizzato da “chan” / “mempos” e usa una serie di tabelle RAM:
• typetable[] → tipo di messaggio / canale (note, CC, PC, aftertouch…)
• valuetable[] → valore principale (nota, CC number o value, ecc.)
• maxvalue[] → limite superiore / parametro extra (es. velocity, soglie, ecc.)
• minvalue[] → limite inferiore / parametro extra
• modetable[] → modalità di funzionamento (button, toggle, encoder, joystick, pagina, touch…)
• lightable[] → indirizzamento e stato dei LED collegati a quell’ITEM
• qwertyvalue[] → parametri speciali e configurazioni interne per alcuni mode

Esistono ITEM “speciali” usati come puntatori o configurazioni:
• general_mempos → slot delle impostazioni generali del controller
• mouse_mempos → slot associato all’emulazione del mouse (se attiva)
• page_mempos → slot che controlla lo stato di pagina A/B
• touch_mempos[] → slot logici associati ai sensori di tocco

L’editor MIDI lavora su questi stessi 60 ITEM: legge e scrive le tabelle
(typetable, valuetable, modetable, ecc.) via messaggi MIDI secondo un
protocollo proprietario.

Ciclo principale: loop()

Il loop principale (D_loop.ino) coordina tutte le parti dello sketch:
• aggiornamento dei contatori interni (es. cycletimer)
• gestione di eventuali lampeggi generali (blinker)
• scansione degli ingressi fisici:
- AIN() → versione classica DART (stratos == 0)
- AIN_stratos() → versione per il controller STRATOS (stratos == 1)
• chiamata al nucleo logico:
- ain_nucleo() → traduce il valore letto in comportamento (button, pot, encoder, ecc.)
• gestione di eventuali funzioni mouse (mouse_control) se abilitate
• gestione MIDI in (D_MIN.ino) con relativo aggiornamento dei LED
• aggiornamento LED:
- via shifter (D_LED.ino) se shifter_active == 1
- via MAX7219/matrici (D_mtrx.ino) se Matrix_Pads == 1
• gestione dei cambi pagina (PAGE A/B)

In pratica, in ogni ciclo:
ingresso fisico → AIN / AIN_stratos → ain_nucleo → modulo specifico
→ eventuale MIDI / DMX / HID → aggiornamento LED e matrici.

Scansione degli ingressi: AIN() e AIN_stratos()

3.1 AIN() – versione DART classica (stratos == 0)

Si trova in D_INS.ino.

Scansiona gli ingressi che arrivano dai 4051 (multiplexer analogici).
Per ogni ciclo:
• scorre “channel” (0–7 ecc.) per leggere le uscite dei plexer
• usa analogRead_1024(plexer) per ottenere un valore 0–1024
• calcola “chan”: lo slot logico collegato a quel plexer/pin
• l’eventuale blocco AUTODETECT può usare direttamente questa lettura
per capire se il canale si comporta come pulsante o come pot.
• a lettura finita, passa il controllo a:
ain_nucleo()

3.2 AIN_stratos() – versione STRATOS (stratos == 1)

Si trova in D_STRATOS_ain.ino.

Effettua una scansione diversa (fino a 20 canali) adatta al layout di STRATOS.

Nel modello STRATOS al posto dei 4051 viene usato un sistema di multiplexing
più “rudimentale” basato su diodi e su un paio di linee di grounding,
per ampliare il numero di ingressi utilizzabili sull’Arduino Pro Micro
(che è la base hardware del DART più piccolo).

Nucleo logico: ain_nucleo()

Si trova in D_INS.ino.

Prende:
• chan → quale ITEM stiamo aggiornando
• modetable[chan]→ che tipo di controllo è (button, toggle, encoder, touch, mouse, ecc.)

È il grande “smistatore”: per ogni ITEM decide quale modulo di controllo chiamare.

Schema semplificato (i numeri di mode sono di riferimento):
• mode 0 → ITEM vuoto (nessuna azione)
• mode 1–10 circa → vari tipi di pulsanti:
- momentanei, toggle, toggle di gruppo, radio, ecc.
- delega a push_buttons() in D_MOD.ino
• mode 11–15 circa → potenziometri / slider:
- lettura analogica, smoothing, mapping a MIDI
- delega alla funzione per i pot in D_MOD.ino
• mode 17 → page switch:
- usa page_mempos, qwertyvalue[] e pagestate
- gestisce il passaggio tra PAGE A e PAGE B
• mode 18 → joystick / XY (dove presente)
• mode 19 → encoder generici:
- delega a encoder() in D_MOD_ENC.ino
• mode 21–22 → spinner (top e side)
• mode 23–24 → touch sensor 1/2
• mode 25 → mouse: seleziona lo slot per mouse_control()
• mode 26 → general settings slot (parametri generali del controller)

Prima/dopo queste diramazioni, ain_nucleo può chiamare mouse_control()
se mouse_mempos è valido e l’emulazione mouse è attiva.

Moduli di controllo: D_MOD., D_MOD_ENC., D_MOD_mouse.*

5.1 Moduli “generali” (D_MOD.ino)

Funzioni dedicate alla logica dei controlli base:
• push_buttons(velo)
- gestisce pulsanti semplici, toggle, toggle di gruppo, radio.
- può calcolare la velocity (per i pad) in base al tempo di pressione
e alle soglie upper_val / lower_val.
- aggiorna valuetable[] e lightable[] secondo il tipo di pulsante.

• funzioni per potenziometri/fader:
- leggono il valore analogico filtrato
- applicano range e limiti da minvalue[] / maxvalue[]
- aggiornano valuetable[] e inviano i relativi messaggi MIDI
- aggiornano lo stato dei LED di feedback (in lightable[]).

• moduli per pad velocity e altre modalità speciali:
- usano minvalue/maxvalue come parametri di sensibilità/dinamica.

Questi moduli sono pensati per lavorare in sincronia con i parametri
salvati in EEPROM e modificabili dall’editor.

5.2 Moduli per encoder e spinner (D_MOD_ENC.ino)

Funzione principale: encoder(byte numero)
• interpreta variazioni di MSB/LSB e usa lastbutton[numero] per il verso (- / 0 / +)
• gestisce:
- encoder generici (mode 19)
- spinner top/side (mode 21/22)
• usa tabelle come encodervaluepot[] per modalità a scala / pot
• aggiorna:
- il valore interno associato all’ITEM (es. posizione)
- i messaggi MIDI (incrementi, note, CC, scale)
- il feedback LED specifico per encoder (es. “collana” di LED).

Funzioni di supporto (esempio):
• led_enc_exe(), led_enc_exe_matrix():
- disegnano graficamente la posizione dell’encoder su shifter o matrice.

5.3 Modulo mouse (D_MOD_mouse.ino)

Funzione principale: mouse_control()
• disponibile sulle MCU tipo ATmega32U4 (Leonardo/Micro, ecc.)
• legge i pot XY associati a mouse_mempos
• applica:
- deadzone attorno al centro (per ridurre il jitter)
- accumulo e soglie per avere movimenti morbidi
- velocità configurabile da editor (dmxtable[mouse_mempos])
- opzionale “Boostax” (se abilitato via define) per aumentare la velocità
quando il joystick resta inclinato al massimo da un po’ di tempo.
• converte il risultato in movimenti reali del mouse HID (Mouse.move()).

Tutto il comportamento è configurabile tramite lo slot mouse_mempos,
in modo analogo agli altri ITEM.

Output visivi: shifter, MAX7219, LED di stato

6.1 Shifter e MAX7219

Lo sketch può pilotare:
• shift register (tipo 74HC595) → tramite _DART_Shifter.*
• matrici LED basate su MAX7219 → tramite DartLedControl.* e LedControl.*

La scelta è controllata da alcuni define:
• shifter_active:
- 1 → attiva la gestione degli shift register
- 0 → esclude il blocco shifter
• Matrix_Pads:
- 0 → nessun MAX7219 attivo
- 1 → attiva le matrici MAX7219 per i pad / indicatori
- 2 → percorso particolare e raro, mantenuto solo per una vecchia
gestione con sensori di forza accoppiati alle matrici di LED
(uso molto sporadico, rimane a livello di codice).

6.2 Effetti LED e coerenza con lo stato dei pulsanti

La gestione LED è studiata per creare effetti grafici non distruttivi:
quando finisce l’effetto, la “base” dei LED torna a rappresentare in modo
chiaro lo stato vero dei pulsanti (toggle, gruppi, radio).

Funzioni come buttonledefx() e encledefx():
• disegnano piccoli effetti temporizzati (flash, espansioni simmetriche, ecc.)
attorno al pulsante o encoder che è stato usato
• lo fanno senza cancellare in modo permanente la mappa di fondo:
ledrestore(page) e la logica interna si occupano di ripristinare la
situazione coerente con lightable[] e con lo stato dei toggle.

I LED reagiscono sia:
• alle azioni sul pannello (premi un tasto, giri un encoder…)
• ai messaggi MIDI in arrivo dall’esterno, se mappati come feedback.

6.3 LED di stato per touch, potenziometri e blinker

Alcuni LED sono riservati a segnalare:
• stato dei sensori di touch (attivo / virtuale / test, ecc.)
• “allineamento” dei pot rispetto ai valori salvati:
- blinker e cycletimer vengono usati per far lampeggiare i pot
che non corrispondono al valore del preset, oppure per altre
situazioni particolari (status di calibrazione, ecc.).

MIDI IN, MIDI OUT e protocollo con l’editor

7.1 MIDI OUT (D_OUT.ino)

Qui ci sono tutte le funzioni che spediscono messaggi verso l’host:
• noteOn / noteOff
• ccOut
• pcOut
• aftertouch, ecc.

Le funzioni di output vengono chiamate dai moduli di controllo (pulsanti,
pot, encoder…). In parallelo allo stesso evento si possono attivare anche:
• eventuali uscite DMX (se DMX_active == 1)
• eventuali emulazioni HID (tastiera / mouse) se abilitate.

Il percorso logico è quindi unico: un’azione sul pannello arriva a un
“nucleo” di uscita che può generare MIDI, DMX e HID in parallelo
secondo la configurazione.

7.2 MIDI IN (D_MIN.ino)

Gestisce note e CC in arrivo dall’host:
• aggiorna i LED di feedback (su shifter o matrice)
• può aggiornare vari stati interni legati agli ITEM.

In pratica i messaggi in ingresso possono:
• accendere o spegnere pad
• muovere indicatori su encoder/pot
• sincronizzare lo stato del controller con quello dell’host.

7.3 Comunicazione con l’editor

L’editor usa un protocollo MIDI per leggere/scrivere i preset nello sketch.

Un messaggio speciale apre/chiude la modalità editor:
• openeditor = 1 → il firmware si mette in ascolto dei dati di preset
• openeditor = 0 → ritorno alla modalità normale

In modalità editor:
• i dati ricevuti vanno a popolare le tabelle (typetable, valuetable,
maxvalue, minvalue, modetable, lightable, qwertyvalue, ecc.)
• i preset sono compatibili con le pagine A/B (PAGE 1 / PAGE 2) dei 60 ITEM.

EEPROM, preset e pagine A/B

La libreria _DART_EEPROM.h gestisce letture/scritture sulla EEPROM.

D_STP_Presets.ino contiene le funzioni di:
• caricamento dei preset da EEPROM all’avvio
• salvataggi/note relative ai preset “di fabbrica” o di test.

Struttura logica:
• ci sono due pagine operative A/B per ciascuno dei 60 ITEM:
- corrispondono alle PAGE dell’editor (prima e seconda pagina
dello stesso slot).
• esistono zone di EEPROM dedicate a parametri generali:
- impostazioni di touch
- general_mempos
- eventuali parametri mouse_mempos e altri slot speciali.

Se all’avvio non viene trovato un preset valido in EEPROM:
• eeprom_preset_active rimane a 0
• viene attivata la modalità AUTODETECT (vedi punto successivo).

Varianti hardware, NOMOBO e modalità AUTODETECT

Lo sketch è progettato per funzionare su più controller usando un unico codice:
• define stratos:
- 0 → versione DART classica con 4051 e layout standard
- 1 → versione STRATOS, basata su Pro Micro con multiplexing via diodi
• define Matrix_Pads:
- 0 → nessuna matrice MAX7219
- 1 → matrici attive per pad/indicatori
- 2 → variante storica per una vecchia gestione con sensori di forza
e matrici (uso raro, mantenuto per compatibilità)
• altri define (hid_keys, hid_mouse, top_spinner, side_spinner, ecc.)
permettono di attivare o disattivare funzionalità intere a compile-time.

Modalità NOMOBO:
• quando valuetable[general_mempos] è diversa da 0, il controller entra
in una modalità “ridotta” NOMOBO.
• in questa modalità viene sfruttata soprattutto la dotazione base
di pin dell’Arduino Uno/Leonardo, senza appoggiarsi a tutta la
complessità di shifter, matrici e sensori extra.
• è utile per usare lo sketch su hardware molto semplice o durante
test rapidi su una scheda “nuda”.

Modalità AUTODETECT:
• se all’avvio non viene trovato un preset nella EEPROM
(eeprom_preset_active == 0), lo sketch entra in modalità AUTODETECT.
• in questa modalità:
- AIN() osserva direttamente il comportamento elettrico degli ingressi:
pulsanti: letture che scendono sotto la soglia quando premi
potenziometri: valori “stabili” e continui lungo il range
- in base a queste osservazioni, lo sketch:
- sceglie un mode adeguato (button / pot)
- imposta soglie, min/max e parametri interni sensati
- assegna automaticamente messaggi MIDI “di base”.
• Risultato: anche senza aver caricato un preset dall’editor, un controller
cablato correttamente può iniziare a funzionare “subito” in modo
ragionevole, appena si carica lo sketch.

// Fine panoramica
// Questo blocco serve solo come mappa mentale ad alto livello.
// Per aggiungere nuove funzioni conviene:
// - partire da ain_nucleo() (nuovo mode),
// - collegare il modulo in D_MOD.* / D_MOD_ENC.* / D_MOD_mouse.*,
// - estendere eventuali LED in D_LED.ino / D_mtrx.ino,
// - e, se serve, adattare preset/editor in D_STP_Presets.ino e D_MIN.ino.
*/


/*
 * 
 * 
STRUTTURA EEPROM – SITUAZIONE ATTUALE (DART_SKETCH)
approfondimento

Questo commento riassume come sono organizzate in EEPROM le tabelle
principali (typetable, valuetable, modetable, ecc.) nelle due pagine
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

typetable[] → tipo messaggio + canale MIDI (note/cc/pc/at, ecc.)

valuetable[] → valore di base (nota, CC number, ecc.)

modetable[] → mode (button, pot, encoder, toggle, ecc.)

dmxtable[] → canale/parametro DMX (0 = DMX disattivo)

minvalue[] → minimo o parametro extra (soglia bassa, range, ecc.)

maxvalue[] → massimo o parametro extra (soglia alta, range, ecc.)

qwertyvalue[] → extra vari (qwerty-mode, flag interni, ecc.)

lightable[] → mappa/indirizzo LED + opzioni luce

In RAM, per la PAGE 2, si usano spesso gli offset +max_modifiers:

typetable[i+max_modifiers] → tipo pagina 2

valuetable[i+max_modifiers] → valore pagina 2
mentre modetable/min/max/dmx/qwerty/lightable restano accessibili
tramite la variabile globale "page" (0 o max_modifiers).

MAPPA EEPROM – BLOCCO 0 (PAGE 1)

Indirizzi 0–511 → contenuto PAGE 1.

Per i cicli con i = 0..59 (max_modifiers):

0–59 : typetable PAGE 1
typetable[i] = EEPROM.read( i );

64–123 : valuetable PAGE 1
valuetable[i] = EEPROM.read( i + 64 );

128–187: modetable PAGE 1
modetable[i] = EEPROM.read( i + 128 + (0*512) );

192–251: dmxtable PAGE 1
dmxtable[i] = EEPROM.read( i + 192 + (0*512) );

256–315: minvalue PAGE 1
minvalue[i] = EEPROM.read( i + 256 + (0*512) );
(in alcuni casi remapper() viene applicato a questi valori)

320–379: maxvalue PAGE 1
maxvalue[i] = EEPROM.read( i + 320 + (0*512) );

384–443: qwertyvalue PAGE 1
qwertyvalue[i] = EEPROM.read( i + 384 + (0*512) );

448–507: lightable PAGE 1
lightable[i] = EEPROM.read( i + 448 + (0*512) );

60–63, 124–127, 188–191, 252–255, 316–319, 380–383, 444–447, 508–511:
zona non utilizzata in modo strutturale (piccoli “cuscinetti” tra le fasce).

Riassunto blocco 0:
0– 59 typetable (PAGE 1)
64–123 valuetable (PAGE 1)
128–187 modetable
192–251 dmxtable
256–315 minvalue
320–379 maxvalue
384–443 qwertyvalue
448–507 lightable
508–511 attualmente non usato in modo critico

MAPPA EEPROM – BLOCCO 1 (PAGE 2)

Indirizzi 512–1023 → contenuto PAGE 2.

La struttura è identica, traslata di +512:

Per i = 0..59:

512–571 : typetable PAGE 2
typetable[i+max_modifiers] = EEPROM.read( i + 512 );

576–635 : valuetable PAGE 2
valuetable[i+max_modifiers] = EEPROM.read( i + 64 + 512 );

640–699 : modetable PAGE 2
modetable[i] = EEPROM.read( i + 128 + (1*512) );

704–763 : dmxtable PAGE 2
dmxtable[i] = EEPROM.read( i + 192 + (1*512) );

768–827 : minvalue PAGE 2
minvalue[i] = EEPROM.read( i + 256 + (1*512) );
(stesso discorso del remapper)

832–891 : maxvalue PAGE 2
maxvalue[i] = EEPROM.read( i + 320 + (1*512) );

896–955 : qwertyvalue PAGE 2
qwertyvalue[i] = EEPROM.read( i + 384 + (1*512) );

960–1019: lightable PAGE 2
lightable[i] = EEPROM.read( i + 448 + (1*512) );

Anche qui, le posizioni corrispondenti agli indici 60–63 di ogni fascia
restano piccoli spazi di margine, non usati come parte del “rettangolo”
principale di 60 elementi.

Riassunto blocco 1:
512–571 typetable (PAGE 2)
576–635 valuetable (PAGE 2)
640–699 modetable
704–763 dmxtable
768–827 minvalue
832–891 maxvalue
896–955 qwertyvalue
960–1019 lightable
1020–1023 (fuori range, non accessibili su 1KB EEPROM)

RUOLO DI load_preset_base() E load_preset()

load_preset_base():
• legge typetable e valuetable per entrambe le pagine:
- PAGE 1: da 0 / 64
- PAGE 2: da 512 / 576
• serve per inizializzare i “tipi” e i “valori base” di tutti
gli ITEM in RAM, così da avere subito le info minime di entrambe
le pagine (anche se al momento viene usata solo PAGE 1).

load_preset( numero ):
• numero = 0 → PAGE 1
numero = 1 → PAGE 2
• in base a numero, legge:
modetable, dmxtable, minvalue, maxvalue, qwertyvalue, lightable
dagli offset:
+128, +192, +256, +320, +384, +448
aggiungendo (numero*512) per selezionare il blocco giusto.

• dopo aver caricato modetable[i], chiama setup_mempos(i) per:
- individuare distance_mempos, encoder_mempos, touch_mempos,
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
*/
// 24 11 25 - merge con aggiornamento funzioni void pushbuttons - adesso i velopads possono essere settati in toggle - correzioni varie anche su outnucleo

// 18 11 25 - oggi ho fatto un allineamento alle impostazioni da editor per gli effetti led dei pot - attuale : 0 no effetti, 1-32 blinker con numero lightable che corrisponde al led che blinka, >32 effetti led a 16led
  // eliminato il #define fader_type

// ho sistemato le cose per fare funzionare fastanalogread a pieno regime: mi serviva una variabile per la finestra temporale e ho usato qwertyvalue - 

// 13 11 25 - ho raqggiunto un buon funzionamento dei pad velocity sensitive con soglie ben studiate per attivazione dell'accensione pulsante e registrazione della velocity
// tutto si gioca con le soglie upper_val e lower_val - se si tratta di pulsanti normali abbiamo una lettura digital quindi le soglie 900 400 vanno benissimo per tutto 
// se si tratta di velocity sensing ho scelto di usare minvalue come selettore di sensibilità del pad - più aumenta minvalue più la soglia di trigger si abbassa e il pad diventa sensibile.
// la velocity viene adattata al range di trigger minimo, quindi, vicino alla soglia avremo velocity bassa e lontano dalla sogli velocity alta - il range è adattivo

// 



// 10 11 25 sostituisco analogread() con analogread_1024() in tutto lo sketch 
 
 
 /*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
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
//      sprite        = lightable[chan] → quale “slot” / sprite mostrare.
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
 
