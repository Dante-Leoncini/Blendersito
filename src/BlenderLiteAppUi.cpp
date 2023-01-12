/*
 * ==============================================================================
 *  Name        : BlenderLiteAppUi.cpp
 *  Part of     : OpenGLEx / BlenderLite
 *
 *  Copyright (c) 2004-2006 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation.
 * ==============================================================================
 */

// INCLUDE FILES
#include "BlenderLiteAppUi.h"
#include "BlenderLiteContainer.h"
#include <BlenderLite.rsg>
#include "blenderlite.hrh"

#include <eikmenup.h>
#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CBlenderLiteAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------
//
void CBlenderLiteAppUi::ConstructL(){
    BaseConstructL();
    iAppContainer = new (ELeave) CBlenderLiteContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );
}

// Destructor
CBlenderLiteAppUi::~CBlenderLiteAppUi(){
	if ( iAppContainer ){
		RemoveFromStack( iAppContainer );
		delete iAppContainer;
	}
}

// ------------------------------------------------------------------------------
//  CBlenderLiteAppUi::DynInitMenuPaneL
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CBlenderLiteAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane ){
    // Item texts for the lighting menu.
    if ( aResourceId == R_BLENDERLITE_LIGHTING_MENU )
        {
        // Text for switching the lights on/off.
        if ( iAppContainer->iBlenderLite->iLightingEnabled )
            {
            aMenuPane->SetItemTextL( EBlenderLiteLights, R_LIGHTS_OFF );
            }
        else
            {
            aMenuPane->SetItemTextL( EBlenderLiteLights, R_LIGHTS_ON  );
            }

        // Text for switching the lamp on/off.
        if ( iAppContainer->iBlenderLite->iLampEnabled )
            {
            aMenuPane->SetItemTextL( EBlenderLiteLamp, R_LAMP_OFF );
            }
        else
            {
            aMenuPane->SetItemTextL( EBlenderLiteLamp, R_LAMP_ON );
            }

        // Text for switching the spot on/off.
        if ( iAppContainer->iBlenderLite->iSpotEnabled )
            {
            aMenuPane->SetItemTextL( EBlenderLiteSpot, R_SPOT_OFF );
            }
        else {
            aMenuPane->SetItemTextL( EBlenderLiteSpot, R_SPOT_ON );
		}
	}
}

// ----------------------------------------------------
// CBlenderLiteAppUi::HandleKeyEventL
// Key event handler
// ----------------------------------------------------
//
TKeyResponse CBlenderLiteAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType ){
    if ( iAppContainer->iBlenderLite->GetState() == CBlenderLite::ERunning ){
		if( aType == EEventKeyDown ){
			TUint scan = aKeyEvent.iScanCode;
			switch(scan){
			case(22): //Shift
				iAppContainer->iBlenderLite->Tab(); //cambiar objeto
				return EKeyWasConsumed;
			case(18): //left Shift
				iAppContainer->iBlenderLite->Tab(); //cambiar objeto
				return EKeyWasConsumed;
			case(19): //rigth Shift
				iAppContainer->iBlenderLite->Tab(); //cambiar objeto
				return EKeyWasConsumed;
			case(1): //Delete
				iAppContainer->iBlenderLite->Borrar();
				return EKeyWasConsumed;
			case(49): //1
				iAppContainer->iBlenderLite->TecladoNumerico(1);
				return EKeyWasConsumed;
			case(50): //2
				iAppContainer->iBlenderLite->TecladoNumerico(2);
				return EKeyWasConsumed;
			case(51): //3
				iAppContainer->iBlenderLite->TecladoNumerico(3);
				return EKeyWasConsumed;
			case(52): //4
				iAppContainer->iBlenderLite->TecladoNumerico(4);
				return EKeyWasConsumed;
			case(53): //5
				iAppContainer->iBlenderLite->TecladoNumerico(5);
				return EKeyWasConsumed;
			case(54): //6
				iAppContainer->iBlenderLite->TecladoNumerico(6);
				return EKeyWasConsumed;
			case(55): //7
				iAppContainer->iBlenderLite->TecladoNumerico(7);
				return EKeyWasConsumed;
			case(56): //8
				iAppContainer->iBlenderLite->TecladoNumerico(8);
				return EKeyWasConsumed;
			case(57): //9
				iAppContainer->iBlenderLite->TecladoNumerico(9);
				return EKeyWasConsumed;
			case(48): //0
				iAppContainer->iBlenderLite->TecladoNumerico(0);
				return EKeyWasConsumed;
			case(42): //*
				iAppContainer->iBlenderLite->TecladoNumerico(10);
				return EKeyWasConsumed;
			case(127): //#
				iAppContainer->iBlenderLite->TecladoNumerico(11);
				return EKeyWasConsumed;
			case(226): //camara
				iAppContainer->iBlenderLite->Extruir();
				return EKeyWasConsumed;
			case(196): //llamada
				iAppContainer->iBlenderLite->SetPosicion();
				return EKeyWasConsumed;
			case(71): //G
				iAppContainer->iBlenderLite->SetPosicion();
				return EKeyWasConsumed;
			case(82): //R
				iAppContainer->iBlenderLite->SetRotacion();
				return EKeyWasConsumed;
			case(83): //S
				iAppContainer->iBlenderLite->SetEscala();
				return EKeyWasConsumed;
			case(81): //Q
				iAppContainer->iBlenderLite->Cancelar();
				return EKeyWasConsumed;
			case(3): //Enter
				iAppContainer->iBlenderLite->Aceptar();
				return EKeyWasConsumed;
			case(167): //OK
				iAppContainer->iBlenderLite->Aceptar();
				return EKeyWasConsumed;
			case(88): //X
				iAppContainer->iBlenderLite->SetEje(0);
				return EKeyWasConsumed;
			case(89): //Y
				iAppContainer->iBlenderLite->SetEje(1);
				return EKeyWasConsumed;
			case(90): //Z
				iAppContainer->iBlenderLite->SetEje(2);
				return EKeyWasConsumed;
				
			//case(16): //arriba
				//iAppContainer->iBlenderLite->NextPos(0,1);
				//return EKeyWasConsumed;
			//case(15): //derecha
				//iAppContainer->iBlenderLite->Rotar(2);
				//return EKeyWasConsumed;
			//case(17): //abajo
				//iAppContainer->iBlenderLite->NextPos(8,1);
				//return EKeyWasConsumed;
			//case(14): //izquierda
				//iAppContainer->iBlenderLite->Rotar(1);
				//return EKeyWasConsumed;
			default:
				return EKeyWasNotConsumed;
			}
		}
	}
    return EKeyWasNotConsumed;
}

enum{
	cubo,
    esfera,
    cilindro,
    plano,
    vacio,
    camara,
    cad,
    luz,
    suzanne,
    claude,
    marcus,
    vertice
};

enum{
	top,
	front,
	right
};

// ----------------------------------------------------
// CBlenderLiteAppUi::HandleCommandL
// Command handler
// ----------------------------------------------------
//
void CBlenderLiteAppUi::HandleCommandL(TInt aCommand){
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        case EBlenderLiteLights:
            iAppContainer->iBlenderLite->ToggleLighting();
            break;
        case EBlenderLiteLamp:
            iAppContainer->iBlenderLite->ToggleLamp();
            break;
        case EBlenderLiteSpot:
            iAppContainer->iBlenderLite->ToggleSpot();
            break;
        //nuevos!!!
        case EBlenderLiteCrearCubo:
            iAppContainer->iBlenderLite->CrearObjeto(cubo);
            break;  
        case EBlenderLiteCrearClaude:
            iAppContainer->iBlenderLite->CrearObjeto(claude);
            break;  
        case EBlenderLiteCrearSuzanne:
            iAppContainer->iBlenderLite->CrearObjeto(suzanne);
            break;  
        case EBlenderLiteCrearVertice:
            iAppContainer->iBlenderLite->CrearObjeto(vertice);
            break;  
        case EBlenderLiteSetSpecular:
            iAppContainer->iBlenderLite->SetSpecular();
            break;  
        case EBlenderLiteSetSmooth:
            iAppContainer->iBlenderLite->SetSmooth();
            break; 
        case EBlenderLiteEditarMesh:
            iAppContainer->iBlenderLite->SetEditMode();
            break;  
        case EBlenderLiteSetEscala:
            iAppContainer->iBlenderLite->SetEscala();
            break;   
        case EBlenderLiteSetPosicion:
            iAppContainer->iBlenderLite->SetPosicion();
            break;   
        case EBlenderLiteSetRotacion:
            iAppContainer->iBlenderLite->SetRotacion();
            break;    
        case EBlenderLiteSetDiffuse:
            iAppContainer->iBlenderLite->SetDiffuse();
            break;   
        case EBlenderLiteSetEmission:
            iAppContainer->iBlenderLite->SetEmission();
            break;  
        case EBlenderLiteSetTexture:
            iAppContainer->iBlenderLite->SetTexture();
            break;  
        case EBlenderLiteActivarTextura:
            iAppContainer->iBlenderLite->ActivarTextura();
            break; 
        case EBlenderLiteInfoObject:
            iAppContainer->iBlenderLite->InfoObject(1);
            break; 
        case EBlenderLiteBorrar:
            iAppContainer->iBlenderLite->Borrar();
            break;  
        case EBlenderLitePerspectiva:
            iAppContainer->iBlenderLite->SetPerspectiva();
            break;
        case EBlenderLiteTopView:
            iAppContainer->iBlenderLite->SetViewpoint(top);
            break;
        case EBlenderLiteFrontView:
            iAppContainer->iBlenderLite->SetViewpoint(front);
            break;
        case EBlenderLiteRightView:
            iAppContainer->iBlenderLite->SetViewpoint(right);
            break;
        case EBlenderLiteObjFocus:
            iAppContainer->iBlenderLite->EnfocarObjeto();
            break; 
        case EBlenderLiteViewMaterial:
            iAppContainer->iBlenderLite->SetShader(0);
            break; 
        case EBlenderLiteViewSolid:
            iAppContainer->iBlenderLite->SetShader(1);
            break; 
        case EBlenderLiteViewWireframe:
            iAppContainer->iBlenderLite->SetShader(2);
            break; 
        case EBlenderLiteCambiarObjeto:
            iAppContainer->iBlenderLite->changeSelect();
            break; 
            //case EBlenderLiteNavegacion:
            //iAppContainer->iBlenderLite->SetMode();
            //break;            
        default:
            break;
        }
    }

// End of File
