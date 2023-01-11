/*
 * ==============================================================================
 *  Name        : BlenderLiteApp.cpp
 *  Part of     : OpenGLEx / BlenderLite
 *
 *  Copyright (c) 2004-2006 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation.
 * ==============================================================================
 */

// INCLUDE FILES
#include    <eikstart.h>
#include    "BlenderLiteApp.h"
#include    "BlenderLiteDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBlenderLiteApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CBlenderLiteApp::AppDllUid() const {
    return KUidBlenderLite;
}


// ---------------------------------------------------------
// CBlenderLiteApp::CreateDocumentL()
// Creates CBlenderLiteDocument object
// ---------------------------------------------------------
//
CApaDocument* CBlenderLiteApp::CreateDocumentL(){
    return CBlenderLiteDocument::NewL( *this );
}


// ================= OTHER EXPORTED FUNCTIONS ==============


CApaApplication* NewApplication(){
    return new CBlenderLiteApp;
}

TInt E32Main(){
	return EikStart::RunApplication(NewApplication);
}

// End of File

