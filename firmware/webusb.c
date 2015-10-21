#include <avr/pgmspace.h>

#include "usbdrv.h"

#define USB_BOS_DESCRIPTOR_TYPE (15)

PROGMEM const char BOS_DESCRIPTOR[29] = {
  // BOS descriptor header.
  0x05, 0x0F, 0x1C, 0x00, 0x01,

  // WebUSB Platform Capability descriptor (bVendorCode == 0x01).
  0x17, 0x10, 0x05, 0x00, 0x38, 0xB6, 0x08, 0x34, 0xA9, 0x09, 0xA0, 0x47,
  0x8B, 0xFD, 0xA0, 0x76, 0x88, 0x15, 0xB6, 0x65, 0x00, 0x01, 0x01,
};

PROGMEM const char WEBUSB_LANDING_PAGE[41] = {
  0x29, 0x03, 'h', 't', 't', 'p', 's', ':', '/', '/', 'r', 'e', 'i', 'l', 'l',
  'y', 'e', 'o', 'n', '.', 'g', 'i', 't', 'h', 'u', 'b', '.', 'i', 'o', '/',
  'w', 'e', 'b', 'u', 's', 'b', '/', 'd', 'e', 'm', 'o'
};

PROGMEM const char WEBUSB_ALLOWED_ORIGINS[33] = {
  0x04, 0x00, 0x21, 0x00, 0x1D, 0x03, 'h', 't', 't', 'p', 's', ':', '/', '/',
  'r', 'e', 'i', 'l', 'l', 'y', 'e', 'o', 'n', '.', 'g', 'i', 't', 'h', 'u',
  'b', '.', 'i', 'o'
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
  USB_CFG_DESCR_PROPS_STRING_VENDOR != 0 ? 1 : 0,         /* manufacturer string index */
  USB_CFG_DESCR_PROPS_STRING_PRODUCT != 0 ? 2 : 0,        /* product string index */
  USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER != 0 ? 3 : 0,  /* serial number string index */
  1,          /* number of configurations */
};

#define WEBUSB_REQUEST_GET_ALLOWED_ORIGINS			0x01
#define WEBUSB_REQUEST_GET_LANDING_PAGE					0x02
USB_PUBLIC usbMsgLen_t usbFunctionDescriptor(usbRequest_t *rq) {
  switch (rq->wValue.bytes[1]) {
  case USB_BOS_DESCRIPTOR_TYPE:
    usbMsgPtr = (usbMsgPtr_t)(BOS_DESCRIPTOR);
    return sizeof(BOS_DESCRIPTOR);
  default:
    break;
  }
  return 0;
}
