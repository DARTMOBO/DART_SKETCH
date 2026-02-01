/*
 *    LedControl.cpp - A library for controling Leds with a MAX7219/MAX7221
 *    Copyright (c) 2007 Eberhard Fahle
 * 
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 * 
 *    This permission notice shall be included in all copies or 
 *    substantial portions of the Software.
 * 
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */


#include "_DART_LedControl.h"
#include <SPI.h>

// Per DART su Leonardo: evitare le macro "veloci" dipendenti dalla
// mappa pin stile UNO. Usiamo sempre le API standard Arduino.
#define pinAsOutput(P) pinMode(P, OUTPUT)
#define digitalLow(P)  digitalWrite(P, LOW)
#define digitalHigh(P) digitalWrite(P, HIGH)



//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

// DART: numero massimo di MAX7219 gestiti in catena.
// Il tuo hardware usa 12 matrici.
#define DART_MAX_DEVICES 12

DartLedControl::DartLedControl(int csPin, int numDevices) {
    legacy = false;

    SPI_CS=csPin;
    
if (numDevices <= 0 || numDevices > DART_MAX_DEVICES)
    numDevices = DART_MAX_DEVICES;
maxDevices = numDevices;

    pinAsOutput(MOSI);
    pinAsOutput(SCK);
    pinAsOutput(SPI_CS);
    
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	SPI.begin();

    digitalHigh(SPI_CS);

    for(int i=0;i<64;i++) 
        status[i]=0x00;
    for(int i=0;i<maxDevices;i++) {
        spiTransfer(i,OP_DISPLAYTEST,0);
        //scanlimit is set to max on startup
        setScanLimit(i,7);
        //decode is done in source
        spiTransfer(i,OP_DECODEMODE,0);
        clearDisplay(i);
        //we go into shutdown-mode on startup
        shutdown(i,true);
    }
}

DartLedControl::DartLedControl(int dataPin, int clkPin, int csPin, int numDevices) {
    legacy = true;

    SPI_MOSI=dataPin;
    SPI_CLK=clkPin;
    SPI_CS=csPin;
    
if (numDevices <= 0 || numDevices > DART_MAX_DEVICES)
    numDevices = DART_MAX_DEVICES;
maxDevices = numDevices;

    pinMode(SPI_MOSI,OUTPUT);
    pinMode(SPI_CLK,OUTPUT);
    pinMode(SPI_CS,OUTPUT);

    digitalWrite(SPI_CS,HIGH);
    SPI_MOSI=dataPin;
    for(int i=0;i<64;i++) 
        status[i]=0x00;
    for(int i=0;i<maxDevices;i++) {
        spiTransfer(i,OP_DISPLAYTEST,0);
        //scanlimit is set to max on startup
        setScanLimit(i,7);
        //decode is done in source
        spiTransfer(i,OP_DECODEMODE,0);
        clearDisplay(i);
        //we go into shutdown-mode on startup
        shutdown(i,true);
    }
}

int DartLedControl::getDeviceCount() {
    return maxDevices;
}

void DartLedControl::shutdown(int addr, bool b) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(b)
        spiTransfer(addr, OP_SHUTDOWN,0);
    else
        spiTransfer(addr, OP_SHUTDOWN,1);
}

void DartLedControl::setScanLimit(int addr, int limit) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(limit>=0 && limit<8)
        spiTransfer(addr, OP_SCANLIMIT,limit);
}

void DartLedControl::setIntensity(int addr, int intensity) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(intensity>=0 && intensity<16)	
        spiTransfer(addr, OP_INTENSITY,intensity);
}

void DartLedControl::clearDisplay(int addr) {
    int offset;

    if(addr<0 || addr>=maxDevices)
        return;
    offset=addr*8;
    for(int i=0;i<8;i++) {
        status[offset+i]=0;
        spiTransfer(addr, i+1,status[offset+i]);
    }
}
/*
void DartLedControl::setLed(int addr, int row, int column, boolean state) {
    int offset;
    byte val=0x00;

    if(addr<0 || addr>=maxDevices)
        return;
    if(row<0 || row>7 || column<0 || column>7)
        return;
    offset=addr*8;
    val=B10000000 >> column;
    if(state)
        status[offset+row]=status[offset+row]|val;
    else {
        val=~val;
        status[offset+row]=status[offset+row]&val;
    }
    spiTransfer(addr, row+1,status[offset+row]);
}
*/


void DartLedControl::setRow(int addr, int row, byte value) {
    int offset;
    if(addr<0 || addr>=maxDevices)
        return;
    if(row<0 || row>7)
        return;
    offset=addr*8;
    status[offset+row]=value;
    spiTransfer(addr, row+1,status[offset+row]);
}

void DartLedControl::setRow(int addr, int row, byte value, byte send_) {
    int offset;

    if (addr < 0 || addr >= maxDevices)
        return;
    if (row < 0 || row > 7)
        return;

    offset = addr * 8;
    status[offset + row] = value;

    // come nella vecchia LedControl: solo se send_ == 1
    // mandiamo subito il dato al MAX7219
    if (send_) {
        spiTransfer(addr, row + 1, status[offset + row]);
    }
}

// DART: versione semplificata di sendRow2 per SPI hardware.
// Aggiorna tutte le righe di un gruppo di matrici.
// scatto = 0 -> aggiorna solo le matrici con indice pari  (0,2,4,...)
// scatto = 1 -> aggiorna solo le matrici con indice dispari (1,3,5,...)
// scatto > 1 -> aggiorna tutte le matrici.


void DartLedControl::sendRow2(byte scatto) {
    for (int row = 0; row < 8; row++) {
        for (int addr = 0; addr < maxDevices; addr++) {

            bool doUpdate;
            if (scatto > 1) {
                // valore "speciale": aggiorna tutte
                doUpdate = true;
            } else {
                // 0 = pari, 1 = dispari
                doUpdate = ((addr & 1) == (scatto & 1));
            }

            if (doUpdate) {
                int offset = addr * 8;
                byte value = status[offset + row];
                spiTransfer(addr, row + 1, value);
            }
        }
    }
}

void DartLedControl::setRowOverlay(int addr, int row, byte value) {
    if (addr < 0 || addr >= maxDevices)
        return;
    if (row < 0 || row > 7)
        return;

    // Non aggiorniamo status[]: usiamo solo lo SPI
    spiTransfer(addr, row + 1, value);
}

void DartLedControl::restoreRowFromStatus(int addr, int row) {
    if (addr < 0 || addr >= maxDevices)
        return;
    if (row < 0 || row > 7)
        return;

    int offset = addr * 8;
    byte value = status[offset + row];   // leggiamo lo SFONDO
    spiTransfer(addr, row + 1, value);   // rimandiamo SOLO quella riga al MAX7219
}

void DartLedControl::xorRowOverlayFromStatus(int addr, int row, byte mask) {
    if (addr < 0 || addr >= maxDevices)
        return;
    if (row < 0 || row > 7)
        return;

    int offset = addr * 8;
    byte base = status[offset + row];   // sfondo “pulito”
    byte value = base ^ mask;           // XOR = flip dei bit dove mask ha 1

    // overlay: NON tocchiamo status[], mandiamo solo al MAX7219
    spiTransfer(addr, row + 1, value);
}




/*
void DartLedControl::setColumn(int addr, int col, byte value) {
    byte val;

    if(addr<0 || addr>=maxDevices)
        return;
    if(col<0 || col>7) 
        return;
    for(int row=0;row<8;row++) {
        val=value >> (7-row);
        val=val & 0x01;
        setLed(addr,row,col,val);
    }
}
*/

/*
void DartLedControl::setDigit(int addr, int digit, byte value, boolean dp) {
    int offset;
    byte v;

    if(addr<0 || addr>=maxDevices)
        return;
    if(digit<0 || digit>7 || value>15)
        return;
    offset=addr*8;
    v=pgm_read_byte_near(charTable + value); 
    if(dp)
        v|=B10000000;
    status[offset+digit]=v;
    spiTransfer(addr, digit+1,v);
}
*/
/*
void DartLedControl::setChar(int addr, int digit, char value, boolean dp) {
    int offset;
    byte index,v;

    if(addr<0 || addr>=maxDevices)
        return;
    if(digit<0 || digit>7)
        return;
    offset=addr*8;
    index=(byte)value;
    if(index >127) {
        //no defined beyond index 127, so we use the space char
        index=32;
    }
    v=pgm_read_byte_near(charTable + index); 
    if(dp)
        v|=B10000000;
    status[offset+digit]=v;
    spiTransfer(addr, digit+1,v);
}
*/

void DartLedControl::spiTransfer(int addr, volatile byte opcode, volatile byte data) {
    //Create an array with the data to shift out
    int offset=addr*2;
    int maxbytes=maxDevices*2;

    for(int i=0;i<maxbytes;i++)
        spidata[i]=(byte)0;
    //put our device data into the array
    spidata[offset+1]=opcode;
    spidata[offset]=data;
    //enable the line 
    digitalLow(SPI_CS);
    //Now shift out the data
    if(legacy) {
        for(int i = maxbytes; i > 0; i--)
            shiftOut(MOSI,SCK,MSBFIRST,spidata[i-1]);
    }
    else {

        SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
        
        for(int i = maxbytes; i > 0; i--)
            SPI.transfer(spidata[i-1]);

        SPI.endTransaction();
    }
    //latch the data onto the display
    digitalHigh(SPI_CS);
}    


// DART: versione semplificata di restore_shutdown() per SPI hardware.
// Rimette tutte le matrici in modalità "normal operation"
// (registro 0x0C = OP_SHUTDOWN, data = 1 per ogni device).
void DartLedControl::restore_shutdown() {
    for (int addr = 0; addr < maxDevices; addr++) {
        spiTransfer(addr, OP_SHUTDOWN, 1);
    }
}




// DART: versione hardware-SPI di Parola_diretta.
// Accende o spegne una riga intera su una matrice,
// e opzionalmente manda anche uno SHUTDOWN alla matrice successiva
// (effetto "eco" come nel vecchio codice).
void DartLedControl::Parola_diretta(byte matrice_partenza, byte row, byte onoff) {
    // sicurezza: fuori range → non fare nulla
    if (matrice_partenza >= maxDevices) {
        return;
    }

    // le righe del MAX7219 sono 0–7, opcode = 1–8
    row &= 0x07;
    byte opcode = row + 1;

    if (onoff) {
        // accendi tutta la riga sulla matrice di partenza
        spiTransfer(matrice_partenza, opcode, 0xFF);

        // opzionale: spegni hardware la matrice successiva (eco)
        if (matrice_partenza + 1 < maxDevices) {
            spiTransfer(matrice_partenza + 1, OP_SHUTDOWN, 0x00);
        }
    } else {
        // spegni la riga sulla matrice di partenza
        spiTransfer(matrice_partenza, opcode, 0x00);

        // riaccendi eventualmente la matrice successiva
        if (matrice_partenza + 1 < maxDevices) {
            spiTransfer(matrice_partenza + 1, OP_SHUTDOWN, 0x01);
        }
    }
}
