// Copyright 2015 Mike Tsao
//
// WebLight firmware
// https://github.com/sowbug/weblight

#if USE_CANDLE
#include "candle.h"
#endif

#include "eeprom.h"
#include "sequencer.h"
#include "requests.h"
#include "webusb.h"

#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <string.h>
#include <stdbool.h>

#define USB_BOS_DESCRIPTOR_TYPE (15)

#define MS_OS_20_DESCRIPTOR_LENGTH (0x1e)

const uchar BOS_DESCRIPTOR[] PROGMEM = {
    // BOS descriptor header
    0x05, 0x0F, 0x39, 0x00, 0x02,

    // WebUSB Platform Capability descriptor
    0x18, // Descriptor size (24 bytes)
    0x10, // Descriptor type (Device Capability)
    0x05, // Capability type (Platform)
    0x00, // Reserved

    // WebUSB Platform Capability ID (3408b638-09a9-47a0-8bfd-a0768815b665)
    0x38, 0xB6, 0x08, 0x34,
    0xA9, 0x09,
    0xA0, 0x47,
    0x8B, 0xFD,
    0xA0, 0x76, 0x88, 0x15, 0xB6, 0x65,

    0x00, 0x01,        // WebUSB version 1.0
    WL_REQUEST_WEBUSB, // Vendor-assigned WebUSB request code
    0x01,              // Landing page: https://sowbug.github.io/webusb

    // Microsoft OS 2.0 Platform Capability Descriptor
    // Thanks http://janaxelson.com/files/ms_os_20_descriptors.c
    0x1C, // Descriptor size (28 bytes)
    0x10, // Descriptor type (Device Capability)
    0x05, // Capability type (Platform)
    0x00, // Reserved

    // MS OS 2.0 Platform Capability ID (D8DD60DF-4589-4CC7-9CD2-659D9E648A9F)
    0xDF, 0x60, 0xDD, 0xD8,
    0x89, 0x45,
    0xC7, 0x4C,
    0x9C, 0xD2,
    0x65, 0x9D, 0x9E, 0x64, 0x8A, 0x9F,

    0x00, 0x00, 0x03, 0x06, // Windows version (8.1) (0x06030000)
    MS_OS_20_DESCRIPTOR_LENGTH, 0x00,
    WL_REQUEST_WINUSB, // Vendor-assigned bMS_VendorCode
    0x00               // Doesn’t support alternate enumeration
};

// Microsoft OS 2.0 Descriptor Set
//
// See https://goo.gl/4T73ef for discussion about bConfigurationValue:
//
// "It looks like we'll need to update the MSOS 2.0 Descriptor docs to
// match the implementation in USBCCGP. The bConfigurationValue in the
// configuration subset header should actually just be an index value,
// not the configuration value. Specifically it's the index value
// passed to GET_DESCRIPTOR to retrieve the configuration descriptor.
// Try changing the value to 0 and see if that resolves the issue.
// Sorry for the confusion."
#define WINUSB_REQUEST_DESCRIPTOR (0x07)
const uchar MS_OS_20_DESCRIPTOR_SET[MS_OS_20_DESCRIPTOR_LENGTH] PROGMEM = {
    // Microsoft OS 2.0 descriptor set header (table 10)
    0x0A,
    0x00, // Descriptor size (10 bytes)
    0x00,
    0x00, // MS OS 2.0 descriptor set header
    0x00,
    0x00,
    0x03,
    0x06, // Windows version (8.1) (0x06030000)
    MS_OS_20_DESCRIPTOR_LENGTH,
    0x00, // Size, MS OS 2.0 descriptor set

    // Microsoft OS 2.0 compatible ID descriptor (table 13)
    0x14,
    0x00, // wLength
    0x03,
    0x00, // MS_OS_20_FEATURE_COMPATIBLE_ID
    'W',
    'I',
    'N',
    'U',
    'S',
    'B',
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};

#define WEBUSB_REQUEST_GET_ALLOWED_ORIGINS (0x01)
#define WEBUSB_REQUEST_GET_URL (0x02)

const char usbDescriptorDevice[] PROGMEM = {
    // USB device descriptor
    0x12,            // sizeof(usbDescriptorDevice): length of descriptor in bytes
    USBDESCR_DEVICE, // descriptor type
    0x10, 0x02,      // USB version supported == 2.1
    USB_CFG_DEVICE_CLASS,
    USB_CFG_DEVICE_SUBCLASS,
    0, // protocol
    8, // max packet size
    // the following two casts affect the first byte of the constant only, but
    // that's sufficient to avoid a warning with the default values.
    (char)USB_CFG_VENDOR_ID,
    (char)USB_CFG_DEVICE_ID,
    USB_CFG_DEVICE_VERSION,
    1, // manufacturer string index
    2, // product string index
    3, // serial number string index
    1, // number of configurations
};

#define SERIAL_NUMBER_BYTE_COUNT (EEPROM_SERIAL_LENGTH * sizeof(int))
const int webUsbDescriptorStringSerialNumber[EEPROM_SERIAL_LENGTH + 1] = {
    USB_STRING_DESCRIPTOR_HEADER(EEPROM_SERIAL_LENGTH)};

USB_PUBLIC usbMsgLen_t usbFunctionDescriptor(usbRequest_t *rq)
{
  switch (rq->wValue.bytes[1])
  {
  case USBDESCR_STRING:
    switch (rq->wValue.bytes[0])
    {
    case 3:
      usbMsgPtr = (usbMsgPtr_t)(webUsbDescriptorStringSerialNumber);
      return sizeof(webUsbDescriptorStringSerialNumber);
    }
    break;
  case USB_BOS_DESCRIPTOR_TYPE:
    usbMsgPtr = (usbMsgPtr_t)(BOS_DESCRIPTOR);
    return sizeof(BOS_DESCRIPTOR);
  default:
    break;
  }
  return 0;
}

void forceReset()
{
  StatusBlink(3);
  wdt_enable(WDTO_15MS);
  for (;;)
    ;
}

void HandleEffect(uint16_t value)
{
  switch (value)
  {
#if USE_CANDLE
  case 0:
    Stop();
    CandleInit();
    SetProgramMode(CANDLE);
    break;
#endif
  default:
    break;
  }
}

static uchar buffer[64];
static uint16_t currentValue, currentIndex;
static uchar currentPosition, bytesRemaining;
const uchar *pmResponsePtr;
uchar pmResponseBytesRemaining;
uchar pmResponseIsEEPROM;
uchar currentRequest;
usbMsgLen_t usbFunctionSetup(uchar data[8])
{
  usbRequest_t *rq = (void *)data;
  static uchar dataBuffer[4];
  currentRequest = rq->bRequest;
  currentValue = rq->wValue.word;
  currentIndex = rq->wIndex.word;

  usbMsgPtr = (int)dataBuffer;
  switch (rq->bRequest)
  {
  case WL_REQUEST_ECHO:
    dataBuffer[0] = rq->wValue.bytes[0];
    dataBuffer[1] = rq->wValue.bytes[1];
    dataBuffer[2] = rq->wIndex.bytes[0];
    dataBuffer[3] = rq->wIndex.bytes[1];
    return 4;
  case WL_REQUEST_COLOR:
  case WL_REQUEST_TRANSITION:
  case WL_REQUEST_PAUSE:
  case WL_REQUEST_SELECT_LEDS:
    currentPosition = 0;
    bytesRemaining = rq->wLength.word;
    if (bytesRemaining > sizeof(buffer))
    {
      bytesRemaining = sizeof(buffer);
    }
    return USB_NO_MSG;
  case WL_REQUEST_SET_SERIAL_NUMBER:
    currentPosition = 0;
    bytesRemaining = rq->wLength.word;
    return USB_NO_MSG;
  case WL_REQUEST_SET_WEBUSB_URLS:
    currentPosition = 0;
    bytesRemaining = rq->wLength.word;
    return USB_NO_MSG;
  case WL_REQUEST_HALT:
    HandleHALT();
    break;
  case WL_REQUEST_RECORD:
    Record();
    break;
  case WL_REQUEST_PLAY:
    Play();
    break;
  case WL_REQUEST_STOP:
    Stop();
    break;
  case WL_REQUEST_SAVE:
    Save();
    break;
  case WL_REQUEST_LOAD:
    Load();
    break;
  case WL_REQUEST_EFFECT:
    HandleEffect(rq->wValue.word);
    break;
  case WL_REQUEST_RESET_WATCHDOG:
    ResetAppWatchdog(rq->wValue.word * 1000);
    break;
  case WL_REQUEST_WEBUSB:
  {
    pmResponseIsEEPROM = true;
    switch (rq->wIndex.word)
    {
    case WEBUSB_REQUEST_GET_ALLOWED_ORIGINS:
      GetDescriptorStart(0, &pmResponsePtr, &pmResponseBytesRemaining);
      return USB_NO_MSG;
    case WEBUSB_REQUEST_GET_URL:
      if (GetDescriptorStart(rq->wValue.word,
                             &pmResponsePtr,
                             &pmResponseBytesRemaining))
      {
        return USB_NO_MSG;
      }
      else
      {
        // Host is messing with us.
        forceReset();
      }
    }
    break;
  }
  case WL_REQUEST_WINUSB:
  {
    switch (rq->wIndex.word)
    {
    case WINUSB_REQUEST_DESCRIPTOR:
      pmResponsePtr = MS_OS_20_DESCRIPTOR_SET;
      pmResponseBytesRemaining = sizeof(MS_OS_20_DESCRIPTOR_SET);
      return USB_NO_MSG;
    }
    break;
  }
  case WL_REQUEST_RESET_DEVICE:
    forceReset();
    break;
  }

  return 0;
}

// Reading is from the perspective of the host: this method supplies
// data for the host to read.
USB_PUBLIC uchar usbFunctionRead(uchar *data, uchar len)
{
  if (len > pmResponseBytesRemaining)
  {
    len = pmResponseBytesRemaining;
  }
  if (pmResponseIsEEPROM)
  {
    eeprom_read_block(data, pmResponsePtr, len);
  }
  else
  {
    memcpy_P(data, pmResponsePtr, len);
  }
  pmResponsePtr += len;
  pmResponseBytesRemaining -= len;
  return len;
}

// Writing is from the perspective of the host: this method handles
// incoming data from the host.
USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len)
{
  uchar i;

  if (currentRequest == WL_REQUEST_SET_WEBUSB_URLS)
  {
    eeprom_update_block(data,
                        (void *)(EEPROM_WEBUSB_URLS_START + currentPosition),
                        len);
    currentPosition += len;
    bytesRemaining -= len;
    return bytesRemaining == 0;
  }

  if (currentRequest == WL_REQUEST_SET_SERIAL_NUMBER)
  {
    eeprom_update_block(data,
                        (void *)(EEPROM_SERIAL_START + currentPosition),
                        len);
    currentPosition += len;
    bytesRemaining -= len;
    return bytesRemaining == 0;
  }

  if (len > bytesRemaining)
  {
    len = bytesRemaining;
  }
  bytesRemaining -= len;
  for (i = 0; i < len; i++)
  {
    buffer[currentPosition++] = data[i];
  }

  if (bytesRemaining == 0)
  {
    switch (currentRequest)
    {
    case WL_REQUEST_COLOR:
      if (!IsRecording())
      {
        Stop();
      }
      HandleTRANSITION(NONE, 0);
      HandleCOLOR(buffer[1], buffer[2], buffer[3], buffer[0]);
      break;
    case WL_REQUEST_PAUSE:
      if (IsRecording())
      {
        HandlePAUSE((buffer[0] << 8) | buffer[1]);
      }
      else
      {
        // This doesn't make sense interactively. Ignore.
      }
      break;
    case WL_REQUEST_TRANSITION:
      if (!IsRecording())
      {
        Stop();
      }
      HandleTRANSITION(buffer[0], (buffer[1] << 8) | buffer[2]);
      break;
    case WL_REQUEST_SELECT_LEDS:
      if (!IsRecording())
      {
        Stop();
      }
      HandleSELECT((buffer[0] << 8) | (buffer[1]));
      break;
    }
  }

  return bytesRemaining == 0; // return 1 if we have all data
}
