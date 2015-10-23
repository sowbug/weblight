# WebLight

![WebLight](https://sowbug.github.io/weblight/img/IMG_20151021_141538.jpg)

A USB-controlled LED designed for the
[WebUSB API](https://reillyeon.github.io/webusb/). Hardware, software, and
firmware.

Toolchain/environment setup
===

* On Windows, download avr-gcc from
http://andybrown.me.uk/2015/03/08/avr-gcc-492/. Also make sure you've installed
a base MinGW and MSYS system or equivalent, and have them in your system path.
* On Linux, try something like `sudo apt-get install gcc-avr binutils-avr gdb-avr avr-libc avrdude`.
* Build and install the [micronucleus command-line tool](https://github.com/micronucleus/micronucleus/tree/master/commandline) (which will need `sudo apt-get install libusb-dev` on Linux) to be able to upload to the board (which is assumed to have been flashed with that bootloader, running at 16.5MHz, etc.)
* Copy the files in `udev` to `/etc/udev/rules.d/` and then `sudo udevadm control --reload-rules`.

Bringing the board to life
===

* Once you've built the board, connect an AVR programmer.
* `cd bootloader`
* Configure the makefile to talk to your type of programmer (default usbtiny).
* `make fuse`
* `make flash`

Development
===

1. In the `firmware` directory, `make clean && make hex && micronucleus --run main.hex`.
1. Plug in the board. After a few moments you should see `Micronucleus done. Thank you!`
1. Now the lights on the board should run a startup sequence.
1. Using `lsusb` you should see something like `Bus 002 Device 019: ID 16c0:05dc Van Ooijen Technische Informatica shared ID for use with libusb`.

Usage
===

1. Install [PyUSB](http://walac.github.io/pyusb/). `sudo pip install
pyusb==1.0.0.b2`
2. `cd commandline`
3. `./weblight 800000` for a medium-brightness red light (it uses
CSS-style RGB color format).

Notable Bugs
===

* I haven't figured out how to stop the WS2812 code from interfering
  with V-USB's picky interrupt requirements. So every once in a while
  the weblight command-line tool will throw an exception.
