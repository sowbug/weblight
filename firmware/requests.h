// WebLight
//
// Copyright 2015 Mike Tsao


// This header is (ideally) shared between the firmware and the host
// software. It defines the USB request numbers (and optionally data
// types) used to communicate between the host and the device.

#ifndef __REQUESTS_H_INCLUDED__
#define __REQUESTS_H_INCLUDED__

// Echo: 0
//
// Requests that the device sends back wValue and wIndex. This is used
// with random data to test the reliability of the communication.
#define WL_REQUEST_ECHO (0)

// Set RGB: 1
//
// Sets the RGB color of all LEDs. Control-OUT.
//
// Pass three bytes as OUT data to indicate the desired RGB color.
#define WL_REQUEST_SET_RGB (1)

// Set HSV: 2
//
// Sets the HSV color of all LEDs. Control-OUT.
//
// Note that HSV is the same as HSB: Hue, Saturation,
// Value/Brightness.
//
// Pass three bytes as OUT data to indicate the desired HSV color. The
// first byte is hue, the second saturation, and the third brightness.
#define WL_REQUEST_SET_HSV (2)

// Set RGB sequence: 3
//
// Sets a repeating sequence for all LEDs. Control-OUT.
//
// Format:
//
// ticks (dwValue): number of ticks (each about one-sixtieth of a
// second) that should elapse per frame of sequence.
//
// frame_count (dwIndex): the number of 3-byte frames. Max 16.
//
// RGB (OUT: 3 bytes x number of frames): each frame's RGB color. Max
// 16 frames.
#define WL_REQUEST_SET_RGB_SEQUENCE (3)

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
