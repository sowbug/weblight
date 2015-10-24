#include <avr/io.h>

#define LED_COUNT (2)  // TODO: 2 takes too long and screws up the USB

void SetLED(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
void SetLEDs(uint8_t r, uint8_t g, uint8_t b);
void LEDsOff();

// None of the other functions talk over the wire. This one does.
void UpdateLEDs();
