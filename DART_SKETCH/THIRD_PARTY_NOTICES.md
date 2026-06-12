# Third-Party Notices

This project uses third-party components. Each component keeps its own license.

## Arduino Core / USB / Official Libraries
- **Arduino Core & USB (PluggableUSB, Mouse.h, Keyboard.h, EEPROM original)**  
  License: **LGPL-2.1-or-later** (or compatible, as stated in the upstream headers).  
  **Notes:** used as linked libraries; no copyleft on this sketch. If you modify LGPL files, publish the modifications and keep their license headers.

- **AVR/Toolchain headers** (`avr/io.h`, `avr/interrupt.h`, `util/delay.h`, etc.)  
  Part of the AVR toolchain; permissive licensing from the toolchain vendor.  
  **Notes:** no action required for this sketch; keep upstream notices when present.

## Libraries included in this repo
- **LedControl (MAX7219/7221)** — © Eberhard Fahle  
  License: **MIT** (see `LedControl.h/.cpp`).  
  **Notes:** keep the MIT notice in the files.

- **_DART_EEPROM.h** — derived from Arduino EEPROM  
  License: **LGPL-2.1-or-later** (see header in the file).  
  **Notes:** keep the header; any modifications remain under LGPL.

- **_DART_MIDI.* (Gary Grewal)** — © 2015 Gary Grewal  
  License: **ISC** (permissive; header in the file).  
  **Notes:** keep the ISC header.

## DMX
- **DmxSimple (historic)** — upstream header “All rights reserved” (no clear OSS license).  
  **Project status:** removed. DMX features are disabled via `#define DMX_active 0`.  
  In the future, select a DMX library with a clear license (e.g., BSD/LGPL) before re-enabling DMX.

---
This file is informational. When in doubt, refer to the original license texts in the upstream sources.


---
Note: questo file è informativo; in caso di dubbi, fare riferimento ai testi di licenza presenti nei sorgenti originali.
