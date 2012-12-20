/*
 * libdigi/cmdopt.h
 *
 * Copyright (C) 2001,2002 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: Helper functions to access command line options.
 *              The results are stored in global variables.
 *
 */

#ifndef DG_CMDOPT_H
#define DG_CMDOPT_H

typedef enum {
	COT_BOOL,		// if present,bool is set 1, otherwise unchanged
	COT_INT,		// sets *valuePtr to the int value
				// with conversions (if 0x
				// prefix is present)
	COT_STRING,		// sets *valuePtr to the string
	COT_MORE,		// to end the array. Any additional arguments
				// are allowed
	COT_MORE_OPT,		// to end the array. Any optional arguments
				// are allowed
	COT_NONE		// to end the array. Any additional arguments
				// are considered as failures
} CmdOptTypes;

typedef struct {
	CmdOptTypes type;	// type of option to be read
	signed char cOptChar;	// character to identify that option, if 0, then
				// no option is used but parameter is required
	void *vValuePtr;	// ptr to the variable where value is stored
	const char *szLabelStr;	// label displayed in command line
	const char *szHelpStr;	// displays this help string for the variable
	char *pbPresent;	// will be set to if this option is present
} CmdOptEntry;

/*  can be set to display additional usage */
extern void (*fnCmdOptExtendedUsage) (char bCmdLine);

extern const char *szCmdOptVersion;     // overwrite it before calling cmdOptParse to
                                        // define version of application

int cmdOptParse(int argc, char *argv[], const CmdOptEntry entries[], const char *szDescr);

void cmdOptUsageAndExit(int argc,
			char *argv[], const CmdOptEntry entries[], const char *szDescr);

#endif	/* DG_CMDOPT_H */
