
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

 
