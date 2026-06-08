#if defined(ARDUINO_ARCH_SAMD)
#include "_DART_EEPROM_SAMD21.h"

// Global instance
DART_EEPROM_SAMD21_Class EEPROM;

// -----------------------------------------------------------------------------
// Internals
// -----------------------------------------------------------------------------
static inline const DART_EE_Block* dart_flash_block_ptr() {
  return (const DART_EE_Block*)DART_EE_FLASH_BASE;
}

void DART_EEPROM_SAMD21_Class::_build_block(DART_EE_Block& b) const {
  b.magic = DART_EE_MAGIC;
  b.boots = _boots;
  memcpy(b.data, _mirror, DART_EE_SIZE);
  b.checksum = dart_checksum32(b.data, DART_EE_SIZE);
}

void DART_EEPROM_SAMD21_Class::_load_from_flash() {
  const DART_EE_Block* fb = dart_flash_block_ptr();
  if (dart_block_is_valid(*fb)) {
    _boots = fb->boots;
    memcpy(_mirror, fb->data, DART_EE_SIZE);
    _dirty = false;
  } else {
    // Se la flash non contiene un blocco valido, partiamo "puliti" a 0xFF
    memset(_mirror, 0xFF, DART_EE_SIZE);
    _boots = 0;
    _dirty = true; // così begin() può commit() e rendere valido il blocco
  }
}

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------
void DART_EEPROM_SAMD21_Class::begin() {
  _load_from_flash();

  // In ogni boot, incrementiamo il contatore e rendiamo persistente,
  // ma SOLO se in precedenza avevamo un blocco valido.
  // Se non era valido, commit() lo renderà valido.
  _boots++;
  _dirty = true;
  commit();
}

void DART_EEPROM_SAMD21_Class::format(uint8_t fill) {
  memset(_mirror, fill, DART_EE_SIZE);
  _dirty = true;
  commit();
}

void DART_EEPROM_SAMD21_Class::commit() {
  if (!_dirty) return;

  DART_EE_Block b;
  _build_block(b);

  dart_flash_write_block(DART_EE_FLASH_BASE, (const uint8_t*)&b, DART_EE_BLOCK_SIZE);

  _dirty = false;
}

uint8_t DART_EEPROM_SAMD21_Class::read(int addr) const {
  if (addr < 0) return 0;
  if (addr >= (int)DART_EE_SIZE) return 0;
  return _mirror[(uint16_t)addr];
}

void DART_EEPROM_SAMD21_Class::write(int addr, uint8_t val) {
  if (addr < 0) return;
  if (addr >= (int)DART_EE_SIZE) return;

  _mirror[(uint16_t)addr] = val;
  _dirty = true;
}

void DART_EEPROM_SAMD21_Class::update(int addr, uint8_t val) {
  if (addr < 0) return;
  if (addr >= (int)DART_EE_SIZE) return;

  uint16_t a = (uint16_t)addr;
  if (_mirror[a] != val) {
    _mirror[a] = val;
    _dirty = true;
  }
}

#endif // ARDUINO_ARCH_SAMD
