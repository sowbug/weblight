#if !defined(__LED_CONTROL_H__)
#define __LED_CONTROL_H__

#include <avr/io.h>

typedef enum {
  AD_HOC,
  SEQUENCER,
  CANDLE
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

// None of the other functions talk over the wire. This one does.
void UpdateLEDs();

#endif  // #if !defined(__LED_CONTROL_H__)
