/*
 *  Copyright (C) 2011 by Digi International Inc.
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version2  as published by
 *  the Free Software Foundation.
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "digi-platforms.h"
#include "log.h"

/*
 * Gets the platform ID from the file /proc/cpuinfo
 * (the kernel needs to write the machine ID in this file).
 * Returns the machine ID or -1 on error
 */
int get_platform_id(void)
{
	char buffer[80];
	FILE *fp;
	long id = -1;

	fp = popen("cat /proc/cpuinfo | grep \"Machine ID\" | cut -f 2 -d :", "r");
	if (fp == NULL)
		systemError("cannot access /proc/cpuinfo");

	if (fgets(buffer, sizeof(buffer) - 1, fp)) {
		errno = 0;	/* to distinguish success/failure after call */
		id = strtol(buffer, NULL, 10);
		if (errno != 0)
			id = -1;	/* don't care about the error code */
	}
	fclose(fp);

	return id;
}

/*
 * get_platform_name_from_fdt
 *
 * Read the device tree and return platform name or NULL
 */
static char *get_platform_name_from_fdt(void)
{
	static const char *fdt = "/proc/device-tree/digi,machine,name";
	static char buffer[64];	/* static buffer so it can be used in the caller function */
	char *plat_name = NULL;
	FILE *fp;

	fp = fopen(fdt, "r");
	if (fp == NULL)
		goto out;

	plat_name = fgets(buffer, sizeof(buffer), fp);
	fclose(fp);

out:
	return plat_name;
}

/*
 * Checks whether platform requires an atomic access to NAND OOB
 */
char is_nand_oob_atomic(void)
{
	int platform_id;

	platform_id = get_platform_id();
	if (platform_id != -1) {
		/* The following platforms require atomic access to NAND OOB */
		if (MACH_TYPE_CPX2 == platform_id ||
		    MACH_TYPE_WR21 == platform_id ||
		    MACH_TYPE_CCMX51 == platform_id ||
		    MACH_TYPE_CCMX51JS == platform_id ||
		    MACH_TYPE_CCWMX51 == platform_id ||
		    MACH_TYPE_CCWMX51JS == platform_id ||
		    MACH_TYPE_CCIMX51 == platform_id ||
		    MACH_TYPE_CCIMX51JS == platform_id ||
		    MACH_TYPE_CCMX53 == platform_id ||
		    MACH_TYPE_CCMX53JS == platform_id ||
		    MACH_TYPE_CCWMX53 == platform_id ||
		    MACH_TYPE_CCWMX53JS == platform_id ||
		    MACH_TYPE_CCIMX53 == platform_id ||
		    MACH_TYPE_CCIMX53JS == platform_id ||
		    MACH_TYPE_CCARDMX28 == platform_id ||
		    MACH_TYPE_CCARDMX28JS == platform_id ||
		    MACH_TYPE_CCARDWMX28 == platform_id ||
		    MACH_TYPE_CCARDWMX28JS == platform_id ||
		    MACH_TYPE_CCARDIMX28 == platform_id ||
		    MACH_TYPE_CCARDIMX28JS == platform_id)
			return 1;
	} else {
		/*
		 * Workaround to detect is_nand_oob_atomic in ccardimx28 using
		 * linux 3.x
		 * TODO: generalize this.
		 */
		char *platform_name = get_platform_name_from_fdt();
		if (platform_name && !strcmp(platform_name, "ccardimx28"))
			return 1;
	}

	return 0;
}
