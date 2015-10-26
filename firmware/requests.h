// This header is shared between the firmware and the host
// software. It defines the USB request numbers (and optionally data
// types) used to communicate between the host and the device.

#ifndef __REQUESTS_H_INCLUDED__
#define __REQUESTS_H_INCLUDED__

// Requests that the device sends back wValue and wIndex. This is used
// with random data to test the reliability of the communication.
#define CUSTOM_RQ_ECHO (0)

// Sets the RGB color of all LEDs. Control-OUT.
//
// Pass three bytes as OUT data to indicate the desired RGB color.
#define CUSTOM_RQ_SET_RGB (1)

// Sets the number of active LEDs. Control-OUT.
//
// The requested number is passed in the "wValue" field of the control
// transfer. No OUT data is sent. The number is clamped if it exceeds
// MAX_LED_COUNT in led_control.h.
#define CUSTOM_RQ_SET_LED_COUNT (2)

// WebUSB descriptors.
#define CUSTOM_RQ_WEBUSB    (3)

// Reset the device and trigger the bootloader.
#define CUSTOM_RQ_RESET_DEVICE    (254)

#endif  // __REQUESTS_H_INCLUDED__
