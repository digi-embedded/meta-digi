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
#include <GLU3/glu3.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG2RAD(d) ((d) * M_PI / 180.0)

const GLUmat4 gluIdentityMatrix = {
	{
		{ { 1.0f, 0.0f,  0.0f,  0.0f } },
		{ { 0.0f, 1.0f,  0.0f,  0.0f } },
		{ { 0.0f, 0.0f,  1.0f,  0.0f } },
		{ { 0.0f, 0.0f,  0.0f,  1.0f } }
	}
};
INLINE void gluAdd4v_4v(GLUvec4 *result,
			       const GLUvec4 *v1, const GLUvec4 *v2)
{
	result->values[0] = v1->values[0] + v2->values[0];
	result->values[1] = v1->values[1] + v2->values[1];
	result->values[2] = v1->values[2] + v2->values[2];
	result->values[3] = v1->values[3] + v2->values[3];
}

INLINE GLfloat gluDot4_4v(const GLUvec4 *v1, const GLUvec4 *v2)
{
	return v1->values[0] * v2->values[0]
		+ v1->values[1] * v2->values[1]
		+ v1->values[2] * v2->values[2]
		+ v1->values[3] * v2->values[3];
}

INLINE GLfloat gluLengthSqr4v(const GLUvec4 *v)
{
	return gluDot4_4v(v, v);
}


INLINE GLfloat gluLength4v(const GLUvec4 *v)
{
	return (GLfloat) sqrt(gluLengthSqr4v(v));
}

INLINE void gluDiv4v_f(GLUvec4 *result,
			      const GLUvec4 *v1, GLfloat f)
{
	result->values[0] = v1->values[0] / f;
	result->values[1] = v1->values[1] / f;
	result->values[2] = v1->values[2] / f;
	result->values[3] = v1->values[3] / f;
}


INLINE void gluNormalize4v(GLUvec4 *result, const GLUvec4 *v)
{
	gluDiv4v_f(result, v, gluLength4v(v));
}

INLINE void gluCross4v(GLUvec4 *result,
			      const GLUvec4 *v1, const GLUvec4 *v2)
{
	GLUvec4 temp;

	temp.values[0] = (v1->values[1] * v2->values[2])
		- (v1->values[2] * v2->values[1]);
	temp.values[1] = (v1->values[2] * v2->values[0])
		- (v1->values[0] * v2->values[2]);
	temp.values[2] = (v1->values[0] * v2->values[1])
		- (v1->values[1] * v2->values[0]);
	temp.values[3] = 0.0;
	*result = temp;
}

INLINE void gluTranspose4m(GLUmat4 *result, const GLUmat4 *m)
{
	unsigned i;
	unsigned j;
	GLUmat4 temp;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			temp.col[i].values[j] = m->col[j].values[i];
		}
	}

	*result = temp;
}

INLINE void gluMult4v_f(GLUvec4 *result,
			       const GLUvec4 *v1, GLfloat f)
{
	result->values[0] = v1->values[0] * f;
	result->values[1] = v1->values[1] * f;
	result->values[2] = v1->values[2] * f;
	result->values[3] = v1->values[3] * f;
}

INLINE void gluMult4m_4v(GLUvec4 *result,
				const GLUmat4 *m, const GLUvec4 *v)
{
	GLUvec4 temp[6];
	unsigned i;

	for (i = 0; i < 4; i++) {
		gluMult4v_f(& temp[i], & m->col[i], v->values[i]);
	}

	gluAdd4v_4v(& temp[4], & temp[0], & temp[1]);
	gluAdd4v_4v(& temp[5], & temp[2], & temp[3]);
	gluAdd4v_4v(result,    & temp[4], & temp[5]);
}

INLINE void gluMult4m_4m(GLUmat4 *result,
				const GLUmat4 *m1, const GLUmat4 *m2)
{
	GLUmat4 temp;
	unsigned i;

	for (i = 0; i < 4; i++) {
		gluMult4m_4v(& temp.col[i], m1, & m2->col[i]);
	}

	*result = temp;
}

void gluTranslate4v(GLUmat4 *result, const GLUvec4 *t)
{
	memcpy(result, &gluIdentityMatrix, sizeof(gluIdentityMatrix));
	result->col[3] = *t;
	result->col[3].values[3] = 1.0f;
}


void gluScale4v(GLUmat4 *result, const GLUvec4 *t)
{
	memcpy(result, &gluIdentityMatrix, sizeof(gluIdentityMatrix));
	result->col[0].values[0] = t->values[0];
	result->col[1].values[1] = t->values[1];
	result->col[2].values[2] = t->values[2];
}

void gluLookAt4v(GLUmat4 *result, const GLUvec4 *_eye, const GLUvec4 *_center, const GLUvec4 *_up)
{
	static const GLUvec4 col3 = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	const GLUvec4 e = {
		{ -_eye->values[0], -_eye->values[1], -_eye->values[2], 0.0f }
	};
	GLUmat4  translate;
	GLUmat4  rotate;
	GLUmat4  rotateT;
	GLUvec4  f;
	GLUvec4  s;
	GLUvec4  u;
	GLUvec4  center, up;

	center = *_center;
	center.values[3] = 0;
	up = *_up;
	up.values[3] = 0;

	gluAdd4v_4v(& f, &center, &e);
	gluNormalize4v(& f, & f);

	gluNormalize4v(& u, &up);

	gluCross4v(& s, & f, & u);
	gluCross4v(& u, & s, & f);

	rotate.col[0] = s;
	rotate.col[1] = u;
	rotate.col[2].values[0] = -f.values[0];
	rotate.col[2].values[1] = -f.values[1];
	rotate.col[2].values[2] = -f.values[2];
	rotate.col[2].values[3] = 0.0f;
	rotate.col[3] = col3;
	gluTranspose4m(& rotateT, & rotate);

	gluTranslate4v(& translate, & e);
	gluMult4m_4m(result, & rotateT, & translate);
}

void gluRotate4v(GLUmat4 *result, const GLUvec4 *_axis, GLfloat angle)
{
	GLUvec4 axis;
	const float c = cos(angle);
	const float s = sin(angle);
	const float one_c = 1.0 - c;

	float xx;
	float yy;
	float zz;

	float xs;
	float ys;
	float zs;

	float xy;
	float xz;
	float yz;


	gluNormalize4v(& axis, _axis);

	xx = axis.values[0] * axis.values[0];
	yy = axis.values[1] * axis.values[1];
	zz = axis.values[2] * axis.values[2];

	xs = axis.values[0] * s;
	ys = axis.values[1] * s;
	zs = axis.values[2] * s;

	xy = axis.values[0] * axis.values[1];
	xz = axis.values[0] * axis.values[2];
	yz = axis.values[1] * axis.values[2];


	result->col[0].values[0] = (one_c * xx) + c;
	result->col[0].values[1] = (one_c * xy) + zs;
	result->col[0].values[2] = (one_c * xz) - ys;
	result->col[0].values[3] = 0.0;

	result->col[1].values[0] = (one_c * xy) - zs;
	result->col[1].values[1] = (one_c * yy) + c;
	result->col[1].values[2] = (one_c * yz) + xs;
	result->col[1].values[3] = 0.0;


	result->col[2].values[0] = (one_c * xz) + ys;
	result->col[2].values[1] = (one_c * yz) - xs;
	result->col[2].values[2] = (one_c * zz) + c;
	result->col[2].values[3] = 0.0;

	result->col[3].values[0] = 0.0;
	result->col[3].values[1] = 0.0;
	result->col[3].values[2] = 0.0;
	result->col[3].values[3] = 1.0;
}

void gluFrustum6f(GLUmat4 *result,
	     GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
	     GLfloat n, GLfloat f)
{
	if ((right == left) || (top == bottom) || (n == f)
	    || (n < 0.0) || (f < 0.0))
		return;


	memcpy(result, &gluIdentityMatrix, sizeof(gluIdentityMatrix));

	result->col[0].values[0] = (2.0 * n) / (right - left);
	result->col[1].values[1] = (2.0 * n) / (top - bottom);

	result->col[2].values[0] = (right + left) / (right - left);
	result->col[2].values[1] = (top + bottom) / (top - bottom);
	result->col[2].values[2] = -(f + n) / (f - n);
	result->col[2].values[3] = -1.0;

	result->col[3].values[2] = -(2.0 * f * n) / (f - n);
	result->col[3].values[3] =  0.0;
}

void gluPerspective4f(GLUmat4 *result,
		 GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f)
{
	const double sine = sin(DEG2RAD(fovy / 2.0));
	const double cosine = cos(DEG2RAD(fovy / 2.0));
	const double sine_aspect = sine * aspect;
	const double dz = f - n;


	memcpy(result, &gluIdentityMatrix, sizeof(gluIdentityMatrix));
	if ((sine == 0.0) || (dz == 0.0) || (sine_aspect == 0.0)) {
		return;
	}

	result->col[0].values[0] = cosine / sine_aspect;
	result->col[1].values[1] = cosine / sine;
	result->col[2].values[2] = -(f + n) / dz;
	result->col[2].values[3] = -1.0;
	result->col[3].values[2] = -2.0 * n * f / dz;
	result->col[3].values[3] =  0.0;
}

void gluOrtho6f(GLUmat4 *result,
	   GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
	   GLfloat n, GLfloat f)
{
	if ((right == left) || (top == bottom) || (n == f))
		return;

	(void) memcpy(result, & gluIdentityMatrix, sizeof(*result));
	result->col[0].values[0] = 2.0 / (right - left);
	result->col[1].values[1] = 2.0 / (top - bottom);
	result->col[2].values[2] = -2.0 / (f - n);

	result->col[3].values[0] = -(right + left) / (right - left);
	result->col[3].values[1] = -(top + bottom) / (top - bottom);
	result->col[3].values[2] = -(f + n) / (f - n);
}

void gluOrtho4f(GLUmat4 *result, GLfloat left, GLfloat right, GLfloat bottom,
	   GLfloat top)
{
	gluOrtho6f(result, left, right, bottom, top, -1.0, 1.0);
}


static double det3(const GLUmat4 *m, unsigned i, unsigned j)
{
	unsigned r;
	unsigned c;
	double det = 0.0;
	GLUvec4 col[6];


	/* Generate a 3x3 matrix from the original matrix with the ith column
	 * and the jth row removed.  The columns of the matrix are duplicated
	 * to make the 'c - r' column addressing, below, work out easier.
	 */
	for (c = 0; c < 4; c++) {
		if (c < i) {
			col[c + 0] = m->col[c];
			col[c + 3] = m->col[c];
		} else if (c > i) {
			col[c - 1] = m->col[c];
			col[c + 2] = m->col[c];
		}
	}

	for (r = j; r < 3; r++) {
		col[0].values[r] = col[0].values[r + 1];
		col[1].values[r] = col[1].values[r + 1];
		col[2].values[r] = col[2].values[r + 1];
		col[3].values[r] = col[3].values[r + 1];
		col[4].values[r] = col[4].values[r + 1];
		col[5].values[r] = col[5].values[r + 1];
	}


	/* Calculate the determinant of the resulting 3x3 matrix.
	 */
	for (c = 0; c < 3; c++) {
		double diag1 = col[c].values[0];
		double diag2 = col[c].values[0];

		for (r = 1; r < 3; r++) {
			diag1 *= col[(0 + c) + r].values[r];
			diag2 *= col[(3 + c) - r].values[r];
		}

		det += (diag1 - diag2);
	}

	return det;
}

GLfloat gluDeterminant4_4m(const GLUmat4 *m)
{
	double det = 0.0;
	unsigned c;

	for (c = 0; c < 4; c++) {
		if (m->col[c].values[3] != 0.0) {
			/* The usual equation is -1**(i+j) where i and j are
			 * the row and column of the matrix on the range
			 * [1, rows] and [1, cols].  Note that r and c are on
			 * the range [0, rows - 1] and [0, cols - 1].
			 */
			const double sign = ((c ^ 3) & 1) ? -1.0 : 1.0;
			const double d = det3(m, c, 3);

			det += sign * m->col[c].values[3] * d;
		}
	}

	return det;
}


GLboolean gluInverse4_4m(GLUmat4 *result, const GLUmat4 *m)
{
	const double det = gluDeterminant4_4m(m);
	double inv_det;
	unsigned c;
	unsigned r;


	if (det == 0.0)
		return GL_FALSE;

	inv_det = 1.0 / det;
	for (c = 0; c < 4; c++) {
		for (r = 0; r < 4; r++) {
			/* The usual equation is -1**(i+j) where i and j are
			 * the row and column of the matrix on the range
			 * [1, rows] and [1, cols].  Note that r and c are on
			 * the range [0, rows - 1] and [0, cols - 1].
			 */
			const double sign = ((c ^ r) & 1) ? -1.0 : 1.0;
			const double d = det3(m, c, r);

			result->col[r].values[c] = sign * inv_det * d;
		}
	}

	return GL_TRUE;
}




