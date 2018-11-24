// corretto un piccolo problema on animazione led del pot mode - non veniva usato valuepot per calcolare l'efetto.
// inserisco possibilità di cambiare impostazioni touch - #define touch_version 1 
// autodetect funzionante.

// la dmxvalue è libera per potere aiutare l'autodetect
// adesso non c'è uscita dmx nel default preset
// sto lavorando all'autodetector
// v63


// non capisco perchè per attivare il secondo spinner nel korova nero, ho dovuto mettere dmxvalue[general] a 5

// v62 aggiungo la accensione della pullup sull'extraplex , digitalWrite(9, HIGH);, da vedere se crea probemi al touchsensor t2

// update scala ha sicuramente dei problemi.. 
// già nella versione prima che cambiassi la funzione di maxvalue[enc]... in lastencoded, per contenere la scala
// ci sono dei riferimenti di lettura di maxvalue[] che vanno evidentemente fuori range... verificare

// ho cambiato funzione a lastencoded[] adesso serve come contenitore della scala scritta da editor... all'avvio iil valore viene caricato...
// da eeprom a maxvalue poi da maxvalue a lastencoded ... perchè maxvalue, negli encoder, adesso viene usato nella codifica (lastencoded)

//  void update_scala - fa uso di maxvalue[encoder] ... che sto usando per gli encoder semplici -  capire come evitare problemi

// forse il filto antidoppioni del noteon potrebbe essere reso più efficiente- 
// filtrando tipi di messaggio diversi... note e non note per esempio

// fase di debug: le modalità pot, pare non funzioni per niente.
// il touch 2 sfalsa.. forse è la sensitivity sbagliata. -  ma con lo sketch precedente non crea problemi


// senseEncoder_2nd può essere modificata per utilizzare digitalread al posto di analogread e map

// nuova regola - per attivare la mousewheel, usare la memoryposition dell'encoder scelto
// modifico prima tutta la parte endless - poi la void encoder_pot_mode()
// conminio i lavori per rendere encoder () moltipicabile.
// v 59 stratos 6

// far ein modo che encoder() possa accettare un argomento (chan) ovvero la emoryposition da cui andare a leggere i dati per il funzionamento.
// cosa distingue spinner ed encoder? la modetable... // istituire la modalità encoder sul numero 19 (modetable)

// touch stop, virtual touch e scale devono funzionare solo per gli spinner

// encodervaluepot[] che è un int si userà solo per gli spinner altrimenti si userà lightable[]*8

// in questo modo encoder() si può usare ovunque... basta prima leggere un encoder, mettere le letture in MSB[2] E LSB[2] 
// lastencoded sarà dentro maxvalue
// 

// no touch stop
// no scale
// no virtual touch
// si endless e pot mode

//  fondamentale moltiplicare 
//  lastencoded[] - che si può mettere in maxvalue
//  encodervaluepot - che si può mettere in lightable - a patto di farlo funzionare come int 
//  serve un mecanismo rallentatore


// inizio a lavorare a encoder multipli
// // la void encoder attualmente funziona con 0 e 1 - 
// 0 e 1 portano void encoder a leggere i dati di funzionamente dagli slot di memoria targati encoder_mempos 0 e 1
// nello slot di memorie di un encoder_mempo abbiamo le variabili :
// ** lastbutton : che normalmente contiene il numero neutro 64, quando un encoder si muove, questa variabile viene caricata con valori diversi
//              per poi ritornare a 64 dopo l'emissione del messaggio midi.
//                - moltiplicabile
// ** v_touch_regulator: di queste variabili ce ne sono due! come fare nel caso di encoder multipli? dovrebbe funzionare solo per i principali
// ** lightable[touch_mempos]  : anche di queste ce ne sono due sole - relative ai settaggi del touch - 
//                            per decidere se il touch è reale, virtuale, o monitor
//
// ** lightable [contoencoder] è LIBERA!!!
//
// ** dmxtable[general_mempos] : quali spinner sono attivati 0 nessuno 1 top 2 top+side
// ** dmxtable[contoencoder] : dove contoencoder è la emoryposition dell'encoder scelto... 
//                           - questa variabile decide la modalità: 
//                             0 endless 63/65 
//                             1 endless 0/127 
//                             2 pot/scale - se impostato su note e non su cc, viene inviata la scala di note
//                             3 scale learn
//                             - MOLTIPLICABILE
// ** qwertyvalue[contoencoder] : decide se il touchstop è 0 disattivato 1 attivato
//                                - moltiplicabile
// ** minvalue[contoencoder]    : speed- contiene un valore da 0 a 64 
//                              - il centro è 32 ed equivale a speed 0
//                              - sotto il 32 si ha una velocità negativa
//                              - in modalità endless la velocità negativa significa inversione max-min, e velocità dell'efx visivo
//                              - MOLTIPLICABILE
// maxvalue[contoencoder] è LIBERA!
//
// ** typetable[contoencoder] :   - MOLTIPLICABILE . tipo di segnale midi da mandare (note, cc etc etc)
// ** valuetable[contoencoder] :   - MOLTIPLICABILE . value del segnale midi da mandare 
// ** encled      : non moltiplicabile , ma va bene così. è una variabile usata al momento da tutti i modificatori.
//
// ** valuetable[mouse_mempos+page] == numero+1 : questa variabile è contenuta nello slot del mouse (presente solo una volta)
//                                                - decide quale dei due spinner agisce da mousewheel 0 nessuno 1 top 2 side
// void  encoder_pot_calcolo(numero) : 
//
// encodervaluepot[numero] : è una variabile con estensione 0-1024 che contiene il valore del POT VIRTUALE emulato dagi spinner in potmode
//                          - il touchreset rimette questa variabile nella posizione iniziale.
//                         - non moltiplicabile
//                          - l'unico motivo per cui è un int è per rendere più graduale l'aumento di value in pot mode
//                            per il resto viene utilizzato sempre come un byte
//                                                                                    *************************************************
//                         viene utilizzata in: 
//                              scale learn
//                              touch execute - nello smooth per il ritorno graduato a valore centrale, touch reset
//                              virtual_touch_end -  sempre per in ritorno alla posizione centrale nel touch reset
//                              encoder_pot_mode - per emettere proprio il midi caricato sulla variabile
//                                               - anche per enettere midi in scale mode
//                              encoder_pot_calcolo - per caricare la variabile del suo valore
//                              
// encoder_block[numero]  :  non moltiplicabile
//                          serve per migliorare la suonabilità in modalità scale
// scala_learn, at e tutte le variabili scala: non sono moltiplicabili

// lastencoded[numero] - non moltiplicabile - è fondamentale nella codifica           *************************************************
// volatile byte MSB[2]  e  LSB[2] : devono essere distinti [2] solo perchè l'enc principale usa gli interrupt.
//                                  - ma MSB e LSB [2] possono essere usati per tutti gli encoder in più
// 

// v 59 stratos 5

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// aggiungo il led del touch

// cambio decay_sensor da 2 a 100... il loop() è molto più leggero ed è più facile che un tocco leggero mandi più segnali acceso spento

// ho cambiato la resistenza del touch nello stratos . metto la 680k

// non si capisce proprio da cosa dipenda questa oscillazione
// neanche disattivando perennemente gli interrupt (cosa che crea comunque problemi) perchè il delay() non funziona) l'oscillazione va via
// usando un condensatore 101 sul pin sensore o altro... cambia l'ampiezza ma continua a oscillare.
// si potrebbe in effetti usare un trimmer da 1m... per tarare perfettamente il touch... ma una resistenza sembra qualcosa di molto più solido

// oscillazione letture touch: ho provato a togliere librerie midi keyboard mouse... niente l'oscillazione rimane.

// in setup() forzo la macchina a partire con aux preset() --- mi serve per fare test con altri dart

// ancora impasse col touch
// due problemi ... le lettura Ã¨ ondeggiante se toccato da 60 a 127 ... l'ondeggiamento non credo si una questione hardware
// inoltre adesso le letture rimangono fisse a 64 se attivo ain_nucleo - esecuzione buttons
// ho risolto mtendo un messaggio seria.print dentro il setup... misteriosamente funziona tutto
//
// oscillazione valori con  delay60 inserito nel loop:
// se attivo ain l'oscillazione Ã¨ piÃ¹ veloce circa 8 valori massimi e 9 minimi
// se disattivo ain 12 valori massimi 
// pare che questa oscillazione sia indipendente dal delay... se il delay Ã¨ 20 per esempio vedo dei valori molto scanditi
// se il delay Ã¨ 30, questa alternanza di valori bassi e alti appare caotica...
// 
// l'oscillazione posso vederla anche con un circuito meno sensibile
// il circuito meno sensibile sembra avere piÃ¹ separation ... se tocco la knob ... ma l'oscillazione rimane
// avere un circuito meno sensibile torna anche utile per sensorizzare con meno letture...
//
// perchÃ¨ attivare AIN modifica l'oscillazione ?? probabilmente il ritardo che introduce, 
// funge da fine tuning per cui si crea una maggiore sincronia, fortuita, 
// tra letture e oscillazione stessa
//
// forse potrei provare con condensatoi piÃ¹ appropriati (101) io ho provato il 104
// potrei provare anche con un diodo... chissÃ 


//lettoure touch sensibilissimo : 180 250 3000.
//pongo un limite di 510 (che diviso in due fa 255, perfetto per utilizzo con variabili int)
// faccio in modo che limit touch arrivi fino a massimo 255 - le letture sono doppie (possono arrivare a 510) in modo da leggere anche circuiti vecchi
// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// preparazione per stratos : tolgo dmx, tolgo shifter, 
// tolgo serial midi in (din), 
// tolgo void pads, 
// toglo tutta la libreria dmx
// sostituisco AIN con AIN_stratos, 
// tolgo void detect plexer
// tolgo void diversifica value
// tolgo setplexer
// tolgo righe 118 135 in setup(); // settaggio touch - lower value in setup()
// accendo le pullups stratos nel setup ... tolgo le pullups dartmobo
// tolgo setup riga 92 a 100 : settaggi per serial over din
// tolgo sense_encoder_2nd - anche da dentro ain_stratos...
// toglo riga 10 setup
// tolgo riga 14 a 26 setup // encoder input settings e midi over din settings
// modifico il default preset... tutte le note sono diseguali void aux_preset riga 288
// 59 stratos

// v59
// ho inserito un sistema di auto limiting per la lunghezza delle letture del touch, il sistema Ã¨ retrocompatibile... 
// con l'ultimo circuito che usa una resistenza da 680k, la lunghezza del while di sensing Ã¨ limitata a 60 (molto veloce)
// il touch Ã¨ abbastanza stabile

// trasformo la variabile total in un int. da testare se funziona
// modifico la void capacitivesensorRAW in modo da poter indicare un valore LIMIT per il numero di letture massime nel while del sensore
// cerco di snellire ancora capacitive sensor
// 58a

// versione 57 DARIETTO : pullups e dmx disattivati - encledtable con un il led 16 (il primo del terzo shifter) al posto del led 0 
//                      : il touch sensor Ã¨ composto con semplici resistenze da 680K una sola per ogni touch sensor.
//                      : anche i pads sono disattivati, ma da editor.


// ho fatto in modo che i led del touch sensor restano accesi anche se si gira l'encoder endless
// ho tolto delle variabili inutili nella libreria capacitive sensor guadagnando un po di memoria
// ho inserito di nuovo la lower_value che viene ricalibrata a ogni cambio pagina e all'avvio come higher_xen
// 57

// provato a disabilitare interrupt timer2 con comendo / TIMSK4 &= ~_BV(TOIE4); ... niente

// stratos - touch- circuito sensibilissimo - letture NO-touch, knob-touch, plate-touch: 180 260 3000
// metto un limite a 500


// ho provato a disabilitare gli interrupts (quindi anche del dmx) prima della lettura del touch ela void touch . quella mia . niente
// pare che il dmx metta in pausa il conto while del senseonecycle... 

// ci sarebbero due soluzioni da provare ... o interompere il timer2 mentre il while del touch sta contando..
// oppure inserire la lettura del touch all'interno del loop del dmx

// ho provato a scendere il timeout se sensore touch fino a 170 --- ma si perde troppo dettaglio
// --- aveve sceso il timeout fino a una soglia simile a quella minima a cui il dmx interromper periodicamente il touch sensing



// ho provato a inserire anche il while dentro la finestre nointerrupts in SenseOneCycle .... niente

// modifico capacitiveSensorRaw - adeso misura sempre solo un singolo ciclo

// disattivo encoder principale in ain - encoder[0]

// provo a odificare righe 15 e 17 in dmxsimple.h diminuisco il size a 64 - non cambia nulla


// qustione interrupt - ci sono molte cose nel dart e ci sono alcuni conflitti
// - il touch Ã¨ misorato diversamente se c'Ã¨ il libreria dmx attiva 
// elenco funzioni critiche per interrupt:

// main encoder
// dmx - usa i timer interrupt
// touch sensing
// midi in / out ?? da verificare

// questione include - piÃ¹ libreri inludono arduino.h e pins_arduino.h ... non Ã¨ che si somma tutta sta roba?



// il touch adesso ÃƒÂ¨ proporzionale funziona meglio. c'ÃƒÂ¨ meno rischio che non funzioni in differenti condizioni elettriche
// il page resetta la lettura "higher" 
// quindi se servisse, basta cambiare pagina per ricalibrarlo
// il loop di lettura ÃƒÂ¨ fissato a 630 ... che come massimo valore produce 78


// il valore LETTURA del touchsensor viene influenzato da:
// presenza o no di una libreria dmx
// se ci sono dei serialprint di monitoring
// dalle resistenze - al momento attuale sto ottenendo risultati migliori con due resistenze da 2,2m il sensore ÃƒÂ¨ piazzato al centro tra le due
// il touch ÃƒÂ¨ anche influenzato da quanto la vite della jog ÃƒÂ¨ stretta bene
// da quanto il plate riesce a condurre attraverso la vite
// ho provato un po di combinazioni con i condensatori... niente di fatto - ho provato i 470uf elettrolitici e i 104 piccolini... niente risultati



// dunque: riguardo al problemino del toggle che richiede due pressioni per essere rilasciato... questo succede quando dart ÃƒÂ¨ in momentary e traktor in toggle
// basta fare il settaggio correttamente - e tutto funziona

// risolto un problemino con la soglia minima del beam - la const deve essere un int - altrimenti il confronto con VALORE crea casini

// rimane da aggiustare leggermente il range, per chi vuole usare il sensore son pullup da 100k

// il sensore beam non vuole le pullup interne - che sono troppo deboli. 
// ----quindi ho fatto in modo che la lettura del beam escluda temporaneamente la pullup
// ----l'acensione delle pullup viene fatta con digitalwrite ... i numeri dei pin analogici variano tra leo e UNO
// ---- quindi ho usato delle dependencies per avere il numero giusto


// HO AUMENTATO IL TEMPO DI DETECT DEL CAPACITIVE SENSOR A 700 CICLI - VED CAPACITIVESENSOR.CPP - 111 126

// il detector cotinua a mandarmi in modalitÃƒÂ  diferenziata anche i midificatori collegati al'in analogico
/// perchÃƒÂ¨?
/// la lettura di un canale del plexer viene confrontata con la lettura del canale precedente
/// se le letture sono diverse... scatta
/// ma scatta anche quando non dovrebbe...
/// perchÃƒÂ¨ legge come diverse delle letture che dovrebbero essere uguali?
/// variazioni di voltaggio?
/// cambio di stato (pulsante rilasciato) tra una lettura e l'altra?



// funzionicchia - ma se premo il pulsante nel istante tra la lettura 6 e la lettura 5 (per esempio), ottengo valori diversi
// - invece se quando leggo l'input plexer6 lo confronto subito con una lettura plexer nuova (setplexer + analogread) sul 5 per esempio... 
// - ma quando farglielo fare? per non appesantire tutto il ciclo con mille letture e confronti

// detector-plexer fatto... da debuggare

// per decidere se un analog input ÃƒÂ¨ multiplexato ...
/// devo confrontare la lettura attuale con la lettura di un input diverso, ma all'interno dello stesso gruppo plexer
/// quindi chan-1... per esempio? se channel va da 0 a 7
/// se channel ÃƒÂ¨ 0 non posso confrontare con chan-1 perchÃƒÂ¨ finirei per sforare in basso l'array o leggere in un altro gruppo plexer
/// devo leggere chan-1 solo se chan ÃƒÂ¨ maggiore di 0
/// altrimenti leggo chan +1
/// dovrebbe funzionare

//
// sto usando un downgrade da 0-1024 a 0-32 nel registrare le letture su qwertyvalue, nel default preset, per un confronto che tenga conto di un minimo di noise
// 
// scala_array[6]; ÃƒÂ¨ perfetto - ma forse posso usare qwertyvalue
// servono sei byte ( piÃƒÂ¹ uno extraplex ?) per memorizzare la lettura valore precedente per ogni plexer
// adesso il default-preset parte con valuetable tutte uguali per gruppo-plexer - da testare
// comincio a fare il detector plexer
// 54

// aggiunto una riga che acende la pullup sul pin 9 se  l'extraplex ÃƒÂ¨ attivo ... da testare
// va testato su korova senza resistenza da 100k... 
// se non funziona (a causa del touchsensor) inserire nella documentazione la resistenza da 100k 


// add possibilitÃƒÂ  di disabilitare pullups - fatto

// add possibilitÃƒÂ  di eliminare plexers -da setup e da ain


// sto facendo chiarezza sulle memorypositions di default
// il mouse deve essere sempre dichiarato con la sua memoryposition? se in un preset (da editor) non c'ÃƒÂ¨ il modificatore mouse, 
// la mouse_mempos sarÃƒÂ  0
// e se la dmxtable[mouse_mempos] ÃƒÂ¨ 1 ... allora si attiverÃƒÂ  il mouse!
// per ora ho messo che la mouse_mempos deve essere > 0 ... altrimenti tutta la void muse_control non si attiva. ciao

// notare che la lightable[general memops] non ÃƒÂ¨ usata...
 
 // ho usato qwertyvalue per migliorare il detector - fatto
 
 //voglio migliorare la resa del detect_pot... vorrei un secondo stadio di lettura. ma non posso usare lastbutton... devo usare altro
// potrei usare lightable
// dmxtable
// qwertyvalue
 
 
 // aggiungo il detect pot // per controller diy - i pot vengono riconosciuti automaticamente
 //52
 
// il flashing delle luci quando si riceve un settaggio da editor ÃƒÂ¨ stato aggiustato.. ora flasha solo quando viene ricevuto un intero modificatore
// prima faceva un flash ad ogni messaggio midi
// 51
// andesso ÃƒÂ¨ facile escludere le librerie dmx e shifter
// 50

// #if (shifter_active == 1 && stratos == 0)
// #endif

// alleggerito un po la libreria capacitivesensor... io la uso in condizioni particolari e ho tolto parti inutili 
// rimane ancora qualcosa da alleggerire cmq

// risolto un problemino con la lettura dell'extraplexer su leonardo. (leggeva analog)
// l'array qwertymod ÃƒÂ¨ sgtato spostato sulla memoria flash tramite progmem... 
// 47

// test memoria:
// qwertymod 2 - 427
// qwertymod 33 - 395
// capacitiv 323
// buttonledfx - 319

// qwertymod 2 . 361
// qwertymod 33 . 327

// risolti problemi feedback in t-group e momentary

// problema con momentary feedback su ableton... ÃƒÂ¨ necessario premere due volte un tasto per attivare una clip precedentemente attivata

// adesso lo sketch ÃƒÂ¨ valido per UNO e per LEO
// 45 

// da sistemare il feedback su toggle group...
// adesso il feedback va bene su momentary e toggle con traktor e varie
// 43 

// ho inserito un dispositivo encoder_block che facilita le operazionii di scale learn... quando l'encoder viene tenuto fermo con la mano sul touchsensor, 
// vengono evitati messaggi non voluti , causati da lievi movimenti della jog

// problema - ne note ricevute col touch disattivato cancellano la scala - risolto
// ottimizzato lo smoothing del touch reset
// problema scale learn anche se touch non attivo. - dipendeva dal touch2 che aveva una soglia sbagliata
// problema - valori min e max fuori scala in scale mode. - risolto

// risolto un problemino con il touch reset e il midi in... le note in ingressoo resettavano la posizione dell'encoder in pot mode.
// aggiunto uno smoothing nel touch reset per un ritorno piÃƒÂ¹ morbido alla posizione zero
//v41


// aggiunto encledtable vector inverted
// aggiunto encledtable per vector
// v40

// corretto effetto visivo endless spin - non seguiva l'inversione del verso
// ho riscritto buttonledefx risparmiando parecchia memoria. da testare con beam pads e altre cose
// v39 

// adesso l'effetto visivo led della rotazione spinner in modalitÃƒÂ  endless puÃƒÂ² essere regolato dal valore speed (velocitÃƒÂ  effetto) 
// - valore normale 3 per optical 8 per analog
// v38

//  disattivaati i buttonledfx ancora
// istemato alcune incongruenze con l'accensione del led relativo al touch nella pagina 1 e 2
// adesso i pot non fanno effettini luminosi se led (lightable) ÃƒÂ¨ messo a 0
// v37

// adesso si puÃƒÂ² scegliere il led che segnala il touch (maxvalue touch)
// per la soglia del touch ci sarÃƒÂ  solo un valore (minvalue touch)
// v36

// sistemato il pitch bend con relatiovi effetti luce (controllato da potenziometro)
// al momento il problema memoria ed effetti pare risolto...
// 35

// all'accensione non ci sono piÃƒÂ¹ problemi col toggle... perchÃƒÂ¨ "lastbutton" viene inizializzata con tutti i 64 byte settati su 1
// 34

// il midifeedback converter in DMX aveva dei problemi nello spegnere le luci
// v 33

// gli effetti luminosi vengono attivati da qwertyvalue[general_mempos]
// 0 = solo pulsanti
// >0 = effetti pot
// >2 = effetti buttonfx

// v 32
// il note off in ener  //00::AAbbgy-xt include una velocity uguale a noteon... adesso il noteoff spegne i led

// c'era un errorino nel settaggio del numero di spinner attivi: anche senza secondo spinner venivano disattivati gli input 45 47
// -------------31
// adesso gli effetti led controllati dai potenziometri si spengono anche se virtualtouch ÃƒÂ¨ disattivato

// ho ottimizzato ancora il touch... adesso il decay ÃƒÂ¨ veramente efficace... prima non risolveva bene le intermittenze

//----------------
// adesso ÃƒÂ¨ possibile disattivare gli effetti uminosi (pot, encoder) tramite qwrtyvaue[generalmempos]

//migliorato accensione led dei touchsensors... adesso viene acceso il led anche nelle memorie... non ci sono contrasti con altri controlli

// sistemato vari problemi sul caricamento per preset ausiliario all'accensione -
//il primo pulsante aveva valori sballati perchÃƒÆ’Ã‚Â¨ general mempos si metteva in posizione 0

// elimino la selexione plexers o non plexers- ÃƒÆ’Ã‚Â¨ inutile e complessa

// adesso si usano 5 messaggi per il trasferimento da editor - meglio
// preset default (aux-preset) tutti note60 - 
// c'ÃƒÆ’Ã‚Â¨ una lightable di default tarata per funzionare con le controlboard classiche.
// 23  


// migliorato dettaglio pitch bend anche via encoders

// sto migliorando il trattamento del pitch bend
// lightable touch > 1 = no touch - niente.

// aggiustato movimento mouse tramite joystick. adesso ÃƒÆ’Ã‚Â¨ abbastanza veloce

// il beam in modalitÃƒÆ’Ã‚Â  scale dava problemi ai led e al feedback led. ora risolto

//possibilitÃƒÆ’Ã‚Â  di scegliere quanti plexer

//adesso il learn scale funziona correttamente - ancor col beam

// cerco di usare la procedura scale anche per il beam
// associo il valore beam a variabile AT
// scala[numero] avrÃƒÆ’Ã‚Â  numero dell'encoder nel caso degli spinner e numero fisso nel caso del beam

// ho inserito una riga in void setup_mempos che rporta la modetable a blind input, nei pin 45 e 47 se il secondo encoder ÃƒÆ’Ã‚Â¨ attivato.

// sostituisco tutti i "+1" con ++ 

// elimino buttonstate - sostituito con lastbutton[page_mampos]
// gli array BIT_status sono stati ridotti a 4


// ritestato il virtual touch - adesso sembra ok

// vorrei condensare ancora encoder - soprattutto relativamente all'invert - ma non ci sto riuscendo minimamente

// elimino byte scala_counter_speed; sostituita con la mouse_wheel_speed_counter

// ho condensato update_encoder 1 e 2 in una unica void

//testare virtualtouch - 
//reazionalizzare encoder endless mode - ci sono molte ripetizioni

// ho razionalizzato virtualtouch - uso incomingbyte (normalmet usata nel midiIN - come varibile per contenere boolean(page) 
// testare che non ci siano conflitti


// il mouse e le freccette funzionano - da ottimizzare

// inserire una qualche forma di azione che porta la modetable a BLIND 19 se joyx e joyy devono funzionare come freccette - fatto
// perchÃƒÆ’Ã‚Â¨ si crea un conflitto tra funzionamento analogico e digitale

// ripristinare mouse (eliminate mousex e mousey che erano float) e virtualtouch

//eliminare getvalue -fatto

//semplifico ledcontrol riga 8
//semplifico load preset - riduco i cicli for

// sto razionalizzanto tutta la gestione per BIT in multiarray chiamato bit_status

// bit_ledstatus diventa bit_status - lo faccio diventare un multi-array che ospiterÃƒÆ’Ã‚Â  : ledstatus1 ledstatus2. toggle1 toggle2, feedback1 feedback2;

// cerco di comprimere ledrestore in una unica procedura void - creo quindi un array di array

// ho abbassato l'array dedicato allo scale learn a 6 slot, risparmiando questo poco di memoria le cose vanno un pÃƒÆ’Ã‚Â² meglio

// dopo mille prove ho scoperto che con l'uso della memoria al 90% memoria dinamica- ci sono difficoltÃƒÆ’Ã‚Â  di caricamento del preset all'avvio.

// corretto piccolo errore in pageswitch - la media del touch veniva caricata in modo errato - e il touch non funzionava sulla page2 - ora corretto

//rivedere void beamefx - ho tolto switchvalue[distance ... i sarÃƒÆ’Ã‚Â  qualhe bordellino
// ristabilisco encodervaluepot (byte) al posto di switchstate

// ho notato che la memoria utilizzata da serial1 - per ricevere messaggi via DIN ÃƒÆ’Ã‚Â¨ di circa il 5% - TANTISSIMO

// provo a eliminare switchstate faccio tutto con lastbutton

// sto cercando di far funzionare insieme USB e DIN midi 
// pare che i problemi coincino a comparire uando la memoria dinamica utilizzata supera il 90%
// si potrebbe provare a ridimensionare l'array switchstate o anche eliminarlo
// si potrebbe ridurre modetable a 64 slot - da un breve sguardo pare fattbile


// ved x snellire bit processing con funzioni
// learn scale


// migliorato alcuni elementi del touch sensor... adesso ancora piÃƒÆ’Ã‚Â¹ stabile nella lettura. non ci sono ritorni a zero nella somma, nella media, e nella lettura 


// risolto problem escursione encled, quando si muoveva un pot o uno spinner...
// risolti i problemi con la gestione bit_ di toggle , feedback, e ledstatus
// of al momnto anche il feedback su momentary pare risolto... rimane quanche piccolo inghippo su feedback t-group ma niente di grave


/*
 sto cercando di risolvere la questioe feedback su momentary buttons
 se un comando (clip o pulsante in traktor) ÃƒÂ¨ in modalitÃƒÂ  toggle succede che su schermo il pulsante rimane acceso
 ma su pannello si spegne
 quidi si deve fare cosÃƒÂ¬:
 il segnale midi on off deve rimanere momentary... come sempre
 ma solo nel caso in cui in seguito alla pressione di un tasto viene ricevuto un segnale di accensione midi... 
 il controllo del led (solo del led non del segnale midi on off) deve essere subordinato al ricevimento di un segnale esterno di off
 
 */
 
 

// 46 ------ sistemato feedback su togle group
// problema : pagina 1 , b9 in modalitÃƒÆ’Ã‚Â  toggle , se b9 ÃƒÆ’Ã‚Â¨ acceso, molto altri pulsanti non accendono
// evidentemente anche con bit_toggle c'ÃƒÆ’Ã‚Â¨ qualche problema

// ho notato che cycletimer torna indietro periodicamente. quando raggionge 255 riparte da 0

// il virtualtouch continua a creare problemi - se da editor tengo premuto un modificatore per testare il feedback, dopo qualche secondo, oltre al led giusto...
// si accende un led sbagliato alrove



// CONTROLLARE OFFGROUP - HO CAMBIATO IL FOR - partiva da 1 e non da 0

// max_feedback adesso si puÃƒÆ’Ã‚Â² aumentare fino a 60 64// 
// il feedback led non funziona sul pulsante b10 - ok il problema era che molti FOR in MIDI_IN partivano da 1 e non da 0 - RISOLTO

// c'ÃƒÆ’Ã‚Â¨ qualche problema con il virtualtouch... il pulsante b10 (position 6 - che diventa 0) manda un segnale ripetuto a scatti... 
// ripetuto col timing del cycletimer
// er il virtualtouch2 che stava attivato e non essendo definito nell'editor... faceva riferimento alla posizione 0 - quindi il primo pulsante

// imposto l'attivazione del virtual touch per essere controllata per ogni spinner dal valore di lightable

// nella void offgroup il for arriva a 49 massimo - ricontrollare

// sto sostituendo toggletable2 e toggletable2a con _bit_toggle e bit_toggle2 - 
//sostituisco toggletable[distance_mempos] con lightable[distance_mempos]  nelle funzioni beam.

// riga 307 e simili - forse c'ÃƒÆ’Ã‚Â¨ un errore nella formuletta per il bit_feedback - controllare

// possibile usare tabella LEDS per selezionare il numero di multiplexers usati- 
// oppure si sposta l'attivazione del virtual touch dalla tabela value_general alla tabella led_touch

// in ain nucleo sezione pot - ripristino ma possibilitÃƒÆ’Ã‚Â  di avere un'uscita midi raw, se non c'ÃƒÆ’Ã‚Â¨ nulla nella EEPROM - ma solo sul primo multiplexer.

// ho limitato il remapping degli input (sezione midi IN - comunicazione ccon eeditor) a 55
// quindi da 0 a 55 gli input vengono rimappati... poi vengono assegnati normalmente - per esempio 56 = 56 , 59 = 59.

// problema . spuntano dei dati indesiderati insieme a quelli dovuti - cambiando la pagina... non dipende dal touchsensor
// il problema viene dal touch_real...

// ho messo un limitatore a ledcontrol. in modo che oltre il range di max_leds non succede nulla

// aumentando lastbutton da 60 a 64 la cosa si ÃƒÆ’Ã‚Â¨ risolta... non ho capito perche

// problema: i tasti posteriori arcade, collegari agli ultimi posti 53 54 55 56, sulla pagina 1 mandano in tilt il controller provocando un flusso continuo di dati in uscita
// questo flusso ha come riferimento il pot 16 (ccc 40) e il pulsante b10 (nota 69) 
// ho escluso il beam, il virtualtouch, il secondo encoder, i pads, il touch


// ho corretto il range di messaggi che possono entrare in midi in, c'erano difficoltaÃƒÆ’Ã‚Â  con certi valori di LED

// virtualtouch si attiva con valutable[general_mempos] > 0 - altrimenti rimane disattivato.
// ho razionalizzato jogtouch (touch real) con una void unica
// ved se possibile razionalizzare touch_real - creando una void unica per i due touchsensors

// minvalue[general_mempos] > 0 attiva il plexer extra.
// dmxvalue[general_mempos] >0 attiva l'encoder secondario. - bisogna fare attenzione a mettere i relativi input 44 e 46 in MODE.BLIND_INPUT
// maxvalue[general_mempos] > 0 attiva la funzione PADS; 
// funziona come la vecchia speedA5 - 0 significa pads attivati, 1 significa pads disattivati e plexer normale
// 

// dichiaro "channel" che ÃƒÆ’Ã‚Â¨ la variabile utilizzata dal FOR della procedura AIN, - adesso ÃƒÆ’Ã‚Â¨ una variabile generale
// sto separando il secondo encoder dai pads... in procedure void separate.
// 26a - cerco di razionalizzare AIN 

// ho modificato jog_touch - semplificata e razionalizzata . la media average trale varie letture adesso ÃƒÆ’Ã‚Â¨ piÃƒÆ’Ã‚Â¹ precisa
// ho inserito una prima calibrazione delle lower_values dei youch sensors all'avvio - ogni volta che si cambia pagina, le lower values vengono ricalibrate



// nella modalitÃƒÆ’Ã‚Â  63-65 non ÃƒÆ’Ã‚Â¨ implementata la velocitÃƒÆ’Ã‚Â  ma solo il numero 63 o 65

// ho relativizzato la lettura della PAGE all'accensione

//sostituzioni variabili per rsparmio memoria:
// byte encoderValue[2] ;  //sostituisco ccon lastbutton[encoder_mempos[numero]]
// int encodervaluepot[2] ; sostituita con switchstate[encoder_mempos[numero]]
// boolean beamstate2; // sostituisco con lastbutton[distance_mempos]


// gli array utilizzati per pot e pulsanti devono essere tanti quanti gli input utilizzati dal dart (56) 
// gli array utilizzati anche per funzioni speciali (virtual touch usa switchstate e lastbutton) devono essere tanti quanti sono i modificatori massimi accessibili da editor e memorizzabili su eeprom - cioÃƒÆ’Ã‚Â¨ 64
// ma dato che anche min max qwerty e altro, vengono usete per funzioni speciali... tutto va portato a 64... o comunque oltre il 56, per dare spazio a funzioni speciali 
// se non si utilizzano ledstatus e feedbacktable per funzioni specciali... possono  restare limitate a 56
// lastbutton e switchstate vanno portate a 64


// ho messo touch_mempos e encoder_mempos in forma di array in modo che si possano selezionare tramite "numero"
// tutta la sezione virtual touch va relativizzata // provo a usare switchstate

// ho sostituito lastsensor 1 e 2 con - lastbutton[touch1_mempos] 1 e 2



//perchÃƒÆ’Ã‚Â¨ ledstatus arriva solo 51 ??
// perchÃƒÆ’Ã‚Â¨ feedbacktable arriva a 96? risale ai 48 elementi del dart one?

// tutti gli array sono 0 indexed - tutto va riscritto meglio per  non sprecare bytes iniziali - sia nel midi in - sia nella lettura da memoria di ain nucleo

// ved se possibile unire ledrestore 1 e 2
// aumentare capienza delle variabili array - a livello korova - 
// capire una volta per tutte gli spazi vuoti sugli array - se si scrive a partire da 0 o da 1

// ho rimappato la memorizzazione dei preset- adesso si usa la numerazione semplificata

// non c'ÃƒÆ’Ã‚Â¨ attivitÃƒÆ’Ã‚Â  midi quando non c'ÃƒÆ’Ã‚Â¨ preset
// c'ÃƒÆ’Ã‚Â¨ un grosso problema nella memorizzazione dei preset- relativamente ai led! - risolto --  la seconda pagine caricata da editor andava a scrivere oltre il 2014


// assicurarsi che se touch e encoders non vengono specificati nell'editor, non succeddano casini... tutti i loro settaggi vanno a finire su posizione memory 0 - 
// inizializzo le variabili tutte su 64 ...
////                 se il dart non ha touch sensors - in teoria le procedure void non vengono triggerate - quindi no problem
////                  se il dart non ha encoders , in teoria le vvoi dell'encoder non vengono triggerate - 

// aggiunte modalitÃƒÆ’Ã‚Â  centercurve 
// relativizzato page e beam

// relativizzato encoders 12 e touch 12
// add ctrl alt shift - modalitÃƒÆ’Ã‚Â  modifiers

// cambio beam section - adesso si sceglie tramite qwertyvalue e non tramite modetable - se deve funzionare come pot o pulsante - quindi libero modetable, per migliorare coerenza editor
// in beam() riga 9 naturalmente lasci chan - come riferimanto - cosÃƒÆ’Ã‚Â¬ si possono usare molti distance sensors.

// ÃƒÆ’Ã‚Â¨ possibile disattivare scegliere se il sesto multiplexer (analog in A5) deve essere dedicato a funzioni speciali (pads - encoder) oppure a normali input


// aggiunta riga 61 nell'efditor per accettare anche aftertouch, utile nel caso vengano ricavuti messaggi LED fuori range, per avere effetto ZERO
// aggiungo codica in load preset e MIDIIN per memorizzare mappatura LED output. 
// migiorato il dettaglio pot in ain nucleo - la soglia ddi differenza con lastbutton ÃƒÆ’Ã‚Â¨ 10 adesso... pare stabile.


// ved qwerty send
// ved dettaglio pot
// lo spazio di memoria del mouse nel korova era 58 - nel one e speciale porting va definito! scelgo il 47
// aggiungo riga per mandare mousewheel in void encoder 
// ho notato che all'avvio qualcosa non va... bisogna smuovere la levetta page... ok c'era un page=64... doveva esssere page=48

// encoderpads aveva un errore- c'ÃƒÆ’Ã‚Â¨era il "+page" anche in min e maxvalue... sballava la seconda pagina


//porting: metto void midiout direttamente importata da korova.
// in void loop - inserisco le condizioni midi in del korova
// dichiaro variabile MIDIEvent eventomidiUSB;
// dichiaro volatile byte qwertyvalue[65]; ma la limito a 49
// dichiaro qwertymod - copiata da korova - contiene la tabella dei caratteri ulteriori
// in void load preset - inserisco righe per loading qweertyvalue
// inserisco voi modifiers - copiata da korova
// creo voi outnucleo sotto cartella midiout - per facilitare il senout dei modificatori... ved ain nucleo per utilizzo

// tolgo il baudrate midi - solo usb - metto la possibiltÃƒÆ’Ã‚Â  di scegliere all'avvio con B12 - come nel korova
// !!!!! ho avuto molte difficoltÃƒÆ’Ã‚Â  quando quertyvalue[49] veniva caricata fino a 64 valori... faceva sballare i pulsanti!! - fatto
// inserisco possibilitÃƒÆ’Ã‚Â  scelta baudrate all'avvio - riga per spegnimeto di tutti i ledstatus, dopo loadpreset , come nel korova
//----------------------

// virtual touch end - void - dmx arrima al massimo a 65 - eliminato "+page"
// la memorizzazione del valore dmx su eeprom partiva da 186, con conseguente accavallamento di valori, possibile... adesso parte normalmente da 192

// corretto bug pages su encoders

// in modalita pot-hypercurve i led si spengono ad ogni ciclo
// migliorato alcune inesattezze e ridondanze buttonledefx
// eliminato le vaiabili long per conteggio millis effetti led... adesso si usa il tempo di esecuzione ciclo per i led

// aggiunta funzione hypercurve - utile per scratching--- imposstando toggletale su 11 in poi si attiva, l'escursione da min a max si riduce, facilitando molte operazioni live 
// se impostato su 12 la curva ÃƒÆ’Ã‚Â¨ ridotta e invertita (nella parte supariore del fader)

// spannato meglio il segnale dei pot da 0 a 127
// corretto tochreset - encodervalueppot *8 !! ricordare he adesso ÃƒÆ’Ã‚Â¨ adesso ÃƒÆ’Ã‚Â¨ 0-1024
// toggletable dei touchsensor == 0 niente, == 1 touchreset normale, == 2 touchreset virtual - a value del touchreset ÃƒÆ’Ã‚Â¨ dmxtable dei touch

// vitual touch viene sparato ffuori insieme al touch normale ha lo stesso setup ma su un canale aumentato di 1
// adesso la velocitÃƒÆ’Ã‚Â   deglli encoder puÃƒÆ’Ã‚Â² essere aggiustata al ribasso - va settata diversamente per encoder ottico ed encoder meccanico
// toggletable degli encoder decide se viene sparato ffuori segnale <63 >65 oppure 0-127
// risolto picccolo bug in encoderpads - i pulsanti extra erano impostati ancora con page*48
// encodervaluepot adesso ÃƒÆ’Ã‚Â¨ int

// modificato beamefx - ora piÃƒÆ’Ã‚Â¹ semplice
// possibile mandare dmx da encoder - solo in pot mode

// separato ain nucleo
// aggiunto effetti led pot
// possibilitÃƒÆ’Ã‚Â  invertire encoders (maxvalue 43 44 < 127)
// moltiplicatore velocitÃƒÆ’Ã‚Â  minvalue 43 44
// aggiunto virtual touch su 45 e 46 - diverso per i due encoders
// page adesso cambia da 0 a 48 - non piÃƒÆ’Ã‚Â¹ 0 e 1 - piÃƒÆ’Ã‚Â¹ facile programmare

// 19 5 2017 - fixed toggle mode number == 2 - groups >3 
// openeditor messagge == 241 system undefined

// Each modifier can now send Note, poly AT, Control Change, Program Change, Channel AT and PitchBend messages
// Touch-reset function for jogwheels j1 and j2 : if set to PithBend, the touch sensors will reset the PitchBending to it's mid position.
// The two pages/presets are now completely indipendent (different type, channel, min/max values, DMX essage, mode, can be set for each modifier for each page)
// Toggle-group mode available.
// A song position message (242,x,x) message is used to open/close the editor-upload procedure.
// Improved midifeedback processing - no feedback loops - it works for both the pages - attenzione: il midifeedback funziona con 0=spento !0=acceso- non tiene conto dei valori mix e max
