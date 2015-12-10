# Copyright 2015 Mike Tsao
#
# WebLight bootloader
# https://github.com/sowbug/weblight

# `make factory` to create a brand-new device that's connected by an
# AVR programmer. Then connect the device with regular USB, and in the
# firmware directory, `make clean upload`.
#
# `make factory_mp` is like `make factory` except that it flashes a
# so-called mass-produced factory device that has the application
# firmware included.  This requires the following steps:
#
# 1. Build device.
# 2. Attach it to programmer.
# 3. `make factory`
# 4. Plug into USB port.
# 5. (in firmware directory) `make clean upload`
# 6. Unplug from USB and attach to programmer.
# 7. (in this directory) `make create_full`
#
# This clones the complete firmware from that device. For convenience,
# a relatively stable version of that complete firmware image is
# already here in the repository. If you overwrite it, we'll assume
# you know what you're doing.
#
# NB: avrdude crashes at the end of flashing on Windows machines. If
# this happens to you, do a separate `make eeprom` to complete the
# factory setup process.

DEVICE  = attiny85
AVRDUDE = avrdude -c usbtiny -p $(DEVICE)
BOOTLOADER = t85_watchdog_or_ext_reset
FULL = full-1.0

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

flash_bootloader: $(BOOTLOADER).hex
	$(AVRDUDE) -U flash:w:$(BOOTLOADER).hex:i

flash_full: $(FULL).hex
	$(AVRDUDE) -U flash:w:$(FULL).hex:i

create_full:
	$(AVRDUDE) -U flash:r:$(FULL).hex:i

eeprom:
	./gen_eeprom
	$(AVRDUDE) -U eeprom:w:eeprom.tmp:r
	rm eeprom.tmp

factory: fuse flash_bootloader eeprom

factory_mp: fuse flash_full eeprom
