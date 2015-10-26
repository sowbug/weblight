// Memory map
//
//   0-3: Signature ('WebL')
//  4-19: Serial number (16-digit string)
//    20: LED count (uint8_t)

#define EEPROM_SIG 'WebL'
#define EEPROM_SIG_START (0)
#define EEPROM_SIG_LENGTH (4)

#define EEPROM_SERIAL_START (4)
#define EEPROM_SERIAL_LENGTH (16)

#define EEPROM_LED_COUNT_START (20)

void ReadEEPROM();
void WriteLEDCount();
