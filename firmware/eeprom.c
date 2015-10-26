#include "eeprom.h"
#include "led_control.h"
#include "usbdrv.h"
#include "usbconfig.h"

#include <avr/eeprom.h>

// in webusb.c
extern int webUsbDescriptorStringSerialNumber[EEPROM_SERIAL_LENGTH + 1];

void ReadEEPROM() {
  uchar serial_bytes[EEPROM_SERIAL_LENGTH];
  eeprom_read_block((void*)serial_bytes,
                    (void*)EEPROM_SERIAL_START,
                    EEPROM_SERIAL_LENGTH);
  size_t i;
  for (i = 0; i < EEPROM_SERIAL_LENGTH; ++i) {
    webUsbDescriptorStringSerialNumber[1 + i] = serial_bytes[i];
  }

  SetLEDCount(eeprom_read_byte((void*)EEPROM_LED_COUNT_START));
}

void WriteLEDCount() {
  uint8_t count = eeprom_read_byte((void*)EEPROM_LED_COUNT_START);
  uint8_t sram_count = GetLEDCount();
  if (count != sram_count) {
    eeprom_write_byte((void*)EEPROM_LED_COUNT_START, sram_count);
  }
}
