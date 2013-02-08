/*
 * es11_example.c
 *
 * This code was created by Jeff Molofee '99
 * (ported to Linux by Ti Leggett '01)
 * (ported to i.mx51, i.mx31 and x11 by Freescale '10)
 * (ported to ConnectCore Wi-i.MX51 by Digi International Inc '10)
 *
 * If you've found this code useful, please let him know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 *
 * Description: Rotating textured 3D object (using OpenGL ES 1.1)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <math.h>
#include <GLU3/glu3.h>
#include <FSL/fslutil.h>

#include "GLES/gl.h"
#include "GLES/glext.h"
#include "EGL/egl.h"

EGLDisplay egldisplay;
EGLConfig eglconfig;
EGLSurface eglsurface;
EGLContext eglcontext;
int currentFrame = 0;

GLfloat xrot; /* X Rotation ( NEW ) */
GLfloat yrot; /* Y Rotation ( NEW ) */
GLfloat zrot; /* Z Rotation ( NEW ) */

GLuint texture[1]; /* Storage For One Texture ( NEW ) */

/* function to load in bitmap as a GL texture */
int LoadGLTextures()
{
	Image *image1;

	// allocate space for texture we will use
	image1 = (Image *) malloc(sizeof(Image));
	if (image1 == NULL) {
		printf("Error allocating space for image");
		return 0;
	}

	/* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
	if (ImageLoad("/usr/share/wallpapers/texture.bmp", image1)) {
		/* Create The Texture */
		glGenTextures(1, texture);
		/* Typical Texture Generation Using Data From The Bitmap */
		glBindTexture(GL_TEXTURE_2D, *texture);
		/* Generate The Texture */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image1->sizeX, image1->sizeY, 0, GL_RGB,
			     GL_UNSIGNED_BYTE, image1->data);
		/* Linear Filtering */
		glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		return 0;
	}

	/* Free up any memory we may have used */
	if (image1 != NULL) {
		free(image1);
	}

	return 1;
}

/* function to release/destroy our resources and restoring the old desktop */
void Cleanup()
{
}

/* general OpenGL initialization function */
int init(void)
{
	static const EGLint s_configAttribs[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 0,
		EGL_SAMPLES, 0,
		EGL_NONE
	};

	EGLint numconfigs;

	//get egl display
	egldisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	//Initialize egl
	eglInitialize(egldisplay, NULL, NULL);
	assert(eglGetError() == EGL_SUCCESS);
	//tell the driver we are using OpenGL ES
	eglBindAPI(EGL_OPENGL_ES_API);

	//pass our egl configuration to egl
	eglChooseConfig(egldisplay, s_configAttribs, &eglconfig, 1, &numconfigs);
	printf("chooseconfig, \n");
	assert(eglGetError() == EGL_SUCCESS);
	assert(numconfigs == 1);

	//You must pass in the file system handle to the linux framebuffer when creating a window
	eglsurface =
	    eglCreateWindowSurface(egldisplay, eglconfig, open("/dev/fb0", O_RDWR), NULL);
	assert(eglGetError() == EGL_SUCCESS);

	//create the egl graphics context
	eglcontext = eglCreateContext(egldisplay, eglconfig, NULL, NULL);
	printf("creatcontext, \n");
	assert(eglGetError() == EGL_SUCCESS);

	//make the context current
	eglMakeCurrent(egldisplay, eglsurface, eglsurface, eglcontext);
	printf("makecurrent, \n");
	assert(eglGetError() == EGL_SUCCESS);

	/* Load in the texture */
	if (LoadGLTextures() == 0) {
		return 0;
	}

	/* Enable Texture Mapping ( NEW ) */
	glEnable(GL_TEXTURE_2D);

	/* Enable smooth shading */
	glShadeModel(GL_SMOOTH);

	/* Set the background black */
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);

	/* Depth buffer setup */
	glClearDepthf(1.0f);

	/* Enables Depth Testing */
	glEnable(GL_DEPTH_TEST);

	/* The Type Of Depth Test To Do */
	glDepthFunc(GL_LEQUAL);

	/*enable cullface */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/* Really Nice Perspective Calculations */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	/*get width and height from egl */
	EGLint h, w;
	eglQuerySurface(egldisplay, eglsurface, EGL_WIDTH, &w);
	eglQuerySurface(egldisplay, eglsurface, EGL_HEIGHT, &h);

	/*change to projection matrix */
	glMatrixMode(GL_PROJECTION);
	/*reset the projection matrix */
	glLoadIdentity();
	/*set the viewport */
	glViewport(0, 0, w, h);

	GLUmat4 perspective;
	/*use glu to set perspective */
	gluPerspective4f(&perspective, 45.0f, ((GLfloat) w / (GLfloat) h), 1.0f, 100.0f);
	glMultMatrixf(&perspective.col[0].values[0]);

	/*get back to model view matrix */
	glMatrixMode(GL_MODELVIEW);
	/*reset modevl view matrix */
	glLoadIdentity();

	return 1;
}

/* Here goes our drawing code */
void render()
{
	/* These are to calculate our fps */
	GLfloat texcoords[4][2];
	GLfloat vertices[4][3];
	GLubyte indices[4] = { 0, 1, 3, 2 };	/* QUAD to TRIANGLE_STRIP conversion; */

	/* Clear The Screen And The Depth Buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Move Into The Screen 5 Units */
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);	/* Rotate On The X Axis */
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);	/* Rotate On The Y Axis */
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);	/* Rotate On The Z Axis */

	/* Select Our Texture */
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	/* Set pointers to vertices and texcoords */
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

	/* Enable vertices and texcoords arrays */
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	/* Front Face */
	texcoords[0][0] = 0.0f;
	texcoords[0][1] = 0.0f;
	vertices[0][0] = -1.0f;
	vertices[0][1] = -1.0f;
	vertices[0][2] = 1.0f;
	texcoords[1][0] = 1.0f;
	texcoords[1][1] = 0.0f;
	vertices[1][0] = 1.0f;
	vertices[1][1] = -1.0f;
	vertices[1][2] = 1.0f;
	texcoords[2][0] = 1.0f;
	texcoords[2][1] = 1.0f;
	vertices[2][0] = 1.0f;
	vertices[2][1] = 1.0f;
	vertices[2][2] = 1.0f;
	texcoords[3][0] = 0.0f;
	texcoords[3][1] = 1.0f;
	vertices[3][0] = -1.0f;
	vertices[3][1] = 1.0f;
	vertices[3][2] = 1.0f;

	/* Draw one textured plane using two stripped triangles */
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);

	/* Back Face */
	/* Normal Pointing Away From Viewer */
	texcoords[0][0] = 0.0f;
	texcoords[0][1] = 1.0f;
	vertices[0][0] = -1.0f;
	vertices[0][1] = -1.0f;
	vertices[0][2] = -1.0f;
	texcoords[1][0] = 0.0f;
	texcoords[1][1] = 0.0f;
	vertices[1][0] = -1.0f;
	vertices[1][1] = 1.0f;
	vertices[1][2] = -1.0f;
	texcoords[2][0] = 1.0f;
	texcoords[2][1] = 0.0f;
	vertices[2][0] = 1.0f;
	vertices[2][1] = 1.0f;
	vertices[2][2] = -1.0f;
	texcoords[3][0] = 1.0f;
	texcoords[3][1] = 1.0f;
	vertices[3][0] = 1.0f;
	vertices[3][1] = -1.0f;
	vertices[3][2] = -1.0f;

	/* Draw one textured plane using two stripped triangles */
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);

	/* Top Face */
	texcoords[0][0] = 1.0f;
	texcoords[0][1] = 0.0f;
	vertices[0][0] = -1.0f;
	vertices[0][1] = 1.0f;
	vertices[0][2] = -1.0f;
	texcoords[1][0] = 1.0f;
	texcoords[1][1] = 1.0f;
	vertices[1][0] = -1.0f;
	vertices[1][1] = 1.0f;
	vertices[1][2] = 1.0f;
	texcoords[2][0] = 0.0f;
	texcoords[2][1] = 1.0f;
	vertices[2][0] = 1.0f;
	vertices[2][1] = 1.0f;
	vertices[2][2] = 1.0f;
	texcoords[3][0] = 0.0f;
	texcoords[3][1] = 0.0f;
	vertices[3][0] = 1.0f;
	vertices[3][1] = 1.0f;
	vertices[3][2] = -1.0f;

	/* Draw one textured plane using two stripped triangles */
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);

	/* Bottom Face */
	texcoords[0][0] = 1.0f;
	texcoords[0][1] = 1.0f;
	vertices[0][0] = -1.0f;
	vertices[0][1] = -1.0f;
	vertices[0][2] = -1.0f;
	texcoords[1][0] = 0.0f;
	texcoords[1][1] = 1.0f;
	vertices[1][0] = 1.0f;
	vertices[1][1] = -1.0f;
	vertices[1][2] = -1.0f;
	texcoords[2][0] = 0.0f;
	texcoords[2][1] = 0.0f;
	vertices[2][0] = 1.0f;
	vertices[2][1] = -1.0f;
	vertices[2][2] = 1.0f;
	texcoords[3][0] = 1.0f;
	texcoords[3][1] = 0.0f;
	vertices[3][0] = -1.0f;
	vertices[3][1] = -1.0f;
	vertices[3][2] = 1.0f;

	/* Draw one textured plane using two stripped triangles */
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);

	/* Right face */
	texcoords[0][0] = 0.0f;
	texcoords[0][1] = 1.0f;
	vertices[0][0] = 1.0f;
	vertices[0][1] = -1.0f;
	vertices[0][2] = -1.0f;
	texcoords[1][0] = 0.0f;
	texcoords[1][1] = 0.0f;
	vertices[1][0] = 1.0f;
	vertices[1][1] = 1.0f;
	vertices[1][2] = -1.0f;
	texcoords[2][0] = 1.0f;
	texcoords[2][1] = 0.0f;
	vertices[2][0] = 1.0f;
	vertices[2][1] = 1.0f;
	vertices[2][2] = 1.0f;
	texcoords[3][0] = 1.0f;
	texcoords[3][1] = 1.0f;
	vertices[3][0] = 1.0f;
	vertices[3][1] = -1.0f;
	vertices[3][2] = 1.0f;

	/* Draw one textured plane using two stripped triangles */
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);

	/* Left Face */
	texcoords[0][0] = 0.0f;
	texcoords[0][1] = 0.0f;
	vertices[0][0] = -1.0f;
	vertices[0][1] = -1.0f;
	vertices[0][2] = -1.0f;
	texcoords[1][0] = 1.0f;
	texcoords[1][1] = 0.0f;
	vertices[1][0] = -1.0f;
	vertices[1][1] = -1.0f;
	vertices[1][2] = 1.0f;
	texcoords[2][0] = 1.0f;
	texcoords[2][1] = 1.0f;
	vertices[2][0] = -1.0f;
	vertices[2][1] = 1.0f;
	vertices[2][2] = 1.0f;
	texcoords[3][0] = 0.0f;
	texcoords[3][1] = 1.0f;
	vertices[3][0] = -1.0f;
	vertices[3][1] = 1.0f;
	vertices[3][2] = -1.0f;

	/* Draw one textured plane using two stripped triangles */
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);

	/* Disable texcoords and vertices arrays */
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	/* Flush all drawings */
	glFinish();

	xrot += 0.3f;		/* X Axis Rotation */
	yrot += 0.2f;		/* Y Axis Rotation */
	zrot += 0.4f;		/* Z Axis Rotation */
}

void resize(int w, int h)
{
	/*change to projection matrix */
	glMatrixMode(GL_PROJECTION);
	/*reset the projection matrix */
	glLoadIdentity();
	/*set the viewport */
	glViewport(0, 0, w, h);

	GLUmat4 perspective;
	/*use glu to set perspective */
	gluPerspective4f(&perspective, 45.0f, ((GLfloat) w / (GLfloat) h), 1.0f, 100.0f);
	glMultMatrixf(&perspective.col[0].values[0]);

	/*get back to model view matrix */
	glMatrixMode(GL_MODELVIEW);
	/*reset modevl view matrix */
	glLoadIdentity();
}

void deinit(void)
{
	//call clean up to release any memory allocated
	Cleanup();
	//Make a empty surface current
	eglMakeCurrent(egldisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	assert(eglGetError() == EGL_SUCCESS);
	//destroy egl desplay
	eglTerminate(egldisplay);
	assert(eglGetError() == EGL_SUCCESS);
	//end egl thread
	eglReleaseThread();
}

int main(int argc, char **argv)
{
	assert(init());

	while (currentFrame < 1000) {
		EGLint width = 0;
		EGLint height = 0;
		eglQuerySurface(egldisplay, eglsurface, EGL_WIDTH, &width);
		eglQuerySurface(egldisplay, eglsurface, EGL_HEIGHT, &height);
		render();
		currentFrame++;
		eglSwapBuffers(egldisplay, eglsurface);
	}

	deinit();
	return 1;
}
