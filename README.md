# Raspberry Pi Pico Random Number Generator

A basic random number generator that generates numbers from the onboard DAC of the Raspberry Pi Pico. The project used the Raspberry Pi Pico USB dev_lowlevel as a starting point. The RNG is not meant to be FIPS 140-2 compliant by a long shot. This is not meant to by used in a production system as a TRNG. Maybe one day the next gen Pico's will include an onboard crypto module.


## Project Goals
* Raspberry Pi Pico firmware generates random numbers as a USB Endpoint
* Linux Kernel Module (aka driver) provides random numbers to the Kernel
* Driver can transmit random numbers on demand to the system and/or user processes via a character device


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

```bash
# Assumes CWD is 'build/'
# debug will enable debug log level
# timeout will set the usb endpoint timeout. Currently defaults to 100 msecs
sudo insmod driver/pico_rng.ko [debug=1] [timeout=<msec timeout>]
```

### Testing

You can test Pico RNG firmware with the [pico_rng_test.py](firmware/pico_rng_test.py) script.

```bash
# Running with --performance will measure the devices' KB/s.
# if the kernel module is inserted, then the test tool will use /dev/pico_rng otherwise python's libusb implementation will be used.
sudo firmware/pico_rng_test.py [--performance]
```


# Remove
```bash
sudo rmmod pico_rng
```

### License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

### References

* https://github.com/raspberrypi/pico-examples/tree/master/usb/device/dev_lowlevel
