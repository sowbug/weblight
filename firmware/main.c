// Copyright 2015 Mike Tsao
//
// weblight

#include <avr/interrupt.h>  // for sei()
#include <avr/io.h>
#include <avr/pgmspace.h>   // required by usbdrv.h
#include <avr/wdt.h>
#include <util/delay.h>     // for _delay_ms()

#include "eeprom.h"
#include "led_control.h"
#include "requests.h"       // The custom request numbers we use
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

// TODO: if memory gets tight, many of these can be collapsed into a
// union.
static uint8_t state;
static uint8_t active_led;
static uint8_t r, g, b;

void initReady() {
  state = STATE_READY;
}

void doGratuitousBlinking() {
  static uchar blinkie = 0;
  if (blinkie) {
    SetLED(0, 4, 0, 0);
  } else {
    SetLED(0, 0, 0, 4);
  }
  blinkie = !blinkie;
}

void doAnimation() {
  if (0) {
    // This is where we'd do any animation of the lights that isn't
    // initiated by a direct host request.
    doGratuitousBlinking();
  }

  UpdateLEDs();
}

static uchar hostIsAsleep = 0;  // TODO: set this appropriately.
static uchar readyForUpdate = 0;
static uchar lastUsbSofCount = 0;
void doReady() {
  usbPoll();

  // If Timer 1 matched the output-compare register A, then clear it
  // and mark that it's time for an animation frame.
  if (TIFR & _BV(OCF1A)) {
    TIFR |= _BV(OCF1A);
    readyForUpdate = 1;
  }
  if (readyForUpdate) {
    if (hostIsAsleep) {
      doAnimation();
      readyForUpdate = 0;
    } else {
      if (usbSofCount != lastUsbSofCount) {
        doAnimation();
        readyForUpdate = 0;
      }
    }
  }
  // No matter whether we ran an animation frame, we want to reset the
  // indicator that we just got a frame from the host, because our
  // goal is to run animation frames only right after a host frame.
  lastUsbSofCount = usbSofCount;
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
  UpdateLEDs();
  _delay_ms(1);
  if (r == 255 && b == 0 && g == 0) {
    LEDsOff();
    if (active_led++ == GetLEDCount()) {
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
  // Clear on compare match
  // CK / 16384
  TCCR1 = _BV(CTC1) |
    _BV(CS13) | _BV(CS12) | _BV(CS11) | _BV(CS10);

  // CTC compare value: 16500000 / 16384 / 32 = match at ~32Hz
  //
  // TODO: for some reason this is looking more like 3Hz. Am I
  // misreading the datasheet?
  OCR1A = F_CPU >> (14 + 5);  // 2^14 = 16384; 2^5 = 32

  // Even if you don't use the watchdog, turn it off here. On newer
  // devices, the status of the watchdog (on/off, period) is PRESERVED
  // OVER RESET!
  wdt_enable(WDTO_1S);

  // Side effect: turns off LEDs
  ReadEEPROM();

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
