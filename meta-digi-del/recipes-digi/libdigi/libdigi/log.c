/*
 * log.c
 *
 * Copyright (C) 2001,2002 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: implements logging interface
 *
 */

#include <stdio.h>		// fprintf
#include <stdlib.h>		// EXIT_FAILURE
#include <errno.h>		// errno
#include <netdb.h>              // h_errno
#include <string.h>		// strerror
#include <stdarg.h>		// vprintf

#include "log.h"

/* Globaly visible */
LogLevel logLevel = LOG_STATUS;


/***********************************************************************
 * @Function: log
 * @Return: nothing
 * @Descr: format and arg1..arg3 are printed on stderr only if level
 *         is <= logLevel
 *         format may not contain any newline character.
 ***********************************************************************/
void logMsg(LogLevel level, const char *szFormat, ...)
{
	if (level <= logLevel) {
		va_list ap;

		/*@-formatconst@ */
		va_start(ap, szFormat);
		vfprintf(stderr, szFormat, ap);
		va_end(ap);
		/*@-formatconst@ */
		fputs("\n", stderr);
	}
}

/***********************************************************************
 * @Function: systemLog
 * @Return: nothing
 * @Descr: dumps the system error that happened just before
 ***********************************************************************/
void systemLog(const char *szFormat, ...)
{
	char *szError = NULL;
	va_list ap;

	if (errno)
		szError = strdup(strerror(errno));
	else if (h_errno)
		szError = strdup(hstrerror(h_errno));

	/*@-formatconst@ */
	va_start(ap, szFormat);
	vfprintf(stderr, szFormat, ap);
	va_end(ap);
	/*@-formatconst@ */
	if (NULL != szError)
		fprintf(stderr, " (%s)", szError);

	fputs("\n", stderr);

	if (NULL != szError)
		free(szError);
}

/***********************************************************************
 * @Function: error
 * @Return: never
 * @Descr: format and arg1..arg3 are printed on stderr only if level
 *         is <= logLevel
 *         format may not contain any newline character.
 ***********************************************************************/
void error(const char *szFormat, ...)
{
	va_list ap;

	fprintf(stderr, "*** Error: ");
	/*@-formatconst@ */
	va_start(ap, szFormat);
	vfprintf(stderr, szFormat, ap);
	va_end(ap);
	/*@-formatconst@ */
	fputs("\n", stderr);

	exit(EXIT_FAILURE);
}

/***********************************************************************
 * @Function: systemError
 * @Return: never
 * @Descr: dumps the system error that happened just before and exits the
 *         application
 ***********************************************************************/
void systemError(const char *szFormat, ...)
{
	char *szError = NULL;
	va_list ap;

	if (errno)
		szError = strdup(strerror(errno));
	else if (h_errno)
		szError = strdup(hstrerror(h_errno));

	fprintf(stderr, "*** Error: ");
	/*@-formatconst@ */
	va_start(ap, szFormat);
	vfprintf(stderr, szFormat, ap);
	va_end(ap);
	/*@-formatconst@ */
	if (NULL != szError)
		fprintf(stderr, " (%s)", szError);

	fputs("\n", stderr);
	free(szError);

	exit(EXIT_FAILURE);
}
