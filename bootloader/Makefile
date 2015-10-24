# Copyright 2015 Mike Tsao
#
# weblight bootloader

DEVICE  = attiny85
AVRDUDE = avrdude -c usbtiny -p $(DEVICE) # edit this line for your programmer
FUSEOPT = -U lfuse:w:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xfe:m

flash: t85_default.hex
	$(AVRDUDE) -U flash:w:t85_default.hex:i

fuse:
	$(AVRDUDE) $(FUSEOPT) -B 20

eeprom:
	./gen_eeprom
	$(AVRDUDE) -U eeprom:w:eeprom.tmp:r
