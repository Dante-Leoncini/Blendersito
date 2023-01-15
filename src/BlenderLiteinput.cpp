/*
 * ==============================================================================
 *  Name        : BlenderLiteinput.cpp
 *  Part of     : OpenGLEx / BlenderLite
 * ==============================================================================
 */

// INCLUDE FILES
#include <aknnotewrappers.h>
#include "blenderliteinput.h"

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
// CBlenderLiteInput::NewL
// Symbian 2-phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
CBlenderLiteInput* CBlenderLiteInput::NewL(){
    // Symbian 2-phase constructor. Calls both the default
    // C++ constructor and Symbian ConstructL methods
    CBlenderLiteInput* self = new (ELeave) CBlenderLiteInput();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// ----------------------------------------------------------------------------
// CBlenderLiteInput::CBlenderLiteInput
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlenderLiteInput::CBlenderLiteInput(){
}

// ----------------------------------------------------------------------------
// CBlenderLiteInput::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlenderLiteInput::ConstructL( void ){
    iInputPressed = new (ELeave) TBool[ MAX_NUM_INPUTS ];
    for( int i = 0; i < MAX_NUM_INPUTS; i++ ){
        iInputPressed[i] = EFalse;
    }
}

// ----------------------------------------------------------------------------
// CBlenderLiteInput::~CBlenderLiteInput()
// Destructor.
// ----------------------------------------------------------------------------
//
CBlenderLiteInput::~CBlenderLiteInput(){
    delete iInputPressed;
}

// ----------------------------------------------------------------------------
// CBlenderLiteInput::KeyDown( const int aKeyCode )
// Sets the given keycode to pressed state.
// ----------------------------------------------------------------------------
//
TKeyResponse CBlenderLiteInput::KeyDown( const int aKeyCode ){
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
// CBlenderLiteInput::KeyUp( const int aKeyCode )
// Sets the given keycode to non-pressed state.
// ----------------------------------------------------------------------------
//
TKeyResponse CBlenderLiteInput::KeyUp( const int aKeyCode ){
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
// CBlenderLiteInput::IsInputPressed( Input aInput )
// Returns whether or not a given input is pressed at the moment or not.
// ----------------------------------------------------------------------------
//
TBool CBlenderLiteInput::IsInputPressed( TUint aInput ){
    return iInputPressed[ aInput ];
}


// End of File
