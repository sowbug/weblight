#include "eeprom.h"
#include "usbdrv.h"
#include "usbconfig.h"

#include <avr/eeprom.h>

// in webusb.c
extern int webUsbDescriptorStringSerialNumber[EEPROM_SERIAL_LENGTH + 1];

void readEEPROM() {
  uchar serial_bytes[EEPROM_SERIAL_LENGTH];
  eeprom_read_block((void*)serial_bytes,
                    (void*)EEPROM_SERIAL_START,
                    EEPROM_SERIAL_LENGTH);
  size_t i;
  for (i = 0; i < EEPROM_SERIAL_LENGTH; ++i) {
    webUsbDescriptorStringSerialNumber[1 + i] = serial_bytes[i];
  }
}
