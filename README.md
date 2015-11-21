# WebLight

![WebLight](https://sowbug.github.io/weblight/img/IMG_20151111_091258_sq.jpg)

A USB-controlled LED designed for the
[WebUSB API](https://reillyeon.github.io/webusb/).

Toolchain/environment setup
===

* Install avr-gcc. On Windows, download avr-gcc from
http://andybrown.me.uk/2015/03/08/avr-gcc-492/. Make sure you've
installed a base MinGW and MSYS system or equivalent, and have them in
your system path. On Linux, try something like `sudo apt-get install
gcc-avr binutils-avr gdb-avr avr-libc avrdude`. On OS X,
[CrossPack](https://www.obdev.at/products/crosspack/index.html) works.
* Install libusb. `sudo apt-get install libusb-dev` works on
  Linux, and `brew install libusb libusb-compat` works on OS X.
* Build and install the
  [micronucleus command-line tool](https://github.com/micronucleus/micronucleus/tree/master/commandline)
  to be able to upload to the ATTiny85. Have it somewhere in your PATH.
* Copy the files in `udev` to `/etc/udev/rules.d/` and then `sudo
  udevadm control --reload-rules`.

Programming a newly built board
===

* Connect a 6-pin AVR programmer. Note that if you've built the
  production version of the board, you'll need either an SOIC-8 clip
  manually wired for the ATtiny85, or else a
  [ProtoProg](http://protofusion.org/wordpress/2013/05/open-hardware-pogo-pin-programmer/)
  programming
  adapter. [This is the project I ordered](https://oshpark.com/shared_projects/fqvxyzoH)
  from OSH Park, and you'll want [0.68mm diameter, 16mm length pogo
  pins](http://www.ebay.com/sch/i.html?_trksid=p3984.m570.l1313.TR0.TRC0&_nkw=%09+10pcs+P50-J1+Dia+0.68mm+Length+16mm+75g+Spring+Test.&_sacat=0&_from=R40), probably from eBay.
* `cd bootloader`
* Configure the makefile to talk to your type of programmer (default
  usbtiny).
* `make factory`

Development
===

1. In the `firmware` directory, `make clean upload`.
1. Plug in the board. After a few moments you should see `Micronucleus
   done. Thank you!`
1. Now the lights on the board should run a startup sequence.
1. Using `lsusb` you should see something like `Bus 001 Device 006: ID
   1209:a800 InterBiometrics`.

Usage
===

1. Install [PyUSB](http://walac.github.io/pyusb/). `sudo pip install
pyusb==1.0.0.b2`
1. From the base directory of the weblight project,
`./commandline/wlctl 800000` will give you a medium-brightness red
light (it uses CSS-style RGB color format).
1. `--help` for more options.

Tips
===

* In addition to `wlctl` in the `commandline` directory, there is
`lswusb`, which attempts to parse WebUSB descriptors. This utility
will help you when you're developing your own WebUSB-compatible
device.

FAQ
===

* **What is WebUSB?** It's a [new web
  standard](https://reillyeon.github.io/webusb/) that will make it
  easy to write a USB driver once (in JavaScript) and have it work
  everywhere that WebUSB is supported.

* **Aren't there already a lot of USB-controlled lights out there?**
  Yes, it's more or less a rite of passage in hobbyist electronics to
  make some kind of blinking light. The purpose of this project was to
  design a very simple WebUSB device that might actually be useful.

* **You said "useful" in the prior answer. Please define that.** A
  WebUSB-enabled light can alert you of interesting things, like a
  broken continuous build, a service interruption, or yet another
  crash in the price of bitcoin.

* **Which board should I build?** If you're interested in hacking on
  the firmware, build [`weblight_developer.brd` for the
  APA102](https://github.com/sowbug/weblight/blob/master/hardware/apa102/weblight_developer.brd). This
  one is bigger and has only two lights, but it has more features that
  you'll need for firmware development, and it uses 0805 SMT
  components. Otherwise build
  [`weblight.brd`](https://github.com/sowbug/weblight/blob/master/hardware/apa102/weblight.brd),
  but beware that it uses 0402 components and was meant to be
  manufactured by a service.

* **What's the difference between the WS2812 and APA102 versions?**
  Look at the [LED section in
  DESIGN.md](https://github.com/sowbug/weblight/blob/master/DESIGN.md#led)
  for the discussion. In short, WS2812 wasn't compatible with
  bitbanged USB. It probably could have worked, but the extra firmware
  development time wasn't worth the tradeoffs (the advantage being
  slightly wider availability of the WS2812 vs. the APA102). Consider
  the WS2812 references to be historical only; that version of the
  board is obsolete and unsupported.

* **Why does the device have both a male Type A and female micro-USB
  connector?** That's an old version of the hardware. I was
  experimenting with both kinds of connector to see which one was more
  practical. It would be a bad idea to ship a device with both,
  because someone would try to use it to connect two PCs at the same
  time. Currently the developer board is Type A, and the production
  board is micro-USB.
