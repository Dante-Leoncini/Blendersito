/*
 * ==============================================================================
 *  Name        : Blendersitoinput.h
 *  Part of     : OpenGLEx / Blendersito
 * ==============================================================================
 */

#ifndef __BLENDERSITOINPUTH__
#define __BLENDERSITOINPUTH__

//  INCLUDES

#include <e32base.h> // for CBase definition
#include <GLES\gl.h> // OpenGL ES header file
#include <aknappui.h>

// MACROS

// Number of inputs (used to create the lookup table)
#define MAX_NUM_INPUTS   5

/**
 * Defined joystick values that can be queried from this class.
 */
enum
{
EJoystickUp,
EJoystickDown,
EJoystickLeft,
EJoystickRight,
//EJoystickFire,
//EDelete,
//EShift,
//EMenu,
EVolumenUp,
EVolumenDown,
};

/** Size of the keycode map. */
static const TUint KKeyCodeMapSize = 10 * 2;

/**
 * The keycode map defines which scancode is mapped to which input signal.
 */
static const TUint KKeyCodeMap[] = {
    EStdKeyUpArrow,    EJoystickUp,
    EStdKeyDownArrow,  EJoystickDown,
    EStdKeyLeftArrow,  EJoystickLeft,
    EStdKeyRightArrow, EJoystickRight,
    //EStdKeyDevice3,    EJoystickFire,
    //EKeyDelete,        EDelete,
    //EKeyLeftShift,     EShift,
    EKeyIncVolume,     EVolumenUp,                
    EKeyDecVolume,     EVolumenDown,
    //EKeyMenu,          EMenu
};


// FORWARD DECLARATIONS

/**
 * Class that handles the mapping of scancodes to input signals that can then
 * be queried from the application. When the program is initialized
 * this class reference is given by the CBlendersitoContainer to CBlendersito. CBlendersitoContainer
 * class then notifies this class of key presses and releases. If an input signal
 * mapping exists for a scancode the state of that input signal's state is modified.
 * The CBlendersito class can then query the state of the various input signal's from this class.
 *
 * Note that this class supports mapping of multiple scancodes to a single input signal.
 * But only one input signal can be mapped to one scancode (that means single keypress can't
 * change multiple input signals to pressed state).
 *
 */
 class CBlendersitoInput
    {
    public: // Constructors and destructor

        /**
         * Factory method for creating a new CBlendersito object.
         * Calls the hidden constructors.
         */
        static CBlendersitoInput* NewL();

        /**
         * Destructor. Does nothing.
         */
        virtual ~CBlendersitoInput();

    protected: // Hidden constructors

        /**
         * Standard constructor that must never Leave.
         */
        CBlendersitoInput();

        /**
         * Second phase constructor that can call methods that may leave. Does nothing.
         */
        void ConstructL( void );


    public: // New methods

        /**
         * If a signal mapping exists for the given keycode, then that
         * input signal is set to pressed state.
         * @param aKeyCode One of the values defined in TStdScanCode.
         */
        TKeyResponse KeyDown( const int aKeyCode );

        /**
         * If a signal mapping exists for the given keycode, then that
         * input signal is set to non-pressed state.
         * @param aKeyCode One of the values defined in TStdScanCode.
         */
        TKeyResponse KeyUp( const int aKeyCode );

        /**
         * Returns whether or not a given input signal is pressed at the moment or not.
         * @param aInput One of the defined input signal values.
         * @return ETrue if the given input is pressed or EFalse if it is not pressed.
         */
        TBool IsInputPressed( TUint aInput );

    private: // Data

        /** Pressed status for each input. */
		TBool* iInputPressed;
    };

#endif

// End of File
