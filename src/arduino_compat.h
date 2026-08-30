/*
 * hebcal-c - Arduino / embedded compatibility layer.
 *
 * Provides PROGMEM / pgm_read_* macros that are no-ops on desktop (gcc/clang)
 * and ESP32, but map to AVR flash macros when compiling for AVR.  This lets
 * the same source compile for all three targets.
 *
 * License: GPL-2.0-or-later
 */
#ifndef ARDUINO_COMPAT_H
#define ARDUINO_COMPAT_H

/* --- AVR (Arduino Uno/Nano, ATmega328P etc.) ----------------------------- */
#if defined(__AVR__)
#include <avr/pgmspace.h>
/* PROGMEM and pgm_read_* are provided by avr/pgmspace.h. */

/* --- ESP32 / ESP8266 / desktop ------------------------------------------- */
#else
/* On these platforms flash and RAM share an address space, so PROGMEM is
 * a no-op and pgm_read is a plain dereference. */
#define PROGMEM
#define pgm_read_byte_near(addr)  (*(const uint8_t *)(addr))
#define pgm_read_word_near(addr)  (*(const uint16_t *)(addr))
#define pgm_read_dword_near(addr) (*(const uint32_t *)(addr))
#define pgm_read_ptr_near(addr)   (*(const void *const *)(addr))

/* Helper used widely in the string tables. */
#define PSTR(s)   (s)
#define F(s)      (s)
#endif

/* --- Shared helpers ------------------------------------------------------- */

/* Read a PROGMEM string pointer (char*) stored in a PROGMEM table of
 * pointers.  Works on all platforms. */
static inline const char *heb_pgm_read_str(const char *const *addr) {
#if defined(__AVR__)
    return (const char *)pgm_read_ptr_near(addr);
#else
    return *addr;
#endif
}

#endif /* ARDUINO_COMPAT_H */
