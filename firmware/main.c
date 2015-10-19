// Copyright 2015 Mike Tsao
//
// weblight

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  // for sei()
#include <util/delay.h>     // for _delay_ms()

#include <avr/pgmspace.h>   // required by usbdrv.h
#include "usbdrv.h"
#include "requests.h"       // The custom request numbers we use

#include "light_ws2812.h"

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
  static uchar    dataBuffer[4];  // buffer must stay valid when
                                  // usbFunctionSetup returns

  if (rq->bRequest == CUSTOM_RQ_ECHO){ // echo -- used for reliability tests
    dataBuffer[0] = rq->wValue.bytes[0];
    dataBuffer[1] = rq->wValue.bytes[1];
    dataBuffer[2] = rq->wIndex.bytes[0];
    dataBuffer[3] = rq->wIndex.bytes[1];
    usbMsgPtr = (int)dataBuffer;
    return sizeof(dataBuffer);
  } else if (rq->bRequest == CUSTOM_RQ_SET_STATUS) {
    if (rq->wValue.bytes[0] & 1) { // set LED
      SetLEDs(255, 0, 0);
      _delay_ms(100);
      SetLEDs(0, 255, 0);
      _delay_ms(100);
      SetLEDs(0, 0, 255);
      _delay_ms(100);
    } else {  // clear LED
      SetLEDs(0, 0, 0);
    }
  } else if (rq->bRequest == CUSTOM_RQ_GET_STATUS) {
    dataBuffer[0] = 0x42;  // TMP
    usbMsgPtr = (int)dataBuffer;
    return 1;  // tell the driver to send 1 byte
  }
  return 0;  // default for not implemented requests:
             // return no data back to host
}

enum {
  STATE_USB_INIT = 0,
  STATE_FAKE_DISCONNECT,
  STATE_STARTUP_SEQUENCE,
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
    if (active_led == 0) {
      LEDsOff();
      active_led = 1;
    } else {
      LEDsOff();
      initReady();
    }
  }
}

void initFakeDisconnect() {
  state = STATE_FAKE_DISCONNECT;
  state_counter = 255;
}

void doFakeDisconnect() {
  if (state_counter-- > 0) {
    _delay_ms(1);
  } else {
    sei();
    usbDeviceConnect();
    initStartupSequence();
  }
}

void initUSBInit() {
  state = STATE_USB_INIT;
}

void doUSBInit() {
  // RESET status: all port bits are inputs without pull-up. That's
  // the way we need D+ and D-. Therefore we don't need any additional
  // hardware initialization.
  usbInit();
  usbDeviceDisconnect();  // enforce re-enumeration, do this while
                          // interrupts are disabled!
  initFakeDisconnect();
}

int __attribute__((noreturn)) main(void) {
  // Even if you don't use the watchdog, turn it off here. On newer devices,
  // the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
  wdt_enable(WDTO_1S);

  LEDsOff();

  initUSBInit();

  while (1 == 1) {
    switch (state) {
    case STATE_USB_INIT:
      doUSBInit();
      break;
    case STATE_FAKE_DISCONNECT:
      doFakeDisconnect();
      break;
    case STATE_STARTUP_SEQUENCE:
      doStartupSequence();
      break;
    case STATE_READY:
      doReady();
      break;
    }
    wdt_reset();
  }
}
