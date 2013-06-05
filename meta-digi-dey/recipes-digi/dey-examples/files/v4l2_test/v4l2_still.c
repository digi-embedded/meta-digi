/*
 * Copyright 2004-2010 Freescale Semiconductor, Inc. All rights reserved.
 * Copyright 2010 Digi International. All rights reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*
 * @file v4l2_still.c
 *
 * @brief Video For Linux 2 driver test application
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

    /*=======================================================================
                                            INCLUDE FILES
    =======================================================================*/
    /* Standard Include Files */
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <asm/types.h>
#include <linux/videodev.h>
#include <sys/mman.h>
#include <string.h>
#include <malloc.h>

#define ipu_fourcc(a,b,c,d)\
        (((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))

#define IPU_PIX_FMT_YUYV    ipu_fourcc('Y','U','Y','V') /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_UYVY    ipu_fourcc('U','Y','V','Y') /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_NV12    ipu_fourcc('N','V','1','2') /* 12 Y/CbCr 4:2:0 */
#define IPU_PIX_FMT_YUV420P ipu_fourcc('I','4','2','0') /*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YUV420P2 ipu_fourcc('Y','U','1','2') /*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YUV422P ipu_fourcc('4','2','2','P') /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_YUV444  ipu_fourcc('Y','4','4','4') /*!< 24 YUV 4:4:4 */
#define IPU_PIX_FMT_RGB565  ipu_fourcc('R','G','B','P') /*!< 16 RGB-5-6-5 */
#define IPU_PIX_FMT_BGR24   ipu_fourcc('B','G','R','3') /*!< 24 BGR-8-8-8 */
#define IPU_PIX_FMT_RGB24   ipu_fourcc('R','G','B','3') /*!< 24 RGB-8-8-8 */
#define IPU_PIX_FMT_BGR32   ipu_fourcc('B','G','R','4') /*!< 32 BGR-8-8-8-8 */
#define IPU_PIX_FMT_BGRA32  ipu_fourcc('B','G','R','A') /*!< 32 BGR-8-8-8-8 */
#define IPU_PIX_FMT_RGB32   ipu_fourcc('R','G','B','4') /*!< 32 RGB-8-8-8-8 */
#define IPU_PIX_FMT_RGBA32  ipu_fourcc('R','G','B','A') /*!< 32 RGB-8-8-8-8 */
#define IPU_PIX_FMT_ABGR32  ipu_fourcc('A','B','G','R') /*!< 32 ABGR-8-8-8-8 */

    static int g_convert = 0;
    static int g_width = 640;
    static int g_height = 480;
    static int g_top = 0;
    static int g_left = 0;
    static unsigned long g_pixelformat = IPU_PIX_FMT_UYVY;
    static int g_bpp = 16;
    static int g_camera_framerate = 30;
    static int g_capture_mode = 0;

    void usage(void)
    {
        printf("Usage: v4l2_still [-w width] [-h height] [-t top] [-l left] [-f pixformat] [-c] [-m] [-fr]\n"
               "-w    Image width, 640 by default\n"
               "-h    Image height, 480 by default\n"
               "-t    Image top(crop from the source frame), 0 by default\n"
               "-l    Image left(crop from the source frame), 0 by default\n"
               "-f    Image pixel format, YUV420, YUV422P, YUYV, UYVY ((default) or YUV444\n"
               "-c    Convert to YUV420P. This option is valid for interleaved pixel\n"
               "      formats only - YUYV, UYVY, YUV444\n"
               "-m    Capture mode, 0-low resolution(default), 1-high resolution \n"
               "-fr   Capture frame rate, 30fps by default\n"
               "The output is saved in ./still.uyvy\n"
              );
    }

    /* Convert to YUV420 format */
    void fmt_convert(char *dest, char *src, struct v4l2_format *fmt)
    {
        int row, col, pos = 0;
        int bpp, yoff, uoff, voff;

        if (fmt->fmt.pix.pixelformat == IPU_PIX_FMT_YUYV) {
            bpp = 2;
            yoff = 0;
            uoff = 1;
            voff = 3;
        }
        else if (fmt->fmt.pix.pixelformat == IPU_PIX_FMT_UYVY) {
            bpp = 2;
            yoff = 1;
            uoff = 0;
            voff = 2;
        }
        else {	/* YUV444 */
            bpp = 4;
            yoff = 0;
            uoff = 1;
            voff = 2;
        }

        /* Copy Y */
        for (row = 0; row < fmt->fmt.pix.height; row++)
            for (col = 0; col < fmt->fmt.pix.width; col++)
                dest[pos++] = src[row * fmt->fmt.pix.bytesperline + col * bpp + yoff];

        /* Copy U */
        for (row = 0; row < fmt->fmt.pix.height; row += 2) {
            for (col = 0; col < fmt->fmt.pix.width; col += 2)
                dest[pos++] = src[row * fmt->fmt.pix.bytesperline + col * bpp + uoff];
        }

        /* Copy V */
        for (row = 0; row < fmt->fmt.pix.height; row += 2) {
            for (col = 0; col < fmt->fmt.pix.width; col += 2)
                dest[pos++] = src[row * fmt->fmt.pix.bytesperline + col * bpp + voff];
        }
    }

    int bytes_per_pixel(int fmt)
    {
        switch (fmt) {
        case IPU_PIX_FMT_YUV420P:
        case IPU_PIX_FMT_YUV422P:
        case IPU_PIX_FMT_NV12:
            return 1;
            break;
        case IPU_PIX_FMT_RGB565:
        case IPU_PIX_FMT_YUYV:
        case IPU_PIX_FMT_UYVY:
            return 2;
            break;
        case IPU_PIX_FMT_BGR24:
        case IPU_PIX_FMT_RGB24:
            return 3;
            break;
        case IPU_PIX_FMT_BGR32:
        case IPU_PIX_FMT_BGRA32:
        case IPU_PIX_FMT_RGB32:
        case IPU_PIX_FMT_RGBA32:
        case IPU_PIX_FMT_ABGR32:
            return 4;
            break;
        default:
            return 1;
            break;
        }
        return 0;
    }

    int v4l_capture_setup(int * fd_v4l)
    {
        char v4l_device[100] = "/dev/video0";
        struct v4l2_streamparm parm;
        struct v4l2_format fmt;
        struct v4l2_crop crop;
        int ret = 0;

        if ((*fd_v4l = open(v4l_device, O_RDWR, 0)) < 0)
        {
            printf("Unable to open %s\n", v4l_device);
            return -1;
        }

        parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        parm.parm.capture.timeperframe.numerator = 1;
        parm.parm.capture.timeperframe.denominator = g_camera_framerate;
        parm.parm.capture.capturemode = g_capture_mode;

        if ((ret = ioctl(*fd_v4l, VIDIOC_S_PARM, &parm)) < 0)
        {
            printf("VIDIOC_S_PARM failed\n");
            return ret;
        }

        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c.left = g_left;
        crop.c.top = g_top;
        crop.c.width = g_width;
        crop.c.height = g_height;
        if ((ret = ioctl(*fd_v4l, VIDIOC_S_CROP, &crop)) < 0)
        {
            printf("set cropping failed\n");
            return ret;
        }

        memset(&fmt, 0, sizeof(fmt));
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.pixelformat = g_pixelformat;
        fmt.fmt.pix.width = g_width;
        fmt.fmt.pix.height = g_height;
        fmt.fmt.pix.sizeimage = fmt.fmt.pix.width * fmt.fmt.pix.height * g_bpp / 8;
        fmt.fmt.pix.bytesperline = g_width * bytes_per_pixel(g_pixelformat);

        if ((ret = ioctl(*fd_v4l, VIDIOC_S_FMT, &fmt)) < 0)
        {
            printf("set format failed\n");
            return ret;
        }

        return ret;
    }

    int v4l_capture_test(int fd_v4l)
    {
        struct v4l2_format fmt;
        int fd_still = 0, ret = 0;
        char *buf1, *buf2;
        char still_file[100] = "./still.uyvy";
        int bytes = 0;

        if ((fd_still = open(still_file, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) < 0)
        {
            printf("Unable to create y frame recording file\n");
            return -1;
        }

        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if ((ret = ioctl(fd_v4l, VIDIOC_G_FMT, &fmt)) < 0) {
            printf("get format failed\n");
            goto exit1;
        } else {
            printf("\t Width = %d\n", fmt.fmt.pix.width);
            printf("\t Height = %d\n", fmt.fmt.pix.height);
            printf("\t Image size = %d\n", fmt.fmt.pix.sizeimage);
            printf("\t Pixel format = %c%c%c%c\n",
                   (char)(fmt.fmt.pix.pixelformat & 0xFF),
                   (char)((fmt.fmt.pix.pixelformat & 0xFF00) >> 8),
                   (char)((fmt.fmt.pix.pixelformat & 0xFF0000) >> 16),
                   (char)((fmt.fmt.pix.pixelformat & 0xFF000000) >> 24));
        }

        buf1 = (char *)malloc(fmt.fmt.pix.sizeimage);
        buf2 = (char *)malloc(fmt.fmt.pix.sizeimage);
        if (!buf1 || !buf2)
            goto exit0;

        memset(buf1, 0, fmt.fmt.pix.sizeimage);
        memset(buf2, 0, fmt.fmt.pix.sizeimage);

        if ((bytes = read(fd_v4l, buf1, fmt.fmt.pix.sizeimage)) != fmt.fmt.pix.sizeimage) {
            printf("v4l2 read error.\n");
            printf("read %d, expected %d.\n",bytes, fmt.fmt.pix.sizeimage);
            goto exit0;
        }

        if ((g_convert == 1) && (g_pixelformat != IPU_PIX_FMT_YUV422P)
                && (g_pixelformat != IPU_PIX_FMT_YUV420P2)) {
            fmt_convert(buf2, buf1, &fmt);
            if ((write(fd_still, buf2, fmt.fmt.pix.width * fmt.fmt.pix.height * 3 / 2)) < 0)
                goto exit0;
        }
        else
            if ((write(fd_still, buf1, fmt.fmt.pix.sizeimage)) < 0)
                goto exit0;

exit0:
        free(buf1);
        free(buf2);
        close(fd_v4l);
exit1:
        close(fd_still);

        return ret;
    }

    int main(int argc, char **argv)
    {
        int fd_v4l;
        int i;
        int ret;

        for (i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-w") == 0) {
                if (argv[++i])
                    g_width = atoi(argv[i]);
                else {
                    usage();
                    return -1;
                }
            }
            else if (strcmp(argv[i], "-h") == 0) {
                if (argv[++i])
                    g_height = atoi(argv[i]);
                else {
                    usage();
                    return -1;
                }
            }
            else if (strcmp(argv[i], "-t") == 0) {
                if (argv[++i])
                    g_top = atoi(argv[i]);
                else {
                    usage();
                    return -1;
                }
            }
            else if (strcmp(argv[i], "-l") == 0) {
                if (argv[++i])
                    g_left = atoi(argv[i]);
                else {
                    usage();
                    return -1;
                }
            }
            else if (strcmp(argv[i], "-c") == 0) {
                g_convert = 1;
            }
            else if (strcmp(argv[i], "-m") == 0) {
                if (argv[++i])
                    g_capture_mode = atoi(argv[i]);
                else {
                    usage();
                    return -1;
                }
            }
            else if (strcmp(argv[i], "-fr") == 0) {
                if (argv[++i])
                    g_camera_framerate = atoi(argv[i]);
                else {
                    usage();
                    return -1;
                }
            }
            else if (strcmp(argv[i], "-f") == 0) {
                i++;
                if (strcmp(argv[i], "NV12") == 0) {
                    g_pixelformat = IPU_PIX_FMT_NV12;
                    g_bpp = 12;
                }
                else if (strcmp(argv[i], "YUV420") == 0) {
                    g_pixelformat = IPU_PIX_FMT_YUV420P2;
                    g_bpp = 12;
                }
                else if (strcmp(argv[i], "YUV422P") == 0) {
                    g_pixelformat = IPU_PIX_FMT_YUV422P;
                    g_bpp = 16;
                }
                else if (strcmp(argv[i], "YUYV") == 0) {
                    g_pixelformat = IPU_PIX_FMT_YUYV;
                    g_bpp = 16;
                }
                else if (strcmp(argv[i], "UYVY") == 0) {
                    g_pixelformat = IPU_PIX_FMT_UYVY;
                    g_bpp = 16;
                }
                else if (strcmp(argv[i], "YUV444") == 0) {
                    g_pixelformat = IPU_PIX_FMT_YUV444;
                    g_bpp = 32;
                }
                else if (strcmp(argv[i], "RGB565") == 0) {
                    g_pixelformat = IPU_PIX_FMT_RGB565;
                    g_bpp = 16;
                }
                else {
                    printf("Pixel format not supported.\n");
                    usage();
                    return -1;
                }
            }
            else {
                usage();
                return -1;
            }
        }

        ret = v4l_capture_setup(&fd_v4l);
        if (ret)
            return ret;

        ret = v4l_capture_test(fd_v4l);

        return ret;
    }
