/*
 * ==============================================================================
 *  Name        : BlenderLiteDocument.cpp
 *  Part of     : OpenGLEx / BlenderLite
 * ==============================================================================
 */

// INCLUDE FILES
#include "BlenderLiteDocument.h"
#include "BlenderLiteAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CBlenderLiteDocument::CBlenderLiteDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBlenderLiteDocument::CBlenderLiteDocument(CEikApplication& aApp)
: CAknDocument(aApp)
    {
    }

// -----------------------------------------------------------------------------
// ?classname::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBlenderLiteDocument::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CBlenderLiteDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBlenderLiteDocument* CBlenderLiteDocument::NewL(
        CEikApplication& aApp)     // CBlenderLiteApp reference
    {
    CBlenderLiteDocument* self = new (ELeave) CBlenderLiteDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// destructor
CBlenderLiteDocument::~CBlenderLiteDocument()
    {
    }

// ----------------------------------------------------
// CBlenderLiteDocument::CreateAppUiL()
// constructs CBlenderLiteAppUi
// ----------------------------------------------------
//
CEikAppUi* CBlenderLiteDocument::CreateAppUiL()
    {
    return new (ELeave) CBlenderLiteAppUi;
    }

// End of File
