/*
 * ==============================================================================
 *  Name        : BlenderLiteDocument.h
 *  Part of     : OpenGLEx / BlenderLite
 *
 *  Copyright (c) 2004-2006 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation.
 * ==============================================================================
 */

#ifndef BLENDERLITEDOCUMENT_H
#define BLENDERLITEDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
 * Document class that is just used as the container for the application
 * (as required by the Symbian UI application architecture).
 */
class CBlenderLiteDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
         * Factory method for creating a new CBlenderLiteDocument object.
         */
        static CBlenderLiteDocument* NewL(CEikApplication& aApp);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CBlenderLiteDocument();

    private:  // Functions from base classes

        /**
         * C++ constructor. Just passes the given application reference to the baseclass.
         */
        CBlenderLiteDocument(CEikApplication& aApp);

        /**
         * Second phase constructor. Does nothing.
         */
        void ConstructL();

    private: // New functions

        /**
         * From CEikDocument, creates and returns CBlenderLiteAppUi application UI object.
         */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

