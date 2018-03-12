# WebLight

![WebLight](img/DSC00108.jpg)

A USB-controlled LED designed for the
[WebUSB API](https://wicg.github.io/webusb/).

Try the web app
===
Have a WebLight device already?  
Launch the web app here:  
https://sowbug.github.io/weblight/

What does the javascript look like to write color values?  
Check out [software/src/weblight-card.html](https://github.com/sowbug/weblight/blob/master/software/src/weblight-card.html)

Developing WebLight Devices
===

Toolchain/environment setup
---

* Install avr-gcc. On Windows, download avr-gcc from
http://andybrown.me.uk/2015/03/08/avr-gcc-492/. Make sure you've
installed a base MinGW and MSYS system or equivalent, and have them in
your system path. On Linux, try something like `sudo apt-get install
gcc-avr binutils-avr gdb-avr avr-libc avrdude`. On OS X,
[CrossPack](https://www.obdev.at/products/crosspack/index.html) works.
* Install libusb. `sudo apt-get install libusb-dev` works on
  Linux, and `brew install libusb libusb-compat` works on OS X.
* Install pyusb. `sudo apt-get install python-usb` on Linux, or `sudo pip
  install pyusb` on other platforms.
* Build and install the
  [micronucleus command-line tool](https://github.com/micronucleus/micronucleus/tree/master/commandline)
  to be able to upload to the ATTiny85. Have it somewhere in your PATH. Don't forget [the udev rule](https://raw.githubusercontent.com/micronucleus/micronucleus/master/commandline/49-micronucleus.rules) if needed.
* Copy the files in `udev` to `/etc/udev/rules.d/` and then `sudo
  udevadm control --reload-rules`.

Programming a newly built board
---

* Connect a 6-pin AVR programmer. Note that if you've built the
  production version of the board, you'll need either an SOIC-8 clip
  manually wired for the ATtiny85, or else a
  [ProtoProg](http://protofusion.org/wordpress/2013/05/open-hardware-pogo-pin-programmer/)
  programming
  adapter. [This is the project I ordered](https://oshpark.com/shared_projects/fqvxyzoH)
  from OSH Park, and you'll want [0.68mm diameter, 16mm length pogo
  pins](http://www.ebay.com/sch/i.html?_trksid=p3984.m570.l1313.TR0.TRC0&_nkw=%09+10pcs+P50-J1+Dia+0.68mm+Length+16mm+75g+Spring+Test.&_sacat=0&_from=R40), probably from eBay.
* `cd firmware`
* Configure the makefile to talk to your type of programmer (default
usbtiny).
* `make factory` Now you have a bootloader-ready board with no firmware.
* Disconnect the programmer and plug it into a USB port.
* `make clean upload`

Mass-producing boards
---

This clones the complete firmware from an existing device. For
convenience, a relatively stable version of that complete firmware
image is already here in the repository (full-[version].hex). If you
overwrite it, we'll assume you know what you're doing.

1. Make one device using the instructions from the previous section.
1. Unplug from USB and attach to programmer.
1. `cd firmware`
1. `make create_full` This clones the firmware of the board you just
built.
1. Attach your next board to the programmer.
1. `make factory_mp`
1. Repeat last two steps for all remaining boards.

Development
---

1. In the `firmware` directory, `make clean upload`.
1. Plug in the board. After a few moments you should see `Micronucleus
   done. Thank you!`
1. Now the lights on the board should run a startup sequence.
1. Using `lsusb` you should see something like `Bus 001 Device 006: ID
   1209:a800 InterBiometrics`.

Usage
---

1. Install [PyUSB](http://walac.github.io/pyusb/). `sudo pip install
pyusb==1.0.0.b2`
1. From the base directory of the weblight project,
`./commandline/wlctl 800000` will give you a medium-brightness red
light (it uses CSS-style RGB color format).
1. `--help` for more options.

BOM for production (not developer) circuit
---

The best results are with an ENIG finish 2.0mm thickness PCB. If you're just prototyping, 1.6mm will work, but you'll want to put some thick solder on the USB leads to bulk them up a bit. Even then, the fit in the USB socket will be loose. ENIG takes longer to wear out than regular HASL.

The board measures 12mm x 41.1734mm x 2.0mm.

* C1: 1uF 0805 capacitor (DigiKey 311-1365-1-ND)
* C2: 0.1uF 0805 capacitor (DigiKey 311-1142-1-ND)
* D1, D2: 3.6V SOD-323 Zener diode. I successfully used BZT52C3V6S-7-F and MM3Z3V6T1G (DigiKey MM3Z3V6T1GOSCT-ND). According to [V-USB Hardware Considerations](http://vusb.wikidot.com/hardware), they should be "those that look like 1N4148 (usually 500 mW or less)"
* D3, D4: low forward-voltage SOD-323 diode (0.4-volt or less forward voltage so that the supply to the LEDs is higher than 4.5 volts). I have successfully used Toshiba 1SS367 (forward voltage about 0.3V, DigiKey 1SS367H3FCT-ND).
* LED1, LED2: APA102C LED (Real versions of these are not especially easy to find)
* R1: 1.5K 0805 resistor (DigiKey 311-1.5KARCT-ND)
* R2, R3: 68 0805 resistor (DigiKey 311-68ARCT-ND)
* R4: 10K 0805 resistor (DigiKey 311-10KARCT-ND)
* U1: ATtiny85 ATTINY85-20SU (DigiKey ATTINY85-20SURCT-ND)

Tips
---

* In addition to `wlctl` in the `commandline` directory, there is
`lswusb`, which attempts to parse WebUSB descriptors. This utility
will help you when you're developing your own WebUSB-compatible
device.

FAQ
===

* **What is WebUSB?** It's a [new web
  standard](https://reillyeon.github.io/webusb/) that makes it
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
  the firmware, build
  [`weblight_developer.brd`](https://github.com/sowbug/weblight/blob/master/hardware/weblight_developer.brd). This
  one has a few more features that make firmware development more convenient. Otherwise if
  you just want a device to play with, build
  [`weblight.brd`](https://github.com/sowbug/weblight/blob/master/hardware/apa102/weblight.brd). That
  one is more compact, but it requires the ProtoProg ISP adapter, rather than the more common
  AVRISP 6-pin adapter. Both designs use 0805 SMT components. Either board can be ordered from [OSH Park](https://oshpark.com/) or [Seeed](https://www.seeedstudio.com/service/). Note that if you manufacture with a
  standard 1.6mm PCB, you'll have to solder the contacts to give them enough thickness to connect. For end-user
  boards, 2.0mm or 2.4mm will fit better in USB sockets, and ENIG finish will last longer without tarnishing.

* **I don't want to build a board, but I'd like one. Will you sell me one?** I don't have plans to mass-produce boards for sale. But if you're interested in getting one, [subscribe to notifications for this issue](https://github.com/sowbug/weblight/issues/31).

* **I remember reading something about Origin Trials. What does that have to do with WebUSB?** Chrome 61 and beyond ship with WebUSB, so if your users are running up-to-date Chrome, you're all set! But in case you're curious, the Chrome team has a system called Origin Trials for gathering real developer feedback on provisional APIs. Go to https://bit.ly/OriginTrials to learn more.

Acknowledgments
===

* Jenna Fox ([bluebie](https://github.com/bluebie)) and Tim ([cpldcpu](https://github.com/cpldcpu/)) for micronucleus advice. (Update December 2016: [Tim's article about SK9822](https://cpldcpu.com/2016/12/13/sk9822-a-clone-of-the-apa102/).)
* bobhackmelb on freenode for advice on PCB manufacturing, specifically thickness.
* [Passerby](http://electronics.stackexchange.com/users/17178/passerby) for help [reverse-engineering](http://electronics.stackexchange.com/questions/209941/two-sided-connectorless-usb-on-a-pcb) the double-sided USB connector.
* [gerblook.org](http://gerblook.org/) for an awesome Gerber visualizer.
* [Lars Knudsen](https://github.com/larsgk) for keeping docs up to date.
