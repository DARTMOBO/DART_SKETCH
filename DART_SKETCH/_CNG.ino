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

//1.69 tutti i toggle funzionano senza bug e senza rischi premendo più pulsanti.
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
