/*
 * ==============================================================================
 *  Name        : BlenderLiteApp.h
 *  Part of     : OpenGLEx / BlenderLite
 * ==============================================================================
 */

#ifndef BLENDERLITEAPP_H
#define BLENDERLITEAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
/** UID of the application. */
const TUid KUidBlenderLite = { 0xA00D021D };

// CLASS DECLARATION

/**
 * Application class. Provides factory method to create a concrete document object.
 */
class CBlenderLiteApp : public CAknApplication
    {
    private: // Functions from base classes

        /**
         * From CApaApplication, creates and returns CBlenderLiteDocument document object.
         * @return Pointer to the created document object.
         */
        CApaDocument* CreateDocumentL();

        /**
         * From CApaApplication, returns application's UID (KUidBlenderLite).
         * @return Value of KUidBlenderLite.
         */
        TUid AppDllUid() const;
    };

// OTHER EXPORTED FUNCTIONS

/**
 * Factory method used by the E32Main method to create a new application instance.
 */
LOCAL_C CApaApplication* NewApplication();

/**
 * Entry point to the EXE application. Creates new application instance and
 * runs it by giving it as parameter to EikStart::RunApplication() method.
 */
GLDEF_C TInt E32Main();

#endif

// End of File
