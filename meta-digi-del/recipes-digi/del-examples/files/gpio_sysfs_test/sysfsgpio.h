/*
 * sysfsgpio.h
 *
 * Copyright (C) 2011 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: GPIO SYSFS API header file.
 *
 */

#ifndef SYSFSGPIO_H_
#define SYSFSGPIO_H_

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_LEN 64

int sysfs_gpio_export(int gpio);
int sysfs_gpio_unexport(int gpio);

int sysfs_gpio_set_direction(int gpio, unsigned int out);
int sysfs_gpio_get_direction(int gpio, char * dir, int len);

int sysfs_gpio_set_active_low(int gpio, unsigned int low);
int sysfs_gpio_get_active_low(int gpio, unsigned int *value);

int sysfs_gpio_set_value(int gpio, unsigned int value);
int sysfs_gpio_get_value(int gpio, unsigned int *value);

int sysfs_gpio_set_edge(int gpio, char *edge);
int sysfs_gpio_get_edge(int gpio, char *edge, int len);

int sysfs_gpio_poll(int gpio, unsigned int timeout);

#endif /* SYSFSGPIO_H_ */
