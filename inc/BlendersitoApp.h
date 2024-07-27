/*
 * ==============================================================================
 *  Name        : BlendersitoApp.h
 *  Part of     : OpenGLEx / Blendersito
 * ==============================================================================
 */

#ifndef BLENDERSITOAPP_H
#define BLENDERSITOAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
/** UID of the application. */
const TUid KUidBlendersito = { 0xA00D021D };

// CLASS DECLARATION

/**
 * Application class. Provides factory method to create a concrete document object.
 */
class CBlendersitoApp : public CAknApplication
    {
    private: // Functions from base classes

        /**
         * From CApaApplication, creates and returns CBlendersitoDocument document object.
         * @return Pointer to the created document object.
         */
        CApaDocument* CreateDocumentL();

        /**
         * From CApaApplication, returns application's UID (KUidBlendersito).
         * @return Value of KUidBlendersito.
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
