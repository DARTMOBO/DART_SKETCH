// ===============================================
// DEBUG VELOCITY PAD  (attiva/disattiva con define)
// ===============================================
     // metti 0 per spegnere tutto

#if PAD_VELO_DEBUG

void debugPadVelocityFilm()
{
  const byte  FRAMES       = 15;      // quanti "fotogrammi" dopo il gate
  const unsigned int STEP_US = 3000;  // distanza tra un frame e l'altro (µs)
  uint16_t film[FRAMES];

  // 1) Registrazione "filmato"
  for (byte i = 0; i < FRAMES; i++) {
    film[i] = analogRead_1024(plexer);   // usa il tuo ADC (fast o normale)
    delayMicroseconds(STEP_US);          // blocca tutto: va bene, è solo debug
  }

  // 2) Stampa su Serial come barrette di '*'
  Serial.println(F("=== DEBUG PAD VELOCITY FILM ==="));
  Serial.print(F("chan="));
  Serial.print(chan);
  Serial.print(F("  plexer="));
  Serial.println(plexer);

  for (byte i = 0; i < FRAMES; i++) {
    // Nel tuo circuito: 0 = forte, 1023 = piano
    // Quindi invertiamo: valore basso → barra lunga.
    uint8_t barLen = map(film[i], 1023, 0, 0, 40);  // max 40 asterischi

    Serial.print(i);        // numero frame = "ritardo" dal gate
    Serial.print(F(" | "));

    for (byte s = 0; s < barLen; s++) {
      Serial.print('*');
    }

    Serial.print(F("  ("));
    Serial.print(film[i]);  // valore grezzo 0..1023 (o 0..1020 fast)
    Serial.println(')');
  }

  Serial.println();
}

#endif
