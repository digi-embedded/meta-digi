/*
 * libdigi/misc_helper.h
 *
 * Copyright (C) 2006 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: miscellanous definitions that simplifies developing
 *              May require 'string.h' or 'log.h'
 *
 */

#ifndef DG_MISC_HELPER_H
#define DG_MISC_HELPER_H

#include <stdio.h>		/* snprintf */
#include <string.h>		/* memset */

#define ARRAY_SIZE(x)   (sizeof(x)/sizeof(*(x)))
#define CLEAR(x)        memset( &x, 0, sizeof( x ) )

/* round up to kB */
#define TO_KiB(x)       (((x) + 1023) / 1024)

/* to bytes */
#define KiB(x)          ((x) * 1024)
#define MiB(x)          (KiB(x) * 1024)

#define MAX(a, b)       ((a) < (b) ? (b) : (a))
#define MIN(a, b)       ((a) > (b) ? (b) : (a))

#define FREE(x)                                 \
        do {                                    \
                free((void *)x);                \
                x = NULL;                       \
        } while (0)

#define CLOSE(x)                                \
        do {                                    \
                if (close(x))                   \
                        systemError("close");   \
                x = -1;                         \
        } while (0)

#define SPRINTF(acStr, args...)                 \
        snprintf(acStr, sizeof(acStr), args)

#endif	/* DG_MISC_HELPER_H */
