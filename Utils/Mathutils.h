/*
 * ==============================================================================
 *  Name        : Mathutils.h
 *
 *  Copyright (c) 2004-2006 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation.
 * ==============================================================================
 */

#ifndef __MATHUTILSH__
#define __MATHUTILSH__

//  INCLUDE FILES

#include <fbs.h>
#include <e32base.h>
#include <e32std.h>
#include <e32math.h>
#include <w32std.h>
#include <eikenv.h>
#include <GLES/gl.h>      // OpenGL ES header file

// MACROS

// Redefine value of pi.
#undef PI
#define PI 3.1415926535897932384626433832795f


// Converts angle __a from deg. to rad.
#define DEG_2_RAD(__a) ((__a)*(PI/180))

// Converts angle __a from rad. to deg.
#define RAD_2_DEG(__a) ((__a)*(180/PI))

// 16.16 Fixed point conversions and operations
#define INT_2_FIXED(__a) ( ( (GLfixed) (__a) ) << 16 )
#define FIXED_2_INT(__a) ( (GLint) ( (__a) >> 16 ) )
#define FLOAT_2_FIXED(__a) ( (GLfixed) ( 65536.0f * ( __a ) ) )
#define FIXED_2_FLOAT(__a) ( ( (GLfloat) (__a) ) * ( 1 / 65536.0f ) )
#define FIXED_ONE INT_2_FIXED( 1 )

#define FIXED_PI FLOAT_2_FIXED( 3.1415926535897932384626433832795f )


// =============================================================================
// =============================================================================
// Math helper functions for floating point calculations

/**
 * Helper function for calculating the square root for a given value.
 * Ignores any errors that may occur in the math library during the calculations.
 * @param aIn Value whose square root is to be returned.
 * @return Square root for the given value or 0 if an error occured.
 */
extern GLfloat sqrt(GLfloat aIn);

/**
 * Helper function for calculating the sine for a given value.
 * Ignores any errors that may occur in the math library during the calculations.
 * @param aIn Value whose sine is to be returned.
 * @return Sine for the given value or 0 if an error occured.
 */
extern GLfloat sin(GLfloat aIn);

/**
 * Helper function for calculating the inverse sine for a given value.
 * Ignores any errors that may occur in the math library during the calculations.
 * @param aIn Value whose inverse sine is to be returned.
 * @return Inverse sine for the given value or 0 if an error occured.
 */
extern GLfloat asin(GLfloat aIn);

/**
 * Helper function for calculating the cosine for a given value.
 * Ignores any errors that may occur in the math library during the calculations.
 * @param aIn Value whose cosine is to be returned.
 * @return Cosine for the given value or 0 if an error occured.
 */
extern GLfloat cos(GLfloat aIn);

/**
 * Helper function for calculating the inverse cosine for a given value.
 * Ignores any errors that may occur in the math library during the calculations.
 * @param aIn Value whose inverse cosine is to be returned.
 * @return Inverse cosine for the given value or 0 if an error occured.
 */
extern GLfloat acos(GLfloat aIn);

/**
 * Helper function for calculating the tangent for a given value.
 * Ignores any errors that may occur in the math library during the calculations.
 * @param aIn Value whose tangent is to be returned.
 * @return Tangent for the given value or 0 if an error occured.
 */
extern GLfloat tan(GLfloat aIn);

/**
 * Helper function for calculating the inverse tangent for a given x and y values.
 * Ignores any errors that may occur in the math library during the calculations.
 * @param aInY Y-value whose inverse tangent is to be returned.
 * @param aInX X-value whose inverse tangent is to be returned.
 * @return Inverse tangent for the given values or 0 if an error occured.
 */
extern GLfloat atan(GLfloat aInY, GLfloat aInX);

/**
 * Create a random float number in the given range using the given seed.
 * Because this method uses user given seed, this method is most usefull
 * for predictable random value generation.
 * @param aLow Lowest value that may be returned.
 * @param aHigh Highest value that may be returned.
 * @param aSeed Seed value used in the random number generation.
 * @return Random number in the given range.
 */
extern GLfloat randrange( GLfloat aLow, GLfloat aHigh, TInt64 &aSeed );

/**
 * Create a random float number in the given range using built in seed.
 * Because this method uses built in seed, this method is most usefull
 * for non-predictive random value generation.
 * @param aLow Lowest value that may be returned.
 * @param aHigh Highest value that may be returned.
 * @return Random number in the given range.
 */
extern GLfloat randrange( GLfloat aLow, GLfloat aHigh );

// =============================================================================
// =============================================================================
// Math helper functions for fixed point calculations

/**
 * Multiplies the two given fixed point values and returns the result.
 * @param aIn1 1st multiplier.
 * @param aIn2 2nd multiplier.
 * @return Result of the multiplication (= aIn1 * aIn2).
 */
extern GLfixed fixedMul( GLfixed aIn1, GLfixed aIn2 );

/**
 * Divides the first fixed point parameter with the second fixed point parameter
 * (aIn1/aIn2) and returns the result.
 * @param aIn1 Divident.
 * @param aIn2 Divider.
 * @return Result of the division (= aIn1 / aIn2).
 */
extern GLfixed fixedDiv( GLfixed aIn1, GLfixed aIn2 );

/**
 *  @returns the next largest power of two for n.
 */
extern GLint nextPowerOfTwo(GLint n);

#endif
