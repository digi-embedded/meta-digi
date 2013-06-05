/*
 * v4l2_preview_test.c
 *
 * Copyright (C) 2012 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: V4L2 preview test application.
 *
 * Video overlay, framebuffer overlay or previewing stores images directly into
 * the video memory of the graphics card. Video overlays are accessible through
 * /dev/video after a VIDIOC_S_FMT to overlay (the default for /dev/video is
 * video capture).
 *
 * V4L2 API @ http://v4l2spec.bytesex.org/
 *
 */
#include <pthread.h>
#include <sched.h>

#include "v4l2_defs.h"

int verbose = 0;
int fb_fg_fd = -1;
static int should_stop = 0;

static void signal_handler(int signum, siginfo_t * info, void *myact)
{
	switch (signum) {
		case SIGHUP:
		case SIGTERM:
		case SIGINT:
			should_stop = TRUE;
			/* FALLTHROUGH */
		default:
			log("signal %d caught\n", signum);
			break;
	}
}

/**
 * Prints online help
 *
 * @fd file to print on
 */
static void printUsage(FILE * fd)
{
	fprintf(fd, "V4L2 preview test application.\n\n"
		"Usage: v4l_preview_test -d </dev/video0> -o </dev/fb0> -h <height> "
		"-w <width> -t <top> -l <left> [-r] [-v] [-?]\n\n"
		"     -d V4L2 capture device, by default /dev/video0\n"
		"     -o V4L2 output (framebuffer) device, by default /dev/fb0\n"
		"     -h Video height in pixels\n"
		"     -w Video width in pixels\n"
		"     -t Video top offset in pixels\n"
		"     -l Video left offset in pixels\n"
		"     -x Non destructive overlay (do not overwrite framebuffer)\n"
		"        Setting '-x' discards the output device option '-o' and\n"
		"        uses the overlay background framebuffer.\n"
		"     -r RGB565 (default is UYVY)\n"
		"     -v Verbose mode\n"
		"     -? This help\n\n");
}

static void *v4l2_camera_thread(void *pargs)
{
	struct fb_fix_screeninfo fb_fix;
	struct fb_var_screeninfo fb_var;
	ARGUMENTS *args = pargs;

	/* Check overlay is supported */
	if (!v4l2_is_capability_supported(args->fd_in, V4L2_CAP_VIDEO_OVERLAY)) {
		printf("Overlay not supported\n");
		return ((void *)-EINVAL);
	}

	/* Check requested image format is supported by capture device */
	if (!v4l2_is_format_supported(args->fd_in, args->options.format)) {
		printf("Requested format not supported.\n");
		return ((void *)-EINVAL);
	}

	/* Obtain framebuffer parameters */
	if ((args->fd_out = v4l2_fb_get_info(args->fb_device, &fb_fix, &fb_var)) < 0) {
		printf("Unable to open %s: %s\n", args->fb_device, strerror(errno));
		exit(-1);
	}

	/* Set video output */
	if (v4l2_set_output(args->fd_in, &fb_fix) < 0) {
		printf("\nUnable to set current video output.\n");
		exit(-1);
	}

	/* Restore default cropping - no zoom */
	v4l2_reset_cropping_rectangle(args->fd_in);

	/* Set size in overlay */
	v4l2_set_format_overlay(args->fd_in,
				args->options.top, args->options.left,
				args->options.height, args->options.width);

	/* Now the overlay functionality is available */

	/* Obtain overlay framebuffer's fd */
	v4l2_mxc_find_overlay(&fb_fix, &fb_fg_fd);

	/* Draws over the fb (destructive) and sets image format */
	v4l2_overlay_set_framebuffer(args->fd_in, &fb_fix, &fb_var,
				     args->options.non_destructive);

	v4l2_overlay_control(args->fd_in, 1);

	while (should_stop != TRUE)
		sleep(1);

	v4l2_overlay_control(args->fd_in, 0);

	close(args->fd_in);
	close(args->fd_out);
	if (fb_fg_fd > 0)
		close(fb_fg_fd);

	return NULL;
}

static void v4l2_display_menu(int non_destructive)
{
	printf("\t[h]display the operation Help\n");
	printf("\t[x]eXit\n");
	printf("\t[r]Rotate input\n");
	printf("\t[b]adjust Brightness\n");
	printf("\t[s]adjust Saturation\n");
	printf("\t[c]adjust Color balance\n");
	printf("\t[z]automatic Zoom\n");
	printf("\t[a]zoom Area\n");
	if (non_destructive) {
		/* These options are only available in non_destructive overlay */
		printf("\t[g]set Global transparency\n");
		printf("\t[l]set Local transparency\n");
	}
	printf("\nChoose option: ");
}

static int cmd_rotate(int fd)
{
	unsigned int rotate = 0;

	printf("Rotation options:\n");
	printf("\t0 - No rotation\n");
	printf("\t1 - Vertical flip\n");
	printf("\t2 - Horizontal flip\n");
	printf("\t3 - Rotate 180 degree\n");
	printf("\t4 - Rotate 90 degree\n");
	printf("\t5 - Rotate 90 degree right and vertical flip\n");
	printf("\t6 - Rotate 90 degree right and horizontal flip\n");
	printf("\t7 - Rotate 90 degree left\n");
	printf("Choose rotation: ");
	scanf("%u", &rotate);
	if (rotate > 7) {
		printf("Invalid value\n");
		return -1;
	}
	return v4l2_rotate(fd, rotate);
}

static int cmd_brightness(int fd)
{
	int brightness = 0;
	int ret = -1;

	/* Read current brightness value */
	v4l2_get_brightness(fd, &brightness);
	printf("Enter brightness [0-100%%] (current: %u%%): ", brightness);
	if ((scanf("%u", &brightness)) != 1 || should_stop) {
		return ret;
	}

	if (brightness > 100 || brightness < 0) {
		printf("Brightness value [%d] out of range.\n", brightness);
	} else {
		ret = v4l2_set_brightness(fd, brightness);
	}

	return ret;
}

static int cmd_saturation(int fd)
{
	int saturation;
	int ret = -1;

	/* Read current saturation value */
	v4l2_get_saturation(fd, &saturation);
	printf("Enter saturation (valid values are 0, 25, 37, 50, 75, 100 and 150) [0-150%%] (current: %u%%): ", saturation);
	if (scanf("%u", &saturation) != 1 || should_stop)
		return ret;

	if (saturation > 150 || saturation < 0) {
		printf("Saturation value [%d] out of range.\n", saturation);
	} else {
		ret = v4l2_set_saturation(fd, saturation);
	}
	return ret;
}

static int cmd_zoom(int fd)
{
	int zoom;
	int ret = -1;

	printf("Enter zooming percentage [100 - 300]%%: ");
	if ((scanf("%i", &zoom)) != 1)
		return ret;

	if (zoom < 100 || zoom > 300) {
		printf("Zoom value [%d] out of range.\n", zoom);
	} else {
		ret = v4l2_zoom(fd, zoom);
	}
	return ret;
}

static int cmd_area_zoom(int fd)
{
	struct v4l2_rect zoom_rectangle;
	int ret = -1;

	printf("Enter rectangle's values (top, left, width, height): ");
	if ((scanf("%d,%d,%d,%d", &zoom_rectangle.top, &zoom_rectangle.left,
		   &zoom_rectangle.width, &zoom_rectangle.height)) != 4 || should_stop) {
		printf("Enter four parameters\n");
		return ret;
	} else {
		ret = v4l2_area_zoom(fd, &zoom_rectangle);
	}
	return ret;
}

static int cmd_color_balance(int fd, char cmd)
{
	int ret = -1;
	int red, blue, black;

	red = blue = black = 0;
	switch (cmd) {
	case 'r':
		if (v4l2_get_red_balance(fd, &red) < 0) {
			printf("\n[ERROR] Unable to get Red balance\n\n");
		} else {
			printf("Enter Red balance value (current: %d): ", red);
			scanf("%u", &red);
			ret = v4l2_set_red_balance(fd, red);
		}
		break;
	case 'b':
		if (v4l2_get_blue_balance(fd, &blue) < 0) {
			printf("\n[ERROR] Unable to get Blue balance\n\n");
		} else {
			printf("Enter Blue balance value (current: %d): ", blue);
			scanf("%u", &blue);
			ret = v4l2_set_blue_balance(fd, blue);
		}
		break;
	case 'w':
		printf("\nNot implemented (yet...)\n\n");
		break;
	case 'l':
		if (v4l2_get_black_level(fd, &black) < 0) {
			printf("\n[ERROR] Unable to get Black level\n\n");
		} else {
			printf("Enter Black level value (current: %d): ", black);
			scanf("%u", &black);
			ret = v4l2_set_black_level(fd, black);
		}
		break;
	default:
		break;
	}

	return ret;
}

static int cmd_global_alpha(int fd)
{
	int ret = -1;
	int alpha_value;

	printf("Enter alpha_value global transparency [0-100%%]: ");
	if (scanf("%d", &alpha_value) != 1 || should_stop)
		return ret;
	if (alpha_value > 100 || alpha_value < 0) {
		printf("Alpha value [%d] out of range.\n", alpha_value);
		return ret;
	}
	ret = v4l2_global_alpha_set(fd, 1, alpha_value);
	return ret;
}

static int cmd_local_alpha(ARGUMENTS * args)
{
	int ret = -1;
	struct v4l2_rect alpha_rectangle;
	int alpha_value;

	printf("Enter rectangle's values (top, left, width, height): ");
	if ((scanf("%d,%d,%d,%d", &alpha_rectangle.top, &alpha_rectangle.left,
		   &alpha_rectangle.width, &alpha_rectangle.height)) != 4 || should_stop) {
		printf("Enter four parameters\n");
		return ret;
	} else {
		printf("Enter rectangle's transparency [0-100%%]: ");
		if (scanf("%d", &alpha_value) != 1 || should_stop)
			return ret;
		if (alpha_value > 100 || alpha_value < 0) {
			printf("Alpha value [%d] out of range.\n", alpha_value);
			return ret;
		}
		ret = v4l2_local_alpha_set(args, fb_fg_fd, &alpha_rectangle, alpha_value);
	}
	return ret;
}

static void display_color_balance_menu(void)
{
	printf("Color balance:\n");
	printf("\t[r]adjust Red balance\n");
	printf("\t[b]adjust Blue balance\n");
	printf("\t[w]adjust White balance\n");
	printf("\t[l]adjust bLack level\n");
	printf("Choose option: ");
}

int main(int argc, char **argv)
{
	int opt;
	int retval = 0;
	ARGUMENTS args;
	pthread_t v4l2_camera_th;
	int ret = 0;
	char cmd[64] = "";
	struct sigaction act;
	char fb_device_flag = 0;	/* fb_device set in command line */

	/* Signal handling */
	memset(&act, 0, sizeof(struct sigaction));
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = signal_handler;

	if ((ret = sigaction(SIGINT, &act, NULL)) < 0) {
		log("Signal installation error: %d\n", SIGINT);
		return -1;
	}

	/* Initialize options */
	memset(&args, 0, sizeof(args));
	args.options.width = 640;
	args.options.height = 480;
	args.options.format = V4L2_PIX_FMT_UYVY;
	strcpy(args.fb_device, "/dev/fb0");
	strcpy(args.v4l2_device, "/dev/video0");

	/* Input processing */
	while ((opt = getopt(argc, argv, "w:d:h:l:t:o:xvr?")) != -1) {
		switch (opt) {
			case '?':
				printUsage(stderr);
				return 1;
			case 'd':
				strncpy(args.v4l2_device, optarg, sizeof(args.v4l2_device) - 1);
				args.v4l2_device[sizeof(args.v4l2_device) - 1] = '\0';
				break;
			case 'w':
				args.options.width = atoi(optarg);
				break;
			case 't':
				args.options.top = atoi(optarg);
				break;
			case 'l':
				args.options.left = atoi(optarg);
				break;
			case 'h':
				args.options.height = atoi(optarg);
				break;
			case 'o':
				strncpy(args.fb_device, optarg, sizeof(args.fb_device) - 1);
				args.fb_device[sizeof(args.fb_device) - 1] = '\0';
				fb_device_flag = 1;
				break;
			case 'r':
				args.options.format = V4L2_PIX_FMT_RGB565;
				break;
			case 'x':
				args.options.non_destructive = 1;
				break;
			case 'v':
				verbose = 1;
				break;
		}
	}

	/* Input validation */
	if (args.options.height == 0 || args.options.width == 0) {
		printf("Invalid input: Display height and width cannot be zero.\n");
		exit(-EINVAL);
	}

	if (args.options.non_destructive) {
		if (fb_device_flag) {
			printf("\n[WARNING] The application will discard '-o' option and use"
			       " the overlay background framebuffer device.\n");
		}
		if (v4l2_get_overlay_bg(args.fb_device) < 0) {
			printf("Unable to find overlay background framebuffer device\n");
			exit(-1);
		}
	}

	printf("\nVideo height %d width %d top %d left %d\n\n",
	       args.options.height, args.options.width, args.options.top, args.options.left);

	/* Open input device */
	if ((args.fd_in = open(args.v4l2_device, O_RDWR, 0)) < 0) {
		printf("Unable to open %s: %s\n", args.v4l2_device, strerror(errno));
		exit(-1);
	}

	log("Opened %s\n", args.v4l2_device);
	log("V4L2 preview test application started.\n");

	if (pthread_create(&v4l2_camera_th, NULL, v4l2_camera_thread, (void *)&args)) {
		exit(-1);
	}

	while (should_stop != TRUE) {
		v4l2_display_menu(args.options.non_destructive);
		cmd[0] = ' ';
		errno = 0;
		scanf("%s", cmd);
		switch (cmd[0]) {
			case 'x':	/* Exit */
				should_stop = TRUE;
				break;
			case 'h':	/* Help */
				/* Nothing to do, as the help is shown in each iteration */
				break;
			case 'r':
				cmd_rotate(args.fd_in);
				break;
			case 'b':
				cmd_brightness(args.fd_in);
				break;
			case 's':
				cmd_saturation(args.fd_in);
				break;
			case 'z':
				cmd_zoom(args.fd_in);
				break;
			case 'a':
				cmd_area_zoom(args.fd_in);
				break;
			case 'c':
				display_color_balance_menu();
				scanf("%s", cmd);
				cmd_color_balance(args.fd_in, cmd[0]);
				break;
			case 'g':
				if (args.options.non_destructive)
					cmd_global_alpha(args.fd_out);
				break;
			case 'l':
				if (args.options.non_destructive)
					cmd_local_alpha(&args);
				break;
			default:
				break;
		}
	}

	if (pthread_join(v4l2_camera_th, NULL)) {
		exit(-1);
	}

	log("V4L2 preview test application finished.\n");
	return retval;
}
