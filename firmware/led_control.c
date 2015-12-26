// Copyright 2015 Mike Tsao
//
// WebLight firmware
// https://github.com/sowbug/weblight

#include "led_control.h"
#include "light_apa102.h"
#include <util/delay.h>  // for _delay_ms()

static struct cRGB led[LED_COUNT];

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

static ProgramMode program_mode = AD_HOC;
ProgramMode GetProgramMode() { return program_mode; }
void SetProgramMode(ProgramMode mode) { program_mode = mode; }
