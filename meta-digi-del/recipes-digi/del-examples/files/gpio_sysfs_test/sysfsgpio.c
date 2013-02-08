/*
 * sysfsgpio.c
 *
 * Copyright (C) 2011 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: GPIO SYSFS API library.
 *
 */

#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "sysfsgpio.h"

int sysfs_gpio_export(int gpio)
{
	int fd, len;
	char buf[MAX_LEN];

	if( gpio < 0 )
		return 0;

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		perror("Error on GPIO export\n");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);

	return 0;
}

int sysfs_gpio_unexport(int gpio)
{
	int fd, len;
	char buf[MAX_LEN];

	if( gpio < 0 )
		return 0;

	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0) {
		perror("Error on GPIO unexport");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);

	return 0;
}

int sysfs_gpio_set_direction(int gpio, unsigned int out)
{
	int fd, len;
	char buf[MAX_LEN];

	if( gpio < 0 )
		return 0;

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/direction", gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("Error setting GPIO direction\n");
		return fd;
	}

	if (out)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);

	close(fd);
	return 0;
}

int sysfs_gpio_get_direction(int gpio, char * dir, int len)
{
	int fd, length;
	char buf[MAX_LEN];

	if( gpio < 0 )
		return 0;

	length = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/direction",
	        gpio);

	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		perror("Error getting GPIO direction\n");
		return fd;
	}

	memset(dir,0,len);
	read(fd, dir, len);
	dir[strlen(dir) - 1] = '\0';
	if (strcmp(dir, "out") && strcmp(dir, "in")) {
		perror("Error getting direction\n");
	}

	close(fd);
	return 0;
}

int sysfs_gpio_set_active_low(int gpio, unsigned int low)
{
	int fd, len;
	char buf[MAX_LEN];

	if( gpio < 0 )
		return 0;

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/active_low", gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("Error setting GPIO value\n");
		return fd;
	}

	snprintf(buf, sizeof(buf), "%s", low ? "1" : "0");
	write(fd, buf, 2);

	close(fd);
	return 0;
}

int sysfs_gpio_get_active_low(int gpio, unsigned int *value)
{
	int fd, len;
	char buf[MAX_LEN];
	char ch;

	if( gpio < 0 )
		return 0;

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/active_low", gpio);

	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		perror("Error getting GPIO value\n");
		return fd;
	}

	read(fd, &ch, 1);

	if (ch != '0') {
		*value = 1;
	}
	else {
		*value = 0;
	}

	close(fd);
	return 0;
}

int sysfs_gpio_set_value(int gpio, unsigned int value)
{
	int fd, len;
	char buf[MAX_LEN];

	if( gpio < 0 )
		return 0;

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("Error setting GPIO value\n");
		return fd;
	}
	snprintf(buf, sizeof(buf), "%s", value ? "1" : "0");
	write(fd, buf, 2);

	close(fd);
	return 0;
}

int sysfs_gpio_get_value(int gpio, unsigned int *value)
{
	int fd, len;
	char buf[MAX_LEN];
	char ch;

	if( gpio < 0 )
		return 0;

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		perror("Error getting GPIO value\n");
		return fd;
	}

	read(fd, &ch, 1);

	if (ch != '0') {
		*value = 1;
	}
	else {
		*value = 0;
	}

	close(fd);
	return 0;
}

static int sysfs_gpio_fset_value(int fd, unsigned int value)
{
	char buf[MAX_LEN];

	snprintf(buf, sizeof(buf), "%s", value ? "1" : "0");
	write(fd, buf, 2);

	return fd;
}

static int sysfs_gpio_fget_value(int fd, unsigned int *value)
{
	char buf;

	read(fd, &buf, sizeof(char));

	if (buf != '0') {
		*value = 1;
	}
	else {
		*value = 0;
	}

	return fd;
}

int sysfs_gpio_set_edge(int gpio, char *edge)
{
	int fd, len;
	char buf[MAX_LEN];

	if( gpio < 0 )
		return 0;

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("Error setting GPIO edge\n");
		return fd;
	}

	write(fd, edge, strlen(edge) + 1);
	close(fd);
	return 0;
}

int sysfs_gpio_get_edge(int gpio, char *edge, int len)
{
	int fd, length;
	char buf[MAX_LEN];

	if( gpio < 0 )
		return 0;

	length = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);

	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		perror("Error setting GPIO edge\n");
		return fd;
	}

	memset(edge,0,len);
	read(fd, edge, len);
	//Remove trailing newline
	edge[strlen(edge) - 1] = '\0';
	close(fd);
	return 0;
}

int sysfs_gpio_poll(int gpio, unsigned int timeout)
{
	int fd, len;
	char buf[MAX_LEN];
	struct pollfd pfd;
	int ret = 0;
	unsigned int inval;

	if( gpio < 0 )
		return 0;

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		perror("Error on GPIO open\n");
		ret = -1;
		goto error;
	}

	/* Edge triggers are relative to the last read by the application
	 * and not to the start of poll. Read here to avoid poll returning
	 * immediately.*/
	read(fd, &buf, sizeof(char));

	memset((void*) &pfd, 0, sizeof(pfd));
	pfd.fd = fd;
	pfd.events = POLLPRI;
	ret = poll(&pfd, 1, timeout);
	if (ret > 0) {

		if (pfd.revents & POLLNVAL) {
			printf("Return error event %x\n", pfd.revents);
			ret = -1;
		}
		else if ((pfd.revents & POLLPRI)) {
			sysfs_gpio_fget_value(fd, &inval);
		}
		else {
			printf("Unhandled event %x\n", pfd.revents);
			ret = -1;
		}
	}
	else if (ret == 0) {
		printf("Timeout on poll\n");
	}
	else {
		perror("Error on poll\n");
		ret = -1;
		;
	}

	close(fd);
	error: return ret;
}

