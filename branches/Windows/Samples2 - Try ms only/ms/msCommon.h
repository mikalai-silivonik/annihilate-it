/*
 *  Common.h
 *  Tutorial1
 *
 *  Created by Michael Daley on 19/04/2009.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "msGL.h"

#include "math.h"





// Macro which returns a random value between -1 and 1
#define RANDOM_MINUS_1_TO_1() (((GLfloat)rand() / (GLfloat)RAND_MAX * 2.0f ) - 1.0f)

// MAcro which returns a random number between 0 and 1
#define RANDOM_0_TO_1() (((GLfloat)rand() / (GLfloat)RAND_MAX ))

#define GL_PI  (3.14159265f)

// Macro which converts degrees into radians
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0f * GL_PI)

struct msColor
{
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
};

struct msVector {
	GLfloat x;
	GLfloat y;
};

struct msQuad
{
	GLfloat bl_x, bl_y;
	GLfloat br_x, br_y;
	GLfloat tl_x, tl_y;
	GLfloat tr_x, tr_y;
};

struct msParticle
{
	msVector position;
	msVector direction;
	msColor color;
	msColor deltaColor;
	GLfloat particleSize;
	GLfloat timeToLive;
};


struct msPointSprite
{
	GLfloat x;
	GLfloat y;
	GLfloat size;
};

struct msPoint
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

struct msSize
{
	GLfloat width;
	GLfloat height;
};


struct msBorder
{
	GLint left;
	GLint top;
	GLint right;
	GLint bottom;
};



static const msVector Vector2fZero = {0.0f, 0.0f};

static inline msVector Vector2fMake(GLfloat x, GLfloat y)
{
	msVector v;
	v.x = x;
	v.y = y;
	return v;
}

static inline msColor colorMake(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	msColor c;
	c.r = red;
	c.g = green;
	c.b = blue;
	c.a = alpha;
	return c;
}

static inline msVector Vector2fMultiply(msVector v, GLfloat s)
{
	msVector vv;
	vv.x = v.x * s;
	vv.y = v.y * s;
	return vv;
}

static inline msVector Vector2fAdd(msVector v1, msVector v2)
{
	msVector vr;
	vr.x = v1.x + v2.x;
	vr.y = v1.y + v2.y;
	return vr;
}

static inline msVector Vector2fSub(msVector v1, msVector v2)
{
	msVector vr;
	vr.x = v1.x - v2.x;
	vr.y = v1.y - v2.y;
	return vr;
}

static inline GLfloat Vector2fDot(msVector v1, msVector v2)
{
	return (GLfloat) v1.x * v2.x + v1.y * v2.y;
}

static inline GLfloat Vector2fLength(msVector v)
{
	return (GLfloat) sqrt(Vector2fDot(v, v));
}

static inline msVector Vector2fNormalize(msVector v)
{
	return Vector2fMultiply(v, 1.0f/Vector2fLength(v));
} 