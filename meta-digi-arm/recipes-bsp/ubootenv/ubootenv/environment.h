/*
 * environment.h
 *
 * Copyright (C) 2006-2013 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: Data types and definitions for environment in NVRAM
 *
 */

#ifndef __ENV_TOOL_H_
#define __ENV_TOOL_H_

#define VAR_SEP		'\0'
#define VAR_ASIGN	'='

typedef struct {
	unsigned long crc;
	char data[];
} env_t;

#endif
