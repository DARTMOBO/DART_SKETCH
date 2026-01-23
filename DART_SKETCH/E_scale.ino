//////////////////////////////////////////////////////////////////////

#if (Scale == 1) 
void scale_learn(byte pitch) {
  if (lastbutton[touch_mempos[0]] == 0 || lastbutton[touch_mempos[1]] == 0) { // se touch premuto/toccato
    if (dmxtable[encoder_mempos[0]] > 1) {
      encodervaluepot[0] = pitch * 8;
    }
   
    if (scala_reset == 1) { // azzera la scala
      for (byte i = 0; i < scala_lenght; i++) {
        scala_array[i] = 0;
        scala_learn = 0;
      }
      scala_reset = 0;
    }

    scala_array[scala_counter] = (pitch - (pitch / 12) * 12) + 1; // 0 == nessuna nota,
    scala_counter++;
    
    if (scala_counter >= scala_lenght) {
      scala_counter = 0;
    }
    
    scala_learn = 0;
    for (byte i = 0; i < scala_lenght; i++) {
      // if (scala_array[i] != 0) 
      if (scala_array[i] > 0) { // 0 = nessuna nota da caricare nella scala
        bitWrite(scala_learn, scala_array[i] - 1, 1);
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////
void update_scala(byte quale_spinner) { // quale_spinner è 0 top 1 side
  // richiamato da setup_mempos() e da switchpage()
  // scala[4] è un array che contiene 4 scale... scalaSpinner0-page1, scalaSpinner1-page1, ScalaSpinner0-page2, scalaSpinner1-page2
  // per caricare una scala dalla eeprom di servono 12 BIT - per le 12 note di un'ottava
  // i primi 7 BIT sono dentro valuetable
  // gli altri 5 BIT sono dentro lastencoded[2]
  // lastEncoded[] viene ricaricata ogni volta che si carica un preset - quindi è sempre "attuale" rispetto alla pagina.

  // scala[0] = 14;
   
  for (byte i = 0; i < 7; i++) {
    bitWrite(scala[quale_spinner + (page / max_modifiers) * 2], i, bitRead(valuetable[encoder_mempos[quale_spinner] + page], i));
    // bitWrite(scala[0], i, bitRead(valuetable[encoder_mempos[quale_spinner] + page], i));
  }
  
  for (byte i = 0; i < 5; i++) {
    bitWrite(scala[quale_spinner + (page / max_modifiers) * 2], i + 7, bitRead(EEPROM.read(encoder_mempos[quale_spinner] + 320 + (page / max_modifiers * 512)), i));
    // bitWrite(scala[0], i + 7, bitRead(EEPROM.read(encoder_mempos[quale_spinner] + 320 + (page / max_modifiers * 512)), i));
  }

  /*
  for (byte i = 0; i < 12; i++) {
    Serial.print(bitRead(scala[0], i));
  }
  Serial.println(" - scala[]");
  */
}

#endif
