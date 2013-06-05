/*
 * gpio_test.c
 *
 * Copyright (C) 2009-2013 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: GPIO test application (needs gpio.ko external module)
 *
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "gpio.h"

#define PROGRAM		"gpio_test"
#define VERSION		"2.0"

#define MX5X_GPIO(port,pin)	((port - 1) * 32 + pin)
#define MX28_GPIO(port,pin)	(port * 32 + pin)

typedef enum {
	CCARDIMX28JS = 0,
	CCIMX51JS,
	CCIMX53JS,
	PLAT_NONE
} platform_e;

#define GPIO_UNDEFINED	0xffff

/* Global variables */
static platform_e platform = PLAT_NONE;

#define MK(x, name, button, led, gpio_in, gpio_out, irq_capable, pullup_in, pullup_descr) \
	[x] = {name, button, led, gpio_in, gpio_out, irq_capable, pullup_in, pullup_descr}
static struct {
	const char *name;
	const char *button;
	const char *led;
	int gpio_in;
	int gpio_out;
	int irq_capable;
	int pullup_in;
	char pullup_descr[15];
} plat_list[PLAT_NONE + 1] = {
        MK(CCARDIMX28JS, "ccardimx28", "BUTTON2", "LED2", MX28_GPIO(2,9), MX28_GPIO(3,4),  1, GPIO_UNDEFINED, ""),
	MK(CCIMX51JS,    "ccimx51",    "BUTTON2", "LED2", 1 /*GPIO1_1*/,  73 /*GPIO3_9*/,  1, GPIO_UNDEFINED, ""),
        MK(CCIMX53JS,    "ccimx53",    "BUTTON2", "LED2", MX5X_GPIO(4,1), MX5X_GPIO(7,12), 1, GPIO_UNDEFINED, ""),
	MK(PLAT_NONE,    "",            "",        "",    0,              0,               0, GPIO_UNDEFINED, "")  /* always last */
};
#undef MK

/*
 * Function:    get_platform
 * Description: get machine name from "/sys/kernel/machine/name"
 * Description: parse "/etc/version" file and set "platform" global variable
 */
static void get_platform(void)
{
	static const char *machine = "/sys/kernel/machine/name";
	char machine_name[64];
	FILE *fp;

	fp = fopen(machine, "r");
	if (fp == NULL) {
		printf("[ERROR] open %s: %s\n", machine, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fgets(machine_name, sizeof(machine_name) - 1, fp)) {
		/* Remove newline: fgets may store newlines if it finds them */
		if (machine_name[strlen(machine_name) - 1] == '\n')
			machine_name[strlen(machine_name) - 1] = 0;

		for (platform = 0; platform < PLAT_NONE; platform++) {
			if (!strcmp(plat_list[platform].name, machine_name))
				goto end;
		}
	}

end:
	fclose(fp);
}

/*
 * Function:    check_module_loaded
 * Description: check if gpio module is loaded by parsing "/proc/modules"
 */
static char check_module_loaded(void)
{
	static const char *modules = "/proc/modules";
	static const char *module = "gpio ";
	char buffer[80];
	char found = 0;
	FILE *fp;

	fp = fopen(modules, "r");
	if (fp == NULL) {
		printf("[ERROR] open %s: %s\n", modules, strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (fgets(buffer, sizeof(buffer) - 1, fp)) {
		if (!strncmp(buffer, module, strlen(module))) {
			found = 1;
			break;
		}
	}
	fclose(fp);

	return found;
}

/*
 * Function:    show_banner
 * Description: show some simple information
 */
static void show_banner(void)
{
	fprintf(stdout, "%s %s Copyright Digi International Inc.\n\n"
		"Test the GPIOs using a simple button and a led. The gpio driver has to be loaded\n"
		"prior to run this test and the device nodes /dev/gpio/%d and /dev/gpio/%d\n"
		"have to be created.\n\n",
		PROGRAM, VERSION, plat_list[platform].gpio_in, plat_list[platform].gpio_out);
}

/*
 * Function:    main
 * Description: application's main function
 */
int main(int argc, char *argv[])
{
	int inval = 1, lastinval, outval = 0, loops;
	int ret_val = 0;
	int fd_button;
	int fd_led;
	int fd_pullup;
	char dev_button[64];
	char dev_led[64];
	char dev_pullup[64];
	ext_irq_type_t irqtype = IRQ_FALLING;

	/* Set global variable platform */
	get_platform();
	if (platform >= PLAT_NONE) {
		printf("[ERROR] platform not detected\n");
		exit(EXIT_FAILURE);
	}

	show_banner();

	/* Check if gpio kernel module is loaded */
	if (!check_module_loaded()) {
		printf("[ERROR] gpio driver not loaded, please run 'modprobe gpio'\n\n");
		exit(EXIT_FAILURE);
	}

	/* Button */
	snprintf(dev_button, sizeof(dev_button) - 1, "/dev/gpio/%d",
		 plat_list[platform].gpio_in);
	if ((fd_button = open(dev_button, O_RDWR)) < 0) {
		printf("[ERROR] open (%s): %s\n\n", dev_button, strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* LED */
	snprintf(dev_led, sizeof(dev_led) - 1, "/dev/gpio/%d", plat_list[platform].gpio_out);
	if ((fd_led = open(dev_led, O_RDWR)) < 0) {
		printf("[ERROR] open (%s): %s\n\n", dev_led, strerror(errno));
		close(fd_button);
		exit(EXIT_FAILURE);
	}

	if (GPIO_UNDEFINED != plat_list[platform].pullup_in) {
		snprintf(dev_pullup, sizeof(dev_led) - 1, "/dev/gpio/%d",
			 plat_list[platform].pullup_in);
		if ((fd_pullup = open(dev_pullup, O_RDWR)) < 0) {
			printf("[ERROR] open (%s): %s\n\n", dev_pullup, strerror(errno));
			close(fd_led);
			close(fd_button);
			exit(EXIT_FAILURE);
		}
	}

	fprintf(stdout, "Configuring %s as input and %s as output\n", dev_button, dev_led);

	/* Configure button as input */
	if ((ret_val = ioctl(fd_button, GPIO_CONFIG_AS_INP)) < 0) {
		printf("[ERROR] ioctl (button): %s\n\n", strerror(errno));
		goto err_close;
	}

	/* Configure LED as output */
	if ((ret_val = ioctl(fd_led, GPIO_CONFIG_AS_OUT)) < 0) {
		printf("[ERROR] ioctl (led): %s\n\n", strerror(errno));
		goto err_close;
	}

	/* Initialize outval with the current led value */
	if ((ret_val = ioctl(fd_led, GPIO_READ_PIN_VAL, &outval)) < 0) {
		printf("[ERROR] ioctl (led): %s\n\n", strerror(errno));
		goto err_close;
	}

	fprintf(stdout, "Using ioctl system call to control the GPIOs\n"
		"Press the button %s 10 times and check the led %s\n\n",
		plat_list[platform].button, plat_list[platform].led);

	loops = 10;
	while (loops) {
		do {
			lastinval = inval;
			if ((ret_val = ioctl(fd_button, GPIO_READ_PIN_VAL, &inval)) < 0) {
				printf("[ERROR] ioctl (button): %s\n\n", strerror(errno));
				goto err_close;
			}
			usleep(1000);
		} while (!((lastinval == 1) && (inval == 0)));

		fprintf(stdout, "%s pressed\n", plat_list[platform].button);

		outval = outval ? 0 : 1;

		if ((ret_val = ioctl(fd_led, GPIO_WRITE_PIN_VAL, &outval)) < 0) {
			printf("[ERROR] ioctl (led): %s\n\n", strerror(errno));
			goto err_close;
		}
		loops--;
	}

	fprintf(stdout, "\nUsing read/write system calls to control the GPIOs\n"
		"Press the button %s 10 times and check the led %s\n\n",
		plat_list[platform].button, plat_list[platform].led);

	loops = 10;
	while (loops) {
		do {
			lastinval = inval;
			if ((ret_val =
			     read(fd_button, (char *)&inval, sizeof(char))) != sizeof(char)) {
				printf("[ERROR] read (button): %s\n\n", strerror(errno));
				goto err_close;
			}
			usleep(1000);
		} while (!((lastinval == 1) && (inval == 0)));

		fprintf(stdout, "%s pressed\n", plat_list[platform].button);

		outval = outval ? 0 : 1;

		if ((ret_val = write(fd_led, (char *)&outval, sizeof(char))) != sizeof(char)) {
			printf("[ERROR] write (led): %s\n\n", strerror(errno));
			goto err_close;
		}
		loops--;
	}

	if (plat_list[platform].irq_capable) {
		usleep(500000);
		fprintf(stdout, "\nConfiguring %s as IRQ input\n"
			"Press the button %s 10 times and check the led %s\n\n",
			dev_button, plat_list[platform].button, plat_list[platform].led);

		if (ioctl(fd_button, GPIO_CONFIG_AS_IRQ, &irqtype) < 0) {
			printf("[ERROR] ioctl (button): %s\n\n", strerror(errno));
			goto err_close;
		}

		loops = 10;
		while (loops) {
			if ((ret_val =
			     read(fd_button, (char *)&inval, sizeof(char))) != sizeof(char)) {
				printf("[ERROR] read (button): %s\n\n", strerror(errno));
				goto err_close;
			}

			fprintf(stdout, "%s pressed\n", plat_list[platform].button);

			outval = outval ? 0 : 1;

			if ((ret_val = write(fd_led, (char *)&outval, sizeof(char))) != 1) {
				printf("[ERROR] write (led): %s\n\n", strerror(errno));
				goto err_close;
			}
			loops--;
		}
	} else {
		fprintf(stdout, "\nConfiguring %s as IRQ is not possible\n"
			"Skipping IRQ input test\n", plat_list[platform].button);
	}

	if (GPIO_UNDEFINED != plat_list[platform].pullup_in) {
		fprintf(stdout, "\nGPIO %s pull up/down resistor will now be configured.\n",
			plat_list[platform].pullup_descr);
		if ((ret_val = ioctl(fd_pullup, GPIO_CONFIG_AS_INP)) < 0) {
			printf("[ERROR] ioctl (button): %s\n\n", strerror(errno));
			goto err_close;
		}
		fprintf(stdout, "Press <ENTER> key to enable the internal pull up: ");
		while (getchar() != '\n') ;
		if (ioctl(fd_pullup, GPIO_CONFIG_PULLUPDOWN, PULLUP) < 0) {
			printf("[ERROR] ioctl (pull up/down): %s\n\n", strerror(errno));
			goto err_close;
		}
		fprintf(stdout, "\nPull up was enabled (you can check the voltage at the gpio).\n"
			"Press <ENTER> key to enable the internal pull down: ");
		while (getchar() != '\n') ;
		if (ioctl(fd_pullup, GPIO_CONFIG_PULLUPDOWN, PULLDOWN) < 0) {
			printf("[ERROR] ioctl (pull up/down): %s\n\n", strerror(errno));
			goto err_close;
		}
		fprintf(stdout, "\nPull down was enabled (you can check the voltage at the gpio).\n");
	}

	fprintf(stdout, "\nTest completed\n\n");

 err_close:
	close(fd_button);
	close(fd_led);
	close(fd_pullup);

	return ret_val;
}
