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

static int timeout = 500;
module_param(timeout, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(timeout, "Set the read timeout for the pico rng usb device");

/**
 * The main data structure for this module
 **/
struct pico_rng_data {
	struct usb_device                      *dev;
	struct usb_interface                   *interface;
	struct usb_endpoint_descriptor         *endpoint;
	char                                   *buffer;
	int                                    pipe;
} module_data;

/**
 * Prototype Functions
 **/
static int pico_rng_usb_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void pico_rng_usb_disconnect(struct usb_interface *interface);
static int __init pico_rng_driver_init(void);
static void __exit pico_rng_driver_exit(void);

static ssize_t pico_rng_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset);
static int pico_rng_open(struct inode *inode, struct file *file);

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
static struct file_operations pico_rng_fops = {
	.owner          = THIS_MODULE,
	.read           = pico_rng_read,
	.open           = pico_rng_open,
};

/**
 * USB class data structure
 **/
 struct usb_class_driver pico_rng_usb_class = {
	.name           = "pico_rng",
	.fops           = &pico_rng_fops,
};

/**
 * File ops:open
 **/
static int pico_rng_open(struct inode *inode, struct file *file)
{
	LOGGER_DEBUG("inside pico_rng_open with inode %p file %p\n", inode, file);
	return 0;
}

/**
 * File ops:read
 **/
static ssize_t pico_rng_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset)
{
	int actual_length = 0;
	int retval = 0;

	LOGGER_DEBUG("inside pico_rng_read with file %p, user_buffer %p, size %ld, offset %lld\n", file, user_buffer, size, *offset);

    // int usb_bulk_msg(struct usb_device *usb_dev, unsigned int pipe, void *data, int len, int *actual_length, int timeout)
	LOGGER_DEBUG("Calling usb_bulk_msg dev %p, pipe %d, buffer %p, size %d, and timeout %d", \
	        module_data.dev, module_data.pipe, module_data.buffer, module_data.endpoint->wMaxPacketSize, timeout);

    retval = usb_bulk_msg(module_data.dev, 
	                      module_data.pipe,
						  module_data.buffer,
						  module_data.endpoint->wMaxPacketSize,
						  &actual_length,
						  timeout);

    if(retval)
	{
		return -EFAULT;
	}

    LOGGER_DEBUG("Copying %d bytest of random data to userspace with offset %lld\n", actual_length, *offset);
	if(copy_to_user(user_buffer, module_data.buffer, module_data.endpoint->wMaxPacketSize))
	{
        return -EFAULT;
	}

    return module_data.endpoint->wMaxPacketSize;
}

/**
 * USB: Probe
 * This method will be called if the device we plug in matches the vid:pid we are listening for
 **/
static int pico_rng_usb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	int retval = -ENODEV;

    module_data.dev = interface_to_usbdev(interface);
	if(!module_data.dev)
	{
		LOGGER_ERR("Unable to locate usb device");
		return retval;
	}

	module_data.interface = interface;
	if(!module_data.interface)
	{
		LOGGER_ERR("Invalid interface");
		return retval;
	}

	retval = usb_find_bulk_in_endpoint(module_data.interface->cur_altsetting, &(module_data.endpoint));
	if(retval)
	{
        LOGGER_ERR("Unable to find bulk endpoint %d\n", retval);
		return(retval);
    }

	module_data.pipe = usb_rcvbulkpipe(module_data.dev, module_data.endpoint->bEndpointAddress);

	LOGGER_DEBUG("endpoint found %p with pipe %d", module_data.endpoint, module_data.pipe);

    module_data.buffer = NULL;
	module_data.buffer = kmalloc(module_data.endpoint->wMaxPacketSize, GFP_KERNEL);
	if(!module_data.buffer)
	{
        LOGGER_ERR("failed to allocate buffer");
		return -1;
	}

	retval = usb_register_dev(module_data.interface, &pico_rng_usb_class);
	if(retval)
	{
		LOGGER_ERR("not able to get a minor for this device\n");
		return -1;
	}

    // int usb_bulk_msg(struct usb_device *usb_dev, unsigned int pipe, void *data, int len, int *actual_length, int timeout)
    //retval = usb_bulk_msg(dev, pipe, buffer, 64, &actual_length, 500);

	return retval;
}

/**
 * USB:disconnect
 * This method is used to cleanup everything after the device is disconnected
 **/
static void pico_rng_usb_disconnect(struct usb_interface *interface)
{
	LOGGER_INFO("pico rng usb device disconnected\n");
	usb_deregister_dev(module_data.interface, &pico_rng_usb_class);
	module_data.dev = NULL;
	module_data.interface = NULL;
	module_data.pipe = 0;
	kfree(module_data.buffer);
	module_data.buffer = NULL;
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

	return 0;
}

static void __exit pico_rng_driver_exit(void)
{
	usb_deregister(&pico_rng_usb_driver);

	if(module_data.buffer)
	{
		kfree(module_data.buffer);
	}

	return;
}

module_init(pico_rng_driver_init);
module_exit(pico_rng_driver_exit); 

