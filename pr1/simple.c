/**
 * simple.c
 *
 * A simple kernel module. 
 * 
 * To compile, run makefile by entering "make"
 *
 * Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <asm/param.h>
#include <linux/jiffies.h>

unsigned long initJiffies;
unsigned long finalJiffies;
/* This function is called when the module is loaded. */
int simple_init(void)
{
	   initJiffies = jiffies;
       printk(KERN_INFO "Loading Module\n");
	   printk(KERN_INFO "%lu\n", GOLDEN_RATIO_PRIME);
	   printk("%d\n", HZ);
	   printk("%d\n", initJiffies);

       return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
	finalJiffies = jiffies;
	printk(KERN_INFO "Removing Module\n");
	unsigned long g = gcd(3300, 24);
	printk ("%lu\n",g);
	printk("%d\n", finalJiffies);
	printk("seconds:%d\n", (finalJiffies - initJiffies) / HZ);

}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

