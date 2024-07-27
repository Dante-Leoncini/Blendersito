/*
 * ==============================================================================
 *  Name        : BlendersitoDocument.h
 *  Part of     : OpenGLEx / Blendersito
 * ==============================================================================
 */

#ifndef BLENDERSITODOCUMENT_H
#define BLENDERSITODOCUMENT_H

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
 * Document class that is just used as the container for the application
 * (as required by the Symbian UI application architecture).
 */
class CBlendersitoDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
         * Factory method for creating a new CBlendersitoDocument object.
         */
        static CBlendersitoDocument* NewL(CEikApplication& aApp);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CBlendersitoDocument();

    private:  // Functions from base classes

        /**
         * C++ constructor. Just passes the given application reference to the baseclass.
         */
        CBlendersitoDocument(CEikApplication& aApp);

        /**
         * Second phase constructor. Does nothing.
         */
        void ConstructL();

    private: // New functions

        /**
         * From CEikDocument, creates and returns CBlendersitoAppUi application UI object.
         */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

