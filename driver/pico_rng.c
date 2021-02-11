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
#define VENDOR_ID             0x0
#define PRODUCT_ID            0x4
#define SC_MINOR_BASE         31

/**
 * Helper macros
 **/
#define LOGGER_INFO(fmt, args ...) printk( KERN_INFO "[info]  %s(%d): " fmt, __FUNCTION__, __LINE__, ## args)
#define LOGGER_ERR(fmt, args ...) printk( KERN_ERR "[err]  %s(%d): " fmt, __FUNCTION__, __LINE__, ## args)
#define LOGGER_WARN(fmt, args ...) printk( KERN_ERR "[warn]  %s(%d): " fmt, __FUNCTION__, __LINE__, ## args)
#define LOGGER_DEBUG(fmt, args ...) if (debug == 1) { printk( KERN_DEBUG "[debug]  %s(%d): " fmt, __FUNCTION__, __LINE__, ## args); }

static int debug = 0;
module_param(debug, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(debug, "Set the log level to debug");

/**
 * Prototype Functions
 **/
static int pico_rng_usb_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void pico_rng_usb_disconnect(struct usb_interface *interface);
static int __init pico_rng_driver_init(void);
static void __exit pico_rng_driver_exit(void);

/**
static int pico_rng_open(struct inode *inode, struct file *file);
static int pico_rng_release(struct inode *inode, struct file *file);
static ssize_t pico_rng_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos);
static ssize_t pico_rng_read(struct file *file, char __user *user, size_t count, loff_t *ppos);
**/


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
 * File operations data structure for the character device
 * that will be implemented in this module
 **/
/**static struct file_operations pico_rng_fops = {
	.owner          = THIS_MODULE,
	.open           = pico_rng_open,
	.read           = pico_rng_read,
	.release        = pico_rng_release,
};
**/

/**
 * USB: Probe
 * This method will be called if the device we plug in matches the vid:pid we are listening for
 **/
static int pico_rng_usb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device              *dev = interface_to_usbdev(interface);
	struct usb_endpoint_descriptor *endpoint = NULL;
	int                            retval = -ENODEV;
	char                           *buffer;
	int                            actual_length;
	int                            pipe = 0;

	retval = usb_find_bulk_in_endpoint(interface->cur_altsetting, &endpoint);
	if(retval)
	{
        LOGGER_ERR("Unable to find bulk endpoint %d\n", retval);
		return(retval);
    }

	pipe = usb_rcvbulkpipe(dev, endpoint->bEndpointAddress);

	LOGGER_DEBUG("endpoint found %p with pipe %d", endpoint, pipe);

	buffer = kmalloc(endpoint->wMaxPacketSize, GFP_KERNEL);

    // int usb_bulk_msg(struct usb_device *usb_dev, unsigned int pipe, void *data, int len, int *actual_length, int timeout)
    retval = usb_bulk_msg (dev,
                           pipe,
                           buffer,
                           64,
                           &actual_length,
						   5000);

    /* if the read was successful, copy the data to user space */
    if (!retval)
	{
        LOGGER_INFO("%s\n", buffer);
    }
	else
	{
		LOGGER_INFO("usb_bulk_msg raised an error %d\n", retval);
	}

	kfree(buffer);

	return retval;
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
	int retval = 0;
   	LOGGER_INFO("pico rng driver debut\n");
	
	retval = usb_register(&pico_rng_usb_driver);
	if(retval)
	{
		LOGGER_ERR("registering pico rng driver failed\n");
		return retval;
	}
	else
	{
		LOGGER_INFO("pico rng driver registered successfully\n");
	}

	return retval;
}

static void __exit pico_rng_driver_exit(void)
{
	usb_deregister(&pico_rng_usb_driver);
	return;
}

module_init(pico_rng_driver_init);
module_exit(pico_rng_driver_exit); 

