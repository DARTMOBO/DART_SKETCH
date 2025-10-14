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

void LedControl::setRow(int addr, int row, byte value, byte send_) {
    int offset;
 /*
  if(addr<0 || addr>=maxDevices)
        return;
    if(row<0 || row>7)
        return;
        */
    offset=addr*8;
    status[offset+row]=value;
   if (send_ ==1 ) spiTransfer(addr, row+1,status[offset+row]); // spiTransfer(int addr, volatile byte opcode, volatile byte data)
}


void LedControl::sendRow(int addr, byte row) {

  
    int offset1=addr*8;

int offset=addr*2;              
    int maxbytes=maxDevices*2; // 12 matrici di led  x2 = 24

    for(int i=0;i<maxbytes;i++)
        spidata[i]=(byte)0;    // azzeramento 
              
    spidata[offset+1]=row+1; 
    spidata[offset]=status[offset1+row];         // posiziono il byte da inviare

    /*
    for(int rr=0;rr<8;rr++){
    spidata[offset+1]=rr+1; 
    spidata[offset]=status[offset1+rr];         // posiziono il byte da inviare
    */
 
    digitalWrite(SPI_CS,LOW);
    for(int i=maxbytes;i>0;i--)   shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);  // i-1 per fare scendere i fino a 
    digitalWrite(SPI_CS,HIGH);

}

/*
void LedControl::sendRow2(int addr) {

 for(byte yy=0;yy<12;yy++)
  {
    byte offset1=yy*8;

byte offset=yy*2;              
    int maxbytes=maxDevices*2; // 12 matrici di led  x2 = 24

   
        
                 for(int i=0;i<maxbytes;i++)
                 spidata[i]=(byte)0;

              for(byte row=0;row<8;row++)
              {
                  
        
    spidata[offset+1]=row+1; 
    spidata[offset]=status[offset1+row];         // posiziono il byte da inviare

    
    digitalWrite(SPI_CS,LOW);
    for(int i=24;i>0;i--)   shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);  // i-1 per fare scendere i fino a 
    digitalWrite(SPI_CS,HIGH);
    //delay(5);
              }

    
   
 
  }

  
}
*/


void LedControl::sendRow2(byte scatto) {


// qualche appunto perchè la confusione è bestiale:
// voglio inviare solo 8 spitransfer completi, uno per ogni riga di tutte le 12 matrici - 
// esempio, il primo spitransfer vale per la riga uno di tutte le matrici... benissimo
//byte scatto =1;

// for(byte scatto=0;scatto<2;scatto++)


 // scatto = 0;

 // qualche chiarimento: ho notato che a causa sicuramente di interferenze elettriche, 
 // mandare uno spitransfer completo con una riga per tutte le 12 matrici è difficile
 // quindi ho separato bene i mesaggi tramite "scatto"
 // le matrici vengono controllate in due gruppi da 6 separando bene i segnali
 // quindi controllo le matrici una si e una no prima le pari poi le dispari... così funziona tutto bene
 
 {
   for(byte row=0;row<8;row++)
   {

      for(int i=0;i<24;i++)
      spidata[i]=(byte)0; //azzeramento


    /*
        if (scatto == 0)  // restore shutdown     ////// qui riaccendo le matrici che prima avevo spento con gli effetti luci endenc... 
                                                  // ho spento le matrici di numero pari per un difetto di comunicazione coi max7219
                                                  // non posso mandare un aggioramento con tutto l'array spidata pieno
                                                  // pernso per problemi di interferenza elettrica
         {
          for(byte row2=0;row2<6;row2++){
         spidata[3+row2*4]=12;  // 2-3     6-7     10-11     14-15     18-19      22-23
         spidata[2+row2*4]=1; }

          digitalWrite(SPI_CS,LOW);
    for(int i=24;i>0;i--)  { shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);  // delay(1);
    } // i-1 per fare scendere i fino a 
    digitalWrite(SPI_CS,HIGH); //delay(5); 

           for(int i=0;i<24;i++)
           spidata[i]=(byte)0;   
         }


*/
         
               
         for(byte yy=0;yy<6;yy++)
         {
         byte offset1=yy*16+scatto*8;
         byte offset=yy*4+scatto*2;              
        
         spidata[offset+1]=row+1; 
         spidata[offset]=status[offset1+row];         // posiziono il byte da inviare
         
       
        
         }
 
    digitalWrite(SPI_CS,LOW); //delay(10);
    for(int i=24;i>0;i--)  { shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]); // delay(10);
  //    shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);  // i-1 per fare scendere i fino a 0
    } // i-1 per fare scendere i fino a 
    digitalWrite(SPI_CS,HIGH); //delay(10); 
   }
   //delay(500);
 }
  
}







// ----------------------------------------------------------------------------------------------
/*
void LedControl::restore_shutdown() // questa versione invia uno spidata da 24 byte per ogni matrice da spegnere
{
       // for(int i=0;i<24;i++)
      //  spidata[i]=(byte)0; 

          for(int i=0;i<6;i++)   /// 01  
           {
            for(int f=0;f<24;f++)
        spidata[f]=(byte)0; 
        
             spidata[i*4+3]=12;      //  0 .. 0*2+3 = 3    // 1*4+3 = 7    .. 7   .. 2*4+3 = 11
             spidata[i*4+2]=1;       //  0 .. 0*2+2 = 2    // 1*4+2 = 6    .. 6   .. 2*4+2 = 10

                digitalWrite(SPI_CS,LOW);                                                // queste tre righe valgono come uno spitransfer
    for(int i=24;i>0;i--) shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]); 
    digitalWrite(SPI_CS,HIGH);
            }
     
    // digitalWrite(SPI_CS,LOW);                                                // queste tre righe valgono come uno spitransfer
    // for(int i=4;i>0;i--) shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]); 
    // digitalWrite(SPI_CS,HIGH);
  
  }
*/

void LedControl::restore_shutdown() // 
{
        for(int i=0;i<24;i++) // azzeramento
       spidata[i]=(byte)0; 

          for(int i=0;i<6;i++)   /// 01  
           {
      //      for(int f=0;f<24;f++) // azzeramento
       // spidata[f]=(byte)0; 
        
             spidata[i*4+3]=12;      //  0 .. 0*2+3 = 3    // 1*4+3 = 7    .. 7   .. 2*4+3 = 11
             spidata[i*4+2]=1;       //  0 .. 0*2+2 = 2    // 1*4+2 = 6    .. 6   .. 2*4+2 = 10

   // digitalWrite(SPI_CS,LOW);                                                // queste tre righe valgono come uno spitransfer
   //  for(int i=24;i>0;i--) shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]); 
   //  digitalWrite(SPI_CS,HIGH);
            }
     
     digitalWrite(SPI_CS,LOW);                                                // queste tre righe valgono come uno spitransfer
     for(int i=24;i>0;i--) shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]); 
     digitalWrite(SPI_CS,HIGH);
  
  }



void LedControl::Parola_diretta(byte matrice_partenza, byte row, byte onoff ) {   // addr = quale matrice // row = quale riga/row devo illuminare
      

         for(int i=0;i<4;i++)
        spidata[i]=(byte)0; 

        if (onoff ==1){

          spidata[1+(matrice_partenza*2)]=row+1;
    //   spidata[(matrice_partenza*2)]=~status[(matrice_partenza)*8+row];
     spidata[(matrice_partenza*2)]=255;                                        // tutto bianco

    //   spidata[1+(matrice_partenza*2+2)]=12;                                   // seconda matrice - shutdown
    //  spidata[(matrice_partenza*2)+2]=0;
      
   //   spidata[1+(matrice_partenza*2+4)]=row+1;
     //  spidata[(matrice_partenza*2)+4]=~status[(matrice_partenza+2)*8+row]; // stesso lavoro sulla matrice accanto
  //  spidata[(matrice_partenza*2)+4]=255;

     //   spidata[1+(matrice_partenza*2+6)]=row+1;
     //  spidata[(matrice_partenza*2)+6]=~status[(matrice_partenza+3)*8+row];;
   //  spidata[(matrice_partenza*2)+6]=255; 
        }
        else  //// ---------------------------------------------------------------
        {

       spidata[1+(matrice_partenza*2)]=row+1;
    //   spidata[(matrice_partenza*2)]=status[(matrice_partenza)*8+row];
    spidata[(matrice_partenza*2)]=0;

  //   spidata[1+(matrice_partenza*2+2)]=row+1;                                      // seconda matrice
    //   spidata[(matrice_partenza*2)]=status[(matrice_partenza)*8+row];
  //  spidata[(matrice_partenza*2)+2]=0;
       
     //  spidata[1+(matrice_partenza*2+2)]=row+1;                   // seconda matrice
     //  spidata[(matrice_partenza*2)+2]=status[(matrice_partenza+1)*8+row];
      
    //   spidata[1+(matrice_partenza*2+4)]=row+1;
      // spidata[(matrice_partenza*2)+4]=status[(matrice_partenza+2)*8+row];
    //  spidata[(matrice_partenza*2)+4]=0;

     //   spidata[1+(matrice_partenza*2+6)]=row+1;
     //  spidata[(matrice_partenza*2)+6]=status[(matrice_partenza+3)*8+row];
          }

       
     //   spidata[1+(matrice_partenza*2+4)]=row;
     //   spidata[(matrice_partenza*2)+4]=onoff*255;

    
    //       spidata[1+(matrice_partenza*2+6)]=row;
    //   spidata[(matrice_partenza*2)+6]=onoff*255;


        
        

    digitalWrite(SPI_CS,LOW);                                                // queste tre righe valgono come uno spitransfer
    for(int i=4;i>0;i--) shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]); 
    digitalWrite(SPI_CS,HIGH);

     for(int i=0;i<4;i++)
        spidata[i]=(byte)0; 

        if (onoff ==1){    //   spidata[1+(matrice_partenza*2+2)]=12;                                   // seconda matrice - shutdown
        spidata[1+(matrice_partenza*2+2)]=12;                                   // seconda matrice - shutdown
        spidata[(matrice_partenza*2)+2]=0;
        }
    
    digitalWrite(SPI_CS,LOW);                                                // queste tre righe valgono come uno spitransfer
    for(int i=4;i>0;i--) shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]); 
    digitalWrite(SPI_CS,HIGH);

    // spiegazione: se devo comunicare qualcosa alla mia catena di max7219, devo inviare tutto un blocco spidata
    // un blocco spidata che, nel caso in cui abbiamo 12 matrici, sarà da 24 bytes - perche devo sapere cosa fare (numero OPCODE) e il byte del contenuto. 
    // insomma due bytes per ogni matrice.
      

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
