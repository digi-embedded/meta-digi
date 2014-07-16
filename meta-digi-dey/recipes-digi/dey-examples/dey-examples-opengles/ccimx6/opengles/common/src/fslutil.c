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

#define GL_FUNCS 1


#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <EGL/egl.h>
#include <FSL/fslutil.h>
#include <math.h>

int LoadBMP(char *filename, Image *image)
{
	FILE *file;
	unsigned long size;                 // size of the image in bytes.
	unsigned long i;                    // standard counter.
	unsigned short int planes;          // number of planes in image (must be 1)
	unsigned short int bpp;             // number of bits per pixel (must be 24)
	char temp;                          // temporary color storage for bgr-rgb conversion.

	// make sure the file is there.
	if ((file = fopen(filename, "rb"))==NULL)
	{
		printf("File Not Found : %s\n",filename);
		return 0;
	}

	// seek through the bmp header, up to the width/height:
	fseek(file, 18, SEEK_CUR);

	// read the width
	if ((i = fread(&image->sizeX, 4, 1, file)) != 1)
	{
		printf("Error reading width from %s.\n", filename);
		return 0;
	}


	// read the height
	if ((i = fread(&image->sizeY, 4, 1, file)) != 1)
	{
		printf("Error reading height from %s.\n", filename);
		return 0;
	}

	// calculate the size (assuming 24 bits or 3 bytes per pixel).
	size = image->sizeX * image->sizeY * 3;
	//printf("size is %lu\n",size);

	// read the planes
	if ((fread(&planes, 2, 1, file)) != 1)
	{
		printf("Error reading planes from %s.\n", filename);
		return 0;
	}
	if (planes != 1)
	{
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return 0;
	}

	// read the bpp
	if ((i = fread(&bpp, 2, 1, file)) != 1)
	{
		printf("Error reading bpp from %s.\n", filename);
		return 0;
	}
	if (bpp != 24)
	{
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
	}

	// seek past the rest of the bitmap header.
	fseek(file, 24, SEEK_CUR);

	// read the data.
	image->data = (char *) malloc(size);
	if (image->data == NULL)
	{
		printf("Error allocating memory for color-corrected image data");
		return 0;
	}

	if ((i = fread(image->data, size, 1, file)) != 1)
	{
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}

	for (i=0;i<size;i+=3)
	{ // reverse all of the colors. (bgr -> rgb)
		temp = image->data[i];
		image->data[i] = image->data[i+2];
		image->data[i+2] = temp;
	}


	// we're done.
	return 1;
}


int LoadTGA(const char *textureFileName, Image *image)
{
	FILE *f = fopen(textureFileName, "rb");
#ifdef UNDER_CE
	if (f == NULL)
	{
		wchar_t moduleName[MAX_PATH];
		char path[MAX_PATH], * p;
		GetModuleFileName(NULL, moduleName, MAX_PATH);
		wcstombs(path, moduleName, MAX_PATH);
		p = strrchr(path, '\\');
		strcpy(p + 1, textureFileName);
		f = fopen(path, "rb");
	}
#endif
	if(!f) return 0;

	unsigned short width, height;
	unsigned char widthLow, widthHigh, heightLow, heightHigh;
	unsigned char headerLength = 0;
	unsigned char imageType = 0;
	unsigned char bits = 0;
	int format= 0;
	int lineWidth = 0;

	fread(&headerLength, sizeof(unsigned char), 1, f);
	fseek(f,1,SEEK_CUR);
	fread(&imageType, sizeof(unsigned char), 1, f);
	fseek(f, 9, SEEK_CUR);
	fread(&widthLow,  sizeof(unsigned char), 1, f);
	fread(&widthHigh,  sizeof(unsigned char), 1, f);
	width = (widthHigh << 16) + widthLow;
	fread(&heightLow, sizeof(unsigned char), 1, f);
	fread(&heightHigh, sizeof(unsigned char), 1, f);
	height = (heightHigh << 16) + heightLow;
	fread(&bits,   sizeof(unsigned char), 1, f);

	image->sizeX = width;
	image->sizeY = height;

	printf("width=%d height=%d\n", width, height);

	/* Check pixel depth. */
    switch (bits)
    {
    case 16:
        /* 16-bpp RGB. */
        image->Format = GL_UNSIGNED_SHORT_5_6_5;
        break;

    case 24:
        /* 24-bpp RGB. */
        image->Format = GL_RGB;
        break;

    case 32:
        /* 32-bpp RGB. */
        image->Format = GL_RGBA;
        break;

    default:
        /* Invalid pixel depth. */
        return 0;
    }
	fseek(f, headerLength + 1, SEEK_CUR);
	char *buffer = NULL;
	if(imageType != 10)
	{
		int y;
		int i;
		printf("bits=%d\n", bits);
		if((bits == 24)||(bits == 32)) //added to support for LUMINANCE and RGBA textures
		{
			format = bits >> 3;
			lineWidth = format * width;
			buffer = malloc( (bits / 8) * lineWidth * height);

			for(y = 0; y < height; y++)
			{
				GLubyte *line = &buffer[lineWidth * y];
				fread(line, lineWidth, 1, f);

				if(format!= 1)
				{
					for(i=0;i<lineWidth ; i+=format) //swap R and B because TGA are stored in BGR format
					{
						int temp  = line[i];
						line[i]   = line[i+2];
						line[i+2] = temp;
					}
				}
			}
		}
		else
		{
			fclose(f);
			image->data = buffer;
			return 0;
		}
	}
	fclose(f);

	image->data = buffer;

	return 1;
}

//--------------------------------------------------------------------------------------
// Name: fslLoadCTES
// Desc: Helper function to load an compressed image file (ATC, ETC, etc.) from the compressenator
// 		 At exit nFormat contains the GL
//--------------------------------------------------------------------------------------
char* fslLoadCTES( char* strFileName, unsigned int* pWidth, unsigned int* pHeight,
				  unsigned int* nFormat, unsigned int* nSize )
{
	unsigned int nTotalBlocks, nBytesPerBlock, nHasAlpha;
	char* pBits8;

	struct CTES_HEADER
	{
		unsigned int	signature;
		unsigned int	width;
		unsigned int	height;
		unsigned int	flags;
		unsigned int	dataOffset;  // From start of header/file
	} header;

	// Read the file
	FILE* file = fopen( strFileName, "rb" );
	if( NULL == file )
	{
		printf("Error loading file: %s \n",strFileName);
		return NULL;
	}


	if (fread( &header, sizeof(header), 1, file ) != 1)
	{
		printf("Error loading file : %s \n",strFileName);
		fclose( file );
		return NULL;
	}

	nTotalBlocks = ((header.width + 3) >> 2) * ((header.height + 3) >> 2);
	nHasAlpha = header.flags & ATC_RGBA;
	nBytesPerBlock = (nHasAlpha) ? 16 : 8;

	(*nSize)   = nTotalBlocks * nBytesPerBlock;
	(*pWidth)  = header.width;
	(*pHeight) = header.height;

	switch (header.signature)
	{
	case ATC_SIGNATURE:
		if(nHasAlpha && (header.flags & ATC_ALPHA_INTERPOLATED))
			(*nFormat) = GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD;
		else if(nHasAlpha)
			(*nFormat) = GL_ATC_RGBA_EXPLICIT_ALPHA_AMD;
		else
			(*nFormat) = GL_ATC_RGB_AMD;
		break;
	case ATI1N_SIGNATURE:
		(*nFormat) = GL_3DC_X_AMD;
		break;
	case ATI2N_SIGNATURE:
		(*nFormat) = GL_3DC_XY_AMD;
		break;
	case ETC_SIGNATURE:
		if(nHasAlpha)
		{
			printf("Unsupported texture format\n");
			return NULL;
		}
		(*nFormat) = GL_ETC1_RGB8_OES;
		break;

	default:
		printf("Unsupported texture format\n");
		return NULL;
		break;
	}
	pBits8 = (char*)malloc(sizeof(char) * (*nSize));

	// Read the encoded image
	fseek(file, header.dataOffset, SEEK_SET);
	if (fread(pBits8, *nSize, 1, file) != 1) 	{
		printf("Error loading file : %s \n",strFileName);
		fclose( file );
		free( pBits8 );
		return NULL;
	}

	fclose( file );

	return pBits8;
}

fslBool fslUnProject(float winx,float winy, float winz, float modelMatrix[16],  float projMatrix[16],  int viewport[4], float *objx, float *objy, float *objz)
{
	float finalMatrix[16];
	float finalMatrixTmp[16];
	float in[4];
	float out[4];

	fslMultMatrix4x4(finalMatrixTmp,modelMatrix,projMatrix);

	if(!fslInvertMatrix4x4( finalMatrixTmp, finalMatrix))
	{
		printf("error inverting matrix \n");
		return FSL_FALSE;
	}

	in[0]=winx;
	in[1]=winy;
	in[2]=winz;
	in[3]=1.0;
	//map x and y from coordinates
	in[0]=(in[0]-viewport[0])/ viewport[2];
	in[1]=(in[1]-viewport[1])/ viewport[3];

	//map range to -1 to 1
	in[0]= in[0]*2-1;
	in[1]= in[1]*2-1;
	in[2]= in[2]*2-1;
	//printf("in: %f %f %f \n",in[0],in[1],in[2]);
	fslMultMatrix4x4Vec4x1 ( finalMatrix, in, out );


	if(out[3]== 0.0)
		return FSL_FALSE;


	out[0] /=out[3];
	out[1] /=out[3];
	out[2] /=out[3];


	*objx = out[0];
	*objy = out[1];
	*objz = out[2];

	return FSL_TRUE;


}


//--------------------------------------------------------------------------------------
// Name: fslEGLCheck
// Desc: Helper function to print EGL errors and exits application
//--------------------------------------------------------------------------------------
fslBool fslEGLCheck( fslBool bExitOnFailure)
{
	EGLint eglerr = eglGetError();

	if(eglerr != EGL_SUCCESS)
	{
		switch(eglerr){
				case EGL_NOT_INITIALIZED:
					fprintf(stdout, "EGL Fail = EGL_NOT_INITIALIZED (0x%x) \n", eglerr);
					break;
				case EGL_BAD_ACCESS:
					fprintf(stdout, "EGL Fail = EGL_BAD_ACCESS (0x%x) \n", eglerr);
					break;
				case EGL_BAD_ALLOC:
					fprintf(stdout, "EGL Fail = EGL_BAD_ALLOC (0x%x) \n", eglerr);
					break;
				case EGL_BAD_ATTRIBUTE:
					fprintf(stdout, "EGL Fail = EGL_BAD_ATTRIBUTE(0x%x) \n", eglerr);
					break;
				case EGL_BAD_CONFIG:
					fprintf(stdout, "EGL Fail = EGL_BAD_CONFIG (0x%x) \n", eglerr);
					break;
				case EGL_BAD_CONTEXT:
					fprintf(stdout, "EGL Fail = EGL_BAD_CONTEXT (0x%x) \n", eglerr);
					break;
				case EGL_BAD_CURRENT_SURFACE:
					fprintf(stdout, "EGL Fail = EGL_BAD_CURRENT_SURFACE (0x%x) \n", eglerr);
					break;
				case EGL_BAD_DISPLAY:
					fprintf(stdout, "EGL Fail = EGL_BAD_DISPLAY (0x%x) \n", eglerr);
					break;
				case EGL_BAD_MATCH:
					fprintf(stdout, "EGL Fail = EGL_BAD_MATCH (0x%x) \n", eglerr);
					break;
				case EGL_BAD_NATIVE_PIXMAP:
					fprintf(stdout, "EGL Fail = EGL_BAD_NATIVE_PIXMAP (0x%x) \n", eglerr);
					break;
				case EGL_BAD_NATIVE_WINDOW:
					fprintf(stdout, "EGL Fail = EGL_BAD_NATIVE_WINDOW (0x%x) \n", eglerr);
					break;
				case EGL_BAD_PARAMETER:
					fprintf(stdout, "EGL Fail = EGL_BAD_PARAMETER (0x%x) \n", eglerr);
					break;
				case EGL_BAD_SURFACE:
					fprintf(stdout, "EGL Fail = EGL_BAD_SURFACE (0x%x) \n", eglerr);
					break;
				case EGL_CONTEXT_LOST:
					fprintf(stdout, "EGL Fail = EGL_CONTEXT_LOST (0x%x) \n", eglerr);
					break;
				default:
					fprintf(stdout, "EGL Fail = 0x%x \n", eglerr);
		}

		if (bExitOnFailure)
			exit(EXIT_FAILURE);
		else
			return FSL_FALSE;
	}

	return FSL_TRUE;
}

#ifdef GL_FUNCS
fslBool fslInit2DBMPTextureGL(char* strFileName, GLuint *pTextureHandle){

	Image *image1;

	// allocate space for texture we will use
	image1 = (Image *) malloc(sizeof(Image));
	if (image1 == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	if (!LoadBMP(strFileName, image1)) {
		exit(1);
	}

	glGenTextures( 1, pTextureHandle );
	glBindTexture( GL_TEXTURE_2D, *pTextureHandle );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,image1->sizeX,image1->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,image1->data);
	free(image1);
	return FSL_TRUE;

}


fslBool fslInit2DTGATextureGL(char* strFileName, unsigned int *pTextureHandle){

	Image *image1;

	// allocate space for texture we will use
	image1 = (Image *) malloc(sizeof(Image));
	if (image1 == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	if (!LoadTGA(strFileName, image1)) {
		printf("Error loading TGA\n");
		exit(1);
	}

	glGenTextures( 1, pTextureHandle );
	glBindTexture( GL_TEXTURE_2D, *pTextureHandle );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	 /* Set unpack alignment. */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D,0 , image1->Format, image1->sizeX,image1->sizeY,0,image1->Format,GL_UNSIGNED_BYTE,image1->data);
	free(image1->data);
	free(image1);
	return FSL_TRUE;

}

//--------------------------------------------------------------------------------------
// Name: fslInit2DCTESTextureGL
// Desc: Helper function to load a CTES texture file and bind it to a given GL texture handle
//--------------------------------------------------------------------------------------
fslBool fslInit2DCTESTextureGL( char* strFileName, GLuint *pTextureHandle )
{
	unsigned int nWidth, nHeight, nFormat, nSize;
	char* pImageData = fslLoadCTES( strFileName, &nWidth, &nHeight, &nFormat, &nSize );

	if( NULL == pImageData )
	{
		printf("Error loading texture! \n");
		return FSL_FALSE;
	}
	else
	{
		printf("Texture [%s] succesully read (%d x %d , %d) \n",strFileName,nWidth,nHeight,nSize);
	}

	glGenTextures( 1, pTextureHandle );
	glBindTexture( GL_TEXTURE_2D, *pTextureHandle );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glCompressedTexImage2D( GL_TEXTURE_2D, 0, nFormat, nWidth, nHeight, 0, nSize, pImageData );
	free(pImageData);

	return FSL_TRUE;
}
#endif

#ifdef  FSL_EGL_USE_X11
//--------------------------------------------------------------------------------------
// Name: fslLoadFontX
// Desc: Helper function to load a X11 font into given struct
//--------------------------------------------------------------------------------------
void fslLoadFontX(Display *display, XFontStruct **font_info)
{
	char *fontname = "9x15";

	if ((*font_info=XLoadQueryFont(display,fontname)) == NULL)
	{
		printf("stderr, basicwin: cannot open 9x15 font\n");
		exit(EXIT_FAILURE);
	}
}

//--------------------------------------------------------------------------------------
// Name: fslErrorHandlerX
// Desc: Helper function to print incoming X11 server errors
//--------------------------------------------------------------------------------------
int fslErrorHandlerX( Display *display, XErrorEvent *error )
{
	char errorText[1024];
	XGetErrorText( display, error->error_code, errorText, sizeof(errorText) );
	printf( "\t --- X Error: %s ---\n", errorText );
	return 0;
}
#endif

//--------------------------------------------------------------------------------------
// Name: fslGetTickCount
// Desc: Helper function to get current time
//--------------------------------------------------------------------------------------
unsigned int fslGetTickCount()
{
	struct timeval tv;
	if(gettimeofday(&tv, NULL) != 0)
	{
		return 0;
	}

	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

//--------------------------------------------------------------------------------------
// Name: fslMulMatrix4x4
// Desc: 4x4 Matix Muliply
//--------------------------------------------------------------------------------------
void fslMultMatrix4x4( float *matC, float *matA, float *matB)
{
	matC[ 0] = matA[ 0] * matB[ 0] + matA[ 1] * matB[ 4] + matA[ 2] * matB[ 8] + matA[ 3] * matB[12];
	matC[ 1] = matA[ 0] * matB[ 1] + matA[ 1] * matB[ 5] + matA[ 2] * matB[ 9] + matA[ 3] * matB[13];
	matC[ 2] = matA[ 0] * matB[ 2] + matA[ 1] * matB[ 6] + matA[ 2] * matB[10] + matA[ 3] * matB[14];
	matC[ 3] = matA[ 0] * matB[ 3] + matA[ 1] * matB[ 7] + matA[ 2] * matB[11] + matA[ 3] * matB[15];
	matC[ 4] = matA[ 4] * matB[ 0] + matA[ 5] * matB[ 4] + matA[ 6] * matB[ 8] + matA[ 7] * matB[12];
	matC[ 5] = matA[ 4] * matB[ 1] + matA[ 5] * matB[ 5] + matA[ 6] * matB[ 9] + matA[ 7] * matB[13];
	matC[ 6] = matA[ 4] * matB[ 2] + matA[ 5] * matB[ 6] + matA[ 6] * matB[10] + matA[ 7] * matB[14];
	matC[ 7] = matA[ 4] * matB[ 3] + matA[ 5] * matB[ 7] + matA[ 6] * matB[11] + matA[ 7] * matB[15];
	matC[ 8] = matA[ 8] * matB[ 0] + matA[ 9] * matB[ 4] + matA[10] * matB[ 8] + matA[11] * matB[12];
	matC[ 9] = matA[ 8] * matB[ 1] + matA[ 9] * matB[ 5] + matA[10] * matB[ 9] + matA[11] * matB[13];
	matC[10] = matA[ 8] * matB[ 2] + matA[ 9] * matB[ 6] + matA[10] * matB[10] + matA[11] * matB[14];
	matC[11] = matA[ 8] * matB[ 3] + matA[ 9] * matB[ 7] + matA[10] * matB[11] + matA[11] * matB[15];
	matC[12] = matA[12] * matB[ 0] + matA[13] * matB[ 4] + matA[14] * matB[ 8] + matA[15] * matB[12];
	matC[13] = matA[12] * matB[ 1] + matA[13] * matB[ 5] + matA[14] * matB[ 9] + matA[15] * matB[13];
	matC[14] = matA[12] * matB[ 2] + matA[13] * matB[ 6] + matA[14] * matB[10] + matA[15] * matB[14];
	matC[15] = matA[12] * matB[ 3] + matA[13] * matB[ 7] + matA[14] * matB[11] + matA[15] * matB[15];
}

fslBool fslInvertMatrix4x4( float *src, float *inverse)
{
	int i, j, k, swap;
	double t;
	double temp[4][4];

	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			temp[i][j] = src[i*4+j];
		}
	}

	inverse[ 0] = 1.0f;
	inverse[ 1] = 0.0f;
	inverse[ 2] = 0.0f;
	inverse[ 3] = 0.0f;
	inverse[ 4] = 0.0f;
	inverse[ 5] = 1.0f;
	inverse[ 6] = 0.0f;
	inverse[ 7] = 0.0f;
	inverse[ 8] = 0.0f;
	inverse[ 9] = 0.0f;
	inverse[10] = 1.0f;
	inverse[11] = 0.0f;
	inverse[12] = 0.0f;
	inverse[13] = 0.0f;
	inverse[14] = 0.0f;
	inverse[15] = 1.0f;


	for (i = 0; i < 4; i++) {
		/*
		** Look for largest element in column
		*/
		swap = i;
		for (j = i + 1; j < 4; j++) {
			if (fabs(temp[j][i]) > fabs(temp[i][i])) {
				swap = j;
			}
		}

		if (swap != i) {
			/*
			** Swap rows.
			*/
			for (k = 0; k < 4; k++) {
				t = temp[i][k];
				temp[i][k] = temp[swap][k];
				temp[swap][k] = t;

				t = inverse[i*4+k];
				inverse[i*4+k] = inverse[swap*4+k];
				inverse[swap*4+k] = t;
			}
		}

		if (temp[i][i] == 0) {
			/*
			** No non-zero pivot.  The matrix is singular, which shouldn't
			** happen.  This means the user gave us a bad matrix.
			*/
			return FSL_FALSE;
		}

		t = temp[i][i];
		for (k = 0; k < 4; k++) {
			temp[i][k] /= t;
			inverse[i*4+k] /= t;
		}
		for (j = 0; j < 4; j++) {
			if (j != i) {
				t = temp[j][i];
				for (k = 0; k < 4; k++) {
					temp[j][k] -= temp[i][k]*t;
					inverse[j*4+k] -= inverse[i*4+k]*t;
				}
			}
		}
	}
	return FSL_TRUE;
}

void fslPerspectiveMatrix4x4 ( float *m, float fov, float aspect, float zNear, float zFar)
{
	const float h = 1.0f/tan(fov*PI_OVER_360);
	float neg_depth = zNear-zFar;

	m[0] = h / aspect;
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;

	m[4] = 0;
	m[5] = h;
	m[6] = 0;
	m[7] = 0;

	m[8] = 0;
	m[9] = 0;
	m[10] = (zFar + zNear)/neg_depth;
	m[11] = -1;

	m[12] = 0;
	m[13] = 0;
	m[14] = 2.0f*(zNear*zFar)/neg_depth;
	m[15] = 0;

}

void fslMultMatrix4x4Vec4x1 ( float *matA, float *vecA, float *vecB )
{
	int i;

	for ( i = 0; i < 4; i++ )
	{
		vecB[i] = vecA[0] * matA[0*4+i] + vecA[1] * matA[1*4+i] + vecA[2] * matA[2*4+i] + vecA[3] * matA[3*4+i];
	}
}

void fslRotateMatrix4x4 (float *m, float angle, fslAxis axis)
{
	float radians = PI_OVER_360*2*angle;
	float rotate[16] = {0};

	fslLoadIdentityMatrix4x4(rotate);

	switch (axis)
	{
	case FSL_X_AXIS:
		rotate[5] = cos(radians);
		rotate[6] = sin(radians);
		rotate[9] = -sin(radians);
		rotate[10] = cos(radians);
		fslMultMatrix4x4(m, rotate, m);
		break;
	case FSL_Y_AXIS:
		rotate[0] = cos(radians);
		rotate[2] = -sin(radians);
		rotate[8] = sin(radians);
		rotate[10] = cos(radians);
		fslMultMatrix4x4(m, rotate, m);
		break;
	case FSL_Z_AXIS:
		rotate[0] = cos(radians);
		rotate[1] = sin(radians);
		rotate[4] = -sin(radians);
		rotate[5] = cos(radians);
		fslMultMatrix4x4(m, rotate, m);
		break;
	default:
		printf("invalid axis \n");
		break;

	}


}

void fslTranslateMatrix4x4 (float *m, float transX, float transY, float transZ)
{
	float trans[16] = {0};
	fslLoadIdentityMatrix4x4(trans);

	trans[12]=transX;
	trans[13]=transY;
	trans[14]=transZ;

	fslMultMatrix4x4(m, trans, m);
}

void fslScaleMatrix4x4 (float *m, float scaleX, float scaleY, float scaleZ)
{
	float scale[16] = {0};
	fslLoadIdentityMatrix4x4(scale);

	scale[0]=scaleX;
	scale[5]=scaleY;
	scale[10]=scaleZ;

	fslMultMatrix4x4(m, scale, m);
}

void fslLoadIdentityMatrix4x4 (float *m)
{
	m[0] = 1;
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;

	m[4] = 0;
	m[5] = 1;
	m[6] = 0;
	m[7] = 0;

	m[8] = 0;
	m[9] = 0;
	m[10] = 1;
	m[11] = 0;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;
}

float fslInnerProduct( float *v, float *q)
{
	return v[0]*q[0]+v[1]*q[1]+v[2]*q[2];
}
//result = endpoint-startPoint
void fslDirectionVector(float *result, float *endPoint, float *startPoint)
{
	result[0]= endPoint[0]-startPoint[0];
	result[1]= endPoint[1]-startPoint[1];
	result[2]= endPoint[2]-startPoint[2];
}
//a = crossProduct(b,c)
void fslCrossProduct(float *result, float *b, float* c)
{
	result[0]= b[1]*c[2]-c[1]*b[2];
	result[1]= b[2]*c[0]-c[2]*b[0];
	result[2]= b[0]*c[1]-c[0]*b[1];
}
//p is the startpoint, d is the direction vector, v0,v1,v2 represent the triangle
int fslRayIntersectsTriangle(float *p, float *d,float *v0, float *v1, float *v2)
{
	float e1[3]={0.0, 0.0, 0.0};
	float e2[3]={0.0, 0.0, 0.0};
	float h[3] = {0.0, 0.0, 0.0};
	float s[3] = {0.0, 0.0, 0.0};
	float q[3] = {0.0, 0.0, 0.0};
	float a,f,u,v,t;
	fslDirectionVector(e1,v1,v0);
	fslDirectionVector(e2,v2,v0);
	fslCrossProduct(h,d,e2);
	//get the inner product
	a=fslInnerProduct(e1,h);
	if(a>-0.00001&& a<0.00001)
		return 0;
	f= 1/a;
	fslDirectionVector(s,p,v0);
	u=f*(fslInnerProduct(s,h));

	if(u<0.0 || u>1.0)
		return 0;

	fslCrossProduct(q,s,e1);
	v = f*fslInnerProduct(d,q);
	if(v< 0.0 || u+v > 1.0)
		return 0;
	//we can compute t to find out where the intersection point is on the line
	t= f*fslInnerProduct(e2,q);
	if(t > 0.00001) //ray intersection
		return 1;
	else
		return 0;


}
void fslNormalize(float *v)
{
	float mag =v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
	sqrt(mag);
	v[0]=v[0]/mag;
	v[1]=v[1]/mag;
	v[2]=v[2]/mag;
	//printf("vector Normalized is %f,%f,%f\n",v[0],v[1],v[2]);
}
void fslPrintMatrix4x4(float *m){

	printf(" %f %f %f %f \n", m[0], m[1], m[2], m[3]);
	printf(" %f %f %f %f \n", m[4], m[5], m[6], m[7]);
	printf(" %f %f %f %f \n", m[8], m[9], m[10], m[11]);
	printf(" %f %f %f %f \n", m[12], m[13], m[14], m[15]);

}
void fslCalculateNormals(float *triangleArray, int size, float *normalArray)
{
	printf("enter fslCalculateNormals \n");


	int index =0;

	int i=0;
	for(i=0; i< size/3; i++){

		float v1[3]={0.0f};
		float v2[3]={0.0f};
		float v3[3]={0.0f};
		//float v4[3]={0.0f};

		float A[3]={0.0f};
		float B[3]={0.0f};
		/*float C[3]={0.0f};
		float D[3]={0.0f};*/

		v1[0]=triangleArray[9*i];//9 is for 9 values (3 vertices) used per loop
		v1[1]=triangleArray[9*i+1];
		v1[2]=triangleArray[12*i+2];

		v2[0]=triangleArray[9*i+3];
		v2[1]=triangleArray[9*i+4];
		v2[2]=triangleArray[9*i+5];

		v3[0]=triangleArray[9*i+6];
		v3[1]=triangleArray[9*i+7];
		v3[2]=triangleArray[9*i+8];


		A[0]=v1[0]-v2[0];
		A[1]=v1[1]-v2[1];
		A[2]=v1[2]-v2[2];

		B[0]=v2[0]-v3[0];
		B[1]=v2[1]-v3[1];
		B[2]=v2[2]-v3[2];
		float result[3]={0.0f};

		printf("adding normal  %i\n",index);
		fslCrossProduct(result, A, B);
		fslNormalize(result);
		normalArray[3*i]=result[0];
		normalArray[3*i+1]=result[1];
		normalArray[3*i+2]=result[2];

		printf("adding normal:  %f %f %f \n",result[0],result[1],result[2]);
		index++;
	}
}



