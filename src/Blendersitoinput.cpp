/*
 * ==============================================================================
 *  Name        : Blendersitoinput.cpp
 *  Part of     : OpenGLEx / Blendersito
 * ==============================================================================
 */

// INCLUDE FILES
#include <aknnotewrappers.h>
#include "blendersitoinput.h"

// MACROS
#define FRUSTUM_LEFT   -1.f     //left vertical clipping plane
#define FRUSTUM_RIGHT  +1.f     //right vertical clipping plane
#define FRUSTUM_BOTTOM -1.f     //bottom horizontal clipping plane
#define FRUSTUM_TOP    +1.f     //top horizontal clipping plane
#define FRUSTUM_NEAR   +3.f     //near depth clipping plane
#define FRUSTUM_FAR    +1000.f  //far depth clipping plane

// CONSTANTS

// ============================= MEMBER FUNCTIONS ==============================
// ============================= LOCAL FUNCTIONS ===============================
// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBlendersitoInput::NewL
// Symbian 2-phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
CBlendersitoInput* CBlendersitoInput::NewL(){
    // Symbian 2-phase constructor. Calls both the default
    // C++ constructor and Symbian ConstructL methods
    CBlendersitoInput* self = new (ELeave) CBlendersitoInput();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// ----------------------------------------------------------------------------
// CBlendersitoInput::CBlendersitoInput
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlendersitoInput::CBlendersitoInput(){
}

// ----------------------------------------------------------------------------
// CBlendersitoInput::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlendersitoInput::ConstructL( void ){
    iInputPressed = new (ELeave) TBool[ MAX_NUM_INPUTS ];
    for( int i = 0; i < MAX_NUM_INPUTS; i++ ){
        iInputPressed[i] = EFalse;
    }
}

// ----------------------------------------------------------------------------
// CBlendersitoInput::~CBlendersitoInput()
// Destructor.
// ----------------------------------------------------------------------------
//
CBlendersitoInput::~CBlendersitoInput(){
    delete iInputPressed;
}

// ----------------------------------------------------------------------------
// CBlendersitoInput::KeyDown( const int aKeyCode )
// Sets the given keycode to pressed state.
// ----------------------------------------------------------------------------
//
TKeyResponse CBlendersitoInput::KeyDown( const int aKeyCode ){
    TInt inputIdx = -1;
    for( int i = 0; i < KKeyCodeMapSize; i +=2 ){
        if( KKeyCodeMap[i] == aKeyCode ){
            inputIdx = KKeyCodeMap[i + 1];
            break;
        }
    }

    if( inputIdx >= 0 ){
        iInputPressed[ inputIdx ] = ETrue;
		return EKeyWasConsumed;
    }

    return EKeyWasNotConsumed;
}

// ----------------------------------------------------------------------------
// CBlendersitoInput::KeyUp( const int aKeyCode )
// Sets the given keycode to non-pressed state.
// ----------------------------------------------------------------------------
//
TKeyResponse CBlendersitoInput::KeyUp( const int aKeyCode ){
    TInt inputIdx = -1;
    for( int i = 0; i < KKeyCodeMapSize; i +=2 ){
        if( KKeyCodeMap[i] == aKeyCode ){
            inputIdx = KKeyCodeMap[i + 1];
            break;
            }
        }

    if( inputIdx >= 0 ){
        iInputPressed[ inputIdx ] = EFalse;
		return EKeyWasConsumed;
    }

    return EKeyWasNotConsumed;
}

// ----------------------------------------------------------------------------
// CBlendersitoInput::IsInputPressed( Input aInput )
// Returns whether or not a given input is pressed at the moment or not.
// ----------------------------------------------------------------------------
//
TBool CBlendersitoInput::IsInputPressed( TUint aInput ){
    return iInputPressed[ aInput ];
}


// End of File
