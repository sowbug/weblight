// Copyright 2015 Mike Tsao
//
// WebLight firmware
// https://github.com/sowbug/weblight

#include "app.h"

#include "candle.h"
#include "eeprom.h"
#include "led_control.h"
#include "requests.h"  // The custom request numbers we use
#include "sequencer.h"
#include "webusb.h"

#define TICKS_PER_SECOND (60)
#define CLOCK_DIVISOR (8192)

void AppInit(App *ctx) {
  LEDsOff();

  // Set the timer we use for our animation frames: CK / 8192 (CLOCK_DIVISOR)
  // CTC compare value: 16500000 / CLOCK_DIVISOR / 60 = match at ~60Hz
  //
  // So each match happens about 60x/second (actually about 61.03x)
  TCCR1 = _BV(CS13) | _BV(CS12) | _BV(CS11);
  OCR1A = (F_CPU / CLOCK_DIVISOR) / TICKS_PER_SECOND;

  ReadEEPROM();
  // Start up the sequencer.
  Load();
  Play();
}

void AppRun(App *ctx) {
  // If Timer 1 matched the output-compare register A, then clear it
  // and do an animation frame.
  if (TIFR & _BV(OCF1A)) {
    TIFR |= _BV(OCF1A);
    uint16_t msec_elapsed = TCNT1 * CLOCK_DIVISOR / 1000;
    TCNT1 = 0;

    switch (GetProgramMode()) {
    case AD_HOC:
    case SEQUENCER:
      // Sequencer needs to run during AD_HOC because of transitions.
      Run(msec_elapsed);
      break;
    case CANDLE:
      CandleRun(msec_elapsed);
      break;
    default:
      break;
    }
  }
  UpdateLEDs();
}
