/*
 * v4l2_common.c
 *
 * Copyright (C) 2012 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: V4L2 common library.
 *
 * V4L2 API @ http://v4l2spec.bytesex.org/
 *
 */

#include "v4l2_defs.h"

#define BRIGHTNESS_MAX_VALUE	0xFF
#define BRIGTHNESS_MIN_VALUE	0
#define SATURATION_MAX_VALUE	0xFF
#define SATURATION_MIN_VALUE	0
#define GLOBAL_ALPHA_MAX_VALUE	0xFF
/* This is confusing, because they are inverted */
#define LOCAL_ALPHA_MAX_VALUE	0x0
#define LOCAL_ALPHA_MIN_VALUE	0xFF

/* Returns the open file descriptor if the fb is an MXC fb overlay,
 * error otherwise. */
static int v4l2_mxc_is_overlay(char *fb_device)
{
	int fd, ret;
	struct fb_fix_screeninfo fb_fix;

	fd = ret = -1;

	if ((fd = open(fb_device, O_RDWR)) < 0) {
		log("Unable to open frame buffer 0\n");
		return ret;
	}

	if ((ret = ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix)) < 0) {
		close(fd);
		return ret;
	}

	if (!strcmp(fb_fix.id, "DISP3 FG"))
		return fd;

	close(fd);
	return -1;
}

/* Return overlay background framebuffer device path, error otherwise */
int v4l2_get_overlay_bg(char *fb_device)
{
	static const char *fb_path = "/dev/fb";
	int ipu_ch, i;
	int fd = -1;

	for (i = 0; i < 2; i++) {
		sprintf(fb_device, "%s%d", fb_path, i);
		if ((fd = open(fb_device, O_RDWR)) < 0) {
			log("Unable to open frame buffer %s\n", fb_device);
			goto error;
		}
		if (ioctl(fd, MXCFB_GET_FB_IPU_CHAN, &ipu_ch) < 0) {
			log("ioctl MXCFB_GET_FB_IPU_CHAN error\n");
			close(fd);
			goto error;
		}
		close(fd);
		if (ipu_ch == MEM_BG_SYNC) {
			log("Overlay background frame buffer device: %s\n", fb_device);
			return 0;
		}
	}

error:
	return -1;
}

/* Enables or disables the V4L2 previewing */
int v4l2_overlay_control(int fd_overlay, int start)
{
	/* Start overlay */
	return ioctl(fd_overlay, VIDIOC_OVERLAY, &start);
}

/* Returns a string representation of some v4L2 pixel formats, or its
 * numerical code string otherwise. */
char *v4l2_fmt_str(int pixelformat)
{
	static char tmp[64] = "";

	switch (pixelformat) {
		case IPU_PIX_FMT_RGB332:
			return "RGB332";
		case IPU_PIX_FMT_RGB555:
			return "RGB555";
		case IPU_PIX_FMT_RGB565:
			return "RGB565";
		case IPU_PIX_FMT_RGB666:
			return "RGB666";
		case IPU_PIX_FMT_RGB24:
			return "RGB24";
		case IPU_PIX_FMT_BGR32:
			return "BGR32";
		case IPU_PIX_FMT_BGRA32:
			return "BGRA32";
		case IPU_PIX_FMT_RGB32:
			return "RGB32";
		case IPU_PIX_FMT_RGBA32:
			return "RGBA32";
		case IPU_PIX_FMT_ABGR32:
			return "ABGR32";
		case IPU_PIX_FMT_BGR24:
			return "BGR24";
		case IPU_PIX_FMT_YUYV:
			return "YUYV";
		case IPU_PIX_FMT_UYVY:
			return "UYVY";
		case IPU_PIX_FMT_YVYU:
			return "YVYU";
		case IPU_PIX_FMT_VYUY:
			return "VYUY";
		case IPU_PIX_FMT_Y41P:
			return "Y41P";
		case IPU_PIX_FMT_YUV444:
			return "YUV444";
		case IPU_PIX_FMT_VYU444:
			return "VYU444";
		case IPU_PIX_FMT_NV12:
			return "NV12";
		// Missing formats
		default:
			sprintf(tmp, "Undecoded format: %x\n", pixelformat);
			return tmp;
	}
}

/* Returns true if the specified V4L2 capability is supported by the
 * device, false or error otherwise.*/
int v4l2_is_capability_supported(int fd, int cap)
{
	struct v4l2_capability capability;
	int ret = 0;

	/* Check the device capabilities */
	memset(&capability, 0, sizeof(capability));
	ret = ioctl(fd, VIDIOC_QUERYCAP, &capability);
	if (ret < 0) {
		if (errno == -EINVAL)
			log("Not a V4L2 device.\n");
		return ret;
	}

	if (capability.capabilities & cap) {
		return TRUE;
	}
	return FALSE;
}

/* Returns true if the specified V4L2 video standard is supported by the
 * device, false or error otherwise.*/
int v4l2_is_video_std_supported(int fd, int standard)
{
	v4l2_std_id std_id;
	int ret = 0;

	/* Check video standard */
	memset(&std_id, 0, sizeof(std_id));
	if ((ret = ioctl(fd, VIDIOC_G_STD, &std_id)) < 0) {
		log("VIDIOC_G_STD failed with %d\n", ret);
		return ret;
	}

	if (std_id & standard)
		return TRUE;
	return FALSE;
}

/* Returns true if the specified V4L2 capture pixel format is supported by the
 * device, false or error otherwise.*/
int v4l2_is_format_supported(int fd, int format)
{
	struct v4l2_fmtdesc fmtdesc;
	int ret = 0;

	/* Check whether the requested format is supported by the device */
	memset(&fmtdesc, 0, sizeof(fmtdesc));
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	do {
		ret = ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc);
		fmtdesc.index++;
		if (fmtdesc.pixelformat == format)
			break;
	} while (ret >= 0);

	if (ret == 0)
		return TRUE;

	if (ret < 0 && errno != EINVAL)
		log("VIDIOC_ENUM_FMT failed with %d\n", errno);
	return FALSE;
}

/* Check whether a specified framerate is configured in the device */
int v4l2_check_frame_rate(int fd, int framerate)
{
	struct v4l2_streamparm streamparm;

	memset(&streamparm, 0, sizeof(streamparm));
	streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	streamparm.parm.capture.timeperframe.numerator = 0;
	streamparm.parm.capture.timeperframe.denominator = 0;
	if (ioctl(fd, VIDIOC_G_PARM, &streamparm) < 0) {
		log("get frame rate failed\n");
		return FALSE;
	}
	log("Frame rate is %d\n", streamparm.parm.capture.timeperframe.denominator);

	if (streamparm.parm.capture.timeperframe.denominator == framerate)
		return TRUE;
	return FALSE;
}

/* Set the specified framerate on the V4L2 device */
int v4l2_set_frame_rate(int fd, int framerate)
{
	struct v4l2_streamparm streamparm;

	memset(&streamparm, 0, sizeof(streamparm));

	/* Set stream parameters */
	streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	streamparm.parm.capture.timeperframe.numerator = 1;
	streamparm.parm.capture.timeperframe.denominator = framerate;

	return ioctl(fd, VIDIOC_S_PARM, &streamparm);
}

/* Return true if the output specified by name is supported by the device,
 * false or error otherwise */
int v4l2_check_output(int fd, char *name)
{
	struct v4l2_output output;
	int ret = 0;

	/* Check what are the available video outputs */
	memset(&output, 0, sizeof(output));

	do {
		ret = ioctl(fd, VIDIOC_ENUMOUTPUT, &output);
		output.index++;
		if (ret >= 0)
			log("Supported output %s\n", output.name);
		if (!strcmp((char *)output.name, name))
			return TRUE;
	} while (ret >= 0);

	if (ret != 0 && errno != EINVAL) {
		log("VIDIOC_ENUMOUTPUT failed with %d\n", errno);
		return ret;
	}
	return FALSE;
}

/* Set the current video output */
int v4l2_set_output(int fd, struct fb_fix_screeninfo *fb_fix)
{
	struct v4l2_output output;
	int ret = 0;

	memset(&output, 0, sizeof(output));
	while ((ret = ioctl(fd, VIDIOC_ENUMOUTPUT, &output)) >= 0) {
		if (!strcmp((char *)output.name, fb_fix->id)) {
			if ((ret = ioctl(fd, VIDIOC_S_OUTPUT, &(output.index))) < 0) {
				log("Set output failed with %d\n", ret);
			}
			break;
		}
		output.index++;
	}

	return ret;
}

/* Sets a specified pixel format and scale to the V4L2 device overlay */
int v4l2_set_format_capture(int fd, int pixelformat, int height, int width)
{
	struct v4l2_format format;
	int ret = 0;

	/* Get the data format */
	if ((ret = ioctl(fd, VIDIOC_G_FMT, &format)) < 0) {
		log("Get format failed with %d\n", ret);
		return ret;
	}

	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.width = width;
	format.fmt.pix.height = height;
	format.fmt.pix.pixelformat = pixelformat;

	if ((ret = ioctl(fd, VIDIOC_S_FMT, &format)) < 0) {
		log("Set format failed with %d\n", ret);
	}
	log("Pixel format set to %s\n", v4l2_fmt_str(pixelformat));
	log("Sensor cropping dimensions set to H %d W %d\n", height, width);
	return ret;
}

/* Sets a specified pixel format and scale to the V4L2 device overlay */
int v4l2_set_format_overlay(int fd, int top, int left, int height, int width)
{
	struct v4l2_format format;
	int ret = 0;

	/* Get the data format */
	if ((ret = ioctl(fd, VIDIOC_G_FMT, &format)) < 0) {
		log("Get format failed with %d\n", ret);
		return ret;
	}

	format.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	format.fmt.win.w.top = top;
	format.fmt.win.w.left = left;
	format.fmt.win.w.width = width;
	format.fmt.win.w.height = height;

	if ((ret = ioctl(fd, VIDIOC_S_FMT, &format)) < 0) {
		log("Set format failed with %d\n", ret);
	}
	log("Overlay dimensions set to H %d W %d L %d T %d\n", height, width, left, top);
	return ret;
}

int v4l2_fb_blank(int fd)
{
	return (ioctl(fd, FBIOBLANK, FB_BLANK_NORMAL));
}

int v4l2_fb_unblank(int fd)
{
	return (ioctl(fd, FBIOBLANK, FB_BLANK_UNBLANK));
}

static int v4l2_fb_put_vinfo(int fd, struct fb_var_screeninfo *fb_var)
{
	return (ioctl(fd, FBIOPUT_VSCREENINFO, fb_var));
}

static int v4l2_fb_get_vinfo(int fd, struct fb_var_screeninfo *fb_var)
{
	return (ioctl(fd, FBIOGET_VSCREENINFO, fb_var));
}

static int v4l2_fb_get_finfo(int fd, struct fb_fix_screeninfo *fb_fix)
{
	return (ioctl(fd, FBIOGET_FSCREENINFO, fb_fix));
}

/* Retrieves fixed and variable framebuffer information */
int v4l2_fb_get_info(char *fb_device, struct fb_fix_screeninfo *fb_fix,
		     struct fb_var_screeninfo *fb_var)
{
	int ret = 0;
	int fd = 0;

	/* Open framebuffer device */
	if ((fd = open(fb_device, O_RDWR)) < 0) {
		log("Unable to open frame buffer %s\n", fb_device);
		return -EIO;
	}

	log("Opened %s\n", fb_device);

	/* Obtain fixed and variable screen info for framebuffer */
	if ((ret = v4l2_fb_get_vinfo(fd, fb_var)) < 0)
		goto out;

	if ((ret = v4l2_fb_get_finfo(fd, fb_fix)) < 0)
		goto out;

out:
	return fd;
}

/* Identifies the available outputs for a framebuffer device.
 *
 * MXC hardcoded ID strings are:
 * DISP3 BG: For background framebuffer.
 * DISP3 BG - DI1: For secondary background framebuffer.
 * DISP3 FG: For overlay (foreground) framebuffer.
 */
void v4l2_ident_outputs(int fd)
{
	struct v4l2_output output;
	int ret = 0;

	/* Identify available outputs */
	memset(&output, 0, sizeof(output));
	do {
		ret = ioctl(fd, VIDIOC_ENUMOUTPUT, &output);
		output.index++;
		log("Supported output %s\n", output.name);
	} while (ret >= 0);
}

/* Returns an opened file descriptor for the MXC overlay framebuffer device. */
int v4l2_mxc_find_overlay(struct fb_fix_screeninfo *fb_fix, int *fb_fg_fd)
{
	/* Look for overlay (foreground) framebuffer */
	if (!strcmp(fb_fix->id, "DISP3 BG")) {
		/* Overlay can be /dev/fb1 or /dev/fb2 */
		if ((*fb_fg_fd = v4l2_mxc_is_overlay("/dev/fb1")) < 0) {
			*fb_fg_fd = v4l2_mxc_is_overlay("/dev/fb2");
			log("Overlay framebuffer is /dev/fb2\n");
		} else {
			log("Overlay framebuffer is /dev/fb1\n");
		}

		if (*fb_fg_fd < 0) {
			log("No overlay framebuffer found.\n");
			return -1;
		}
	} else if (!strcmp(fb_fix->id, "DISP3 BG - DI1")) {
		/* Overlay must be /dev/fb2 */
		if ((*fb_fg_fd = v4l2_mxc_is_overlay("/dev/fb2")) < 0) {
			log("No overlay framebuffer found.\n");
			return -1;
		} else {
			log("Overlay framebuffer is /dev/fb2\n");
		}
	}

	return 0;
}

/* Sets the V4L2 device overlay framebuffer format, dimensions and type */
int v4l2_overlay_set_framebuffer(int fd, struct fb_fix_screeninfo *fb_fix,
				 struct fb_var_screeninfo *fb_var, int non_destructive)
{
	struct v4l2_framebuffer framebuffer;
	int ret = 0;

	memset(&framebuffer, 0, sizeof(framebuffer));

	framebuffer.fmt.width = fb_var->xres;
	framebuffer.fmt.height = fb_var->yres;

	if (fb_var->bits_per_pixel == 32) {
		framebuffer.fmt.pixelformat = IPU_PIX_FMT_BGR32;
		framebuffer.fmt.bytesperline = 4 * framebuffer.fmt.width;
		log("Framebuffer: BGR32\n");
	} else if (fb_var->bits_per_pixel == 24) {
		framebuffer.fmt.pixelformat = IPU_PIX_FMT_BGR24;
		framebuffer.fmt.bytesperline = 3 * framebuffer.fmt.width;
		log("Framebuffer: BGR24\n");
	} else if (fb_var->bits_per_pixel == 16) {
		framebuffer.fmt.pixelformat = IPU_PIX_FMT_RGB565;
		framebuffer.fmt.bytesperline = 2 * framebuffer.fmt.width;
		log("Framebuffer: RGB565\n");
	}

	/* Draw over the framebuffer, destructive overlay */
	if (!non_destructive) {
		framebuffer.flags = V4L2_FBUF_FLAG_PRIMARY;
		/* Physical base address of the framebuffer, that is the address of the
		 * pixel in the top left corner of the framebuffer. Only relevant
		 * for destructive video overlay. */
		framebuffer.base = (void *)fb_fix->smem_start
		    + fb_fix->line_length * fb_var->yoffset;
	} else {
		framebuffer.flags = V4L2_FBUF_FLAG_OVERLAY;
	}

	if ((ret = ioctl(fd, VIDIOC_S_FBUF, &framebuffer)) < 0) {
		log("set framebuffer failed with %d\n", errno);
		return ret;
	}

	return ret;
}

/* Reset V4L2 device cropping rectangle to its default. */
int v4l2_reset_cropping_rectangle(int fd)
{
	struct v4l2_crop crop;
	struct v4l2_cropcap cropcap;
	int ret = 0;

	memset(&cropcap, 0, sizeof(cropcap));
	memset(&crop, 0, sizeof(crop));

	cropcap.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	if ((ret = ioctl(fd, VIDIOC_CROPCAP, &cropcap)) < 0) {
		log("VIDIOC_CROPCAP failed with %d\n", ret);
		return ret;
	}

	crop.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	crop.c = cropcap.defrect;
	/* Ignore if cropping is not supported (EINVAL). */
	if (((ret = ioctl(fd, VIDIOC_S_CROP, &crop)) < 0)
	    && errno != EINVAL) {
		log("VIDIOC_S_CROP failed with %d\n", ret);
		return ret;
	}
	return ret;
}

int v4l2_get_cropping_limits(int fd, struct v4l2_rect *limits)
{
	int retval;
	struct v4l2_cropcap cropcap;

	cropcap.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	if ((retval = ioctl(fd, VIDIOC_CROPCAP, &cropcap)) < 0) {
		log("VIDIOC_CROPCAP failed: %d", retval);
		return retval;
	}

	memcpy(limits, &cropcap.bounds, sizeof(cropcap.bounds));
	return 0;
}

int v4l2_crop_input(int fd, struct v4l2_rect *crop_rectangle)
{
	int retval;
	struct v4l2_crop crop;
	struct v4l2_rect crop_bounds;

	/* Sanity check */
	retval = v4l2_get_cropping_limits(fd, &crop_bounds);
	if (retval < 0)
		return retval;

	if (crop_rectangle->left + crop_rectangle->width > crop_bounds.left +
	    crop_bounds.width || crop_rectangle->top +
	    crop_rectangle->height > crop_bounds.top + crop_bounds.height) {
		log("Invalid input\n");
		return -EINVAL;
	}

	crop.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	memcpy(&crop.c, crop_rectangle, sizeof(crop.c));

	retval = ioctl(fd, VIDIOC_S_CROP, &crop);
	if (retval < 0) {
		log("VIDIOC_S_CROP failed: %d", retval);
		return retval;

	}

	return 0;
}

int v4l2_area_zoom(int fd, struct v4l2_rect *crop_rectangle)
{
	int retval;
	struct v4l2_rect crop_bounds;

	retval = v4l2_overlay_control(fd, 0);
	if (retval < 0)
		return retval;

	retval = v4l2_crop_input(fd, crop_rectangle);
	if (retval < 0) {
		if (retval == -EINVAL) {
			v4l2_get_cropping_limits(fd, &crop_bounds);
			printf("\nInvalid rectangle!\n");
			printf("Input bounds are:\tTop: %d\tLeft: %d\tWidth: %d\tHeight: %d",
			       crop_bounds.top, crop_bounds.left, crop_bounds.width,
			       crop_bounds.height);
			printf("\nRequested rectangle: \tTop: %d\tLeft: %d"
			       "\tWidth: %d\tHeight: %d",
			       crop_rectangle->top, crop_rectangle->left,
			       crop_rectangle->width, crop_rectangle->height);
			printf("\nValid rectangles are those which:\n"
			       "Top + Height < Bound_Top + Bound_Height\n"
			       "and\n" "Left + Width < Bound_Left + Bound_Width\n\n");
		}
		return retval;
	}

	retval = v4l2_overlay_control(fd, 1);
	if (retval < 0)
		return retval;

	return 0;
}

int v4l2_zoom(int fd, int zoom_percentage)
{
	int retval;
	float zoom;
	struct v4l2_rect crop_rectangle;
	struct v4l2_rect crop_bounds;

	/* Get cropping capabilities */
	retval = v4l2_get_cropping_limits(fd, &crop_bounds);
	if (retval < 0)
		return retval;

	/* Calculate new values */
	zoom = (float)zoom_percentage / 100;
	crop_rectangle.height = crop_bounds.height / zoom;
	crop_rectangle.width = crop_bounds.width / zoom;
	crop_rectangle.left = (crop_bounds.width - crop_rectangle.width) / 2;
	crop_rectangle.top = (crop_bounds.height - crop_rectangle.height) / 2;

	return v4l2_area_zoom(fd, &crop_rectangle);
}

int v4l2_set_control(int fd, int id, int value)
{
	int retval;
	struct v4l2_control control;

	control.id = id;
	control.value = value;

	retval = ioctl(fd, VIDIOC_S_CTRL, &control);
	if (retval < 0) {
		log("VIDIOC_S_CTRL failed. Id: %d\tValue: %d\n", id, value);
	}

	return retval;
}

int v4l2_get_control(int fd, int id, int *value)
{
	int retval;
	struct v4l2_control control;

	control.id = id;
	control.value = 0;
	retval = ioctl(fd, VIDIOC_G_CTRL, &control);
	if (retval < 0) {
		log("VIDIOC_G_CTRL failed. Id: %d\tValue: %d\n", id, *value);
	} else {
		*value = control.value;
	}

	return retval;
}

int v4l2_rotate(int fd, int rotate)
{
	/* This is not supported by V4L2, it's done by the IPUv3 driver */
	int retval;

	/* Stop overlay */
	retval = v4l2_overlay_control(fd, 0);
	if (retval < 0)
		return retval;

	/* Set new rotation value */
	retval = v4l2_set_control(fd, V4L2_CID_MXC_VF_ROT, rotate);
	if (retval < 0)
		return retval;
	/* Start overlay */
	retval = v4l2_overlay_control(fd, 1);
	if (retval < 1)
		return retval;

	return retval;
}

int v4l2_set_brightness(int fd, int percentage)
{
	int retval;
	int brightness = percentage * BRIGHTNESS_MAX_VALUE / 100;

	retval = v4l2_set_control(fd, V4L2_CID_BRIGHTNESS, brightness);
	if (retval < 0) {
		log("Set control V4L2_CID_BRIGHTNESS failed. Error: %d\n", retval);
	}

	return retval;
}

int v4l2_get_brightness(int fd, int *percentage)
{
	int retval;
	int brightness = 0;

	retval = v4l2_get_control(fd, V4L2_CID_BRIGHTNESS, &brightness);
	if (retval < 0) {
		log("Get control V4L2_CID_BRIGHTNESS failed. Error: %s\n", strerror(errno));
	} else {
		/* (+ 0.5 * BRIGHTNESS_MAX_VALUE) to round integer not using libm */
		*percentage = (brightness * 100 + 0.5 * BRIGHTNESS_MAX_VALUE) / BRIGHTNESS_MAX_VALUE;
	}

	return retval;
}

int v4l2_set_saturation(int fd, int percentage)
{
	int retval;
	int saturation = 0;

	/* Valid values are 0, 25, 37, 50, 75, 100 and 150 (hardware limitation) */
	if (percentage < 0 || percentage > 150) {
		log("Invalid parameter\n");
		saturation = 50;
	} else if (percentage == 0) {
		saturation = 0;
	} else if (percentage <= 25) {
		saturation = 25;
	} else if (percentage <= 37) {
		saturation = 37;
	} else if (percentage <= 50) {
		saturation = 50;
	} else if (percentage <= 75) {
		saturation = 75;
	} else if (percentage <= 100) {
		saturation = 100;
	} else if (percentage <= 150) {
		saturation = 150;
	}

	retval = v4l2_set_control(fd, V4L2_CID_SATURATION, saturation);
	if (retval < 0) {
		log("Set control V4L2_CID_SATURATION failed. Error: %s\n", strerror(errno));
	}

	return retval;
}

int v4l2_get_saturation(int fd, int *percentage)
{
	int retval;
	int saturation = 0;

	retval = v4l2_get_control(fd, V4L2_CID_SATURATION, &saturation);
	if (retval < 0) {
		log("Get control V4L2_CID_SATURATION failed. Error: %s\n", strerror(errno));
	} else {
		*percentage = saturation;
	}

	return retval;
}

int v4l2_set_red_balance(int fd, int red)
{
	int retval;

	/* Sanity check */
	if (red > 0xFF) {
		log("Invalid parameter\n");
		errno = EINVAL;
		return -1;
	}
	retval = v4l2_set_control(fd, V4L2_CID_RED_BALANCE, red);
	if (retval < 0) {
		log("Set control V4L2_CID_RED_BALANCE failed. Error: %s\n", strerror(errno));
	}
	return retval;
}

int v4l2_set_blue_balance(int fd, int blue)
{
	int retval;

	/* Sanity check */
	if (blue > 0xFF) {
		log("Invalid parameter\n");
		errno = EINVAL;
		return -1;
	}

	/* Set blue value */
	retval = v4l2_set_control(fd, V4L2_CID_BLUE_BALANCE, blue);
	if (retval < 0) {
		log("Set control V4L2_CID_BLUE_BALANCE failed. Error: %d\n", retval);
	}

	return retval;
}

int v4l2_set_black_level(int fd, int black)
{
	int retval;

	/* Sanity check */
	if (black > 0xFF) {
		log("Invalid parameter\n");
		errno = EINVAL;
		return -1;
	}

	/* Set black value */
	retval = v4l2_set_control(fd, V4L2_CID_BLACK_LEVEL, black);
	if (retval < 0) {
		log("Set control V4L2_CID_BLACK_LEVEL failed. Error: %d\n", retval);
	}

	return retval;
}

int v4l2_get_blue_balance(int fd, int *blue)
{
	int retval;

	/* Sanity check */
	if (blue == NULL) {
		log("Invalid parameter\n");
		errno = EINVAL;
		return -1;
	}

	retval = v4l2_get_control(fd, V4L2_CID_BLUE_BALANCE, blue);
	if (retval < 0) {
		log("Get control V4L2_CID_BLUE_BALANCE failed. Error: %d\n", retval);
	}

	return retval;
}

int v4l2_get_red_balance(int fd, int *red)
{
	int retval;

	/* Sanity check */
	if (red == NULL) {
		log("Invalid parameter\n");
		errno = EINVAL;
		return -1;
	}

	retval = v4l2_get_control(fd, V4L2_CID_RED_BALANCE, red);
	if (retval < 0) {
		log("Get control V4L2_CID_RED_BALANCE failed. Error: %d\n", retval);
	}

	return retval;
}

int v4l2_get_black_level(int fd, int *black)
{
	int retval;

	/* Sanity check */
	if (black == NULL) {
		log("Invalid parameter\n");
		errno = EINVAL;
		return -1;
	}

	retval = v4l2_get_control(fd, V4L2_CID_BLACK_LEVEL, black);
	if (retval < 0) {
		log("Get control V4L2_CID_BLACK_LEVEL failed. Error: %d\n", retval);
	}

	return retval;
}

int v4l2_global_alpha_set(int fd, int enable, int percentage)
{
	int retval;
	struct mxcfb_gbl_alpha global_alpha;

	global_alpha.enable = enable;
	global_alpha.alpha = percentage * GLOBAL_ALPHA_MAX_VALUE / 100;

	retval = ioctl(fd, MXCFB_SET_GBL_ALPHA, &global_alpha);
	if (retval < 0)
		log("Set global alpha failed MXCFB_SET_GBL_ALPHA. Error: %s\n",
		    strerror(errno));

	return retval;
}

/* Set overlay background framebuffer through sysfs */
int v4l2_sysfs_set_overlay_bg(char *fb_device)
{
	int ret = 0;
	FILE *fp = NULL;

	/* +5 to remove '/dev/' */
	if (!strncmp(fb_device + 5, "fb0", 3)) {
		fp = fopen(SYSFS_FSL_DISP_PROPERTY_FB0, "w");
	} else if (!strncmp(fb_device + 5, "fb1", 3)) {
		fp = fopen(SYSFS_FSL_DISP_PROPERTY_FB1, "w");
	}

	if (fp) {
		fprintf(fp, "%s", "2-layer-fb-bg");
		fclose(fp);
	} else {
		ret = -1;
	}

	return ret;
}


void fill_alpha_buffer(char *alpha_buf, int left, int top, int right,
		       int bottom, char alpha_val, int alpha_fb_w)
{
	char *pPointAlphaValue;
	int x, y;

	for (y = top; y < bottom; y++) {
		for (x = left; x < right; x++) {
			pPointAlphaValue = (char *)(alpha_buf + alpha_fb_w * y + x);
			*pPointAlphaValue = alpha_val;
		}
	}
}

void fill_alpha_rect(char *alpha_buf, struct v4l2_rect *rect, char alpha_val, int alpha_fb_w)
{
	char *pPointAlphaValue;
	int x, y;

	for (y = rect->top; y < rect->height; y++) {
		for (x = rect->left; x < rect->width; x++) {
			pPointAlphaValue = (char *)(alpha_buf + alpha_fb_w * y + x);
			*pPointAlphaValue = alpha_val;
		}
	}
}

int v4l2_local_alpha_set(ARGUMENTS * args, int fd_fb_fg,
			 struct v4l2_rect *alpha_rectangle, int alpha_percentage)
{
	struct fb_var_screeninfo fb_fg_var;
	struct mxcfb_loc_alpha l_alpha;
	struct v4l2_rect aux_rect;	/* Used for setting the default alpha */
	unsigned long loc_alpha_phy_addr0;
	unsigned long loc_alpha_phy_addr1;
	char *alpha_buf0 = NULL;
	char *alpha_buf1 = NULL;
	int alpha_buf_size;
	int alpha_value = (100 - alpha_percentage) * LOCAL_ALPHA_MIN_VALUE / 100;

	/* Sanity check */
	if (alpha_rectangle->left + alpha_rectangle->width > args->options.width ||
	    alpha_rectangle->top + alpha_rectangle->height > args->options.height) {
		log("Invalid alpha_rectangle\n");
		return -1;
	}

	/* Switch off overlay */
	v4l2_overlay_control(args->fd_in, 0);

	/* Fetch current fb var info */
	if (v4l2_fb_get_vinfo(fd_fb_fg, &fb_fg_var) < 0) {
		log("Failed get varinfo %s\n", strerror(errno));
		close(fd_fb_fg);
		return -1;
	}

	/* Set dimensions */
	fb_fg_var.xres = args->options.width;
	fb_fg_var.yres = args->options.height;
	fb_fg_var.xres_virtual = args->options.width;
	fb_fg_var.yres_virtual = args->options.height * 2;

	if (v4l2_fb_put_vinfo(fd_fb_fg, &fb_fg_var) < 0) {
		log("Failed get varinfo %s\n", strerror(errno));
		close(fd_fb_fg);
		return -1;
	}

	/* Assure we have the correct var info */
	if (v4l2_fb_get_vinfo(fd_fb_fg, &fb_fg_var) < 0) {
		log("Failed get varinfo %s\n", strerror(errno));
		close(fd_fb_fg);
		return -1;
	}

	/* Enable local alpha */
	memset(&l_alpha, 0, sizeof(struct mxcfb_loc_alpha));
	l_alpha.enable = 1;
	l_alpha.alpha_phy_addr0 = 0;
	l_alpha.alpha_phy_addr1 = 0;
	l_alpha.alpha_in_pixel = 0;

	if (ioctl(fd_fb_fg, MXCFB_SET_LOC_ALPHA, &l_alpha) < 0) {
		log("Set local alpha failed: %s\n", strerror(errno));
		close(fd_fb_fg);
		return -1;
	}

	/* mmap the memory */
	loc_alpha_phy_addr0 = (unsigned long)(l_alpha.alpha_phy_addr0);
	loc_alpha_phy_addr1 = (unsigned long)(l_alpha.alpha_phy_addr1);

	alpha_buf_size = fb_fg_var.xres * fb_fg_var.yres;

	alpha_buf0 = (char *)mmap(0, alpha_buf_size, PROT_READ | PROT_WRITE,
				  MAP_SHARED, fd_fb_fg, loc_alpha_phy_addr0);
	if ((int)alpha_buf0 == -1) {
		log("\nError: failed to map alpha buffer 0 to memory: %s\n", strerror(errno));
		close(fd_fb_fg);
		return -1;
	}

	alpha_buf1 = (char *)mmap(0, alpha_buf_size, PROT_READ | PROT_WRITE,
				  MAP_SHARED, fd_fb_fg, loc_alpha_phy_addr1);
	if ((int)alpha_buf1 == -1) {
		log("\nError: failed to map alpha buffer 1 to memory: %s\n", strerror(errno));
		munmap((void *)alpha_buf0, alpha_buf_size);
		close(fd_fb_fg);
		return -1;
	}

	/* Switch on overlay */
	v4l2_overlay_control(args->fd_in, 1);

	/* Initialize the rectangle */
	aux_rect.top = 0;
	aux_rect.left = 0;
	aux_rect.width = args->options.width;
	aux_rect.height = args->options.height;

	fill_alpha_rect(alpha_buf0, &aux_rect, 0xFF, args->options.width);

	if (ioctl(fd_fb_fg, MXCFB_SET_LOC_ALP_BUF, &loc_alpha_phy_addr0) < 0) {
		printf("Set local alpha buf failed\n");
		close(fd_fb_fg);
		return -1;
	}

	/* Fill and display the rectangle */
	fill_alpha_rect(alpha_buf0, alpha_rectangle, alpha_value, args->options.width);

	if (ioctl(fd_fb_fg, MXCFB_SET_LOC_ALP_BUF, &loc_alpha_phy_addr0) < 0) {
		printf("Set local alpha buf failed\n");
		close(fd_fb_fg);
		return -1;
	}

	return 0;
}
