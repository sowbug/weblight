// Copyright 2015 Mike Tsao
//
// weblight

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  // for sei()
#include <util/delay.h>     // for _delay_ms()

#include <avr/pgmspace.h>   // required by usbdrv.h
#include "usbconfig.h"
#include "usbdrv.h"
#include "requests.h"       // The custom request numbers we use

#include "light_ws2812.h"

#define TRUE (1==1)
#define FALSE (!TRUE)

#define LED_COUNT (2)

struct cRGB led[LED_COUNT];

void SetLED(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
  led[i].r = r;
  led[i].g = g;
  led[i].b = b;
  ws2812_setleds(led, LED_COUNT);
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

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
  usbRequest_t    *rq = (void *)data;
  // buffer must stay valid when usbFunctionSetup returns
  static uchar    dataBuffer[4];

  if (rq->bRequest == CUSTOM_RQ_ECHO) { // echo -- used for reliability tests
    dataBuffer[0] = rq->wValue.bytes[0];
    dataBuffer[1] = rq->wValue.bytes[1];
    usbMsgPtr = (int)dataBuffer;
    return 2;
  } else if (rq->bRequest == CUSTOM_RQ_SET_STATUS) {
    if (rq->wValue.bytes[0] & 1) {  // set LED
      SetLEDs(255, 0, 0);
      _delay_ms(100);
      SetLEDs(0, 255, 0);
      _delay_ms(100);
      SetLEDs(0, 0, 255);
      _delay_ms(100);
    } else {  // clear LED
      SetLEDs(0, 0, 0);
    }
    return 0;
  }

  // default for not implemented requests: return no data back to host
  return 0;
}

// Thanks http://codeandlife.com/2012/02/22/v-usb-with-attiny45-attiny85-without-a-crystal/
#define abs(x) ((x) > 0 ? (x) : (-x))
void calibrateOscillator(void) {
  int frameLength, targetLength =
      (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);
  int bestDeviation = 9999;
  uchar trialCal, bestCal, step, region;

  // do a binary search in regions 0-127 and 128-255 to get optimum OSCCAL
  for (region = 0; region <= 1; region++) {
    frameLength = 0;
    trialCal = (region == 0) ? 0 : 128;

    for (step = 64; step > 0; step >>= 1) {
      if (frameLength < targetLength) // true for initial iteration
        trialCal += step; // frequency too low
      else
        trialCal -= step; // frequency too high

      OSCCAL = trialCal;
      frameLength = usbMeasureFrameLength();

      if (abs(frameLength-targetLength) < bestDeviation) {
        bestCal = trialCal; // new optimum found
        bestDeviation = abs(frameLength -targetLength);
      }
    }
  }

  OSCCAL = bestCal;

  // Flash LEDs to let everyone know the calibration happened
  SetLEDs(255, 0, 0);
  _delay_ms(10);
  SetLEDs(0, 0, 0);
}

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

#define BREATH_MIN (1 << 9)
#define BREATH_MAX (1 << 14)
#define BREATH_SHIFT (8)

// TODO: if memory gets tight, many of these can be collapsed into a
// union.
uint8_t state;
uint16_t state_counter;
uint8_t active_led;
int8_t state_direction;
uint8_t r, g, b;

void initReady() {
  state = STATE_READY;
  state_counter = BREATH_MIN + 1;
  state_direction = 1;
}

void doReady() {
  usbPoll();
  if (state_direction == 1) {
    ++state_counter;
  } else {
    --state_counter;
  }
  if (state_counter <= BREATH_MIN || state_counter >= BREATH_MAX) {
    state_direction *= -1;
  }
  SetLEDs(0, state_counter >> BREATH_SHIFT, 0);
}

void initConnectUSB() {
  state = STATE_CONNECT_USB;
}

void doConnectUSB() {
  sei();
  usbDeviceConnect();
  initReady();
}

void initStartupSequence() {
  state = STATE_STARTUP_SEQUENCE;
  r = 255; g = 0; b = 0; active_led = 0;
}

void doStartupSequence() {
  // Thanks http://codepen.io/Codepixl/pen/ogWWaK/
  if (r > 0 && b == 0) {
    r--;
    g++;
  }
  if (g > 0 && r == 0) {
    g--;
    b++;
  }
  if (b > 0 && g == 0) {
    r++;
    b--;
  }
  SetLED(active_led, r >> 3, g >> 3, b >> 3);
  _delay_ms(1);
  if (r == 255 && b == 0 && g == 0) {
    LEDsOff();
    if (active_led++ == LED_COUNT) {
      initConnectUSB();
    }
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
  // select clock: 16.5M/1k ->
  // overflow rate = 16.5M/256k = 62.94 Hz
  TCCR1 = 0x0b;

  // RESET status: all port bits are inputs without pull-up. That's
  // the way we need D+ and D-. Therefore we don't need any additional
  // hardware initialization.

  // Even if you don't use the watchdog, turn it off here. On newer devices,
  // the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
  wdt_enable(WDTO_1S);

  LEDsOff();

  usbInit();
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
