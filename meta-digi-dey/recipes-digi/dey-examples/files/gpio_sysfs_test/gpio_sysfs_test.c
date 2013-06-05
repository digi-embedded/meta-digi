/*
 * gpio_sysfs_test.c
 *
 * Copyright (C) 2011 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: GPIO SYSFS test application
 *
 */

#include "sysfsgpio.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

static int run = 1;

#define ABORT_ON_ERROR(fn) if( fn < 0 ) abort()
#define RETURN_ON_ERROR(fn) if( fn < 0 ) return

void signal_handler(int sig)
{
	switch (sig) {
	case SIGHUP:
		printf("hangup signal catched\n");
		break;
	case SIGTERM:
		printf("terminate signal catched\n");
		break;
	case SIGINT:
		printf("Interrupt signal catched\n");
		break;
	default:
		printf("signal %d catched\n", sig);
		break;
	}
	run = 0;
}

void print_settings(unsigned int gpio)
{
	unsigned int active_low;
	char edge[MAX_LEN];

	sysfs_gpio_get_active_low(gpio, &active_low);
	sysfs_gpio_get_edge(gpio, edge, sizeof(edge));
	printf("GPIO %d: Active %s and %s edge\n", gpio, active_low ? "low"
	        : "high", edge);
}

int gpio_sysfs_test_loop(unsigned int gpioin, unsigned int gpiout, int loops)
{
	int i;
	int value = 0;

	if (!run)
		return -1;

	print_settings(gpioin);
	printf("Press the button (for %d events):\n",loops);
	for (i = 0; (i < loops) && run; i++) {
		ABORT_ON_ERROR (sysfs_gpio_poll(gpioin,-1 /* no timeout */));
		ABORT_ON_ERROR(sysfs_gpio_set_value(gpiout,!value));
		value = !value;
		printf("Press %d\n", i+1);
	}
	return 0;
}

void poll_test(unsigned int gpioin, unsigned int gpioout)
{
	char buf[MAX_LEN]="";

	ABORT_ON_ERROR ( sysfs_gpio_set_direction( gpioin , 0 /* in */) );
	ABORT_ON_ERROR ( sysfs_gpio_set_direction( gpioout , 1 /* out */) );

	// Active low and rising edge
	ABORT_ON_ERROR( sysfs_gpio_set_active_low( gpioin, 1 /*low*/) );
	ABORT_ON_ERROR ( sysfs_gpio_set_edge( gpioin, "rising") );
	RETURN_ON_ERROR( gpio_sysfs_test_loop(gpioin,gpioout,10) );

	// Active low and falling edge
	ABORT_ON_ERROR ( sysfs_gpio_set_edge( gpioin, "falling") );
	RETURN_ON_ERROR( gpio_sysfs_test_loop(gpioin,gpioout,10) );

	// Active low and both edges
	ABORT_ON_ERROR ( sysfs_gpio_set_edge( gpioin, "both") );
	sysfs_gpio_get_edge( gpioin , buf, sizeof(buf) );
	if( !strcmp(buf,"both") )
		RETURN_ON_ERROR( gpio_sysfs_test_loop(gpioin,gpioout,10) );

	// Active high and rising edge
	ABORT_ON_ERROR( sysfs_gpio_set_active_low( gpioin, 0 /*high*/) );
	ABORT_ON_ERROR ( sysfs_gpio_set_edge( gpioin, "rising") );
	RETURN_ON_ERROR( gpio_sysfs_test_loop(gpioin,gpioout,10) );

	// Active high and falling edge
	ABORT_ON_ERROR ( sysfs_gpio_set_edge( gpioin, "falling") );
	RETURN_ON_ERROR( gpio_sysfs_test_loop(gpioin,gpioout,10) );

	// Active high and both edges
	ABORT_ON_ERROR ( sysfs_gpio_set_edge( gpioin, "both") );
	sysfs_gpio_get_edge( gpioin , buf, sizeof(buf) );
	if( !strcmp(buf,"both") )
		RETURN_ON_ERROR( gpio_sysfs_test_loop(gpioin,gpioout,10) );
}

int main(int argc, char **argv, char **envp)
{
	int gpioin;
	int gpioout = -1;

	if (argc < 2) {
		printf("Usage: gpio-sysfs-test <gpio_in> [gpio_out]\n\n");
		printf("Where gpio_in is a pushbutton and gpio_out an optional LED.\n");
		exit(-1);
	}

	gpioin = atoi(argv[1]);
	if(argv[2])
		gpioout = atoi(argv[2]);

	ABORT_ON_ERROR (sysfs_gpio_export(gpioin) );
	ABORT_ON_ERROR (sysfs_gpio_export(gpioout) );

	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGHUP, signal_handler);

	poll_test(gpioin, gpioout);

	sysfs_gpio_unexport(gpioin);
	sysfs_gpio_unexport(gpioout);
	return 0;
}
