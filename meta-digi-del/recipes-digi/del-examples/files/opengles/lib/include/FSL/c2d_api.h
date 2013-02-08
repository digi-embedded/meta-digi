/*
    Copyright (C) 2010  QUALCOMM Incorporated.

    This library is free software; you can redistribute it and/or modify it
	under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation; either version 2.1 of the License, or (at
	your option) any later version.

    This library is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
	or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
	License for more details.

    You should have received a copy of the GNU Lesser General Public License
	along with this library; if not, write to the Free Software Foundation,
	Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/* -------------------------------------------------------------------------
 * \file    c2d_api.h
 * \brief   C2D API
 *//*----------------------------------------------------------------------*/


#ifndef __c2d_api_h_
#define __c2d_api_h_

#ifdef __cplusplus
extern "C" {
#endif
/* -------------------------------------------------------------------------
 * C2D Defines
 *//*----------------------------------------------------------------------*/

#ifndef _LINUX
#define C2D_API __declspec (dllexport)     /*!< DLL exports */
#ifndef C2D_API
#ifdef WIN32
#define C2D_API __declspec (dllexport)     /*!< DLL exports */
#else
#define C2D_API
#endif
#endif
#else //_LINUX
#define C2D_API
#endif //_LINUX

typedef void*                   C2D_CONTEXT; /*!< C2D Context type */
typedef void*                   C2D_SURFACE; /*!< C2D Surface type */

typedef enum __C2D_STATUS {         /*!< Status codes, returned by any c2d function */
    C2D_STATUS_OK              = 0, /*!<   operation was successful     */
    C2D_STATUS_FAILURE         = 1, /*!<   unspecified failure          */
    C2D_STATUS_NOT_SUPPORTED   = 2, /*!<   not supported functionality  */
    C2D_STATUS_OUT_OF_MEMORY   = 3, /*!<   memory allocation failed     */
    C2D_STATUS_INVALID_PARAM   = 4, /*!<   invalid parameter or combination of parameters */
} C2D_STATUS;


typedef enum _C2D_COLORFORMAT {             /*!< Color formats */
    /* 1bit formats (alpha mask) */
    C2D_COLOR_A1                            = 0,    /*!< 1bit per pixel (alpha mask)    */
    /* 4bit formats (alpha mask) */
    C2D_COLOR_A4                            = 1,    /*!< 4bit per pixel (alpha mask)    */
    /* 8bit formats */
    C2D_COLOR_A8                            = 2,    /*!< 8bit per pixel (alpha mask)    */
    C2D_COLOR_8                             = 3,    /*!< 8bit per pixel                 */
    /* 16bit formats */
    C2D_COLOR_4444                          = 4,    /*!< 16bit per pixel 4444 ARGB      */
    C2D_COLOR_4444_RGBA                     = 5,    /*!< 16bit per pixel 4444 RGBA      */
    C2D_COLOR_1555                          = 6,    /*!< 16bit per pixel 1555 ARGB      */
    C2D_COLOR_5551_RGBA                     = 7,    /*!< 16bit per pixel 5551 RGBA      */
    C2D_COLOR_0565                          = 8,    /*!< 16bit per pixel 0565 RGB       */
    /* 32bit formats */
    C2D_COLOR_8888                          = 9,    /*!< 32bit per pixel 8888 ARGB      */
    C2D_COLOR_8888_RGBA                     = 10,   /*!< 32bit per pixel 8888 RGBA      */
    C2D_COLOR_8888_ABGR                     = 11,   /*!< 32bit per pixel 8888 ABGR      */
    /* 24bit formats */
    C2D_COLOR_888                           = 12,   /*!< 24bit per pixel 888 BGR        */
    /* YUV Formats etc.   */
    C2D_NUMBER_OF_COLORFORMATS              = 13,    /*!< number of color formats; keep this the last */
    C2D_COLOR_DUMMY                         = (1<<30) /*!< dummy enum. C2D_COLORFORMAT is used in C2D_SURFACE_DEF struct,
                                                           this makes sure that C2D_COLORFORMAT size is aligned to 32bit  */
} C2D_COLORFORMAT;


typedef struct _C2D_RECT /*!< c2d rectangle              */
{
    int x;         /*!<   upper-left x */
    int y;         /*!<   upper-left y */
    int width;     /*!<   width */
    int height;    /*!<   height */
} C2D_RECT;

typedef struct _C2D_POINT
{
    int x;
    int y;
} C2D_POINT;


typedef struct _C2D_SURFACE_DEF { /*!< Structure for creating a c2d surface                  */
    C2D_COLORFORMAT format;     /*!< RGBA color format                                       */
    unsigned int    width;      /*!< defines width in pixels                                 */
    unsigned int    height;     /*!< defines height in pixels                                */
    unsigned int    stride;     /*!< set by c2dSurfAlloc, defines stride in bytes            */
    void           *buffer;     /*!< set by c2dSurfAlloc, physical address to surface buffer */
    void           *host;       /*!< set by c2dSurfAlloc, virtual address to surface buffer  */
    unsigned int    flags;      /*!< different flags to control the surface behavior         */
} C2D_SURFACE_DEF;

#define C2D_SURFACE_NO_BUFFER_ALLOC   1
#define C2D_SURFACE_CLIPRECT_OVERRIDE 2


typedef enum _C2D_GRADIENT_DIRECTION {  /*!< Direction of linear color fill */
    C2D_GD_LEFTTOP_RIGHTBOTTOM,  /*!< Left to Right, Top to Bottom */
    C2D_GD_RIGHTTOP_LEFTBOTTOM,  /*!< Right to Left, Top to Bottom */
    C2D_GD_LEFTBOTTOM_RIGHTTOP,  /*!< Left to Right, Bottom to Top */
    C2D_GD_RIGHTBOTTOM_LEFTTOP,  /*!< Right to Left, Bottom to Top */
    C2D_GD_TOP_BOTTOM,           /*!< Top to bottom */
    C2D_GD_LEFT_RIGHT,           /*!< Left to right */
    C2D_GD_BOTTOM_TOP,           /*!< Bottom up */
    C2D_GD_RIGHT_LEFT            /*!< Right to left */
} C2D_GRADIENT_DIRECTION;

typedef enum __C2D_STRETCH_MODE {    /*!< Stretching modes */
    C2D_STRETCH_POINT_SAMPLING,      /*!< Simple point sampling */
    C2D_STRETCH_BILINEAR_SAMPLING    /*!< Linear interpolation in x- and y-direction */
} C2D_STRETCH_MODE;

typedef enum __C2D_ALPHA_BLEND_MODE {    /*!< Blending modes enumeration */
    C2D_ALPHA_BLEND_NONE        = 0,     /*!< disables alpha blending */
    C2D_ALPHA_BLEND_SRCOVER     = 1,     /*!< Source Over Destination */
    C2D_ALPHA_BLEND_DIRECT      = 2,
} C2D_ALPHA_BLEND_MODE;

typedef enum __C2D_PARAMETERS {               /*!< Draw parameters                */
    C2D_PARAM_FILL_BIT        = (1<<0),       /*!< fill rect or arc               */
    C2D_PARAM_GRADIENT_BIT    = (1<<1),       /*!< fill with fg color to bg color */
    C2D_PARAM_PATTERN_BIT     = (1<<2),       /*!< fill with brush                */
    C2D_PARAM_TILING_BIT      = (1<<3),       /*!< tiling(repeat), no scaling for brush */
    C2D_PARAM_MIRROR_BIT      = (1<<4),       /*!< horizontal mirroring           */
    C2D_PARAM_LINE_LAST_PIXEL = (1<<5),       /*!< draw the last pixel of a line segment */
} C2D_PARAMETERS;


 typedef enum _C2D_DISPLAY {               /*!< Display enumeration */
    C2D_DISPLAY_MAIN        = (1 << 0),    /*!< main display */
    C2D_DISPLAY_SECONDARY   = (1 << 1),    /*!< secondary display */
    C2D_DISPLAY_TV_OUT      = (1 << 2),    /*!< tv-out etc. */
    C2D_DISPLAY_OVERLAY     = (1 << 3),    /*!< overlay window bit for display surface if overlay surfaces are supported */
    C2D_DISPLAY_BG          = (1 << 4),    /*!< background window bit */
} C2D_DISPLAY;


/* -------------------------------------------------------------------------
 *                          C2D API
 *//*----------------------------------------------------------------------*/

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Create C2D context,
 *          allocates and returns new handle to the draw state
 *
 * \param   a_c2dContext is pointer to C2D_CONTEXT where context id is stored
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dCreateContext(C2D_CONTEXT *a_c2dContext);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Destroy C2D context,
 *          free the given draw state
 *
 * \param   a_c2dContext is the C2D_CONTEXT where context was created
 *                       with c2dCreateContext function
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dDestroyContext(C2D_CONTEXT a_c2dContext);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Alloc C2D surface,
 *          allocates surface and returns handle to it
 *
 * \param   a_c2dSurface is the pointer to C2D_SURFACE where surface id is stored
 * \param   a_surfaceDef is the pointer to C2D_SURFACE_DEF
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSurfAlloc(C2D_CONTEXT a_c2dContext, C2D_SURFACE *a_c2dSurface, C2D_SURFACE_DEF *a_surfaceDef);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Free C2D surface,
 *          free the given surface
 *
 * \param   a_c2dSurface is the C2D_SURFACE
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSurfFree(C2D_CONTEXT a_c2dContext, C2D_SURFACE a_c2dSurface);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Lock C2D surface,
 *          Lock surface and return virtual address to surface buffer.
 *
 * \param   a_c2dContext is the context handle
 * \param   a_c2dSurface is the C2D_SURFACE
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSurfLock(C2D_CONTEXT a_c2dContext, C2D_SURFACE a_c2dSurface, void** a_ptr);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Unlock C2D surface,
 *          Give surface back for HW access
 *
 * \param   a_c2dContext is the context handle
 * \param   a_c2dSurface is the C2D_SURFACE
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSurfUnlock(C2D_CONTEXT a_c2dContext, C2D_SURFACE a_c2dSurface);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set destination surface
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_c2dSurface is C2D_SURFACE
 * \param   a_type is the C2D_SURFACE_TYPE
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetDstSurface(C2D_CONTEXT a_c2dContext, C2D_SURFACE a_c2dSurface);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set source surface
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_c2dSurface is C2D_SURFACE
 * \param   a_type is the C2D_SURFACE_TYPE
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetSrcSurface(C2D_CONTEXT a_c2dContext, C2D_SURFACE a_c2dSurface);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set ROP mode
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_rop is the 32bit rop mode
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetRop(C2D_CONTEXT a_c2dContext, unsigned int a_rop);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set foreground color
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_fgColor is the 32bit value for the fgcolor
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetFgColor(C2D_CONTEXT a_c2dContext, unsigned int a_fgColor);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set background color
 *
 * \param   a_c2dContext, C2D_CONTEXT
 * \param   a_bgColoris the 32bit value for the bgcolor
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetBgColor(C2D_CONTEXT a_c2dContext, unsigned int a_bgColor);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set gradient direction
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_direction is the C2D_GRADIENT_DIRECTION
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetGradientDirection(C2D_CONTEXT a_c2dContext, C2D_GRADIENT_DIRECTION a_direction);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set stretching mode
 *
 * \param   a_c2dContext, C2D_CONTEXT
 * \param   a_mode, C2D_STRETCH_MODE, how to perform stretching
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetStretchMode(C2D_CONTEXT a_c2dContext, C2D_STRETCH_MODE a_mode);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set source rectangle
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_rect is the C2D_RECT
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetSrcRectangle(C2D_CONTEXT a_c2dContext, C2D_RECT *a_rect);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set destination rectangle
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_rect is the C2D_RECT
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetDstRectangle(C2D_CONTEXT a_c2dContext, C2D_RECT *a_rect);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Sets the region of the destination surface that is clipped
 *          during rendering. Works similar to destination rectangle except
 *          clipping rectangle can be used to clip a stretched blt.
 *
 * \param   a_c2dContext, C2D_CONTEXT
 * \param   a_clipRect, *C2D_RECT, pass NULL to disable
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetDstClipRect(C2D_CONTEXT a_c2dContext, C2D_RECT *a_clipRect);


/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set source rotation
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_degree is the 32bit value for rotation
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetSrcRotate(C2D_CONTEXT a_c2dContext, unsigned int a_rotation);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set destination rotation
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_degree is the 32bit value for rotation
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetDstRotate(C2D_CONTEXT a_c2dContext, unsigned int a_rotation);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set alpha blend mode
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_mode is the C2D_ALPHA_BLEND_MODE
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetBlendMode(C2D_CONTEXT a_c2dContext, C2D_ALPHA_BLEND_MODE a_mode);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set global alpha
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_value is the 32bit value for global alpha, (8bit used -> 0-255)
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetGlobalAlpha(C2D_CONTEXT a_c2dContext, unsigned int a_value);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set source colorkey
 *
 * \param   a_c2dContext, C2D_CONTEXT
 * \param   a_color is the 32bit RGB value for source colorkey, alpha channel ignored
 * \param   a_bEnable, int, enable or disable the use of color key
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetSrcColorkey(C2D_CONTEXT a_c2dContext, unsigned int a_color, int a_bEnable);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set destination colorkey
 *
 * \param   a_c2dContext, C2D_CONTEXT
 * \param   a_color, unsigned int, RGB, alpha channel ignored
 * \param   a_bEnable, int, enable or disable the use of color key
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetDstColorkey(C2D_CONTEXT a_c2dContext, unsigned int a_color, int a_bEnable);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set surface as the current brush
 *
 * \param   a_c2dContext is C2D_CONTEXT
 * \param   a_c2dSurface is C2D_SURFACE
 * \param   a_tilingOffset, C2D_POINT*, offset added to upper left corner of the destination rectangle
 *          for brush aligment. Passing NULL equals to zero offset.
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetBrushSurface(C2D_CONTEXT a_c2dContext, C2D_SURFACE a_c2dSurface, C2D_POINT *a_tilingOffset);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set mask surface, if a_c2dSurface is NULL the mask usage is disabled
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_c2dSurface is the C2D_SURFACE
 * \param   a_offset, C2D_POINT*, mask offset to use, NULL means no offset
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetMaskSurface(C2D_CONTEXT a_c2dContext, C2D_SURFACE a_c2dSurface, C2D_POINT *a_offset);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Blits the source surface to the destination surface with current state
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dDrawBlit(C2D_CONTEXT a_c2dContext);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Draws a rectangle,
 *          The dest regtangle is used to set draw coordinates
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_drawConfig is the 32bit param containing the C2D_PARAMETERS.
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dDrawRect(C2D_CONTEXT a_c2dContext, C2D_PARAMETERS a_drawConfig);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Draws a line
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_drawConfig is the 32bit param containing the C2D_PARAMETERS.
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dDrawLine(C2D_CONTEXT a_c2dContext,
                               C2D_POINT *a_start, C2D_POINT *a_end,
                               unsigned int a_drawConfig);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Draws a circular or elliptical arc with coordinates from
 *          the destination rectangle
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \param   a_drawConfig is the 32bit param containing the C2D_PARAMETERS.
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dDrawArc(C2D_CONTEXT a_c2dContext, int a_startAngle, int a_arcAngle, unsigned int a_drawConfig);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Flush all the context draws to HW
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dFlush(C2D_CONTEXT a_c2dContext);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Flush all the context draws to HW and waits them to be executed
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dFinish(C2D_CONTEXT a_c2dContext);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Waits for the end-of-pipeline timestamp of the last submitted command buffer
 *
 * \param   a_c2dContext is the C2D_CONTEXT
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dWaitForTimestamp(C2D_CONTEXT a_c2dContext);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Get display info, a_displayInfo will be filled with information
 *
 * \param   a_display, C2D_DISPLAY
 * \param   a_displayInfo is the pointer to the C2D_SURFACE_DEF
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dGetDisplayInfo(C2D_DISPLAY a_display, C2D_SURFACE_DEF *a_displayInfo);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Set display surface
 *
 * \param   a_display is the C2D_DISPLAY
 * \param   a_c2dSurface is the C2D_SURFACE
 * \param   a_displayConfig is the colorkey enable etc. if supported by diplay controller
 * \param   a_configParam is the colorkey value if supported by display controller
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dSetDisplaySurface(C2D_DISPLAY a_display, C2D_SURFACE a_c2dSurface, unsigned int a_displayConfig, unsigned int a_configParam);

/*-------------------------------------------------------------------*//*!
 * \external
 * \brief   Get current display surface
 *
 * \param   a_display is the C2D_DISPLAY
 * \param   a_c2dSurface is the pointer to the C2D_SURFACE where surface id is stored
 * \return  C2D_STATUS
 *//*-------------------------------------------------------------------*/
C2D_API C2D_STATUS c2dGetCurrentDisplaySurface(C2D_DISPLAY a_display, C2D_SURFACE *a_c2dSurface);


C2D_API C2D_STATUS c2dWaitIrq(C2D_CONTEXT a_c2dContext, unsigned int *Count, unsigned int timeout);
C2D_API C2D_STATUS c2dLibOpen(void);
C2D_API C2D_STATUS c2dLibClose(void);

C2D_API C2D_STATUS c2dTranslatePhysaddr(void* virtAddr, unsigned int* physAddr);

#ifdef __cplusplus
}
#endif

#endif /* __c2d_api_h_ */
