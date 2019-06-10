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
