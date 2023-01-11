/*
 * ==============================================================================
 *  Name        : Utils3d.cpp
 *
 *  Copyright (c) 2004-2006 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation.
 * ==============================================================================
 */

// INCLUDE FILES

#include "Utils3d.h"
#include <eikapp.h>
#include <eikappui.h>

// CONSTANTS

// =============================================================================
// TVector
// =============================================================================

// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// TVector::TVector
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------

TVector::TVector()
	{
	*this = TVector(0, 0, 0);
	}

// -----------------------------------------------------------------------------
// TVector::TVector
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------

TVector::TVector(GLfloat aX, GLfloat aY, GLfloat aZ )
	: iX(aX), iY(aY), iZ(aZ)
	{
	}

// -----------------------------------------------------------------------------
// TVector::operator +
// Return a new vector with sum of this and rhs vector
// -----------------------------------------------------------------------------

TVector TVector::operator + (TVector aVector) const
	{
	return TVector(this->iX + aVector.iX, this->iY + aVector.iY,
		this->iZ + aVector.iZ);
	}

// -----------------------------------------------------------------------------
// TVector::operator +=
// Component-wise addition of vectors.
// -----------------------------------------------------------------------------

void TVector::operator += (TVector aVector)
	{
	*this = *this + aVector;
	}

// -----------------------------------------------------------------------------
// TVector::operator -
// Calculate the difference of this, and rhs vector.
// -----------------------------------------------------------------------------
TVector TVector::operator - (TVector aVector) const
	{
	return (*this + (aVector * - 1));
	}

// -----------------------------------------------------------------------------
// TVector::operator -
// Unary - == negation operator. Ineffective implementation, uses multiplication
// with -1.
// -----------------------------------------------------------------------------

TVector TVector::operator - () const
	{
	return ((*this) * (-1));
	}

// -----------------------------------------------------------------------------
// TVector::operator *
// Calculate the scalar multiplication, i.e. dot product, of this and rhs
// vector.
// -----------------------------------------------------------------------------
GLfloat TVector::operator * (TVector aVector) const
	{
	return (this->iX * aVector.iX + this->iY * aVector.iY +
		this->iZ * aVector.iZ);
	}

// -----------------------------------------------------------------------------
// TVector::operator *
// Calculate the multiplication of this vector and a scalar rhs
// -----------------------------------------------------------------------------
TVector TVector::operator * (GLfloat aScalar) const
	{
	return TVector(this->iX * aScalar, this->iY * aScalar, this->iZ * aScalar);
	}

// -----------------------------------------------------------------------------
// TVector::Magnitude
// Calculate the magnitude of this vector. Standard trigonometric calculation:
// sqrt(x**2 + y**2 + z**2)
// -----------------------------------------------------------------------------
GLfloat TVector::Magnitude()
	{
	return sqrt( (*this) * (*this) );
	}

// -----------------------------------------------------------------------------
// TVector::Normalize
// Normalizes the vector by dividing each component with the length of
// the vector.
// -----------------------------------------------------------------------------
void TVector::Normalize()
	{
	_LIT(KNormPanic, "TVector::Normalize()");
	GLfloat magnitude = Magnitude();
	if ( magnitude == 0.0f )
		{
		User::Panic( KNormPanic, 0 );
		}

	(*this) = (*this) * (1 / magnitude);
	}

// -----------------------------------------------------------------------------
// TVector::MultMatrix4x4
// Computes the multiplication of this vector with the given matrix.
// Basically transforms the this vector with the given transformation matrix.
// -----------------------------------------------------------------------------
void TVector::MultMatrix4x4( const GLfloat aMatrix[] )
    {
    GLfloat iXOrig = iX;
    GLfloat iYOrig = iY;
    GLfloat iZOrig = iZ;
    iX = ( iXOrig * aMatrix[0] ) + ( iYOrig * aMatrix[1] ) + ( iZOrig * aMatrix[ 2] ) + aMatrix[3];
    iY = ( iXOrig * aMatrix[4] ) + ( iYOrig * aMatrix[5] ) + ( iZOrig * aMatrix[ 6] ) + aMatrix[7];
    iZ = ( iXOrig * aMatrix[8] ) + ( iYOrig * aMatrix[9] ) + ( iZOrig * aMatrix[10] ) + aMatrix[11];
    }
// -----------------------------------------------------------------------------
// TVector::MultMatrix4x4
// Computes the multiplication of given vector with the given matrix.
// Basically transforms the vector with the given transformation matrix.
// -----------------------------------------------------------------------------
TVector TVector::MultMatrix4x4( const TVector aVector, const GLfloat aMatrix[] )
	{
	TVector vector;
    vector.iX = ( aVector.iX * aMatrix[0] ) + ( aVector.iY * aMatrix[1] ) + ( aVector.iZ * aMatrix[ 2] ) + aMatrix[3];
    vector.iY = ( aVector.iX * aMatrix[4] ) + ( aVector.iY * aMatrix[5] ) + ( aVector.iZ * aMatrix[ 6] ) + aMatrix[7];
    vector.iZ = ( aVector.iX * aMatrix[8] ) + ( aVector.iY * aMatrix[9] ) + ( aVector.iZ * aMatrix[10] ) + aMatrix[11];
    return vector;
    }


// -----------------------------------------------------------------------------
// TVector::CrossProduct
// Direct implementation of the cross-product calculation.
// -----------------------------------------------------------------------------
TVector TVector::CrossProduct(const TVector aVector1, const TVector aVector2)
	{
	TVector Vector;

	Vector.iX = aVector1.iY * aVector2.iZ - aVector1.iZ * aVector2.iY;
	Vector.iY = aVector1.iZ * aVector2.iX - aVector1.iX * aVector2.iZ;
	Vector.iZ = aVector1.iX * aVector2.iY - aVector1.iY * aVector2.iX;

	return Vector;
	}

// =============================================================================
// TVectorx: Fixed-point vector class
// =============================================================================

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TVectorx::TVectorx
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TVectorx::TVectorx()
	{
	*this = TVectorx(INT_2_FIXED(0), INT_2_FIXED(0), INT_2_FIXED(0) );
	}

// -----------------------------------------------------------------------------
// TVectorx::TVectorx
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TVectorx::TVectorx(GLfixed aX, GLfixed aY, GLfixed aZ )
	: iX(aX), iY(aY), iZ(aZ)
	{
	}

// -----------------------------------------------------------------------------
// TVectorx::operator +
// Return a new vector with sum of this and rhs vector
// -----------------------------------------------------------------------------
//
TVectorx TVectorx::operator + (TVectorx aVector) const
	{
	return TVectorx(this->iX + aVector.iX, this->iY + aVector.iY,
		this->iZ + aVector.iZ);
	}

// -----------------------------------------------------------------------------
// TVectorx::operator +=
// Component-wise addition of vectors.
// -----------------------------------------------------------------------------

void TVectorx::operator += (TVectorx aVector)
	{
	*this = *this + aVector;
	}

// -----------------------------------------------------------------------------
// TVector::operator -
// Calculate the difference of this, and rhs vector.
// -----------------------------------------------------------------------------
TVectorx TVectorx::operator - (TVectorx aVector) const
	{
	return (*this + (aVector * INT_2_FIXED(-1)));
	}

// -----------------------------------------------------------------------------
// TVector::operator -
// Unary - == negation operator. Ineffective implementation, uses multiplication
// with -1.
// -----------------------------------------------------------------------------
TVectorx TVectorx::operator - () const
	{
	return ((*this) * INT_2_FIXED(-1));
	}


// -----------------------------------------------------------------------------
// TVector::operator *
// Calculate the scalar multiplication, i.e. dot product, of this and rhs
// vector.
// -----------------------------------------------------------------------------
GLfixed TVectorx::operator * (TVectorx aVector) const
	{
	return (fixedMul(this->iX, aVector.iX) + fixedMul(this->iY, aVector.iY) +
		fixedMul(this->iZ, aVector.iZ));
	}

// -----------------------------------------------------------------------------
// TVector::operator *
// Calculate the multiplication of this vector and a scalar rhs
// -----------------------------------------------------------------------------
//
TVectorx TVectorx::operator * (GLfixed aScalar) const
	{
	return TVectorx( fixedMul( this->iX, aScalar ),
	                 fixedMul( this->iY, aScalar ),
		             fixedMul( this->iZ, aScalar ));
	}

// -----------------------------------------------------------------------------
// TVector::Magnitude
// Calculate the magnitude of this vector. Standard trigonometric calculation:
// sqrt(x**2 + y**2 + z**2)
// -----------------------------------------------------------------------------
//
GLfixed TVectorx::Magnitude() const
	{
	return FLOAT_2_FIXED( sqrt( FIXED_2_FLOAT( (*this) * (*this) ) ) );
	}

// -----------------------------------------------------------------------------
// TVector::Normalize
// Normalizes the vector by dividing each component with the length of
// the vector.
// -----------------------------------------------------------------------------
//
void TVectorx::Normalize()
	{
	_LIT(KNormPanic, "TVectorx::Normalize()");
	GLfixed magnitude = Magnitude();
	if ( magnitude == INT_2_FIXED(0) )
		{
		User::Panic( KNormPanic, 0 );
		}

	(*this) = (*this) * FLOAT_2_FIXED(1 / FIXED_2_FLOAT(magnitude));
	}

// -----------------------------------------------------------------------------
// TVector::CrossProduct
// Direct implementation of the cross-product calculation.
// -----------------------------------------------------------------------------
//
TVectorx TVectorx::CrossProduct(TVectorx aVector1, TVectorx aVector2)
	{
	TVectorx Vector;

	Vector.iX = fixedMul(aVector1.iY, aVector2.iZ) -
		fixedMul(aVector1.iZ, aVector2.iY);
	Vector.iY = fixedMul(aVector1.iZ, aVector2.iX) -
		fixedMul(aVector1.iX, aVector2.iZ);
	Vector.iZ = fixedMul(aVector1.iX, aVector2.iY) -
		fixedMul(aVector1.iY, aVector2.iX);

	return Vector;
	}

// =============================================================================
// T3DModel
// =============================================================================

// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// T3DModel::T3DModel
// C++ default constructor can NOT contain any code, that
// might leave.
// Constructs and initializes a T3DModel to Position (0,0,0), with orientation
// [Yaw=0, Pitch=0, Roll=0].
// -----------------------------------------------------------------------------
//
T3DModel::T3DModel()
	{
	*this = T3DModel(TVector(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f);
	}

// -----------------------------------------------------------------------------
// T3DModel::T3DModel
// C++ constructor can NOT contain any code, that
// might leave.
// Constructs and initializes a T3DModel to position aPosition, with
// orientation [aYaw, aPitch, aRoll].
// -----------------------------------------------------------------------------
//
T3DModel::T3DModel(TVector aPosition, GLfloat aYaw, GLfloat aPitch,
	GLfloat aRoll)
	{
	SetPosition(aPosition);
	SetYaw(aYaw);
	SetPitch(aPitch);
	SetRoll(aRoll);
	}

// -----------------------------------------------------------------------------
// T3DModel::MakeWorldViewMatrix
// Sets up a world + a view matrix.
// -----------------------------------------------------------------------------
//
void T3DModel::MakeWorldViewMatrix(TCamera & aCamera)
	{
	MakeWorldViewMatrix( aCamera, GetPosition(), GetYaw(), GetPitch(),
		GetRoll());
	}

// -----------------------------------------------------------------------------
// T3DModel::MakeWorldViewMatrix
// Sets up a world + a view matrix.
// -----------------------------------------------------------------------------
//
void T3DModel::MakeWorldViewMatrix(TCamera & aCamera, TVector aPosition,
	GLfloat aYaw, GLfloat aPitch, GLfloat aRoll)
	{
    glMultMatrixf( (GLfloat*)aCamera.GetViewMatrix() );

    glTranslatef( aPosition.iX-aCamera.iPosition.iX,
		aPosition.iY-aCamera.iPosition.iY, aPosition.iZ-aCamera.iPosition.iZ );
	if ( aRoll )
		{
		glRotatef( aRoll , 0, 0, 1);
		}
	if ( aYaw )
		{
		glRotatef( aYaw  , 0, 1, 0);
		}
	if ( aPitch )
		{
		glRotatef( aPitch, 1, 0, 0);
		}
	}

// -----------------------------------------------------------------------------
// T3DModel::MakeBlenderLiteWorldViewMatrix
// Sets up a blenderlite matrix, which is a matrix that rotates objects in such a
// way that they always face the camera.
// Refer to the blenderlite example to see how this method is used.
// -----------------------------------------------------------------------------
//
void T3DModel::MakeBlenderLiteWorldViewMatrix(TCamera & aCamera)
	{
	MakeBlenderLiteWorldViewMatrix( aCamera, GetPosition() );
	}

// -----------------------------------------------------------------------------
// T3DModel::MakeBlenderLiteWorldViewMatrix
// Sets up a blenderlite matrix, which is a matrix that rotates objects in such a
// way that they always face the camera.
// Refer to the blenderlite example to see how this method is used.
// -----------------------------------------------------------------------------
//
void T3DModel::MakeBlenderLiteWorldViewMatrix(TCamera & aCamera, TVector aPosition)
	{
	// Set up a rotation matrix to orient the blenderlite towards the camera.
	TVector dir   = aCamera.iLookAt - aCamera.iPosition;
	GLfloat angle = atan( dir.iZ, dir.iX );
	// The Yaw angle is computed in such a way that the object always faces the
	// camera.
	angle = -(RAD_2_DEG( angle ) + 90);

	MakeWorldViewMatrix( aCamera, aPosition, angle );
	}

// -----------------------------------------------------------------------------
// T3DModel::SetYaw
// -----------------------------------------------------------------------------
//
void T3DModel::SetYaw(GLfloat aYaw)
	{
	iYaw = aYaw;
	}

// -----------------------------------------------------------------------------
// T3DModel::SetPitch
// -----------------------------------------------------------------------------
//
void T3DModel::SetPitch(GLfloat aPitch)
	{
	iPitch = aPitch;
	}

// -----------------------------------------------------------------------------
// T3DModel::SetRoll
// -----------------------------------------------------------------------------
//
void T3DModel::SetRoll(GLfloat aRoll)
	{
	iRoll = aRoll;
	}

// -----------------------------------------------------------------------------
// T3DModel::SetPosition
// -----------------------------------------------------------------------------
//
void T3DModel::SetPosition(TVector aPosition)
	{
	iPosition = aPosition;
	}

// -----------------------------------------------------------------------------
// T3DModel::GetYaw
// -----------------------------------------------------------------------------
//
GLfloat T3DModel::GetYaw()
	{
	return iYaw;
	}
// -----------------------------------------------------------------------------
// T3DModel::GetPitch
// -----------------------------------------------------------------------------
//
GLfloat T3DModel::GetPitch()
	{
	return iPitch;
	}
// -----------------------------------------------------------------------------
// T3DModel::GetRoll
// -----------------------------------------------------------------------------
//
GLfloat T3DModel::GetRoll()
	{
	return iRoll;
	}
// -----------------------------------------------------------------------------
// T3DModel::GetPosition
// -----------------------------------------------------------------------------
//
TVector T3DModel::GetPosition()
	{
	return iPosition;
	}

// =============================================================================
// T3DModelx
// =============================================================================

// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// T3DModelx::T3DModelx
// C++ default constructor can NOT contain any code, that
// might leave.
// Constructs and initializes a T3DModelx to Position (0,0,0), with orientation
// [Yaw=0, Pitch=0, Roll=0].
// -----------------------------------------------------------------------------
//
T3DModelx::T3DModelx()
	{
	*this = T3DModelx(TVectorx(INT_2_FIXED(0), INT_2_FIXED(0), INT_2_FIXED(0)),
							   INT_2_FIXED(0), INT_2_FIXED(0), INT_2_FIXED(0));
	}
// -----------------------------------------------------------------------------
// T3DModelx::T3DModelx
// C++ default constructor can NOT contain any code, that
// might leave.
// Constructs and initializes a T3DModelx to position aPosition, with
// orientation [aYaw, aPitch, aRoll].
// -----------------------------------------------------------------------------
//
T3DModelx::T3DModelx(TVectorx aPosition, GLfixed aYaw, GLfixed aPitch,
	GLfixed aRoll)
	{
	SetPosition(aPosition);
	SetYaw(aYaw);
	SetPitch(aPitch);
	SetRoll(aRoll);
	}

// -----------------------------------------------------------------------------
// T3DModelx::MakeWorldViewMatrix
// Sets up a world + a view matrix.
// -----------------------------------------------------------------------------
//
void T3DModelx::MakeWorldViewMatrix(TCamerax & aCamera)
	{
	MakeWorldViewMatrix( aCamera, GetPosition(), GetYaw(), GetPitch(),
		GetRoll());
	}

// -----------------------------------------------------------------------------
// T3DModelx::MakeWorldViewMatrix
// Sets up a world + a view matrix.
// -----------------------------------------------------------------------------
//
void T3DModelx::MakeWorldViewMatrix(TCamerax & aCamera, TVectorx aPosition,
	GLfixed aYaw, GLfixed aPitch, GLfixed aRoll)
	{
    glMultMatrixx((GLfixed*)aCamera.GetViewMatrix());

    glTranslatex(aPosition.iX-aCamera.iPosition.iX,
		aPosition.iY-aCamera.iPosition.iY, aPosition.iZ-aCamera.iPosition.iZ);
	if ( aRoll != INT_2_FIXED(0) )
		{
		glRotatex( aRoll , INT_2_FIXED(0), INT_2_FIXED(0), INT_2_FIXED(1));
		}
	if ( aYaw != INT_2_FIXED(0) )
		{
		glRotatex( aYaw  , INT_2_FIXED(0), INT_2_FIXED(1), INT_2_FIXED(0));
		}
	if ( aPitch != INT_2_FIXED(0) )
		{
		glRotatex( aPitch, INT_2_FIXED(1), INT_2_FIXED(0), INT_2_FIXED(0));
		}
	}

// -----------------------------------------------------------------------------
// T3DModelx::MakeBlenderLiteWorldViewMatrix
// Sets up a blenderlite matrix, which is a matrix that rotates objects in such a
// way that they always face the camera.
// Refer to the blenderlite example to see how this method is used.
// -----------------------------------------------------------------------------
//
void T3DModelx::MakeBlenderLiteWorldViewMatrix(TCamerax & aCamera)
	{
	MakeBlenderLiteWorldViewMatrix( aCamera, GetPosition() );
	}

// -----------------------------------------------------------------------------
// T3DModelx::MakeBlenderLiteWorldViewMatrix
// Sets up a blenderlite matrix, which is a matrix that rotates objects in such a
// way that they always face the camera.
// Refer to the blenderlite example to see how this method is used.
// -----------------------------------------------------------------------------
//
void T3DModelx::MakeBlenderLiteWorldViewMatrix(TCamerax & aCamera,
	TVectorx aPosition)
	{
	// Set up a rotation matrix to orient the blenderlite towards the camera.
	TVectorx Dir = aCamera.iLookAt - aCamera.iPosition;

	GLfloat Angle = atan( FIXED_2_FLOAT(Dir.iZ), FIXED_2_FLOAT(Dir.iX) );
	// The Yaw angle is computed in such a way that the object always faces the camera.
	Angle = -(RAD_2_DEG( Angle ) + 90);

	MakeWorldViewMatrix( aCamera, aPosition, FLOAT_2_FIXED( Angle ) );
	}

// -----------------------------------------------------------------------------
// T3DModelx::SetYaw
// -----------------------------------------------------------------------------
//
void T3DModelx::SetYaw(GLfixed aYaw)
	{
	iYaw = aYaw;
	}
// -----------------------------------------------------------------------------
// T3DModelx::SetPitch
// -----------------------------------------------------------------------------
//
void T3DModelx::SetPitch(GLfixed aPitch)
	{
	iPitch = aPitch;
	}
// -----------------------------------------------------------------------------
// T3DModelx::SetRoll
// -----------------------------------------------------------------------------
//
void T3DModelx::SetRoll(GLfixed aRoll)
	{
	iRoll = aRoll;
	}
// -----------------------------------------------------------------------------
// T3DModelx::SetPosition
// -----------------------------------------------------------------------------
//
void T3DModelx::SetPosition(TVectorx aPosition)
	{
	iPosition = aPosition;
	}
// -----------------------------------------------------------------------------
// T3DModelx::GetYaw
// -----------------------------------------------------------------------------
//
GLfixed T3DModelx::GetYaw()
	{
	return iYaw;
	}
// -----------------------------------------------------------------------------
// T3DModelx::GetPitch
// -----------------------------------------------------------------------------
//
GLfixed T3DModelx::GetPitch()
	{
	return iPitch;
	}
// -----------------------------------------------------------------------------
// T3DModelx::GetRoll
// -----------------------------------------------------------------------------
//
GLfixed T3DModelx::GetRoll()
	{
	return iRoll;
	}
// -----------------------------------------------------------------------------
// T3DModelx::GetPosition
// -----------------------------------------------------------------------------
//
TVectorx T3DModelx::GetPosition()
	{
	return iPosition;
	}

// =============================================================================
// TCamera
// =============================================================================

// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// TCamera::TCamera
// C++ default constructor can NOT contain any code, that
// might leave.
// Constructs and initializes a TCamera to Position (0,0,0), LookAt (0,0,-1),
// Up(0,1,0).
// -----------------------------------------------------------------------------
//
TCamera::TCamera()
	{
	*this = TCamera(TVector(0, 0, 0), TVector(0, 0, -1), TVector(0, 1, 0));
	}

// -----------------------------------------------------------------------------
// TCamera::TCamera
// C++ constructor can NOT contain any code, that
// might leave.
// Constructs and initializes a TCamera to aPosition, aLookAt, aUp.
// -----------------------------------------------------------------------------
//
TCamera::TCamera(TVector aPosition, TVector aLookAt, TVector aUp)
	{
	LookAt(aPosition, aLookAt, aUp);
	}

// -----------------------------------------------------------------------------
// TCamera::LookAt
// Initializes a TCamera to aPosition, aLookAt, aUp.
// -----------------------------------------------------------------------------
//
void TCamera::LookAt(TVector aPosition, TVector aLookAt, TVector aUp)
	{
    TVector XAxis, YAxis, ZAxis;

	iPosition = aPosition;
	iLookAt   = aLookAt;
	iUp       = aUp;

    // Get the z basis vector, which points straight ahead; the
    // difference from the position (eye point) to the look-at point.
	// This is the direction of the gaze (+z).
	ZAxis = (iLookAt - iPosition);

    // Normalize the z basis vector.
	ZAxis.Normalize();

    // Compute the orthogonal axes from the cross product of the gaze
    // and the Up vector.
	XAxis = TVector::CrossProduct( ZAxis, iUp );
	XAxis.Normalize();
	YAxis = TVector::CrossProduct( XAxis, ZAxis );
	YAxis.Normalize();

    // Start building the matrix. The first three rows contain the
    // basis vectors used to rotate the view to point at the look-at point.
	MakeIdentity( &iViewMatrix[0][0] );

    iViewMatrix[0][0] =  XAxis.iX;
    iViewMatrix[1][0] =  XAxis.iY;
    iViewMatrix[2][0] =  XAxis.iZ;

    iViewMatrix[0][1] =  YAxis.iX;
    iViewMatrix[1][1] =  YAxis.iY;
    iViewMatrix[2][1] =  YAxis.iZ;

    iViewMatrix[0][2] = -ZAxis.iX;
    iViewMatrix[1][2] = -ZAxis.iY;
    iViewMatrix[2][2] = -ZAxis.iZ;
	}


// -----------------------------------------------------------------------------
// TCamera::GetPosition
// Get camera position vector.
// -----------------------------------------------------------------------------
//
TVector TCamera::GetPosition()
	{
	return iPosition;
	}

// -----------------------------------------------------------------------------
// TCamera::GetLookAt
// Get camera lookat position vector.
// -----------------------------------------------------------------------------
//
TVector TCamera::GetLookAt()
	{
	return iLookAt;
	}

// -----------------------------------------------------------------------------
// TCamera::GetUp
// Get camera "up" vector.
// -----------------------------------------------------------------------------
//
TVector TCamera::GetUp()
	{
	return iUp;
	}

// -----------------------------------------------------------------------------
// TCamera::GetViewMatrix
// Get camera matrix representation (world -> camera transform).
// -----------------------------------------------------------------------------
//
GLfloat* TCamera::GetViewMatrix()
	{
	return (GLfloat *)&iViewMatrix[0][0];
	}

// -----------------------------------------------------------------------------
// TCamera::MakeIdentity
// Initializes the matrix aMatrix to identity matrix.
// -----------------------------------------------------------------------------
//
void TCamera::MakeIdentity(GLfloat * aMatrix)
	{
    aMatrix[0 + 4 * 0] = 1.0f; aMatrix[0 + 4 * 1] = 0.0f;
	aMatrix[0 + 4 * 2] = 0.0f; aMatrix[0 + 4 * 3] = 0.0f;

	aMatrix[1 + 4 * 0] = 0.0f; aMatrix[1 + 4 * 1] = 1.0f;
	aMatrix[1 + 4 * 2] = 0.0f; aMatrix[1 + 4 * 3] = 0.0f;

	aMatrix[2 + 4 * 0] = 0.0f; aMatrix[2 + 4 * 1] = 0.0f;
	aMatrix[2 + 4 * 2] = 1.0f; aMatrix[2 + 4 * 3] = 0.0f;

	aMatrix[3 + 4 * 0] = 0.0f; aMatrix[3 + 4 * 1] = 0.0f;
	aMatrix[3 + 4 * 2] = 0.0f; aMatrix[3 + 4 * 3] = 1.0f;
	}


// =============================================================================
// TCamerax, fixed-point camera implementation.
// =============================================================================

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TCamerax::TCamerax
// C++ default constructor can NOT contain any code, that
// might leave.
// Constructs and initializes a TCamera to Position (0,0,0), LookAt (0,0,-1),
// Up(0,1,0).
// -----------------------------------------------------------------------------
//
TCamerax::TCamerax()
	{
	*this = TCamerax(TVectorx(INT_2_FIXED(0), INT_2_FIXED(0), INT_2_FIXED(0)),
					 TVectorx(INT_2_FIXED(0), INT_2_FIXED(0), INT_2_FIXED(-1)),
					 TVectorx(INT_2_FIXED(0), INT_2_FIXED(1), INT_2_FIXED(0)));
	}

// -----------------------------------------------------------------------------
// TCamerax::TCamerax
// C++ constructor can NOT contain any code, that
// might leave.
// Constructs and initializes a TCamera to aPosition, aLookAt, aUp.
// -----------------------------------------------------------------------------
//
TCamerax::TCamerax(TVectorx aPosition, TVectorx aLookAt, TVectorx aUp)
	{
	LookAt(aPosition, aLookAt, aUp);
	}

// -----------------------------------------------------------------------------
// TCamerax::LookAt
// Initializes a TCamera to aPosition, aLookAt, aUp.
// -----------------------------------------------------------------------------
//
void TCamerax::LookAt(TVectorx aPosition, TVectorx aLookAt, TVectorx aUp)
	{
    TVectorx XAxis, YAxis, ZAxis;

	iPosition = aPosition;
	iLookAt = aLookAt;
	iUp = aUp;

    // Get the z basis vector, which points straight ahead; the
    // difference from the position (eye point) to the look-at point.
	// This is the direction of the gaze (+z).
	ZAxis = (iLookAt - iPosition);

    // Normalize the z basis vector.
	ZAxis.Normalize();

    // Compute the orthogonal axes from the cross product of the gaze
    // and the Up vector.
	XAxis = TVectorx::CrossProduct(ZAxis, iUp);
	XAxis.Normalize();
	YAxis = TVectorx::CrossProduct(XAxis, ZAxis);
	YAxis.Normalize();

    // Start building the matrix. The first three rows contain the
    // basis vectors used to rotate the view to point at the look-at point.
	MakeIdentity(&iViewMatrix[0][0]);

    iViewMatrix[0][0] =  XAxis.iX;
    iViewMatrix[1][0] =  XAxis.iY;
    iViewMatrix[2][0] =  XAxis.iZ;

    iViewMatrix[0][1] =  YAxis.iX;
    iViewMatrix[1][1] =  YAxis.iY;
    iViewMatrix[2][1] =  YAxis.iZ;

    iViewMatrix[0][2] = -ZAxis.iX;
    iViewMatrix[1][2] = -ZAxis.iY;
    iViewMatrix[2][2] = -ZAxis.iZ;
	}

// -----------------------------------------------------------------------------
// TCamerax::GetPosition
// Get camera position vector.
// -----------------------------------------------------------------------------
//
TVectorx TCamerax::GetPosition()
	{
	return iPosition;
	}

// -----------------------------------------------------------------------------
// TCamerax::GetLookAt
// Get camera lookat position vector.
// -----------------------------------------------------------------------------
//
TVectorx TCamerax::GetLookAt()
	{
	return iLookAt;
	}

// -----------------------------------------------------------------------------
// TCamerax::GetUp
// Get camera "up" vector.
// -----------------------------------------------------------------------------
//
TVectorx TCamerax::GetUp()
	{
	return iUp;
	}

// -----------------------------------------------------------------------------
// TCamerax::GetViewMatrix
// Get camera matrix representation (world -> camera transform).
// -----------------------------------------------------------------------------
//
GLfixed* TCamerax::GetViewMatrix()
	{
	return (GLfixed *)&iViewMatrix[0][0];
	};


// -----------------------------------------------------------------------------
// TCamerax::MakeIdentity
// Initializes the matrix aMatrix to identity matrix.
// -----------------------------------------------------------------------------
//
void TCamerax::MakeIdentity(GLfixed * aMatrix)
	{
    aMatrix[0 + 4 * 0] = INT_2_FIXED(1); aMatrix[0 + 4 * 1] = INT_2_FIXED(0);
	aMatrix[0 + 4 * 2] = INT_2_FIXED(0); aMatrix[0 + 4 * 3] = INT_2_FIXED(0);

	aMatrix[1 + 4 * 0] = INT_2_FIXED(0); aMatrix[1 + 4 * 1] = INT_2_FIXED(1);
	aMatrix[1 + 4 * 2] = INT_2_FIXED(0); aMatrix[1 + 4 * 3] = INT_2_FIXED(0);

	aMatrix[2 + 4 * 0] = INT_2_FIXED(0); aMatrix[2 + 4 * 1] = INT_2_FIXED(0);
	aMatrix[2 + 4 * 2] = INT_2_FIXED(1); aMatrix[2 + 4 * 3] = INT_2_FIXED(0);

	aMatrix[3 + 4 * 0] = INT_2_FIXED(0); aMatrix[3 + 4 * 1] = INT_2_FIXED(0);
	aMatrix[3 + 4 * 2] = INT_2_FIXED(0); aMatrix[3 + 4 * 3] = INT_2_FIXED(1);
	}

// =============================================================================
// CParticleEngine
// =============================================================================

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CParticleEngine::CParticleEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CParticleEngine::CParticleEngine()
	{
	}


// -----------------------------------------------------------------------------
// CParticleEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// Constructs a CParticleEngine object with aParticlesCount particles at
// position aPosition.
// -----------------------------------------------------------------------------
//
void CParticleEngine::ConstructL( TInt aParticlesCount, TVector aPosition)
	{
    iParticlesCount = aParticlesCount;
    iParticles = new (ELeave) TParticle[iParticlesCount];

	SetPosition( aPosition );
	}

// Destructor

CParticleEngine::~CParticleEngine()
	{
	delete[] iParticles;
	}

// -----------------------------------------------------------------------------
// CParticleEngine::ResetEngine
// Resets the particle engine
// -----------------------------------------------------------------------------
//
void CParticleEngine::ResetEngine()
	{
    for ( GLint i = 0; i < iParticlesCount; i++ )
		{
        ResetParticle(i);
		}
	}

// -----------------------------------------------------------------------------
// CParticleEngine::GetPosition
// Get the position of this particle engine
// -----------------------------------------------------------------------------
//
TVector CParticleEngine::GetPosition()
	{
	return iPosition;
	}

// -----------------------------------------------------------------------------
// CParticleEngine::GetParticleCount
// Get the number of particles in this effect.
// -----------------------------------------------------------------------------
//
GLint CParticleEngine::GetParticleCount()
	{
	return iParticlesCount;
	}

// -----------------------------------------------------------------------------
// CParticleEngine::SetPosition
// Set the position of this effect.
// -----------------------------------------------------------------------------
//
void CParticleEngine::SetPosition( TVector & aPosition )
	{
	iPosition = aPosition;
	}


// =============================================================================
// CLookUpTable
// =============================================================================

// -----------------------------------------------------------------------------
// CLookUpTable::CLookUpTable
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLookUpTable::CLookUpTable()
	{
	}

// -----------------------------------------------------------------------------
// CLookUpTable::ConstructL
// Symbian 2nd phase constructor can leave.
// Computes the lookup-tables according to the flags input.
// -----------------------------------------------------------------------------
//
void CLookUpTable::ConstructL(TInt aFlags)
	{
	if ( aFlags & ESin )
		{
		ComputeSinTableL();
		}
	if ( aFlags & ECos )
		{
		ComputeCosTableL();
		}
	if ( aFlags & ETan )
		{
		ComputeTanTableL();
		}

	if ( aFlags & ESinx )
		{
		ComputeSinxTableL();
		}
	if ( aFlags & ECosx )
		{
		ComputeCosxTableL();
		}
	if ( aFlags & ETanx )
		{
		ComputeTanxTableL();
		}
	}

// -----------------------------------------------------------------------------
// CLookUpTable::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CLookUpTable* CLookUpTable::NewL(TInt aFlags)
	{
	CLookUpTable* self = new (ELeave) CLookUpTable;
	CleanupStack::PushL( self );
	self->ConstructL(aFlags);
	CleanupStack::Pop();

	return self;
	}

// Destructor

CLookUpTable::~CLookUpTable()
	{
	delete[] iSinTable;
	delete[] iCosTable;
	delete[] iTanTable;

	delete[] iSinxTable;
	delete[] iCosxTable;
	delete[] iTanxTable;
	}

// -----------------------------------------------------------------------------
// CLookUpTable::ComputeSinTableL
// Precomputes the floating-point sine-table
// -----------------------------------------------------------------------------
//
void CLookUpTable::ComputeSinTableL()
	{
	_LIT(KSinErr, "ComputeSinTableL()");
	iSinTable = new GLfloat[360];
	if ( iSinTable == NULL )
		{
		User::Panic( KSinErr, 0 );
		}

	for ( GLushort i = 0; i < 360; i++ )
		{
		TReal sin;
		TReal alpha = DEG_2_RAD( i );

		User::LeaveIfError( Math::Sin( sin, alpha ) );
		iSinTable[i] = (GLfloat) sin;
		}
	}

// -----------------------------------------------------------------------------
// CLookUpTable::ComputeCosTableL
// Precomputes the floating-point cosine-table
// -----------------------------------------------------------------------------
//
void CLookUpTable::ComputeCosTableL()
	{
	_LIT(KCosErr, "ComputeCosTableL()");
	iCosTable = new GLfloat[360];
	if ( iCosTable == NULL )
		{
		User::Panic( KCosErr, 0 );
		}

	for ( GLushort i = 0; i < 360; i++ )
		{
		TReal cos;
		TReal alpha = DEG_2_RAD(i);

		User::LeaveIfError(Math::Cos(cos, alpha));
		iCosTable[i] = (GLfloat)cos;
		}
	}

// -----------------------------------------------------------------------------
// CLookUpTable::ComputeTanTableL
// Precomputes the floating-point tan-table
// -----------------------------------------------------------------------------
//
void CLookUpTable::ComputeTanTableL()
	{
	_LIT(KTanErr, "ComputeTanTableL()");
	iTanTable = new GLfloat[360];
	if ( iTanTable == NULL )
		{
		User::Panic( KTanErr, 0 );
		}

	for ( GLushort i = 0; i < 360; i++ )
		{
		TReal tan;
		TReal alpha = DEG_2_RAD(i);

		User::LeaveIfError(Math::Tan(tan, alpha));
		iTanTable[i] = (GLfloat)tan;
		}
	}

// -----------------------------------------------------------------------------
// CLookUpTable::ComputeSinxTableL
// Precomputes the fixed-point sin-table
// -----------------------------------------------------------------------------
//
void CLookUpTable::ComputeSinxTableL()
{
	_LIT(KSinErr, "ComputeSinxTableL()");
	iSinxTable = new GLfixed[360];
	if ( iSinxTable == NULL )
		{
		User::Panic( KSinErr, 0 );
		}

	for ( GLushort i = 0; i < 360; i++ )
		{
		TReal sin;
		TReal alpha = DEG_2_RAD(i);

		User::LeaveIfError(Math::Sin(sin, alpha));
		iSinxTable[i] = FLOAT_2_FIXED((GLfloat)sin);
		}
}

// -----------------------------------------------------------------------------
// CLookUpTable::ComputeCosxTableL
// Precomputes the fixed-point cos-table
// -----------------------------------------------------------------------------
//
void CLookUpTable::ComputeCosxTableL()
{
	_LIT(KCosErr, "ComputeCosxTableL()");
	iCosxTable = new GLfixed[360];
	if ( iCosxTable == NULL )
		{
		User::Panic( KCosErr, 0 );
		}

	for ( GLushort i = 0; i < 360; i++ )
		{
		TReal cos;
		TReal alpha = DEG_2_RAD(i);

		User::LeaveIfError(Math::Cos(cos, alpha));
		iCosxTable[i] = FLOAT_2_FIXED((GLfloat)cos);
		}
}

// -----------------------------------------------------------------------------
// CLookUpTable::ComputeTanxTableLs
// Precomputes the fixed-point cos-table
// -----------------------------------------------------------------------------
//
void CLookUpTable::ComputeTanxTableL()
{
	_LIT(KTanErr, "ComputeTanxTableL()");
	iTanxTable = new GLfixed[360];
	if ( iTanxTable == NULL )
		{
		User::Panic( KTanErr, 0 );
		}

	for ( GLushort i = 0; i < 360; i++ )
		{
		TReal tan;
		TReal alpha = DEG_2_RAD(i);

		User::LeaveIfError(Math::Tan(tan, alpha));
		iTanxTable[i] = FLOAT_2_FIXED((GLfloat)tan);
		}
}

// -----------------------------------------------------------------------------
// CLookUpTable::Sin
// Fetch floating-point sin from the lookup-table
// -----------------------------------------------------------------------------
//
GLfloat CLookUpTable::Sin(GLushort aAngle)
	{
	return iSinTable[aAngle];
	}

// -----------------------------------------------------------------------------
// CLookUpTable::Cos
// Fetch floating-point cos from the lookup-table
// -----------------------------------------------------------------------------
//
GLfloat CLookUpTable::Cos(GLushort aAngle)
	{
	return iCosTable[aAngle];
	}

// -----------------------------------------------------------------------------
// CLookUpTable::Tan
// Fetch floating-point tan from the lookup-table
// -----------------------------------------------------------------------------
//
GLfloat CLookUpTable::Tan(GLushort aAngle)
	{
	return iTanTable[aAngle];
	}

// -----------------------------------------------------------------------------
// CLookUpTable::Sinx
// Fetch fixed-point sin from the lookup-table
// -----------------------------------------------------------------------------
//
GLfixed CLookUpTable::Sinx(GLfixed aAngle)
	{
    GLfixed v1 = iSinxTable[FIXED_2_INT(aAngle)];
    GLfixed v2 = iSinxTable[(FIXED_2_INT(aAngle) + 1) % 360];
    GLfixed c = aAngle & (FIXED_ONE - 1);
    return (v1 + fixedMul((v2 - v1), c));
	}

// -----------------------------------------------------------------------------
// CLookUpTable::Cosx
// Fetch fixed-point cos from the lookup-table
// -----------------------------------------------------------------------------
//
GLfixed CLookUpTable::Cosx(GLfixed aAngle)
	{
    GLfixed v1 = iCosxTable[FIXED_2_INT(aAngle)];
    GLfixed v2 = iCosxTable[(FIXED_2_INT(aAngle) + 1) % 360];
    GLfixed c = aAngle & (FIXED_ONE - 1);
    return (v1 + fixedMul((v2 - v1), c));
	}

// -----------------------------------------------------------------------------
// CLookUpTable::Tanx
// Fetch fixed-point tan from the lookup-table
// -----------------------------------------------------------------------------
//
GLfixed CLookUpTable::Tanx(GLfixed aAngle)
	{
	return iTanxTable[FIXED_2_INT(aAngle)];
	}

// =============================================================================
// =============================================================================
// Other miscallenous helper functions

// -----------------------------------------------------------------------------
// utilOutputText
// Outputs a text at a given position with a given color and font.
// -----------------------------------------------------------------------------
//
void utilOutputText(CWindowGc & aGc, const TDesC &aString,
	const TPoint &aPosition, const TRgb &aColor, const CFont *aFont )
	{
	aGc.SetPenColor(aColor);
	aGc.UseFont(aFont);
	aGc.DrawText(aString, aPosition);
	}

// -----------------------------------------------------------------------------
// utilOutputText
// Outputs a text at a given position with a given color using the
// CEikonEnv::Static()->SymbolFont() font.
// -----------------------------------------------------------------------------
//
void utilOutputText(CWindowGc & aGc, const TDesC &aString,
	const TPoint &aPosition, const TRgb &aColor )
	{
	utilOutputText(aGc, aString, aPosition, aColor, CEikonEnv::Static()->SymbolFont());
	}

