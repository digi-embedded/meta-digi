/*
 * gpio.c
 *
 * Copyright (C)2006-2008 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/poll.h>
#include <linux/gpio.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include <asm/gpio.h>

#include "gpio.h"

#define	DRIVER_VERSION		"2.1"
#define	DRIVER_NAME		"gpio"

static int gpio_major = 240;	/* set to 0 for dynamic allocation */
static int gpio_minor = 0;
static int gpio_nr_devs;
static struct gpio_dev *gpio_device = NULL;

static int gpio_ioctl(struct inode *inode, struct file *file,
		unsigned int cmd, unsigned long arg );
static unsigned int gpio_poll(struct file *file, poll_table *wait );
static ssize_t gpio_write(struct file * file, const char * buf,
		size_t count, loff_t * ppos );
static ssize_t gpio_read(struct file * file, char * buf,
		size_t count, loff_t * ppos );
static int gpio_open(struct inode * inode, struct file * file );
static int gpio_release_close(struct inode * inode, struct file * file );

static struct file_operations gpio_fops = {
	.owner		= THIS_MODULE,
	.read		= gpio_read,
	.write		= gpio_write,
	.ioctl		= gpio_ioctl,
	.poll		= gpio_poll,
	.open		= gpio_open,
	.release	= gpio_release_close,
};

struct gpio_conf {
	int			dev_id;		/* minor */
	int			irqnum;		/* irq number assigned */

	/* needed for irq-waiting */
	wait_queue_head_t	wait_q;
	int			triggered;
};

struct gpio_dev {
	struct cdev		cdev;
	struct gpio_conf	*gpio;
};

static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
	struct gpio_conf *gpio = dev_id;

	/* awake those processes waiting... */
	gpio->triggered = 1;
	wake_up_interruptible(&gpio->wait_q);

	return IRQ_HANDLED;
}

static int gpio_ioctl(struct inode *inode,
                             struct file *file,
                             unsigned int cmd,
                             unsigned long arg )
{
	struct gpio_conf *gpio = (struct gpio_conf *)file->private_data;
	unsigned long irqflags = 0;
	int gpionum = MINOR(inode->i_rdev);
	char __user *argp = (char __user *)arg;
	int retval = 0, irqnum;

	if(_IOC_TYPE(cmd)!= GPIO_IOCTL_BASE )return -ENOTTY;
	if(_IOC_NR(cmd)> GPIO_IOCTL_MAXNR )return -ENOTTY;

	if (gpionum != gpio->dev_id)
		return -EINVAL;

	switch(cmd ){
		case GPIO_CONFIG_AS_INP:  /* config as input */
			if(gpio->irqnum >= 0 ){
				free_irq(gpio->irqnum, gpio );
				gpio->irqnum = -1;
			}
			retval = gpio_direction_input((unsigned int)gpionum);
			break;

		case GPIO_CONFIG_AS_OUT:  /* config as output */
			if(gpio->irqnum >= 0 ){
				free_irq(gpio->irqnum, gpio );
				gpio->irqnum = -1;
			}
			retval = gpio_direction_output((unsigned int)gpionum,
					gpio_get_value((unsigned int)gpionum));
			break;

		case GPIO_READ_PIN_VAL:  /* read value of the selected pin */
			retval = gpio_get_value((unsigned int)gpionum);
			gpio->triggered = 0;
			if (copy_to_user(argp, &retval, sizeof(int)))
				return -EINVAL;
			break;

		case GPIO_WRITE_PIN_VAL:  /* write value to the selected pin */
			gpio_set_value((unsigned int)gpionum, *argp);
			break;

		case GPIO_CONFIG_AS_IRQ:  /* Configure this pin as external interrupt line */
			if(gpio->irqnum >= 0 )
				break;
			retval = gpio_direction_input((unsigned int)gpionum);
			irqnum = gpio_to_irq(gpionum);
			if (irqnum >= 0){
				gpio->irqnum = irqnum;

				init_waitqueue_head(&gpio->wait_q );
				gpio->triggered = 0;

				switch( (ext_irq_type_t)*argp ) {
					case IRQ_HIGH:
						irqflags = IRQ_TYPE_LEVEL_HIGH;
						break;
					case IRQ_LOW:
						irqflags = IRQ_TYPE_LEVEL_LOW;
						break;
					case IRQ_RISING:
						irqflags = IRQ_TYPE_EDGE_RISING;
						break;
					case IRQ_FALLING:
						irqflags = IRQ_TYPE_EDGE_FALLING;
						break;
					default:
						return -EINVAL;
				}

				if((retval = request_irq(gpio->irqnum, gpio_irq_handler,
							    irqflags, DRIVER_NAME, gpio ))!= 0 ){
					printk(KERN_ERR "Unable to request irq %d, ret %d\n", gpio->irqnum, retval );
					gpio->irqnum = -1;
					return retval;
				}
			}
			break;
#if defined(CONFIG_MACH_CC9M2443JS) || defined(CONFIG_MACH_CCW9M2443JS)
		case GPIO_CONFIG_PULLUPDOWN:  /* Configure this pin pull up/down */
			gpio_set_pullupdown((unsigned int)gpionum, (int)arg);
			break;
#endif
		default:	return -ENOTTY;
	} /* switch(cmd )*/

	return retval;
}

static unsigned int gpio_poll(struct file *file, poll_table *wait )
{
	unsigned int ret = 0;
	struct gpio_conf *gpio = (struct gpio_conf *)file->private_data;

	poll_wait(file, &gpio->wait_q, wait);

	if (file->f_mode & FMODE_WRITE)
		ret |= POLLOUT | POLLWRNORM;
	if (gpio->triggered)
		ret |= POLLOUT | POLLRDNORM;

	return ret;
}

static ssize_t gpio_write(struct file * file, const char * buf, size_t count,
                                 loff_t * ppos)
{
	struct gpio_conf *gpio = (struct gpio_conf *)file->private_data;
	int gpionum = MINOR(file->f_dentry->d_inode->i_rdev);
	char outval;

	if ((gpionum != gpio->dev_id)|| (count != sizeof(char)))
		return -EINVAL;

	if (copy_from_user(&outval, buf, sizeof(char)))
		return -EFAULT;

	gpio_set_value(gpionum, (unsigned int)outval);

	return sizeof(char);
}

static ssize_t gpio_read(struct file * file, char * buf, size_t count,
                                loff_t * ppos)
{
	struct gpio_conf *gpio = (struct gpio_conf *)file->private_data;
	int gpionum = MINOR(file->f_dentry->d_inode->i_rdev);
	int ret;

	if ((gpionum != gpio->dev_id)|| (count != sizeof(char)))
		return -EINVAL;

	/* Check if configured for interrupt operation... */
	if (gpio->irqnum >= 0){
		/* Wait for data */
		if (!(file->f_flags & O_NONBLOCK)){
			wait_event_interruptible(gpio->wait_q, gpio->triggered != 0);
			if (signal_pending(current))
				return -ERESTARTSYS;
		}
		gpio->triggered = 0;
	}

	ret = gpio_get_value((unsigned int)gpionum);

	if (copy_to_user(buf, (char *)&ret, sizeof(char)))
		return -EFAULT;

	return sizeof(char);
}

static int gpio_open(struct inode * inode, struct file * file)
{
	int gpionum = MINOR(inode->i_rdev);
	int ret;
	struct gpio_dev *dev =
		container_of(inode->i_cdev, struct gpio_dev, cdev);

	if (gpionum != dev->gpio[gpionum].dev_id)
		return -EINVAL;

	ret = gpio_request(gpionum, DRIVER_NAME);
	if (ret < 0)
		return ret;

	file->private_data = &dev->gpio[gpionum];

	return 0;
}

static int gpio_release_close(struct inode * inode, struct file * file)
{
	struct gpio_conf *gpio = (struct gpio_conf *)file->private_data;
	int gpionum = MINOR(inode->i_rdev);

	if (gpionum != gpio->dev_id)
		return -EINVAL;

	if (gpio->irqnum >= 0){
		free_irq(gpio->irqnum, gpio);
		gpio->irqnum = -1;
	}

	gpio_free(gpionum);

	return 0;
}

static void gpio_exit(void)
{
	int i;
	dev_t devno = MKDEV(gpio_major, gpio_minor);

	if (gpio_device){
		for (i = 0; i < gpio_nr_devs; i++){
			if (gpio_device->gpio[i].irqnum != -1)
				free_irq(gpio_device->gpio[i].irqnum,
						&gpio_device->gpio[i]);
			cdev_del(&gpio_device->cdev);
		}
		kfree(gpio_device);
	}
	unregister_chrdev_region(devno, gpio_nr_devs);
}

static int gpio_init(void)
{
	int ret, i;
	dev_t dev;

	gpio_nr_devs = 256;

	if (gpio_major){
		dev = MKDEV(gpio_major, gpio_minor);
		ret = register_chrdev_region(dev, gpio_nr_devs, DRIVER_NAME);
	} else {
		ret = alloc_chrdev_region(&dev, gpio_minor,
				gpio_nr_devs, DRIVER_NAME);
		gpio_major = MAJOR(dev);
	}

	if (ret < 0){
		pr_err(DRIVER_NAME ": major %d allready in use \n", gpio_major);
		return ret;
	}

	gpio_device = kzalloc(sizeof(*gpio_device), GFP_KERNEL);
	if (!gpio_device){
		ret = -ENOMEM;
		goto fail;
	}

	gpio_device->gpio =
		kzalloc(sizeof(*gpio_device->gpio)* gpio_nr_devs, GFP_KERNEL);
	if (!gpio_device->gpio){
		ret = -ENOMEM;
		goto fail;
	}

	cdev_init(&gpio_device->cdev, &gpio_fops);
	gpio_device->cdev.owner = THIS_MODULE;
	gpio_device->cdev.ops = &gpio_fops;
	if (cdev_add(&gpio_device->cdev, dev, gpio_nr_devs)){
		pr_err(DRIVER_NAME ": Error adding cdev\n");
		ret = -ENODEV;
		goto fail;
	}

	for (i = 0; i < gpio_nr_devs; i++){
		gpio_device->gpio[i].dev_id = i;
		gpio_device->gpio[i].irqnum = -1;
	}

	pr_info(DRIVER_NAME ": GPIO driver v%s\n", DRIVER_VERSION);
	return 0;

fail:
	gpio_exit();
	return ret;
}

MODULE_AUTHOR("Digi International Inc.");
MODULE_DESCRIPTION("GPIO driver for the user space");
MODULE_LICENSE("GPL v2");

module_init(gpio_init);
module_exit(gpio_exit);
