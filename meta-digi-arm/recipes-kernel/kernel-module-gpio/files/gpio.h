/*
 * gpio.h
 *
 * Copyright (C) 2006 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#ifndef	__DIGI_GPIO_H_
#define __DIGI_GPIO_H_

typedef enum {
	IRQ_HIGH,
	IRQ_LOW,
	IRQ_RISING,
	IRQ_FALLING
}ext_irq_type_t;

#define PULLUP                  0x00
#define PULLDOWN                0x02
#define PULLUPDOWN_DISABLED     0x03

/* ioctl magic numbers */
#define GPIO_IOCTL_BASE		'G'

/* inputs */
#define GPIO_CONFIG_AS_INP	_IO  (GPIO_IOCTL_BASE, 0)	/* config this pin as input */
#define GPIO_READ_PIN_VAL	_IOR (GPIO_IOCTL_BASE, 1, int)	/* read pin value */

/* outputs */
#define GPIO_CONFIG_AS_OUT	_IO  (GPIO_IOCTL_BASE, 2)	/* config this pin as output */
#define GPIO_WRITE_PIN_VAL	_IOW (GPIO_IOCTL_BASE, 3, int)	/* sets the pin value */

/* irqs */
#define GPIO_CONFIG_AS_IRQ	_IOR (GPIO_IOCTL_BASE, 4, ext_irq_type_t)	/* config this pin as interrupt */

/* pull up/down */
#define GPIO_CONFIG_PULLUPDOWN  _IOW (GPIO_IOCTL_BASE, 5, int)   /* config this pin pull up/down resistor */

#define GPIO_IOCTL_MAXNR	5

#endif /* __DIGI_GPIO_H_ */
