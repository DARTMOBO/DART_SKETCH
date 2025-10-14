
/*
 * 
 * setup pronto per LIME old
 * 
#define shifter_active    0         // 1 = enabled // 0 = disabled // SHIFT REGISTERS_ // if enabled, Matrix_pads must be disabled
#define stratos  0                   // 1 = enabled // 0 = disabled // Stratos sketch version.
#define LED_pattern 3                // 0 = dart one // 1 = kombat // 2 = NB-boards // 3 = Kombat-NB // 4 kombat-NB2 - // 5 KOMBAT-NB3 // 6 kombat personal - Led animation pattern used by buttons and pots
#define capacitivesensor_active 2   // 1 = enabled // 0 = disabled // 2 = EXTERNAL TOUCH IC on pin 7 - 9 // 3 = EXTERNAL TOUCH IC on pin 7 - 8 // 4 = EXTERNAL TOUCH IC (inverted) on pin 7 - 9--- CAPACITIVE SENSORS_
#define touch_version 1              // 1 = 680k //  2 = 10m //     resistor settings for touch sensing circuit
#define touch_led_onboard 1        // 1 = led output  on arduino pin 8 // 0 = disabled
#define DMX_active    0             // 1 = enabled // 0 = disabled // enable-disable also from _DART_Dmx_out.cpp !!!!!!!!!
#define Matrix_Pads 2             // 1 = enabled // 2 = pads on 17-32 circuitposition (old)// 0 = disabled // max7219 chips
#define hid_keys 1                   // 1 = enabled
#define hid_mouse 1                  // 1 = enabled
#define top_spinner 1                // 1 = enabled // 0 = disabled // TOP SPINNER
#define side_spinner 1               // 1 = enabled // 0 = disabled // SIDE SPINNER
#define note_off 0                   // 1 = enabled // 0 = disabled // send NOTE-OFF messages on button release -  if NOTE Type has been selected
#define pullups_active 1             // 1 = enabled // 0 = disabled // pullup resistors
#define page_active 1                // 1 = enabled // 0 = disabled //  page_switch
#define page_LEDs 0                  // 1 = page LEDs active
#define MIDI_thru 0                  // 1 = MIDI Thru active
#define mouse_block 1                // 1 = enabled // 0 = disabled // mouse messages are stopped after 2 seconds of repeated activity
#define arrows_block 0               // 1 = enabled // 0 = disabled // arrow key messages are stopped after 2 seconds of repeated activity
#define encoders_generic 1           // 1 = enabled 
#define MIDI_IN_block 0              // 1 = MIDI IN blocked
#define MIDI_OUT_block 0             // 1 = MIDI out blocked
#define fader_type 0                 // 0 = normal // 1 =  scratch style fader - experimental - leave it to 0
#define LED_rings 0                  // 1 = LED rings active - experimental - leave it to 0
#define blinker 1                    // blink effect on a selected led depending on pot position
//---------------------------------------------
 */

// 14 42 8 
// 38

// AGGIUNGO FUNZIIONE BLINK ANCHE PER MATRIX DART - funziona solo sul led 8 onboard

// voglio provare a fare un effetto blink - uso typetable[general_mempos] - funziona abbastanza bene - da testare sul campo

// 4 5 25 - ho aggiunto possibilità controllo led da pot - per segnalare che un effetto è attivo - esempio filtro di trktor


// 1 5 25 - ho completato la funzione shifter all - con possibilità di scegliere di quanto shiftare - da testare
// tab d_out riga 36 inserisco " || cmd != old_cmd " per tentare di risolvere il problema del filtraggio degli encoder , da testare
/* 18 4 25 - ho fatto in modo che il controller si avvia sulla pagina 1 con led indicatore acceso  ( quando pageswitch è n button  - se levetta è forzto dalla sua posizione)
 *  
 *  7 4 25
 *  ho fatto in modo che se c'è il touch stop attivato, non venga triggerato l'effetto led involontariamente
 *  tab d_mod_enc riga 238
 *  
 *  7 1 25 
 *  alla tab d_mod riga 1262 olgo l'azzeramento di encled perchè non fa funzionare bene l'effeto visivo degli spinners in modalità endless
 *  
 *  28 12 24 
 *  sto cercando di capire perchè scale non inverte il verso di rotazione in base a speed
 *  d_mod_enc- 407 ripristinare   481 rallentatore ripristinare
 *  
 *  24 12 24
 *  ho reso più semplice attivare al mousewheel, basta selezionare a memoryposition dell'enc scelto
 *  ho unificato il verso della mousewheel con quello degli enc
 *  
 *  
 *  22 12 24 - oggi ho fatto queste correzioni: 
 *  - ho unificato il senso di rotazione dello spinner / encoder per tutte le modatità (endless , pot , pot ramp , scale)
 *  - ho fatto in modo che minvalue/speed possa influire sulla velocità in endless mode 63-65 
 *  - ho sistemato la modalità qwerty per encoder , che aveva tutta una serie di problemi - comunque si può scegliere il carattere ascii da mandare 
 *      in base al valuetable - quindi non solo B e D
 *  * 
 * -----------------------------------------------
 * 
 * dopo varie sperimentazioni su Stratos ho aggiunto delle funzionalità che rendono più stabile il sistema
 * 
 * una funzione debounce sui punsanti che evita doppi segnali quando si ha a che fare con pulsanti di scarsa qualità
 * in questi casi se si preme il pulsante con poca forza lo switch non fa bene contatto e si ha l'invio di una gran quantità di segnali midi acceso spento
 * e allora ho aggiunto un valore fisso di decay : byte lastbutton_debounce
 * 
 * il valore di questa variabile cambia a secondase si tratta di stratos o altro controller, perchè stratos ha uno scanning più rapido e il decay dura meno
 * tale variabile viene usata solo da void push_buttons er ora
 * 
 * -----------------------------------------------------
 * 
 * in statos ho ripetuto più volte la accensione dei diodi per le linee di controllo dei due diversi gnd
 * perchè c'erano interferenze con interrupt del main encoder, che finiva per triggerare segnali dei pulsanti o del page switch
 * 
 * -------------------------------------------------------
 * 
 * ho aggiunto un fitro sul midi out - vedi void noteOn 
 * mettendo filter su 2 si può fare in modo che il segnale in uscita di encoders non di alta qualità sia più pulito
 * (devo vedere 0 o 127 puliti che escono fuori a seconda della direzione di rotazione)
 * tale filtro è anche utilissimo per evitare segnali indesiderati sul top spinner, causati sa spostamenti microscopici a ruota ferma
 */


/*
 * ho notatoche c'erano interferenze in stratos- tra encoder main  , legato a interrupt, e vari pulsanti
 * credo che gli interrupt abbiano un efetto particolare sulla struttura di stratos, con le sue letture su due gnd diversi comandati da diodi
 * in ain stratos ho provato a usare analogread al posto di digitalread, proprio perchè analogread è più lento... ma non cambia niente
 * invece i risutati migliorano quando ripeto l'accensione delle serie di diodi gnd sui pin 14 15
 * 
 * -------------------------------------
 * sto lavorando su stratos- sto notando che ci sono svariate interferenze tra enc principale e vari pulsanti
 * ciò accade sopratutto sulla prima pagina - 
 * credo che le cause siano perlopiù elettriche fisiche
 * 
 * possibili prove: aggiungere un condensatore generale per tutto il sistema
 * aggiungere condensatori specifici sui pulsanti
 * diminuire la frequenza delle letture (in statos è molto veloce)
 * 
 * gli encoder generici hanno letture molto sporche comunque - da vedere se gli encoder bourns che sono undetented di fabbrica, vanno meglio
 * 
 */


/*
 * aggiungo un Define per disabilitare il side spinner da sketch.
 */

/*
 * lc.sendRow2 da sempre problemi con simbolini che non vengono visualizzati correttamente
 * pare che il sistema digerisca solo il controllo di una matrice per volta... 
 * e mi chiedo come si possano creare creitte scrolling sofisticate con una trasmissione dati così incasinata
 * 
 * l'alternativa che mi viene in mente è diluire con un cycletimer il refresh delle matrici un pò come facevo con gli effeti led dart classici
 * in tal modo quando c'è qualche cambiamento di pulsanti o potenziometri non ci dovrebbero essere reazioni ritardate
 */


/*
 * a che serviva void autosend???? l'ho tolta
 */


/*
 * pot emulation : speed 1 è velocissimo, 32 non succede niente(correggere), 31 molto lento (quindi invertire, 1 deve essere lento)
 * poi non funziona l'inversione con speed negativa
 * 
 * endless mode 1 cioè 0-127 
 * 127 dovrebbe seguire il senso di rotazione, è anche l'efetto visivo
 * il controllo speed funziona ok , valori vicino a zero diminuiscono la velocità dell'effetto visivo
 * 
 * il touch stop funziona al contrario...
 */

/*
 * dovrebbe essere possibile invertire la progressione della scala tramite speed
 */

/*
 * sto studiando i problemi relativi al caricamento di una scala . ho notato che all'accensione la variabile scale[] non viene aggiornata
 * poi c'è anche il problema che maxvalue è utilizzata del funzionamento interno degli encoder
 * quindi bisognerebbe fare riferimento alla eeprom ogni volte che si deve caricare scale[]
 */



/* ancora sulla funzione scale - ho visto che diminuendo la velocità , sempre con mouse wheel speed counter
 *  vengono saltate alcune note della scala...
 * 
 * ///////////////////////////////////////////////////////////////////////////////////////////////////////
 * sto cercando di migliorare la funzione scale 
 * intanto devo osservare che la trasmissione della scala fissa da editor ha qualche problema... 
 * sembra che 5 bit vengano trascritti bene e gli altri 7 invece no.
 * per fare queste verifiche ho modificato void update_scala 
 * inserendo dei serialprint per vedere effettivamente i bit dei numeri mandati
 * 
 * 
 * 
 * ma la cosa più grave è la suonabilita  - a seconda della speed dello spinner la scala viene suonata o no e a volte si blocca tra due note da suonare
 * vado a vedere ind_mod_enc -  void pot mode - riga 304 circa
 * come fnzionano le cose
 * 
 * intanto
 * mouse_wheel_speed_counter - qui è usato come rallentatore regolato da SPEED via editor 
 * come? piu la cifra è alta (in negatovo o positivo è uguale) più il rallentatore rende RARI gli eventi midi
 * 
 * intanto per semplicità elimino questa condizione/rallentatore - quindi tolgo mouse_wheel_speed_counter dal gioco - riga 308 d_mod_enc
 * 
 * incontro encoder_block[] altra variabile condizione che dovrebbe eitare che suonino note mentre si registra una scala
 * per adesso tolgo anche questa condizione - riga 330 d_mod_enc
 * 
 * alla riga 352 vedo   encoder_pot_calcolo(numero,1); - il moltiplicatore è settato su 1
 * provo a metterlo su 33 - perchè il moltiplicatore generalmente è un valore 0-64 che viene usato con un -32 per ottenere un range positivo o negativo
 * 
 * ok , una parte dei problemi è risolta, ma adesso bisogna tornare al rallentatore
 * perchè le note vengono effettivamente sparate fuori troppo in fretta
 * 
 * dunque ristabilisco il rallentatore mouse_wheel_speed_counter
 * ma con valore fisso 32
 * 
 * cosa succede ? effettivamente le nte sono più separate tra loro temporalmente ma... ne vengono saltate tantissime
 * insomma impostando così il rallentatore si saltano note (il che potrebbe essere anche cosa buona,se ben regolata) 
 * io credo che bisognerebbe mettere il allentatore proprio in encoder_pot_calcolo
 * in questo modo servirebbero una serie di "scatti" ( si potra scegliere quanti) a vuoto che lo spinner deve fare 
 * prima di suonare una nuova nota, che sarà comunque sempre disponibile.
 * 
 * provo intanto il rallentatore fisso con quota minore 8
 * un po meglio ma siamo sempre lì
 * 
 * vedo di rallentare proprio encoder_pot_calcolo
 * 
 * ok , funziona abbastanza bene... da sistemare il valore speed che si sceglie via editor...
 * bisogna modificare l'editor in modo che sia intuitivo il modo in cui si scegle la velocità
 * 0 o 1 dovrebbe essere lentissimo, mentre 32 dovrebbe essere velocissimo.
 * 
 * anche in modalità Ramped l'effetto funziona bene. da correggere l'editor per fare in modo che tutto sia ben chiaro... 
 * ci sarebbe una cosa che non mi convince:
 * il limite superioe e inferiore della scala, sia in modalità ramp che limitata, hanno come un vuoto, 
 * uno scarto temporale prima che la scala ricominci da sotto
 * credo sia qualcosa che ha a che fare con la nota che deve essere messa a disposizione per suonare 
 * 
 * encoder_pot_mode 
 * va a richiamare 
 * encoder_pot_calcolo 
 * 
 * ed è come se il rallentatore si moltiplicasse
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */


//touch stop corretto // touch decay nuovo

// adesso minvalue touchmempos, che normalmente è il valore di sensitivity del sensore touch, viene usato anche come valore di decay per il virtualtouch
// inserire istruzioni in editor e sul sito. per il decay


/*
 * sto lavorando sul nuovo touch sensor - esterno.
 * devo invertire il valore digitale in d_mod 842 ??
 * 
 * no, sia void touch sensor che void touch execute sembrano funzionare correttamente
 * 
 * il problema deve essere in void encoder
 * 
 * d_mod_enc riga 37 non mi convince il led che si accende col virtualtouch.... 
 * 
 * 
 * sto cercando di risolvere il bug del touch stop/shift che sul side spinner fa riferimento al touch 1
 * dai test che ho fatto modatable[numero2] dentro void mod_enc , relativamente al side spinner mi rislta 0
 * 
 * normalmente dovrebbe risultare 21 per il top spinner e 21 per il side spinner
 * 
 * 
 * ho neotato che il bug si verifica indipendentemente dal tipo di touchsensor - se interno o esterno
 * ma dipende dal preset che viene caricato in memoria.
 * --- provare a fare un confronto tra i due preset - per esempio speciale e kombat personal (anche kombat normale) osa cambia?
 * --- cos'è che fa risultare modetable dello sidespinner = 0 --- 
 * 
 * facendo prove cambiando la memoryposition su editor... mettendo una memoryposition sopra il valore 50
 * inspiegabilmente mi risulta finalmente un modificatore che ha una modetable = 22
 * 
 * è capitato anche che sparisse la modetable dello spinner principale - cioè non risultava nessun modificatore con una modetable = 21
 * è qualcosa che ha a che vedere con la memorizzazione rimappata dei preset? 
 * è qualcosa che riguarda la comunicazione editor - controller?
 * 
 * risolto - ho disattivato le riga 119 in d_stp_presets che andava ad azzerare la modetable di 45 e 37 cioè i pin dedicati allo scanning del side spinner
 * per evitare lo scanning di questi input da AIN avevo introdotto questa riga - ma a quando pare porta problemi... vediamo se così stiamo più tranquilli
 * 
 * 
 * 
 */


// 185 - bug fix - hypercurves la selezione del tipo di hypercurve non avveniva correttamente

// 184 - adesso il feedback midi to dmx funziona anche con i pots

// 183 - risolto contrasto con encoders generici e side spinner - il canale 5 dei multiplexers veniva saltato - gli encoders collegati su 1 , 9 17, 25,33 non venivano letti


// 181 risolto il problema tra b6 (input in posizione reale = 0 ) e lo spinner1 (quando non viene dichiarato nel preset.
// da adesso , quando encoder_mempos[0] = 0 il controller va a verificare che non ci sia un conflitto con un item in memoryposition 0 
// ved d_stp_presets riga 60 

// 181 ho inserito il pattern led kombatNB - 


// 1.80
// bug: se in un prest manca il touch 2 (come nel korva noir) 
// - si creano problemi -- c'è un invio continuo di messaggi midi 176 1 0 - che rallentano tutto     // fixed

// modifico d_mod_enc riga 95

// 1.79 ho limitato il feedback visivo ai soli pulsanti // modetable < 11

// il feedback led ha dei problemi sui pulsanti extraplex.. e un'altro problema riguarda il fatto che si accende un led b20 quando viene ricevuto un CC


// 1.78 ho eliminato un bug che riguardava il mouse, che non aveva a disposizione una lettura sulla variabile "valore" per funionare
// inoltre ho stabilizzato il centro del joystick, evitando spostamenti minimi del mouse quando il joystick non tiene bene il centro.



//   1.77
//   ho modificato shifter.h // byte _shiftRegisters[24] adesso è byte _shiftRegisters[8]; 
//   void setPin(int index, boolean val) adesso è  void setPin(byte index, boolean val);
//   ho modificato shifter.h

 // correggere - mancano gli offgroup sulla seconda pagina - lato feedback dall'esterno

// 1.76 è possibile scegliere dal main tab quale sarà il pattern per le animazioni LED

// 1.75 ho aggiustato il led feedback, adesso può essere ricevuto da traktor senza problemi anche a ridosso della pressione di un pulsante.

// sistemare e testare feedback anche su radiogroups e su entrambe le pagine

// d_min riga 90 - non ricordo perche old_pitch viene settato a 126 !

// 1.74 inserisco l'opzione note-off nel tab principale

// 1.73 il pulsante b6 (posizione 0) aveva altri conflitti col pageswitch (qualora non fosse attivo come item nell'editor... adesso dovrebbe andare
// 1.72 problema risolto sul pulsante B6 che corrisponde alla circuitposition 0 
// se l'encoder principale non viene inserito (non c'è il relativo item , nel preset editor) succedeva che lastbutton veniva portato a 64
// ciò impediva di funzionare al pulsante
// alla riga 224 di  d_stp ho ovviato al problema, ma bisognerebbe associare delle memoryposition certe agli item "critici" come l'enc principale
// attualmente se non viene espresamente disattivato l'enc principale ( dall'item General) il problema può ripresentarsi.

// 1.71 ho sistemato il led feedback nel caso in cui il numero led specificato sia 0 - adesso nessuna luce si accende se led è zero

// il midi thru non mi pare sia programmato in modo serio... bisogna migliorarlo se lo si vuole offrire

// i movimenti del mouse hanno una sorta di ritorno indietro stranissimo e molto fastidioso, capire perchè - risolto

// 1.70 ho sistemato il problema del mouse, se mancava l'item MOUSE sull'editor, succedevano azioni mouse e freccette indesiderate
// accadeva perchè la variabile mouse_mampos veniva inizializzata sul valore 58, non ricordo perchè... 
// adesso rimane sempre zero e viene riportata a zero dopo ogni upload

// verificare che l'input 6 che non funziona al tipo che mi ha scritto via mail, non sia una cosa dovuta all'attivazione dell'encoder secondario.

// 1.69 tutti i toggle funzionano senza bug e senza rischi premendo più pulsanti.
// 1.68 al momento ho sistemato la gestione del side spinner leggendolo dentro AIN una sola volta per ciclo, come un normale encoder
//
// muovendo il main encoder vengono inviati fuori segnali anche del side spinner... non disturbano molto ma bisogna capire perchè!
// problema elettrico?
// no, il problema naturalmente è sul firmware, ha a che fare con gli interrupt del main encoder che influenzano la lettura del side spinner
// ho notato che se la lettura dell'encoder secondario (con caricamento delle variabili msb lsb) è immediatamente preceduta da void setplexer, funziona bene

// possibile rinominare in modo impersonale le tables , perchè può essere spesso fuorviante vedere una table che si chiama DMXtable, utilizzata per altro...

// 1.67 provare a fare modalità radio button - ok fatta , da perfezionare

// la modalità toggle e radio unziona male quando si premono più pulanti insieme - risolto

// sembra che il touch sensor riporti sempre l'animazione dei led a zero, anche senza touch reset
// sezione _mod riga 502 - per ora disattivato il reset dell'animazione

// fare in modo che se non è presente l'item "mouse" in un preset... il mouse sia spento di default

// C'è UN PROBLEMA COL LED FEEDBACK - i potenziometri fanno accendere i led dei pulsanti! - ved korova


// 1.66 - risolto il problema della lettura valori del mouse X Y . il MODEtable di X e Y vengono portate al valore 27, che serve solo per fare leggere il valore analogico

// 1.64 risolto problema pagina vuota.
// il problema del fatto che alla classe di dario il controller smetteva di funzionare dopo aver uppato un preset dipendeva dal fato che il controller si avviava su una pagina vuota.
// se la seconda pagina non veniva compilata e il page switch veniva associato alla memoryposition 0, il controller poteva partire e non dare più sgni di vita...



// min 316... non credo che le corrispondenze dei pin siano esatte... non vanno rimappate?


// problemi... ogni volta che uppo il general settings in modalità BLIND, devo spegnere e riattaccare il dispositivo per avere la modalità autodetect.
// non dovrebbe succedere, 
// inoltre, si è verificato quel problema descritto da dario, dopo 'upload tutto rimane completamente inattivo!


// si potrebbe aggiungere la possibilità di scegliere se l'encoder generico debba controllare un proprio ledring o inluire su ENCLED come fanno gli spinner
// si potrebbe dare la possibilità di assegnare un ledring a uno spinner (si tratterebbe di riversare encled su un lightable[] che la funzione LED_rings possa leggere.


// i led rings non funzionano bene quando è attiva la funzione DMX out, la luminosità è incostante.

// la void dmxSendByte si potrebbe rendere dipendente da una variabile regolata da editor, 
// in questo modo si potrebbe automaticamente eliminare il send di messaggi dmx quando vengono attivati i ledrings 

// v1.63 i touch sono indipendenti dalla quantità di encoders attivi, dipende se nel preset ho attivato un item touch- touch_mempos[] deve esser > 0 per attivare


// ogni tanto il touch smette di funzionare, deve essere il fatto che la autocalbrazione del minimo e del massimo viene ritriggerata dal page switch,
// si dovrebbe ideare un modo per ripristinare il touch 

// è possibile mettere un argomento [] alla funzione void touch_sensors, in questo modo si possono saltare del tutto, quando non è presente un touch nel preset.
// indexxen bisogna metterlo fuori, è un contatore usato per entrambi i touch.

//  if ( modetable[i] == 24) touch_mempos[1] = i;

// il note off mi manda un segnale off in più


// POSSIBILITà disattivare solo gli shifters e non i multiplexers. avendo accessoa ai solo pin digitali d10 d11 d12 ler led attaccati direttamete alla board. basta mettere valuetable[general_mempos] su 2

// risolto problema relativo al riposizionalemte dell'encoder secondario. si annullava la lettura del fader (dart one)

// la nuova funzione qwerty sui pot , porta problemi in modalità autodetect. da risolvere - ok dovrebbe essere risolto.

// idea, i pas si potrebbero velocizzare usando un digitalread come soglia, dopo il digitalread si possono fare i susseguenti analogread..

// v1.60 adesso si possono collegare tanti encoders - l'encoder secondario (side spinner) va collegato a pins 33 e 41 - da testare, ma dovrebbe funzionare molto meglio

 
// 1.59 aggiunta possibilità controllo due leds con page switch

// voglio provare a controllare due led con lo switch page - uso ledcontrol(chan,1);

// 1.58 - aumentata velocità ciclo tramite letture digitali

// ho fatto modifiche necessarie per poter scegliere la seconda key , nella emulazion hid dei pot

// i.57 aggiunto funzionalità qwerty su pot

// sto lavorando su una modalità qwerty per i pulsanti

// dispositivo anti blocco del mouse fatto!

// per fare un antiblocco mouse posso usare qwertyvalue[mouse_mempos]

// testare corrispondenza feedback led in modalità nomobo
// 1.55 aggiustato la corrispondenza in numerazione del led on/off del touchsensor - 
// led feedback was turning on the wrong output pin - now solved  

// risolto problema touch  sensor 20 5 19

//                                                     
