/*
 * ==============================================================================
 *  Name        : Textureutils.cpp
 *
 *  Copyright (c) 2004-2006 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation.
 * ==============================================================================
 */

// INCLUDE FILES

#include "Textureutils.h"


// CONSTANTS

/** Define texture coordinates used by class CTextureManager for bliting operations. */
static const GLshort BlitTextureCoords [] =
	{
	1, 1,
	1, 0,
	0, 1,
	0, 0,
	};

/** Indices to BlitTextureCoords used by class CTextureManager for bliting operations. */
static const GLshort BlitIndices [] =
	{
    0,1,2,3
	};



/**
 * A constant used internally by class CTextureManager to store
 * the textures loading request into a single linked list (class TSglQue).
 */
const TInt TTexture::iOffset = _FOFF(TTexture,iLink);

// =============================================================================
// CFiniteStateMachine
// =============================================================================

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFiniteStateMachine::CFiniteStateMachine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFiniteStateMachine::CFiniteStateMachine()
	{
	iPrevState = iState = -1;
	}

// -----------------------------------------------------------------------------
// CFiniteStateMachine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFiniteStateMachine::ConstructL()
	{
	iPrevState = iState = -1;
	}

// -----------------------------------------------------------------------------
// CFiniteStateMachine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFiniteStateMachine* CFiniteStateMachine::NewL()
	{
	CFiniteStateMachine* self = NewLC();
	CleanupStack::Pop();

	return self;
	}

// -----------------------------------------------------------------------------
// CFiniteStateMachine::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFiniteStateMachine* CFiniteStateMachine::NewLC()
{
	CFiniteStateMachine* self = new (ELeave) CFiniteStateMachine;
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
}

// Destructor
CFiniteStateMachine::~CFiniteStateMachine()
	{
	}

// -----------------------------------------------------------------------------
// CFiniteStateMachine::SetStateL
// Set the current state and trigger OnEnterStateL.
// -----------------------------------------------------------------------------
//
void CFiniteStateMachine::SetStateL( TInt aNewState )
	{
	if ( aNewState != -1 )
		{
		if ( aNewState != iState)
			{
			 if ( iPrevState != -1 )
				{
				OnLeaveStateL( iState );
				}
			 iPrevState = iState;
			 iState = aNewState;
			 OnEnterStateL( iState );
			}
		}
	}

// -----------------------------------------------------------------------------
// CFiniteStateMachine::GetCurrentState
// Return the current state.
// -----------------------------------------------------------------------------
//
TInt CFiniteStateMachine::GetCurrentState() const
	{
	return iState;
	}

// -----------------------------------------------------------------------------
// CFiniteStateMachine::GetState
// Return the current state.
// -----------------------------------------------------------------------------
//
TInt CFiniteStateMachine::GetState() const
	{
	return GetCurrentState();
	}

// -----------------------------------------------------------------------------
// CFiniteStateMachine::GetPreviousState
// Return the current previous state.
// -----------------------------------------------------------------------------
//
TInt CFiniteStateMachine::GetPreviousState() const
	{
	return iPrevState;
	};


// -----------------------------------------------------------------------------
// CFiniteStateMachine::OnLeaveStateL
// Empty implementation of overloadable "leave state" handler.
// -----------------------------------------------------------------------------
//
void CFiniteStateMachine::OnLeaveStateL(TInt /*aState*/)
	{
	// Empty implementation
	}

// -----------------------------------------------------------------------------
// CFiniteStateMachine::OnEnterStateL
// Empty implementation of overloadable "enter state" handler.
// -----------------------------------------------------------------------------
//
void CFiniteStateMachine::OnEnterStateL(TInt /*aState*/)
	{
	// Empty implementation
	}

// =============================================================================
// CImageHandler, a utility class for loading images.
// =============================================================================

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CImageHandler::CImageHandler
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CImageHandler::CImageHandler( CFbsBitmap* aBitmap,
    CFbsBitmap* aBitmapMask,
	RFs& aFs,
	MImageHandlerCallback& aCallback )
	: CActive(CActive::EPriorityStandard),
	  iDecoder(NULL),
	  iCallback(aCallback),
	  iBitmap(aBitmap),
	  iBitmapMask(aBitmapMask),
	  iFs(aFs)
	{
	}

// -----------------------------------------------------------------------------
// CImageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImageHandler::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// CImageHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImageHandler* CImageHandler::NewL(CFbsBitmap* aBitmap, CFbsBitmap* aBitmapMask, RFs& aFs,
	MImageHandlerCallback& aCallback)
{
	CImageHandler* self = NewLC(aBitmap, aBitmapMask, aFs, aCallback);
	CleanupStack::Pop();
	return self;
}

// -----------------------------------------------------------------------------
// CImageHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImageHandler* CImageHandler::NewLC(CFbsBitmap* aBitmap, CFbsBitmap* aBitmapMask,
	RFs& aFs,
	MImageHandlerCallback& aCallback)
{
	CImageHandler* self = new (ELeave) CImageHandler(aBitmap, aBitmapMask, aFs, aCallback);
	CleanupStack::PushL( self );
	self->ConstructL();

	return self;
}

// Destructor
CImageHandler::~CImageHandler()
	{
 	delete iDecoder;
	}

// -----------------------------------------------------------------------------
// CImageHandler::SetBitmapL
// Set the destination bitmap.
// -----------------------------------------------------------------------------
//
TBool CImageHandler::SetBitmapL( CFbsBitmap *aBitmap, CFbsBitmap* aBitmapMask )
	{
	if ( IsActive() )
		{
		// If active, fail.
		return EFalse;
		}
	// else
	iBitmap = aBitmap;
	iBitmapMask = aBitmapMask;
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CImageHandler::LoadFileL
// Loads a selected frame from a named file
// -----------------------------------------------------------------------------
//
void CImageHandler::LoadFileL(const TFileName& aFileName,
	TInt aSelectedFrame)
	{
	__ASSERT_ALWAYS(!IsActive(),User::Invariant());
	if ( iDecoder )
		{
		delete iDecoder;
		}
	iDecoder = NULL;
	iDecoder = CImageDecoder::FileNewL(iFs, aFileName);
	// Get image information
	iFrameInfo = iDecoder->FrameInfo(aSelectedFrame);
	// Resize to fit.
	TRect bitmapSize = iFrameInfo.iFrameCoordsInPixels;
	iBitmap->Resize(bitmapSize.Size());

    // Check for alpha channel availability
    if (iFrameInfo.iFlags & (TFrameInfo::ETransparencyPossible | TFrameInfo::EAlphaChannel))
        {
	    // Decode as bitmap + alpha channel mask.
    	iBitmapMask->Resize(bitmapSize.Size());
	    iDecoder->Convert(&iStatus, *iBitmap, *iBitmapMask, aSelectedFrame);
        }
    else
        {
	    // Decode as bitmap.
	    iDecoder->Convert(&iStatus, *iBitmap, aSelectedFrame);
        }

	SetActive();
	}

// -----------------------------------------------------------------------------
// CImageHandler::FrameInfo
// Get the current frame information.
// -----------------------------------------------------------------------------
//
const TFrameInfo& CImageHandler::FrameInfo() const
	{
	return iFrameInfo;
	}

// -----------------------------------------------------------------------------
// CImageHandler::RunL
// CActive::RunL() implementation. Called on image load success/failure.
// -----------------------------------------------------------------------------
//
void CImageHandler::RunL()
	{
	// Invoke callback.
	iCallback.ImageOperationCompleteL(iStatus.Int());
	}

// -----------------------------------------------------------------------------
// CImageHandler::DoCancel
// CActive::Cancel() implementation. Stops decoding.
// -----------------------------------------------------------------------------
//
void CImageHandler::DoCancel()
	{
	if ( iDecoder )
		{
		iDecoder->Cancel();
		}
	}

// =============================================================================
// CTextureManager
// =============================================================================

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTextureManager::CTextureManager
// C++ default constructor can NOT contain any code, that
// might leave.
// Initializes the loading queue and the base class
// -----------------------------------------------------------------------------
//
CTextureManager::CTextureManager()
	: CFiniteStateMachine(),
	iLoadingQueue( TTexture::iOffset ),
	iLoadingQueueIterator(iLoadingQueue)
	{
	}

// -----------------------------------------------------------------------------
// CTextureManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTextureManager::ConstructL( TFileName & aTexturePath,
	GLuint aScreenWidth,
	GLuint aScreenHeight,
	GLfloat aFrustumTop,
	GLfloat aFrustumBottom,
	GLfloat aFrustumRight,
	GLfloat aFrustumLeft,
	GLfloat aFrustumNear,
	MTextureLoadingListener * aTextureLoadingListener )

{
	SetStateL( EIdle );

	// Check if texture path was given
	// Texture path was given, just use it
	iTexturePath = aTexturePath;
	iTextureLoadingListener = aTextureLoadingListener;

	iBitmap = new (ELeave) CFbsBitmap;
    iBitmap->Create(TSize(10, 10), EColor16M);
	iBitmapMask = new (ELeave) CFbsBitmap;
    iBitmapMask->Create(TSize(10, 10), EGray256);

	/* Create a bitmap utility for loading the textures. */
	iFs.Connect();
    iBmapUtil = CImageHandler::NewL( iBitmap, iBitmapMask, iFs, *this );

	iTextureNames = NULL;

	iFrustumTop = aFrustumTop;
	iFrustumBottom = aFrustumBottom;
	iFrustumRight = aFrustumRight;
	iFrustumLeft = aFrustumLeft;
	iFrustumNear = aFrustumNear;
	iFrustumWidth = aFrustumRight-aFrustumLeft;
	iFrustumHeight = aFrustumBottom-aFrustumTop;

	iScreenWidth  = aScreenWidth;
	iScreenHeight = aScreenHeight;
}

// -----------------------------------------------------------------------------
// CTextureManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTextureManager::ConstructL(
	GLuint aScreenWidth,
	GLuint aScreenHeight,
	GLfloat aFrustumTop,
	GLfloat aFrustumBottom,
	GLfloat aFrustumRight,
	GLfloat aFrustumLeft,
	GLfloat aFrustumNear,
	MTextureLoadingListener * aTextureLoadingListener )

{
	TFileName fullTexturePath;
    iFs.Connect();

	// Parse the current drive letter from application location
	TPtrC16 driveLetter = utilGetAppRootDirectoryL().Left( 2 );
	fullTexturePath.Append( driveLetter );

    // Append the application private path
    TPath privateDir;
    User::LeaveIfError( iFs.PrivatePath( privateDir ) );
    fullTexturePath.Append( privateDir );
    iFs.Close();

    ConstructL( fullTexturePath, aScreenWidth, aScreenHeight,
                aFrustumTop, aFrustumBottom, aFrustumRight,
                aFrustumLeft, aFrustumNear, aTextureLoadingListener );
}


// -----------------------------------------------------------------------------
// CTextureManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTextureManager* CTextureManager::NewL(
	GLuint aScreenWidth,
	GLuint aScreenHeight,
	GLfloat aFrustumTop,
	GLfloat aFrustumBottom,
	GLfloat aFrustumRight,
	GLfloat aFrustumLeft,
	GLfloat aFrustumNear,
	MTextureLoadingListener * aTextureLoadingListener )
{
	CTextureManager* self = new (ELeave) CTextureManager();
	CleanupStack::PushL( self );
	self->ConstructL( aScreenWidth, aScreenHeight,
		aFrustumTop, aFrustumBottom, aFrustumRight, aFrustumLeft,
		aFrustumNear, aTextureLoadingListener );
	CleanupStack::Pop();

	return self;
}

// -----------------------------------------------------------------------------
// CTextureManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTextureManager* CTextureManager::NewL( TFileName & aTexturePath,
	GLuint aScreenWidth,
	GLuint aScreenHeight,
	GLfloat aFrustumTop,
	GLfloat aFrustumBottom,
	GLfloat aFrustumRight,
	GLfloat aFrustumLeft,
	GLfloat aFrustumNear,
	MTextureLoadingListener * aTextureLoadingListener )
{
	CTextureManager* self = new (ELeave) CTextureManager();
	CleanupStack::PushL( self );

	self->ConstructL( aTexturePath, aScreenWidth, aScreenHeight,
		aFrustumTop, aFrustumBottom, aFrustumRight, aFrustumLeft,
		aFrustumNear, aTextureLoadingListener );
	CleanupStack::Pop();

	return self;
}


// Destructor
CTextureManager::~CTextureManager( )
	{
	delete iBmapUtil;
	delete iBitmap;
	delete iBitmapMask;
	DeleteTextures();
	iFs.Close();
	}

// -----------------------------------------------------------------------------
// CTextureManager::DeleteTextures
// Deletes named textures by calling glDeleteTextures.
// -----------------------------------------------------------------------------
//
void CTextureManager::DeleteTextures()
	{
	if ( iTextureNames != NULL )
		{
		glDeleteTextures( iTextureNameCount, iTextureNames );
		delete [] iTextureNames;
		iTextureNames = NULL;
		iTextureNameCount = 0;
		}
	}

// -----------------------------------------------------------------------------
// CTextureManager::TextureExists
// Checks if a given texture has already been loaded.
// else returns NULL
// -----------------------------------------------------------------------------
//
TTexture * CTextureManager::TextureExists( const TDesC &aTextureName )
	{
	TTexture * item;

	iLoadingQueueIterator.SetToFirst();
	while ( (item = iLoadingQueueIterator) != NULL )
		{
        if ( item->iTextureName.Compare( aTextureName ) == 0 )
			{
			return item;
			}
		else
			{
			iLoadingQueueIterator++;
			}
		}

	return NULL;
	}

// -----------------------------------------------------------------------------
// CTextureManager::RequestToLoad
// Sends a request to load a texture
// -----------------------------------------------------------------------------
//
void CTextureManager::RequestToLoad( const TDesC &aTextureName,
	TTexture * aTexture,
	TUint8 * aMinColorKey,
	TUint8 * aMaxColorKey,
    TBool aGenerateMipmaps)
	{
	if ( GetState() == EIdle )
		{
		TTexture * text;
		if ( (text = TextureExists( aTextureName )) != NULL )
			{
			*aTexture = *text;
			return;
			}

		aTexture->iTextureName = aTextureName;
        aTexture->iTextureHasColorKey = ( aMinColorKey != NULL && aMaxColorKey != NULL);
		if ( aTexture->iTextureHasColorKey )
			{
			aTexture->iMinColorKey[0] = aMinColorKey[0];
			aTexture->iMinColorKey[1] = aMinColorKey[1];
			aTexture->iMinColorKey[2] = aMinColorKey[2];

			aTexture->iMaxColorKey[0] = aMaxColorKey[0];
			aTexture->iMaxColorKey[1] = aMaxColorKey[1];
			aTexture->iMaxColorKey[2] = aMaxColorKey[2];
			}

		aTexture->iGenerateMipmaps = aGenerateMipmaps;
		iLoadingQueue.AddLast( *aTexture );
		}
	}

// -----------------------------------------------------------------------------
// CTextureManager::DoLoadL
// Starts the loading process.
// -----------------------------------------------------------------------------
//
void CTextureManager::DoLoadL()
	{
	switch ( GetState() )
		{
		case EIdle:
			SetStateL( ELoading );
			break;
		case ELoading:
			break;
		default:
			break;
		}
}

// -----------------------------------------------------------------------------
// CTextureManager::OnEnterStateL
// Implementation of method TFiniteStateMachine::OnEnterState( TInt aState ).
// Generates the needed GL textures. Signals the listener.
// -----------------------------------------------------------------------------
//
void CTextureManager::OnEnterStateL( TInt aState)
	{
	switch ( aState )
		{
		case EIdle:
			break;
		case ELoading:
			if ( iTextureLoadingListener != NULL )
				iTextureLoadingListener->OnStartLoadingTexturesL(); //Signals the MTextureLoadingListener client
		                                                        //That texture loading has started.
			if ( iLoadingQueue.IsEmpty() )
				SetStateL( EIdle );
			else
				{
				DeleteTextures();
				iLoadingQueueIterator.SetToFirst();
				iTextureNameCount = 0;
				while ( ((TTexture *)iLoadingQueueIterator) != NULL )
					{
					iLoadingQueueIterator++;
					iTextureNameCount++;
					}

				iTextureNames = new (ELeave) GLuint [iTextureNameCount];
				glGenTextures( iTextureNameCount, iTextureNames );
				iIndex = 0;
				iLoadingQueueIterator.SetToFirst();
				LoadNextTextureL();
				}
			break;
		default:
			break;
		}
	}

// -----------------------------------------------------------------------------
// CTextureManager::OnLeaveStateL
// Implementation of method TFiniteStateMachine::OnLeaveStateL( TInt aState ).
// -----------------------------------------------------------------------------
//
void CTextureManager::OnLeaveStateL( TInt aState )
	{
	switch ( aState )
		{
		case EIdle:
			break;
		case ELoading:
			if ( iTextureLoadingListener != NULL )
				iTextureLoadingListener->OnEndLoadingTexturesL(); //Signals the MTextureLoadingListener client
																  //That texture loading is completed.
			break;
		default:
			break;
		}
	}

// -----------------------------------------------------------------------------
// CTextureManager::LoadNextTextureL
// Loads the next texture in the loading queue.
// -----------------------------------------------------------------------------
//
void CTextureManager::LoadNextTextureL()
	{
	TFileName FullTextureName;

	iCurrentRequest = iLoadingQueueIterator++;
	if ( iCurrentRequest == NULL )
		{
		SetStateL( EIdle );
		}
	else
		{
		FullTextureName.Append( iTexturePath );
		FullTextureName.Append( iCurrentRequest->iTextureName );
		iBmapUtil->LoadFileL(FullTextureName, 0);
		}
	}

// -----------------------------------------------------------------------------
// CTextureManager::ImageOperationCompleteL
// Implements MImageHandlerCallback::ImageOperationCompleteL.
// Converts the loaded texture data into an appropriate format
// by flipping moving from BGR order to RGB. Implements the transparent
// color keying by setting the alpha according to the key.
//
// Creates the GL texture object.
// -----------------------------------------------------------------------------
//
void CTextureManager::ImageOperationCompleteL(TInt aError)
	{
	_LIT(KConvErr, "Conversion failed!");
	if ( aError )
		{
		User::Panic( KConvErr, aError );
		}

    glBindTexture( GL_TEXTURE_2D, iTextureNames[iIndex] );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, iCurrentRequest->iGenerateMipmaps ? GL_TRUE : GL_FALSE );

	const TFrameInfo frameInfo      = iBmapUtil->FrameInfo();
	iCurrentRequest->iTextureWidth  = frameInfo.iOverallSizeInPixels.iWidth;
	iCurrentRequest->iTextureHeight = frameInfo.iOverallSizeInPixels.iHeight;

	iCurrentRequest->iID = iTextureNames[iIndex];

	TInt TextureSize = iCurrentRequest->iTextureWidth *
		iCurrentRequest->iTextureHeight;

	// Ask the bitmap heap to be locked if this is a large bitmap
	iBitmap->LockHeap( EFalse );

	/* The data in the texture are in RGB order but is read in BGR order.
	   So we have to swap the 1st and 3rd bytes. */
	TUint8 * TextureData = (TUint8 *) iBitmap->DataAddress();
	TUint8 * AlphaData = (TUint8 *) iBitmapMask->DataAddress();
	for ( TInt i = 0; i < TextureSize; i++ )
		{
		TUint8 t = TextureData[i*3];
		TextureData[i*3] = TextureData[i*3+2];
		TextureData[i*3+2] = t;
		}

	if ( iCurrentRequest->iTextureHasColorKey )
		{
		/*
		 If a color key interval was specified, then we create an alpha channel.
		*/
		TUint8 * TextureDataWithAlphaChannel =
			new (ELeave) TUint8[TextureSize * 4];

	   // walk through the original image and copy it over to the destination
	for ( TInt i = 0, j = 0; i < TextureSize * 3; i += 3, j += 4 )
		{
         /*
		  If a pixel color is within the color key interval, we assign it an
		  alpha value of 0, which makes it transparent.
          Otherwise the alpha value of the pixel is set to 255, full alpha.
         */
		if ( TextureData[i] >= iCurrentRequest->iMinColorKey[0] &&
			TextureData[i + 1] >= iCurrentRequest->iMinColorKey[1] &&
			TextureData[i + 2] >= iCurrentRequest->iMinColorKey[2] &&
			TextureData[i] <= iCurrentRequest->iMaxColorKey[0] &&
			TextureData[i + 1] <= iCurrentRequest->iMaxColorKey[1] &&
			TextureData[i + 2] <= iCurrentRequest->iMaxColorKey[2] )
			{
			TextureDataWithAlphaChannel[j + 3] = 0;
			}
		else
			{
			 TextureDataWithAlphaChannel[j + 3] = 255;
			}

		TextureDataWithAlphaChannel[j    ] = TextureData[i    ];
		TextureDataWithAlphaChannel[j + 1] = TextureData[i + 1];
		TextureDataWithAlphaChannel[j + 2] = TextureData[i + 2];
		}

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
			iCurrentRequest->iTextureWidth, iCurrentRequest->iTextureHeight,
			0, GL_RGBA, GL_UNSIGNED_BYTE, TextureDataWithAlphaChannel );

		delete [] TextureDataWithAlphaChannel;
		}
    else if (frameInfo.iFlags & (TFrameInfo::ETransparencyPossible | TFrameInfo::EAlphaChannel))
        {
		/*
		 If an alpha channel was present in the image, combine the bitmap and
         the mask into a single RGBA texture.
		*/
		TUint8 * TextureDataWithAlphaChannel =
			new (ELeave) TUint8[TextureSize * 4];
	    for ( TInt i = 0, j = 0, k = 0; i < TextureSize * 3; i += 3, j += 4, k++ )
		    {
			TextureDataWithAlphaChannel[j + 0] = TextureData[i + 0];
			TextureDataWithAlphaChannel[j + 1] = TextureData[i + 1];
			TextureDataWithAlphaChannel[j + 2] = TextureData[i + 2];
			TextureDataWithAlphaChannel[j + 3] = AlphaData[k];
		    }
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
			iCurrentRequest->iTextureWidth, iCurrentRequest->iTextureHeight,
			0, GL_RGBA, GL_UNSIGNED_BYTE, TextureDataWithAlphaChannel );

		delete [] TextureDataWithAlphaChannel;
        }
    else
		{
		glTexImage2D(  GL_TEXTURE_2D, 0, GL_RGB, iCurrentRequest->iTextureWidth,
			iCurrentRequest->iTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
			TextureData );

		}

	// Ask the bitmap heap to be unlocked if this is a large bitmap
	iBitmap->UnlockHeap( EFalse );

	iIndex++;

    /* When the conversion of the current texture completes,
       start loading the next one. */
	LoadNextTextureL();
	}

// -----------------------------------------------------------------------------
// CTextureManager::Blit
// Performs a 2D blits using OpenGL ES by rendering a textured polygon on the
// near depth clipping plane.
// This method supports alpha blending and color keying during blit operations.
// -----------------------------------------------------------------------------
//
void CTextureManager::Blit(TTexture & aTexture, GLuint aX, GLuint aY)
	{
	Blit(aTexture, aX, aY, aX+aTexture.iTextureWidth,
		aY+aTexture.iTextureHeight);
	}

// -----------------------------------------------------------------------------
// CTextureManager::Blit
// Performs a 2D blits using OpenGL ES by rendering a textured polygon on the
// near depth clipping plane.
// This method supports alpha blending and color keying during blit operations.
// Note: arbitrary stretching or shrinking will be performed for the texture to
// fit the destination rectangle (achieved using GL functions).
// -----------------------------------------------------------------------------
//

void CTextureManager::Blit( TTexture & aTexture, GLuint aX1, GLuint aY1,
	GLuint aX2, GLuint aY2 )
	{
	GLfloat x1, y1, x2, y2, ax, bx, ay, by;
    GLfloat aspectRatio = (GLfloat)(iScreenWidth) / (GLfloat)(iScreenHeight);

    //Transforming screen coordinates to 3D world coordinates.
	ax = aspectRatio * iFrustumWidth / iScreenWidth;
	bx = aspectRatio * iFrustumLeft;
	ay = (GLfloat)iFrustumHeight / iScreenHeight;
	by = iFrustumTop;
	x1 = ax * aX1 + bx;
	y1 = ay * aY1 + by;
	x2 = ax * aX2 + bx;
	y2 = ay * aY2 + by;

    GLfloat Vertices[4 * 3] =
    {
        x2, y2, -iFrustumNear,
        x2, y1, -iFrustumNear,
        x1, y2, -iFrustumNear,
        x1, y1, -iFrustumNear,
    };

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
    glLoadIdentity();
    glDisable( GL_DEPTH_TEST );

    glBindTexture(  GL_TEXTURE_2D, aTexture.iID );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	glVertexPointer( 3, GL_FLOAT, 0, Vertices );
	glTexCoordPointer( 2, GL_SHORT, 0, BlitTextureCoords );
    glDrawElements( GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, BlitIndices );

    glEnable( GL_DEPTH_TEST );

	glPopMatrix();
	}

// -----------------------------------------------------------------------------
// CTextureManager::SetScreenSize
// Sets the screen width and height. Must be called if the screen size changes during execution
// of the program and if the Blit() method are used as screen width and height values are used in the Blit() methods.
// Note: If either of the given values is zero, that value is ignored (the old width/height is used).
// -----------------------------------------------------------------------------
//
void CTextureManager::SetScreenSize( GLuint aScreenWidth, GLuint aScreenHeight )
    {
    if( aScreenWidth )
        {
        iScreenWidth  = aScreenWidth;
        }
    if( aScreenHeight )
        {
        iScreenHeight = aScreenHeight;
        }
    }

// -----------------------------------------------------------------------------
// utilOutputText
// Returns the root directory of an application in the form <drive-letter>:\<path>\
// -----------------------------------------------------------------------------
//
static TFileName utilGetAppRootDirectoryL()
	{
	TFileName RootDirectory;

    TParsePtrC Parse( CEikonEnv::Static()->EikAppUi()->Application()->AppFullName() );
    RootDirectory = Parse.DriveAndPath();

	return RootDirectory;
	}

