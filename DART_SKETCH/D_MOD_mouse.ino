 /*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DART_SKETCH   —   Copyright (c) 2015–2025 M. Marchese - dartmobo.com
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See the LICENSE file for details.
 */


#define ENABLE_BOOSTAX  // Disattiva commentando questa riga

// ------------------------------------------------------------
// helper unico per X e Y (più corto, evita abs(), meno confronto)
// moveAxis: 0 = X, 1 = Y
// ------------------------------------------------------------
static void mouse_axis_step(int8_t d, int16_t* accum,
#ifdef ENABLE_BOOSTAX
                            uint16_t* boost_counter,
#endif
                            int8_t baseSpeed,
                            uint8_t moveAxis)
{
  // deadzone: abs(d) <= 2  --> confronto equivalente senza abs()
  if (d >= -2 && d <= 2) {
    *accum = 0;
#ifdef ENABLE_BOOSTAX
    *boost_counter = 0;
#endif
    return;
  }

  int8_t boost = 0;

#ifdef ENABLE_BOOSTAX
  // Costanti identiche alla tua taratura
  const uint16_t boost_delay_max  = 200;
  const uint8_t  boost_step_delay = 12;
  const uint8_t  boost_max        = 35;
  const uint16_t boost_limit      = 620; // 200 + 12*35

  // abs(d) >= 14  --> confronto equivalente
  if (d <= -14 || d >= 14) {
    if (*boost_counter < boost_limit) (*boost_counter)++;
  } else {
    if (*boost_counter > 0) (*boost_counter)--;
  }

  if (*boost_counter > boost_delay_max) {
    boost = (int8_t)((*boost_counter - boost_delay_max) / boost_step_delay);
    if (boost > boost_max) boost = boost_max;
  }
#endif

  const int8_t speed_total = baseSpeed + boost;

  if (speed_total <= 0) return;

  *accum += (int16_t)((d * speed_total) / 8);

  // abs(*accum) >= 16  --> confronto equivalente senza abs()
  if (*accum <= -16 || *accum >= 16) {
    int8_t move = (int8_t)(*accum / 16);
    *accum -= (int16_t)(move * 16);

    #if (hid_mouse == 1)
      if (moveAxis == 0) Mouse.move(move, 0, 0);
      else               Mouse.move(0, move, 0);
    #endif
  }
}


// ------------------------------------------------------------
// mouse_control() compatibile con la tua logica attuale
// ------------------------------------------------------------
void mouse_control() {
  #if defined(__AVR_ATmega32U4__)
  if (mouse_mempos != 0) {

    static int16_t accum_x = 0;
    static int16_t accum_y = 0;

#ifdef ENABLE_BOOSTAX
    static uint16_t boost_counter_x = 0;
    static uint16_t boost_counter_y = 0;
#endif

    int8_t raw_speed = dmxtable[mouse_mempos];
    if (raw_speed == 0) return;
    int8_t baseSpeed = raw_speed - 32;

    // inversioni via lightable[mouse_mempos]
    // bit0 = invert Y, bit1 = invert X
    byte inv = lightable[mouse_mempos] & 0x03;

    // ---- ASSE X ----
    if (chan == minvalue[mouse_mempos]) {

      valore = analogRead_1024(plexer);

      // prima: mousex = 111 + ((valore + 1) / 32); d = mousex - 127;
      // ora: d = ((valore + 1) >> 5) - 16;  (identico, ma più corto)
      int8_t d = (int8_t)(((valore + 1) >> 5) - 16);

      if (inv & 0x02) d = -d;  // invert X

      mouse_axis_step(d, &accum_x,
#ifdef ENABLE_BOOSTAX
                      &boost_counter_x,
#endif
                      baseSpeed, 0);
    }

    // ---- ASSE Y ----
    else if (chan == maxvalue[mouse_mempos]) {

      valore = analogRead_1024(plexer);

      int8_t d = (int8_t)(((valore + 1) >> 5) - 16);

      if (inv & 0x01) d = -d;  // invert Y

      mouse_axis_step(d, &accum_y,
#ifdef ENABLE_BOOSTAX
                      &boost_counter_y,
#endif
                      baseSpeed, 1);
    }
  }
  #endif
}


//-------------------------------------------
