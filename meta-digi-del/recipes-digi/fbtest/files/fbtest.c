/*
 * fbtest.c
 *
 * Copyright (C) 2006-2009 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: draws some test patterns (R,G,B and white colorbars)
 *
 */
#include <fcntl.h>
#include <linux/kd.h>
#include <stdio.h>		/* fprintf */
#include <stdlib.h>		/* EXIT_SUCCESS */
#include <string.h>		/* strcpy */
#include <sys/ioctl.h>
#include <unistd.h>

#define MWINCLUDECOLORS
#include <microwin/nano-X.h>	/* GrOpen */

#ifdef S_SPLINT_S
typedef /*@abstract@ */ MWCOLORVAL;
typedef /*@abstract@ */ GR_EVENT;
typedef /*@abstract@ */ GR_EVENT_EXPOSURE;
typedef /*@abstract@ */ GR_WINDOW_ID;
typedef /*@abstract@ */ GR_GC_ID;
#endif

static void draw(const GR_EVENT * e);
static void draw_colorbar(GR_WINDOW_ID wid, GR_GC_ID gc,
			  int x, int y, int height, int dred, int dgreen, int dblue, char *label);

static GR_SCREEN_INFO l_xSIP;

static int dX = 0;
static int dY = 0;

int main(int argc, char *argv[])
{
	GR_EVENT event;
	GR_WINDOW_ID w;

	printf("fbtest $Revision: 1.3 $ " __TIME__ "\n");

	if ((argc != 1) && (argc != 3)) {
		fprintf(stderr, "Usage: %s [dX dY]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Disable framebuffer console cursor */
	int fd = open("/sys/class/graphics/fbcon/cursor_blink", O_WRONLY);
	if (fd >= 0) {
		write(fd, "0", 1);
		close(fd);
	}

	if (3 == argc) {
		/* dX and dY is for testing the display to see that lines
		 * vanish from display */
		dX = atoi(argv[1]);
		dY = atoi(argv[2]);
	}

	if (GrOpen() < 0) {
		fprintf(stderr, "cannot open graphics\n");
		exit(EXIT_FAILURE);
	}

	GrGetScreenInfo(&l_xSIP);
	/* create window, center it in display */
	w = GrNewWindowEx(GR_WM_PROPS_NOAUTOMOVE,
			  (unsigned char *)"nanox_mini", GR_ROOT_WINDOW_ID,
			  dX, dY, l_xSIP.vs_width, l_xSIP.vs_height, GR_RGB(0, 0, 0));
	GrSelectEvents(w, GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_CLOSE_REQ);
	GrMapWindow(w);

	while (1) {
		GrGetNextEvent(&event);

		switch (event.type) {
		case GR_EVENT_TYPE_EXPOSURE:
			/* now do the actual display work */
			draw(&event);
			break;
		case GR_EVENT_TYPE_CLOSE_REQ:
			GrClose();
			exit(EXIT_SUCCESS);
		}
	}
	/* @notreached@ */
}

/***********************************************************************
 * @Function: draw
 * @Return: n/a
 * @Descr: displays colorbar
 ***********************************************************************/
static void draw(const GR_EVENT * e)
{
	GR_WINDOW_ID wid = ((GR_EVENT_EXPOSURE *) e)->wid;
	GR_GC_ID gc = GrNewGC();
	char szResolution[64];
	int iY = 40;
	int idY = ((l_xSIP.vs_height - (2 * iY)) / 4) - 10;
	int i;

	GrSetGCForeground(gc, WHITE);

	/* border frame */
	GrRect(wid, gc, 0, 0, l_xSIP.vs_width, l_xSIP.vs_height);
	for (i = 0; i < l_xSIP.vs_height; i += 10) {
		int iLength;

		if (!(i % 100))
			iLength = 10;
		else if (!(i % 50))
			iLength = 5;
		else
			iLength = 2;

		GrLine(wid, gc, 0, i, iLength, i);
		GrLine(wid, gc, l_xSIP.vs_width, i, l_xSIP.vs_width - iLength - 1, i);
	}

	for (i = 0; i < l_xSIP.vs_width; i += 10) {
		int iLength;

		if (!(i % 100))
			iLength = 10;
		else if (!(i % 50))
			iLength = 5;
		else
			iLength = 2;

		GrLine(wid, gc, i, 0, i, iLength);
		GrLine(wid, gc, i, l_xSIP.vs_height, i, l_xSIP.vs_height - iLength - 1);
	}

	/* resolution */
	sprintf(szResolution, "%ix%i", l_xSIP.vs_width, l_xSIP.vs_height);
	GrText(wid, gc, l_xSIP.vs_width / 2 - 20, 20, szResolution, -1, GR_TFASCII | GR_TFTOP);

	/* display colorbards  */
	draw_colorbar(wid, gc, 10, iY, idY, 1, 0, 0, "red");
	iY += idY + 10;
	draw_colorbar(wid, gc, 10, iY, idY, 0, 1, 0, "green");
	iY += idY + 10;
	draw_colorbar(wid, gc, 10, iY, idY, 0, 0, 1, "blue");
	iY += idY + 10;
	draw_colorbar(wid, gc, 10, iY, idY, 1, 1, 1, "white");

	GrDestroyGC(gc);
}

/***********************************************************************
 * @Function: draw_colorbar
 * @Return: n/a
 * @Descr: draws a colorbar with RGB modified by dred/dgreen/dblue
 ***********************************************************************/
static void draw_colorbar(GR_WINDOW_ID wid, GR_GC_ID gc,
			  int x, int y, int height, int dred, int dgreen, int dblue, char *label)
{
	int i;
	int red = 0;
	int green = 0;
	int blue = 0;

	GrSetGCForeground(gc, WHITE);
	GrText(wid, gc, x, y, label, -1, GR_TFASCII | GR_TFTOP);

	x += 40;	/* leave room for text */

	for (i = x; i < l_xSIP.vs_width - 20; i++) {
		GrSetGCForeground(gc, MWRGB(red, green, blue));
		GrLine(wid, gc, x, y, x, y + height);

		red += dred;
		green += dgreen;
		blue += dblue;

		x++;
	}
}
