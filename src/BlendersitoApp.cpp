/*
 * ==============================================================================
 *  Name        : BlendersitoApp.cpp
 *  Part of     : OpenGLEx / Blendersito
 * ==============================================================================
 */

// INCLUDE FILES
#include    <eikstart.h>
#include    "BlendersitoApp.h"
#include    "BlendersitoDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBlendersitoApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CBlendersitoApp::AppDllUid() const {
    return KUidBlendersito;
}


// ---------------------------------------------------------
// CBlendersitoApp::CreateDocumentL()
// Creates CBlendersitoDocument object
// ---------------------------------------------------------
//
CApaDocument* CBlendersitoApp::CreateDocumentL(){
    return CBlendersitoDocument::NewL( *this );
}


// ================= OTHER EXPORTED FUNCTIONS ==============


CApaApplication* NewApplication(){
    return new CBlendersitoApp;
}

TInt E32Main(){
	return EikStart::RunApplication(NewApplication);
}

// End of File

