# Copyright 2015 Mike Tsao
#
# WebLight firmware
# https://github.com/sowbug/weblight
#
# NB: avrdude crashes at the end of flashing on Windows machines. If
# this happens to you, do a separate `make flash_eeprom` to complete the
# factory setup process.

DEVICE  = attiny85
F_CPU   = 16500000	# in Hz

AVRDUDE = avrdude -c usbtiny -p $(DEVICE) -B 1
WLCTL = ../commandline/wlctl
MICRONUCLEUS = micronucleus

APP = main
BOOTLOADER = hex/t85_watchdog_or_ext_reset
FULL = hex/full-1.0
EEPROM = eeprom

# http://eleccelerator.com/fusecalc/fusecalc.php?chip=attiny85&LOW=E1&HIGH=DD&EXTENDED=FE&LOCKBIT=FF
#
# SPI enabled, brown-out at 2.7V, self-programming enabled, PLL clock
# (identical to recommended DigiSpark/micronucleus settings for ATtiny85)
FUSEOPT = \
	-U lfuse:w:0xe1:m \
	-U hfuse:w:0xdd:m \
	-U efuse:w:0xfe:m

LED_COUNT ?= 2

CFLAGS  = -Iusbdrv -I. -DDEBUG_LEVEL=0
OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o \
          $(APP).o eeprom.c led_control.c osccal.c webusb.c
OBJECTS += candle.c light_apa102.c sequencer.c

COMPILE = avr-gcc \
									-Wall -Os -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=$(DEVICE) \
									-DLED_COUNT=$(LED_COUNT)

# symbolic targets:
help:
	@echo "This Makefile has no default rule. Read the README."

fuse:
	$(AVRDUDE) $(FUSEOPT) -B 20

.PHONY: $(EEPROM).hex flash_bootloader flash_full factory factory_mp upload

$(BOOTLOADER).hex:

flash_bootloader: $(BOOTLOADER).hex
	$(AVRDUDE) -U flash:w:$(BOOTLOADER).hex:i

flash_full: $(FULL).hex
	$(AVRDUDE) -U flash:w:$(FULL).hex:i

create_full:
	$(AVRDUDE) -U flash:r:$(FULL).hex:i

flash_eeprom: $(EEPROM).hex
	$(AVRDUDE) -U eeprom:w:$(EEPROM).hex:r
	rm -f $(EEPROM).hex

$(EEPROM).hex:
	python tools/gen_eeprom > $(EEPROM).hex

factory: fuse flash_bootloader flash_eeprom

factory_mp: fuse flash_full flash_eeprom

upload: $(APP).hex
	$(WLCTL) --reset
	$(MICRONUCLEUS) --run $(APP).hex

clean:
	rm -f *.hex *.lst *.obj *.o $(APP).cof $(APP).list \
              $(APP).map $(APP).eep.hex $(APP).elf *.o usbdrv/*.o \
              $(APP).s usbdrv/usbdrv.s

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

$(APP).elf: $(OBJECTS)
	$(COMPILE) -o $(APP).elf $(OBJECTS)

$(APP).hex: $(APP).elf
	rm -f $(APP).hex $(APP).eep.hex
	avr-objcopy -j .text -j .data -O ihex $(APP).elf $(APP).hex
	avr-size $(APP).hex

disasm:	$(APP).elf
	avr-objdump -d $(APP).elf

cpp:
	$(COMPILE) -E $(APP).c
