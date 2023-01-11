/*
 * ==============================================================================
 *  Name        : Glutils.h
 *
 *  Copyright (c) 2004-2006 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation.
 * ==============================================================================
 */

#ifndef __GLUTILS__
#define __GLUTILS__

//  INCLUDE FILES

#include <fbs.h>
#include <e32math.h>
#include <w32std.h>
#include <eikenv.h>
#include <GLES/gl.h>      // OpenGL ES header file

#include "Mathutils.h"


// MACROS

// FORWARD DECLARATIONS

// =============================================================================
// =============================================================================
// Selected utility functions from the GLU and GLUT libraries

/**
 * Implementation of the GLU function that allows the set up of a perspective
 * projection matrix using field of view angles and aspect ratio.
 * @param aFovY Field of view in angles in the Y direction.
 * @param aAspectRatio Aspect ratio that determines the field of view in the X direction.
 *                     The aspect ratio is the ratio of x (width) to y (height).
 * @param aNearZ  Distance from the viewer to the near clipping plane (always positive).
 * @param aFarZ  Distance from the viewer to the far clipping plane (always positive).
 */
void gluPerspective( GLfloat aFovY,  GLfloat aAspectRatio,
                     GLfloat aNearZ, GLfloat aFarZ );

/**
 * Fixed point implementation of the GLU function that allows the set up of a
 * perspective projection matrix using field of view angles and aspect ratio.
 * @param aFovY Field of view in angles in the Y direction.
 * @param aAspectRatio Aspect ratio that determines the field of view in the X direction.
 *                     The aspect ratio is the ratio of x (width) to y (height).
 * @param aNearZ  Distance from the viewer to the near clipping plane (always positive).
 * @param aFarZ  Distance from the viewer to the far clipping plane (always positive).
 */
void gluPerspectivex( GLfixed aFovY,  GLfixed aAspectRatio,
                      GLfixed aNearZ, GLfixed aFarZ );

/**
 * Implementation of the GLU function that sets up the modelview transformation so that
 * the eye (or camera) is located at the given coordinates and the center (look at) point
 * is at the given coordinates and the up direction of the camera points to the given direction.
 */
void gluLookAt( GLfloat aCameraX, GLfloat aCameraY, GLfloat aCameraZ,
	  		    GLfloat aLookAtX, GLfloat aLookAtY, GLfloat aLookAtZ,
	  		    GLfloat aUpDirX,  GLfloat aUpDirY,  GLfloat aUpDirZ );

/**
 * Fixed point implementation of the GLU function that sets up the modelview
 * transformation so that the eye (or camera) is located at the given coordinates
 * and the center (look at) point is at the given coordinates and the up direction
 * of the camera points to the given direction.
 */
void gluLookAtx( GLfixed aCameraX, GLfixed aCameraY, GLfixed aCameraZ,
	  		     GLfixed aLookAtX, GLfixed aLookAtY, GLfixed aLookAtZ,
	  		     GLfixed aUpDirX,  GLfixed aUpDirY,  GLfixed aUpDirZ );

/**
 * Implementation of GLUT function that draws a solid cube centered at
 * the modeling coordinates origin with sides of length size.
 * @param aSize Length of one side of the cube.
 */
void glutSolidCube( GLfloat aSize );

/**
 * Implementation of GLUT function that draws a wireframe cube centered at
 * the modeling coordinates origin with sides of length size.
 * @param aSize Length of one side of the cube.
 */
void glutWireCube( GLfloat aSize );

#endif
