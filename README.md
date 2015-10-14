# WebLight

A USB-controlled LED designed for the WebUSB API. Hardware, software, and
firmware.

Toolchain setup
===============

* On Windows, download avr-gcc from
http://andybrown.me.uk/2015/03/08/avr-gcc-492/. Also make sure you've installed
a base MinGW and MSYS system or equivalent, and have them in your system path.
* On Linux, try something like `sudo apt-get install gcc-avr binutils-avr gdb-avr avr-libc avrdude`.
* Build and install the [micronucleus command-line tool](https://github.com/micronucleus/micronucleus/tree/master/commandline) to be able to upload to the board (which is assumed to have been flashed with that bootloader, running at 16.5MHz, etc.)

Development
===========

1. In the `firmware` directory, `make clean && make hex && sudo ~/bin/micronucleus --run main.hex`.
1. Plug in the board. After a few moments you should see `Micronucleus done. Thank you!`
1. Now the first light on the board should turn green.
1. Using `lsusb` you should see something like `Bus 002 Device 019: ID 16c0:05dc Van Ooijen Technische Informatica shared ID for use with libusb`.
