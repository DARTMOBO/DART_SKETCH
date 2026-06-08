#if defined(ARDUINO_ARCH_SAMD) && !defined(__AVR_ATmega32U4__)
#include <Adafruit_TinyUSB.h>

// Global USB-MIDI instance for SAMD boards (Metro M0 etc.)
Adafruit_USBD_MIDI usb_midi;
#endif
