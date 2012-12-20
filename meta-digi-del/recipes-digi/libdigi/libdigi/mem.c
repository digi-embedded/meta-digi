/*
 * mem.c
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

#include <stdio.h>	/* printf */

#include "mem.h"

/***********************************************************************
 * !Function: MemCmp
 * !Descr:    compares memory
 * !Return:   offset of failure or -1 if none
 ***********************************************************************/
loff_t MemCmp(const void *pvS1, const void *pvS2, size_t iSize)
{
	const char *pcS2 = (const char *)pvS2;
	const char *pcS1 = (const char *)pvS1;
	loff_t iOffset = 0;

	while (iOffset < iSize) {
		if (*pcS2 != *pcS1)
			return iOffset;

		pcS2++;
		pcS1++;
		iOffset++;
	}

	return -1;
}

/***********************************************************************
 * !Function: MemDump
 * !Descr: Prints memory from pvbase + iOffset to pvBase + iOffset + iLen
 ***********************************************************************/
void MemDump(const void *pvBase, loff_t iOffset, size_t iLen)
{
	const unsigned char *pucBuf = (const unsigned char *)pvBase + iOffset;
	const int COLUMN_COUNT = 16;
	int i;

	for (i = 0; i < iLen; i += COLUMN_COUNT) {
		/* print one row */
		int j, iRowLen;

		if ((i + COLUMN_COUNT) <= iLen)
			iRowLen = COLUMN_COUNT;
		else
			iRowLen = iLen - i;

		printf("%08llx  ", (long long)iOffset);

		/* print hexadecimal representation */
		for (j = 0; j < iRowLen; j++) {
			printf("%02x ", *(pucBuf + j));
			if (((COLUMN_COUNT / 2) - 1) == j)
				/* additional separator */
				printf("   ");
		}

		printf("  ");

		/* print character representation row */
		for (j = 0; j < iRowLen; j++) {
			unsigned char c = *(pucBuf + j);
			if ((c < 32) || (c > 127))
				c = '.';

			if (((COLUMN_COUNT / 2) - 1) == j)
				/* additional separator */
				printf(" ");

			printf("%c", c);
		}

		printf("\r\n");
		pucBuf += iRowLen;
		iOffset += iRowLen;
	}
}
