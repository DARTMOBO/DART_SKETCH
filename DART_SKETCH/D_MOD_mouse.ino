/*void mouse_control() {
  #if defined (__AVR_ATmega32U4__)  
  if (mouse_mempos != 0) {

    // Accumulatori statici per filtrare i micro movimenti
    static int16_t accum_x = 0;
    static int16_t accum_y = 0;

    int8_t Mousespeed = dmxtable[mouse_mempos]; // ⚠️ TODO: ricalibrare scala → 32 = normal

    if (chan == minvalue[mouse_mempos]) {  // Asse X
      valore = analogRead(plexer);
      mousex = 111 + ((valore + 1) / 32);  // maggiore risoluzione → da 111 a 143 (33 step)

      #if (mouse_block == 1)
      if (mousex > 133) {
        if (qwertyvalue[mouse_mempos] < 255) qwertyvalue[mouse_mempos]++;
      } else {
        qwertyvalue[mouse_mempos] = 0;
      }
      if (qwertyvalue[mouse_mempos] < 230)
      #endif

      if (Mousespeed > 0 && Mousespeed < 64) {
        int8_t dx = (mousex - 127);
       
if (abs(dx) <= 2) {
  dx = 0;
  accum_x = 0;
}

        int8_t direction = (constrain(lightable[mouse_mempos] - 1, 0, 1) * 2) - 1;

        accum_x += dx * Mousespeed * direction;

        if (abs(accum_x) >= 64) {
          int8_t move_x = accum_x / 16;
          accum_x -= move_x * 16;

          #if (hid_mouse == 1)
          Mouse.move(move_x, 0, 0);
          #endif
        }
      }
    }

    else if (chan == maxvalue[mouse_mempos]) {  // Asse Y
      valore = analogRead(plexer);
      mousex = 111 + ((valore + 1) / 32);  // anche per Y usiamo il nuovo mapping

      if (mousex > 133) {
        if (mousey < 255) mousey++;
      } else {
        mousey = 0;
      }

      #if (mouse_block == 1)
      if (mousey < 230)
      #endif

      if (Mousespeed > 0 && Mousespeed < 64) {
        int8_t dy = (mousex - 127);

        if (abs(dy) <= 2) {
  dy = 0;
  accum_y = 0;
}

        int8_t direction = ((lightable[mouse_mempos] & 1) * 2) - 1;

        accum_y += dy * Mousespeed * direction;

        if (abs(accum_y) >= 64) {
          int8_t move_y = accum_y / 16;
          accum_y -= move_y * 16;

          #if (hid_mouse == 1)
          Mouse.move(0, move_y, 0);
          #endif
        }
      }
    }
  }
  #endif
}
*/



/// questa funziona molto bene - forse pesante
/*
void mouse_control() {
  #if defined (__AVR_ATmega32U4__)
  if (mouse_mempos != 0) {

    // === 🎚️ PARAMETRI DI CONTROLLO ===
    const int DEADZONE = 1;           // Zona morta centrale: 0 = disattivata, 2 = ±2 intorno a 127
    const int SPEED_DIVISORE = 150;    // Valore più alto = movimento più lento
    const int SPEED_BOOST = 12;        // Moltiplica la velocità reale (1 = neutro, 2 = più veloce, 0.5 = più lenta)

    // === Accumulatori statici per continuità ===
    static int16_t accum_x = 0;
    static int16_t accum_y = 0;

    int8_t Mousespeed = dmxtable[mouse_mempos];

    if (chan == minvalue[mouse_mempos]) {  // Asse X
      valore = analogRead(plexer);
      mousex = 111 + ((valore + 1) / 32);  // Risoluzione fine

      #if (mouse_block == 1)
      if (mousex > 133) {
        if (qwertyvalue[mouse_mempos] < 255) qwertyvalue[mouse_mempos]++;
      } else {
        qwertyvalue[mouse_mempos] = 0;
      }
      if (qwertyvalue[mouse_mempos] < 230)
      #endif

      if (Mousespeed > 0 && Mousespeed < 64) {
        int8_t dx = (mousex - 127);
        if (abs(dx) <= DEADZONE) {
          dx = 0;
          accum_x = 0;
        }

        int8_t direction = (constrain(lightable[mouse_mempos] - 1, 0, 1) * 2) - 1;
        accum_x += dx * Mousespeed * direction * SPEED_BOOST;

        if (abs(accum_x) >= SPEED_DIVISORE) {
          int8_t move_x = accum_x / SPEED_DIVISORE;
          accum_x -= move_x * SPEED_DIVISORE;

          #if (hid_mouse == 1)
          Mouse.move(move_x, 0, 0);
          #endif
        }
      }
    }

    else if (chan == maxvalue[mouse_mempos]) {  // Asse Y
      valore = analogRead(plexer);
      mousex = 111 + ((valore + 1) / 32);

      if (mousex > 133) {
        if (mousey < 255) mousey++;
      } else {
        mousey = 0;
      }

      #if (mouse_block == 1)
      if (mousey < 230)
      #endif

      if (Mousespeed > 0 && Mousespeed < 64) {
        int8_t dy = (mousex - 127);
        if (abs(dy) <= DEADZONE) {
          dy = 0;
          accum_y = 0;
        }

        int8_t direction = ((lightable[mouse_mempos] & 1) * 2) - 1;
        accum_y += dy * Mousespeed * direction * SPEED_BOOST;

        if (abs(accum_y) >= SPEED_DIVISORE) {
          int8_t move_y = accum_y / SPEED_DIVISORE;
          accum_y -= move_y * SPEED_DIVISORE;

          #if (hid_mouse == 1)
          Mouse.move(0, move_y, 0);
          #endif
        }
      }
    }
  }
  #endif
}


*/

void mouse_control() {
  #if defined (__AVR_ATmega32U4__)
  if (mouse_mempos != 0) {

    // Parametri base
    const int DEADZONE = 2;         // zona morta centrale ±2
    const int CYCLE_LIMIT = 20;      // quanti cicli attendere prima di spostare il mouse

    // Counter statici per i due assi
    static uint8_t counter_x = 0;
    static uint8_t counter_y = 0;

    int8_t Mousespeed = dmxtable[mouse_mempos];

    if (chan == minvalue[mouse_mempos]) {  // Asse X
      valore = analogRead(plexer);
      int8_t mousex = 111 + ((valore + 1) / 32);
      int8_t dx = mousex - 127;

      if (abs(dx) <= DEADZONE) dx = 0;

      int8_t direction = (constrain(lightable[mouse_mempos] - 1, 0, 1) * 2) - 1;

      counter_x++;
      if (counter_x >= CYCLE_LIMIT && dx != 0 && Mousespeed > 0) {
        counter_x = 0;

        #if (hid_mouse == 1)
        Mouse.move(dx * Mousespeed * direction, 0, 0);
        #endif
      }
    }

    else if (chan == maxvalue[mouse_mempos]) {  // Asse Y
      valore = analogRead(plexer);
      int8_t mousex = 111 + ((valore + 1) / 32);
      int8_t dy = mousex - 127;

      if (abs(dy) <= DEADZONE) dy = 0;

      int8_t direction = ((lightable[mouse_mempos] & 1) * 2) - 1;

      counter_y++;
      if (counter_y >= CYCLE_LIMIT && dy != 0 && Mousespeed > 0) {
        counter_y = 0;

        #if (hid_mouse == 1)
        Mouse.move(0, dy * Mousespeed * direction, 0);
        #endif
      }
    }
  }
  #endif
}
