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

// Set LED Count: 2
//
// Sets the number of active LEDs. Control-OUT.
//
// The requested number is passed in the "wValue" field of the control
// transfer. No OUT data is sent. The number is clamped if it exceeds
// MAX_LED_COUNT in led_control.h.
#define WL_REQUEST_SET_LED_COUNT (2)

// Reset: 253
//
// Reset the device, which triggers the bootloader.
#define WL_REQUEST_RESET_DEVICE    (253)

// WebUSB: 254
//
// WebUSB descriptors.
#define WL_REQUEST_WEBUSB    (254)

#endif  // __REQUESTS_H_INCLUDED__
