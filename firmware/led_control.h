#include <avr/io.h>

#define MAX_LED_COUNT (4)

uint8_t GetLEDCount();
void SetLEDCount(uint8_t c);
void SetLED(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
void SetLEDs(uint8_t r, uint8_t g, uint8_t b);
void LEDsOff();

void StatusBlink(uint8_t count);

// None of the other functions talk over the wire. This one does.
void UpdateLEDs();
