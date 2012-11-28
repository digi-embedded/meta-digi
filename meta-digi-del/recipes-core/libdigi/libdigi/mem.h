/*
 * libdigi/mem.h
 *
 * Copyright (C) 2006 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: provides MemCmp() and MemDump()
 *
 */

#ifndef DG_MEM_H
#define DG_MEM_H

#include <stdlib.h>		/* size_t */

extern loff_t MemCmp(const void *pvS1, const void *pvS2, size_t iSize);
extern void MemDump(const void *pvBase, loff_t iOffset, size_t iLen);

#endif	/* DG_MEM_H */
