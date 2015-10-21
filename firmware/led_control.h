#include <avr/io.h>

#define LED_COUNT (1)  // TODO: 2 takes too long and screws up the USB

void SetLED(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
void SetLEDs(uint8_t r, uint8_t g, uint8_t b);
void LEDsOff();
