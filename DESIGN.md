WebLight Design
===

Goal
===

To create a simple and inexpensive device that shows off
[WebUSB](https://reillyeon.github.io/webusb/) and helps validate the
specification.

Overview
===

WebLight is a small circuit on a standalone printed circuit board that
contains multicolored lights controllable by the computer it's plugged
into. When plugged into a Type A USB receptable (the kind normally
found on desktop and laptop PCs), it appears as a custom-class USB
device that can be controlled by host software. Additionally, it
responds to requests for WebUSB descriptors that indicate its
willingness to communicate with certain web origins through the WebUSB
API.

The user benefit of WebUSB is that hardware devices are even more
plug-and-play than the traditional sense of that term: any
WebUSB-compatible device will work without the requirement to install
USB drivers.

Circuit
===

* Influenced by
[Adafruit Trinket](https://learn.adafruit.com/introducing-trinket/downloads),
[V-USB design suggestions](http://vusb.wikidot.com/hardware), and the
[DigiSpark](https://digistump.com/wiki/digispark/tutorials/digispark).

* We chose the "Level conversion on D+ and D-" option for solving the
mismatch between 5v AVR/LED and 3.3v USB data lines. The reason we
picked that approach rather than a lower-voltage AVR circuit is that
it requires fewer and less-expensive components, and the disadvantages
("Not a clean solution," slight power wastage) wouldn't affect the end
product in noticeable ways.

PCB
===

* HASL (Hot Air Solder Leveling) is cheapest and easiest for manual
soldering, but it usually contains lead (lead-free HASL is available).

* ENIG (Electroless Nickel Immersion Gold) is expensive and looks
nice. It's harder to deal with when manually soldering.

* Most connectorless PCBs in production range from 2.4 to 2.6mm in
thickness. This thickness is not available from short-run PCB
manufacturers.

* 2.0mm is
[reputed](http://electronics.stackexchange.com/questions/13174/connectorless-usb-on-a-pcb)
to work OK, and short-run PCB manufacturers support it.

* 1.6mm is typical for prototyping PCBs. This works with a piece of
paper taped on the back of the PCB, beneath the connector area.
Otherwise it's a loose fit that is unreliable in some USB ports.

* Typical width of connectorless PCBs is 12mm.

* Length doesn't matter, except for ergonomics.

* Making the PCB pluggable in either direction (upside-down or normal)
requires extra diodes.

Microcontroller
===

* Atmel AVR is inexpensive and has a fully open toolchain. I have
experience with AVR.

* Microchip PIC is usually the other contender. I don't have
experience with PICs. While there are free compilers, the widely used
ones are not open. For at least the past five years, the open-source
[SDCC](http://sdcc.sourceforge.net/) has been aiming to support PIC16
and PIC18.

* The [ATTiny85](http://www.atmel.com/devices/attiny85.aspx) is a
popular choice for small applications that bitbang USB. The SOIC8
version costs about 75 cents in quantity 100+.

* The ATMega u2 and u4 series have built-in USB support, but they are
relatively expensive (more than $3/unit in quantity).

LED
===

* The WS2812B is the current hobbyist's choice for low-cost
addressable RGB LED. It has substantial software support thanks in
part to the [Adafruit Neopixel](http://www.adafruit.com/category/168)
brand. Brightness is adequate; maximum current is about 60mA at 5V.
Disadvantages: the component has a reputation for being easily killed
by voltage spikes, leading to
[best-practice recommendations](https://learn.adafruit.com/adafruit-neopixel-uberguide/best-practices)
to protect with large capacitors and resistors. The chip protocol is
extremely timing-sensitive, meaning it will monopolize the
microcontroller's execution for significant periods of time. Finally,
the refresh rate is relatively low (400Hz), which makes it
inappropriate for some applications (but isn't a consideration for
this design).

* The APA102C has a similar form factor to the WS2812B (5050 SMD) and
similar luminosity and power characteristics. However, it uses an
SPI-like protocol that is not sensitive to timing, making it ideal for
an application like this where the microcontroller will be servicing
timing-sensitive bitbanged USB requests. As of October 2015, is
slightly more expensive than the WS2812 in quantity (12 cents vs. 25
cents), but because the maker and DIY lighting communities are
embracing it, its price should come down soon.

Firmware
===

* [V-USB](https://www.obdev.at/products/vusb/) is the standard for AVR
USB bitbanging. Its
[license terms](https://www.obdev.at/products/vusb/license.html) are
annoying but tolerable for this entirely open-source project.

* [Micronucleus](https://github.com/micronucleus/micronucleus/) is a
bootloader based on V-USB. It has a smaller binary size, is reported
to be faster, and claims better compatibility.

* Having a bootloader is desirable during development, and likely to
be useful to update production devices. It might turn out to be
unnecessary for production devices, but today there is little cost to
keeping it.

* [light_ws2812](https://github.com/cpldcpu/light_ws2812) is a much
lighterweight WS2812 library compared to
[Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel). As
noted above, WS2812 is not the long-term choice for this project.

* As shown in the
[Adafruit DotStar](https://github.com/adafruit/Adafruit_DotStar)
Arduino library for APA102, there isn't a tremendous need for a large
or resource-intensive software library for this LED.

* TODO: describe V-USB and usbFunctionSetup()

WebUSB
===

* TODO

* TODO: describe challenge with WinUSB

USB VID/PID ===

* [pid.codes](http://pid.codes/) provides one-off maker/hobbyist
VID/PID pairs, eliminating the need to use garbage pairs or submit to
the
[burdensome and expensive requirements](http://www.usb.org/developers/vendor/)
of the USB Implementers Forum.

Prototype Manufacturing
===

* [OSH Park](https://oshpark.com/)

* [MacroFab](http://macrofab.net/)

Production Manufacturing
===

* TODO
