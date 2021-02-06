#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mickey Malone");
MODULE_DESCRIPTION("Random number generator using a Raspberry Pi Pico");
MODULE_VERSION("1.0");

/**
 * Static constants
 **/
#define VENDOR_ID                                       0x0
#define PRODUCT_ID                                      0x4
#define SC_MINOR_BASE                                   31

/**
 * Helper macros
 **/
#define LOGGER_INFO(fmt, args ...) printk( KERN_INFO "[info]  %s(%d): " fmt, __FUNCTION__, __LINE__, ## args)
#define LOGGER_ERR(fmt, args ...) printk( KERN_ERR "[err]  %s(%d): " fmt, __FUNCTION__, __LINE__, ## args)
#define LOGGER_WARN(fmt, args ...) printk( KERN_ERR "[warn]  %s(%d): " fmt, __FUNCTION__, __LINE__, ## args)
#define LOGGER_DEBUG(fmt, args ...) if (debug == 1) { printk( KERN_DEBUG "[debug]  %s(%d): " fmt, __FUNCTION__, __LINE__, ## args); }

/**
 * Prototype Functions
 **/
static int pico_rng_usb_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void pico_rng_usb_disconnect(struct usb_interface *interface);
static int __init pico_rng_driver_init(void);
static void __exit pico_rng_driver_exit(void);


/**
 * Data structure of the USB vid:pid device that we will support
 **/
static struct usb_device_id pico_rng_usb_table [] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{ }
};

/**
 * USB driver data structure
 **/
static struct usb_driver pico_rng_usb_driver = {
	.name           = "pico_rng",
	.id_table       = pico_rng_usb_table,
	.probe          = pico_rng_usb_probe,
	.disconnect     = pico_rng_usb_disconnect,
};

/**
 * USB: Probe
 * This method will be called if the device we plug in matches the vid:pid we are listening for
 **/
static int pico_rng_usb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	LOGGER_INFO("pico rng usb device initialized\n");

	// TODO: Setup USB device, create seed thread to continuously gather entropy from device.
	return 0;
}

/**
 * USB:disconnect
 * This method is used to cleanup everything after the device is disconnected
 **/
static void pico_rng_usb_disconnect(struct usb_interface *interface)
{
	LOGGER_INFO("pico rng usb device disconnected\n");
}


static int __init pico_rng_driver_init(void)
{
	int result;
   	LOGGER_INFO("pico rng driver debut\n");

	result = usb_register(&pico_rng_usb_driver);
	if(result)
	{
		LOGGER_ERR("registering pico rng driver failed\n");
	}
	else
	{
		LOGGER_INFO("pico rng driver registered successfully\n");
	}

	return result;

}

static void __exit pico_rng_driver_exit(void)
{
	usb_deregister(&pico_rng_usb_driver);
	return;
}

module_init(pico_rng_driver_init);
module_exit(pico_rng_driver_exit); 

