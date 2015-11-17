# Copyright 2015 Mike Tsao
#
# WebLight bootloader
# https://github.com/sowbug/weblight

# `make factory` to create a brand-new device that's connected by an
# AVR programmer. Then connect the device with regular USB, and in the
# firmware directory, `make clean upload`.
#
# NB: avrdude crashes at the end of flashing on Windows machines. If
# this happens to you, do a separate `make eeprom` to complete the
# factory setup process.

DEVICE  = attiny85
AVRDUDE = avrdude -c usbtiny -p $(DEVICE)
HEXFILE = t85_watchdog_or_ext_reset.hex

# http://eleccelerator.com/fusecalc/fusecalc.php?chip=attiny85&LOW=E1&HIGH=DD&EXTENDED=FE&LOCKBIT=FF
#
# SPI enabled, brown-out at 2.7V, self-programming enabled, PLL clock
# (identical to recommended DigiSpark/micronucleus settings for ATtiny85)
FUSEOPT = \
	-U lfuse:w:0xe1:m \
	-U hfuse:w:0xdd:m \
	-U efuse:w:0xfe:m

fuse:
	$(AVRDUDE) $(FUSEOPT) -B 20

flash: $(HEXFILE)
	$(AVRDUDE) -U flash:w:$(HEXFILE):i

eeprom:
	./gen_eeprom
	$(AVRDUDE) -U eeprom:w:eeprom.tmp:r

factory: fuse flash eeprom
