/*
 * ==============================================================================
 *  Name        : BlenderLiteContainer.cpp
 *  Part of     : OpenGLEx / BlenderLite
 * ==============================================================================
 */

// INCLUDE FILES
#include "BlenderLiteContainer.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBlenderLiteContainer::ConstructL
// Symbian 2nd phase constructor
// ---------------------------------------------------------
//
void CBlenderLiteContainer::ConstructL(const TRect& /*aRect*/){
    CreateWindowL();
    iOpenGlInitialized = EFalse;
    // Create the input handler
    iInputHandler = CBlenderLiteInput::NewL();

    SetExtentToWholeScreen();                // Take the whole screen into use
    ActivateL();

    iFrame = 0;                              // Frame counter

    EGLConfig Config;                        // Describes the format, type and
                                             // size of the color buffers and
                                             // ancillary buffers for EGLSurface

    // Get the display for drawing graphics
    iEglDisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );
    if ( iEglDisplay == NULL ){
        _LIT(KGetDisplayFailed, "eglGetDisplay failed");
        User::Panic( KGetDisplayFailed, 0 );
    }

    // Initialize display
    if ( eglInitialize( iEglDisplay, NULL, NULL ) == EGL_FALSE ){
        _LIT(KInitializeFailed, "eglInitialize failed");
        User::Panic( KInitializeFailed, 0 );
    }

    EGLConfig *configList = NULL;            // Pointer for EGLConfigs
    EGLint numOfConfigs = 0;
    EGLint configSize   = 0;

    // Get the number of possible EGLConfigs
    if ( eglGetConfigs( iEglDisplay, configList, configSize, &numOfConfigs )
        == EGL_FALSE )
        {
        _LIT(KGetConfigsFailed, "eglGetConfigs failed");
        User::Panic( KGetConfigsFailed, 0 );
        }

    configSize = numOfConfigs;

    // Allocate memory for the configList
    configList = (EGLConfig*) User::Alloc( sizeof(EGLConfig)*configSize );
    if ( configList == NULL )
        {
        _LIT(KConfigAllocFailed, "config alloc failed");
        User::Panic( KConfigAllocFailed, 0 );
        }

    /* Define properties for the wanted EGLSurface.
       To get the best possible performance, choose
       an EGLConfig with a buffersize matching
       the current window's display mode*/
    TDisplayMode DMode = Window().DisplayMode();
    TInt BufferSize = 0;

    switch ( DMode ){
         case(EColor4K):
             BufferSize = 12;
             break;
         case(EColor64K):
             BufferSize = 16;
             break;
         case(EColor16M):
             BufferSize = 24;
             break;
         case(EColor16MU):
         case(EColor16MA):
             BufferSize = 32;
             break;
         default:
             _LIT(KDModeError,       "unsupported displaymode");
             User::Panic( KDModeError, 0 );
             break;
    }

    // Define properties for requesting a full-screen antialiased window surface
    const EGLint attrib_list_fsaa[] = {
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BUFFER_SIZE,  BufferSize,
        EGL_DEPTH_SIZE,   16,
				EGL_SAMPLE_BUFFERS, 1,
				EGL_SAMPLES,        4,//antialiasing
        EGL_NONE
    };

    // Define properties for requesting a non-antialiased window surface
    const EGLint attrib_list[] = {
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BUFFER_SIZE,  BufferSize,
        EGL_DEPTH_SIZE,   16,
        EGL_NONE
    };

    // Choose an EGLConfig that best matches to the properties in attrib_list_fsaa
    if ( eglChooseConfig( iEglDisplay, attrib_list_fsaa, configList, configSize, &numOfConfigs ) == EGL_FALSE )
        {
        _LIT( KChooseConfigFailed, "eglChooseConfig failed" );
        User::Panic( KChooseConfigFailed, 0 );
        }

    // Check if no configurations were found
    if ( numOfConfigs == 0 )
        {
        // No configs with antialising were found. Try to get the non-antialiased config
		    if ( eglChooseConfig( iEglDisplay, attrib_list, configList, configSize, &numOfConfigs ) == EGL_FALSE )
			      {
	          _LIT( KChooseConfigFailed, "eglChooseConfig failed" );
	          User::Panic( KChooseConfigFailed, 0 );
			      }

		    if ( numOfConfigs == 0 )
			      {
			      // No configs found without antialiasing
	          _LIT( KNoConfig, "Can't find the requested config." );
	          User::Panic( KNoConfig, 0 );
	          }
	      }

    Config = configList[0];   // Choose the best EGLConfig. EGLConfigs
                              // returned by eglChooseConfig are sorted so
                              // that the best matching EGLConfig is first in
                              // the list.
    User::Free( configList ); // Free configList, not used anymore.

    // Create a window where the graphics are blitted
    iEglSurface = eglCreateWindowSurface( iEglDisplay, Config, &Window(), NULL );
    if ( iEglSurface == NULL )
        {
        _LIT(KCreateWindowSurfaceFailed, "eglCreateWindowSurface failed");
        User::Panic( KCreateWindowSurfaceFailed, 0 );
        }

    // Create a rendering context
    iEglContext = eglCreateContext( iEglDisplay, Config, EGL_NO_CONTEXT, NULL );
    if ( iEglContext == NULL )
        {
        _LIT(KCreateContextFailed, "eglCreateContext failed");
        User::Panic( KCreateContextFailed, 0 );
        }

    /* Make the context current. Binds context to the current rendering thread
       and surface.*/
    if ( eglMakeCurrent( iEglDisplay, iEglSurface, iEglSurface, iEglContext )
        == EGL_FALSE )
        {
        _LIT(KMakeCurrentFailed, "eglMakeCurrent failed");
        User::Panic( KMakeCurrentFailed, 0 );
        }


    TSize size;
    size = this->Size();

    iBlenderLite = CBlenderLite::NewL( size.iWidth, size.iHeight, iInputHandler ); // Create an instance of BlenderLite
    iBlenderLite->AppInit();                                       // Initialize OpenGL ES

    iOpenGlInitialized = ETrue;

    iPeriodic = CPeriodic::NewL( CActive::EPriorityIdle );         // Create an active object for
                                                                  // animating the scene
    iPeriodic->Start( 100, 100,
                      TCallBack( CBlenderLiteContainer::DrawCallBack, this ) );
    }

// ------------------------------------------------------------------------------
// CBillboardContainer::OfferKeyEventL()
// Handles joystick movements.
// ------------------------------------------------------------------------------
//
TKeyResponse CBlenderLiteContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType ){
	switch( aType )
		{
		case EEventKeyDown:
			{
			return iInputHandler->KeyDown( aKeyEvent.iScanCode );
			}
		case EEventKeyUp:
			{
			return iInputHandler->KeyUp( aKeyEvent.iScanCode );
			}
		}
	return EKeyWasNotConsumed;
}

// Destructor
CBlenderLiteContainer::~CBlenderLiteContainer()
    {
    delete iPeriodic;

    if ( iBlenderLite )
        {
        iBlenderLite->AppExit();
        delete iBlenderLite;
        }
    delete iInputHandler;
    
    eglMakeCurrent( iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
    eglDestroySurface( iEglDisplay, iEglSurface );
    eglDestroyContext( iEglDisplay, iEglContext );
    eglTerminate( iEglDisplay );                   // Release resources associated
                                                   // with EGL and OpenGL ES
    }

// ---------------------------------------------------------
// CBlenderLiteContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CBlenderLiteContainer::SizeChanged(){
    if( iOpenGlInitialized && iBlenderLite )
        {
        TSize size;
        size = this->Size();

        iBlenderLite->SetScreenSize( size.iWidth, size.iHeight );
        }
    }


// ---------------------------------------------------------
// CBlenderLiteContainer::HandleResourceChange(
//     TInt aType)
// Dynamic screen resize changes by calling the
// SetExtentToWholeScreen() method again.
// ---------------------------------------------------------
//
 void CBlenderLiteContainer::HandleResourceChange(TInt aType)
    {
	switch( aType )
    	{
	    case KEikDynamicLayoutVariantSwitch:
		    SetExtentToWholeScreen();
		    break;
	    }
    }

// ---------------------------------------------------------
// CBlenderLiteContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CBlenderLiteContainer::CountComponentControls() const
    {
    return 0; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CBlenderLiteContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CBlenderLiteContainer::ComponentControl(TInt /*aIndex*/ ) const
    {
    return NULL;
    }

// ---------------------------------------------------------
// CBlenderLiteContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CBlenderLiteContainer::Draw(const TRect& /*aRect*/ ) const
    {
    // No need to implement anything here!
    }

// ---------------------------------------------------------
// CBlenderLiteContainer::DrawCallBack( TAny* aInstance )
// Called by the CPeriodic in order to draw the graphics
// ---------------------------------------------------------
//
int CBlenderLiteContainer::DrawCallBack( TAny* aInstance )
    {
    CBlenderLiteContainer* instance = (CBlenderLiteContainer*) aInstance;
    instance->iFrame++;

    // Compute the elapsed time in seconds since the startup of the example
#ifdef __WINS__

    // In the emulator the tickcount runs at 200Hz
    GLfloat timeSecs = ( (GLfloat) ( User::NTickCount() - instance->iStartTimeTicks ) ) / 200.f;

#else

    // In the device the tickcount runs at 1000hz (as intended)
    GLfloat timeSecs = ( (GLfloat) ( User::NTickCount() - instance->iStartTimeTicks ) ) / 1000.f;

#endif

    // Compute the elapsed time since last frame was drawn. Note that due to the
    // resolution of the system timer this value can be 0.0 even though new frames are being rendered.
    // This applies especially when running the emulator on a PC.
    GLfloat deltaTimeSecs = timeSecs - instance->iLastFrameTimeSecs;

    // Set the current time to be the last frame time for the upcoming frame
    instance->iLastFrameTimeSecs = timeSecs;

    // Call the main OpenGL ES Symbian rendering 'loop'
    instance->iBlenderLite->AppCycle( instance->iFrame, timeSecs, deltaTimeSecs );

    // Call eglSwapBuffers, which blit the graphics to the window
    eglSwapBuffers( instance->iEglDisplay, instance->iEglSurface );

    // To keep the background light on
    if ( !(instance->iFrame%100) )
        {
        User::ResetInactivityTime();
        }

    /* Suspend the current thread for a short while. Give some time
       to other threads and AOs, avoids the ViewSrv error in ARMI and
       THUMB release builds. One may try to decrease the callback
       function instead of this. */
    if ( !(instance->iFrame%50) )
         {
         User::After(0);
         }

    return 0;
    }

// ---------------------------------------------------------
// CBlenderLiteContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CBlenderLiteContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }

// End of File
