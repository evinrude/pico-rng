#!/usr/bin/env python3

import usb.core
import usb.util
import random
import time
import argparse

# Parser stuff
parser = argparse.ArgumentParser(description="Raspberry Pi Pico Random Number Generator Test Tool")
parser.add_argument("--performance", action="store_true", help="Performance test the RNG.")
args = parser.parse_args()

# Get the device
rng = usb.core.find(idVendor=0x0000, idProduct=0x0004)
assert rng is not None

# Get the configuration of the device
cfg = rng.get_active_configuration()

# Get the only interface of our device
intf = cfg.interfaces()[0]

# Get the endpoint
endpt = intf.endpoints()[0]

# Time tracking for bits/s
count = 0
start_time = (int(time.time()) - 1)

if args.performance:
    while True:
        try:
            from_device = endpt.read(endpt.wMaxPacketSize, 500)
            count = count+1
            print(":".join("{:02x}".format(b) for b in from_device), end="")
            print(" KBps {0:.2f}".format((int((count * 64) / (int(time.time()) - start_time))) / 1024 ))
        except KeyboardInterrupt:
            exit(0)
else:
    print(":".join("{:02x}".format(b) for b in endpt.read(endpt.wMaxPacketSize, 500)))
