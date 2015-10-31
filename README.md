# WebLight

![WebLight](https://sowbug.github.io/weblight/img/IMG_20151021_141538.jpg)

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

* Connect an AVR programmer.
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

Notable Bugs and Next Steps
===

* I haven't figured out how to stop the WS2812 code from interfering
  with V-USB's picky interrupt requirements. So every once in a while
  the weblight command-line tool will throw an exception. After
  thinking about this for a while, I've concluded that having two
  timing-sensitive components in the same single-threaded
  microcontroller is an unsolvable problem, so I'm currently
  investigating the
  [APA102C](https://cpldcpu.wordpress.com/2014/11/30/understanding-the-apa102-superled/),
  which is externally clocked and thus can almost certainly handle
  being interrupted while the ATTiny is dealing with USB matters.
