/*
 * light_ws2812_config.h
 *
 * Created: 18.01.2014 09:58:15
 *
 * User Configuration file for the light_ws2812_lib
 *
 */


#ifndef WS2812_CONFIG_H_
#define WS2812_CONFIG_H_

///////////////////////////////////////////////////////////////////////
// Define I/O pin
///////////////////////////////////////////////////////////////////////

#define ws2812_port B     // Data port
#define ws2812_pin  0     // Data out pin

// If DISABLE_WS2812_CLI is defined, it removes the cli() at the start
// of the light-control block. It means that any interrupts will
// interfere with the timing, making the lights flicker or display the
// wrong colors.
//
// Added only for experimentation.

//#define DISABLE_WS2812_CLI 1

#endif /* WS2812_CONFIG_H_ */
