
// 1.64 risolto problema pagina vuota.
// il problema del fatto che alla classe di dario il controller smetteva di funzionare dopo aver uppato un preset dipendeva dal fato che il controller si avviava su una pagina vuota.


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
