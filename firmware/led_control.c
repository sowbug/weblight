#include "led_control.h"
#include "light_apa102.h"
#include <util/delay.h>  // for _delay_ms()

struct cRGB led[LED_COUNT];

uint8_t GetLEDCount() {
  return LED_COUNT;
}

static unsigned char leds_need_update = 0;
void UpdateLEDs() {
  if (leds_need_update) {
    leds_need_update = 0;
    apa102_setleds(led, LED_COUNT);
  }
}

void GetLED(uint8_t i, uint8_t *r, uint8_t *g, uint8_t *b) {
  *r = led[i].r;
  *g = led[i].g;
  *b = led[i].b;
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

void SetLEDsToRgbColor(RgbColor rgb) {
  SetLEDs(rgb.r, rgb.g, rgb.b);
}

void LEDsOff() {
  uint8_t i;
  for (i = 0; i < LED_COUNT; ++i) {
    SetLED(i, 0, 0, 0);
  }
  UpdateLEDs();
}

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

void StatusBlinkRed(uint8_t count) {
  uint8_t i;
  LEDsOff();
  for (i = 0; i < count; i++) {
    SetLED(0, 128, 0, 0);
    UpdateLEDs();
    _delay_ms(10);
    SetLED(0, 0, 0, 0);
    UpdateLEDs();
    _delay_ms(30);
  }
}

// Thanks http://stackoverflow.com/a/22120275/344467
RgbColor HsvToRgb(HsvColor hsv) {
  RgbColor rgb;
  unsigned char region, p, q, t;
  unsigned int h, s, v, remainder;

  if (hsv.s == 0) {
    rgb.r = hsv.v;
    rgb.g = hsv.v;
    rgb.b = hsv.v;
    return rgb;
  }

  // converting to 16 bit to prevent overflow
  h = hsv.h;
  s = hsv.s;
  v = hsv.v;

  region = h / 43;
  remainder = (h - (region * 43)) * 6;

  p = (v * (255 - s)) >> 8;
  q = (v * (255 - ((s * remainder) >> 8))) >> 8;
  t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

  switch (region) {
  case 0:
    rgb.r = v;
    rgb.g = t;
    rgb.b = p;
    break;
  case 1:
    rgb.r = q;
    rgb.g = v;
    rgb.b = p;
    break;
  case 2:
    rgb.r = p;
    rgb.g = v;
    rgb.b = t;
    break;
  case 3:
    rgb.r = p;
    rgb.g = q;
    rgb.b = v;
    break;
  case 4:
    rgb.r = t;
    rgb.g = p;
    rgb.b = v;
    break;
  default:
    rgb.r = v;
    rgb.g = p;
    rgb.b = q;
    break;
  }

  return rgb;
}

HsvColor RgbToHsv(RgbColor rgb) {
  HsvColor hsv;
  unsigned char rgbMin, rgbMax;

  rgbMin = rgb.r < rgb.g ?
    (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
  rgbMax = rgb.r > rgb.g ?
    (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

  hsv.v = rgbMax;
  if (hsv.v == 0) {
      hsv.h = 0;
      hsv.s = 0;
      return hsv;
    }

  hsv.s = 255 * ((long)(rgbMax - rgbMin)) / hsv.v;
  if (hsv.s == 0) {
      hsv.h = 0;
      return hsv;
    }

  if (rgbMax == rgb.r)
    hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
  else if (rgbMax == rgb.g)
    hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
  else
    hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

  return hsv;
}

static ProgramMode program_mode = AD_HOC;
ProgramMode GetProgramMode() { return program_mode; }
void SetProgramMode(ProgramMode mode) { program_mode = mode; }
