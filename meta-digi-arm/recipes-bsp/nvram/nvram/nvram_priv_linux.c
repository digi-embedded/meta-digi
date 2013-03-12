/*
 *  nvram/src/nvram_priv_linux.c
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
 *  !Descr:      Defines the private functions needed by the nvram core to
 *               access I2C, Flash and a console for linux userspace.
 */

#define _XOPEN_SOURCE	500     /* for pread/pwrite */

#include <errno.h>              /* ENOTSUP */
#include <fcntl.h>              /* open */
#include <mtd/mtd-user.h>       /* MEMERASE */
#include <stdarg.h>             /* vprintf */
#include <sys/ioctl.h>          /* ioctl */
#include <sys/stat.h>           /* stat */

/* from libdigi */
#include <log.h>                /* systemError */
#include <misc_helper.h>        /* CLEAR */

#include "nvram_priv.h"

#define NVRAM_PARTITION	1

char g_markBadBlocks = 0;

/* ********** local variables ********** */
static char l_acMtd[32];
static int l_iFdMtd = -1;
static mtd_info_t l_xMtdInfo;
static unsigned char bbMaxRetries = 3;
extern void MemDump(const void *pvBase, loff_t iOffset, size_t iLen);

static char l_bManufMode = 0;

/* ********** global functions ********** */
void NvPrivLinuxSetMode(char bManufMode)
{
	/* Hack so linux nvram can change behavior of priv_linux */
	/* without ubootenv also changing. */
	l_bManufMode = bManufMode;
}

int NvPrivOSInit(void)
{
	struct stat xStat;

	CLEAR(xStat);

	/* detect NVRAM partition  */
	SPRINTF(l_acMtd, "/dev/mtd/%i", NVRAM_PARTITION);

	/* determine whether we are /dev/mtd/ or /dev/mtd */
	if (-1 == stat(l_acMtd, &xStat)) {
		SPRINTF(l_acMtd, "/dev/mtd%i", NVRAM_PARTITION);
		/* not dev fs */
		if (-1 == stat(l_acMtd, &xStat))
			return NV_SET_ERROR(NVE_NO_DEV, strerror(errno));
	}

	return 1;
}

int NvPrivOSFinish(void)
{
	return 1;
}

int NvPrivOSPostInit(void)
{
	return 1;
}

int NvPrivOSCriticalPostReset(struct nv_critical *pParams)
{
	/* nothing to do */
	return 1;
}

int NvPrivOSCriticalPartReset(struct nv_critical *pCrit, nv_os_type_e eForOS)
{
	if (l_bManufMode) {
		/*
		 * In manufacturing mode, don't return an error, assuming
		 * that caller knows what they're doing.
		 */
		return 1;
	} else {
		/*
		 * Retain previous behavior of returning an error, which
		 * will cause us to exit before calling NvSave(); this is
		 * done to prevent 'nvram reset' from linux from erasing the
		 * OSCfgTable, since we won't properly restore the ubootenv
		 * section.
		 */
		RETURN_NOT_IMPLEMENTED();
	}
}

int NvPrivOSFlashOpen(char bForWrite)
{
	l_iFdMtd = open(l_acMtd, bForWrite ? (O_RDWR | O_SYNC) : O_RDONLY);
	if (-1 == l_iFdMtd)
		return NV_SET_ERROR(NVE_NO_DEV, strerror(errno));

	CLEAR(l_xMtdInfo);
	/* read partition info */
	if (ioctl(l_iFdMtd, MEMGETINFO, &l_xMtdInfo)) {
		CLOSE(l_iFdMtd);
		return NV_SET_ERROR(NVE_NO_DEV, strerror(errno));
	}

	return 1;
}

int NvPrivOSFlashClose(void)
{
	CLOSE(l_iFdMtd);

	return 1;
}

/* A block is marked as bad as a consequence of consecutive read/write errors,
 * for example unrecoverable CRC errors, or if the data verification after a
 * write finds data mismatch after a number of retries. */
static int NvPrivMarkBadBlock(int fd, loff_t iOffset)
{
	logMsg(LOG_STATUS, "Marking offset %d as bad\n", (int)iOffset);
	return (ioctl(fd, MEMSETBADBLOCK, &iOffset));
}

int NvPrivOSFlashRead(void *pvBuf, loff_t iOffs, size_t iLength)
{
	int iRead;
	int i, iRet;

	for (i = 0; i < bbMaxRetries; i++) {
		iRead = pread(l_iFdMtd, pvBuf, iLength, iOffs);

		if (iRead != iLength) {
			if (g_markBadBlocks) {
				systemLog("Retrying failed read:Got %i "
					  "Bytes instead of %i.\n", iRead, iLength);
				continue;
			} else {
				systemLog("read failed. Got %i Bytes "
					  "instead of %i\n", iRead, iLength);
				return NV_SET_ERROR(NVE_IO, strerror(errno));
			}
		}
		break;
	}

	if (g_markBadBlocks && (i >= bbMaxRetries)) {
		/* Read error, for example unrecoverable ECC */
		iRet = NvPrivMarkBadBlock(l_iFdMtd, iOffs);
		return NV_SET_ERROR(NVE_IO, strerror(iRet));
	}

	return 1;
}

int NvPrivOSFlashErase(loff_t iOffs)
{
	erase_info_t xErase;
	CLEAR(xErase);

	xErase.length = l_xMtdInfo.erasesize;
	xErase.start = iOffs;
	if (ioctl(l_iFdMtd, MEMERASE, &xErase))
		return NV_SET_ERROR(NVE_IO, strerror(errno));

	return 1;
}

int NvPrivOSFlashWrite( /*@in@ */ const void *pvBuf, loff_t iOffs, size_t iLength)
{
	int iWritten, iRead;
	int i, iRet;
	unsigned char *pvRdBuf;

	/* we are not called for bad sectors */

	for (i = 0; i < bbMaxRetries; i++) {
		iWritten = pwrite(l_iFdMtd, pvBuf, iLength, iOffs);
		if (iWritten != iLength) {
			if (g_markBadBlocks) {
				systemLog("Retrying failed write:"
					  "Wrote %i Bytes"
					  " instead of %i.\n", iWritten, iLength);
				continue;
			} else {
				logMsg(LOG_ERR, "write failed."
				       " Wrote %i Bytes" " instead of %i\n", iWritten, iLength);
				return NV_SET_ERROR(NVE_IO, strerror(errno));
			}
		}

		if (g_markBadBlocks) {
			pvRdBuf = (unsigned char *)malloc(iLength);
			if (NULL == pvRdBuf) {
				systemLog("Malloc failed.\n");
				return NV_SET_ERROR(NVE_IO, strerror(errno));
			}
			for (i = 0; i < bbMaxRetries; i++) {
				iRead = pread(l_iFdMtd, pvRdBuf, iLength, iOffs);
				if (iRead != iLength) {
					systemLog("Retrying failed read:"
						  "%i < > %i.\n", iRead, iLength);
					continue;
				}
				if (memcmp(pvRdBuf, pvBuf, iLength) != 0) {
					logMsg(LOG_ERR,
					       "\nData mismatch at offset 0x%08x\n", iOffs);
					logMsg(LOG_ERR, "Source is");
					MemDump(pvBuf, iOffs & ~0xf, MIN(iLength, 0x20));
					logMsg(LOG_ERR, "Flash is");
					MemDump(pvRdBuf, iOffs & ~0xf, MIN(iRead, 0x20));
					continue;
				}
				break;
			}
			FREE(pvRdBuf);
		}
		break;
	}

	if (g_markBadBlocks && (i >= bbMaxRetries)) {
		iRet = NvPrivMarkBadBlock(l_iFdMtd, iOffs);
		return NV_SET_ERROR(NVE_IO, strerror(iRet));
	}

	return 1;
}

int NvPrivOSFlashProtect(loff_t iOffs, size_t iLength, char bProtect)
{
	erase_info_t xErase;
	CLEAR(xErase);

	xErase.length = l_xMtdInfo.erasesize;
	xErase.start = iOffs;
	if (ioctl(l_iFdMtd, (bProtect ? MEMLOCK : MEMUNLOCK), &xErase)) {
		if (ENOTSUP != errno)
			/* e.g. NAND */
			return NV_SET_ERROR(NVE_IO, strerror(errno));
	}

	return 1;
}

int NvPrivOSFlashInfo(loff_t iOffs,
/*@out@*/ struct nv_priv_flash_status *pStatus)
{
	int iRes;

	CLEAR(*pStatus);

	/* linux hasn't an interface yet to determine erase size at iOffs.
	   Anyway, we place NVRAM immediately after U-Boot, so we have unique
	   erase sizes */
	pStatus->iEraseSize = l_xMtdInfo.erasesize;
	pStatus->type = l_xMtdInfo.type;

	/* determine whether block at iOffs is bad */
	iRes = ioctl(l_iFdMtd, MEMGETBADBLOCK, &iOffs);

	if (iRes > 0)
		pStatus->bBad = 1;
	else if ((iRes < 0) && (ENOTSUP != errno))
		return NV_SET_ERROR(NVE_IO, strerror(errno));
	/* else if not supported (NOR), is is assumed good */

	return 1;
}

void NvPrivOSPrintf(const char *szFormat, ...)
{
	va_list ap;

	va_start(ap, szFormat);
	vprintf(szFormat, ap);
	va_end(ap);
}

void NvPrivOSPrintfError(const char *szFormat, ...)
{
	va_list ap;

	va_start(ap, szFormat);
	vfprintf(stderr, szFormat, ap);
	va_end(ap);
}
