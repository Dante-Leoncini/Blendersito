/*
 * ==============================================================================
 *  Name        : Textureutils.h
 *
 *  Copyright (c) 2004-2006 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation.
 * ==============================================================================
 */

#ifndef __TEXUTILS__
#define __TEXUTILS__

//  INCLUDE FILES

#include <fbs.h>
#include <e32base.h>
#include <e32std.h>
#include <e32math.h>
#include <w32std.h>
#include <eikenv.h>
#include <eikapp.h>
#include <eikappui.h>

#include <GLES/gl.h>      // OpenGL ES header file
#include <ImageConversion.h>
#include "Mathutils.h"



// MACROS

/** Texture name maximum length */
#define MAX_TEXTURE_NAME 20

// FORWARD DECLARATIONS

// =============================================================================
// =============================================================================

/**
 *  Abstract baseclass that implements basic finite state machine behaviour.
 */

class CFiniteStateMachine : public CBase
	{
	public: // Constructors and destructor
	    /** Factory method that constructs a CFiniteStateMachine by using the NewLC method and then cleans the cleanup stack. */
		static CFiniteStateMachine* NewL();
		/** Factory method that constructs a CFiniteStateMachine and leaves it to the cleanup stack. */
		static CFiniteStateMachine* NewLC();

        /** Destructor. Does nothing. */
		virtual ~CFiniteStateMachine();

	public: // New functions

		/*
		 * Sets the finite state machine's state to given state.
		 * pre-condition (aNewState != -1) : the value -1 should not be used to represent a state.
	 	 * @param aNewState Machine's next state.
		 */
		void SetStateL(TInt aNewState);

		/*
		* Returns the finite state machine's current state.
		* @return Current state.
		*/
		TInt GetCurrentState() const;

		/**
		* Returns the finite state machine's current state  (same as GetCurrentState())
		* @return Current state.
		*/
		TInt GetState() const;

		/**
		* Returns the finite state machine's previous state.
		* @return Current state.
		*/
		TInt GetPreviousState() const;

		/**
		 * This method is called when the finite state machine leaves the state aState
		 * @param aState State that was left.
		 */
		virtual void OnLeaveStateL( TInt aState );

		/**
		* This method is called when the finite state machine enters the state aState
		* @param aState State that was entered.
		*/
		virtual void OnEnterStateL( TInt aState );

	protected:
        /**
         * C++ default constructor. Initializes the current and previous states to -1.
         */
		CFiniteStateMachine();

		/**
		 * Symbian 2nd phase constructor. Initializes the current and previous states to -1.
         */
		virtual void ConstructL( void );


	private: // Data

		/** Current state. */
		TInt iState;
		/** Previous state. */
		TInt iPrevState;
	};

// =============================================================================
// =============================================================================

/**
 * Listener interface that can be used to listen for texture loading events from
 * CTextureManager.
 *
 * The class is intended to be implemented by a client class that observes the
 * texture loading operations of texture manager. The methods in this class
 * are called by the texture manager (class CTextureManager) during the loading
 * operations.
 *
 * Reference to implementations of this listener interface can be passed as
 * a parameter to the constructor of the texture manager (class CTextureManager).
 */
class MTextureLoadingListener
	{
	public:
		/** Called by the CTextureManager when it starts to load textures. */
		virtual void OnStartLoadingTexturesL() = 0;
		/** Called by the CTextureManager when it has loaded all the textures in the queue. */
		virtual void OnEndLoadingTexturesL() = 0;
	};


// =============================================================================
// =============================================================================

/**
 * This structure is used by the texture manager (class CTextureManager). It
 * stores information about a loaded texture or a texture about to be loaded.
 * The instance of this class must be created by the user of CTextureManager.
 * Optionally the iTextureHasColorKey and the related iMinColorKey and iMaxColorKey
 * fields can be set by the user before requesting the texture to be loaded.
 *
 * The iTextureName, iID, iTextureWidth and iTextureHeight fields are filled by the
 * texture manager upon loading the texture.
 */
struct TTexture
{
	/** Texture name (file name from which the texture has been loaded). */
	TBufC<MAX_TEXTURE_NAME> iTextureName;
	/** Unique ID that will be used to reference the texture through the glBindTexture() function. */
	GLuint iID;
	/** Indicates if this texture has some color key info. */
	TBool iTextureHasColorKey;
	/** Min. color key value. Only valid if field iTextureHasColorKey is true. */
	TUint8 iMinColorKey[3];
	/** Max. color key value. Only valid if field iTextureHasColorKey is true. */
	TUint8 iMaxColorKey[3];
	/** Width of the texture in pixels. */
	TInt iTextureWidth;
	/** Height of the texture in pixels. */
	TInt iTextureHeight;
	/** Mipmap generation flag. */
	TBool iGenerateMipmaps;

	//The following fields are used internally by the texture manager
	// to store this struct into a single linked list (class TSglQue).
	/** Do not change! Reserved for CTextureManager internal use! */
	static const TInt iOffset;
	/** Do not change! Reserved for CTextureManager internal use! */
	TSglQueLink iLink;
};

// =============================================================================
// =============================================================================

/**
 * Listener interface that can be used to listen for image loading operation
 * completion events from CImageHandler.
 *
 * The class is intended to be implemented by a client class that observes the
 * loading operation of image handler. The methods in this class
 * are called by the image handler (class CImageHandler) when it loads
 * an image.
 *
 * Reference to implementations of this listener interface can be passed as
 * a parameter to the constructor of the image handler (class CImageHandler).
 */
class MImageHandlerCallback
	{
	public: // New functions
		/**
		 * Called by CImageHandler when an image has been loaded.
		 * @param aError Error code given by the CImageHandler or 0 (zero) if the image was loaded successfully.
		 */
		virtual void ImageOperationCompleteL(TInt aError) = 0;
	};

// =============================================================================
// =============================================================================

/**
*  CImageHandler
*  Image loader class.
*/

class CImageHandler : public CActive
	{
	public: // Constructors and destructor
	    /** Factory method that constructs a CImageHandler by using the NewLC method and then cleans the cleanup stack. */
		static CImageHandler* NewL(CFbsBitmap* aBitmap, CFbsBitmap* aBitmapMask, RFs& aFs, MImageHandlerCallback& aCallback);
		/** Factory method that constructs a CImageHandler and leaves it to the cleanup stack. */
		static CImageHandler* NewLC(CFbsBitmap* aBitmap, CFbsBitmap* aBitmapMask, RFs& aFs, MImageHandlerCallback& aCallback);
        /** Desctructor. Destroys the CImageDecoder used by the image handler. */
		virtual ~CImageHandler();

	public: // New functions
		/**
		 * Sets the target bitmap where the bitmap data is loaded to.
         * @param aBitmap Bitmap where the image data is loaded to.
         * @param aBitmapMask Bitmap where the alpha channel data is loaded to.
		 * @return True if operation is successfull, false if not.
		 */
		TBool SetBitmapL( CFbsBitmap *aBitmap, CFbsBitmap *aBitmapMask );

		/**
		 * Loads a the given frame from the given file.
		 * @param aFileName Filename wherefrom the bitmap data is loaded.
		 * @param aSelectedFrame A single frame index in a multi-frame file. If not given the first frame is loaded.
	  	 */
		void LoadFileL(const TFileName& aFileName, TInt aSelectedFrame = 0);

		/**
		 * Returns the current frame information.
		 * @return Current frame information.
		 */
		const TFrameInfo& FrameInfo() const;

	private: // Functions from base classes

		/**
		 * CActive::RunL() implementation. Called on image load success/failure.
		 */
		void RunL();
		/**
		 * CActive::Cancel() implementation. Stops decoding.
		 */
		void DoCancel();

	protected:
        /**
         * C++ default constructor. Just stores the given parameters to corresponding attributes.
         * @param aBitmap Bitmap where the image data is loaded to.
         * @param aFs File server reference that is used to load the image data.
         * @param aCallback Listener interface implementation that is notified when an image has been loaded.
         */
		CImageHandler(CFbsBitmap *aBitmap, CFbsBitmap *aBitmapMask, RFs &aFs, MImageHandlerCallback &aCallback);
        /**
         * 2nd phase constructor. Adds this object to the active scheduler.
         */
		void ConstructL();

	private: // Data
		/** Image decoder that is used to load the image data from file. */
		CImageDecoder* iDecoder;

		/** Listener that is notified when an image has been loaded. */
		MImageHandlerCallback& iCallback;
		/** Bitmap (owned by the user of this class) where the loaded image data is put. */
		CFbsBitmap *iBitmap;
		/** Bitmap mask (owned by the user of this class) where the loaded alpha channel data is put. */
		CFbsBitmap *iBitmapMask;
		/** File server (owned by the user of this class) that is used to load the raw image data from files. */
		RFs& iFs;
		/** Current image frame information. */
		TFrameInfo iFrameInfo;
    };


// =============================================================================
// =============================================================================
/**
 * Texture manager implementation that makes loading of texture bitmaps a lot easier.
 *
 * As bitmap loading/converting operations on Symbian OS
 * (class CImageDecoder) are asynchronous, the task of loading textures from
 * bitmaps becomes very complex. Especially when it comes to loading several
 * textures. This class can be used to reduce that complexity by
 * handling all the texture loading/deleting, color keying and blitting
 * operations.
 *
 * The texture loading events can be listened by implementing a MTextureLoadingListener.
 *
 */
class CTextureManager: public CFiniteStateMachine, public MImageHandlerCallback
	{
	public: // Constructor and destructor
		/**
		 * Factory method that creates a texture manager that uses the application's private
		 * storage path as the texture path. This is the default place where all applications
		 * in the Symbian security framework should store their private data.
		 * @param aScreenWidth Width of the screen in pixels.
		 * @param aScreenHeight Height of the screen in pixels.
		 * @param aFrustumBottom Bottom clipping plane.
		 * @param aFrustumTop Top horizontal clipping plane.
		 * @param aFrustumLeft Left vertical clipping plane.
		 * @param aFrustumRight Right vertical clipping plane.
		 * @param aFrustumNear Distance to the near depth clipping plane.
		 * @param aTextureLoadingListener Pointer to a MTextureLoadingListener class that is notified of textures loading operations.
 		 */
		static CTextureManager* NewL(
			GLuint aScreenWidth, GLuint aScreenHeight,
			GLfloat aFrustumTop, GLfloat aFrustumBottom,
			GLfloat aFrustumRight, GLfloat aFrustumLeft, GLfloat aFrustumNear,
			MTextureLoadingListener * aTextureLoadingListener = NULL);

		/**
		 * Factory method that creates a texture manager that uses the given texture path.
		 * @param aTexturePath Full path to the directory where textures are stored.
		 * @param aScreenWidth Width of the screen in pixels.
		 * @param aScreenHeight Height of the screen in pixels.
		 * @param aFrustumBottom Bottom clipping plane.
		 * @param aFrustumTop Top horizontal clipping plane.
		 * @param aFrustumLeft Left vertical clipping plane.
		 * @param aFrustumRight Right vertical clipping plane.
		 * @param aFrustumNear Distance to the near depth clipping plane.
		 * @param aTextureLoadingListener Pointer to a MTextureLoadingListener class that is notified of textures loading operations.
 		 */
		static CTextureManager* NewL( TFileName & aTexturePath,
			GLuint aScreenWidth, GLuint aScreenHeight,
			GLfloat aFrustumTop, GLfloat aFrustumBottom,
			GLfloat aFrustumRight, GLfloat aFrustumLeft, GLfloat aFrustumNear,
			MTextureLoadingListener * aTextureLoadingListener = NULL);

        /**
         * Destructor. Deletes the internal CImageHandler and CFsbBitmap objects.
         * Also destroys all the textures that were loaded using this texture manager
         * and finally closes the file server that was given in the constructor.
         */
		virtual ~CTextureManager();

	public: // New functions
		/**
		 * States of the CTextureManager finite state machine.
		 *  EIdle indicates that the object is in idle
		 *  ELoading indicates that the object is loading the textures.
		 */
		enum { EIdle, ELoading };

		/**
		 * Requests a texture to be loaded.
		 * @param aTextureName Name of the file containing the texture. Note: The file must be located in the directory
		 *                     specified by parameter aTexturePath of method CTextureManager::NewL.
		 * @param aTexture Parameter initialized by the texture manager after the texture has been loaded.
		 * @param aMinColorKey Used to specify a min. color key value for the texture. This param. is optional.
		 * @param aMaxColorKey Used to specify a max. color key value for the texture. This param. is optional.
		 * @param aGenerateMipmaps Specifies whether mipmaps should be created for the texture.
 		 */
		void RequestToLoad(const TDesC &aTextureName, TTexture *aTexture,
			TUint8 * aMinColorKey =NULL, TUint8 * aMaxColorKey = NULL,
            TBool aGenerateMipmaps = EFalse);

		/**
 		 * Starts the texture loading. All textures added with RequestToLoad() before calling this method
 		 * are loaded. The progress of texture loading is notified to the MTextureLoadingListener given
 		 * in the constructor.
		 */
		void DoLoadL();

		/**
 		 * If the state that is leaved is ELoading then the registered MTextureLoadingListener instance's
 		 * OnEndLoadingTexturesL method is called to notify that texture loading is now finished.
		 */
		void OnLeaveStateL( TInt aState );
		/**
		 * If the state that is entered is ELoading then the registered MTextureLoadingListener instance's
 		 * OnStartLoadingTexturesL method is called to notify that texture loading is starting.
 		 * After this the textures that have been requested to be loaded are loaded.
		 */
		void OnEnterStateL( TInt );

        /**
         * Implementation of the MImageHandlerCallback method. Gets called when the underlying ImageHandler
         * finished loading one image.
         */
		void ImageOperationCompleteL(TInt aError);

		/**
		 * Performs a 2D blit using OpenGL ES by rendering a textured polygon on the near depth clipping plane.
		 * This method supports alpha blending, color keying and stretching (or shrinking) of the blitted
		 * texture.
		 * @param aTexture Texture to be blitted to the screen.
		 * @param aX1 Upper left X screen coordinate of the rectangle where the texture is to be blitted.
		 * @param aY1 Upper left Y screen coordinate of the rectangle where the texture is to be blitted.
		 * @param aX2 Lower right X screen coordinate of the rectangle where the texture is to be blitted.
		 * @param aY2 Lower right Y screen coordinate of the rectangle where the texture is to be blitted.
 		 */
		void Blit(TTexture & aTexture, GLuint aX1, GLuint aY1, GLuint aX2, GLuint aY2);
		/**
		 * Performs a 2D blit using OpenGL ES by rendering a textured polygon on the near depth clipping plane.
		 * This method supports alpha blending and color keying. The texture is blitted with it's default size.
		 * @param aTexture Texture to be blitted to the screen.
		 * @param aX Upper left X screen coordinate where the texture is to be blitted.
		 * @param aY Upper left Y screen coordinate where the texture is to be blitted.
		 */
		void Blit(TTexture & aTexture, GLuint aX, GLuint aY);

        /**
         * Sets the screen width and height. Must be called if the screen size changes during execution
         * of the program and if the Blit() method are used as screen width and height values are used in the Blit() methods.
         * @param aScreenWidth New screen width to be used. Zero values are ignored (the old screen width is used if this value is zero).
         * @param aScreenHeight New screen height to be used.  Zero values are ignored (the old screen height is used if this value is zero).
         */
		void SetScreenSize( GLuint aScreenWidth, GLuint aScreenHeight );

	protected: // New functions

		/**
		 * Loads the next texture in the loading queue.
		 */
		void LoadNextTextureL();

		/**
		 * Deletes named textures (the OpenGL ES assigned name) by calling glDeleteTextures.
		 * After this the internal texture name array and the textures are deleted.
		 * WARNING! The CTextureManager instance can't be used after calling this method.
		 */
		void DeleteTextures();

		/**
		 * Checks if the given texture has already been loaded.
		 * If it has been loaded this method returns a pointer to the struct TTexture containing all the information.
		 * If it hasn't been loaded this method returns NULL.
		 * @param aTextureName Name of the texture.
		 * @return TTexture structure containing the information for the texture or NULL if the texture wasn't yet loaded.
		 */
		TTexture * TextureExists( const TDesC &aTextureName);

	protected:

		/**
		 * Default C++ and Symbian ConstructL are placed in protected
		 * area to force the use of NewL when creating an instance.
		 */
		CTextureManager();

		/**
		 * Constructor that creates a texture manager that uses the application's private
		 * storage path as the texture path. This is the default place where all applications
		 * in the Symbian security framework should store their private data.
 		 * @param aTexturePath Full path to the directory where textures are stored.
		 * @param aScreenWidth Width of the screen in pixels.
		 * @param aScreenHeight Height of the screen in pixels.
		 * @param aFrustumBottom Bottom clipping plane.
		 * @param aFrustumTop Top horizontal clipping plane.
		 * @param aFrustumLeft Left vertical clipping plane.
		 * @param aFrustumRight Right vertical clipping plane.
		 * @param aFrustumNear Distance to the near depth clipping plane.
		 * @param aTextureLoadingListener Reference to a MTextureLoadingListener client class which observe the textures loading operations.
 		 */
		void ConstructL( TFileName & aTexturePath,
			GLuint aScreenWidth, GLuint aScreenHeight,
			GLfloat aFrustumTop, GLfloat aFrustumBottom,
			GLfloat aFrustumRight, GLfloat aFrustumLeft, GLfloat aFrustumNear,
			MTextureLoadingListener * aTextureLoadingListener = NULL );

		/**
		 * Constructor that creates a texture manager that uses the given path as the texture path.
		 * @param aScreenWidth Width of the screen in pixels.
		 * @param aScreenHeight Height of the screen in pixels.
		 * @param aFrustumBottom Bottom clipping plane.
		 * @param aFrustumTop Top horizontal clipping plane.
		 * @param aFrustumLeft Left vertical clipping plane.
		 * @param aFrustumRight Right vertical clipping plane.
		 * @param aFrustumNear Distance to the near depth clipping plane.
		 * @param aTextureLoadingListener Pointer to a MTextureLoadingListener class that is notified of textures loading operations.
  		 */
		void ConstructL(
			GLuint aScreenWidth, GLuint aScreenHeight,
			GLfloat aFrustumTop, GLfloat aFrustumBottom,
			GLfloat aFrustumRight, GLfloat aFrustumLeft, GLfloat aFrustumNear,
			MTextureLoadingListener * aTextureLoadingListener = NULL );

	private:
		/** Directory where the textures are loaded from. */
		TFileName iTexturePath;
		/** Loading queue where all the textures about to be loaded are stored. */
		TSglQue<TTexture> iLoadingQueue;
		/** Loading queue iterator. */
		TSglQueIter<TTexture> iLoadingQueueIterator;
		/** Reference to a MTextureLoadingListener object that gets notified of texture loading progress. */
		MTextureLoadingListener * iTextureLoadingListener;

		/** Texture that is currently being loaded. */
		TTexture * iCurrentRequest;
		/** CImageHandler object that will perform the individual image loading/converting operations. */
		CImageHandler* iBmapUtil;

		/** File server that is used to load the raw texture data. */
		RFs iFs;

		/** Array in which the OpenGL ES generated texture names (ids) are stored. */
		GLuint * iTextureNames;
		/** Lenght of iTextureNames. */
		TInt iTextureNameCount;
		/** Variable used to index iTextureNames. */
		TInt iIndex;
		/** Bitmap object that is currently being loaded. */
		CFbsBitmap * iBitmap;
		/** Bitmap mask object that is currently being loaded. */
		CFbsBitmap * iBitmapMask;
		/** Coordinate for the bottom horizontal clipping plane. */
		GLfloat iFrustumBottom;
		/** Coordinate for the top horizontal clipping plane. */
		GLfloat iFrustumTop;
		/** Coordinate for the left vertical clipping plane. */
		GLfloat iFrustumLeft;
		/** Coordinate for the right vertical clipping plane. */
		GLfloat iFrustumRight;
		/** Distance to the near depth clipping plane. */
		GLfloat iFrustumNear;
		/** View frustum width (equals iFrustumRight-iFrustumLeft). */
		GLfloat iFrustumWidth;
		/** View frustum height (equals iFrustumTop-iFrustumBottom). */
		GLfloat iFrustumHeight;
        /** Screen width in pixels. */
		GLuint iScreenWidth;
		/** Screen height in pixels. */
		GLuint iScreenHeight;
	};

/**
 * Returns the root directory of an application in the form [drive-letter]:\[path]\
 * @return Application's root directory.
 */
static TFileName utilGetAppRootDirectoryL();

#endif
