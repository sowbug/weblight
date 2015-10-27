#include "eeprom.h"
#include "requests.h"
#include "webusb.h"

#include <avr/pgmspace.h>
#include <avr/wdt.h>

#define USB_BOS_DESCRIPTOR_TYPE (15)

PROGMEM const uchar BOS_DESCRIPTOR[] = {
  // BOS descriptor header
  0x05, 0x0F, 0x1C, 0x00, 0x01,

  // WebUSB Platform Capability descriptor
  0x17, 0x10, 0x05, 0x00, 0x38, 0xB6, 0x08, 0x34, 0xA9, 0x09, 0xA0, 0x47,
  0x8B, 0xFD, 0xA0, 0x76, 0x88, 0x15, 0xB6, 0x65, 0x00, 0x01,
  WL_REQUEST_WEBUSB,
};

// The WebUSB descriptors can't be PROGMEM because usbFunctionSetup()
// doesn't know how to read from flash, and for some weird reason when
// I tried to implement flash-reading capabilities there, it stalled
// out the USB pipeline.
//
// Solving this will be important if we ever run low on SRAM.
#define WEBUSB_REQUEST_GET_LANDING_PAGE (0x02)
const uchar WEBUSB_LANDING_PAGE[] = {
  0x23, 0x03, 'h', 't', 't', 'p', 's', ':', '/', '/', 's', 'o', 'w', 'b', 'u',
  'g', '.', 'g', 'i', 't', 'h', 'u', 'b', '.', 'i', 'o', '/',
  'w', 'e', 'b', 'l', 'i', 'g', 'h', 't'
};

#define WEBUSB_REQUEST_GET_ALLOWED_ORIGINS (0x01)
const uchar WEBUSB_ALLOWED_ORIGINS[] = {
  0x04, 0x00, 0x35, 0x00, 0x1A, 0x03, 'h', 't', 't', 'p', 's', ':', '/', '/',
  's', 'o', 'w', 'b', 'u', 'g', '.', 'g', 'i', 't', 'h', 'u', 'b',
  '.', 'i', 'o', 0x17, 0x03, 'h', 't', 't', 'p', ':', '/', '/',
  'l', 'o', 'c', 'a', 'l', 'h', 'o', 's', 't', ':', '8', '0', '0', '0'
};

PROGMEM const char usbDescriptorDevice[] = {  // USB device descriptor
  18,  // sizeof(usbDescriptorDevice): length of descriptor in bytes
  USBDESCR_DEVICE,        /* descriptor type */
  0x10, 0x02,             /* USB version supported == 2.1 */
  USB_CFG_DEVICE_CLASS,
  USB_CFG_DEVICE_SUBCLASS,
  0,                      /* protocol */
  8,                      /* max packet size */
  /* the following two casts affect the first byte of the constant only, but
   * that's sufficient to avoid a warning with the default values.
   */
  (char)USB_CFG_VENDOR_ID,/* 2 bytes */
  (char)USB_CFG_DEVICE_ID,/* 2 bytes */
  USB_CFG_DEVICE_VERSION, /* 2 bytes */
  1,  // manufacturer string index
  2,  // product string index
  3,  // serial number string index
  1,  // number of configurations
};

#define SERIAL_NUMBER_BYTE_COUNT (EEPROM_SERIAL_LENGTH * sizeof(int))
int webUsbDescriptorStringSerialNumber[EEPROM_SERIAL_LENGTH + 1] = {
  USB_STRING_DESCRIPTOR_HEADER(EEPROM_SERIAL_LENGTH)
};

USB_PUBLIC usbMsgLen_t usbFunctionDescriptor(usbRequest_t *rq) {
  switch (rq->wValue.bytes[1]) {
  case USBDESCR_STRING:
    switch (rq->wValue.bytes[0]) {
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

uint8_t maybeHandleSetup(usbRequest_t* rq, usbMsgLen_t* msg_len) {
  if (rq->bmRequestType ==
      (USBRQ_DIR_DEVICE_TO_HOST | USBRQ_TYPE_VENDOR | USBRQ_RCPT_DEVICE) &&
      rq->bRequest == WL_REQUEST_WEBUSB) {
    if (rq->wIndex.word == WEBUSB_REQUEST_GET_ALLOWED_ORIGINS) {
      usbMsgPtr = (usbMsgPtr_t)(WEBUSB_ALLOWED_ORIGINS);
      *msg_len = sizeof(WEBUSB_ALLOWED_ORIGINS);
      return 1;
    }
    if (rq->wIndex.word == WEBUSB_REQUEST_GET_LANDING_PAGE) {
      usbMsgPtr = (usbMsgPtr_t)(WEBUSB_LANDING_PAGE);
      *msg_len = sizeof(WEBUSB_LANDING_PAGE);
      return 1;
    }
  }
  return 0;
}

void forceReset() {
  StatusBlink(3);
  wdt_enable(WDTO_15MS);
  for(;;)
    ;
}

static uchar buffer[8];
static uchar currentPosition, bytesRemaining;
usbMsgLen_t usbFunctionSetup(uchar data[8]) {
  usbRequest_t    *rq = (void *)data;
  static uchar    dataBuffer[4];

  usbMsgLen_t msg_len = 0;
  if (maybeHandleSetup(rq, &msg_len)) {
    return msg_len;
  }

  usbMsgPtr = (int)dataBuffer;
  switch (rq->bRequest) {
  case WL_REQUEST_ECHO:
    dataBuffer[0] = rq->wValue.bytes[0];
    dataBuffer[1] = rq->wValue.bytes[1];
    dataBuffer[2] = rq->wIndex.bytes[0];
    dataBuffer[3] = rq->wIndex.bytes[1];
    return 4;
  case WL_REQUEST_SET_RGB:
    currentPosition = 0;
    bytesRemaining = rq->wLength.word;
    if (bytesRemaining > sizeof(buffer)) {
      bytesRemaining = sizeof(buffer);
    }
    return USB_NO_MSG;
  case WL_REQUEST_SET_LED_COUNT: {
    uint8_t count = rq->wValue.bytes[0];
    SetLEDCount(count);
    WriteLEDCount();
    return count;
  }
  case WL_REQUEST_RESET_DEVICE:
    forceReset();
    break;
  }

  return 0;
}

uchar usbFunctionWrite(uchar *data, uchar len) {
  uchar i;

  if (len > bytesRemaining) {
    len = bytesRemaining;
  }
  bytesRemaining -= len;
  for (i = 0; i < len; i++) {
    buffer[currentPosition++] = data[i];
  }

  if (bytesRemaining == 0) {
    SetLEDs(buffer[0], buffer[1], buffer[2]);
  }

  return bytesRemaining == 0;             // return 1 if we have all data
}
