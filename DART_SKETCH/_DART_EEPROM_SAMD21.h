#pragma once
/*
  DART EEPROM emulation for SAMD21 (Adafruit Metro M0 Express)
  - Emula 1024 bytes di EEPROM usando una "mirror" in RAM + commit su flash.
  - IMPORTANTISSIMO: write/update NON scrivono in flash. Solo commit() scrive in flash.
  - Questo file viene compilato SOLO se:
      ENABLE_EEPROM == 1
      e ARDUINO_ARCH_SAMD definito dal core.
*/

#include <Arduino.h>
#include <stdint.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Config fissa per SAMD21G18A (Metro M0 Express): 256 KB flash.
// Area riservata in fondo (4096 bytes) per stare larghi.
// -----------------------------------------------------------------------------
static const uint32_t DART_EE_MAGIC = 0x0DA7B00Bu;
static const uint16_t DART_EE_SIZE  = 1024;

static const uint32_t DART_PAGE_SIZE = 64;   // SAMD21 page size
static const uint32_t DART_ROW_SIZE  = 256;  // SAMD21 row size (4 pages)

static const uint32_t DART_FLASH_TOTAL_SIZE     = 256u * 1024u;
static const uint32_t DART_FLASH_BASE_ADDR      = 0x00000000u;
static const uint32_t DART_FLASH_RESERVED_BYTES = 4096u;

// Base area (allineata a row)
static const uint32_t DART_EE_FLASH_BASE =
  (DART_FLASH_BASE_ADDR + DART_FLASH_TOTAL_SIZE - DART_FLASH_RESERVED_BYTES);

// -----------------------------------------------------------------------------
// Layout su flash
// -----------------------------------------------------------------------------
struct DART_EE_Block {
  uint32_t magic;
  uint32_t boots;
  uint8_t  data[DART_EE_SIZE];
  uint32_t checksum;
};

static const uint32_t DART_EE_BLOCK_SIZE = (uint32_t)sizeof(DART_EE_Block);

// -----------------------------------------------------------------------------
// Helper
// -----------------------------------------------------------------------------
static inline uint32_t dart_checksum32(const uint8_t* p, uint16_t n) {
  uint32_t s = 0;
  for (uint16_t i = 0; i < n; i++) s += p[i];
  return s;
}

static inline bool dart_block_is_valid(const DART_EE_Block& b) {
  if (b.magic != DART_EE_MAGIC) return false;
  return (dart_checksum32(b.data, DART_EE_SIZE) == b.checksum);
}

// -----------------------------------------------------------------------------
// Low-level NVMCTRL
// -----------------------------------------------------------------------------
static inline void dart_nvm_wait_ready() {
  while (!NVMCTRL->INTFLAG.bit.READY) { }
}

static inline void dart_nvm_erase_row(uint32_t addr) {
  dart_nvm_wait_ready();
  NVMCTRL->ADDR.reg  = addr / 2; // address in 16-bit words
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
  dart_nvm_wait_ready();
}

static inline void dart_nvm_write_page(uint32_t addr, const uint8_t* data64) {
  dart_nvm_wait_ready();

  // Clear page buffer
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
  dart_nvm_wait_ready();

  // Copy 64 bytes into page buffer (16-bit writes)
  volatile uint16_t* dst = (volatile uint16_t*)addr;
  const uint16_t*    src = (const uint16_t*)data64;

  for (uint32_t i = 0; i < (DART_PAGE_SIZE / 2); i++) {
    dst[i] = src[i];
  }

  // Write page
  NVMCTRL->ADDR.reg  = addr / 2;
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
  dart_nvm_wait_ready();
}

static inline void dart_flash_write_block(uint32_t baseAddr, const uint8_t* buf, uint32_t len) {
  uint32_t end = baseAddr + len;

  // Erase all rows involved (align down)
  uint32_t rowStart = (baseAddr / DART_ROW_SIZE) * DART_ROW_SIZE;
  for (uint32_t a = rowStart; a < end; a += DART_ROW_SIZE) {
    dart_nvm_erase_row(a);
  }

  // Write pages (64 bytes)
  uint8_t pageBuf[DART_PAGE_SIZE];
  uint32_t addr = baseAddr;
  uint32_t pos  = 0;

  while (pos < len) {
    memset(pageBuf, 0xFF, sizeof(pageBuf));
    uint32_t chunk = len - pos;
    if (chunk > DART_PAGE_SIZE) chunk = DART_PAGE_SIZE;

    memcpy(pageBuf, buf + pos, chunk);
    dart_nvm_write_page(addr, pageBuf);

    addr += DART_PAGE_SIZE;
    pos  += chunk;
  }
}

// -----------------------------------------------------------------------------
// EEPROM-like class (API compatibile con Arduino EEPROM)
// -----------------------------------------------------------------------------
class DART_EEPROM_SAMD21_Class {
public:
  DART_EEPROM_SAMD21_Class() {
    // Mirror safe default: se qualcuno legge prima di begin(), restituiamo 0xFF.
    memset(_mirror, 0xFF, DART_EE_SIZE);
    _dirty = false;
    _boots = 0;
  }
  void begin();                 // carica mirror da flash (o format se invalida)
  void format(uint8_t fill);    // inizializza mirror e committa
  void commit();                // scrive su flash SOLO se dirty

  uint8_t read(int addr) const;
  void    write(int addr, uint8_t val);
  void    update(int addr, uint8_t val);

  template<typename T>
  T &get(int idx, T &t) {
    uint8_t* ptr = (uint8_t*)&t;
    for (uint16_t i = 0; i < (uint16_t)sizeof(T); i++) {
      ptr[i] = read(idx + (int)i);
    }
    return t;
  }

  template<typename T>
  const T &put(int idx, const T &t) {
    const uint8_t* ptr = (const uint8_t*)&t;
    for (uint16_t i = 0; i < (uint16_t)sizeof(T); i++) {
      update(idx + (int)i, ptr[i]);
    }
    return t;
  }

  uint16_t length() const { return DART_EE_SIZE; }

private:
  uint8_t  _mirror[DART_EE_SIZE];
  bool     _dirty = false;
  uint32_t _boots = 0;

  void _load_from_flash();
  void _build_block(DART_EE_Block& b) const;
};

// Global instance "EEPROM" (come su AVR)
extern DART_EEPROM_SAMD21_Class EEPROM;
