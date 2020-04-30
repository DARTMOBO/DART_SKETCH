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


#include "LedControl.h"

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

LedControl::LedControl(int dataPin, int clkPin, int csPin, int numDevices) {
    SPI_MOSI=dataPin;
    SPI_CLK=clkPin;
    SPI_CS=csPin;
    if(numDevices<=0 || numDevices>12 )
        numDevices=12;
    maxDevices=numDevices;
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

int LedControl::getDeviceCount() {
    return maxDevices;
}

void LedControl::shutdown(int addr, bool b) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(b)
        spiTransfer(addr, OP_SHUTDOWN,0);
    else
        spiTransfer(addr, OP_SHUTDOWN,1);
}

void LedControl::setScanLimit(int addr, int limit) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(limit>=0 && limit<8)
        spiTransfer(addr, OP_SCANLIMIT,limit);
}

void LedControl::setIntensity(int addr, int intensity) {
        spiTransfer(addr, OP_INTENSITY,intensity);
}

void LedControl::clearDisplay(int addr) {
    int offset;

    if(addr<0 || addr>=maxDevices)
        return;
    offset=addr*8;
    for(int i=0;i<8;i++) {
        status[offset+i]=0;
        spiTransfer(addr, i+1,status[offset+i]);
    }
}

void LedControl::setLed(int addr, int row, int column, boolean state) {
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

void LedControl::setRow(int addr, int row, byte value) {
    int offset;
    if(addr<0 || addr>=maxDevices)
        return;
    if(row<0 || row>7)
        return;
    offset=addr*8;
    status[offset+row]=value;
    spiTransfer(addr, row+1,status[offset+row]);
}

void LedControl::setRow_massi(int addr, int row ) {   // addr = quale matrice // row = quale riga/row devo illuminare
   // spiTransfer(addr, order_row[row-1]+1,0);
    spiTransfer(addr, order_row[row]+1,255);      }

void LedControl::Parola_diretta(byte matrice_partenza, byte row, byte onoff ) {   // addr = quale matrice // row = quale riga/row devo illuminare
      

         for(int i=0;i<24;i++)
        spidata[i]=(byte)0;

        if (onoff ==1){
       spidata[1+(matrice_partenza*2+2)]=row+1;
       spidata[(matrice_partenza*2)+2]=~status[(matrice_partenza+1)*8+row];;
      
       spidata[1+(matrice_partenza*2+4)]=row+1;
       spidata[(matrice_partenza*2)+4]=~status[(matrice_partenza+2)*8+row];;
        }
        else
        {
       spidata[1+(matrice_partenza*2+2)]=row+1;
       spidata[(matrice_partenza*2)+2]=status[(matrice_partenza+1)*8+row];
      
       spidata[1+(matrice_partenza*2+4)]=row+1;
       spidata[(matrice_partenza*2)+4]=status[(matrice_partenza+2)*8+row];
          }

       
     //   spidata[1+(matrice_partenza*2+4)]=row;
     //   spidata[(matrice_partenza*2)+4]=onoff*255;

    
    //       spidata[1+(matrice_partenza*2+6)]=row;
    //   spidata[(matrice_partenza*2)+6]=onoff*255;


        
        

        digitalWrite(SPI_CS,LOW);
    for(int i=24;i>0;i--) shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]); 
    digitalWrite(SPI_CS,HIGH);
      

    }

    

void LedControl::spiTransfer(int addr, volatile byte opcode, volatile byte data) {
    //Create an array with the data to shift out
    int offset=addr*2;              // esempio: se voglio controllare la matrice 4, offset sarà 8 
                                    // se la matrice è 0 offset è 0 // 
                                    // se la matrice è 16 offset è 32
                                    // se la matrice è 15 offset è 30 // e opcode sarà messo sul 31
    int maxbytes=maxDevices*2; // 12 matrici di led  x2 = 24

    for(int i=0;i<maxbytes;i++)
        spidata[i]=(byte)0;      //   ripeti 24 volte  // spidata è un array da 32 bytes // adesso viene tutto azzerato
        
    //put our device data into the array
    spidata[offset+1]=opcode;     // esempio: io invio un OPCODE di controllo riga 1 sulla posizione/offset numero 9, 
                                  // così controllerò la prima riga della quarta matrice
                                  // il successivo byte conterrà i bit effettivi da visualizzare, contenuti in DATA
                                  // durante l'invio l'array spidata liene letto al contrario
                                  //
                                  // opcode è un numero che varia da 0 a 15 
                                  // da 1 a 8 controlla le righe delle matrici 
                                  // di seguito comandi particolari (luminosità , spegnimento etc etc)
    spidata[offset]=data;         // posiziono il byte da inviare
    //enable the line 
    digitalWrite(SPI_CS,LOW);
    //Now shift out the data 
    for(int i=maxbytes;i>0;i--)                            // ripeti 24 volte a scendere 
        shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);  // i-1 per fare scendere i fino a 0
    //latch the data onto the display
    digitalWrite(SPI_CS,HIGH);
}    
