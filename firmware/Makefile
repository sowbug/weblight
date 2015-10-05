# Copyright 2015 Mike Tsao
#
# usb-led
#

DEVICE  = attiny85
F_CPU   = 16500000	# in Hz

# PROGRAMMER contains AVRDUDE options to address your programmer
PROGRAMMER ?= -c digispark

# Tools (NOTE hardcoded for micronucleus at present)
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)

SRC = /Users/mike/OneDrive/src
CFLAGS  = -Iusbdrv -I. \
          -I$(SRC)/libusb-win32-bin-1.2.6.0/include \
          -DDEBUG_LEVEL=0
OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o main.o

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=$(DEVICE)

# Generic rule for compiling C files:
.c.o:
	$(COMPILE) -c $< -o $@

# Generic rule for assembling Assembler source files:
.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

# Generic rule for compiling C to assembler, used for debugging only.
.c.s:
	$(COMPILE) -S $< -o $@

all: main.hex

flash: all
	micronucleus main.hex

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size main.hex

clean:
	rm -f main.hex main.lst main.obj main.cof main.list \
  main.map main.eep.hex main.elf \
  *.o usbdrv/*.o main.s usbdrv/oddebug.s usbdrv/usbdrv.s
