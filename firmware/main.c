// Copyright 2015 Mike Tsao
//
// weblight

#include <avr/interrupt.h>  // for sei()
#include <avr/io.h>
#include <avr/pgmspace.h>  // required by usbdrv.h
#include <avr/wdt.h>
#include <util/delay.h>  // for _delay_ms()

#include "eeprom.h"
#include "led_control.h"
#include "requests.h"  // The custom request numbers we use
#include "usbconfig.h"
#include "usbdrv.h"
#include "webusb.h"

#define TRUE (1==1)
#define FALSE (!TRUE)

enum {
  // Everything we need to start up the microcontroller.
  STATE_MCU_INIT = 0,

  // Disconnect USB to force device re-enumeration (see discussion below).
  STATE_DISCONNECT_USB,

  // Display lights to let the developer know that the hardware is
  // probably working. Also serves as a 250ms+ delay for re-enumeration.
  STATE_STARTUP_SEQUENCE,

  // Reconnect USB for device re-enumeration.
  STATE_CONNECT_USB,

  // Normal loop after all initialization is complete.
  STATE_READY
};

static uint8_t state;

void initReady() {
  state = STATE_READY;
}

void doGratuitousBlinking() {
  static uchar blinkie = 0;
  if (blinkie) {
    SetLEDs(4, 0, 0);
  } else {
    SetLEDs(0, 0, 4);
  }
  blinkie = !blinkie;
}

void doAnimation() {
#if BOARD_VARIANT == BV_APA102
  // This is where we do any animation of the lights that isn't
  // initiated by a direct host request.
  if (FALSE) {
    doGratuitousBlinking();
  }
#endif
}

void doReady() {
  usbPoll();
  UpdateLEDs();

  // If Timer 1 matched the output-compare register A, then clear it
  // and do an animation frame.
  if (TIFR & _BV(OCF1A)) {
    TIFR |= _BV(OCF1A);
    TCNT1 = 0;
    doAnimation();
  }
}

void initConnectUSB() {
  state = STATE_CONNECT_USB;
}

void doConnectUSB() {
  sei();
  usbDeviceConnect();
  initReady();
}

static uint8_t startup_color;
static uint8_t remaining_disconnect_delay_ms;
void initStartupSequence() {
  state = STATE_STARTUP_SEQUENCE;
  startup_color = 0;
  remaining_disconnect_delay_ms = 250;
}

void doStartupSequence() {
#define STARTUP_BRIGHTNESS (16)
#define STARTUP_FRAME_LENGTH_MS (40)
  uint8_t r = 0, g = 0, b = 0, done = 0;
  switch (startup_color++) {
    case 0:
      b = STARTUP_BRIGHTNESS;
      break;
    case 1:
      r = STARTUP_BRIGHTNESS;
      break;
    case 2:
      r = STARTUP_BRIGHTNESS;
      g = STARTUP_BRIGHTNESS;
      break;
    case 3:
      b = STARTUP_BRIGHTNESS;
      break;
    case 4:
      g = STARTUP_BRIGHTNESS;
      break;
    case 5:
      r = STARTUP_BRIGHTNESS;
      break;
    default:
      done = 1;
      break;
  }
  SetLEDs(r, g, b);
  UpdateLEDs();

  _delay_ms(STARTUP_FRAME_LENGTH_MS);
  if (remaining_disconnect_delay_ms >= STARTUP_FRAME_LENGTH_MS) {
    remaining_disconnect_delay_ms -= STARTUP_FRAME_LENGTH_MS;
  }

  if (done) {
    LEDsOff();
    while (remaining_disconnect_delay_ms-- > 0) {
      _delay_ms(1);
    }
    initConnectUSB();
  }
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

  // Now let the startup sequence run, which also serves as a delay to
  // ensure the host notices that the device was unplugged.
  initStartupSequence();
}

void initMCUInit() {
  state = STATE_MCU_INIT;
}

void doMCUInit() {
  // ReadEEPROM() turns off the LEDs, but let's be sure because that's
  // an undocumented side effect.
  LEDsOff();

  // Set the timer we use for our animation frames: CK / 16384
  // CTC compare value: 16500000 / 16384 / 32 = match at ~32Hz
  // 2^14 = 16384; 2^5 = 32
  TCCR1 = _BV(CS13) | _BV(CS12) | _BV(CS11) | _BV(CS10);
  OCR1A = (F_CPU >> (14 + 5));

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
    case STATE_STARTUP_SEQUENCE:
      doStartupSequence();
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
