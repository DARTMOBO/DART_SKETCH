
/* 
//  questa versione  è ok, ho precisione e velocità che aumenta in modo temporizzato con levetta al massimo... prossime ptrove : risparmiare memoria e cpu
//  01 08 2025 ore 14.21
 
void mouse_control() {
  #if defined(__AVR_ATmega32U4__)
  if (mouse_mempos != 0) {

    dmxtable[mouse_mempos] = 50;  // Test: 50 è una buona velocità iniziale

    static int16_t accum_x = 0;
    static int16_t accum_y = 0;

    #ifdef ENABLE_BOOSTAX
    // Boostax: contatori indipendenti per X e Y
    static uint16_t boost_counter_x = 0;
    static uint16_t boost_counter_y = 0;

    // Parametri Boostax
    const uint16_t boost_delay_max = 200;      // Aspetta 200 cicli prima di partire
    const uint8_t  boost_step_delay = 12;      // Sale di 1 ogni 50 cicli
    const uint8_t  boost_max = 35;             // Valore massimo del boost
    #endif

    int8_t raw_speed = dmxtable[mouse_mempos];
    int8_t Mousespeed = raw_speed - 32;  // Neutro = 32 → 0

    // === ASSE X ===
    if (chan == minvalue[mouse_mempos]) {
      valore = analogRead(plexer);
      mousex = 111 + ((valore + 1) / 32);
      int8_t dx = mousex - 127;

      if (abs(dx) <= 2) {
        dx = 0;
        accum_x = 0;
        #ifdef ENABLE_BOOSTAX
        boost_counter_x = 0;  // Reset boost se nella deadzone
        #endif
      } else {

        int8_t boost = 0;

        #ifdef ENABLE_BOOSTAX
        // Boostax: incremento o decremento del contatore
        if (abs(dx) >= 14 && boost_counter_x < (boost_delay_max + boost_step_delay * boost_max)) {
          boost_counter_x++;  // Spingi forte = accumula cicli
        } else if (abs(dx) < 14 && boost_counter_x > 0) {
          boost_counter_x--;  // Rilasci = decrementa
        }

        // Calcola il valore attuale di boost
        if (boost_counter_x > boost_delay_max) {
          boost = (boost_counter_x - boost_delay_max) / boost_step_delay;
          if (boost > boost_max) boost = boost_max;
        }
        #endif

        int8_t speed_total = Mousespeed + boost;

        if (speed_total > 0) {
          accum_x += (dx * speed_total) / 8;

          if (abs(accum_x) >= 16) {
            int8_t move_x = accum_x / 16;
            accum_x -= move_x * 16;

            #if (hid_mouse == 1)
            Mouse.move(move_x, 0, 0);
            #endif
          }
        }

        // === DEBUG SERIAL OUTPUT ===
        Serial.println(F("-------- X"));
        Serial.print(F("dx: ")); Serial.println(dx);
        Serial.print(F("Mousespeed: ")); Serial.println(Mousespeed);
        #ifdef ENABLE_BOOSTAX
        Serial.print(F("Boost: ")); Serial.println(boost);
        Serial.print(F("Boost Counter X: ")); Serial.println(boost_counter_x);
        #endif
        Serial.print(F("Speed Total: ")); Serial.println(speed_total);
        Serial.print(F("Accum X: ")); Serial.println(accum_x);
      }
    }

    // === ASSE Y ===
    else if (chan == maxvalue[mouse_mempos]) {
      valore = analogRead(plexer);
      mousex = 111 + ((valore + 1) / 32);
      int8_t dy = mousex - 127;

      if (abs(dy) <= 2) {
        dy = 0;
        accum_y = 0;
        #ifdef ENABLE_BOOSTAX
        boost_counter_y = 0;  // Reset boost se nella deadzone
        #endif
      } else {

        int8_t boost = 0;

        #ifdef ENABLE_BOOSTAX
        // Boostax: incremento o decremento del contatore
        if (abs(dy) >= 14 && boost_counter_y < (boost_delay_max + boost_step_delay * boost_max)) {
          boost_counter_y++;
        } else if (abs(dy) < 14 && boost_counter_y > 0) {
          boost_counter_y--;
        }

        // Calcola il valore attuale di boost
        if (boost_counter_y > boost_delay_max) {
          boost = (boost_counter_y - boost_delay_max) / boost_step_delay;
          if (boost > boost_max) boost = boost_max;
        }
        #endif

        int8_t speed_total = Mousespeed + boost;

        if (speed_total > 0) {
          accum_y += (dy * speed_total) / 8;

          if (abs(accum_y) >= 16) {
            int8_t move_y = accum_y / 16;
            accum_y -= move_y * 16;

            #if (hid_mouse == 1)
            Mouse.move(0, move_y, 0);
            #endif
          }
        }

        // === DEBUG SERIAL OUTPUT ===
        Serial.println(F("-------- Y"));
        Serial.print(F("dy: ")); Serial.println(dy);
        Serial.print(F("Mousespeed: ")); Serial.println(Mousespeed);
        #ifdef ENABLE_BOOSTAX
        Serial.print(F("Boost: ")); Serial.println(boost);
        Serial.print(F("Boost Counter Y: ")); Serial.println(boost_counter_y);
        #endif
        Serial.print(F("Speed Total: ")); Serial.println(speed_total);
        Serial.print(F("Accum Y: ")); Serial.println(accum_y);
      }
    }
  }
  #endif
}

*/
#define ENABLE_BOOSTAX  // Disattiva commentando questa riga

void mouse_control() {
  #if defined(__AVR_ATmega32U4__)
  if (mouse_mempos != 0) {

    dmxtable[mouse_mempos] = 50;  // Test: 50 è una buona velocità iniziale

    static int16_t accum_x = 0;
    static int16_t accum_y = 0;

    #ifdef ENABLE_BOOSTAX
    static uint16_t boost_counter_x = 0;
    static uint16_t boost_counter_y = 0;

    const uint16_t boost_delay_max = 200;
    const uint8_t  boost_step_delay = 12;
    const uint8_t  boost_max = 35;
    #endif

    int8_t raw_speed = dmxtable[mouse_mempos];
    int8_t Mousespeed = raw_speed - 32;

    // === ASSE X ===
    if (chan == minvalue[mouse_mempos]) {
      valore = analogRead(plexer);
      mousex = 111 + ((valore + 1) / 32);
      int8_t dx = mousex - 127;

      if (abs(dx) <= 2) {
        dx = 0;
        accum_x = 0;
        #ifdef ENABLE_BOOSTAX
        boost_counter_x = 0;
        #endif
      } else {

        int8_t boost = 0;

        #ifdef ENABLE_BOOSTAX
        // Boostax semplificato
        if (abs(dx) >= 14) {
          if (boost_counter_x < (boost_delay_max + boost_step_delay * boost_max))
            boost_counter_x++;
        } else if (boost_counter_x > 0) {
          boost_counter_x--;
        }

        if (boost_counter_x > boost_delay_max) {
          boost = (boost_counter_x - boost_delay_max) / boost_step_delay;
          if (boost > boost_max) boost = boost_max;
        }
        #endif

        int8_t speed_total = Mousespeed + boost;

        if (speed_total > 0) {
          accum_x += (dx * speed_total) / 8;

          if (abs(accum_x) >= 16) {
            int8_t move_x = accum_x / 16;
            accum_x -= move_x * 16;

            #if (hid_mouse == 1)
            Mouse.move(move_x, 0, 0);
            #endif
          }
        }

        // === DEBUG SERIAL OUTPUT ===
        Serial.println(F("-------- X"));
        Serial.print(F("dx: ")); Serial.println(dx);
        Serial.print(F("Mousespeed: ")); Serial.println(Mousespeed);
        #ifdef ENABLE_BOOSTAX
        Serial.print(F("Boost: ")); Serial.println(boost);
        Serial.print(F("Boost Counter X: ")); Serial.println(boost_counter_x);
        #endif
        Serial.print(F("Speed Total: ")); Serial.println(speed_total);
        Serial.print(F("Accum X: ")); Serial.println(accum_x);
      }
    }

    // === ASSE Y ===
    else if (chan == maxvalue[mouse_mempos]) {
      valore = analogRead(plexer);
      mousex = 111 + ((valore + 1) / 32);
      int8_t dy = mousex - 127;

      if (abs(dy) <= 2) {
        dy = 0;
        accum_y = 0;
        #ifdef ENABLE_BOOSTAX
        boost_counter_y = 0;
        #endif
      } else {

        int8_t boost = 0;

        #ifdef ENABLE_BOOSTAX
        // Boostax semplificato
        if (abs(dy) >= 14) {
          if (boost_counter_y < (boost_delay_max + boost_step_delay * boost_max))
            boost_counter_y++;
        } else if (boost_counter_y > 0) {
          boost_counter_y--;
        }

        if (boost_counter_y > boost_delay_max) {
          boost = (boost_counter_y - boost_delay_max) / boost_step_delay;
          if (boost > boost_max) boost = boost_max;
        }
        #endif

        int8_t speed_total = Mousespeed + boost;

        if (speed_total > 0) {
          accum_y += (dy * speed_total) / 8;

          if (abs(accum_y) >= 16) {
            int8_t move_y = accum_y / 16;
            accum_y -= move_y * 16;

            #if (hid_mouse == 1)
            Mouse.move(0, move_y, 0);
            #endif
          }
        }

        // === DEBUG SERIAL OUTPUT ===
        Serial.println(F("-------- Y"));
        Serial.print(F("dy: ")); Serial.println(dy);
        Serial.print(F("Mousespeed: ")); Serial.println(Mousespeed);
        #ifdef ENABLE_BOOSTAX
        Serial.print(F("Boost: ")); Serial.println(boost);
        Serial.print(F("Boost Counter Y: ")); Serial.println(boost_counter_y);
        #endif
        Serial.print(F("Speed Total: ")); Serial.println(speed_total);
        Serial.print(F("Accum Y: ")); Serial.println(accum_y);
      }
    }
  }
  #endif
}
