// Copyright 2015 Mike Tsao
//
// WebLight firmware
// https://github.com/sowbug/weblight

// This header is (ideally) shared between the firmware and the host
// software. It defines the USB request numbers (and optionally data
// types) used to communicate between the host and the device.

#ifndef __REQUESTS_H_INCLUDED__
#define __REQUESTS_H_INCLUDED__

// Requests that the device sends back wValue and wIndex. This is used
// with random data to test the reliability of the communication.
#define WL_REQUEST_ECHO (0)

// Sets the RGB color of all LEDs. Control-OUT.
//
// uint8 red
// uint8 green
// uint8 blue
#define WL_REQUEST_COLOR (1)

// During a recorded sequence, pauses for the given number of
// milliseconds. Control-OUT.
//
// uint16 duration_msec
#define WL_REQUEST_PAUSE (2)

// Sets the transition for future COLOR requests. Control-OUT.
//
// uint8 transition_type
// uint16 duration_msec
#define WL_REQUEST_TRANSITION (3)

// During a recorded sequence, pauses forever. Stops loops. Control-OUT.
//
#define WL_REQUEST_HALT (4)

// Starts recording a sequence. Control-OUT.
//
#define WL_REQUEST_RECORD (5)

// Plays a sequence. Control-OUT.
//
#define WL_REQUEST_PLAY (6)

// Stops playing or recording a sequence. Control-OUT.
//
#define WL_REQUEST_STOP (7)

// Saves the current sequence to EEPROM. Control-OUT.
//
#define WL_REQUEST_SAVE (8)

// Replaces the current sequence with contents from
// EEPROM. Control-OUT.
//
#define WL_REQUEST_LOAD (9)

// Plays an effect. Control-OUT.
//
// Send the effect number in wValue.
//
// 0: candle
#define WL_REQUEST_EFFECT (10)

// Resets the watchdog, activating it if not already. Control-OUT.
//
// Send the number of seconds in wValue. If the watchdog isn't reset
// again within that number of seconds, the device switches to a
// visual alert.
#define WL_REQUEST_RESET_WATCHDOG (11)

// Sets the WebUSB landing page and allowed origins, all as a set of
// concatenated descriptors. Control-OUT.
//
#define WL_REQUEST_SET_WEBUSB_URLS (251)

// WinUSB: 252
//
// WinUSB descriptor.
#define WL_REQUEST_WINUSB    (252)

// Reset: 253
//
// Reset the device, which triggers the bootloader.
#define WL_REQUEST_RESET_DEVICE    (253)

// WebUSB: 254
//
// WebUSB descriptors.
#define WL_REQUEST_WEBUSB    (254)

#endif  // __REQUESTS_H_INCLUDED__
