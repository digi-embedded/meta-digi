/*
 * Copyright © 2009 Ian D. Romanick
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __glu3_h__
#define __glu3_h__

/**
 * \file  glu3.h
 * Interface definitions for GLU3 library.
 */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

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

#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else
# ifndef HAVE__BOOL
#  ifdef __cplusplus
typedef bool _Bool;
#  else
#   define _Bool signed char
#  endif
# endif
# define bool _Bool
# define false 0
# define true 1
# define __bool_true_false_are_defined 1
#endif

#define GLU3_VERSION_0_1
#define GLU3_VERSION_0_9

#ifndef GLchar
#define GLchar GLubyte
#endif

#ifndef GLdouble
#define GLdouble GLfloat
#endif

struct GLUmat4;

/**
 * Basic four-component vector type.
 */
struct GLUvec4 {
	/** Data values of the vector. */
	GLfloat values[4];

#ifdef __cplusplus
	/** Default constructor.  Data values are uninitialized. */
	inline GLUvec4(void)
	{
	}

	/** Initialize vector from one float value. */
	inline GLUvec4(GLfloat v)
	{
		values[0] = v;
		values[1] = v;
		values[2] = v;
		values[3] = v;
	}

	/** Initialize vector from four float values. */
	inline GLUvec4(GLfloat x , GLfloat y, GLfloat z, GLfloat w)
	{
		values[0] = x;
		values[1] = y;
		values[2] = z;
		values[3] = w;
	}

	/** Initialize vector from another vector. */
	inline GLUvec4(const GLUvec4 &v)
	{
		values[0] = v.values[0];
		values[1] = v.values[1];
		values[2] = v.values[2];
		values[3] = v.values[3];
	}

	/**
	 * Multiply a vector with a matrix.
	 *
	 * Multiply a row-vector with a 4x4 matrix resulting in a
	 * row-vector.
	 */
	GLUvec4 operator *(const GLUmat4 &) const;

	/**
	 * Component-wise multiplication with a vec4.
	 *
	 * \sa gluMult4v_4v
	 */
	GLUvec4 operator *(const GLUvec4 &) const;

	/**
	 * Multiply with a scalar.
	 *
	 * \sa gluMult4v_f
	 */
	GLUvec4 operator *(GLfloat) const;

	/** Component-wise addition with a vec4. */
	GLUvec4 operator +(const GLUvec4 &) const;

	/** Component-wise subtraction with a vec4. */
	GLUvec4 operator -(const GLUvec4 &) const;
#endif /* __cplusplus */
};


#ifdef __cplusplus
inline GLUvec4 operator *(GLfloat f, const GLUvec4 &v)
{
	return v * f;
}

inline GLUvec4 &operator +=(GLUvec4 &l, const GLUvec4 &r)
{
	l = l + r;
	return l;
}

inline GLUvec4 &operator -=(GLUvec4 &l, const GLUvec4 &r)
{
	l = l - r;
	return l;
}

inline GLUvec4 &operator *=(GLUvec4 &l, const GLUvec4 &r)
{
	l = l * r;
	return l;
}

inline GLUvec4 &operator *=(GLUvec4 &l, GLfloat r)
{
	l = l * r;
	return l;
}
#endif /* __cplusplus */


/**
 * Basic 4x4 matrix type.
 */
struct GLUmat4 {
	/** Columns of the matrix. */
	struct GLUvec4 col[4];

#ifdef __cplusplus
	/** Default constructor.  Columns are uninitialized. */
	inline GLUmat4(void)
	{
	}

	/** Initialize a matrix from four vec4 inputs.
	 *  Each vec4 is a column in the resulting matrix.
	 */
	inline GLUmat4(const GLUvec4 & c0, const GLUvec4 & c1,
		       const GLUvec4 & c2, const GLUvec4 & c3)
	{
		col[0] = c0;
		col[1] = c1;
		col[2] = c2;
		col[3] = c3;
	}

	/** Initialize a matrix from another matrix. */
	inline GLUmat4(const GLUmat4 &m)
	{
		col[0] = m.col[0];
		col[1] = m.col[1];
		col[2] = m.col[2];
		col[3] = m.col[3];
	}


	/**
	 * Multiply a vector with a matrix.
	 *
	 * Multiply as a column-vector with a 4x4 matrix resulting in a
	 * column-vector.
	 *
	 * \sa gluMult4m_4v
	 */
	GLUvec4 operator *(const GLUvec4 &) const;

	/**
	 * Matrix multiply with a 4x4 matrix.
	 *
	 * \sa gluMult4m_4m
	 */
	GLUmat4 operator *(const GLUmat4 &) const;

	/** Multiply with a scalar. */
	GLUmat4 operator *(GLfloat) const;

	/** Component-wise addition with a mat4. */
	GLUmat4 operator +(const GLUmat4 &) const;

	/** Component-wise subtraction with a mat4. */
	GLUmat4 operator -(const GLUmat4 &) const;
#endif	/* __cplusplus */
};

#define GLU_MAX_STACK_DEPTH 32

struct GLUmat4Stack {
	struct GLUmat4 stack[GLU_MAX_STACK_DEPTH];
	unsigned top;

#ifdef __cplusplus
	GLUmat4Stack() : top(0)
	{
		/* empty */
	}
#endif	/* __cplusplus */
};


struct GLUarcball {
	/**
	 * Base location of the viewport.
	 */
	/*@{*/
	unsigned viewport_x;
	unsigned viewport_y;
	/*@}*/

	/**
	 * Dimensions of the viewport.
	 */
	/*@{*/
	unsigned viewport_width;
	unsigned viewport_height;
	/*@}*/

	/**
	 * Screen X/Y location of initial mouse click.
	 */
	/*@{*/
	unsigned click_x;
	unsigned click_y;
	/*@}*/


#ifdef __cplusplus
	void viewport(unsigned x, unsigned y, unsigned width, unsigned height)
	{
		viewport_x = x;
		viewport_y = y;
		viewport_width = width;
		viewport_height = height;
	}

	void click(unsigned x, unsigned y)
	{
		click_x = x;
		click_y = y;
	}

	GLUmat4 drag(unsigned end_x, unsigned end_y);
#endif	/* __cplusplus */
};


#if 0
/**
 * Consumer for shape data generated by a GLUshapeProducer object
 *
 * Objects of this class and its descenents are used to consume data generated
 * by \c GLUshapeProducer.  The \c GLUshapeProducer object is responsible for
 * the format of the data generate, and the \c GLUshapeConsumer object is
 * repsonsible for storing that data.
 *
 * This splits the functionality of the classic GLU's \c GLUquadric structure.
 *
 * \sa GLUshapeProducer
 */
class GLUshapeConsumer {
public:
	/**
	 * Emit an individual vertex
	 *
	 * \param position  Object-space position of the vertex.
	 * \param normal    Object-space normal of the vertex.
	 * \param tangent   Object-space tangent of the vertex.
	 * \param uv        Parameter-space position of the vertex.  The
	 *                  per-vertex values will range from (0,0,0,0) to
	 *                  (1, 1, 0, 0).
	 */
	virtual void vertex(const GLUvec4 &position,
			    const GLUvec4 &normal,
			    const GLUvec4 &tangent,
			    const GLUvec4 &uv) = 0;

	/**
	 * Start a new indexed primitive.
	 *
	 * \param mode  GL primitive drawing mode used for this primitive
	 */
	virtual void begin_primitive(GLenum mode) = 0;

	/**
	 * Emit an element index for drawing
	 */
	virtual void index(unsigned idx) = 0;

	/**
	 * End an index primitive previously started with begin_primitive
	 */
	virtual void end_primitive(void) = 0;
};


/**
 * Base class of a shape generators.
 *
 * Base class defines the interface for all shape generators.  Each concrete
 * subclass is responsible for providing the pure virtual query and data
 * generation methods.  Data produced by a shape generator is pushed to a
 * \c GLUshapeConsumer as it is generated.
 *
 * \sa GLUshapeConsumer
 */
class GLUshapeProducer {
public:
	virtual ~GLUshapeProducer()
	{
	}

	/**
	 * Select the orientation of generated normals
	 *
	 * \param outside  Set to true if normals should point towards the
	 *                 outside of the object.
	 */
	void orientation(bool outside);

	/**
	 * Get the number of vertices in the shape
	 *
	 * This can be used in the constructor for derived classes, for
	 * example, to determine how much storage to allocate for vertex data.
	 */
	virtual unsigned vertex_count(void) const = 0;

	/**
	 * Get the number of elements used to draw primitives for the shape
	 *
	 * This can be used in the constructor for derived classes, for
	 * example, to determine how much storage to allocate for element data.
	 */
	virtual unsigned element_count(void) const = 0;

	/**
	 * Get the number of primitves used to draw the shape
	 *
	 * This can be used in the constructor for derived classes, for
	 * example, to determine the primitive count for a call to
	 * \c glMultiDrawElements or to determine how much padding to allocate
	 * for restart values used with primitive restart.
	 */
	virtual unsigned primitive_count(void) const = 0;

	/**
	 * Generate the primitive
	 *
	 * Causes the data for the primitive to be generated.  This will result
	 * in the \c vertex, \c begin_primitive, \c index, and \c end_primitive
	 * methods of \c consumer being invoked with the data as it is
	 * generated.
	 */
	virtual void generate(GLUshapeConsumer *consumer) const = 0;

protected:
	GLUshapeProducer(void) :
	  normals_point_out(true)
	{
	}

	bool normals_point_out;
};


/**
 * Shape generator that generates a sphere.
 */
class GLUsphereProducer : public GLUshapeProducer {
public:
	/**
	 * Construct a new sphere shape generator
	 *
	 * \param radius  Specifies the radius of the sphere.
	 * \param slices  Specifies the number of subdivisions around the
	 *                z-axis.  These subdivisions are analogous to the
	 *                slices of an orange.  These also match longitude
	 *                lines on the globe.
	 * \param stacks  Specifies the number of subdivisions along the
	 *                z-axis.  These match the latitude lines on the globe.
	 */
	GLUsphereProducer(GLdouble radius, GLint slices, GLint stacks);
	virtual unsigned vertex_count(void) const;
	virtual unsigned element_count(void) const;
	virtual unsigned primitive_count(void) const;
	virtual void generate(GLUshapeConsumer *consumer) const;

private:
	double radius;
	unsigned slices;
	unsigned stacks;
};


/**
 * Shape generator that generates a cube.
 */
class GLUcubeProducer : public GLUshapeProducer {
public:
	/**
	 * Construct a new cube shape generator
	 *
	 * \param radius  Distance from the center of the cube to the center
	 *                of one of the axis-aligned faces.
	 */
	GLUcubeProducer(GLdouble radius);
	virtual unsigned vertex_count(void) const;
	virtual unsigned element_count(void) const;
	virtual unsigned primitive_count(void) const;
	virtual void generate(GLUshapeConsumer *consumer) const;

private:
	double radius;
};
#endif

#ifndef __cplusplus
typedef struct GLUvec4 GLUvec4;
typedef struct GLUmat4 GLUmat4;
typedef struct GLUmat4Stack GLUmat4Stack;
typedef struct GLUarcball GLUarcball;
#endif /*  __cplusplus */


#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Four component dot product from vec4 sources.
 *
 * \sa gluDot4 (C++)
 */
GLfloat gluDot4_4v(const GLUvec4 *, const GLUvec4 *);

/**
 * Three component dot product from vec4 sources.
 *
 * \sa gluDot3 (C++)
 */
GLfloat gluDot3_4v(const GLUvec4 *, const GLUvec4 *);

/**
 * Two component dot product from vec4 sources.
 *
 * \sa gluDot2 (C++)
 */
GLfloat gluDot2_4v(const GLUvec4 *, const GLUvec4 *);

/**
 * Cross product from vec4 sources
 *
 * The 3-dimensional cross product of \c u and \c v is calculated.  The result
 * is stored in the first three components of \c result.  The fourth component
 * is set to 0.0.
 *
 * \sa gluCross (C++)
 */
void gluCross4v(GLUvec4 *result, const GLUvec4 *u, const GLUvec4 *v);

/**
 * Normalize a vec4
 *
 * The 4-dimensional normalization of \c u is stored in \c result.
 *
 * \sa gluNormalize (C++)
 */
void gluNormalize4v(GLUvec4 *result, const GLUvec4 *u);

/**
 * Calculate the length of a vec4
 *
 * The length (magnitude) the 4-dimensional vector \c u is returned.
 *
 * \sa gluLength (C++)
 */
GLfloat gluLength4v(const GLUvec4 *u);

/**
 * Calculate the squared length of a vec4
 *
 * The squared length (magnitude) the 4-dimensional vector \c u is returned.
 *
 * \sa gluLengthSqr (C++)
 */
GLfloat gluLengthSqr4v(const GLUvec4 *);

/**
 * Calculate the four dimensional outer product of two vec4 sources
 *
 * Assuing \c u and \c v are column vectors, the outer product is:
 *
 * \f$\left( \begin{tabular}{cccc}
 * $u_x v_x$ & $u_x v_y$ & $u_x v_y$ & $u_x v_w$ \\
 * $u_y v_x$ & $u_y v_y$ & $u_y v_y$ & $u_y v_w$ \\
 * $u_z v_x$ & $u_z v_y$ & $u_z v_y$ & $u_z v_w$ \\
 * $u_w v_x$ & $u_w v_y$ & $u_w v_y$ & $u_w v_w$ \\
 * \end{tabular} \right)\f$
 */
void gluOuter4v(GLUmat4 *result, const GLUvec4 *u, const GLUvec4 *v);


/**
 * Component-wise multiply two vec4s
 *
 * \sa GLUvec4::operator*
 */
void gluMult4v_4v(GLUvec4 *result, const GLUvec4 *, const GLUvec4 *);

/**
 * Component-wise divide two vec4s
 */
void gluDiv4v_4v(GLUvec4 *result, const GLUvec4 *, const GLUvec4 *);

/**
 * Component-wise add two vec4s
 *
 * \sa GLUvec4::operator+
 */
void gluAdd4v_4v(GLUvec4 *result, const GLUvec4 *, const GLUvec4 *);

/**
 * Component-wise subtract two vec4s
 *
 * \sa GLUvec4::operator-
 */
void gluSub4v_4v(GLUvec4 *result, const GLUvec4 *, const GLUvec4 *);

/**
 * Multiply with a scalar.
 *
 * \sa GLUvec4::operator*
 */
void gluMult4v_f(GLUvec4 *result, const GLUvec4 *, GLfloat);

/** Divide components of a vector by a scalar. */
void gluDiv4v_f(GLUvec4 *result, const GLUvec4 *, GLfloat);

/** Add a scalar to each of the components of a vector. */
void gluAdd4v_f(GLUvec4 *result, const GLUvec4 *, GLfloat);

/** Subtract a scalar from each of the components of a vector. */
void gluSub4v_f(GLUvec4 *result, const GLUvec4 *, GLfloat);

/**
 * Matrix multiply with a 4x4 matrix.
 *
 * \sa GLUmat4::operator*
 */
void gluMult4m_4m(GLUmat4 *result, const GLUmat4 *, const GLUmat4 *);

/**
 * Component-wise addition with a mat4.
 *
 * \sa GLUmat4::operator+
 */
void gluAdd4m_4m(GLUmat4 *result, const GLUmat4 *, const GLUmat4 *);

/**
 * Component-wise subtraction with a mat4.
 *
 * \sa GLUmat4::operator-
 */
void gluSub4m_4m(GLUmat4 *result, const GLUmat4 *, const GLUmat4 *);

/**
 * Multiply a vector with a matrix.
 *
 * Multiply as a column-vector with a 4x4 matrix resulting in a
 * column-vector.
 *
 * \sa GLUmat4::operator*
 */
void gluMult4m_4v(GLUvec4 *result, const GLUmat4 *m, const GLUvec4 *v);

/**
 * Multiply each component of a matrix with a scalar
 *
 * \sa GLUmat4::operator*
 */
void gluMult4m_f(GLUmat4 *result, const GLUmat4 *, GLfloat);

/**
 * Calculate a scaling transformation matrix from a vector
 *
 * A scaling transformation matrix is created using the x, y, and z
 * components of \c u.  Specifically, the matrix generated is:
 *
 * \f$\left( \begin{tabular}{cccc}
 * $u_x$ & $0$   & $0$   & $0$ \\
 * $0$   & $u_y$ & $0$   & $0$ \\
 * $0$   & $0$   & $u_z$ & $0$ \\
 * $0$   & $0$   & $0$   & $1$ \\
 * \end{tabular} \right)\f$
 *
 * \sa gluScale (C++)
 */
void gluScale4v(GLUmat4 *result, const GLUvec4 *u);

/** \name Translation matrix
 */
/*@{*/
/**
 * Calculate a translation matrix using x, y, and z offsets
 *
 * The matrix generated is:
 *
 * \f$\left( \begin{tabular}{cccc}
 * $1$ & $0$ & $0$ & $x$ \\
 * $0$ & $1$ & $0$ & $y$ \\
 * $0$ & $0$ & $1$ & $z$ \\
 * $0$ & $0$ & $0$ & $1$ \\
 * \end{tabular} \right)\f$
 *
 * \sa gluTranslate (C++)
 */
void gluTranslate3f(GLUmat4 *result, GLfloat x, GLfloat y, GLfloat z);

/**
 * Calculate a translation matrix using components of a vector
 *
 * The matrix generated is:
 *
 * \f$\left( \begin{tabular}{cccc}
 * $1$ & $0$ & $0$ & $v_x$ \\
 * $0$ & $1$ & $0$ & $v_y$ \\
 * $0$ & $0$ & $1$ & $v_z$ \\
 * $0$ & $0$ & $0$ & $1$ \\
 * \end{tabular} \right)\f$
 *
 * \sa gluTranslate (C++)
 */
void gluTranslate4v(GLUmat4 *result, const GLUvec4 *v);
/*@}*/

/**
 * Calculate a rotation matrix around an arbitrary axis
 *
 * \param axis  Axis, based at the origin, around which to rotate
 * \param angle Angle of rotation in radians
 *
 * If the specificed axis is not unit length, the vector will be normalized.
 *
 * \sa gluRotate (C++)
 */
void gluRotate4v(GLUmat4 *result, const GLUvec4 *axis, GLfloat angle);

/**
 * Calculate a viewing transformation
 *
 * \param eye    Position, in 3-dimensional space, of the eye point.
 * \param center Position, in 3-dimensional space, that the eye is looking at.
 * \param up     Direction of the up vector.
 * \param result Storage for the resulting matrix.
 *
 * Calculates a transformation matrix that maps the eye point to the origin
 * and the \c center to the negative Z axis.  The direction defined by \c up
 * is projected onto the X/Y plane an is mapped to the positive Y axis.
 *
 * The calculated matrix is:
 *
 * \f{eqnarray*}{
 * f  &=& c - e \\
 * f' &=& f \over |f| \\
 * u' &=& u \over |u| \\
 * s  &=& f \times u \\
 * u''  &=& s \times f \\
 * M &=&
 * \left( \begin{tabular}{cccc}
 * $s_x$   & $s_y$   & $s_z$   & $0$ \\
 * $u''_x$ & $u''_y$ & $u''_z$ & $0$ \\
 * $-f'_x$ & $-f'_y$ & $-f'_z$ & $0$ \\
 * $0$     & $0$     & $0$     & $1$ \\
 * \end{tabular} \right)
 * \times
 * \left( \begin{tabular}{cccc}
 * $1$ & $0$ & $0$ & $-e_x$ \\
 * $0$ & $1$ & $0$ & $-e_y$ \\
 * $0$ & $0$ & $1$ & $-e_z$ \\
 * $0$ & $0$ & $0$ & $1$ \\
 * \end{tabular} \right) \\
 * \f}
 *
 * \sa gluLookAt (C++)
 */
void gluLookAt4v(GLUmat4 *result, const GLUvec4 *eye, const GLUvec4 *center,
		 const GLUvec4 *up);

/**
 * \name Projection matrix
 *
 * Functions that generate various common projection matrixes.
 */
/*@{*/
/**
 * Generate a perspective projection matrix
 *
 * \param result Location to store the calculated matrix
 * \param left   Coordinate for the left clipping plane
 * \param right  Coordinate for the right clipping plane
 * \param top    Coordinate for the top clipping plane
 * \param bottom Coordinate for the bottom clipping plane
 * \param near   Distance to the near plane
 * \param far    Distance to the far plane
 *
 * The matrix calculated is:
 *
 * \f{eqnarray*}{
 * M &=&
 * \left( \begin{tabular}{cccc}
 * ${2 * near} \over {right - left}$    & $0$           & $ {{right + left} \over {right - left}}$ & $0$\\
 * $0$              & ${2 * near} \over {top - bottom}$ & $ {{top + bottom} \over {top - bottom}}$ & $0$ \\
 * $0$              & $0$                               & $-{{far + near}   \over {far - near}}$ & $-{{2 * far * near} \over {far - near}}$ \\
 * $0$              & $0$     & $-1$     & $0$ \\
 * \end{tabular} \right) \\
 * \f}
 *
 * If \c left = \c right, \c top = \c bottom, or \c near = \c far, the function
 * returns without writing any value to \c result.
 *
 * If either\c near or \c far are negative, the function returns without
 * writing any value to \c result.
 */
void gluFrustum6f(GLUmat4 *result, GLfloat left, GLfloat right, GLfloat bottom,
		  GLfloat top, GLfloat near, GLfloat far);

/**
 * Calculate a perspective projection matrix
 *
 * \param result Storage for the resulting matrix.
 * \param fovy   Field-of-view in the Y direction, measured in radians
 * \param aspect The ratio of the size in the X direction to the size in the
 *               Y direction.  This is used to calculate the field-of-view in
 *               the X direction.
 * \param near   Distance to the near plane
 * \param far    Distance to the far plane
 *
 * The matrix calculated is:
 *
 * \f{eqnarray*}{
 * f &=& cotangent {\left({fovy \over 2} \right)} \\
 * M &=&
 * \left( \begin{tabular}{cccc}
 * $f \over aspect$ & $0$  & $0$   & $0$ \\
 * $0$              & $f$ & $0$ & $0$ \\
 * $0$              & $0$ & ${far + near} \over {near - far}$ & ${2 \times far \times near} \over {near - far}$ \\
 * $0$              & $0$     & $0$     & $1$ \\
 * \end{tabular} \right) \\
 * \f}
 */
void gluPerspective4f(GLUmat4 *result, GLfloat fovy, GLfloat aspect,
		      GLfloat near, GLfloat far);

/**
 * Generate an orthographic projection matrix
 *
 * \param result Location to store the calculated matrix
 * \param left   Coordinate for the left clipping plane
 * \param right  Coordinate for the right clipping plane
 * \param top    Coordinate for the top clipping plane
 * \param bottom Coordinate for the bottom clipping plane
 *
 * The matrix calculated is:
 *
 * \f{eqnarray*}{
 * M &=&
 * \left( \begin{tabular}{cccc}
 * $2 \over {right - left}$ & $0$  & $0$   & $-{{right + left} \over {right - left}}$ \\
 * $0$              & $2 \over {top - bottom}$ & $0$ & $-{{top + bottom} \over {top - bottom}}$ \\
 * $0$              & $0$ & $-1$ & $0$ \\
 * $0$              & $0$ & $0$  & $1$ \\
 * \end{tabular} \right) \\
 * \f}
 *
 * If \c left = \c right or \c top = \c bottom the function returns without
 * writing any value to \c result.
 *
 * This function is identical to calling \c gluOrtho6f with \c near = -1 and
 * \c far = 1.
 *
 * \sa gluOrtho6f
 */
void gluOrtho4f(GLUmat4 *result, GLfloat left, GLfloat right, GLfloat bottom,
		GLfloat top);

/**
 * Generate an orthographic projection matrix
 *
 * \param result Location to store the calculated matrix
 * \param left   Coordinate for the left clipping plane
 * \param right  Coordinate for the right clipping plane
 * \param top    Coordinate for the top clipping plane
 * \param bottom Coordinate for the bottom clipping plane
 * \param near   Distance to the near plane
 * \param far    Distance to the far plane
 *
 * The matrix calculated is:
 *
 * \f{eqnarray*}{
 * M &=&
 * \left( \begin{tabular}{cccc}
 * $2 \over {right - left}$ & $0$  & $0$   & $-{{right + left} \over {right - left}}$ \\
 * $0$              & $2 \over {top - bottom}$ & $0$ & $-{{top + bottom} \over {top - bottom}}$ \\
 * $0$              & $0$ & $-2 \over {far - near}$ & $-{{far + near} \over {far - near}}$ \\
 * $0$              & $0$     & $0$     & $1$ \\
 * \end{tabular} \right) \\
 * \f}
 *
 * If \c left = \c right, \c top = \c bottom, or \c near = \c far, the function
 * returns without writing any value to \c result.
 *
 * \s gluOrtho4f
 */
void gluOrtho6f(GLUmat4 *result, GLfloat left, GLfloat right, GLfloat bottom,
		GLfloat top, GLfloat near, GLfloat far);
/*@}*/

/**
 * Calculate the transpose of a matrix.
 */
void gluTranspose4m(GLUmat4 *result, const GLUmat4 *m);

/**
 * Calculate the determinant of a matrix.
 *
 * \sa gluDeterminant4 (C++)
 */
GLfloat gluDeterminant4_4m(const GLUmat4 *m);

/**
 * Calculate the inverse of a matrix.
 *
 * Inverts the matrix \c m and stores the result in \c result.  If \c m is
 * not invertable, \c result is not modified
 *
 * \return
 * If the matrix is invertable (i.e., the determinant is not zero), \c GL_TRUE
 * is returned.  Otherwise GL_FALSE is returned.
 *
 * \sa gluInverse4 (C++)
 */
GLboolean gluInverse4_4m(GLUmat4 *result, const GLUmat4 *m);


/**
 * Identity matrix
 *
 * Global constant containing the matrix:
 *
 * \f$\left( \begin{tabular}{cccc}
 * $1$ & $0$ & $0$ & $0$ \\
 * $0$ & $1$ & $0$ & $0$ \\
 * $0$ & $0$ & $1$ & $0$ \\
 * $0$ & $0$ & $0$ & $1$ \\
 * \end{tabular} \right)\f$
 */
extern const GLUmat4 gluIdentityMatrix;

/**
 * Load a text file from disk
 *
 * \param file_name  Name of the file to be loaded
 *
 * Loads data from a named text file and returns a pointer to that data to the
 * caller.  This may be useful, for example, for loading shader code from flies
 * on disk.
 *
 * The pointer returned by this function should later be released by calling
 * \c gluUnloadTextFile.
 *
 * \note
 * The data pointed to by the return value if this function really is
 * constant.  On some systems this function may be implemented by creating a
 * read-only mapping of the file.  Writes to such data will result in program
 * termination.
 *
 * \sa gluUnloadTextFile
 */
extern const GLchar *gluLoadTextFile(const char *file_name);

/**
 * Release data previously loaded with gluLoadTextFile.
 *
 * \sa gluLoadTextFile
 */
extern void gluUnloadTextFile(const GLchar *text);

extern void gluArcballViewport(GLUarcball *ball, unsigned x, unsigned y,
    unsigned width, unsigned height);

extern void gluArcballClick(GLUarcball *ball, unsigned start_x,
    unsigned start_y);

extern void gluArcballDrag(GLUarcball *ball, GLUmat4 *transformation,
    unsigned end_x, unsigned end_y);

#ifdef __cplusplus
};
#endif

#ifdef __cplusplus
/**
 * Four component dot product from vec4 sources.
 *
 * \sa gluDot4_4v
 */
GLfloat gluDot4(const GLUvec4 &, const GLUvec4 &);

/**
 * Three component dot product from vec4 sources.
 *
 * \sa gluDot3_4v
 */
GLfloat gluDot3(const GLUvec4 &, const GLUvec4 &);

/**
 * Two component dot product from vec4 sources.
 *
 * \sa gluDot2_4v
 */
GLfloat gluDot2(const GLUvec4 &, const GLUvec4 &);

/**
 * Cross product from vec4 sources
 *
 * The 3-dimensional cross product of \c u and \c v is calculated.  The result
 * is stored in the first three components of \c result.  The fourth component
 * is set to 0.0.
 *
 * \sa gluCross4v
 */
inline GLUvec4 gluCross(const GLUvec4 &u, const GLUvec4 &v)
{
	GLUvec4 t;

	gluCross4v(& t, & u, & v);
	return t;
}

/**
 * Normalize a vec4
 *
 * The 4-dimensional normalization of \c u is stored in \c result.
 *
 * \sa gluNormalize4v
 */
inline GLUvec4 gluNormalize(const GLUvec4 &v)
{
	GLUvec4 t;

	gluNormalize4v(& t, & v);
	return t;
}

/**
 * Calculate the length of a vec4
 *
 * The length (magnitude) the 4-dimensional vector \c u is returned.
 *
 * \sa gluLength4v
 */
inline GLfloat gluLength(const GLUvec4 &u)
{
	return gluLength4v(& u);
}

/**
 * Calculate the squared length of a vec4
 *
 * The squared length (magnitude) the 4-dimensional vector \c u is returned.
 *
 * \sa gluLengthSqr4v
 */
inline GLfloat gluLengthSqr(const GLUvec4 &u)
{
	return gluLengthSqr4v(& u);
}

/**
 * Calculate a scaling transformation matrix from a vector
 *
 * A scaling transformation matrix is created using the x, y, and z
 * components of \c u.  Specifically, the matrix generated is:
 *
 * \f$\left( \begin{tabular}{cccc}
 * $u_x$ & $0$   & $0$   & $0$ \\
 * $0$   & $u_y$ & $0$   & $0$ \\
 * $0$   & $0$   & $u_z$ & $0$ \\
 * $0$   & $0$   & $0$   & $1$ \\
 * \end{tabular} \right)\f$
 *
 * \sa gluScale4v
 */
inline GLUmat4 gluScale(const GLUvec4 &u)
{
	GLUmat4 result;

	gluScale4v(& result, & u);
	return result;
}

/**
 * Calculate a scaling transformation matrix from a vector
 *
 * A scaling transformation matrix is created using the x, y, and z
 * components of \c u.  Specifically, the matrix generated is:
 *
 * \f$\left( \begin{tabular}{cccc}
 * $x$   & $0$   & $0$   & $0$ \\
 * $0$   & $y$   & $0$   & $0$ \\
 * $0$   & $0$   & $z$   & $0$ \\
 * $0$   & $0$   & $0$   & $1$ \\
 * \end{tabular} \right)\f$
 *
 * \sa gluScale4v
 */
inline GLUmat4 gluScale(GLfloat x, GLfloat y, GLfloat z)
{
	GLUvec4 u(x, y, z, 1.0);
	GLUmat4 result;

	gluScale4v(& result, & u);
	return result;
}

/** \name Translation matrix
 */
/*@{*/
/**
 * Calculate a translation matrix using x, y, and z offsets
 *
 * The matrix generated is:
 *
 * \f$\left( \begin{tabular}{cccc}
 * $1$ & $0$ & $0$ & $x$ \\
 * $0$ & $1$ & $0$ & $y$ \\
 * $0$ & $0$ & $1$ & $z$ \\
 * $0$ & $0$ & $0$ & $1$ \\
 * \end{tabular} \right)\f$
 *
 * \sa gluTranslate3f
 */
inline GLUmat4 gluTranslate(GLfloat x, GLfloat y, GLfloat z)
{
	GLUmat4 result;

	gluTranslate3f(& result, x, y, z);
	return result;
}

/**
 * Calculate a translation matrix using components of a vector
 *
 * The matrix generated is:
 *
 * \f$\left( \begin{tabular}{cccc}
 * $1$ & $0$ & $0$ & $v_x$ \\
 * $0$ & $1$ & $0$ & $v_y$ \\
 * $0$ & $0$ & $1$ & $v_z$ \\
 * $0$ & $0$ & $0$ & $1$ \\
 * \end{tabular} \right)\f$
 *
 * \sa gluTranslate4v
 */
inline GLUmat4 gluTranslate(const GLUvec4 &v)
{
	GLUmat4 result;

	gluTranslate4v(& result, & v);
	return result;
}
/*@}*/

/**
 * Calculate a rotation matrix around an arbitrary axis
 *
 * \param axis  Axis, based at the origin, around which to rotate
 * \param angle Angle of rotation in radians
 *
 * If the specificed axis is not unit length, the vector will be normalized.
 *
 * \sa gluRotate4v
 */
inline GLUmat4 gluRotate(const GLUvec4 &axis, GLfloat angle)
{
	GLUmat4 result;

	gluRotate4v(& result, & axis, angle);
	return result;
}

/**
 * Calculate a viewing transformation
 *
 * \param eye    Position, in 3-dimensional space, of the eye point.
 * \param center Position, in 3-dimensional space, that the eye is looking at.
 * \param up     Direction of the up vector.
 * \param result Storage for the resulting matrix.
 *
 * Calculates a transformation matrix that maps the eye point to the origin
 * and the \c center to the negative Z axis.  The direction defined by \c up
 * is projected onto the X/Y plane an is mapped to the positive Y axis.
 *
 * The calculated matrix is:
 *
 * \f{eqnarray*}{
 * f  &=& c - e \\
 * f' &=& f \over |f| \\
 * u' &=& u \over |u| \\
 * s  &=& f \times u \\
 * u''  &=& s \times f \\
 * M &=&
 * \left( \begin{tabular}{cccc}
 * $s_x$   & $s_y$   & $s_z$   & $0$ \\
 * $u''_x$ & $u''_y$ & $u''_z$ & $0$ \\
 * $-f'_x$ & $-f'_y$ & $-f'_z$ & $0$ \\
 * $0$     & $0$     & $0$     & $1$ \\
 * \end{tabular} \right)
 * \times
 * \left( \begin{tabular}{cccc}
 * $1$ & $0$ & $0$ & $-e_x$ \\
 * $0$ & $1$ & $0$ & $-e_y$ \\
 * $0$ & $0$ & $1$ & $-e_z$ \\
 * $0$ & $0$ & $0$ & $1$ \\
 * \end{tabular} \right) \\
 * \f}
 *
 * \sa gluLookAt4v
 */
inline GLUmat4 gluLookAt(const GLUvec4 &eye, const GLUvec4 &center,
			 const GLUvec4 &up)
{
	GLUmat4 result;

	gluLookAt4v(& result, & eye, & center, & up);
	return result;
}

/**
 * Calculate the determinant of a matrix.
 *
 * \sa gluDeterminant4_4m
 */
inline GLfloat gluDeterminant4(const GLUmat4 &m)
{
	return gluDeterminant4_4m(& m);
}

/**
 * Calculate the inverse of a matrix.
 *
 * Inverts the matrix \c m and stores the result in \c result.  If \c m is
 * not invertable, \c result is not modified
 *
 * \return
 * If the matrix is invertable (i.e., the determinant is not zero), \c GL_TRUE
 * is returned.  Otherwise GL_FALSE is returned.
 *
 * \sa gluInverse4_4m
 */
inline GLboolean gluInverse4(GLUmat4 &result, const GLUmat4 &m)
{
	return gluInverse4_4m(& result, & m);
}

/**
 * Calculate the inverse of a matrix.
 *
 * \return
 * The inverse of the matrix \c m.  If \c m is not invertable, the return
 * result is undefined.
 *
 * \warning
 * This function is really only safe when the input matrix is known to be
 * invertable.  Nearly all well behaved transformation matrices fall into this
 * category.
 *
 * \sa gluInverse4_4m
 */
inline GLUmat4 gluInverse4(const GLUmat4 &m)
{
	GLUmat4 result;

	gluInverse4_4m(& result, & m);
	return result;
}


inline GLUmat4 GLUarcball::drag(unsigned end_x, unsigned end_y)
{
	GLUmat4 result;

	gluArcballDrag(this, & result, end_x, end_y);
	return result;
}
#endif /* __cplusplus */

#if defined(__cplusplus)
extern "C" {
#endif
/**
 * \name Shading language helper functions
 */
/*@{*/
/**
 * Initialize the GLSL compiler infrastructure
 *
 * This function \b must be called before any of the GLU3 GLSL helper functions
 * can be used.  On Windows, this function must be called each time a context
 * with a different color depth is made current.
 *
 * It is the responsibility of the caller to verify that the required version
 * of GLSL and the required shader targets (e.g., geometry) are supported.
 *
 * \return
 * If GLSL is available, \c true is returned.  Otherwise \c false is returned.
 */
extern bool gluInitializeCompiler(void);

/**
 * Compile a shader
 *
 * Creates a new shader object for the specified target and compiles the
 * supplied code into that shader object.  If \c log_ptr is not \c NULL, a
 * buffer will be allocated and filled with diagnostic messages from the
 * shading language compiler.  A pointer to this buffer will stored in
 * \c log_ptr.
 *
 * The pointer stored in \c log_ptr must be later released with
 * \c gluReleaseInfoLog.
 *
 * \param target   Shader execution unit (e.g., \c GL_VERTEX_SHADER)
 * \param code     Shader source code
 * \param log_ptr  Location to store a pointer to the compiler generate info log
 *
 * \return
 * If compilation was successful, the shader object is returned.  On failure
 * zero is returned.
 *
 * \sa gluReleaseInfoLog
 */
extern GLint gluCompileShader(GLenum target, const char *code, char **log_ptr);

/**
 * Link a shader program
 *
 * Links the specified shader program.  If \c log_ptr is not \c NULL, a buffer
 * will be allocated and filled with diagnostic messages from the shading
 * language linker.  A pointer to this buffer will stored in \c log_ptr.
 *
 * The pointer stored in \c log_ptr must be later released with
 * \c gluReleaseInfoLog.
 *
 * \param prog     Shading language program to be linked
 * \param log_ptr  Location to store a pointer to the compiler generate info log
 *
 * \return
 * If linking was successful, \c true is returned.  Otherwise \c false is
 * returned.
 *
 * \sa gluReleaseInfoLog
 */
extern bool gluLinkProgram(GLuint prog, char **log_ptr);

/**
 * Attach a list of shader objects to a program
 *
 * Attaches a zero-terminated list of shader objects to a program object.
 *
 * \param prog     Shading language program to which shaders will be attached
 * \param shader   First shader to be attached to the program
 */
extern void gluAttachShaders(GLuint prog, GLuint shader, ...);

/**
 * Bind a set of shader program attributes to locations
 *
 * Bind the locations of a set of attributes.  The list of attributes is
 * terminate by a \c NULL \c name pointer.
 *
 * \param prog     Shading language program whose attribute locations will
 *                 be set
 * \param name     Name of the first attribute to set
 * \param location Location of the first attribute
 */
extern void gluBindAttributes(GLuint prog, const char *name, unsigned location,
			      ...);

/**
 * Release an info log
 *
 * Release an info log generated by a call to \c gluLinkProgram or
 * \c gluCompileShader.
 *
 * \param log      Info log buffer to be released
 *
 * \sa gluCompileShader, gluLinkProgram
 */
extern void gluReleaseInfoLog(char *log);
/*@}*/
#if defined(__cplusplus)
};
#endif

#include "glu3_scalar.h"

#endif /* __glu3_h__ */
