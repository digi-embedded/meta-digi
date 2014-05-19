/*
 * v4l2_defs.h
 *
 * Copyright (C) 2012 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: V4L2 definitions header file.
 *
 * From linux/ipu.h
 *
 */

#ifndef V4L2_DEFS_H_
#define V4L2_DEFS_H_

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <linux/fb.h>
#include <linux/mxcfb.h>
#include <linux/videodev.h>

#define fourcc(a,b,c,d)\
        (((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))

/*!
 * @name IPU Pixel Formats
 *
 * Pixel formats are defined with ASCII FOURCC code. The pixel format codes are
 * the same used by V4L2 API.
 */

/*! @{ */
/*! @name Generic or Raw Data Formats */
/*! @{ */
#define IPU_PIX_FMT_GENERIC fourcc('I', 'P', 'U', '0')	/*!< IPU Generic Data */
#define IPU_PIX_FMT_GENERIC_32 fourcc('I', 'P', 'U', '1')	/*!< IPU Generic Data */
#define IPU_PIX_FMT_LVDS666 fourcc('L', 'V', 'D', '6')	/*!< IPU Generic Data */
#define IPU_PIX_FMT_LVDS888 fourcc('L', 'V', 'D', '8')	/*!< IPU Generic Data */
/*! @} */
/*! @name RGB Formats */
/*! @{ */
#define IPU_PIX_FMT_RGB332  fourcc('R', 'G', 'B', '1')	/*!<  8  RGB-3-3-2    */
#define IPU_PIX_FMT_RGB555  fourcc('R', 'G', 'B', 'O')	/*!< 16  RGB-5-5-5    */
#define IPU_PIX_FMT_RGB565  fourcc('R', 'G', 'B', 'P')	/*!< 1 6  RGB-5-6-5   */
#define IPU_PIX_FMT_RGB666  fourcc('R', 'G', 'B', '6')	/*!< 18  RGB-6-6-6    */
#define IPU_PIX_FMT_BGR666  fourcc('B', 'G', 'R', '6')	/*!< 18  BGR-6-6-6    */
#define IPU_PIX_FMT_BGR24   fourcc('B', 'G', 'R', '3')	/*!< 24  BGR-8-8-8    */
#define IPU_PIX_FMT_RGB24   fourcc('R', 'G', 'B', '3')	/*!< 24  RGB-8-8-8    */
#define IPU_PIX_FMT_GBR24   fourcc('G', 'B', 'R', '3')	/*!< 24  GBR-8-8-8    */
#define IPU_PIX_FMT_BGR32   fourcc('B', 'G', 'R', '4')	/*!< 32  BGR-8-8-8-8  */
#define IPU_PIX_FMT_BGRA32  fourcc('B', 'G', 'R', 'A')	/*!< 32  BGR-8-8-8-8  */
#define IPU_PIX_FMT_RGB32   fourcc('R', 'G', 'B', '4')	/*!< 32  RGB-8-8-8-8  */
#define IPU_PIX_FMT_RGBA32  fourcc('R', 'G', 'B', 'A')	/*!< 32  RGB-8-8-8-8  */
#define IPU_PIX_FMT_ABGR32  fourcc('A', 'B', 'G', 'R')	/*!< 32  ABGR-8-8-8-8 */
/*! @} */
/*! @name YUV Interleaved Formats */
/*! @{ */
#define IPU_PIX_FMT_YUYV    fourcc('Y', 'U', 'Y', 'V')	/*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_UYVY    fourcc('U', 'Y', 'V', 'Y')	/*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_YVYU    fourcc('Y', 'V', 'Y', 'U')  /*!< 16 YVYU 4:2:2 */
#define IPU_PIX_FMT_VYUY    fourcc('V', 'Y', 'U', 'Y')  /*!< 16 VYYU 4:2:2 */
#define IPU_PIX_FMT_Y41P    fourcc('Y', '4', '1', 'P')	/*!< 12 YUV 4:1:1 */
#define IPU_PIX_FMT_YUV444  fourcc('Y', '4', '4', '4')	/*!< 24 YUV 4:4:4 */
#define IPU_PIX_FMT_VYU444  fourcc('V', '4', '4', '4')	/*!< 24 VYU 4:4:4 */
/* two planes -- one Y, one Cb + Cr interleaved  */
#define IPU_PIX_FMT_NV12    fourcc('N', 'V', '1', '2') /* 12  Y/CbCr 4:2:0  */
/*! @} */
/*! @name YUV Planar Formats */
/*! @{ */
#define IPU_PIX_FMT_GREY    fourcc('G', 'R', 'E', 'Y')	/*!< 8  Greyscale */
#define IPU_PIX_FMT_YVU410P fourcc('Y', 'V', 'U', '9')	/*!< 9  YVU 4:1:0 */
#define IPU_PIX_FMT_YUV410P fourcc('Y', 'U', 'V', '9')	/*!< 9  YUV 4:1:0 */
#define IPU_PIX_FMT_YVU420P fourcc('Y', 'V', '1', '2')	/*!< 12 YVU 4:2:0 */
#define IPU_PIX_FMT_YUV420P fourcc('I', '4', '2', '0')	/*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YUV420P2 fourcc('Y', 'U', '1', '2')	/*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YVU422P fourcc('Y', 'V', '1', '6')	/*!< 16 YVU 4:2:2 */
#define IPU_PIX_FMT_YUV422P fourcc('4', '2', '2', 'P')	/*!< 16 YUV 4:2:2 */
/*! @} */

#define V4L2_CID_MXC_FLASH		(V4L2_CID_PRIVATE_BASE + 1)
#define V4L2_CID_MXC_VF_ROT		(V4L2_CID_PRIVATE_BASE + 2)

extern int verbose;

#define TRUE 1
#define FALSE 0

#define SYSFS_FSL_DISP_PROPERTY_FB0	"/sys/class/graphics/fb0/fsl_disp_property"
#define SYSFS_FSL_DISP_PROPERTY_FB1	"/sys/class/graphics/fb1/fsl_disp_property"

#define log( fmt, arg...)							\
	do { 									\
		char msg[256];							\
		if(verbose){							\
			snprintf(msg,sizeof(msg),"[%s:%d]"fmt , __FUNCTION__,	\
				__LINE__, ## arg);				\
			printf("%s",msg);}					\
	} while (0)

typedef struct {
	int format;
	int top;
	int left;
	int height;
	int width;
	int non_destructive;
	int camera_framerate;
} OPTIONS;

typedef struct {
	int fd_in;
	int fd_out;
	char v4l2_device[13];	/* /dev/videoNN + '\0' */
	char fb_device[9];	/* /dev/fbN + '\0' */
	OPTIONS options;
} ARGUMENTS;

int v4l2_get_overlay_bg(char *fb_device);
int v4l2_overlay_control(int fd_overlay, int start);
char *v4l2_fmt_str(int pixelformat);
int v4l2_is_capability_supported(int fd, int capability);
int v4l2_is_video_std_supported(int fd, int standard);
int v4l2_is_format_supported(int fd, int format);
int v4l2_check_frame_rate(int fd, int framerate);
int v4l2_set_stream_parms(int fd, int framerate);
int v4l2_check_output(int fd, char *name);
int v4l2_set_output(int fd, struct fb_fix_screeninfo *fb_fix);
int v4l2_set_frame_rate(int fd, int framerate);
int v4l2_check_frame_rate(int fd, int framerate);
int v4l2_set_format_overlay(int fd, int top, int left, int height, int width);
int v4l2_set_format_capture(int fd, int pixelformat, int height, int width);
int v4l2_fb_get_info(char *fb_device, struct fb_fix_screeninfo *fb_fix,
		     struct fb_var_screeninfo *fb_var);
void v4l2_ident_outputs(int fd);
int v4l2_mxc_find_overlay(struct fb_fix_screeninfo *fb_fix, int *fb_fg_fd);
int v4l2_overlay_set_framebuffer(int fd, struct fb_fix_screeninfo *fb_fix,
				 struct fb_var_screeninfo *fb_var, int non_destructive);
int v4l2_reset_cropping_rectangle(int fd);
int v4l2_fb_blank(int fd);
int v4l2_fb_unblank(int fd);
int v4l2_zoom(int fd, int zoom);
int v4l2_rotate(int fd, int rotate);
int v4l2_crop_input(int fd, struct v4l2_rect *crop_rectangle);
int v4l2_get_cropping_limits(int fd, struct v4l2_rect *limits);
int v4l2_area_zoom(int fd, struct v4l2_rect *crop_rectangle);
int v4l2_set_brightness(int fd, int brightness);
int v4l2_get_brightness(int fd, int *brightness);
int v4l2_set_saturation(int fd, int saturation);
int v4l2_get_saturation(int fd, int *saturation);
int v4l2_set_red_balance(int fd, int red);
int v4l2_set_blue_balance(int fd, int blue);
int v4l2_set_black_level(int fd, int black);
int v4l2_get_red_balance(int fd, int *red);
int v4l2_get_blue_balance(int fd, int *blue);
int v4l2_get_black_level(int fd, int *black);
int v4l2_global_alpha_set(int fd, int enable, int value);
int v4l2_sysfs_set_overlay_bg(char *fb_device);
int v4l2_local_alpha_set(ARGUMENTS * args, int fd_fb_fg, struct v4l2_rect *alpha_rectangle,
			 int alpha_percentage);

#endif /* V4L2_DEFS_H_ */
