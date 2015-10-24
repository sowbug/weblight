#!/usr/bin/python

# sudo pip install pyusb==1.0.0b2

from sys import argv
import binascii
import sys
import usb.core
import usb.util

if len(argv) < 2:
    print 'usage: %s <6-digit hex rgb color>' % argv[0]
    sys.exit(1)

VENDOR = 0x16c0
PRODUCT = 0x05dc
getDescriptors = False
if argv[1] == 'weblight':
    REQUEST_ID = 3
    getDescriptors = True
if argv[1] == 'leonardo':
    VENDOR = 0x2341
    PRODUCT = 0x8036
    REQUEST_ID = 1
    getDescriptors = True

dev = usb.core.find(idVendor=VENDOR, idProduct=PRODUCT)
if dev is None:
    raise ValueError('Our device is not connected')

if getDescriptors:
    print dev

# bmRequestType
# USBRQ_DIR_HOST_TO_DEVICE | USBRQ_TYPE_VENDOR | USBRQ_RCPT_DEVICE = 0x40
# USBRQ_DIR_DEVICE_TO_HOST | USBRQ_TYPE_VENDOR | USBRQ_RCPT_DEVICE = 0xC0
# USBRQ_DIR_DEVICE_TO_HOST | USBRQ_TYPE_STANDARD | USBRQ_RCPT_DEVICE = 0x80

# bRequest = whatever you specified for your custom request
# wValue
# wIndex
# (optional) data

if getDescriptors:
    # Device descriptor
    result = dev.ctrl_transfer(0x80, 6, 0x0100, 0, 64)
    print "Device", len(result), binascii.hexlify(result)

    # Manufacturer String Descriptor
    result = dev.ctrl_transfer(0x80, 6, 0x0301, 0, 64)
    print "Manufacturer", len(result), binascii.hexlify(result)

    # Product String Descriptor
    result = dev.ctrl_transfer(0x80, 6, 0x0302, 0, 64)
    print "Product", len(result), binascii.hexlify(result)

    # Serial String Descriptor
    result = dev.ctrl_transfer(0x80, 6, 0x0303, 0, 64)
    print "Serial", len(result), binascii.hexlify(result)

    # BOS
    result = dev.ctrl_transfer(0x80, 6, 0x0f00, 0, 64)
    print "BOS", len(result), binascii.hexlify(result)

    WEBUSB_REQUEST_GET_ALLOWED_ORIGINS = 1
    WEBUSB_REQUEST_GET_LANDING_PAGE = 2
    result = dev.ctrl_transfer(0xC0, REQUEST_ID, 0,
                            WEBUSB_REQUEST_GET_ALLOWED_ORIGINS, 64)
    print len(result), binascii.hexlify(result)
    result = dev.ctrl_transfer(0xC0, REQUEST_ID, 0,
                            WEBUSB_REQUEST_GET_LANDING_PAGE, 64)
    print len(result), binascii.hexlify(result)
else:
    bytes = binascii.unhexlify(('000000' + argv[1])[-6:])
    result = dev.ctrl_transfer(0x40, 1, 0, 0, bytes)
    if result != len(bytes):
        raise IOError('Error', result)
