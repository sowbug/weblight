# Copyright 2015 Mike Tsao
#
# weblight
#
DEVICE  = attiny85
F_CPU   = 16500000	# in Hz
HEXFILE = main.hex
AVRDUDE = avrdude -c usbtiny -p $(DEVICE)
WLCTL = ../commandline/wlctl
MICRONUCLEUS = micronucleus

CFLAGS  = -Iusbdrv -I. -DDEBUG_LEVEL=0
OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o \
          main.o eeprom.c led_control.c osccal.c webusb.c light_ws2812.c

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=$(DEVICE)

# symbolic targets:
help:
	@echo "This Makefile has no default rule. Use one of the following:"
	@echo "make hex ....... to build $(HEXFILE)"
	@echo "make flash ..... to flash the firmware (use this on metaboard)"
	@echo "make clean ..... to delete objects and hex file"

hex: $(HEXFILE)

flash: $(HEXFILE)
	$(AVRDUDE) -U flash:w:$(HEXFILE):i

upload: $(HEXFILE)
	$(WLCTL) --reset
	$(MICRONUCLEUS) --run $(HEXFILE)

clean:
	rm -f *.hex *.lst *.obj *.o main.cof main.list \
              main.map main.eep.hex main.elf *.o usbdrv/*.o \
              main.s usbdrv/usbdrv.s

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

$(HEXFILE): main.elf
	rm -f $(HEXFILE) main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.elf $(HEXFILE)
	avr-size $(HEXFILE)

disasm:	main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c
