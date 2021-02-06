#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h> 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mickey Malone");
MODULE_DESCRIPTION("Random number generator using a Raspberry Pi Pico");
MODULE_VERSION("1.0");

static int __init driver_init(void)
{
    printk(KERN_INFO "Raspberry Pi Pico RNG\n");
    return 0;
}

static void __exit driver_exit(void)
{
	return;
}

module_init(driver_init);
module_exit(driver_exit); 

