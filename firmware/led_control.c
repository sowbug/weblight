#include "led_control.h"
#include "light_ws2812.h"

struct cRGB led[LED_COUNT];

static unsigned char leds_need_update = 0;
void UpdateLEDs() {
  if (leds_need_update) {
    leds_need_update = 0;
    ws2812_setleds(led, LED_COUNT);
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
}
