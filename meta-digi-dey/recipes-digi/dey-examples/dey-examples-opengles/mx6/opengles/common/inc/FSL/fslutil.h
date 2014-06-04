/****************************************************************************
* Copyright (c) 2012 Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*    * Redistributions of source code must retain the above copyright notice,
*		this list of conditions and the following disclaimer.
*
*    * Redistributions in binary form must reproduce the above copyright notice,
*		this list of conditions and the following disclaimer in the documentation
*		and/or other materials provided with the distribution.
*
* 	 * Neither the name of the Freescale Semiconductor, Inc. nor the names of
*		its contributors may be used to endorse or promote products derived from
*		this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Labels parameters

*****************************************************************************/
#ifndef _FSLUTIL_H_
#define _FSLUTIL_H_
#ifdef __cplusplus
extern "C" {
#endif

// Values for reading in ATC compressed texture files
#define GL_ATC_RGB_AMD       	0x8C92
#define GL_ATC_RGBA_AMD      	0x8C93
#define ATC_SIGNATURE       	0xCCC40002
#define ATI1N_SIGNATURE		0x31495441
#define ATI2N_SIGNATURE		0x32495441
#define ETC_SIGNATURE       	0xEC000001
#define ATC_RGB              	0x00000001
#define ATC_RGBA             	0x00000002
#define ATC_TILED     		0X00000004
#define ATC_ALPHA_INTERPOLATED	0X00000010

#define PI_OVER_360		0.00872664f

#ifndef fslBool
#define fslBool int
#define FSL_FALSE 0
#define FSL_TRUE !FSL_FALSE
#endif

#define FSL_POINTER_MAX 10000


/* Image type - contains height, width, and data */
typedef struct Image_s {
        unsigned long sizeX;
        unsigned long sizeY;
        char *data;
		int Format;
} Image;


typedef enum fslStatus_e
{
	FSL_STATUS_DEVICE_ERROR			= -5,  //could not open a hardware device driver
	FSL_STATUS_NO_CONTEXT			= -4,  //something has caused a device to fail and loose context
	FSL_STATUS_BAD_PARAMETER		= -3,
	FSL_STATUS_ALLOCATION_ISSUE		= -2,
	FSL_STATUS_UNSUPPORTED_FEATURE		= -1,
	FSL_STATUS_GENERAL_ERROR 		= 0,
	FSL_STATUS_SUCCESS			= 1,
	FSL_FSLSTATUS_END	 		= 0xFFFFFFFF, //This forces the data type for binary lib compatipility
} fslStatus;

typedef enum fslAxis_e
{
	FSL_X_AXIS, FSL_Y_AXIS, FSL_Z_AXIS
} fslAxis;

typedef enum fslInputType_e
{
	FSL_INPUT_KEYPRESS		=1000,
	FSL_INPUT_KEYRELEASE		=1500,
	FSL_INPUT_POINTERDOWN		=2000,
	FSL_INPUT_POINTERUP		=3000,
	FSL_INPUT_POINTERMOVE		=4000,
	FSL_INPUT_STOP			=5000,
	FSL_INPUT_PLAY			=5001,
	FSL_INPUT_PAUSE			=5002,
	FSL_INPUT_REW			=5003,
	FSL_INPUT_FFWD			=5004,
	FSL_INPUT_SEEK			=5005,
	FSL_INPUT_CLOSEWINDOW		=6000,
	FSL_INPUTTYPE_END		=0xFFFFFFFF,
} fslInputType;

typedef struct fslInputEventType_s
{
	fslInputType input;
	unsigned int signalA;  //X, keystroke 1  Coordinates range from 0 to FSL_POINTER_MAX; 0,0 lower left corner
	unsigned int signalB;  //Y, keystroke 2
	unsigned int signalC;  //undefined, could be pointer enumerator for multi-touch / secondary input
	long time;	  //in microseconds
} fslInputEventType;

typedef enum fslInputDevice_e
{
	FSL_INPUTDEVICE_TOUCHSCREEN			=1000,
	FSL_INPUTDEVICETYPE_END				=0xFFFFFFFF,
} fslInputDevice;

typedef void* fslDeviceHandle;

#define FSL_UTIL_MAX_FILE_NAME_LENGTH  1024 // This is arbitrary
#define FSL_INPUT_MAX_EVENTS 64

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <linux/input.h>

#ifdef FSL_EGL_USE_X11
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/render.h>
#include <X11/extensions/Xrender.h>
#endif


//--------------------------------------------------------------------------------------
// Name: fslLoadCTES
// Desc: Helper function to load an compressed image file (ATC, ETC, etc.) from the compressenator
//--------------------------------------------------------------------------------------
char* fslLoadCTES( char* strFileName, unsigned int* pWidth, unsigned int* pHeight, unsigned int* nFormat, unsigned int* nSize );

//fslLoadBMP
fslBool fslInit2DBMPTextureGL(char *,unsigned int *pTextureHandle);

//fslLoadTGA
fslBool fslInit2DTGATextureGL(char* strFileName, unsigned int *pTextureHandle);

//--------------------------------------------------------------------------------------
// Name: fslEGLCheck
// Desc: Helper function to print EGL errors and exits application
//--------------------------------------------------------------------------------------
fslBool fslEGLCheck( fslBool bExitOnFailure );

//--------------------------------------------------------------------------------------
// Name: fslInit2DCTESTextureGL
// Desc: Helper function to load a CTES texture file and bind it to a given GL texture handle
//--------------------------------------------------------------------------------------
fslBool fslInit2DCTESTextureGL( char* strFileName, unsigned int *pTextureHandle );

#ifdef FSL_EGL_USE_X11
//--------------------------------------------------------------------------------------
// Name: fslLoadFontX
// Desc: Helper function to load a X11 font into given struct
//--------------------------------------------------------------------------------------
void fslLoadFontX( Display *display, XFontStruct **font_info );

//--------------------------------------------------------------------------------------
// Name: fslErrorHandlerX
// Desc: Helper function to print incoming X11 server errors
//--------------------------------------------------------------------------------------
int fslErrorHandlerX( Display *display, XErrorEvent *error );
#endif

//--------------------------------------------------------------------------------------
// Name: fslGetTickCount
// Desc: Helper function to get current time
//--------------------------------------------------------------------------------------
unsigned int fslGetTickCount();

//--------------------------------------------------------------------------------------
// Name: fslMulMatrix4x4
// Desc: 4x4 Matix Muliply DEPRECATED for GLU
//--------------------------------------------------------------------------------------
void fslMultMatrix4x4( float *matC, float *matA, float *matB);

fslBool fslInvertMatrix4x4( float *matA, float *matC);

void fslPerspectiveMatrix4x4 ( float *m, float fov, float aspect, float zNear, float zFar);

void fslMultMatrix4x4Vec4x1 ( float *matA, float *vecA, float *vecB );

void fslRotateMatrix4x4 (float *m, float angle, fslAxis axis);

void fslTranslateMatrix4x4 (float *m, float transX, float transY, float transZ);

void fslScaleMatrix4x4 (float *m, float scaleX, float scaleY, float scaleZ);
void fslNormalize(float *v);
void fslLoadIdentityMatrix4x4 (float *m);
void fslPrintMatrix4x4(float *m);
fslBool fslUnProject(float winx,float winy, float winz,  float modelMatrix[16],  float projMatrix[16],  int viewport[4], float *objx, float *objy, float *objz);
void fslCrossProduct(float *result, float *b, float* c);
void fslDirectionVector(float *result, float *endPoint, float *startPoint);
float fslInnerProduct( float *v, float *q);
int fslRayIntersectsTriangle(float *p, float *d,float *v0, float *v1, float *v2);
int LoadBMP(char *filename, Image *image);
int LoadTGA(const char *textureFileName, Image *image);
void fslCalculateNormals(float *triArray, int size, float *normArray);

#ifdef __cplusplus
}
#endif
#endif //_FSLUTIL_H_

