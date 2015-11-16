#include "led_control.h"
#if BOARD_VARIANT == BV_WS2812
#include "light_ws2812.h"
#else
#include "light_apa102.h"
#endif
#include <util/delay.h>  // for _delay_ms()

struct cRGB led[LED_COUNT];

uint8_t GetLEDCount() {
  return LED_COUNT;
}

static unsigned char leds_need_update = 0;
void UpdateLEDs() {
  if (leds_need_update) {
    leds_need_update = 0;
#if BOARD_VARIANT == BV_WS2812
    ws2812_setleds(led, LED_COUNT);
#else
    apa102_setleds(led, LED_COUNT);
#endif
  }
}

void SetLED(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
  led[i].r = r;
  led[i].g = g;
  led[i].b = b;
  leds_need_update = 1;
}

void SetLEDs(uint8_t r, uint8_t g, uint8_t b) {
  uint8_t i;
  for (i = 0; i < LED_COUNT; ++i) {
    SetLED(i, r, g, b);
  }
}

void LEDsOff() {
  uint8_t i;
  for (i = 0; i < LED_COUNT; ++i) {
    SetLED(i, 0, 0, 0);
  }
  UpdateLEDs();
}

// Note that this is an invasive method for gross debugging only. It
// will interfere with V-USB interrupts.
//
// TODO(miket): make it more polite.
void StatusBlink(uint8_t count) {
  uint8_t i;
  LEDsOff();
  for (i = 0; i < count; i++) {
    SetLED(0, 0, 0, 32);
    UpdateLEDs();
    _delay_ms(10);
    SetLED(0, 0, 0, 0);
    UpdateLEDs();
    _delay_ms(30);
  }
}
