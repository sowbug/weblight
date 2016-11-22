// Copyright 2015 Mike Tsao
//
// WebLight firmware
// https://github.com/sowbug/weblight

#include "eeprom.h"
#include "led_control.h"
#include "usbdrv.h"
#include "usbconfig.h"

#include <avr/eeprom.h>
#include <string.h>
#include <stdbool.h>

// in webusb.c
extern int webUsbDescriptorStringSerialNumber[EEPROM_SERIAL_LENGTH + 1];

const uchar sig[EEPROM_SIG_LENGTH] = { 'W', 'e', 'b', 'L' };

uint8_t IsEEPROMValid() {
  uchar sig_bytes[EEPROM_SIG_LENGTH];
  eeprom_read_block((void*)sig_bytes, (void*)EEPROM_SIG_START,
                    EEPROM_SIG_LENGTH);
  return 0 == memcmp(sig, sig_bytes, EEPROM_SIG_LENGTH);
}

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

void SetUpNewEEPROM() {
  eeprom_update_block(sig, (void*)(EEPROM_SIG_START), EEPROM_SIG_LENGTH);
  eeprom_update_byte((uint8_t*)EEPROM_VERSION_START, DEVICE_VERSION_MAJOR);
  eeprom_update_byte((uint8_t*)EEPROM_VERSION_START + 1,
                     DEVICE_VERSION_MINOR);
  eeprom_update_block((const void*)&webUsbDescriptorStringSerialNumber[1],
                      (void*)EEPROM_SERIAL_START, EEPROM_SERIAL_LENGTH);
}

void GenerateEEPROMData() {
  size_t i;
  for (i = 0; i < EEPROM_SERIAL_LENGTH; ++i) {
    webUsbDescriptorStringSerialNumber[1 + i] = 'a' + i;
  }
}

uint8_t ReadLightProgram(uint8_t *opcode_buf, uint8_t opcode_buf_len) {
  uint8_t program_length =
    eeprom_read_byte((const uint8_t*)EEPROM_PROGRAM_SIZE);
  if (program_length == 0 || program_length > EEPROM_PROGRAM_MAX_SIZE) {
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
  if (opcode_buf_len == 0 || opcode_buf_len > EEPROM_PROGRAM_MAX_SIZE) {
    return;
  }
  eeprom_update_block((const void*)opcode_buf,
                      (void*)EEPROM_PROGRAM_START,
                      opcode_buf_len);
}

uint8_t GetDescriptorStart(uint8_t index,
                           const uint8_t **pmResponsePtr,
                           uint8_t *pmResponseBytesRemaining) {
  *pmResponsePtr = (const uint8_t *)EEPROM_WEBUSB_URLS_START;
  do {
    *pmResponseBytesRemaining = eeprom_read_byte(*pmResponsePtr);

    // Found item. Return it.
    if (index-- == 0) {
      return true;
    }

    // Exceeded number of items in sequence. Return last one but indicate
    // failure.
    if (*pmResponseBytesRemaining == 0) {
      return false;
    }

    *pmResponsePtr += *pmResponseBytesRemaining;

  } while (true);
}
