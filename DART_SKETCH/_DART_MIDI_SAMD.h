//================================================================================
// DART MIDI layer for SAMD (Adafruit Metro M0 Express / SAMD21)
// USB-MIDI class compliant via Adafruit TinyUSB stack.
//
// NOTE:
// - MIDI OUT + MIDI IN helpers for SAMD/TinyUSB.
// - Requires: Tools -> USB Stack -> TinyUSB (Adafruit SAMD core menu)
//
//================================================================================

#pragma once

#if defined(ARDUINO_ARCH_SAMD) && !defined(__AVR_ATmega32U4__)

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

// The actual USB-MIDI device instance lives in _DART_MIDI_SAMD.cpp
extern Adafruit_USBD_MIDI usb_midi;

// Call this often (once per loop is enough) so TinyUSB can do its housekeeping.
static inline void DART_MIDI_Task_SAMD() {
  TinyUSBDevice.task();
}

// Initialize the USB-MIDI interface so the port enumerates even if DART is not
// sending any messages yet.
static inline void DART_MIDI_Init_SAMD() {
  usb_midi.begin();
  TinyUSBDevice.task();
}

// Compute USB-MIDI CIN (Code Index Number) from status byte (channel voice only).
static inline uint8_t DART_MIDI_CIN_from_status(uint8_t status) {
  switch (status & 0xF0) {
    case 0x80: return 0x08; // Note Off
    case 0x90: return 0x09; // Note On
    case 0xA0: return 0x0A; // Poly Key Pressure
    case 0xB0: return 0x0B; // Control Change
    case 0xC0: return 0x0C; // Program Change (2 bytes)
    case 0xD0: return 0x0D; // Channel Pressure (2 bytes)
    case 0xE0: return 0x0E; // Pitch Bend (3 bytes)
    default:   return 0x0F; // Single Byte / fallback
  }
}

// Send a MIDI message over USB on SAMD/TinyUSB.
//
// CTRL-F: DART_M0_MIDI_SEND_MODE
// 0 = USB-MIDI packet a 4 byte (vecchia tecnica)
// 1 = RAW 3-byte (status, data1, data2), allineato allo sketch di test M0
//     che ha eliminato i messaggi fantasma osservati con alcuni ambienti.
static inline void DART_MIDI_Send_SAMD(uint8_t status, uint8_t data1, uint8_t data2) {
#if (DART_M0_MIDI_SEND_MODE == 0)
  uint8_t cin = DART_MIDI_CIN_from_status(status);

  uint8_t packet[4] = { 0, 0, 0, 0 };
  packet[0] = (uint8_t)(cin);
  packet[1] = status;
  packet[2] = data1;

  // Program Change + Channel Pressure are 2-byte messages.
  if (((status & 0xF0) == 0xC0) || ((status & 0xF0) == 0xD0)) {
    packet[3] = 0;
  } else {
    packet[3] = data2;
  }

  usb_midi.write(packet, 4);
#else
  usb_midi.write(status);
  usb_midi.write(data1);

  // Per i messaggi a 2 byte il terzo byte non serve: manteniamo lo stesso
  // comportamento minimale del test M0 e non inventiamo padding extra.
  if (((status & 0xF0) != 0xC0) && ((status & 0xF0) != 0xD0)) {
    usb_midi.write(data2);
  }
#endif
  usb_midi.flush();
}

// Read one raw MIDI byte from USB IN on SAMD/TinyUSB.
// Returns true only when a byte has been received.
static inline bool DART_MIDI_ReadByte_SAMD(uint8_t* outByte) {
  if (usb_midi.available() > 0) {
    int midi_in_byte = usb_midi.read();
    if (midi_in_byte >= 0) {
      *outByte = (uint8_t)midi_in_byte;
      return true;
    }
  }
  return false;
}

// Optional: simple heartbeat to confirm the port is alive (send CC#20 val=64 every 1000 ms).
#ifndef DART_M0_MIDI_HEARTBEAT
  #define DART_M0_MIDI_HEARTBEAT 0
#endif

static inline void DART_MIDI_Heartbeat_SAMD() {
#if (DART_M0_MIDI_HEARTBEAT == 1)
  static uint32_t last_ms = 0;
  uint32_t now = millis();
  if (now - last_ms >= 1000) {
    last_ms = now;
    // CC#20 value 64 on ch1
    DART_MIDI_Send_SAMD(0xB0, 20, 64);
  }
#endif
}

#endif // ARDUINO_ARCH_SAMD
