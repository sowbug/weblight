// Memory map
//
//   0-3: Signature ('WebL')
//   4-5: Version (BCD, 1.0 = 0x0100)
//  6-21: Serial number (16-digit string)
//    22: NEXT_ITEM (was LED count) (uint8_t)

#define EEPROM_SIG 'WebL'
#define EEPROM_SIG_START (0)
#define EEPROM_SIG_LENGTH (4)

#define EEPROM_VERSION_START (4)
#define EEPROM_VERSION_LENGTH (2)

#define EEPROM_SERIAL_START (6)
#define EEPROM_SERIAL_LENGTH (16)

#define EEPROM_NEXT_ITEM_START (22)

void ReadEEPROM();
void WriteLEDCount();
