#!/usr/bin/env python3

import usb.core
import usb.util
import os
import random
import time
import argparse

# Parser stuff
parser = argparse.ArgumentParser(description="Raspberry Pi Pico Random Number Generator Test Tool")
parser.add_argument("--performance", action="store_true", help="Performance test the RNG.")
args = parser.parse_args()

# If this is set, then the /dev/pico_rng file exists
rng_chardev = None

if os.path.exists("/dev/pico_rng"):
    rng_chardev = open("/dev/pico_rng", "rb")
    
# File does not exist, test with usb.core
if not rng_chardev:
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
            from_device = rng_chardev.read(64) if rng_chardev else endpt.read(64, 500)
            count = count+1
            print(from_device, end="")
            print("\t{0:.2f} KB/s".format((int((count * 64) / (int(time.time()) - start_time))) / 1024 ))
        except KeyboardInterrupt:
            exit(0)
else:
    from_device = rng_chardev.read(64) if rng_chardev else endpt.read(64, 500)
    print(from_device)
