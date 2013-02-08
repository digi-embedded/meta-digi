
//#ifndef __GLUE3__
//#define __GLUE3__

#include <math.h>
#include <string.h>
#ifdef USE_GL20
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif USE_MX31
#include <GLES/gl.h>
#else
#include <GLES/gl.h>
#include <GLES/glext.h>
#endif

# define INLINE inline

/**
 * Basic four-component vector type.
 */
typedef struct GLUvec4_s {
	/** Data values of the vector. */
	GLfloat values[4];
}GLUvec4;



/**
 * Basic 4x4 matrix type.
 */
typedef struct GLUmat4_s {
	/** Columns of the matrix. */
	GLUvec4 col[4];
}GLUmat4;

void gluTranslate4v(GLUmat4 *result, const GLUvec4 *t);
void gluScale4v(GLUmat4 *result, const GLUvec4 *t);
void gluLookAt4v(GLUmat4 *result, const GLUvec4 *_eye, const GLUvec4 *_center, const GLUvec4 *_up);
void gluRotate4v(GLUmat4 *result, const GLUvec4 *_axis, GLfloat angle);
void gluFrustum6f(GLUmat4 *result,
	     GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
	     GLfloat n, GLfloat f);
void gluPerspective4f(GLUmat4 *result,
		 GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f);
void gluOrtho6f(GLUmat4 *result,
	   GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
	   GLfloat n, GLfloat f);
void gluOrtho4f(GLUmat4 *result, GLfloat left, GLfloat right, GLfloat bottom,
	   GLfloat top);
// static double det3(const GLUmat4 *m, unsigned i, unsigned j);
GLfloat gluDeterminant4_4m(const GLUmat4 *m);
GLboolean gluInverse4_4m(GLUmat4 *result, const GLUmat4 *m);

//#endif /* __GLU3__ */
