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
}

uint8_t ReadLightProgram(uint8_t *opcode_buf, uint8_t opcode_buf_len) {
  uint8_t program_length =
    eeprom_read_byte((const uint8_t*)EEPROM_PROGRAM_SIZE);
  if (program_length == 0 || program_length == 255) {
    return 0;
  }
  if (program_length > opcode_buf_len) {
    program_length = opcode_buf_len;
  }
  eeprom_read_block((void*)opcode_buf,
                    (void*)EEPROM_PROGRAM_START,
                    program_length);
  return program_length;
}

void WriteLightProgram(const uint8_t *opcode_buf, uint8_t opcode_buf_len) {
  eeprom_update_byte((uint8_t*)EEPROM_PROGRAM_SIZE, opcode_buf_len);
  if (opcode_buf_len == 0 || opcode_buf_len == 0xff) {
    return;
  }
  eeprom_update_block((const void*)opcode_buf,
                      (void*)EEPROM_PROGRAM_START,
                      opcode_buf_len);
}
