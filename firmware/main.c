// Copyright 2015 Mike Tsao
//
// weblight

#include <avr/interrupt.h>  // for sei()
#include <avr/io.h>
#include <avr/pgmspace.h>  // required by usbdrv.h
#include <avr/wdt.h>
#include <util/delay.h>  // for _delay_ms()

#include "candle.h"
#include "eeprom.h"
#include "led_control.h"
#include "requests.h"  // The custom request numbers we use
#include "sequencer.h"
#include "usbconfig.h"
#include "usbdrv.h"
#include "webusb.h"

#define TRUE (1==1)
#define FALSE (!TRUE)

#define TICKS_PER_SECOND (60)
#define CLOCK_DIVISOR (8192)

enum {
  // Everything we need to start up the microcontroller.
  STATE_MCU_INIT = 0,

  // Disconnect USB to force device re-enumeration (see discussion below).
  STATE_DISCONNECT_USB,

  // Reconnect USB for device re-enumeration.
  STATE_CONNECT_USB,

  // Normal loop after all initialization is complete.
  STATE_READY
};

static uint8_t state;

void initReady() {
  state = STATE_READY;

  Load();
  Play();
}

void doReady() {
  usbPoll();

  // If Timer 1 matched the output-compare register A, then clear it
  // and do an animation frame.
  if (TIFR & _BV(OCF1A)) {
    TIFR |= _BV(OCF1A);
    uint16_t msec_elapsed = TCNT1 * CLOCK_DIVISOR / 1000;
    TCNT1 = 0;

    switch (GetProgramMode()) {
    case AD_HOC:
      break;
    case SEQUENCER:
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

void initConnectUSB() {
  state = STATE_CONNECT_USB;
}

void doConnectUSB() {
  sei();
  usbDeviceConnect();
  initReady();
}

void initDisconnectUSB() {
  state = STATE_DISCONNECT_USB;
}

void doDisconnectUSB() {
  // From http://vusb.wikidot.com/examples:
  //
  // Enumeration is performed only when the device is connected to the
  // bus. If the device has a CPU reset, its memory is usually cleared
  // and the assigned address is lost. Since the host does not know
  // that the device had a reset, it still addresses the device under
  // its old address, but the device won't answer.

  // It is therefore useful to simulate a device disconnect in the
  // device initialization code. This ensures that host and device
  // agree on the same address. You should therefore insert
  // [disconnect/reconnect] code in your initialization (best before
  // usbInit() because the USB interrupt must be disabled during
  // disconnect state):
  usbDeviceDisconnect();

  // Now delay to ensure the host notices that the device was
  // unplugged.
  uint8_t disconnect_delay_ms = 250;
  while (disconnect_delay_ms-- > 0) {
    _delay_ms(1);
  }

  // And reconnect USB.
  initConnectUSB();
}

void initMCUInit() {
  state = STATE_MCU_INIT;
}

void doMCUInit() {
  // ReadEEPROM() turns off the LEDs, but let's be sure because that's
  // an undocumented side effect.
  LEDsOff();

  // Set the timer we use for our animation frames: CK / 8192 (CLOCK_DIVISOR)
  // CTC compare value: 16500000 / CLOCK_DIVISOR / 60 = match at ~60Hz
  //
  // So each match happens about 60x/second (actually about 61.03x)
  TCCR1 = _BV(CS13) | _BV(CS12) | _BV(CS11);
  OCR1A = (F_CPU / CLOCK_DIVISOR) / TICKS_PER_SECOND;

  // If this fires, it will (probably) cause the startup sequence to
  // repeat, which will give us an indication that something's wrong.
  wdt_enable(WDTO_1S);

  ReadEEPROM();

  usbInit();

  // We're done with this state. Go to next.
  initDisconnectUSB();
}

int __attribute__((noreturn)) main(void) {
  initMCUInit();

  while (TRUE) {
    switch (state) {
    case STATE_MCU_INIT:
      doMCUInit();
      break;
    case STATE_DISCONNECT_USB:
      doDisconnectUSB();
      break;
    case STATE_CONNECT_USB:
      doConnectUSB();
      break;
    case STATE_READY:
      doReady();
      break;
    }
    wdt_reset();
  }
}
