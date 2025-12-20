



// SCENE GLOBALS
byte scene_record_armed = 0;   // 1 = RECORD premuto (armato)
byte scene_record_done  = 0;   // 1 = ha già registrato durante questa pressione (anti-mitragliata)


//********************************************++
// ------------------------------------------------------------
// SCENE GLOBALS
// ------------------------------------------------------------
#define SCENE_COUNT        8
#define SCENE_PARAM_COUNT 16
#define SCENE_CHAN_INVALID 255


// ============================================================
// CTRL-F: SCENE_BUILD_CONFIG
// ============================================================
// NOTE: Keep this file as small as possible in FLASH.
// When Matrix_Pads is enabled, we are close to the size limit.
//
// 1) SCENE_DEBUG must be 0 in release builds (Serial strings cost FLASH).
// 2) Avoid divisions like /127 (they pull in heavy libgcc routines on AVR).
#ifndef SCENE_DEBUG
#define SCENE_DEBUG 0   // 0 = no Serial debug (saves FLASH)
#endif

// ------------------------------------------------------------
// FAST MATH HELPERS
// ------------------------------------------------------------

// Signed multiply + (rounded) >>7. Works with negative numbers too.
static inline int16_t _mulShift7Round(int16_t a, uint8_t b)
{
  int32_t p = (int32_t)a * (int32_t)b;
  if (p >= 0) p += 64; else p -= 64;
  return (int16_t)(p >> 7); // approx divide by 128
}

// Scale 0..127 by 0..127, keeping endpoints exact and avoiding /127.
static inline uint8_t _scale127(uint8_t value, uint8_t scale)
{
  if (scale == 0) return 0;
  if (scale >= 127) return value;
  // (value * scale) / 128 with rounding
  uint16_t p = (uint16_t)value * (uint16_t)scale;
  p += 64;
  return (uint8_t)(p >> 7);
}

// Lista dei pot soggetto: ogni entry contiene un "chan" (memoryposition) valido.
// Se non ci sono abbastanza soggetti, i rimanenti restano a 255 (invalid).
byte scene_subject_chan[SCENE_PARAM_COUNT];

// Numero reale di soggetti trovati (0..16). Comodo per debug e loop veloci.
byte scene_subject_count = 0;

// Target delle scene: [scena][indice_soggetto] -> 0..127
// NOTA: l'indice_soggetto è l'indice nella lista scene_subject_chan[].
byte scene_target[SCENE_COUNT][SCENE_PARAM_COUNT];

// Livello corrente di ciascuna scena (0..127), comandato dai pot scene_control (mode 38)
byte scene_level[SCENE_COUNT];

// Ultimo valore effettivamente inviato per ciascun soggetto (0..127)
// Serve per evitare invii ripetuti identici (anti-spam).
byte scene_final_value[SCENE_PARAM_COUNT];

// ============================================================
// CTRL-F: CONVOY_GLOBALS_BEGIN
// CONVOY (sacrificale v1): punto unico per i valori live dei scene-subject
// - in questa fase lo usiamo SOLO per i pot subject taggati come PC (group 3) in pots()
// - più avanti estenderemo a: max-wins scene_control e morph
// RAM: 16+16+2 = 34 byte
// ============================================================
byte convoy_live_value[SCENE_PARAM_COUNT];   // verità corrente (fotografabile)
byte convoy_out_value[SCENE_PARAM_COUNT];    // ultimo valore effettivamente inviato
uint16_t convoy_dirty_mask = 0;

byte convoy_find_subjectIndex(byte mem_chan)
{
  for (byte i = 0; i < scene_subject_count && i < SCENE_PARAM_COUNT; i++)
  {
    if (scene_subject_chan[i] == mem_chan) return i;
  }
  return 255;
}

void convoy_set(byte subjectIndex, byte value)
{
  if (subjectIndex >= SCENE_PARAM_COUNT) return;
  convoy_live_value[subjectIndex] = value;
  convoy_dirty_mask |= (uint16_t)(1 << subjectIndex);
}

void convoy_snapshot(byte* dst)
{
  for (byte i = 0; i < SCENE_PARAM_COUNT; i++) dst[i] = convoy_live_value[i];
}

static void convoy_send_subject(byte mem_chan, byte value)
{
  // stesso schema di invio dei subject in scene_apply (PC->CC, CC, DMX)
  byte t = typetable[mem_chan];
  if (t >= 144) {
    byte group = (t - 144) / 16;
    if (group == 3) {
      noteOn(t - 16, valuetable[mem_chan], value, 0);
    }
    else if (group == 2) {
      noteOn(t, valuetable[mem_chan], value, 0);
    }
  }
  #if (DMX_active == 1)
  if (eeprom_preset_active == 1 && dmxtable[mem_chan] > 0) {
    DmxSimple.write(dmxtable[mem_chan], value * 2);
  }
  #endif
}


// ============================================================
// CTRL-F: CONVOY_SYNC_ENCODER_BASE
// Sync "base interna" per encoder/spinner scene-subject in POT-EMU:
// - spinner (mode 21/22): encodervaluepot[idx] = v*8
// - encoder generico (mode 19): lightable[chan] = v*2
// Nota: SOLO se dmxtable[chan] == 2 (pot emulation)
// ============================================================
static inline void convoy_sync_encoder_base(byte chan, byte v)
{
  if (dmxtable[chan] != 2) return;          // non è pot-emulation → niente sync

  byte mode = modetable[chan];

  // spinner top/side (mode 21/22): base = encodervaluepot[0/1] (0..1023)
  if (mode == 21 || mode == 22)
  {
    if (chan == encoder_mempos[0]) { encodervaluepot[0] = ((int)v) << 3; return; }  // v*8
    if (chan == encoder_mempos[1]) { encodervaluepot[1] = ((int)v) << 3; return; }  // v*8
    return;
  }

  // encoder generico (mode 19): base = lightable[chan] (0..255), out = lightable/2
  if (mode == 19)
  {
    lightable[chan] = (byte)(v << 1);       // v*2 (0..254)
  }
}



void convoy_commit()
{
  uint16_t m = convoy_dirty_mask;
  if (!m) return;
  convoy_dirty_mask = 0;

  for (byte i = 0; i < scene_subject_count && i < SCENE_PARAM_COUNT; i++)
  {
    if (!(m & (uint16_t)(1 << i))) continue;
    byte c = scene_subject_chan[i];
    byte v = convoy_live_value[i];
    if (v == convoy_out_value[i]) continue;
    convoy_out_value[i] = v;
    convoy_send_subject(c, v);

      convoy_sync_encoder_base(c, v); 
    // manteniamo allineato anche scene_final_value (così snapshot vecchi restano coerenti)
    scene_final_value[i] = v;
  }
}

// ============================================================
// CTRL-F: CONVOY_GLOBALS_END
// ============================================================



// ============================================================
// CTRL-F: MORPH_GLOBALS_BEGIN
// MORPH (ENCSC) - RAM minima, niente pageswitch
// NOTA: NON usare maxvalue[]: è lo stato quadratura (0..3) in updateEncoder()!
// ============================================================


#define MORPH_TEST_FORCE_SCENE12  0   // 0 = modalità normale (snapshot + valuetable)
#define MORPH_STEP                1   // step base (1 = più fine)
#define MORPH_PULSE_DIV           6   // rallenta spinner: cambia pos ogni N impulsi (6 è un buon start)

// dopo N ms senza impulsi, al prossimo movimento rifacciamo la FOTO e ripartiamo da morph=0
#define MORPH_REARM_MS            350



byte morph_owner = 255;
byte morph_active = 0;
byte morph_scene  = 0;                // usato solo quando NON siamo in test forced
byte morph_value  = 0;                // 0..127
byte morph_base[SCENE_PARAM_COUNT];   // foto base (16 byte)

// posizione per i 2 encoder fisici (indicizzati via encoder_mempos[0/1])
byte morph_pos[2]    = {0, 0};
byte morph_divcnt[2] = {0, 0};
unsigned long morph_last_ms[2] = {0, 0};

void scene_morph_apply(byte scene_index, byte morph);
void scene_morph_encsc(byte enc_chan);

// ============================================================
// CTRL-F: MORPH_GLOBALS_END
// ============================================================






byte scene_record_blink_led = 0;        // 1..N (come lightable), 0=nessuno

//------------------------------------------------------------
// scene_eeprom

#include <EEPROM.h>

// ------------------------------------------------------------
// SCENE EEPROM (usa area ex PAGE2: 512..1023)
// ------------------------------------------------------------
#define EEPROM_SCENE_BASE       512
#define EEPROM_SCENE_HDR_SIZE   8

// Header offsets (EEPROM_SCENE_BASE + ...):
#define SC_EE_SIG0   0   // 'S'
#define SC_EE_SIG1   1   // 'C'
#define SC_EE_VER    2   // versione struttura
#define SC_EE_SCN    3   // scene count
#define SC_EE_PAR    4   // param count
#define SC_EE_FLAGS  5   // flags
#define SC_EE_CSUM   6   // checksum XOR dei dati
#define SC_EE_RSVD   7   // reserved

#define SCENE_EEPROM_VERSION    1

// Dati scene partono qui:
#define EEPROM_SCENE_DATA_BASE  (EEPROM_SCENE_BASE + EEPROM_SCENE_HDR_SIZE)

// Flags
#define SCENE_FLAG_ENABLED      1

byte scene_eeprom_calc_checksum()
{
  // XOR di tutti i target (8x16 = 128 byte)
  byte x = 0;
  for (byte s = 0; s < SCENE_COUNT; s++) {
    for (byte i = 0; i < SCENE_PARAM_COUNT; i++) {
      x ^= scene_target[s][i];
    }
  }
  return x;
}

//-----------------------------------------------------------------------------------------------------------------------

void scene_build_subject_list()
{
  // 1) reset lista + contatore
  scene_subject_count = 0;
  for (byte i = 0; i < SCENE_PARAM_COUNT; i++) {
    scene_subject_chan[i] = SCENE_CHAN_INVALID;
    scene_final_value[i]  = 0; // reset anti-spam (scelta utile)
  }

  // 2) scan in ordine di memoryposition
  for (byte c = 0; c < max_modifiers; c++) {

    // stop se piena
    if (scene_subject_count >= SCENE_PARAM_COUNT) break;

    // sicurezza: modetable_readmode ha 40 elementi (0..39)
    byte mode = modetable[c];
    if (mode >= 40) continue;

    // --- criteri per essere "scene subject" ---
    // A) pot/analogici: readmode == 1
    // B) encoder/spinner: ammessi SOLO se in "pot emulation"
    //    (nel tuo sketch: dmxtable[chan] == 2)
    //    e con mode encoder/spinner (19 / 21 / 22).
    byte isAnalogSubject = (modetable_readmode[mode] == 1);
    byte isEncSubject = 0;
    if (!isAnalogSubject) {
      if ((mode == 19 || mode == 21 || mode == 22) && (dmxtable[c] == 2)) {
        isEncSubject = 1;
      }
    }
    if (!isAnalogSubject && !isEncSubject) continue;

    // TAG via typetable: gruppo PC = 3
    // typetable è uno status MIDI “pronto”: 144 + (group*16) + channel
    byte t = typetable[c];
    if (t < 144) continue; // sicurezza

    byte group = (t - 144) / 16;
    if (group != 3) continue; // 3 = PC (che noi usiamo come "CC-scene")

    // OK: questo chan è un subject
    scene_subject_chan[scene_subject_count] = c;
    scene_subject_count++;
  }

  // ------------------------------------------------------------
  // CTRL-F: CONVOY_INIT_AFTER_SUBJECTS
  // inizializza convoy_live/out con lo stato corrente (per ora 0 o scene_final_value)
  // così snapshot/commit partono coerenti.
  // ------------------------------------------------------------
  for (byte i = 0; i < SCENE_PARAM_COUNT; i++) {
    convoy_live_value[i] = scene_final_value[i];
    convoy_out_value[i]  = scene_final_value[i];
  }
  convoy_dirty_mask = 0;

}

//-------------------------------------

void scene_control_pot()
{
  byte diff = 11;   // #mod_finestra

  // confronto come nei pot classici
  int pot_confronto = abs((lastbutton[chan] * 4) - valore);
  if (pot_confronto <= diff) return;

  // aggiorna memoria pot (0..1023 -> 0..255 circa)
  lastbutton[chan] = valore / 4;

  // clamp coerente col tuo sistema
  if (valore < 0)   valore = 0;
  if (valore > 960) valore = 960;

  // livello scena 0..127
  byte level = map32(valore, 0, 960, 0, 127);

  // quale scena controlla questo pot (da valuetable)
  byte scene_index = valuetable[chan] & 0x07;   // 0..7

  // aggiorna il livello della scena (vince l’ultimo pot mosso se duplicati)
  scene_level[scene_index] = level;

  // ------------------------------------------------------------
  // SCENE RECORD v1 (hold RECORD + move pot scena)
  // ------------------------------------------------------------
  if (scene_record_armed && !scene_record_done) {
    scene_record_done = 1;
    scene_record_execute(scene_index);
    scene_eeprom_save_all();
    scene_record_blink_tick();
  }

  // ------------------------------------------------------------
  // MAX-WINS: ricalcolo di tutti i subject (max 16) su tutte le scene (8)
  // ORA: scrive nel CONVOY (non invia diretto)
  // ------------------------------------------------------------
  for (byte i = 0; i < scene_subject_count && i < SCENE_PARAM_COUNT; i++) {

    byte c = scene_subject_chan[i];
    if (c == SCENE_CHAN_INVALID) break;

    byte final_value = 0;

    // max-wins tra tutte le scene
    for (byte s = 0; s < SCENE_COUNT; s++) {
      byte lv = scene_level[s];
      if (lv == 0) continue;

      byte target = scene_target[s][i];  // 0..127
      byte contrib = _scale127(target, lv);

      if (contrib > final_value) final_value = contrib;
    }

    // ============================================================
    // CTRL-F: MAXWINS_TO_CONVOY
    // Invece di inviare subito MIDI/DMX, aggiorniamo il convoy.
    // Il commit a fine funzione farà anti-spam + invio reale.
    // ============================================================

    // micro-anti-spam: se il valore live è già quello, evita dirty
    if (final_value == convoy_live_value[i]) continue;

    convoy_set(i, final_value);
  }

  // ============================================================
  // CTRL-F: MAXWINS_CONVOY_COMMIT
  // Un solo commit: invia davvero MIDI/DMX (anti-spam) e aggiorna scene_final_value.
  // ============================================================
  convoy_commit();
}


//********************************************************************************

void scene_record_execute(byte scene_index)
{
  // sicurezza
  if (scene_index >= SCENE_COUNT) return;

  // registriamo solo i soggetti trovati dallo scan
  for (byte i = 0; i < scene_subject_count && i < SCENE_PARAM_COUNT; i++) {

    byte c = scene_subject_chan[i];
    if (c == SCENE_CHAN_INVALID) break;

    // ------------------------------------------------------------
    // Lettura valore corrente del soggetto (0..127)
    // - POT analogici: lastbutton[] (0..255) -> >>1
    // - ENCODER (mode 19) in pot-emulation (dmxtable==2): lightable[] (0..255) -> >>1
    // - SPINNER (mode 21/22) in pot-emulation (dmxtable==2): encodervaluepot[idx] (0..1023) -> /8
    // ------------------------------------------------------------
    byte v = 0;
    byte mode = modetable[c];

    // Caso A: pot/analogico (readmode==1)
    if (mode < 40 && modetable_readmode[mode] == 1) {
      v = (byte)(lastbutton[c] >> 1);
    }
    // Caso B: encoder generic (mode 19) in pot-emulation
    else if (mode == 19 && dmxtable[c] == 2) {
      v = (byte)(lightable[c] >> 1);
    }
    // Caso C: spinner (mode 21/22) in pot-emulation
    else if ((mode == 21 || mode == 22) && dmxtable[c] == 2) {
      byte idx = SCENE_CHAN_INVALID;
      if (c == encoder_mempos[0]) idx = 0;
      else if (c == encoder_mempos[1]) idx = 1;

      if (idx != SCENE_CHAN_INVALID) {
        int vv = encodervaluepot[idx] / 8; // 0..127
        if (vv < 0) vv = 0;
        if (vv > 127) vv = 127;
        v = (byte)vv;
      }
    }

    scene_target[scene_index][i] = v;
  }
}

//**********************************************************************************



void scene_record_button()
{
  // Debounce minimale “a tempo”:
  // - legge uno stato RAW (0/1)
  // - aspetta che resti stabile per DEBOUNCE_MS
  // - poi aggiorna lo stato STABLE

  const unsigned long DEBOUNCE_MS = 25;

  // NOTA: qui assumo che la variabile globale "valore"
  // contenga lo stato del pulsante (INPUT_PULLUP tipico: 0 = premuto, 1 = rilasciato).
  // Qui raw=1 significa "premuto".
  byte raw = (valore == 0) ? 1 : 0;

  static byte prevRaw = 0;
  static byte stable  = 0;
  static unsigned long tChange = 0;

  unsigned long now = millis();

  // Se cambia il RAW, resettiamo il timer
  if (raw != prevRaw) {
    prevRaw = raw;
    tChange = now;
  }

  // Se il RAW è stabile da abbastanza tempo e differisce dallo STABLE, aggiorniamo
  if ((now - tChange) >= DEBOUNCE_MS && stable != raw) {
    stable = raw;

    if (stable) {
      // Pressione confermata: arma il record.
      scene_record_armed = 1;

      // memorizziamo quale LED è associato a questo pulsante RECORD
      // così non dobbiamo “ricercarlo” quando registriamo
   //   scene_record_blink_led = lightable[chan];
    scene_record_blink_led = 4;


convoy_dbg_dump_subjects();


   
  //****************************
/*
  Serial.println(F("---- SCENE DEBUG ----"));

Serial.print(F("page_mempos = "));
Serial.println(page_mempos);

Serial.print(F("scene_subject_count = "));
Serial.println(scene_subject_count);

Serial.print(F("scene_eeprom_is_valid = "));
Serial.println(scene_eeprom_is_valid());

Serial.print(F("scene_level[0] = "));
Serial.println(scene_level[0]);

Serial.print(F("scene_target[0][0] = "));
Serial.println(scene_target[0][0]);

Serial.print(F("scene_final_value[0] = "));
Serial.println(scene_final_value[0]);

Serial.println(F("---------------------"));

Serial.print(F("scene_eeprom_load() = "));
Serial.println(scene_eeprom_load());   // 1=caricato, 0=fallito (checksum o guardie)

Serial.print(F("stored_csum = "));
Serial.println(EEPROM.read(EEPROM_SCENE_BASE + SC_EE_CSUM));

Serial.print(F("calc_csum   = "));
Serial.println(scene_eeprom_calc_checksum());
*/
 //*******************************
      scene_record_done  = 0;   // permetti UNA registrazione durante questa pressione
    } else {
      // Rilascio confermato: disarma e prepara la prossima pressione.
      scene_record_armed = 0;
      scene_record_done  = 0;
    }
  }
}



void scene_record_blink_tick()
{
#if ( Matrix_Pads == 0 && shifter_active == 1) 
 shifter.setPin(scene_record_blink_led-1, 1); shifter.write();
 delay(50);
 shifter.setPin(scene_record_blink_led-1, 0); shifter.write();
 delay(50);
 shifter.setPin(scene_record_blink_led-1, 1); shifter.write();
 delay(50);
 shifter.setPin(scene_record_blink_led-1, 0); shifter.write();
 #endif 
}





void scene_eeprom_save_all()
{
  if (page_mempos != 0) return;

  // header
  EEPROM.update(EEPROM_SCENE_BASE + SC_EE_SIG0, 'S');
  EEPROM.update(EEPROM_SCENE_BASE + SC_EE_SIG1, 'C');
  EEPROM.update(EEPROM_SCENE_BASE + SC_EE_VER,  SCENE_EEPROM_VERSION);
  EEPROM.update(EEPROM_SCENE_BASE + SC_EE_SCN,  SCENE_COUNT);
  EEPROM.update(EEPROM_SCENE_BASE + SC_EE_PAR,  SCENE_PARAM_COUNT);
  EEPROM.update(EEPROM_SCENE_BASE + SC_EE_FLAGS, SCENE_FLAG_ENABLED);

  // dati: tutte le scene (8x16 = 128 byte)
  int addr = EEPROM_SCENE_DATA_BASE;
  for (byte s = 0; s < SCENE_COUNT; s++) {
    for (byte i = 0; i < SCENE_PARAM_COUNT; i++) {
      EEPROM.update(addr++, scene_target[s][i]);
    }
  }

  // checksum coerente con ciò che hai scritto
  EEPROM.update(EEPROM_SCENE_BASE + SC_EE_CSUM, scene_eeprom_calc_checksum());
}



byte scene_eeprom_is_valid()
{
  // Se page switch presente, per ora NON usiamo l'area 512+ (coerenza col progetto attuale)
  if (page_mempos != 0) return 0;

  if (EEPROM.read(EEPROM_SCENE_BASE + SC_EE_SIG0) != 'S') return 0;
  if (EEPROM.read(EEPROM_SCENE_BASE + SC_EE_SIG1) != 'C') return 0;

  byte ver  = EEPROM.read(EEPROM_SCENE_BASE + SC_EE_VER);
  byte scn  = EEPROM.read(EEPROM_SCENE_BASE + SC_EE_SCN);
  byte par  = EEPROM.read(EEPROM_SCENE_BASE + SC_EE_PAR);
  byte flg  = EEPROM.read(EEPROM_SCENE_BASE + SC_EE_FLAGS);

  if (ver != SCENE_EEPROM_VERSION) return 0;
  if (scn != SCENE_COUNT) return 0;
  if (par != SCENE_PARAM_COUNT) return 0;
  if ((flg & SCENE_FLAG_ENABLED) == 0) return 0;

  return 1;
}


byte scene_eeprom_load()
{
  if (!scene_eeprom_is_valid()) return 0;

  // Carica i dati scene_target[][] da EEPROM
  int addr = EEPROM_SCENE_DATA_BASE;

  for (byte s = 0; s < SCENE_COUNT; s++) {
    for (byte i = 0; i < SCENE_PARAM_COUNT; i++) {
      scene_target[s][i] = EEPROM.read(addr++);
    }
  }

  // Verifica checksum
  byte stored = EEPROM.read(EEPROM_SCENE_BASE + SC_EE_CSUM);
  byte calc   = scene_eeprom_calc_checksum();

  if (stored != calc) {
    // checksum mismatch: non usiamo dati corrotti
    // azzera tutto e segnala fallimento
    for (byte s = 0; s < SCENE_COUNT; s++) {
      for (byte i = 0; i < SCENE_PARAM_COUNT; i++) {
        scene_target[s][i] = 0;
      }
    }
    return 0;
  }

  return 1; // OK, caricato
}


// ============================================================
// ============================================================
// CTRL-F: SCENE_SNAPSHOT_SUBJECTS
// Foto "reale" dei subject (stessa identica logica del RECORD scene)
// Riempie dst[0..scene_subject_count-1] con valori 0..127.
// ============================================================
void scene_snapshot_subjects(byte* dst)
{
  for (byte i = 0; i < scene_subject_count && i < SCENE_PARAM_COUNT; i++) {

    byte c = scene_subject_chan[i];
    if (c == SCENE_CHAN_INVALID) break;

    // ------------------------------------------------------------
    // Lettura valore corrente del soggetto (0..127)
    // - POT analogici: lastbutton[] (0..255) -> >>1
    // - ENCODER (mode 19) in pot-emulation (dmxtable==2): lightable[] (0..255) -> >>1
    // - SPINNER (mode 21/22) in pot-emulation (dmxtable==2): encodervaluepot[idx] (0..1023) -> /8
    // ------------------------------------------------------------
    byte v = 0;
    byte mode = modetable[c];

    // Caso A: pot/analogico (readmode==1)
    if (mode < 40 && modetable_readmode[mode] == 1) {
      v = (byte)(lastbutton[c] >> 1);
    }
    // Caso B: encoder generic (mode 19) in pot-emulation
    else if (mode == 19 && dmxtable[c] == 2) {
      v = (byte)(lightable[c] >> 1);
    }
    // Caso C: spinner (mode 21/22) in pot-emulation
    else if ((mode == 21 || mode == 22) && dmxtable[c] == 2) {
      byte idx = SCENE_CHAN_INVALID;
      if (c == encoder_mempos[0]) idx = 0;
      else if (c == encoder_mempos[1]) idx = 1;

      if (idx != SCENE_CHAN_INVALID) {
        int vv = encodervaluepot[idx] / 8; // 0..127
        if (vv < 0) vv = 0;
        if (vv > 127) vv = 127;
        v = (byte)vv;
      }
    }
    else {
      // fallback: se non riconosciuto, usa ultimo valore finale (meglio di zero)
      v = scene_final_value[i];
    }

    dst[i] = v;
  }
}


// CTRL-F: MORPH_FUNCTIONS_BEGIN
// ============================================================

// ------------------------------------------------------------
// ENCSC entry-point: chiamata da encoder() quando dmxtable==4
// - usa valuetable[enc_chan] come target scene (NO +page)
// - usa maxvalue[enc_chan] come posizione morph 0..127 (riuso RAM)
// - quando cambia owner (o “riparte” un altro encoder), scatta la FOTO base
// ------------------------------------------------------------
    // scene_morph_encsc(numero);
void scene_morph_encsc(byte enc_chan)
{
  // determina quale dei 2 encoder fisici è (0 o 1) usando encoder_mempos[]
  byte idx = 0;
  if (enc_chan == encoder_mempos[1]) idx = 1;

  // 1) target scena deciso da valuetable (NO pageswitch)
  byte target_scene = (valuetable[enc_chan] & 0x07);

  // 2) SNAPSHOT "on start move":
  //    - se cambia owner (encoder diverso)
  //    - oppure se lo stesso encoder riparte dopo un po' di stop
  unsigned long now = millis();
  byte need_snapshot = 0;

  if (morph_owner != enc_chan) need_snapshot = 1;
  else if ((now - morph_last_ms[idx]) > MORPH_REARM_MS) need_snapshot = 1;

  // questa chiamata esiste perché c'è stato un impulso: aggiorno il tempo
  morph_last_ms[idx] = now;

  if (need_snapshot)
  {
    morph_owner  = enc_chan;
    morph_active = 1;
    morph_scene  = target_scene;

    // FOTO = stato reale attuale (include anche i subject mossi live)
    // (per ora lasciamo questa: NON snapshot da convoy, come mi hai chiesto)
   //  scene_snapshot_subjects(morph_base);
convoy_snapshot(morph_base);

    // riparti da 0 così NON ci sono scatti bruschi
    morph_pos[idx] = 0;
    morph_value    = 0;
    morph_divcnt[idx] = 0;
  }
  else
  {
    // stesso owner, movimento "continuo": posso cambiare scena target al volo
    morph_scene = target_scene;
  }

  // 3) rallenta spinner: aggiorna posizione solo ogni MORPH_PULSE_DIV impulsi
  morph_divcnt[idx]++;
  if (morph_divcnt[idx] < MORPH_PULSE_DIV) return;
  morph_divcnt[idx] = 0;

  // 4) aggiorna posizione morph (0..127) in base all’impulso encoder
  if (lastbutton[enc_chan] == 65)
  {
    if (morph_pos[idx] <= (127 - MORPH_STEP)) morph_pos[idx] += MORPH_STEP;
    else morph_pos[idx] = 127;
  }
  else if (lastbutton[enc_chan] == 63)
  {
    if (morph_pos[idx] >= MORPH_STEP) morph_pos[idx] -= MORPH_STEP;
    else morph_pos[idx] = 0;
  }

  morph_value = morph_pos[idx];

  // 5) applica morph verso la scena target
  scene_morph_apply(morph_scene, morph_value);
}

// ------------------------------------------------------------
// Applica morph: out = base + (target-base) * morph/127
// - invio MIDI e (se attivo) DMX identico al percorso MAX-WINS
// - aggiorna scene_final_value[] per restare coerente con anti-spam e con le “foto” future
// ------------------------------------------------------------

void scene_morph_apply(byte scene_index, byte morph)
{
  if (scene_index >= SCENE_COUNT) return;

  // Calcoliamo tutti i valori e li scriviamo nel convoy.
  // Sarà il convoy (commit) a inviare MIDI/DMX e a fare anti-spam.
  for (byte i = 0; i < scene_subject_count && i < SCENE_PARAM_COUNT; i++)
  {
    byte c = scene_subject_chan[i];
    if (c == SCENE_CHAN_INVALID) break;

    byte base_v   = morph_base[i];                 // foto iniziale
    byte target_v = scene_target[scene_index][i];  // scena target

    int16_t diff = (int16_t)target_v - (int16_t)base_v;

    // Avoid heavy division (/127). Keep endpoints exact.
    int16_t out;
    if (morph == 0)        out = (int16_t)base_v;
    else if (morph >= 127) out = (int16_t)target_v;
    else                   out = (int16_t)base_v + _mulShift7Round(diff, morph);
    if (out < 0) out = 0;
    if (out > 127) out = 127;

    byte final_value = (byte)out;

    // ============================================================
    // CTRL-F: MORPH_TO_CONVOY
    // Invece di inviare direttamente MIDI/DMX, scriviamo nel convoy.
    // ============================================================
    convoy_set(i, final_value);
  }

  // ============================================================
  // CTRL-F: MORPH_CONVOY_COMMIT
  // Commit: qui avviene davvero l'invio (con anti-spam).
  // ============================================================
  convoy_commit();
}

// ============================================================
// CTRL-F: MORPH_FUNCTIONS_END
// ============================================================



// CTRL-F: CONVOY_DBG_DUMP_SUBJECTS
#if SCENE_DEBUG
void convoy_dbg_dump_subjects()
{
  Serial.println(F("---- SUBJECT LIST ----"));
  Serial.print(F("scene_subject_count="));
  Serial.println(scene_subject_count);

  for (byte i = 0; i < scene_subject_count && i < SCENE_PARAM_COUNT; i++) {
    Serial.print(F("i=")); Serial.print(i);
    Serial.print(F(" chan=")); Serial.println(scene_subject_chan[i]);
  }
  Serial.println(F("----------------------"));
}
#else
static inline void convoy_dbg_dump_subjects() {}
#endif
