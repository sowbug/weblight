#include "led_control.h"
#include "light_ws2812.h"
#include <util/delay.h>     // for _delay_ms()

struct cRGB led[MAX_LED_COUNT];
static uint8_t led_count = 0;

uint8_t GetLEDCount() {
  return led_count;
}

void SetLEDCount(uint8_t c) {
  if (c < 1) {
    c = 1;
  }
  if (c > MAX_LED_COUNT) {
    c = MAX_LED_COUNT;
  }
  led_count = c;
}

static unsigned char leds_need_update = 0;
void UpdateLEDs() {
  if (leds_need_update) {
    leds_need_update = 0;
    ws2812_setleds(led, led_count);
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
  for (i = 0; i < led_count; ++i) {
    SetLED(i, r, g, b);
  }
}

void LEDsOff() {
  uint8_t i;
  for (i = 0; i < MAX_LED_COUNT; ++i) {
    SetLED(i, 0, 0, 0);
  }
  UpdateLEDs();
}

void StatusBlink(uint8_t count) {
  uint8_t i;
  for (i = 0; i < count; i++) {
    SetLED(0, 0, 0, 32);
    UpdateLEDs();
    _delay_ms(10);
    SetLED(0, 0, 0, 0);
    UpdateLEDs();
    _delay_ms(50);
  }
}
