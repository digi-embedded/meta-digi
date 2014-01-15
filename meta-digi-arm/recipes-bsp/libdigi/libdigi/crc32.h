/*
 * libdigi/crc32.h
 *
 * Copyright (C) 2006 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: CRC32 functions
 *
 */

#ifndef DG_CRC32_H
#define DG_CRC32_H

#include <stdint.h>		/* uint32_t */
#include <stdlib.h>		/* size_t */

typedef uint32_t crc32_t;
extern crc32_t crc32(crc32_t uiCRC, const void *pvBuf, size_t iLen);

#endif	/* DG_CRC32_H */
