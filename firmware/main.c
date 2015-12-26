// Copyright 2015 Mike Tsao
//
// WebLight firmware
// https://github.com/sowbug/weblight

#include <avr/interrupt.h>  // for sei()
#include <avr/wdt.h>
#include <util/delay.h>  // for _delay_ms()

#include "app.h"
#include "usbdrv.h"

int __attribute__((noreturn)) main(void) {
  wdt_enable(WDTO_1S);
  usbInit();

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

  sei();
  usbDeviceConnect();

  App app;
  AppInit(&app);

  while (1) {
    usbPoll();
    AppRun(&app);
    wdt_reset();
  }
}
