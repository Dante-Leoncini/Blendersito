/*
 * ==============================================================================
 *  Name        : Mathutils.cpp
 *
 *  Copyright (c) 2004-2006 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation.
 * ==============================================================================
 */

// INCLUDE FILES

#include "Mathutils.h"

// =============================================================================
// =============================================================================
// Math helper functions for floating point calculations

// -----------------------------------------------------------------------------
// sqrt()
// Helper function that ignores any errors in the calculations.
// Returns: Square root for the given value or 0 if an error occured.
// -----------------------------------------------------------------------------
//
GLfloat sqrt( GLfloat aIn )
{
    TReal trg;
    if (Math::Sqrt( trg, (TReal) aIn ) == KErrNone)
    {
        return (GLfloat) trg;
    }
    return (GLfloat) 0;
}

// -----------------------------------------------------------------------------
// sin()
// Helper function that ignores any errors in the calculations.
// Returns: Sine for the given value or 0 if an error occured.
// -----------------------------------------------------------------------------
//
GLfloat sin( GLfloat aRad )
{
    TReal trg;
    TReal src = (TReal) aRad;
    if (Math::Sin( trg, src ) == KErrNone)
    {
        return (TReal) trg;
    }
    return (GLfloat) 0;
}

// -----------------------------------------------------------------------------
// asin()
// Helper function that ignores any errors in the calculations.
// Returns: Inverse sine for the given value or 0 if an error occured.
// -----------------------------------------------------------------------------
//
GLfloat asin( GLfloat aRad )
{
    TReal trg;
    TReal src = (TReal) aRad;
    if (Math::ASin( trg, src ) == KErrNone)
    {
        return (TReal) trg;
    }
    return (GLfloat) 0;
}

// -----------------------------------------------------------------------------
// cos()
// Helper function that ignores any errors in the calculations.
// Returns: Cosine for the given value or 0 if an error occured.
// -----------------------------------------------------------------------------
//
GLfloat cos( GLfloat aRad )
{
    TReal trg;
    TReal src = (TReal) aRad;
    if (Math::Cos( trg, src ) == KErrNone)
    {
        return (GLfloat) trg;
    }
    return (GLfloat) 0;
}

// -----------------------------------------------------------------------------
// acos()
// Helper function that ignores any errors in the calculations.
// Returns: Inverse cosine for the given value or 0 if an error occured.
// -----------------------------------------------------------------------------
//
GLfloat acos( GLfloat aRad )
{
    TReal trg;
    TReal src = (TReal) aRad;
    if (Math::ACos( trg, src ) == KErrNone)
    {
        return (GLfloat) trg;
    }
    return (GLfloat) 0;
}

// -----------------------------------------------------------------------------
// tan()
// Helper function that ignores any errors in the calculations.
// Returns: Tangent for the given value or 0 if an error occured.
// -----------------------------------------------------------------------------
//
GLfloat tan( GLfloat aRad )
{
    TReal trg;
    TReal src = (TReal) aRad;
    if (Math::Tan( trg, src ) == KErrNone)
    {
        return (GLfloat) trg;
    }
    return (GLfloat) 0;
}


// -----------------------------------------------------------------------------
// atan()
// Helper function that ignores any errors in the calculations.
// Returns: Inverse tangent for the given value or 0 if an error occured.
// -----------------------------------------------------------------------------
//
GLfloat atan( GLfloat aRadY, GLfloat aRadX )
{
    TReal trg;
    TReal srcY = (TReal) aRadY;
    TReal srcX = (TReal) aRadX;
    if( Math::ATan( trg, srcY, srcX ) == KErrNone )
    {
        return (GLfloat) trg;
    }
    return (GLfloat) 0;
}

// ----------------------------------------------------------------------
// Create a random float number in the given range using the given seed.
// Because this method uses user given seed, this method is most usefull
// for predictable random value generation.
// ----------------------------------------------------------------------
//
GLfloat randrange( GLfloat aLow, GLfloat aHigh, TInt64 &aSeed )
    {
    return (GLfloat) ( Math::FRand( aSeed ) * ( aHigh - aLow ) + aLow );
    }



// ----------------------------------------------------------------------
// Create a random float number in the given range using built in seed.
// Because this method uses built in seed, this method is most usefull
// for non-predictive random value generation.
// ----------------------------------------------------------------------
//
GLfloat randrange( GLfloat aLow, GLfloat aHigh )
    {
    // The internal seed used by the randrange function
    static TInt64 randomSeed = User::NTickCount();
    return (GLfloat) ( Math::FRand( randomSeed ) * ( aHigh - aLow ) + aLow );
    }

// =============================================================================
// =============================================================================
// Math helper functions for fixed point calculations


// -----------------------------------------------------------------------------
// mulx()
// Multiplies the two given fixed point values and returns the result.
// -----------------------------------------------------------------------------
GLfixed fixedMul( GLfixed aIn1, GLfixed aIn2 )
    {
    return ( ( aIn1 >> 8 ) * ( aIn2 >> 8 ) );
    }

/**
 * Divides the first fixed point parameter with the second fixed point parameter
 * (aIn1/aIn2) and returns the result.
 * @param aIn1 Divident.
 * @param aIn2 Divider.
 * @return Result of the division (= aIn1 / aIn2).
 */
// -----------------------------------------------------------------------------
// divx()
// Divides the first fixed point value with the second one and returns the result.
// -----------------------------------------------------------------------------
GLfixed fixedDiv( GLfixed aIn1, GLfixed aIn2 )
    {
    return (GLfixed) ( ( ( (TInt64) aIn1 ) << 16 ) / aIn2 );
    }

// -----------------------------------------------------------------------------
// Finds out the next power of two for a given number.
// -----------------------------------------------------------------------------
GLint nextPowerOfTwo(GLint n)
    {
    GLint m = 1;

    if (n >= (1 << 30))
        return (1 << 30);

    while (m < n)
        {
        m <<= 1;
        }
    return m;
    }
