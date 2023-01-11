/*
 * ==============================================================================
 *  Name        : Utils3d.h
 *
 *  Copyright (c) 2004-2006 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation.
 * ==============================================================================
 */

#ifndef __UTILS3D__
#define __UTILS3D__

//  INCLUDE FILES

#include <fbs.h>
#include <e32base.h>
#include <e32std.h>
#include <e32math.h>
#include <w32std.h>
#include <eikenv.h>
#include <GLES/gl.h>      // OpenGL ES header file
#include <ImageConversion.h>

#include "Mathutils.h"
#include "Textureutils.h"

// MACROS

// FORWARD DECLARATIONS
class TCamera;
class TCamerax;

/**
 * 3D vector that is presented by x,y,z coordinates using single-precision floating point math.
 */
class TVector
	{
	public: // Constructors

		/**
		 * Constructs and initializes a TVector to (0,0,0).
		 */
        TVector();

		/**
		 * Constructs and initializes a TVector from the specified x,y and z coordinates.
		 * @param aX Vector x component
		 * @param aY Vector y component
		 * @param aZ Vector z component
		 */
		TVector(GLfloat aX, GLfloat aY, GLfloat aZ );

	public: // New functions

		/**
		 * Computes the sum of this vector and the given vector.
		 * @param aVector Right hand side vector
		 * @return Sum of this vector and the given vector.
		 */
		TVector operator + (TVector aVector) const;

		/**
 		 * Increments this vector by the given vector.
         * @param aVector Vector to be added to this vector.
 		 */
		void operator += (TVector aVector);

		/**
		 * Computes the difference of this vector and the given vector.
		 * @param aVector Right hand side vector
		 * @return Difference of this vector and the given vector.
		 */
		TVector operator - (TVector aVector) const;

		/**
		 * Computes the negation of this vector.
		 * @return Negated vector.
		 */
		TVector operator - () const;

		/**
		 * Computes the scalar product of this vector and the given vector.
		 * @param aVector Right hand side vector
		 * @return Scalar prouduct of this vector and the given vector.
		 */
		GLfloat operator * (TVector aVector) const;

		/**
		 * Computes the product of this vector and the given scalar value.
		 * @param aScalar Scalar value.
		 * @return Multiplied vector.
		 */
		TVector operator * ( GLfloat aScalar ) const;

		/**
		 * Computes the magnitude (length) of this vector.
		 * @return Magnitude of this vector.
		 */
		GLfloat Magnitude();

		/**
		 * Normalizes this vector, Panics if this vector = (0, 0, 0)
		 */
		void Normalize();

        /**
         * Computes the multiplication of this vector with the given matrix.
         * Basically transforms the this vector with the given transformation matrix.
         * @param aMatrix 4 times 4 transformation matrix.
         */
        void MultMatrix4x4(const GLfloat aMatrix[]);

        /**
         * Computes the multiplication of given vector with the given matrix.
         * Basically transforms the vector with the given transformation matrix.
         * @param aVector Vector to be transformed.
         * @param aMatrix 4 times 4 transformation matrix.
         * @return Transformed vector.
         */
        static TVector MultMatrix4x4(const TVector aVector, const GLfloat aMatrix[]);

		/**
		 * Computes the crossproduct of the two given vectors.
		 * @param aVector1 Left hand side vector.
		 * @param aVector2 Right hand side vector.
		 * @return Crossproduct of aVector1 and aVector2.
		 */
		static TVector CrossProduct(const TVector aVector1, const TVector aVector2);

	public: // Data
		/** X coordinate. */
		GLfloat iX;
		/** Y coordinate. */
		GLfloat iY;
		/** Z coordinate. */
		GLfloat iZ;
	};

// =============================================================================
// =============================================================================

/**
 * 3D vector that is represented by x,y,z coordinates using fixed-point math.
 */
class TVectorx
	{
	public: // Constructors
		/** Constructs and initializes a TVectorx to (0,0,0). */
		TVectorx();

		/** Constructs and initializes a TVectorx from the specified x, y and z coordinates. */
		TVectorx(GLfixed aX, GLfixed aY, GLfixed aZ );
	public: // New functions

		/**
		 * Computes the sum of this vector and the given vector.
		 * @param aVector Right hand side vector
		 * @return Sum of this vector and the given vector.
		 */
		TVectorx operator + (TVectorx aVector) const;

		/**
 		 * Increments this vector by the given vector.
         * @param aVector Vector to be added to this vector.
 		 */
		void operator += (TVectorx aVector);

		/**
		 * Computes the difference of this vector and the given vector.
		 * @param aVector Right hand side vector
		 * @return Difference of this vector and the given vector.
		 */
		TVectorx operator - (TVectorx aVector) const;

		/**
		 * Computes the negation of this vector.
		 * @return Negated vector.
		 */
		TVectorx operator - () const;

		/**
		 * Computes the scalar product of this vector and the given vector.
		 * @param aVector Right hand side vector
		 * @return Scalar prouduct of this vector and the given vector.
		 */
		GLfixed operator * (TVectorx aVector) const;


		/**
		 * Computes the product of this vector and the given scalar value.
		 * @param aScalar Scalar value.
		 * @return Multiplied vector.
		 */
		TVectorx operator * (GLfixed aScalar) const;

		/**
		 * Computes the magnitude (length) of this vector.
		 * @return Magnitude of this vector.
		 */
		GLfixed Magnitude() const;

		/**
		 * Normalizes this vector, Panics if this vector = (0, 0, 0)
		 */
		void Normalize();

		/**
		 * Computes the crossproduct of the two given vectors.
		 * @param aVector1 Left hand side vector.
		 * @param aVector2 Right hand side vector.
		 * @return Crossproduct of aVector1 and aVector2.
		 */
		static TVectorx CrossProduct(TVectorx aVector1, TVectorx aVector2);

	public: // Data
		/** X coordinate. */
		GLfixed iX;
		/** Y coordinate. */
		GLfixed iY;
		/** Z coordinate. */
		GLfixed iZ;
	};

// =============================================================================
// =============================================================================

/**
 * Class for storing 3D model position and orientation using floating point math.
 * The position vector (a TVector) tells where the pivot point of the model is. The single precision
 * (GLfloat) values yaw, pitch and roll tell the models orientation around the model's pivot point.
 */
class T3DModel
	{
	public: // Constructors
		/**
		 * Constructs and initializes a T3DModel to Position (0,0,0), with orientation [Yaw=0, Pitch=0, Roll=0].
		 */
		T3DModel();

		/**
		 * Constructs and initializes a T3DModel to given position and with given orientation.
		 * @param aPosition Position vector that tells where the model's pivot point is located at.
		 * @param aYaw �aw of the model around the pivot point.
		 * @param aPitch Pitch of the model around the pivot point.
		 * @param aRoll Roll of the model around the pivot point.
		 */
		T3DModel(TVector aPosition, GLfloat aYaw, GLfloat aPitch, GLfloat aRoll);

	public: // New functions

		/**
		 * Set yaw of the model around the model's pivot point.
		 * @param aYaw Yaw of the model.
		 */
		void SetYaw(GLfloat aYaw);

		/**
		 * Set pitch of the model around the model's pivot point.
		 * @param aPitch Pitch of the model.
		 */
		void SetPitch(GLfloat aPitch);

		/**
		 * Set roll of the model around the model's pivot point.
		 * @param aRoll Roll of the model.
		 */
		void SetRoll(GLfloat aRoll);

		/**
		 * Set position of the model's pivot point.
		 * @param aPosition Position of the model.
		 */
		void SetPosition(TVector aPosition);

		/**
		 * Returns the yaw of the model around the model's pivot point.
		 * @return Yaw of the model.
		 */
		GLfloat GetYaw();

		/**
		 * Returns the pitch of the model around the model's pivot point.
		 * @return Pitch of the model.
		 */
		GLfloat GetPitch();

		/**
		 * Returns the roll of the model around the model's pivot point.
		 * @return Roll of the model.
		 */
		GLfloat GetRoll();

		/**
		 * Returns the position of the model's pivot point.
		 * @return Position of the model.
		 */
		TVector GetPosition();

		/**
		 * Helper method that sets up the current Open GL's matrix (normally this should be set to
		 * be the worldview matrix for this method to work correctly) so that the object is located at the given
		 * position and has the given yaw, pitch and roll.
		 * @param aCamera Target camera.
		 * @param aPosition Model's position.
		 * @param aYaw Model's yaw.
		 * @param aPitch Model's pitch.
		 * @param aRoll Model's roll.
		 */
		static void MakeWorldViewMatrix( TCamera &aCamera, TVector aPosition,
			GLfloat aYaw = 0.0f, GLfloat aPitch = 0.0f, GLfloat aRoll = 0.0f);
		/**
		 * Sets up the current Open GL's matrix (normally this should be set to be the worldview matrix
		 * for this method to work correctly) so that this object is located at it's current position and
		 * has the current yaw, pitch and roll.
		 * @param aCamera Target camera.
		 */
		void MakeWorldViewMatrix(TCamera &aCamera);

		/**
		 * Helper method that sets up the current Open GL's matrix (normally this should be set to
		 * be the worldview matrix for this method to work correctly) so that the object at the given
		 * position always faces the camera.
		 * @param aCamera Target camera.
		 * @param aPosition Model's position.
		 */
		static void MakeBlenderLiteWorldViewMatrix(TCamera &aCamera, TVector aPosition);

		/**
 		* Sets up a blenderlite matrix, which is a matrix that rotates objects in such a way that they always
		* face the camera.
		* Refer to the blenderlite example to see how this method is used.
		* Uses member data to position the blenderlite.
		* @param aCamera Targer camera.
		*/

		/**
		 * Sets up the current Open GL's matrix (normally this should be set to be the worldview matrix
		 * for this method to work correctly) so that this object at it's current position always faces
		 * the camera. Note that this method doesn't modify the current yeaw, pitch and roll of this object.
		 * @param aCamera Target camera.
		 */
		void MakeBlenderLiteWorldViewMatrix(TCamera &aCamera);

	protected: // Data
		/** Position of the model's pivot point. */
		TVector iPosition;
		/** Yaw of the model around it's pivot point. */
		GLfloat iYaw;
		/** Pitch of the model around it's pivot point. */
		GLfloat iPitch;
		// Roll.
		/** Roll of the model around it's pivot point. */
		GLfloat iRoll;
	};

// =============================================================================
// =============================================================================
/**
 * Class for storing 3D model position and orientation using fixed-point math.
 * The position vector (a TVectorx) tells where the pivot point of the model is. The fixed-point
 * (GLfixed) values yaw, pitch and roll tell the models orientation around the model's pivot point.
 */
class T3DModelx
	{

	public: // Constructors

		/**
		 * Constructs and initializes a T3DModel to Position (0,0,0), with orientation [Yaw=0, Pitch=0, Roll=0].
		 */
		T3DModelx();

		/**
		 * Constructs and initializes a T3DModelx to given position and with given orientation.
		 * @param aPosition Position vector that tells where the model's pivot point is located at.
		 * @param aYaw �aw of the model around the pivot point.
		 * @param aPitch Pitch of the model around the pivot point.
		 * @param aRoll Roll of the model around the pivot point.
		 */
		T3DModelx(TVectorx aPosition, GLfixed aYaw, GLfixed aPitch, GLfixed aRoll);

	public: // New functions

		/**
		 * Set yaw of the model around the model's pivot point.
		 * @param aYaw Yaw of the model.
		 */
		void SetYaw(GLfixed aYaw);

		/**
		 * Set pitch of the model around the model's pivot point.
		 * @param aPitch Pitch of the model.
		 */
		void SetPitch(GLfixed aPitch);

		/**
		 * Set roll of the model around the model's pivot point.
		 * @param aRoll Roll of the model.
		 */
		void SetRoll(GLfixed aRoll);

		/**
		 * Set position of the model's pivot point.
		 * @param aPosition Position of the model.
		 */
		void SetPosition(TVectorx aPosition);

		/**
		 * Returns the yaw of the model around the model's pivot point.
		 * @return Yaw of the model.
		 */
		GLfixed GetYaw();

		/**
		 * Returns the pitch of the model around the model's pivot point.
		 * @return Pitch of the model.
		 */
		GLfixed GetPitch();

		/**
		 * Returns the roll of the model around the model's pivot point.
		 * @return Roll of the model.
		 */
		GLfixed GetRoll();

		/**
		 * Returns the position of the model's pivot point.
		 * @return Position of the model.
		 */
		TVectorx GetPosition();

		/**
		 * Helper method that sets up the current Open GL's matrix (normally this should be set to
		 * be the worldview matrix for this method to work correctly) so that the object is located at the given
		 * position and has the given yaw, pitch and roll.
		 * @param aCamera Target camera.
		 * @param aPosition Model's position.
		 * @param aYaw Model's yaw.
		 * @param aPitch Model's pitch.
		 * @param aRoll Model's roll.
		 */
		static void MakeWorldViewMatrix(TCamerax &aCamera, TVectorx aPosition,
			GLfixed aYaw = INT_2_FIXED(0), GLfixed aPitch = INT_2_FIXED(0),
			GLfixed aRoll = INT_2_FIXED(0));

		/**
		 * Sets up the current Open GL's matrix (normally this should be set to be the worldview matrix
		 * for this method to work correctly) so that this object is located at it's current position and
		 * has the current yaw, pitch and roll.
		 * @param aCamera Target camera.
		 */
		void MakeWorldViewMatrix(TCamerax &aCamera);

		/**
		 * Helper method that sets up the current Open GL's matrix (normally this should be set to
		 * be the worldview matrix for this method to work correctly) so that the object at the given
		 * position always faces the camera.
		 * @param aCamera Target camera.
		 * @param aPosition Model's position.
		 */
		static void MakeBlenderLiteWorldViewMatrix(TCamerax &aCamera, TVectorx aPosition);

		/**
		 * Sets up the current Open GL's matrix (normally this should be set to be the worldview matrix
		 * for this method to work correctly) so that this object at it's current position always faces
		 * the camera. Note that this method doesn't modify the current yeaw, pitch and roll of this object.
		 * @param aCamera Target camera.
		 */
		void MakeBlenderLiteWorldViewMatrix(TCamerax &aCamera);

	protected: // Data

		/** Position of the model's pivot point. */
		TVectorx iPosition;
		/** Yaw of the model around it's pivot point. */
		GLfixed iYaw;
		/** Pitch of the model around it's pivot point. */
		GLfixed iPitch;
		/** Roll of the model around it's pivot point. */
		GLfixed iRoll;
	};

// =============================================================================
// =============================================================================
/**
 * Helper class that implements a camera in 3D space using single precision floating point math.
 * The camera is represented by an eye point vector, "look-at" direction vector and an up vector.
 * This class is very useful since it provides an implementation of the gluLookAt() method
 * which is not part of the OpenGL ES specification.
 */
class TCamera
	{
	public: // Constructors

		/**
		 * Constructs and initializes TCamera to Position (0,0,0), LookAt (0,0,-1), Up(0,1,0).
		 * This equals the default model view matrix setting of OpenGL (camera located at origo, looking towards
		 * negative Z-axis and up direction towards positive Y-axis).
		 */
		TCamera();

		/**
		 * Constructs and initializes TCamera using the given position, look at direction and up orientation vector.
		 * @param aPosition Position of the camera.
		 * @param aLookAt "Look-at" direction of the camera.
		 * @param aUp "Up" vector of the camera.
		 */
		TCamera(TVector aPosition, TVector aLookAt, TVector aUp);

	public: // New functions

		/**
		 * Returns the position of this camera.
		 * @return Position of this camera.
		 */
		TVector GetPosition();

		/**
		 * Returns the "look-at" direction of this camera.
		 * @return "Look-at" direction of this camera.
		 */
		TVector GetLookAt();

		/**
		 * Returns the "up" direction of this camera.
		 * @return "Up" direction of this camera.
		 */
		TVector GetUp();

		/**
		 * Returns the 4x4 model-view matrix representation of this camera.
		 * @return Pointer to 4x4 model-view matrix representation of this camera.
		 */
		GLfloat * GetViewMatrix();

		/**
		 * Resets the given 4x4 matrix to identity matrix.
		 * @param aMatrix Target matrix to be reset to indentity matrix.
		 */
		static void MakeIdentity(GLfloat * aMatrix);

		/**
		 * Sets the camera to use the given position, look-at direction and up direction vectors.
		 * This is NOT an implementation of the gluLookAt() function. This method DOES NOT place
		 * the translation of the camera position into the view matrix. The position must be applied
		 * separately after applying the view matrix.
		 * E.g. glTranslatef( -iCamera->getPosition().iX, -iCamera->getPosition().iY, -iCamera->getPosition().iZ )
		 *
		 * @param aPosition Position of the camera.
		 * @param aLookAt "Look-at" direction of the camera.
		 * @param aUp "Up" vector of the camera.
		 */
		void LookAt(TVector aPosition, TVector aLookAt, TVector aUp);

	protected: // Data
		/** Position of the camera's eye point. */
		TVector iPosition;
		/** Position of the "look-at" direction point. */
		TVector iLookAt;
		/** Direction of the up vector. */
		TVector iUp;
		/** View matrix. */
		GLfloat iViewMatrix[4][4];


	public: // Friend classes
		friend class T3DModel;
	};


// =============================================================================
// =============================================================================

/**
 * Helper class that implements a camera in 3D space using fixed-point math.
 * The camera is represented by an eye point vector, "looking towards" direction vector and an up vector.
 * This class is very useful since it provides an implementation of the gluLookAt() method
 * which is not part of the OpenGL ES specification.
 */
class TCamerax
	{

	public: // Constructors

		/**
 		 * Constructs and initializes a TCamerax to Position (0,0,0), LookAt (0,0,-1), Up(0,1,0).
		 * This equals the default model view matrix setting of OpenGL (camera located at origo, looking towards
		 * negative Z-axis and up direction towards positive Y-axis).
		 */
		TCamerax();

		/**
		 * Constructs and initializes TCamera using the given position, look at direction and up orientation vector.
		 * @param aPosition Position of the camera.
		 * @param aLookAt "Look-at" direction of the camera.
		 * @param aUp "Up" vector of the camera.
		 */
		TCamerax(TVectorx aPosition, TVectorx aLookAt, TVectorx aUp);

	public: // New functions

		/**
		 * Returns the position of the camera.
		 * @return Position of the camera.
		 */
		TVectorx GetPosition();

		/**
		 * Returns the "look-at" direction of the camera.
		 * @return "Look-at" direction of the camera.
		 */
		TVectorx GetLookAt();

		/**
		 * Returns the "up" direction of the camera.
		 * @return "Up" direction of the camera.
		 */
		TVectorx GetUp();

		/**
		 * Returns the 4x4 model-view matrix representation of this camera.
		 * @return Pointer to 4x4 model-view matrix representation of this camera.
		 */
		GLfixed * GetViewMatrix();

		/**
		 * Resets the given 4x4 matrix to identity matrix.
		 * @param aMatrix Target matrix to be reset to indentity matrix.
		 */
		static void MakeIdentity(GLfixed * aMatrix);

		/**
		 * Sets the camera to use the given position, look-at direction and up direction vectors.
		 * This is NOT an implementation of the gluLookAt() function. This method DOES NOT place
		 * the translation of the camera position into the view matrix. The position must be applied
		 * separately after applying the view matrix.
		 * E.g. glTranslatex( -iCamera->getPosition().iX, -iCamera->getPosition().iY, -iCamera->getPosition().iZ )
		 *
		 * @param aPosition Position of the camera.
		 * @param aLookAt "Look-at" direction of the camera.
		 * @param aUp "Up" vector of the camera.
		 */
		void LookAt(TVectorx aPosition, TVectorx aLookAt, TVectorx aUp);

	protected: // Data

		/** Position of the camera's eye point. */
		TVectorx iPosition;
		/** Position of the "look-at" direction point. */
		TVectorx iLookAt;
		/** Direction of the up vector. */
		TVectorx iUp;
		/** View matrix. */
		GLfixed iViewMatrix[4][4];

	public: // Friend classes
		friend class T3DModelx;
	};


// =============================================================================
// =============================================================================
/**
 *  Structure used internally by the class CParticleEngine.
 *  Represents a single particle.
 */
struct TParticle
	{
	/** Current position of the particle. */
    TVector iPosition;
	/** Current velocity and movement direction of the particle. */
    TVector iVelocity;
	/** Current acceleration vector of the particle. */
    TVector iAcceleration;
	};

// =============================================================================
// =============================================================================

/**
 *  Particle engine that can be used to create special effects like Rain, Smoke, Snow, Sparks, etc...
 */
class CParticleEngine : public CBase
	{
	public: // Destructor

		//Destructor
		virtual ~CParticleEngine();

	public: // New functions

		/**
		* Resets the particle engine
		*/
		void ResetEngine();
		/**
		* Resets the particle at index aIndex
		* @param aIndex Index to particle
		*/
		virtual void ResetParticle(GLint aIndex) = 0;
		/**
		* Updates the engine.
		* @param aElapsedTime Elapsed time.
		*/
		virtual void UpdateEngine(GLfloat aElapsedTime) = 0;
		/**
		* Renders the system.
		* @param aCamera Camera
		*/
		virtual void RenderEngine(TCamera &aCamera) = 0;

		/**
		* Gets the position of the effect
		* @return Position
		*/
		TVector GetPosition();
		/**
		* Get the number of particles
		* @return Number of particles
		*/
		GLint GetParticleCount();

		/**
		* Set the effect position
		* @param aPosition Effect position.
		*/
		void SetPosition( TVector & aPosition );

	protected:
		/**
		 * Default C++ and Symbian ConstructL are placed in protected
		 * area to force the use of NewL when creating an instance.
		 * Does nothing in this implementation.
		 */
		CParticleEngine();

		/**
		 * Symbian 2nd phase constructor that can Leave. Constructs a CParticleEngine
		 * object with given number of particles at given position.
		 * @param aParticlesCount Number of particles this particle engine can have.
		 * @param aPosition Position of the particle source.
		 */
		void ConstructL( TInt aParticlesCount, TVector aPosition);

	protected: // Data
		/** Array of the particles owned by this particle engine. */
		TParticle *iParticles;
		/** Lenght of iParticles. */
		GLint iParticlesCount;
		/** Position of the particle source, where all the particles are emanated from. */
		TVector iPosition;
	};

// =============================================================================
// =============================================================================

/**
 *  A lookup-table class for trigonometric (sin, cos and tan) functions.
 */
class CLookUpTable: public CBase
{
	public:
		enum
			{
			ESin    =  1,
			ECos    =  2,
			ETan    =  4,
			ESinx   =  8,
			ECosx   = 16,
			ETanx   = 32
			};

	public: // Constructors and destructor
        /**
         * Destructor. Destroys all the look-up tables.
         */
		virtual ~CLookUpTable();

		/**
		 * Factory method that constructs and initializes a the look up table object.
		 * @param Bit flags that defines which look-up tables are initialized. Value should be an or'ed value of the enumerated
		 *        values of ESin, ECos, ETan, ESinx, ECosx or ETanx.
		 * @return An instance of the look-up table object that has the given tables initialized.
		 */
		static CLookUpTable* NewL(TInt aFlags);

	protected:
		/*
		 * Default C++ and Symbian ConstructL are placed in protected
		 * area to force the use of NewL when creating an instance.
		 * Does nothing in this implementation.
		 */
		CLookUpTable();

		/**
		 * Constructs and initializes the given look up tables.
		 * @param Bit flags that defines which look-up tables are initialized. Value should be an or'ed value of the enumerated
		 *        values of ESin, ECos, ETan, ESinx, ECosx or ETanx.
		 * @return An instance of the look-up table object that has the given tables initialized.
		 */
		void ConstructL(TInt aFlags);

	public: // New functions

		/**
		* Look up a sine of an angle (floating point)
		* @param aAngle Input angle
		* @return Sine of the angle
		*/
		GLfloat Sin(GLushort aAngle);
		/**
		* Look up a cosine of an angle (floating point)
		* @param aAngle Input angle
		* @return Cosine of the angle
		*/
		GLfloat Cos(GLushort aAngle);
		/**
		* Look up a tan of an angle (floating point)
		* @param aAngle Input angle
		* @return Tan of the angle
		*/
		GLfloat Tan(GLushort aAngle);

		/**
		* Look up a sine of an angle (fixed-point)
		* @param aAngle Input angle
		* @return Sine of the angle
		*/
		GLfixed Sinx(GLfixed aAngle);
		/**
		* Look up a cosine of an angle (fixed-point)
		* @param aAngle Input angle
		* @return Cosine of the angle
		*/
		GLfixed Cosx(GLfixed aAngle);
		/**
		* Look up a tan of an angle (fixed-point)
		* @param aAngle Input angle
		* @return Tan of the angle
		*/
		GLfixed Tanx(GLfixed aAngle);

	protected: // New functions
		/**
		 * Calculates a floating point sine values lookup-table.
		 */
		void ComputeSinTableL();
		/**
		 * Calculates a floating point cosine values lookup-table.
 		 */
		void ComputeCosTableL();
		/**
		 * Calculates a floating point tangent values lookup-table.
		 */
		void ComputeTanTableL();

		/**
		 * Calculates a fixed-point sine values lookup-table.
		 */
		void ComputeSinxTableL();
		/**
		 * Calculates a fixed-point cosine values lookup-table.
 		 */
		void ComputeCosxTableL();
		/**
		 * Calculates a fixed-point tangent values lookup-table.
		 */
		void ComputeTanxTableL();

	protected: // Data

		/** Sine floating point value look-up table. */
		GLfloat * iSinTable;
		/** Cosine floating point value look-up table. */
		GLfloat * iCosTable;
		/** Tangent floating point value look-up table. */
		GLfloat * iTanTable;
		/** Sine fixed-point value look-up table. */
		GLfixed * iSinxTable;
		/** Cosine fixed-point value look-up table. */
		GLfixed * iCosxTable;
		/** Tangent fixed-point value look-up table. */
		GLfixed * iTanxTable;
};

// =============================================================================
// =============================================================================
// Other miscallenous helper functions

/**
 * Outputs a text at a given position with a given color and font.
 * @param aGc Window graphics context.
 * @param aString String to be rendered.
 * @param aPosition xy-position of the text to be rendered.
 * @param aColor RGB color of the text.
 * @param aFont Font to be used in rendering.
 */
void utilOutputText(CWindowGc & aGc, const TDesC &aString,
	const TPoint &aPosition, const TRgb &aColor, const CFont *aFont);
/**
 * Outputs a text at a given position with a given color using the
 * CEikonEnv::Static()->SymbolFont() font.
 * @param aGc Window graphics context.
 * @param aString String to be rendered.
 * @param aPosition xy-position of the text to be rendered.
 * @param aColor RGB color of the text.
 */
void utilOutputText(CWindowGc & aGc, const TDesC &aString,
	const TPoint &aPosition, const TRgb &aColor);


#endif
