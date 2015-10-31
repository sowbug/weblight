# Copyright 2015 Mike Tsao
#
# weblight bootloader

DEVICE  = attiny85
AVRDUDE = avrdude -c usbtiny -p $(DEVICE)
FUSEOPT = \
	-U lfuse:w:0xe1:m \
	-U hfuse:w:0xdd:m \
	-U efuse:w:0xfe:m
HEXFILE = t85_watchdog_or_ext_reset.hex

flash: $(HEXFILE)
	$(AVRDUDE) -U flash:w:$(HEXFILE):i

fuse:
	$(AVRDUDE) $(FUSEOPT) -B 20

eeprom:
	./gen_eeprom
	$(AVRDUDE) -U eeprom:w:eeprom.tmp:r

factory: fuse flash eeprom
