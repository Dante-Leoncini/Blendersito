/*
 * ==============================================================================
 *  Name        : Glutils.cpp
 *
 *  Copyright (c) 2004-2006 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation.
 * ==============================================================================
 */

// INCLUDE FILES

#include "Glutils.h"
#include <eikapp.h>
#include <eikappui.h>

// CONSTANTS

// =============================================================================
// =============================================================================
// Selected utility functions from the GLU and GLUT libraries

// -----------------------------------------------------------------------------
// gluPerspective()
// Implementation of the GLU library function that allows the set up of a
// perspective projection matrix using field of view angles and aspect ratio.
// -----------------------------------------------------------------------------
void gluPerspective( GLfloat aFovY,  GLfloat aAspectRatio,
                     GLfloat aNearZ, GLfloat aFarZ )
    {
    GLfloat minX, maxX, minY, maxY;

    maxY = aNearZ * tan( aFovY * PI / 360.f );
    minY = -maxY;
    minX = minY * aAspectRatio;
    maxX = maxY * aAspectRatio;

    glFrustumf( minX, maxX, minY, maxY, aNearZ, aFarZ );
    }

// -----------------------------------------------------------------------------
// gluPerspective()
// Fixed point implementation of the GLU library function that allows the set up
// of a perspective projection matrix using field of view angles and aspect ratio.
// -----------------------------------------------------------------------------
void gluPerspectivex( GLfixed aFovY,  GLfixed aAspectRatio,
                      GLfixed aNearZ, GLfixed aFarZ )
    {
    gluPerspective( FIXED_2_FLOAT( aFovY ),  FIXED_2_FLOAT( aAspectRatio ),
                    FIXED_2_FLOAT( aNearZ ), FIXED_2_FLOAT( aFarZ ) );
    }

// -----------------------------------------------------------------------------
// gluLookAt()
// Implementation of the GLU library function that sets up the modelview
// transformation so that the eye (or camera) is located at the given
// coordinates and the center (look at) point is at the given coordinates and
// the up direction of the camera points to the given direction.
// -----------------------------------------------------------------------------
//
void gluLookAt( GLfloat aCameraX, GLfloat aCameraY, GLfloat aCameraZ,
	  		    GLfloat aLookAtX, GLfloat aLookAtY, GLfloat aLookAtZ,
	  		    GLfloat aUpDirX,  GLfloat aUpDirY,  GLfloat aUpDirZ )
	{
	GLfloat magnitude;         // Used to calculate magnitudes (lengths) of vectors
	GLfloat matrix[16];        // Model view matrix
	GLfloat x[3], y[3], z[3];  // Basis vectors for the "post transformation" coordinate system

	// Build the rotation only matrix

    // Calculate the Z basis vector, which points straight ahead; the
    // difference from the position (eye point) to the look-at point.
	// This is the direction of the gaze.
	z[0] = aCameraX - aLookAtX;
	z[1] = aCameraY - aLookAtY;
	z[2] = aCameraZ - aLookAtZ;

	// Normalize the Z basis vector
	magnitude = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
	if ( magnitude != 0.f )
		{
		z[0] /= magnitude;
		z[1] /= magnitude;
		z[2] /= magnitude;
		}

	// Calculate the Y basis vector, which points straight up;
	// this is simply the up vector given as parameter
	y[0] = aUpDirX;
	y[1] = aUpDirY;
	y[2] = aUpDirZ;

	// Calculate the X basis vector with Y cross Z operation;
	// this makes it sure that the X basis vector is linearly independent
	// (does not lie on the same plane) from the Y and Z vectors.
    x[0] =  y[1] * z[2] - y[2] * z[1];
    x[1] = -y[0] * z[2] + y[2] * z[0];
    x[2] =  y[0] * z[1] - y[1] * z[0];

	// Recompute Y basis vector with Z cross X operation;
	// this is to make sure the new Y vector is at exactly 90
	// degree angle to the plane formed by X and Z vectors.
	// This is turn makes it sure that the view matrix formed
	// from the basis vectors doesn't shear transformed geometry.
    y[0] =  z[1] * x[2] - z[2] * x[1];
    y[1] = -z[0] * x[2] + z[2] * x[0];
    y[2] =  z[0] * x[1] - z[1] * x[0];

	// Normalize both X and Y basis vectors
	magnitude = sqrt( x[0] * x[0] + x[1] * x[1] + x[2] * x[2] );
	if( magnitude != 0.f )
		{
		x[0] /= magnitude;
		x[1] /= magnitude;
		x[2] /= magnitude;
		}
	magnitude = sqrt( y[0] * y[0] + y[1] * y[1] + y[2] * y[2] );
	if( magnitude != 0.f )
		{
		y[0] /= magnitude;
		y[1] /= magnitude;
		y[2] /= magnitude;
		}

	// Use the basis vectors to define the view matrix
   matrix[0]  = x[0];
   matrix[4]  = x[1];
   matrix[8]  = x[2];
   matrix[12] = 0.0;

   matrix[1]  = y[0];
   matrix[5]  = y[1];
   matrix[9]  = y[2];
   matrix[13] = 0.0;

   matrix[2]  = z[0];
   matrix[6]  = z[1];
   matrix[10] = z[2];
   matrix[14] = 0.0;

   matrix[3]  = 0.0;
   matrix[7]  = 0.0;
   matrix[11] = 0.0;
   matrix[15] = 1.0;

	// Apply the new view matrix
    glMultMatrixf( matrix );

	// Translate camera (eye) to origin
	glTranslatef( -aCameraX, -aCameraY, -aCameraZ );
	}

// -----------------------------------------------------------------------------
// gluLookAtx()
// Fixed point implementation of the GLU library function that sets up the
// modelview transformation so that the eye (or camera) is located at the given
// coordinates and the center (look at) point is at the given coordinates and
// the up direction of the camera points to the given direction.
// -----------------------------------------------------------------------------
//
void gluLookAtx( GLfixed aCameraX, GLfixed aCameraY, GLfixed aCameraZ,
	  		     GLfixed aLookAtX, GLfixed aLookAtY, GLfixed aLookAtZ,
	  		     GLfixed aUpDirX,  GLfixed aUpDirY,  GLfixed aUpDirZ )
    {
    gluLookAtx( FIXED_2_FLOAT( aCameraX ),  FIXED_2_FLOAT( aCameraY ), FIXED_2_FLOAT( aCameraZ ),
                FIXED_2_FLOAT( aLookAtX ),  FIXED_2_FLOAT( aLookAtY ), FIXED_2_FLOAT( aLookAtZ ),
                FIXED_2_FLOAT( aUpDirX ),  FIXED_2_FLOAT( aUpDirY ), FIXED_2_FLOAT( aUpDirZ ) );

    }

// -----------------------------------------------------------------------------
// glutSolidCube()
// Implementation of GLUT function that draws a solid cube centered at
// the modeling coordinates origin with sides of length size.
// -----------------------------------------------------------------------------
//
void glutSolidCube( GLfloat aSize )
	{
	static GLfloat objVertexData[] =
    {
 	 1.f, 1.f, 1.f,// 1  = 1
	 1.f, 1.f,-1.f,// 2
	-1.f, 1.f, 1.f,// 0
	 1.f, 1.f,-1.f,// 2  = 2
	-1.f, 1.f,-1.f,// 3
	-1.f, 1.f, 1.f,// 0

	-1.f,-1.f, 1.f,// 4  = 3
	 1.f,-1.f, 1.f,// 5
	-1.f, 1.f, 1.f,// 0
	 1.f,-1.f, 1.f,// 5  = 4
 	 1.f, 1.f, 1.f,// 1
	-1.f, 1.f, 1.f,// 0

	 1.f,-1.f, 1.f,// 5  = 5
	 1.f, 1.f,-1.f,// 2
 	 1.f, 1.f, 1.f,// 1
	 1.f,-1.f, 1.f,// 5  = 6
	 1.f,-1.f,-1.f,// 6
	 1.f, 1.f,-1.f,// 2

	-1.f,-1.f,-1.f,// 7  = 7
	-1.f,-1.f, 1.f,// 4
	-1.f, 1.f, 1.f,// 0
	-1.f, 1.f,-1.f,// 3  = 8
	-1.f,-1.f,-1.f,// 7
	-1.f, 1.f, 1.f,// 0

	-1.f,-1.f,-1.f,// 7  = 9
	-1.f, 1.f,-1.f,// 3
	 1.f, 1.f,-1.f,// 2
	 1.f,-1.f,-1.f,// 6  = 10
	-1.f,-1.f,-1.f,// 7
	 1.f, 1.f,-1.f,// 2

	 1.f,-1.f,-1.f,// 6  = 11
	 1.f,-1.f, 1.f,// 5
	-1.f,-1.f, 1.f,// 4
	 1.f,-1.f,-1.f,// 6  = 12
	-1.f,-1.f, 1.f,// 4
	-1.f,-1.f,-1.f // 7
    };


	static GLbyte objNormaldata[]=
    {
    0,1,0, //  = 1
    0,1,0,
    0,1,0,
    0,1,0, //  = 2
    0,1,0,
    0,1,0,

    0,0,1, //  = 3
    0,0,1,
    0,0,1,
    0,0,1, //  = 4
    0,0,1,
    0,0,1,

    1,0,0, //  = 5
    1,0,0,
    1,0,0,
    1,0,0, //  = 6
    1,0,0,
    1,0,0,

    -1,0,0, //  = 7
    -1,0,0,
    -1,0,0,
    -1,0,0, //  = 8
    -1,0,0,
    -1,0,0,

    0,0,-1, //  = 9
    0,0,-1,
    0,0,-1,
    0,0,-1, //  = 10
    0,0,-1,
    0,0,-1,

    0,-1,0, //  = 11
    0,-1,0,
    0,-1,0,
    0,-1,0, //  = 12
    0,-1,0,
    0,-1,0
    };

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

    // Set array pointers
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glVertexPointer(   3, GL_FLOAT, 0, objVertexData  );
    glNormalPointer(      GL_BYTE,  0, objNormaldata  );

	// Scale to the given size, draw and restore previous scaling
	glScalef( aSize, aSize, aSize );
    glDrawArrays( GL_TRIANGLES, 0, 36 );

	glPopMatrix();
}

// -----------------------------------------------------------------------------
// glutWireCube()
// Implementation of GLUT function that draws a wireframe cube centered at
// the modeling coordinates origin with sides of length size.
// -----------------------------------------------------------------------------
//
void glutWireCube( GLfloat aSize )
	{
	static GLfloat objVertexData[] =
    {
    // Top
	-1.f, 1.f, 1.f,// 0
 	 1.f, 1.f, 1.f,// 1
 	 1.f, 1.f, 1.f,// 1
	 1.f, 1.f,-1.f,// 2
	 1.f, 1.f,-1.f,// 2
	-1.f, 1.f,-1.f,// 3
	-1.f, 1.f,-1.f,// 3
	-1.f, 1.f, 1.f,// 0

    // Bottom
	-1.f,-1.f, 1.f,// 0
 	 1.f,-1.f, 1.f,// 1
 	 1.f,-1.f, 1.f,// 1
	 1.f,-1.f,-1.f,// 2
	 1.f,-1.f,-1.f,// 2
	-1.f,-1.f,-1.f,// 3
	-1.f,-1.f,-1.f,// 3
	-1.f,-1.f, 1.f,// 0

	// Sidelines
	-1.f,-1.f, 1.f,// 0
	-1.f, 1.f, 1.f,// 0
 	 1.f,-1.f, 1.f,// 1
 	 1.f, 1.f, 1.f,// 1
	 1.f,-1.f,-1.f,// 2
	 1.f, 1.f,-1.f,// 2
	-1.f,-1.f,-1.f,// 3
	-1.f, 1.f,-1.f // 3

    };

	static GLbyte objNormaldata[]=
    {
    // Top
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,

	// Bottom
    0,-1,0,
    0,-1,0,
    0,-1,0,
    0,-1,0,
    0,-1,0,
    0,-1,0,
    0,-1,0,
    0,-1,0,

	// Sidelines
	0,0,1,
	0,0,1,
	1,0,0,
	1,0,0,
	0,0,-1,
	0,0,-1,
	-1,0,0,
	-1,0,0

    };

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

    // Set array pointers
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glVertexPointer(   3, GL_FLOAT, 0, objVertexData  );
    glNormalPointer(      GL_BYTE,  0, objNormaldata  );

	// Scale to the given size, draw and restore previous scaling
	glScalef( aSize, aSize, aSize );
    glDrawArrays( GL_LINES, 0, 24 );

	glPopMatrix();
}
