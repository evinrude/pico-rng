# Raspberry Pi Pico Random Number Generator

A basic random number generator that generates numbers from enviromental noise with the onboard DAC of the Raspberry Pi Pico. The project uses the Raspberry Pi Pico USB dev_lowlevel as a starting point. The Pico RNG is not meant to be FIPS 140-2 compliant as a stand-alone device by any means. However it does supply the Linux Kernel with random bits which can be used with the appropriate entropy to supply FIPS 140-2 compliant random numbers. Maybe one day the next gen Pico's will include an onboard crypto module.

## Project Goals
* Raspberry Pi Pico firmware generates random numbers as a USB Endpoint.
* Linux Kernel Module (aka driver) provides random numbers to the Kernel.
* Driver can transmit random numbers on demand to the system and/or user processes via a character device.


### Prerequisites

* Raspberry Pi Pico development environment. See [Raspberry Pi Pico Getting Started Documentation](https://www.raspberrypi.org/documentation/pico/getting-started/)
* Linux Kernel development headers


### Building
The entire project uses CMake to keep with Rasberry Pi Pico's development environment and project setup instructions.

```bash
# Create build directory
mkdir build

# Change to the build directory
cd build

# Run cmake
cmake ..

# Run make
make
```

### Install

The driver can be installed from the build directory using the traditional insmod command.

```bash
# Assumes CWD is 'build/'
# debug will enable debug log level
# timeout will set the usb endpoint timeout. Currently defaults to 100 msecs
sudo insmod driver/pico_rng.ko [debug=1] [timeout=<msec timeout>]
```

The Pico firmware is installed thorugh the normal process as outlined in the Raspberry Pi Pico Development Documentation.

* Unplug the Pico from the host.
* Plug the Pico into the host while holding the 'boot' button.
* Mount the Pico ```sudo mount /dev/sdb1 /mnt```. Note /dev/sdb1 could be different you. Use ```sudo dmesg``` to find out what device the Pico shows up as on your system.
* Copy the uf2 file to the Pico ```sudo cp firmware/pico_rng.uf2 /mnt```.
* Umount the pico ```sudo umount /mnt```.

### Testing

You can test Pico RNG firmware with the [pico_rng_test.py](firmware/pico_rng_test.py) script.

```bash
# Running with --performance will measure the devices' KB/s.
# if the kernel module has been installed, then the test tool will use /dev/pico_rng otherwise python's libusb implementation will be used.
sudo firmware/pico_rng_test.py [--performance]
```

You can also test the Kernel's random number pool that contains random numbers from the Pico
![Pico Random Numbers](pico-rng.gif)


# Remove
```bash
sudo rmmod pico_rng
```

### License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

### References

* https://github.com/raspberrypi/pico-examples/tree/master/usb/device/dev_lowlevel
