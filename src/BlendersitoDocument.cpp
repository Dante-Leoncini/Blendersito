/*
 * ==============================================================================
 *  Name        : BlendersitoDocument.cpp
 *  Part of     : OpenGLEx / Blendersito
 * ==============================================================================
 */

// INCLUDE FILES
#include "BlendersitoDocument.h"
#include "BlendersitoAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CBlendersitoDocument::CBlendersitoDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBlendersitoDocument::CBlendersitoDocument(CEikApplication& aApp)
: CAknDocument(aApp)
    {
    }

// -----------------------------------------------------------------------------
// ?classname::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBlendersitoDocument::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CBlendersitoDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBlendersitoDocument* CBlendersitoDocument::NewL(
        CEikApplication& aApp)     // CBlendersitoApp reference
    {
    CBlendersitoDocument* self = new (ELeave) CBlendersitoDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// destructor
CBlendersitoDocument::~CBlendersitoDocument()
    {
    }

// ----------------------------------------------------
// CBlendersitoDocument::CreateAppUiL()
// constructs CBlendersitoAppUi
// ----------------------------------------------------
//
CEikAppUi* CBlendersitoDocument::CreateAppUiL()
    {
    return new (ELeave) CBlendersitoAppUi;
    }

// End of File
