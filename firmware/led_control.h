// Copyright 2015 Mike Tsao
//
// WebLight firmware
// https://github.com/sowbug/weblight

#if !defined(__LED_CONTROL_H__)
#define __LED_CONTROL_H__

#include <avr/io.h>

typedef enum {
  AD_HOC,
  SEQUENCER,
  CANDLE,
  WATCHDOG_EXPIRATION
} ProgramMode;

ProgramMode GetProgramMode();
void SetProgramMode(ProgramMode mode);

uint8_t GetLEDCount();
void GetLED(uint8_t i, uint8_t *r, uint8_t *g, uint8_t *b);
void SetLED(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
void SetLEDs(uint8_t r, uint8_t g, uint8_t b);
void LEDsOff();

void StatusBlink(uint8_t count);
void StatusBlinkRed(uint8_t count);

void ResetAppWatchdog(int16_t new_msec);
uint8_t CountDownAppWatchdog(uint16_t msec_elapsed);

// None of the other functions talk over the wire. This one does.
void UpdateLEDs();

#endif  // #if !defined(__LED_CONTROL_H__)
