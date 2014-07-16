/*
 * This code was created by Jeff Molofee '99
 * (ported to Linux by Ti Leggett '01)
 * (ported to i.mx51, i.mx31 and x11 by Freescale '10)
 * If you've found this code useful, please let him know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <math.h>
#include <signal.h>

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"

#include "EGL/egl.h"

#include "FSL/fsl_egl.h"
#include "FSL/fslutil.h"


#define TRUE 1
#define FALSE !TRUE
EGLDisplay			egldisplay;
EGLConfig			eglconfig;
EGLSurface			eglsurface;
EGLContext			eglcontext;
EGLNativeWindowType eglNativeWindow;
EGLNativeDisplayType eglNativeDisplayType;
volatile sig_atomic_t quit = 0;

GLuint       g_hShaderProgram      = 0;
GLuint       g_hModelViewMatrixLoc = 0;
GLuint       g_hProjMatrixLoc      = 0;
GLuint       g_hVertexLoc          = 0;
GLuint       g_hVertexTexLoc       = 2;
GLuint       g_hColorLoc           = 1;


//--------------------------------------------------------------------------------------
// Name: g_strVertexShader / g_strFragmentShader
// Desc: The vertex and fragment shader programs
//--------------------------------------------------------------------------------------
const char* g_strVertexShader =
"uniform   mat4 g_matModelView;				\n"
"uniform   mat4 g_matProj;					\n"
"								\n"
"attribute vec4 g_vPosition;				\n"
"attribute vec3 g_vColor;					\n"
"attribute vec2 g_vTexCoord;				\n"
"								\n"
"varying   vec3 g_vVSColor;					\n"
"varying   vec2 g_vVSTexCoord;				\n"
"								\n"
"void main()						\n"
"{								\n"
"    vec4 vPositionES = g_matModelView * g_vPosition;	\n"
"    gl_Position  = g_matProj * vPositionES;		\n"
"    g_vVSColor = g_vColor;					\n"
"    g_vVSTexCoord = g_vTexCoord;				\n"
"}								\n";


const char* g_strFragmentShader =
"#ifdef GL_FRAGMENT_PRECISION_HIGH				\n"
"   precision highp float;					\n"
"#else							\n"
"   precision mediump float;				\n"
"#endif							\n"
"								\n"
"uniform sampler2D s_texture;				\n"
"varying   vec3      g_vVSColor;				\n"
"varying   vec2 g_vVSTexCoord;				\n"
"								\n"
"void main()						\n"
"{								\n"
"    gl_FragColor = texture2D(s_texture,g_vVSTexCoord);	\n"
"}								\n";

float VertexPositions[] = {
	/* Draw A Quad */
	/* Top Right Of The Quad (Top) */
	1.0f, 1.0f, -1.0f,
	/* Top Left Of The Quad (Top) */
	-1.0f, 1.0f, -1.0f,
	/* Bottom Right Of The Quad (Top) */
	1.0f, 1.0f, 1.0f,
	/* Bottom Left Of The Quad (Top) */
	-1.0f, 1.0f, 1.0f,
	/* Top Right Of The Quad (Bottom) */
	1.0f, -1.0f, 1.0f,
	/* Top Left Of The Quad (Bottom) */
	-1.0f, -1.0f, 1.0f,
	/* Bottom Right Of The Quad (Bottom) */
	1.0f, -1.0f, -1.0f,
	/* Bottom Left Of The Quad (Bottom) */
	-1.0f, -1.0f, -1.0f,
	/* Top Right Of The Quad (Front) */
	1.0f, 1.0f, 1.0f,
	/* Top Left Of The Quad (Front) */
	-1.0f, 1.0f, 1.0f,
	/* Bottom Right Of The Quad (Front) */
	1.0f, -1.0f, 1.0f,
	/* Bottom Left Of The Quad (Front) */
	-1.0f, -1.0f, 1.0f,
	/* Top Right Of The Quad (Back) */
	1.0f, -1.0f, -1.0f,
	/* Top Left Of The Quad (Back) */
	-1.0f, -1.0f, -1.0f,
	/* Bottom Right Of The Quad (Back) */
	1.0f, 1.0f, -1.0f,
	/* Bottom Left Of The Quad (Back) */
	-1.0f, 1.0f, -1.0f,
	/* Top Right Of The Quad (Left) */
	-1.0f, 1.0f, 1.0f,
	/* Top Left Of The Quad (Left) */
	-1.0f, 1.0f, -1.0f,
	/* Bottom Right Of The Quad (Left) */
	-1.0f, -1.0f, 1.0f,
	/* Bottom Left Of The Quad (Left) */
	-1.0f, -1.0f, -1.0f,
	/* Top Right Of The Quad (Right) */
	1.0f, 1.0f, -1.0f,
	/* Top Left Of The Quad (Right) */
	1.0f, 1.0f, 1.0f,
	/* Bottom Right Of The Quad (Right) */
	1.0f, -1.0f, -1.0f,
	/* Bottom Left Of The Quad (Right) */
	1.0f, -1.0f, 1.0f
};

float VertexTexCoords[] = {
	/* Top Face */
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	/* Bottom Face */
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	/* Front Face */
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	/* Back Face */
	1.0f, 1.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	/*left face */
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	/* Right face */
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
};

float VertexColors[] = {
	/* Red */
	1.0f, 0.0f, 0.0f, 1.0f,
	/* Red */
	1.0f, 0.0f, 0.0f, 1.0f,
	/* Green */
	0.0f, 1.0f, 0.0f, 1.0f,
	/* Green */
	0.0f, 1.0f, 0.0f, 1.0f,
	/* Blue */
	0.0f, 0.0f, 1.0f, 1.0f,
	/* Blue */
	0.0f, 0.0f, 1.0f, 1.0f,
	/* Red */
	1.0f, 0.0, 0.0f, 1.0f,
	/* Red */
	1.0f, 0.0, 0.0f, 1.0f,
	/* Blue */
	0.0f, 0.0f, 1.0f, 1.0f,
	/* Blue */
	0.0f, 0.0f, 1.0f, 1.0f,
	/* Green */
	0.0f, 1.0f, 0.0f, 1.0f,
	/* Green */
	0.0f, 1.0f, 0.0f, 1.0f,
	/* Red */
	1.0f, 0.0f, 0.0f, 1.0f,
	/* Red */
	1.0f, 0.0f, 0.0f, 1.0f,
	/* Green */
	0.0f, 1.0f, 0.0f, 1.0f,
	/* Green */
	0.0f, 1.0f, 0.0f, 1.0f,
	/* Blue */
	0.0f, 0.0f, 1.0f, 1.0f,
	/* Blue */
	0.0f, 0.0f, 1.0f, 1.0f,
	/* Red */
	1.0f, 0.0f, 0.0f, 1.0f,
	/* Red */
	1.0f, 0.0f, 0.0f, 1.0f,
	/* Blue */
	0.0f, 0.0f, 1.0f, 1.0f,
	/* Green */
	0.0f, 1.0f, 0.0f, 1.0f,
	/* Blue */
	0.0f, 0.0f, 1.0f, 1.0f,
	/* Green */
	0.0f, 1.0f, 0.0f, 1.0f
};

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
	if (LoadBMP("texture.bmp", image1)) {
		/* Create The Texture */
		glGenTextures(1, texture);
		/* Typical Texture Generation Using Data From The Bitmap */
		glBindTexture(GL_TEXTURE_2D, *texture);
		/* Generate The Texture */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image1->sizeX, image1->sizeY, 0, GL_RGB,
			     GL_UNSIGNED_BYTE, image1->data);
		/* Linear Filtering */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		printf("texture loaded and created successfully");

	} else
		return 0;

	/* Free up any memory we may have used */
	if (image1 != NULL) {
		free(image1);
	}

	return 1;
}

void render(float w, float h)
{
	static float fAngle = 0.0f;
	fAngle += 0.01f;

	// Rotate and translate the model view matrix
	float matModelView[16] = { 0 };
	matModelView[0] = +cosf(fAngle);
	matModelView[2] = +sinf(fAngle);
	matModelView[5] = 1.0f;
	matModelView[8] = -sinf(fAngle);
	matModelView[10] = +cosf(fAngle);
	matModelView[12] = 0.0f;	//X
	matModelView[14] = -6.0f;	//z
	matModelView[15] = 1.0f;

	// Build a perspective projection matrix
	float matProj[16] = { 0 };
	matProj[0] = cosf(0.5f) / sinf(0.5f);
	matProj[5] = matProj[0] * (w / h);
	matProj[10] = -(10.0f) / (9.0f);
	matProj[11] = -1.0f;
	matProj[14] = -(10.0f) / (9.0f);

	// Clear the colorbuffer and depth-buffer
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set some state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Set the shader program
	glUseProgram(g_hShaderProgram);
	glUniformMatrix4fv(g_hModelViewMatrixLoc, 1, 0, matModelView);
	glUniformMatrix4fv(g_hProjMatrixLoc, 1, 0, matProj);

	// Bind the vertex attributes
	glVertexAttribPointer(g_hVertexLoc, 3, GL_FLOAT, 0, 0, VertexPositions);
	glEnableVertexAttribArray(g_hVertexLoc);

	glVertexAttribPointer(g_hColorLoc, 4, GL_FLOAT, 0, 0, VertexColors);
	glEnableVertexAttribArray(g_hColorLoc);

	glVertexAttribPointer(g_hVertexTexLoc, 2, GL_FLOAT, 0, 0, VertexTexCoords);
	glEnableVertexAttribArray(g_hVertexTexLoc);

	/* Select Our Texture */
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	/* Drawing Using Triangle strips, draw triangle strips using 4 vertices */
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

	// Cleanup
	glDisableVertexAttribArray(g_hVertexLoc);
	glDisableVertexAttribArray(g_hColorLoc);
	glDisableVertexAttribArray(g_hVertexTexLoc);
}


int init(void)
{

	/*static const EGLint gl_context_attribs[] =
	   {
	   EGL_CONTEXT_CLIENT_VERSION, 2,
	   EGL_NONE
	   }; */

	static const EGLint s_configAttribs[] = {
		EGL_RED_SIZE, 5,
		EGL_GREEN_SIZE, 6,
		EGL_BLUE_SIZE, 5,
		EGL_ALPHA_SIZE, 0,
		EGL_SAMPLES, 0,
		EGL_NONE
	};

	EGLint numconfigs;

	eglNativeDisplayType = fsl_getNativeDisplay();
	egldisplay = eglGetDisplay(eglNativeDisplayType);
	eglInitialize(egldisplay, NULL, NULL);
	assert(eglGetError() == EGL_SUCCESS);
	eglBindAPI(EGL_OPENGL_ES_API);

	eglChooseConfig(egldisplay, s_configAttribs, &eglconfig, 1, &numconfigs);
	assert(eglGetError() == EGL_SUCCESS);
	assert(numconfigs == 1);

	eglNativeWindow = fsl_createwindow(egldisplay, eglNativeDisplayType);
	assert(eglNativeWindow);

	eglsurface = eglCreateWindowSurface(egldisplay, eglconfig, eglNativeWindow, NULL);

	assert(eglGetError() == EGL_SUCCESS);
	EGLint ContextAttribList[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

	eglcontext = eglCreateContext(egldisplay, eglconfig, EGL_NO_CONTEXT, ContextAttribList);
	assert(eglGetError() == EGL_SUCCESS);
	eglMakeCurrent(egldisplay, eglsurface, eglsurface, eglcontext);
	assert(eglGetError() == EGL_SUCCESS);

	{
		// Compile the shaders
		GLuint hVertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(hVertexShader, 1, &g_strVertexShader, NULL);
		glCompileShader(hVertexShader);

		// Check for compile success
		GLint nCompileResult = 0;
		glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &nCompileResult);
		if (0 == nCompileResult) {
			char strLog[1024];
			GLint nLength;
			glGetShaderInfoLog(hVertexShader, 1024, &nLength, strLog);
			return FALSE;
		}

		GLuint hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(hFragmentShader, 1, &g_strFragmentShader, NULL);
		glCompileShader(hFragmentShader);

		// Check for compile success
		glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &nCompileResult);
		if (0 == nCompileResult) {
			char strLog[1024];
			GLint nLength;
			glGetShaderInfoLog(hFragmentShader, 1024, &nLength, strLog);
			return FALSE;
		}
		// Attach the individual shaders to the common shader program
		g_hShaderProgram = glCreateProgram();
		glAttachShader(g_hShaderProgram, hVertexShader);
		glAttachShader(g_hShaderProgram, hFragmentShader);

		// Init attributes BEFORE linking
		glBindAttribLocation(g_hShaderProgram, g_hVertexLoc, "g_vPosition");
		glBindAttribLocation(g_hShaderProgram, g_hColorLoc, "g_vColor");

		printf("about to link shader...");
		glBindAttribLocation(g_hShaderProgram, g_hVertexTexLoc, "g_vTexCoord");

		// Link the vertex shader and fragment shader together
		glLinkProgram(g_hShaderProgram);

		// Check for link success
		GLint nLinkResult = 0;
		glGetProgramiv(g_hShaderProgram, GL_LINK_STATUS, &nLinkResult);
		if (0 == nLinkResult) {
			char strLog[1024];
			GLint nLength;
			glGetProgramInfoLog(g_hShaderProgram, 1024, &nLength, strLog);
			printf("error linking shader");
			return FALSE;
		}
		// Get uniform locations
		g_hModelViewMatrixLoc =
		    glGetUniformLocation(g_hShaderProgram, "g_matModelView");
		g_hProjMatrixLoc = glGetUniformLocation(g_hShaderProgram, "g_matProj");

		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);

		//gen textures
		/* Load in the texture */
		if (LoadGLTextures() == 0) {
			printf("error loading texture");
			return 0;
		}

		/* Enable Texture Mapping ( NEW ) */
		glEnable(GL_TEXTURE_2D);

	}

	return 1;
}

void Cleanup()
{

}

void deinit(void)
{
	printf("Cleaning up...\n");
	Cleanup();
	eglMakeCurrent(egldisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	assert(eglGetError() == EGL_SUCCESS);
	eglDestroyContext(egldisplay, eglcontext);
	eglDestroySurface(egldisplay, eglsurface);
	fsl_destroywindow(eglNativeWindow, eglNativeDisplayType);
	eglTerminate(egldisplay);
	assert(eglGetError() == EGL_SUCCESS);
	eglReleaseThread();
}

void sighandler(int signal)
{
	printf("Caught signal %d, setting flaq to quit.\n", signal);
	quit = 1;
}

int main(void)
{
	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);
	assert(init());

	while (!quit) {
		EGLint width = 0;
		EGLint height = 0;
		eglQuerySurface(egldisplay, eglsurface, EGL_WIDTH, &width);
		eglQuerySurface(egldisplay, eglsurface, EGL_HEIGHT, &height);
		render(width, height);
		eglSwapBuffers(egldisplay, eglsurface);
	}
	deinit();
	return 0;
}
