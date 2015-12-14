The device's VID/PID pair is 1209/a800. All communications with the
device, aside from basic USB communication like the device-descriptor
request, are by control transfers.

0: WL_REQUEST_ECHO
---

Writes back to the host the four bytes in wValue and wIndex. Used to
test that the host and device can communicate reliably.

1: WL_REQUEST_SET_RGB
---

Sets the RGB color of all LEDs. Three bytes sent as OUT data.

80 00 00 will set the LEDs to medium-brightness red.

00 00 80 will set the LEDs to medium-brightness blue.

2: WL_REQUEST_SET_LED_COUNT
---

Sets the number of active LEDs. Pass the number in wValue. The value
is written to EEPROM. This request exists to allow the same firmware
image to work on different versions of the hardware. It should be
correctly set at the factory. There should be no need for application
developers to use this request. TODO: deprecate or at least move out
of the low request enum area where developers won't use it by
accident.

252: WL_REQUEST_WINUSB
---

Requests WinUSB descriptors.

253: WL_REQUEST_RESET_DEVICE
---

Resets the device and puts it into bootloader mode.

254: WL_REQUEST_WEBUSB
---

Requests WebUSB descriptors.

Ideas
---

* Set duty cycle: % of time lights should be on, and duration of cycle

* SET_HSV: hue/saturation/value as alternative to RGB

* Set sequence: each byte is an index into palette? Speed?

* Persistent: sequence to appear on startup, on heartbeat failure, or
  just on demand.
