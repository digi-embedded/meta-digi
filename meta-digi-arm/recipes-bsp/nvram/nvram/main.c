/*
 *  nvram/src/main.c
 *
 *  Copyright (C) 2006-2013 by Digi International Inc.
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version2  as published by
 *  the Free Software Foundation.
 */
/*
 *  !Author:     Markus Pietrek
 *  !Descr:      main() and user code for nvram.
 */

#include <errno.h>              /* errno */
#include <fcntl.h>              /* open */
#include <stdarg.h>             /* vprintf */
#include <stdio.h>              /* snprintf */
#include <stdlib.h>             /* EXIT_SUCCESS */
#include <string.h>             /* strdup */
#include <sys/stat.h>

/* from libdigi */
#include <cmdopt.h>
#include <log.h>
#include <misc_helper.h>

#include "nvram.h"              /* Nv* */

#define CA(cmd) \
        do { \
                if( !cmd ) \
                        ExitError( #cmd );      \
        } while( 0 )

#define VERSION		"1.15" "-g"CMD_GIT_SHA1

/* Hack to change priv_linux mode */
void NvPrivLinuxSetMode(char bManufMode);

/* local functions */
static void ExitError(const char *szFormat, ...);
static void OnExit(void);
static void ExtendedUsage(char bCmdLine);
static void OSLoadFromFile(nv_os_type_e eOS, const char *szFile);
static void OSSaveToFile(nv_os_type_e eOS, const char *szFile);

static char l_bOptDetailed = 0;
static char quiet = 0;
static char l_bManufMode = 0;

int main(int argc, char *argv[])
{
	char acVersion[128];
	uint32_t uiLibVerMajor;
	uint32_t uiLibVerMinor;
	int iExtendedArgs;
	const char *xPrintAll[] = { "printall" };
	const char *szOSOutFile = NULL;
	const char *szOSInFile = NULL;
	const char *szOS = NULL;
	char bSave = 0;
	nv_os_type_e eOS = NVOS_NONE;
	int ret;

	CmdOptEntry aCmdEntries[] = {
		/*@@-nullassign@@ */// only COT_NONE may have a NULL for vValuePtr
		{COT_BOOL, 'e', &l_bOptDetailed, "error_detailed",
		 "detailed error messages"},
		{COT_BOOL, 'b', &g_markBadBlocks, "bad-block-marking",
		 "On repeated error, mark block as bad."},
		{COT_STRING, 'g', &szOSOutFile, "get_os_cfg",
		 "copies the os configuration block to file"},
		{COT_STRING, 's', &szOSInFile, "set_os_cfg",
		 "copies the os configuration block from file"},
		{COT_STRING, 'o', &szOS, "os",
		 "select's the OS to get configuration from"},
		{COT_BOOL, 'q', &quiet, "quiet",
		 "display no error messages"},
		{COT_BOOL, 'm', &l_bManufMode, "manuf-mode",
		 "manufacturing mode (no auto-repair, permit reset)"},
		{COT_MORE_OPT, 0, NULL, "", ""},
		{COT_NONE, 0, NULL, NULL, NULL},
		/*@@+nullassign@@ */
	};

	NvGetLibVersion(&uiLibVerMajor, &uiLibVerMinor);
	snprintf(acVersion,
		 sizeof(acVersion) - 1,
		 "Version: " VERSION ", NVRAM Library %u.%u-g" LIB_GIT_SHA1 ", compiled on "
		 __DATE__ "," __TIME__, uiLibVerMajor, uiLibVerMinor);
	acVersion[sizeof(acVersion) - 1] = 0U;
	szCmdOptVersion = acVersion;
	fnCmdOptExtendedUsage = ExtendedUsage;

	iExtendedArgs = cmdOptParse(argc, argv, aCmdEntries,
				    "NVRAM Tool for updating nvram settings");
	logMsg(LOG_HARDWARE1,
	       "Sizes: Critical:        %i\n"
	       "       Module ID:       %i\n"
	       "       IP:              %i\n"
	       "       IP Device:       %i\n"
	       "       Partition Table: %i\n"
	       "       Partition Entry: %i\n"
	       "       OS Cfg Table:    %i\n"
	       "       OS Cfg:          %i\n",
	       sizeof(nv_critical_t),
	       sizeof(nv_param_module_id_t),
	       sizeof(nv_param_ip_t),
	       sizeof(nv_param_ip_device_t),
	       sizeof(nv_param_part_table_t),
	       sizeof(nv_param_part_t),
	       sizeof(nv_param_os_cfg_table_t),
	       sizeof(nv_param_os_cfg_t));

	/* so we can close everything even on error() or on return of main */
	atexit(OnExit);

	NvPrivLinuxSetMode(l_bManufMode);

	/* In manufacturing mode, do not let library auto-repair the NVRAM */
	ret = NvInit(l_bManufMode ? NVR_MANUAL : NVR_AUTO);
	if (!ret) {
		/* If NVRAM was not initialized, only continue if
		 * we are requesting a reset.
		 */
		if (argc == iExtendedArgs) {
			ExitError("NvInit");
		} else {
			if (strcmp("reset", argv[iExtendedArgs]))
				ExitError("NvInit");
		}
	}

	if (NULL != szOS) {
		if (!NvToOS(&eOS, szOS))
			error("OS not known: %s\n", szOS);
	}

	if (NULL != szOSInFile) {
		bSave = 1;
		OSLoadFromFile(eOS, szOSInFile);
	}

	if (NULL != szOSOutFile)
		/* it's load from NVRAM view */
		OSSaveToFile(eOS, szOSOutFile);

	if (argc == iExtendedArgs) {
		if ((NULL == szOSInFile) && (NULL == szOSOutFile))
			/* on -o, the user likes to read/write something */
			CA(NvCmdLine(ARRAY_SIZE(xPrintAll), xPrintAll));
	} else {
		CA(NvCmdLine(argc - iExtendedArgs, (const char **)&argv[iExtendedArgs]));

		if (!strcmp("set", argv[iExtendedArgs]) ||
		    !strcmp("reset", argv[iExtendedArgs]) ||
		    !strcmp("init", argv[iExtendedArgs]))
			bSave = 1;
	}

	if (bSave)
		CA(NvSave());

	return EXIT_SUCCESS;
}

/* ********** local functions ********** */

static void ExtendedUsage(char bCmdLine)
{
	if (bCmdLine == 1)
		CA(NvPrintHelp());
}

static void ExitError(const char *szFormat, ...)
{
	const char *szError = NULL;
	const char *szWhat = NULL;
	const char *szFunc = NULL;
	const char *szFile = NULL;
	int iLine;

	va_list ap;

	if (!quiet || l_bOptDetailed) {
		fprintf(stderr, "*** Error: ");
		if (l_bOptDetailed) {
			/*@-formatconst@ */
			va_start(ap, szFormat);
			vfprintf(stderr, szFormat, ap);
			va_end(ap);
			fprintf(stderr, ": ");
			/*@+formatconst@ */
		}

		if (NVE_GOOD != NvErrorMsg(&szError, &szWhat, &szFunc, &szFile, &iLine)) {
			if (l_bOptDetailed)
				fprintf(stderr, " %s: (%s) @ %s:%i (%s)",
					szError, szWhat, szFile, iLine, szFunc);
			else
				fprintf(stderr, " %s: (%s)", szError, szWhat);
		}

		fprintf(stderr, "\n");
	}

	exit(EXIT_FAILURE);
}

static void OSLoadFromFile(nv_os_type_e eOS, const char *szFile)
{
	int iFd;
	nv_param_os_cfg_t xCfg;
	void *pvTmp;

	CA(NvOSCfgFind(&xCfg, eOS));
	pvTmp = malloc(xCfg.uiSize);
	if (NULL == pvTmp)
		systemError("malloc: %i", xCfg.uiSize);

	iFd = open(szFile, O_RDONLY);
	if (-1 == iFd)
		systemError("%s", szFile);
	if (-1 == read(iFd, pvTmp, xCfg.uiSize))
		systemError("read");
	CLOSE(iFd);

	CA(NvOSCfgSet(eOS, pvTmp, xCfg.uiSize));

	FREE(pvTmp);

	printf("Loaded from %s\n", szFile);
}

static void OSSaveToFile(nv_os_type_e eOS, const char *szFile)
{
	int iFd;
	nv_param_os_cfg_t xCfg;
	void *pvTmp;
	size_t iSize;

	CA(NvOSCfgFind(&xCfg, eOS));
	pvTmp = malloc(xCfg.uiSize);
	if (NULL == pvTmp)
		systemError("malloc: %i", xCfg.uiSize);

	CA(NvOSCfgGet(eOS, pvTmp, xCfg.uiSize, &iSize));

	iFd = open(szFile, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (-1 == iFd)
		systemError("%s", szFile);
	if (xCfg.uiSize != write(iFd, pvTmp, xCfg.uiSize))
		systemError("write");
	CLOSE(iFd);
	FREE(pvTmp);

	printf("Stored to %s\n", szFile);
}

/*! \brief closes all descriptors on any exit */
static void OnExit(void)
{
	NvFinish();
}
