#!/usr/bin/python

# sudo pip install pyusb==1.0.0b2

from sys import argv
import binascii
import sys
import usb.core
import usb.util

dev = usb.core.find(idVendor=0x16c0, idProduct=0x05dc)
if dev is None:
    raise ValueError('Our device is not connected')

if len(argv) < 2:
    print 'usage: %s <6-digit hex rgb color>' % argv[0]
    sys.exit(1)

bytes = binascii.unhexlify(('000000' + argv[1])[-6:])

result = dev.ctrl_transfer(0x40, 1, 0, 0, bytes)
if result != len(bytes):
    raise IOError('Error', result)
