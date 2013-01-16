/*
 * update_flash.c
 *
 * Copyright (C) 2006 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */
/*
 *
 * !Revision: $Revision: 1.25 $
 * !Descr: Flash Test Util
 * !References: [1] mtd-utils/flash_eraseall.c
 *              [2] http://www.linux-mtd.infradead.org/doc/nand.html
 *              [3] http://www.linux-mtd.infradead.org/tech/mtdnand/x255.html
 */

#define _XOPEN_SOURCE	500     /* for pread/pwrite */

#include <errno.h>		/* ENOENT */
#include <fcntl.h>		/* open */
#include <libgen.h>		/* basename */
#include <regex.h>		/* regexp */
#include <signal.h>		/* kill */
#include <stdarg.h>		/* varg */
#include <stdio.h>		/* printf */
#include <stdlib.h>		/* EXIT_SUCCESS */
#include <string.h>		/* memset */
#include <unistd.h>		/* close */

#include <arpa/inet.h>		/* ntohl */
#include <mntent.h>		/* setmntent */
#include <sys/ioctl.h>		/* ioctl */
#include <sys/mount.h>		/* mount */
#include <sys/stat.h>		/* stat */
#include <sys/vfs.h>		/* statfs */

#include <mtd/mtd-user.h>	/* MEMERASE */

static int target_endian = __BYTE_ORDER; /* for jffs2-user.h, cpu_to_je16 */
#include "jffs2-user.h"		/* jffs2_unknown_node */

/* libdigi */
#include <libdigi/cmdopt.h>
#include <libdigi/crc32.h>
#include <libdigi/digi-platforms.h>
#include <libdigi/log.h>
#include <libdigi/mem.h>
#include <libdigi/misc_helper.h>

#define VERSION			"1.25" "-g"GIT_SHA1

/* man statfs does mention them, but they are only defined inside kernel */
#define NFS_SUPER_MAGIC		0x6969	/* linux/nfs_fs.h */
#define JFFS2_SUPER_MAGIC	0x72b6	/* linux/jffs2.h */

#define IO_BLOCK_SIZE		65536

#define FLASH_ERASED_BYTE	0xff

#define WARNING			"!!! "
#define INFO			"--- "

#define PRINTF(...) \
	do { \
		if (!cSilent) \
			printf(__VA_ARGS__); \
	} while (0)

#define PERCENTAGE(iCurrent, iTotal)	((iCurrent * 100) / (iTotal ? iTotal : 1))

#define SET_CRC32(pMtd, uiCRC32) \
	do { \
		pMtd->uiCRC32 = uiCRC32; \
		pMtd->cChecksumSet = 1; \
	} while (0)

/* open it at least read-only so we can see whether open fails or not */
#define OPEN_READWRITE_IF_NOT_DRY	((cDryRun ? O_RDONLY : O_RDWR) | O_SYNC)

/* ********** data types ********** */

typedef enum {
	PTUBoot = 0,
	PTKernel,
	PTEnvironment,
	PTFPGA,
	PTBootstream,
	PTUnknown		/* always last */
} PartType_e;

typedef enum {
	FTUBoot = 0,
	FTKernel,
	FTFPGA,
	FTNVRAM,
	FTJFFS2,
	FTSQUASHFS,
	FTUBI,
	FTBootstream,
	FTUnknown		/* always last */
} FileType_e;

typedef struct {
	/* configuration data */
	const char *szOrigImageFileName;
	const char *szImageFileName;	/* may later be /tmp/<szOrig*> */
	unsigned int uiPartition;
	char cChecksumSet;
	char cEraseAll;
	uint32_t uiCRC32;

	/* auto-detected */
	char cChecksumCalculated;
	char acName[64];
	char cIsNAND;
	char cWriteCleanMarker;
	char cFileType;
	char cIsJFFS2;
	loff_t iSize;
	loff_t iFileSize;
	size_t iPageSize;
	mtd_info_t xInfo;
	unsigned int uiBadBlocks;
	PartType_e ePartType;
	FileType_e eFileType;
	FileType_e eFileTypeNeeded;

	/* open handles */
	int iFd;		/* of Partition */

	/* status */
	char cAlreadyPrintedVerifyWarning;
	char cAlreadyRemounted;

	unsigned int uiClMPos;
	unsigned int uiClMLen;
	struct jffs2_unknown_node xCleanMarker;
} mtdPartition_t;


/* ********** function definitions ********** */

/* top level functions */
static void DoPrintChecksums(void);
static void DoMtdUpdate(void);
static void DoMtdVerify(void);

static void OnExit(void);

/* helper functions */
static void MtdInit(void);
static void MtdPartInit( /*@out@ */ mtdPartition_t * pMtd, unsigned int uiPartition,
			const char *szImageFileName);
static void MtdPartOpen( /*@inout@ */ mtdPartition_t * pMtd, char cReadOnly);
static void MtdPartClose( /*@inout@ */ mtdPartition_t * pMtd);
static int MtdPartIsBadBlock(const mtdPartition_t * pMtd, loff_t iOffset);
static void MtdPartUseFile( /*@inout@ */ mtdPartition_t * pMtd, const char *szImageFileName);
static void MtdPartErase(const mtdPartition_t * pMtd);
static void MtdPartWrite(mtdPartition_t * pMtd);
static int MtdPartVerify(mtdPartition_t * pMtd);
static void MtdPartCheckCRC32(mtdPartition_t * pMtd);
static void MtdPartCopyFile(mtdPartition_t * pMtd, const char *szDstFileName);
static void MtdPartInitCleanMarker(const mtdPartition_t * pMtd,
				   struct jffs2_unknown_node *pCleanMarker,
				   unsigned int *puiClMPos, unsigned int *puiClMLen);
static void MtdPartInitJFFS2Node(struct jffs2_unknown_node *pNode, unsigned short uhNodeType,
				 size_t iLen);
static void MtdPartCompareCRC32(mtdPartition_t * pMtd, uint32_t uiCRC32);
static void MtdPartRemountAllReadOnly(mtdPartition_t * pMtd);
static void MtdPartDeterminePartType(mtdPartition_t * pMtd);
static void MtdPartDetermineAndCheckFileType(mtdPartition_t * pMtd);
static void MtdPartVerifyFile(mtdPartition_t * pMtd);
static void MtdPartVerifyJFFS2Block(mtdPartition_t * pMtd, unsigned char *pucData,
				    size_t iSize);
static int MtdPartGetThrottle(const mtdPartition_t * pMtd, uint64_t ullSize);
static void PrintProgress(int iPercentage, int iThrottle, const char *szFmt, ...);
static void VerifyTmpDir(void);
static uint32_t CalcCRC32OfFile(const char *szFileName);
static const char *GetRootDevice(void);
static void mtd_part_write_ubi(mtdPartition_t * pMtd);

/* ********** local variables ********** */

/* set by command line */
static const char *szTmpDir = NULL;
static const char *szKey = NULL;
static char cNoImageTypeCheck = 0;
static char cProgressInNewLine = 0;
static char cSilent = 0;
static char cChecksumOnly = 0;
static char cDoReboot = 0;
static char cHasChecksum = 0;
static char cWriteCleanMarker = 0;
static char cDryRun = 0;
static char cVerify = 0;
static char cVerifyOnly = 0;
static char cEraseAll = 0;
static char cMaxRetries = 3;
static char cMarkBadBlocks = 0;

/* calculated */
static mtdPartition_t axMtdParts[64];
static mtdPartition_t *pMtdPartLastToUpdate = axMtdParts;
static const char *szMtdPrefix = "/dev/mtd/";
static const char *szMtdBlockPrefix = "/dev/mtdblock/";
static unsigned int uiMtdPartsCount = 0;

#define MK(x, szName)[x] = szName
static const char *aszPartType[PTUnknown + 1] = {
	MK(PTUBoot,       "U-Boot"),
	MK(PTKernel,      "Kernel"),
	MK(PTEnvironment, "NVRAM"),
	MK(PTFPGA,        "FPGA"),
	MK(PTBootstream,  "Bstrm-U-Boot"),
	MK(PTUnknown,     "Unknown"),
};
#undef MK

#define MK(x, szRegExp, szName)[x] = {szRegExp, szName}
static const struct {
	const char *szExp;
	const char *szName;
} axFileType[FTUnknown + 1] = {
	MK(FTUBoot,       "u-boot-.*\\.bin",   "UBoot"),
	MK(FTNVRAM,       "nvram-.*",          "NVRAM"),
	MK(FTKernel,      "uImage-.*",         "Kernel"),
	MK(FTFPGA,        ".*\\.biu",          "FPGA"),
	MK(FTJFFS2,       ".*\\.jffs2",        "JFFS2"),
	MK(FTSQUASHFS,    ".*\\.squashfs",     "SQUASHFS"),
	MK(FTUBI,       ".*\\.ubi",            "UBI"),
	MK(FTBootstream,  ".*\\.sb",	       "Bootstream"),
	MK(FTUnknown,     ".*",                "Unknown"),
};
#undef MK

/* ********** function implementations ********** */

int main(int argc, char *argv[])
{
	int iPartListIndex;

	CmdOptEntry aCmdEntries[] = {
		{COT_BOOL, 'C', &cChecksumOnly, "checksum-only",
		 "calculates only CRC32 checksum of image"},
		{COT_BOOL, 'R', &cDoReboot, "reboot",
		 "reboots the system"},
		{COT_BOOL, 'V', &cVerifyOnly, "verify-only",
		 "verifies current contents, no updates are done"},
		{COT_BOOL, 'v', &cVerify, "verify",
		 "After flashing, compare flash contents with image on byte-to-byte"},
		{COT_BOOL, 'c', &cHasChecksum, "checksum",
		 "flashes only when checksum matches"},
		{COT_BOOL, -1, &cDryRun, "dry-run",
		 "don't erase or write to the flash"},
		{COT_BOOL, -1, &cProgressInNewLine, "progress-in-new-line",
		 "each percentage is printed in an own line"},
		{COT_BOOL, 'i', &cNoImageTypeCheck, "no-image-type-check",
		 "doesn't check image type for partition"},
		{COT_BOOL, 'f', &cEraseAll, "erase-all",
		 "erases the partition, not only the parts being written"},
		{COT_BOOL, -1, &cWriteCleanMarker, "clean-marker",
		 "writes clean markers to every partition (implies -f)"},
		{COT_BOOL, 'b' , &cMarkBadBlocks, "bad-block-marking",
		 "On repeated error, marks block as bad."},
		{COT_BOOL, 's', &cSilent, "silent",
		 "Silent Mode"},
		{COT_STRING, 't', &szTmpDir, "tmpdir",
		 "copy files to temporary directory before flashing"},
		{COT_STRING, 'k', &szKey, "encrypt_key",
		 "Verify bootstream image against encryption key"},
		{COT_MORE, 0, NULL, "<file [part] [checksum]>",
		 "file to flash to partition and check for checksum"},
		{COT_NONE, 0, NULL, NULL, NULL},
	};

	CLEAR(axMtdParts);

	szCmdOptVersion = "Version: " VERSION ", compiled on " __DATE__ "," __TIME__;
	iPartListIndex = cmdOptParse(argc, argv, aCmdEntries,
		"Flash Update Tool\n\n"
		"Examples of use cases:\n"
		"  update_flash rootfs-ccw9cjsnand-128.jffs2 4\n"
		"      => updates partition /dev/mtd4 with rootfs image\n"
		"\n"
		"  update_flash -C uImage-ccw9cjsnand\n"
		"      => calculates file CRC32 only\n"
		"\n"
		"  update_flash -c uImage-ccw9cjsnand 3 0x1051e3c9\n"
		"      => updates partition /dev/mtd3 only if CRC32 of file is 0x1051e3c9\n"
		"\n"
		"  update_flash uImage-ccw9cjsnand 3 rootfs-ccw9cjsnand-128.jffs2 4:\n"
		"      => updates kernel at partition 3 and rootfs at partition 4\n"
		"\n"
		"  update_flash u-boot-cpx2-ivt.sb 0 -k 48855699413545113545511513300447\n"
		"      => updates bootstream file on partition 0 if the encryption key matches\n");

	/* so we can close everything even on error() or on return of main */
	atexit(OnExit);

	/* Force disable writing clean markers for platforms that require atomic
	 * access to the OOB */
	if (cWriteCleanMarker && is_nand_oob_atomic()) {
		PRINTF(WARNING "JFFS2 clean markers disabled for this platform\n");
		cWriteCleanMarker = 0;
	}

	if (cWriteCleanMarker)
		cEraseAll = 1;

	if (!cChecksumOnly)
		MtdInit();

	/* check what files to write to what partition */
	while (iPartListIndex < argc) {
		unsigned int uiPartition = 0;

		if ((pMtdPartLastToUpdate - axMtdParts) >= ARRAY_SIZE(axMtdParts))
			error("Too many partitions to update on command line");

		if (iPartListIndex > (argc - (1 + (cChecksumOnly ? 0 : 1) + (cHasChecksum ? 1 : 0))))
			error("Require filename%s%s", (cChecksumOnly ? "" : " and partition"),
							(cHasChecksum ? " and checksum" : ""));

		if (!cChecksumOnly) {
			/* check partition argument */
			if (sscanf(argv[iPartListIndex + 1], "%u", &uiPartition) != 1)
				error("Wrong partition number\n");

			if (uiPartition >= uiMtdPartsCount)
				error("Have only %u mtd partitions", uiMtdPartsCount);

			MtdPartInit(pMtdPartLastToUpdate, uiPartition, argv[iPartListIndex]);
			iPartListIndex++;
		} else {
			/* initialize it partly  */
			CLEAR(*pMtdPartLastToUpdate);
			pMtdPartLastToUpdate->iFd = -1;
			MtdPartUseFile(pMtdPartLastToUpdate, argv[iPartListIndex]);
		}

		if (cHasChecksum) {
			/* parse checksum argument */
			const char *szCRC32 = argv[iPartListIndex + 1];
			uint32_t uiCRC32;

			if (sscanf(szCRC32, "%x", &uiCRC32) != 1)
				error("Invalid Checksum: %s", szCRC32);
			SET_CRC32(pMtdPartLastToUpdate, uiCRC32);
			iPartListIndex++;
		}

		iPartListIndex++;
		pMtdPartLastToUpdate++;
	}			/* while( iPartListIndex ) */

	/* all command line parsing verifications complete */

	/* report what will be done */
	if (cNoImageTypeCheck || cDryRun || cWriteCleanMarker || cEraseAll) {
		PRINTF("\nEnabled command line options:\n");
		if (cNoImageTypeCheck)
			PRINTF(INFO "do not check image type\n");
		if (cDryRun)
			PRINTF(INFO "dry-run: flash content is not changed\n");
		if (cWriteCleanMarker)
			PRINTF(INFO "write JFFS2 clean markers\n");
		if (cEraseAll)
			PRINTF(INFO "erase complete partition\n");
		PRINTF("\n");
	}

	if (cChecksumOnly)
		DoPrintChecksums();
	else if (cVerifyOnly)
		DoMtdVerify();
	else
		DoMtdUpdate();

	PRINTF("Done\n");

	if (cDoReboot) {
		sync();

		PRINTF("Rebooting System\n");
		/* kills init */
		kill(1, SIGTERM);
	}

	/* may not be reached in case of kill */
	return EXIT_SUCCESS;
}

/***********************************************************************
 * !Function: DoPrintChecksums
 * !Descr:    Calculates CRC32 of files (and compares them to -c option)
 ***********************************************************************/
static void DoPrintChecksums(void)
{
	mtdPartition_t *pMtd = NULL;

	PRINTF("CRC32 Results:\n");

	for (pMtd = axMtdParts; pMtd < pMtdPartLastToUpdate; pMtd++) {
		uint32_t uiCRC32 = 0;

		uiCRC32 = CalcCRC32OfFile(pMtd->szImageFileName);
		PRINTF("  %-20s : 0x%08x\n", pMtd->szImageFileName, uiCRC32);

		if (pMtd->cChecksumSet)
			MtdPartCompareCRC32(pMtd, uiCRC32);
	}
}

/***********************************************************************
 * !Function: DoMtdUpdate
 * !Descr:    Updates all flash partitions
 ***********************************************************************/
static void DoMtdUpdate(void)
{
	mtdPartition_t *pMtd = NULL;

	if (NULL != szTmpDir) {
		/* checksum is checked or calculated while copying to tmp */
		VerifyTmpDir();

		/* copy all files to tmp before starting to update */
		for (pMtd = axMtdParts; pMtd < pMtdPartLastToUpdate; pMtd++) {
			/* copy it to temp */
			char acTmpFileName[256];
			char *szFileName = strdup(pMtd->szImageFileName);
			snprintf(acTmpFileName, sizeof(acTmpFileName) - 1,
				 "%s/%s", szTmpDir, basename(szFileName));
			acTmpFileName[sizeof(acTmpFileName) - 1] = 0;
			FREE(szFileName);

			PRINTF("Copying %s to %s\n", pMtd->szImageFileName, szTmpDir);
			MtdPartCopyFile(pMtd, acTmpFileName);

			/* use temporary file name from name */
			FREE(pMtd->szImageFileName);	/* get rid of const* */
			pMtd->szImageFileName = strdup(acTmpFileName);
		}
	} else if (cHasChecksum || !cNoImageTypeCheck) {
		/* verify checksum before starting to update */
		PRINTF("Verifying File(s): ");
		for (pMtd = axMtdParts; pMtd < pMtdPartLastToUpdate; pMtd++) {
			PRINTF("  %s\n", pMtd->szImageFileName);
			MtdPartVerifyFile(pMtd);
		}
	}

	/* make it read-only so no one can destroy the data */
	for (pMtd = axMtdParts; pMtd < pMtdPartLastToUpdate; pMtd++)
		MtdPartRemountAllReadOnly(pMtd);

	PRINTF("Updating:\n");

	for (pMtd = axMtdParts; pMtd < pMtdPartLastToUpdate; pMtd++) {
		PRINTF("  %s (%lli KiB)\n", pMtd->szImageFileName, TO_KiB(pMtd->iFileSize));

		MtdPartOpen(pMtd, 0);
		/* For UBI images 'ubiformat' takes care of erasing the partition */
		if (pMtd->eFileType != FTUBI) {
			MtdPartErase(pMtd);
		}
		if (PTBootstream == pMtd->ePartType) {
			char cmd[1024];

			/* Bootstream partitions must be updated by Freescale kobs-ng application */
			/* Close open mtd device */
			MtdPartClose(pMtd);
			/* Build command to call kobs-ng
			 * Use 'strcat' because -O2 compiler optimization
			 * creates problems with 'sprintf' */
			strcpy(cmd, "kobs-ng init -w");
			if (cVerify)
				strcat(cmd, " -c");
			if (NULL != szKey) {
				strcat(cmd, " -k");
				strcat(cmd, szKey);
			}
			strcat(cmd, " --chip_0_device_path=");
			strcat(cmd, pMtd->acName);
			strcat(cmd, " ");
			strcat(cmd, pMtd->szImageFileName);
			strcat(cmd, " > /dev/null");
			if (!system(cmd)) {
				PRINTF("\r    Flashing:  complete                         \n");
				if (cVerify) {
					PRINTF("\r    Verifying: complete                   \n");
				}
			}
			else {
				PRINTF("\r    Flashing:  FAILED!                          \n");
				exit(EXIT_FAILURE);
			}
		} else if (pMtd->eFileType == FTUBI) {
			/* UBI images are flashed using 'ubiformat' command */
			MtdPartClose(pMtd);
			mtd_part_write_ubi(pMtd);
		} else {
			MtdPartWrite(pMtd);
			if (cVerify) {
				if (!MtdPartVerify(pMtd))
					exit(EXIT_FAILURE);
				/* if CRC32 is given, it has been already checked on
				   the input files. And Mtd is now same to them. So no
				   need to check CRC32 again. */
			} else if (cHasChecksum)
				MtdPartCheckCRC32(pMtd);
			MtdPartClose(pMtd);
		}

		PRINTF("    CRC32:     0x%08x\n", pMtd->uiCRC32);
	}

	if (NULL != szTmpDir) {
		/* delete all temporary files */
		for (pMtd = axMtdParts; pMtd < pMtdPartLastToUpdate; pMtd++)
			if (unlink(pMtd->szImageFileName)) {
				/*
				 * Do not exit with error in case the file to remove does not
				 * exist.
				 * Try to address corner cases like CCORE_MX53_EXTENSIONS-170
				 * (using same file to flash several different partitions)
				 */
				if (ENOENT == errno) {
					systemLog("%s", pMtd->szImageFileName);
				} else {
					systemError("%s", pMtd->szImageFileName);
				}
			}
	}
}

/***********************************************************************
 * !Function: DoMtdVerify
 * !Descr:    Verifies the flash images
 ***********************************************************************/
static void DoMtdVerify(void)
{
	mtdPartition_t *pMtd = NULL;

	PRINTF("Verifying Images:\n");

	for (pMtd = axMtdParts; pMtd < pMtdPartLastToUpdate; pMtd++) {
		MtdPartOpen(pMtd, 1);
		MtdPartVerify(pMtd);
		MtdPartClose(pMtd);
	}
}

/***********************************************************************
 * !Function: OnExit
 * !Descr:    Closes all open mtd partitions
 ***********************************************************************/
static void OnExit(void)
{
	mtdPartition_t *pMtd;

	for (pMtd = axMtdParts; pMtd < pMtdPartLastToUpdate; pMtd++) {
		if (-1 != pMtd->iFd)
			MtdPartClose(pMtd);

		FREE(pMtd->szImageFileName);
	}
}

/***********************************************************************
 * !Function: MtdInit
 * !Descr:    Checks where the partitions (devfs or not) and how many we have
 ***********************************************************************/
static void MtdInit(void)
{
	struct stat xStat;

	CLEAR(xStat);

	/* determine whether we are /dev/mtd/ or /dev/mtd */
	if (-1 == stat(szMtdPrefix, &xStat)) {
		/* not dev fs */
		if (-1 == stat("/dev/mtd0", &xStat))
			error("No MTD devices available");

		szMtdPrefix = "/dev/mtd";
		szMtdBlockPrefix = "/dev/mtdblock";
	}

	/* determine number of partitions */
	while (uiMtdPartsCount < ARRAY_SIZE(axMtdParts)) {
		char acName[64];

		axMtdParts[uiMtdPartsCount].iFd = -1;	/* not open yet */

		sprintf(acName, "%s%u", szMtdPrefix, uiMtdPartsCount);
		if (-1 == stat(acName, &xStat))
			break;

		uiMtdPartsCount++;
	}

	if (!uiMtdPartsCount)
		error("No MTD partitions found");
}

/***********************************************************************
 * !Function: MtdPartInit
 * !Descr:    opens partition and initializes all sizes and bad blocks
 ***********************************************************************/
static void MtdPartInit( /*@out@ */ mtdPartition_t * pMtd, unsigned int uiPartition,
			const char *szImageFileName)
{
	CLEAR(*pMtd);
	pMtd->iFd = -1;
	pMtd->uiPartition = uiPartition;

	/* open mtd (may be /dev/mtd0 or /dev/mtd/0) */
	MtdPartOpen(pMtd, 0);

	/* read partition info */
	if (ioctl(pMtd->iFd, MEMGETINFO, &pMtd->xInfo))
		systemError("ioctl( MEMGETINFO )");

	pMtd->iSize = pMtd->xInfo.size;
	pMtd->cIsNAND = (MTD_NANDFLASH == pMtd->xInfo.type);
	pMtd->uiBadBlocks = 0;
	pMtd->cEraseAll = cEraseAll;
	pMtd->cWriteCleanMarker = cWriteCleanMarker;

	if (pMtd->cIsNAND) {
		/* determine bad block count */
		/* !TODO. in 2.6.18 there already exists
		   mtd_ecc_stats/ECCGETLAYOUT */
		loff_t iOffset = 0;

		while (iOffset < pMtd->xInfo.size) {
			if (MtdPartIsBadBlock(pMtd, iOffset)) {
				logMsg(LOG_HARDWARE1,
				       "Bad Block 0x%08llx on partition %u",
				       iOffset, pMtd->uiPartition);
				pMtd->uiBadBlocks++;
			}
			iOffset += pMtd->xInfo.erasesize;
		}
	}

	pMtd->iPageSize = (pMtd->cIsNAND ? pMtd->xInfo.writesize : pMtd->xInfo.erasesize);

	MtdPartDeterminePartType(pMtd);

	PRINTF("Partition %u is %s (%s)\n", pMtd->uiPartition,
		(pMtd->cIsNAND ? "NAND" : ((MTD_NORFLASH == pMtd->xInfo.type) ? "NOR" : "???")),
		(pMtd->ePartType != PTUnknown) ? aszPartType[pMtd->ePartType] : "");
	PRINTF("  Full Size: %llu KiB\n", TO_KiB(pMtd->iSize));

	if (pMtd->uiBadBlocks) {
		/* determine effective (good) size */
		PRINTF("  %u bad blocks\n", pMtd->uiBadBlocks);
		pMtd->iSize -= pMtd->xInfo.erasesize * pMtd->uiBadBlocks;
	}

	PRINTF("  Good Size: %llu KiB\n", TO_KiB(pMtd->iSize));

	if (!is_nand_oob_atomic())
		MtdPartInitCleanMarker(pMtd, &pMtd->xCleanMarker, &pMtd->uiClMPos, &pMtd->uiClMLen);

	MtdPartUseFile(pMtd, szImageFileName);

	/* close it to not leave an unused file descriptor open too long.
	   The partition info is not gonna change anyway. */
	MtdPartClose(pMtd);

	if (!cVerifyOnly && pMtd->cIsJFFS2) {
		PRINTF(INFO "JFFS2 partition %u will be fully erased",
			pMtd->uiPartition);
		/* clean rootfs completely */
		pMtd->cEraseAll = 1;
		if (!is_nand_oob_atomic()) {
			pMtd->cWriteCleanMarker = 1;
			PRINTF(" and clean markers written\n");
		} else {
			PRINTF("\n");
		}
	}
}

/***********************************************************************
 * !Function: MtdPartOpen
 * !Descr:    Opens the partition, either read only or read-writable
 ***********************************************************************/
static void MtdPartOpen( /*@inout@ */ mtdPartition_t * pMtd, char cReadOnly)
{
	char cReallyReadOnly = cReadOnly || cVerifyOnly;
	if (-1 != pMtd->iFd)
		error("Partition %u already open", pMtd->uiPartition);

	sprintf(pMtd->acName, "%s%u", szMtdPrefix, pMtd->uiPartition);
	pMtd->iFd = open(pMtd->acName, (cReallyReadOnly ? O_RDONLY : OPEN_READWRITE_IF_NOT_DRY));
	if (-1 == pMtd->iFd)
		systemError(": %s", pMtd->acName);
}

/***********************************************************************
 * !Function: MtdPartClose
 * !Descr:    closes the Mtd Partition
 ***********************************************************************/
static void MtdPartClose( /*@inout@ */ mtdPartition_t * pMtd)
{
	CLOSE(pMtd->iFd);
}

/***********************************************************************
 * !Function: MtdPartIsBadBlock
 * !Return:   1 if the block at iOffset is bad and mustn't be used
 * !TODO:     on first run, all bad blocks can be stored in a "bad" list and
 *            reused later to reduce kernel calls
 ***********************************************************************/
static int MtdPartIsBadBlock(const mtdPartition_t * pMtd, loff_t iOffset)
{
	char cIsBad = 0;
	int iRes = ioctl(pMtd->iFd, MEMGETBADBLOCK, &iOffset);

	if (iRes > 0)
		cIsBad = 1;
	else if ((iRes < 0) && (ENOTSUP != errno))
		/* if not supported (NOR), assume it is good */
		systemError("ioctl( MEMGETBADBLOCK )");

	return cIsBad;
}

/***********************************************************************
 * !Function: MtdMarkBadBlock
 * !Return:   0 on success, <1 on error
 ***********************************************************************/
static int MtdMarkBadBlock(const mtdPartition_t * pMtd, loff_t iOffset)
{
	PRINTF("Marking offset %d as bad\n",(int)iOffset);
	return ( ioctl(pMtd->iFd, MEMSETBADBLOCK, &iOffset) );
}

/***********************************************************************
 * !Function: MtdPartUseFile
 * !Descr:    Checks whether szImageFileName can be used for updating.
 *            E.g. if !cNoImageTypeCheck is set, the prefixes of the image
 *            file names are checked.
 ***********************************************************************/
static void MtdPartUseFile( /*@inout@ */ mtdPartition_t * pMtd, const char *szImageFileName)
{
	struct stat xStat;

	CLEAR(xStat);
	if (stat(szImageFileName, &xStat))
		systemError("%s", szImageFileName);

	pMtdPartLastToUpdate->szOrigImageFileName = szImageFileName;
	pMtd->szImageFileName = strdup(szImageFileName);
	pMtd->iFileSize = xStat.st_size;
	pMtd->eFileType = FTUnknown;

	if (-1 != pMtd->iFd) {
		if (xStat.st_size > pMtd->iSize)
			error("File %s is %lu KiB, but partition %u has only %lu KiB good free",
			      pMtd->szImageFileName,
			      TO_KiB(xStat.st_size), pMtd->uiPartition, TO_KiB(pMtd->iSize));

		if (!cNoImageTypeCheck)
			MtdPartDetermineAndCheckFileType(pMtd);
	}
}

/***********************************************************************
 * !Function: MtdPartErase
 * !Descr:    erases the flash partition.
 *            !see [1]
 ***********************************************************************/
static void MtdPartErase(const mtdPartition_t * pMtd)
{
	erase_info_t xErase;
	loff_t iEraseSize = 0;
	loff_t iBytesErasedTotal = 0;
	char cLastWasBad = 0;
	struct mtd_oob_buf xoob;
	int iThrottle = MtdPartGetThrottle(pMtd, pMtd->xInfo.erasesize);

	CLEAR(xErase);

	xErase.length = pMtd->xInfo.erasesize;
	/* bad sectors have already been removed in pMtd->iSize */
	iEraseSize = (pMtd->cEraseAll ? pMtd->iSize : pMtd->iFileSize);

	CLEAR(xoob);
	xoob.length = pMtd->uiClMLen;
	xoob.ptr = (unsigned char *)&pMtd->xCleanMarker;

	while (iBytesErasedTotal < iEraseSize) {
		if (!MtdPartIsBadBlock(pMtd, xErase.start)) {
			PrintProgress((((iBytesErasedTotal) * 100) / iEraseSize),
				      iThrottle,
				      "    Erasing%s %i KiB @ 0x%08x:",
				      (pMtd->cWriteCleanMarker ? " (CM)" : ""),
				      TO_KiB(xErase.length), xErase.start);

			if (!cDryRun && ioctl(pMtd->iFd, MEMERASE, &xErase))
				systemError("ioctl(MEMERASE)");

			iBytesErasedTotal += xErase.length;
			cLastWasBad = 0;

			if (!cDryRun && pMtd->cWriteCleanMarker) {
				/* write cleanmarker */
				if (pMtd->cIsNAND) {
					xoob.start = xErase.start + pMtd->uiClMPos;
					if (ioctl(pMtd->iFd, MEMWRITEOOB, &xoob))
						systemError("ioctl( MEMWRITEOOB )");
				} else {
					/* the NOR image already contains them. */
					if (pwrite(pMtd->iFd, &pMtd->xCleanMarker,
						   sizeof(pMtd->xCleanMarker),
						   xErase.start) != sizeof(pMtd->xCleanMarker))
						systemError("pwrite");
				}
			}
		} else {
			logMsg(LOG_HARDWARE1,
			       "%s" WARNING "Skipping bad sector @ 0x%08x           ",
			       (!cLastWasBad ? "\r" : ""), xErase.start);
			cLastWasBad = 1;
		}

		xErase.start += xErase.length;
	}

	PRINTF("\r    Erasing:   complete                                      \n");
}

/***********************************************************************
 * !Function: mtd_part_write_ubi
 * !Descr:    writes an UBI image file to partition
 ***********************************************************************/
static void mtd_part_write_ubi(mtdPartition_t * pMtd)
{
	char line[256];
	FILE *fpin;
	int ret;

	snprintf(line, sizeof(line), "ubiformat %s -f %s -y -q 2>&1 >/dev/null", pMtd->acName,
		 pMtd->szImageFileName);
	fpin = popen(line, "r");
	if (fgets(line, sizeof(line) - 1, fpin) != NULL) {
		line[strlen(line) - 1] = 0;
	}
	ret = pclose(fpin);
	if (!WEXITSTATUS(ret)) {
		PRINTF("\r    Flashing:  complete                         \n");
	} else {
		PRINTF("\r    Flashing:  FAILED! (%s)\n", line);
		exit(EXIT_FAILURE);
	}
}

/***********************************************************************
 * !Function: MtdPartWrite
 * !Descr:    writes the image file to partition
 ***********************************************************************/
static void MtdPartWrite(mtdPartition_t * pMtd)
{
	loff_t iBytesReadTotal = 0;
	loff_t iCurrentOffs = 0;
	int iFdSrc = -1;
	char cLastWasBad = 0;
	int iBytesRead = 0;
	uint32_t uiCRC32 = 0;
	unsigned char *pucBuffer = NULL;
	const size_t iBlockSize = pMtd->iPageSize;
	int iThrottle = MtdPartGetThrottle(pMtd, pMtd->iFileSize);
	unsigned int i,iRet;
	unsigned int ref_uicrc32 = 0,new_uicrc32 = 0;
	unsigned char *pucBufferMtd = NULL;

	pucBuffer = (unsigned char *)malloc(iBlockSize);
	if (NULL == pucBuffer)
		systemError("malloc");

	iFdSrc = open(pMtd->szImageFileName, O_RDONLY);
	if (-1 == iFdSrc)
		systemError("%s", pMtd->szImageFileName);

	/* rewind because descriptor was open and we don't know the state */
	if (lseek(pMtd->iFd, 0, SEEK_SET))
		systemError("%s", pMtd->acName);

	do {
		/* write one sector */
		if (!MtdPartIsBadBlock(pMtd, iCurrentOffs)) {
			int iBytesWritten;

			PrintProgress(PERCENTAGE(iBytesReadTotal, pMtd->iFileSize),
				      iThrottle, "    Flashing:");

			iBytesRead = read(iFdSrc, pucBuffer, iBlockSize);
			if (!iBytesRead)
				break;
			else if (-1 == iBytesRead)
				systemError("%s", pMtd->szImageFileName);

			if (!pMtd->cChecksumCalculated)
				uiCRC32 = crc32(uiCRC32, pucBuffer, iBytesRead);

			if (iBytesRead < iBlockSize) {
				char cEmptyChar = FLASH_ERASED_BYTE;
				char bJFFS2Padding = 0;

				if (pMtd->cIsJFFS2 &&
				    (iBytesRead + sizeof(struct jffs2_unknown_node) <
				     iBlockSize)) {
					bJFFS2Padding = 1;
					cEmptyChar = 0;		/* see wbuf.c */
				}

				/* fill block with empty characters */
				memset(pucBuffer + iBytesRead,
				       cEmptyChar, iBlockSize - iBytesRead);

				if (bJFFS2Padding) {
					/* write padding to avoid Empty block messages.
					   see linux/fs/jffs2/wbuf.c:flush_wbuf */
					struct jffs2_unknown_node *pNode =
					    (struct jffs2_unknown_node *)(pucBuffer +
									  iBytesRead);
					logMsg(LOG_HARDWARE1, "\nPadding last sector");
					MtdPartInitJFFS2Node(pNode,
							     JFFS2_NODETYPE_PADDING,
							     iBlockSize - iBytesRead);
				}
			}

			/* at least nand writes should be aligned */

			if( !cDryRun && cMarkBadBlocks ) {
				ref_uicrc32 = crc32(0, pucBuffer, iBlockSize);
				pucBufferMtd = (unsigned char *)malloc(iBlockSize);
				if (NULL == pucBufferMtd)
					systemError("malloc");
			}

			for( i = 0 ; i < cMaxRetries ; i++ ) {
				int iBytesReadMtd;

			iBytesWritten = (!cDryRun ?
					 pwrite(pMtd->iFd,
						pucBuffer, iBlockSize,
						iCurrentOffs) : iBlockSize);

				if (iBytesWritten != iBlockSize) {
					if( !cDryRun && cMarkBadBlocks ) {
						PRINTF("[%s:%d] %s: Retrying failed write %d.\n",
							__FUNCTION__,__LINE__,pMtd->acName,i);
						continue;
					}
					else {
				systemError("%s", pMtd->acName);
					}
				}

				if (!cDryRun && cMarkBadBlocks) {
					iBytesReadMtd = pread(pMtd->iFd, pucBufferMtd, iBlockSize,
										  iCurrentOffs);
					if ( iBytesReadMtd < 0 ) {
						PRINTF("[%s:%d] %s: Read error.\n",
							__FUNCTION__,__LINE__,pMtd->acName);
						continue;
					}

					new_uicrc32 = crc32(0, pucBufferMtd, iBlockSize);
					if( new_uicrc32 != ref_uicrc32 ) {
						PRINTF("[%s:%d] %s: CRC mismatch %08x <> %08x.\n",
							   __FUNCTION__,__LINE__,pMtd->acName,ref_uicrc32,
							   new_uicrc32);
							   continue;
					}
					FREE(pucBufferMtd);
				}
				break;
			}

			if( !cDryRun && cMarkBadBlocks && (i >= cMaxRetries) ) {
				PRINTF("[%s:%d] %s: Marking as bad block.\n",
					     __FUNCTION__,__LINE__,pMtd->acName);
				iRet = MtdMarkBadBlock( pMtd , iCurrentOffs );
				systemError("%s: Bad block marking %s.", pMtd->acName,
					    strerror(iRet));
			}

			iBytesReadTotal += iBytesRead;

			cLastWasBad = 0;
		} else {
			logMsg(LOG_HARDWARE1,
			       "%s" WARNING "Skipping bad sector @ 0x%08x        ",
			       (!cLastWasBad ? "\r" : ""), iCurrentOffs);

			cLastWasBad = 1;
		}

		iCurrentOffs += iBlockSize;
	} while (iCurrentOffs < pMtd->xInfo.size);

	CLOSE(iFdSrc);
	FREE(pucBuffer);

	if (pMtd->iFileSize != iBytesReadTotal)
		error("Filesize changed while updating: %s", pMtd->szImageFileName);

	if (!pMtd->cChecksumCalculated) {
		SET_CRC32(pMtd, uiCRC32);
		pMtd->cChecksumCalculated = 1;
	}

	PRINTF("\r    Flashing:  complete                         \n");
}

/***********************************************************************
 * !Function: MtdPartVerify
 * !Return:   1 if identical in the used sectors of szOrigImageFileName
 *            otherwise 0
 * !Descr:    compares contents of MtdPartition with szOrigImageFileName
 *            Only checks the last used sector whether the parts the source
 *            file not uses are empty. Other blocks are not checked.
 ***********************************************************************/
static int MtdPartVerify(mtdPartition_t * pMtd)
{
	unsigned char *pucBufferSrc;
	unsigned char *pucBufferMtd;
	int iFdSrc = -1;
	loff_t iBytesReadTotal = 0;
	loff_t iOffsMtd = 0;
	char cRes = 0;
	char cLastWasBad = 0;
	const size_t iBlockSize = pMtd->iPageSize;
	int iThrottle = MtdPartGetThrottle(pMtd, pMtd->iFileSize);
	unsigned int i,iRet;

	pucBufferSrc = (unsigned char *)malloc(iBlockSize);
	if (NULL == pucBufferSrc)
		systemError("malloc");

	pucBufferMtd = (unsigned char *)malloc(iBlockSize);
	if (NULL == pucBufferMtd)
		systemError("malloc");

	/* sync everything */
	MtdPartClose(pMtd);
	MtdPartOpen(pMtd, 1);	/* also rewinds read pointer */

	iFdSrc = open(pMtd->szOrigImageFileName, O_RDONLY);
	if (-1 == iFdSrc)
		systemError("%s", pMtd->szOrigImageFileName);

	do {
		if (!MtdPartIsBadBlock(pMtd, iOffsMtd)) {
			int iBytesReadSrc;
			int iBytesReadMtd;
			loff_t iOffs;

			PrintProgress(PERCENTAGE(iBytesReadTotal, pMtd->iFileSize),
				      iThrottle, "    Verifying:");

			iBytesReadSrc = read(iFdSrc, pucBufferSrc, iBlockSize);
			if (!iBytesReadSrc)
				/* nothing left */
				break;
			else if (iBytesReadSrc < 0)
				systemError("%s", pMtd->szOrigImageFileName);

			for( i = 0 ; i < cMaxRetries ; i++ ) {
			iBytesReadMtd = pread(pMtd->iFd, pucBufferMtd, iBlockSize, iOffsMtd);
				if (iBytesReadMtd < 0) {
					if( !cDryRun && cMarkBadBlocks ) {
						printf("[%s:%d] %s: Retrying failed read %d, %s.\n",
							__FUNCTION__,__LINE__,pMtd->acName,i,strerror(errno));
						continue;
					}
					else
				systemError("%s", pMtd->acName);
				}
				break;
			}

			if( !cDryRun && cMarkBadBlocks && (i >= cMaxRetries) ) {
				PRINTF("[%s:%d] %s: Marking as bad block.\n",
				   __FUNCTION__,__LINE__,pMtd->acName);
				iRet = MtdMarkBadBlock( pMtd , iOffsMtd );
				systemError("%s: Bad block marking %s.", pMtd->acName,  strerror(iRet));
			}

			if (iBytesReadSrc > iBytesReadMtd) {
				logMsg(LOG_ERR,
				       "\nSize mismatch. Source has %i Bytes but flash only %i bytes",
				       iBytesReadSrc, iBytesReadMtd);
				goto ret;
			}

			iOffs = MemCmp(pucBufferSrc, pucBufferMtd, iBytesReadSrc);
			if (-1 != iOffs) {
				logMsg(LOG_ERR,
				       "\nData mismatch @ 0x%08x\n", iBytesReadTotal + iOffs);
				logMsg(LOG_ERR, "Source is");
				MemDump(pucBufferSrc, iOffs & ~0xf, MIN(iBytesReadSrc, 0x20));
				logMsg(LOG_ERR, "Flash is");
				MemDump(pucBufferMtd, iOffs & ~0xf, MIN(iBytesReadMtd, 0x20));
				goto ret;
			}

			if (!pMtd->cIsJFFS2 && (iBytesReadSrc < iBytesReadMtd)) {
				/* !TODO. JFFS2 is padded.
				   This is not yet checked. */
				int i;

				/* check for emptiness of block. */
				/* !TODO. This works only for NOR and NAND */
				for (i = iBytesReadSrc; i < iBytesReadMtd; i++)
					if (FLASH_ERASED_BYTE != pucBufferMtd[i]) {
						logMsg(LOG_ERR,
						       "Mtd is not empty @ 0x%llx\n",
						       iBytesReadTotal + i);
						goto ret;
					}
			}	/* if( iBytesReadSrc < iBytesReadMtd */
			iBytesReadTotal += iBytesReadSrc;

			cLastWasBad = 0;
		} else {
			logMsg(LOG_HARDWARE1,
			       "%s" WARNING "Skipping bad sector @ 0x%08x           ",
			       (!cLastWasBad ? "\r" : ""), iOffsMtd);
			cLastWasBad = 1;
		}

		iOffsMtd += iBlockSize;
	} while (iOffsMtd < pMtd->xInfo.size);

	if (pMtd->iFileSize != iBytesReadTotal) {
		logMsg(LOG_ERR, "Filesize changed while updating: %s", pMtd->szImageFileName);
		goto ret;
	}

	PRINTF("\r    Verifying: complete                   \n");

	cRes = 1;

ret:
	CLOSE(iFdSrc);

	FREE(pucBufferMtd);
	FREE(pucBufferSrc);

	return cRes;
}

/***********************************************************************
 * !Function: MtdPartCheckCRC32
 * !Descr:    calculates checksum of mtd partition up to filesize
 ***********************************************************************/
static void MtdPartCheckCRC32(mtdPartition_t * pMtd)
{
	unsigned char *pucBufferMtd;
	loff_t iBytesReadTotal = 0;
	loff_t iOffsMtd = 0;
	uint32_t uiCRC32 = 0;
	const size_t iBlockSize = pMtd->xInfo.erasesize;
	int iThrottle = MtdPartGetThrottle(pMtd, pMtd->iFileSize);

	pucBufferMtd = (unsigned char *)malloc(iBlockSize);
	if (NULL == pucBufferMtd)
		systemError("malloc");

	/* sync everything */
	MtdPartClose(pMtd);
	MtdPartOpen(pMtd, 1);	/* also rewinds read pointer */

	do {
		if (!MtdPartIsBadBlock(pMtd, iOffsMtd)) {
			int iBytesReadMtd;

			PrintProgress(PERCENTAGE(iBytesReadTotal, pMtd->iFileSize),
				      iThrottle, "    CRC32:   ");

			iBytesReadMtd = pread(pMtd->iFd, pucBufferMtd, iBlockSize, iOffsMtd);
			if (iBytesReadMtd < 0)
				systemError("%s", pMtd->acName);

			if ((iBytesReadTotal + iBytesReadMtd) > pMtd->iFileSize)
				iBytesReadMtd = (pMtd->iFileSize - iBytesReadTotal);

			uiCRC32 = crc32(uiCRC32, pucBufferMtd, iBytesReadMtd);
			iBytesReadTotal += iBytesReadMtd;
		}
		iOffsMtd += iBlockSize;
	} while (pMtd->iFileSize > iBytesReadTotal);

	PRINTF("\r    CRC32:     complete                   \n");

	FREE(pucBufferMtd);

	MtdPartCompareCRC32(pMtd, uiCRC32);
}

/***********************************************************************
 * !Function: MtdPartCopyFile
 * !Descr:    copies the image file to szDstFileName and calculates its
 *            check sum
 ***********************************************************************/
static void MtdPartCopyFile(mtdPartition_t * pMtd, const char *szDstFileName)
{
	unsigned char *pucBuffer = NULL;
	int iFdDst = -1;
	int iFdSrc = -1;
	loff_t iBytesReadTotal = 0;
	int iBytesRead;
	uint32_t uiCRC32 = 0;
	const size_t iBlockSize = pMtd->xInfo.erasesize;

	pucBuffer = (unsigned char *)malloc(iBlockSize);
	if (NULL == pucBuffer)
		systemError("malloc");

	iFdSrc = open(pMtd->szImageFileName, O_RDONLY);
	if (-1 == iFdSrc)
		systemError("%s", pMtd->szImageFileName);

	iFdDst = open(szDstFileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (-1 == iFdDst)
		systemError("%s", szDstFileName);

	do {
		int iBytesWritten;

		iBytesRead = read(iFdSrc, pucBuffer, iBlockSize);
		if (!iBytesRead)
			break;
		else if (-1 == iBytesRead)
			systemError("%s", pMtd->szImageFileName);

		if (pMtd->cIsJFFS2)
			MtdPartVerifyJFFS2Block(pMtd, pucBuffer, iBytesRead);

		uiCRC32 = crc32(uiCRC32, pucBuffer, iBytesRead);

		iBytesWritten = write(iFdDst, pucBuffer, iBytesRead);
		if (iBytesWritten != iBytesRead)
			systemError("%s", szDstFileName);

		iBytesReadTotal += iBytesRead;
	} while (1);

	CLOSE(iFdDst);
	CLOSE(iFdSrc);

	FREE(pucBuffer);

	if (pMtd->iFileSize != iBytesReadTotal)
		error("Filesize changed while updating: %s", pMtd->szImageFileName);

	if (!pMtd->cChecksumSet) {
		SET_CRC32(pMtd, uiCRC32);
		pMtd->cChecksumCalculated = 1;
	} else
		MtdPartCompareCRC32(pMtd, uiCRC32);
}

/***********************************************************************
 * !Function: MtdPartInitCleanMarker
 * !Descr:    initializes the clean marker
 *            !see [1]
 ***********************************************************************/
static void MtdPartInitCleanMarker(const mtdPartition_t * pMtd,
				   struct jffs2_unknown_node *pCleanMarker,
				   unsigned int *puiClMPos, unsigned int *puiClMLen)
{
	*puiClMPos = 0;
	*puiClMLen = 8;

	if (pMtd->cIsNAND) {
		struct nand_oobinfo xoobInfo;

		CLEAR(xoobInfo);
		if (ioctl(pMtd->iFd, MEMGETOOBSEL, &xoobInfo))
			systemError("ioctl( MEMGETOOBSEL )");

		/* check for autoplacement */
		if (MTD_NANDECC_AUTOPLACE == xoobInfo.useecc) {
			/* get the position of the free bytes */
			if (!xoobInfo.oobfree[0][1])
				error("Autoplacement selected and no empty space in oob\n");

			*puiClMPos = xoobInfo.oobfree[0][0];
			*puiClMLen = xoobInfo.oobfree[0][1];

			if (*puiClMLen > 8)
				*puiClMLen = 8;
			else {
				/* legacy mode, detect autoplacement ourselves [3] */
				switch (pMtd->xInfo.oobsize) {
				case 8:
					*puiClMPos = 6;
					*puiClMLen = 2;
					break;
				case 16:
					*puiClMPos = 8;
					*puiClMLen = 8;
					break;
				case 64:
					*puiClMPos = 16;
					*puiClMLen = 8;
					break;
				default:
					error("unsupported oobsize %i\n", pMtd->xInfo.oobsize);
					break;
				}	/* switch */
			}	/* if( *piClMPos ) */
		}		/* if( NAND_AUTOPLACE */
	} else
		MtdPartInitJFFS2Node(pCleanMarker,
				     JFFS2_NODETYPE_CLEANMARKER,
				     sizeof(struct jffs2_unknown_node));

	logMsg(LOG_HARDWARE2,
	       " OOB has %i bytes, CleanMarker is from 0x%02x to 0x%02x",
	       pMtd->xInfo.oobsize, *puiClMPos, *puiClMPos + *puiClMLen);
}

/***********************************************************************
 * !Function: MtdPartInitJFFS2Node
 * !Descr:    initializes a node and generates hdr checksum
 ***********************************************************************/
static void MtdPartInitJFFS2Node(struct jffs2_unknown_node *pNode, unsigned short uhNodeType,
				 size_t iLen)
{
	uint32_t uiStart = 0xffffffff;	/* JFFS CRC32 starts from 0xfffffff, our crc32 from 0x0 */

	CLEAR(*pNode);

	pNode->magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	pNode->nodetype = cpu_to_je16(uhNodeType);

	pNode->totlen = cpu_to_je32(iLen);
	/* don't CRC32 the hdr_crc itself */
	pNode->hdr_crc =
	    cpu_to_je32(crc32(uiStart, pNode, sizeof(struct jffs2_unknown_node) - 4) ^ uiStart);
}

/***********************************************************************
 * !Function: MtdPartCompareCRC32
 * !Descr:    checks whether the CRC32 is same as previously calculated.
 ***********************************************************************/
static void MtdPartCompareCRC32(mtdPartition_t * pMtd, uint32_t uiCRC32)
{
	if (uiCRC32 != pMtd->uiCRC32)
		error("CRC32 mismatch on %s.\n    Expected 0x%08x, have 0x%08x\n",
		      pMtd->szImageFileName, pMtd->uiCRC32, uiCRC32);
}

/***********************************************************************
 * !Function: MtdPartRemountAllReadOnly
 * !Descr:    remounts all uses of the partition as read-only
 ***********************************************************************/
static void MtdPartRemountAllReadOnly(mtdPartition_t * pMtd)
{
	static const char *szMount = "/proc/mounts";
	char acMtdBlock[20];
	FILE *fhMount = NULL;
	const char *szRootDev = GetRootDevice();

	if (pMtd->cAlreadyRemounted)
		/* nothing to do */
		return;

	fhMount = setmntent(szMount, "r");
	if (NULL == fhMount)
		systemError(szMount);

	sprintf(acMtdBlock, "%s%u", szMtdBlockPrefix, pMtd->uiPartition);

	/* scan all mount entries */
	do {
		char bIsRootDev = 0;
		char bRemount = 0;

		struct mntent *pxEnt = getmntent(fhMount);
		if (NULL == pxEnt)
			break;

		bIsRootDev = (!strcmp(pxEnt->mnt_fsname, "/dev/root") &&
			      !strcmp(szRootDev, acMtdBlock));
		if (!bIsRootDev && !strcmp(pxEnt->mnt_fsname, acMtdBlock)) {
			PRINTF(WARNING "Umounting %s\n", pxEnt->mnt_dir);
			if (umount(pxEnt->mnt_dir) || bIsRootDev) {
				/* rootdev can't be unmounted */
				/* it's for our mtd partition, remount it */
				PRINTF(WARNING "Failed, trying to remount read-only\n");
				bRemount = 1;
			}
		}
		if (bIsRootDev || bRemount) {
			PRINTF(WARNING "Remounting %s\n", pxEnt->mnt_dir);
			if (mount(pxEnt->mnt_fsname, pxEnt->mnt_dir,
				  NULL, MS_REMOUNT | MS_RDONLY, NULL)) {
				if (EBUSY == errno)
					error("Partition in use, can't update it\n");
				else
					systemError("%s", pxEnt->mnt_fsname);
			}
		}
	} while (1);

	endmntent(fhMount);

	pMtd->cAlreadyRemounted = 1;
}

/***********************************************************************
 * !Function: MtdPartDeterminePartType
 * !Descr:    Determine partition type (rootfs, uboot, etc.) based on
 *            partition name in /proc/mtd
 ***********************************************************************/
static void MtdPartDeterminePartType(mtdPartition_t * pMtd)
{
	static const char *szMtd = "/proc/mtd";
	FILE *fhMtd = NULL;
	unsigned int uiPart;
	char acBuffer[200];
	int iPart;
	loff_t uiSize;
	loff_t uiEraseSize;
	char acName[200];
	static const struct {
		PartType_e eType;
		FileType_e eFileType;
		const char *szName;
	} axTypes[] = {
		{PTUBoot,       FTUBoot,       "\"U-Boot"},
		{PTKernel,      FTKernel,      "\"Kernel"},
		{PTEnvironment, FTNVRAM,       "\"NVRAM"},
		{PTFPGA,        FTFPGA,        "\"FPGA"},
		{PTBootstream,  FTBootstream,  "\"Bstrm-U-Boot"},
	};
	int i;

	/* open /proc/mtd */
	fhMtd = fopen(szMtd, "r");
	if (NULL == fhMtd)
		systemError(szMtd);

	/* skip table header (check return value to avoid compiler warning) */
	if (fgets(acBuffer, sizeof(acBuffer), fhMtd));

	/* seek partition description */
	for (iPart = 0; iPart <= pMtd->uiPartition; iPart++)
		if (NULL == fgets(acBuffer, sizeof(acBuffer) - 1, fhMtd))
			systemError(szMtd);

	/* break it into parts */
	acBuffer[sizeof(acBuffer) - 1] = 0;
	if (sscanf(acBuffer, "mtd%u: %llx %llx %199s",
		   &uiPart, &uiSize, &uiEraseSize, acName) != 4)
		error("Wrong /proc/mtd line: %s", acBuffer);
	if (uiPart != pMtd->uiPartition)
		error("Wrong partition: %s", acBuffer);

	/* determine partition type */
	pMtd->ePartType = PTUnknown;
	pMtd->eFileTypeNeeded = FTUnknown;
	for (i = 0; i < ARRAY_SIZE(axTypes); i++) {
		if (!strncmp(axTypes[i].szName, acName, strlen(axTypes[i].szName))) {
			pMtd->ePartType = axTypes[i].eType;
			pMtd->eFileTypeNeeded = axTypes[i].eFileType;
			break;
		}
	}

	if (fclose(fhMtd) < 0)
		systemError(szMtd);
}

/***********************************************************************
 * !Function: MtdPartDetermineAndCheckFileType
 * !Descr:    Determine file type (rootfs, uboot, etc.) and checks whether it
 *            is ok for the partition. Done by looking at filename
 ***********************************************************************/
static void MtdPartDetermineAndCheckFileType(mtdPartition_t * pMtd)
{
	FileType_e eType = 0;
	int iMatch = 0;
	regex_t regexp;

	CLEAR(regexp);

	/* !TODO: Verify File Type, JFFS2, uimage, u-boot by looking into it */

	while (eType < ARRAY_SIZE(axFileType)) {
		/* does axFileType[ eType ] matches pMtd->szImageFileName? */
		int iError = regcomp(&regexp,
				     axFileType[eType].szExp,
				     REG_NOSUB);
		if (iError) {
			char acError[200];
			regerror(iError, &regexp, acError, sizeof(acError) - 1);
			error("%s", acError);
		}

		iMatch = !regexec(&regexp, pMtd->szImageFileName, 0, NULL, 0);
		regfree(&regexp);

		if (iMatch) {
			pMtd->eFileType = eType;

			logMsg(LOG_HARDWARE1,
			       "  Detected Image Type for %s: %s",
			       pMtd->szImageFileName, axFileType[eType].szName);

			/* check whether it matches the partition */
			if ((pMtd->eFileTypeNeeded != pMtd->eFileType) &&
			    (pMtd->eFileTypeNeeded != FTUnknown))
				error("File %s (Type %s) doesn't match partition type %s",
				      pMtd->szImageFileName,
				      axFileType[pMtd->eFileType].szName,
				      aszPartType[pMtd->ePartType]);

			if (FTJFFS2 == pMtd->eFileType)
				pMtd->cIsJFFS2 = 1;

			break;
		}
		eType++;
	}
}

/***********************************************************************
 * !Function: MtdPartVerifyFile
 * !Descr:    verifies the file (checksum, JFFS2)
 ***********************************************************************/
static void MtdPartVerifyFile(mtdPartition_t * pMtd)
{
	unsigned char *pucBuffer = NULL;
	uint32_t uiCRC32 = 0;
	int iFd = -1;
	int iBytesRead;
	const size_t iBlockSize = pMtd->xInfo.erasesize;

	pucBuffer = (unsigned char *)malloc(iBlockSize);
	if (NULL == pucBuffer)
		systemError("malloc");

	iFd = open(pMtd->szImageFileName, O_RDONLY);
	if (-1 == iFd)
		systemError("%s", pMtd->szImageFileName);

	do {
		iBytesRead = read(iFd, pucBuffer, iBlockSize);
		if (!iBytesRead)
			break;
		else if (-1 == iBytesRead)
			systemError("%s", pMtd->szImageFileName);

		if (pMtd->cIsJFFS2)
			MtdPartVerifyJFFS2Block(pMtd, pucBuffer, iBytesRead);

		uiCRC32 = crc32(uiCRC32, pucBuffer, iBytesRead);
	} while (1);

	CLOSE(iFd);

	FREE(pucBuffer);

	if (!pMtd->cChecksumSet) {
		SET_CRC32(pMtd, uiCRC32);
		pMtd->cChecksumCalculated = 1;
	} else
		MtdPartCompareCRC32(pMtd, uiCRC32);
}

/***********************************************************************
 * !Function: MtdPartVerifyJFFS2Block
 * !Descr:    verifies whether the JFFS2 block is correct. On NOR clean markers
 *            need to be on the correct position, on NAND there mustn't be.
 ***********************************************************************/
static void MtdPartVerifyJFFS2Block(mtdPartition_t * pMtd, unsigned char *pucData, size_t iSize)
{
	char cCleanMarkerPresent = 0;
	int iOffs;
	/* only magic and node, length field of xCleanMarker may vary */
	static const size_t NODE_SIZE = 4;

	if (pMtd->cAlreadyPrintedVerifyWarning)
		/* do it only once */
		return;

	if (iSize >= 4)
		cCleanMarkerPresent = (MemCmp(pucData, &pMtd->xCleanMarker, NODE_SIZE) == -1);

	if (pMtd->cIsNAND) {
		const jint16_t MAGIC = cpu_to_je16(JFFS2_MAGIC_BITMASK);

		if (cCleanMarkerPresent) {
			logMsg(LOG_ERR,
			       WARNING
			       "CleanMarkers present in image for NAND. JFFS2 will complain but function.");
			pMtd->cAlreadyPrintedVerifyWarning = 1;
		}

		/* MAGIC is < sizeof( xCleanMarker ) */
		if (MemCmp(pucData, &MAGIC, sizeof(MAGIC)) != -1)
			error("No JFFS2 Header at erase block begin");
	} else {
		/* NOR etc. */
		if (!cCleanMarkerPresent)
			error
			    ("No CleanMarkers present in image for NOR. JFFS2 won't like that. Possibly wrong erase block size of jffs2 or NAND image");

		/* there shouldn't be any other clean marker in the block.
		   Check 2^n offsets. */
		for (iOffs = 16; iOffs < iSize - NODE_SIZE; iOffs *= 2) {
			if (MemCmp(pucData + iOffs, &pMtd->xCleanMarker, NODE_SIZE) == -1)
				error
				    ("Clean Markers present in the block at offset 0x%08x. Possibly wrong erase block size of jffs2",
				     iOffs);
		}
	}
}

/***********************************************************************
 * !Function: MtdPartGetThrottle
 ***********************************************************************/
static int MtdPartGetThrottle(const mtdPartition_t * pMtd, uint64_t ullSize)
{
	/* Eclipse has a nicer output with an update every second. Therefore it
	 * is not throttled. */
	return 1;
}

/***********************************************************************
 * !Function: PrintProgress
 * !Descr:    Prints message only when progress has changed
 ***********************************************************************/
static void PrintProgress(int iPercentage, int iThrottle, const char *szFmt, ...)
{
	static int iLastPercentage = -1;
	int iThrottled = iPercentage / iThrottle;

	if (cSilent)
		/* nothing to print */
		return;

	if (iThrottled != iLastPercentage) {
		va_list args;

		iLastPercentage = iThrottled;

		va_start(args, szFmt);
		vprintf(szFmt, args);
		printf("% 3i%%          \r", iPercentage);
		va_end(args);

		if (cProgressInNewLine)
			PRINTF("\n");
		fflush(stdout);
	}
}

/***********************************************************************
 * !Function: VerifyTmpDir
 * !Descr:    checks whether the temporary directory can be used
 ***********************************************************************/
static void VerifyTmpDir(void)
{
	struct statfs xStat;

	CLEAR(xStat);

	if (statfs(szTmpDir, &xStat))
		systemError("statfs");

	switch (xStat.f_type) {
	case JFFS2_SUPER_MAGIC:
		/* flash image temporary on flash fs??? */
		error("Makes no sense to store flash image temporarily on JFFS2");
		break;		/* not reached */
	case NFS_SUPER_MAGIC:
		error("Makes no sense to store flash image temporarily on NFS");
		break;		/* not reached */
	default:
		/* Accept all other ones. If they are read-only, it is detected before
		 * erasing flash */
		break;
	}
}

/***********************************************************************
 * !Function: CalcCRC32OfFile
 * !Descr:    calculates the CRC32 of the file.
 ***********************************************************************/
static uint32_t CalcCRC32OfFile(const char *szFileName)
{
	char acBuffer[IO_BLOCK_SIZE];
	int iFd = -1;
	uint32_t uiCRC32 = 0;

	iFd = open(szFileName, O_RDONLY);
	if (-1 == iFd)
		systemError("%s", szFileName);

	do {
		int iBytesRead = read(iFd, acBuffer, IO_BLOCK_SIZE);

		if (!iBytesRead)
			break;
		else if (iBytesRead < 0)
			systemError("%s", szFileName);

		uiCRC32 = crc32(uiCRC32, acBuffer, iBytesRead);
	} while (1);

	CLOSE(iFd);

	return uiCRC32;
}

/***********************************************************************
 * !Function: GetRootDevice
 * !Descr:    determines the rootdevice from kernel command line root=
 ***********************************************************************/
static const char *GetRootDevice(void)
{
	int iFd = -1;
	int iRead;
	/* arm command line is at max 1024 Bytes */
	char szCmdLine[1024];
	static char szRootDev[32] = "";
	const char *szRootStart = NULL;

	if (*szRootDev)
		return szRootDev;

	iFd = open("/proc/cmdline", O_RDONLY);
	if (-1 == iFd)
		systemError("/proc/cmdline");

	iRead = read(iFd, szCmdLine, sizeof(szCmdLine) - 2);
	if (-1 == iRead)
		systemError("read");
	szCmdLine[iRead + 1] = 0;

	CLOSE(iFd);

	szRootStart = strstr(szCmdLine, "root=");
	if (NULL != szRootStart) {
		const char *szDev = szRootStart + 5;	/* strlen( root=) */
		const char *szDevEnd = szDev;
		int iLen;

		szDevEnd = strchr(szDev, ' ');
		iLen = ((NULL == szDevEnd) ? strlen(szDev) : (szDevEnd - szDev));

		strncpy(szRootDev, szDev, MIN(sizeof(szRootDev), iLen));
	}

	return szRootDev;
}
