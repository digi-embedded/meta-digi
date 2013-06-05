/*
 * libdigi/log.h
 *
 * Copyright (C) 2001,2002 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: Logging facility for applications
 *
 */

#ifndef DG_LOG_H
#define DG_LOG_H

typedef enum {
	LOG_ERR = 0,
	LOG_STATUS,
	LOG_HARDWARE1,
	LOG_HARDWARE2,
	LOG_PACKET,
	LOG_LAST
} LogLevel;

extern LogLevel logLevel;

extern void logMsg(LogLevel level, const char *szFormat, ...);
extern void systemLog(const char *szFormat, ...);
extern void error(const char *szFormat, ...);
extern void systemError(const char *szFormat, ...);

#endif	/* DG_LOG_H */
