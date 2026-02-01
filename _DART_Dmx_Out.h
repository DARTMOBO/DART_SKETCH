

/**
 * DmxSimple - A simple interface to DMX.
 *
 * Copyright (c) 2008-2009 Peter Knight, Tinker.it! All rights reserved.
 */

#ifndef DmxSimple_h
#define DmxSimple_h

#include <inttypes.h>

// DART: dimensione del buffer DMX
// --------------------------------
// Usiamo un define unico DART_DMX_MAX_CHANNELS (definito in config.h)
// per decidere quanti canali DMX tenere in RAM.
//
// Motivo attuale: l’editor permette indirizzi solo fino a 127,
// e l’uso DMX del DART è ludico/creativo → 127 canali sono più che sufficienti.
//
// Per aumentare in futuro a 256 / 512 canali:
//   - modifica DART_DMX_MAX_CHANNELS in config.h
//   - ricompila lo sketch
//
#ifndef DART_DMX_MAX_CHANNELS
  // fallback di sicurezza, nel caso qualcuno dimentichi di definirlo in config.h
  #define DART_DMX_MAX_CHANNELS 127
#endif

#define DMX_SIZE DART_DMX_MAX_CHANNELS


class DmxSimpleClass
{
  public:
    void maxChannel(int);
    void write(int, uint8_t);
    void usePin(uint8_t);
};
extern DmxSimpleClass DmxSimple;

#endif

 
