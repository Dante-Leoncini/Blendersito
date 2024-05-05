/*
 * ==============================================================================
 *  Name        : BlenderLiteAppUi.cpp
 *  Part of     : OpenGLEx / BlenderLite
 * ==============================================================================
 */

// INCLUDE FILES
#include "BlenderLiteAppUi.h"
#include "BlenderLiteContainer.h"
#include <BlenderLite.rsg>
#include "blenderlite.hrh"

#include <eikmenup.h>
#include <avkon.hrh>
#include "Mesh.h"

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
enum{
	navegacion,
	rotacion,
	escala,
	translacion, 
	translacionVertex,
	translacionEdge,
	translacionFace,
	edicion
};

enum {Solid, MaterialPreview, Wireframe, Rendered};

void CBlenderLiteAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane ){
	//oculta el SetOrigen si no esta en modo edicion
    if (aResourceId == R_BLENDERLITE_MENU ) {
        if (iAppContainer->iBlenderLite->estado == edicion) {
            aMenuPane->SetItemDimmed(EBlenderLiteOrigenSetOrigen, EFalse);
            aMenuPane->SetItemDimmed(EBlenderLiteSeleccionar, EFalse);
            aMenuPane->SetItemDimmed(EViewportObject, ETrue);
            aMenuPane->SetItemDimmed(EViewportAdd, ETrue);
            aMenuPane->SetItemDimmed(EImportOBJ, ETrue);
            aMenuPane->SetItemDimmed(EBlenderLiteWidescreen, ETrue);
            
        } else {
            aMenuPane->SetItemDimmed(EBlenderLiteOrigenSetOrigen, ETrue);
            aMenuPane->SetItemDimmed(EBlenderLiteSeleccionar, ETrue);
            aMenuPane->SetItemDimmed(EViewportObject, EFalse);
            aMenuPane->SetItemDimmed(EViewportAdd, EFalse);
            aMenuPane->SetItemDimmed(EImportOBJ, EFalse);
            aMenuPane->SetItemDimmed(EBlenderLiteWidescreen, EFalse);
        }
        // Texto para setear la pantalla ancha
        if ( iAppContainer->iBlenderLite->iWidescreenEnabled ){
            aMenuPane->SetItemTextL( EBlenderLiteWidescreen, R_WIDESCREEN_OFF );
            }
        else {
            aMenuPane->SetItemTextL( EBlenderLiteWidescreen, R_WIDESCREEN_ON  );
        } 
    }
    else if (aResourceId == R_VIEWPORT_OVERLEY_MENU ) {
        if ( iAppContainer->iBlenderLite->showOverlays ){
            aMenuPane->SetItemTextL( EViewportSetOverlay, R_HIDEOVERLAY );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSetOverlay, R_SHOWOVERLAY  );
        } 
        if ( iAppContainer->iBlenderLite->show3DCursor ){
            aMenuPane->SetItemTextL( EViewportSet3DCursor, R_HIDE3DCURSOR );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSet3DCursor, R_SHOW3DCURSOR  );
        } 

        if ( iAppContainer->iBlenderLite->showFloor ){
            aMenuPane->SetItemTextL( EViewportSetShowFloor, R_HIDEFLOOR );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSetShowFloor, R_SHOWFLOOR  );
        } 
        if ( iAppContainer->iBlenderLite->showYaxis ){
            aMenuPane->SetItemTextL( EViewportSetShowYaxis, R_HIDEYAXIS );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSetShowYaxis, R_SHOWYAXIS  );
        } 
        if ( iAppContainer->iBlenderLite->showXaxis ){
            aMenuPane->SetItemTextL( EViewportSetShowXaxis, R_HIDEXAXIS );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSetShowXaxis, R_SHOWXAXIS  );
        } 
        if ( iAppContainer->iBlenderLite->showOutlineSelect ){
            aMenuPane->SetItemTextL( EViewportSetOutlineSelect, R_HIDEOUTLINESELECT );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSetOutlineSelect, R_SHOWOUTLINESELECT  );
        } 
        if ( iAppContainer->iBlenderLite->showOrigins ){
            aMenuPane->SetItemTextL( EViewportSetOrigins, R_HIDEORIGIN );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSetOrigins, R_SHOWORIGIN  );
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
    monkey,
    claude,
    marcus,
    vertice
};

enum{
	top,
	front,
	right
};

enum{
	array,
	mirror,
	screw
};

enum{
	vertexSelect, edgeSelect, faceSelect
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
            iAppContainer->iBlenderLite->SetMouse();
            break;
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        //nuevos!!!      
        case EViewportSetShowFloor:
            iAppContainer->iBlenderLite->ToggleValue(iAppContainer->iBlenderLite->showFloor);
            break;       
        case EViewportSetShowYaxis:
            iAppContainer->iBlenderLite->ToggleValue(iAppContainer->iBlenderLite->showYaxis);
            break;       
        case EViewportSetShowXaxis:
            iAppContainer->iBlenderLite->ToggleValue(iAppContainer->iBlenderLite->showXaxis);
            break;       
        case EViewportSetOutlineSelect:
            iAppContainer->iBlenderLite->ToggleValue(iAppContainer->iBlenderLite->showOutlineSelect);
            break;       
        case EViewportSetOrigins:
            iAppContainer->iBlenderLite->ToggleValue(iAppContainer->iBlenderLite->showOrigins);
            break;          
        case EViewportSetOverlay:
            iAppContainer->iBlenderLite->ToggleValue(iAppContainer->iBlenderLite->showOverlays);
            break;       
        case EViewportSet3DCursor:
            iAppContainer->iBlenderLite->ToggleValue(iAppContainer->iBlenderLite->show3DCursor);
            break; 
        case EImportOBJ:
            iAppContainer->iBlenderLite->ImportOBJ();
            break; 
        case EBlenderLiteWidescreen:
            iAppContainer->SetWidescreen();
            break;    
        case EViewportCursorToSelect:
            iAppContainer->iBlenderLite->CursorToSelect();
            break;
        case EAddCamera:            
            iAppContainer->iBlenderLite->AddObject(camera);
            break;   
        case EAddLight:            
            iAppContainer->iBlenderLite->AddObject(light);
            break;       
        case EAddEmpty:            
            iAppContainer->iBlenderLite->AddObject(empty);
            break;
        case EAddCube:            
            iAppContainer->iBlenderLite->AddMesh(cubo);
            break;  
        case EAddMonkey:
            iAppContainer->iBlenderLite->AddMesh(monkey);
            break;  
        case EAddVertex:
            iAppContainer->iBlenderLite->AddMesh(vertice);
            break; 
        case ESetMaterial:
            iAppContainer->iBlenderLite->SetMaterial();
            break;  
        case ESetSpecular:
            iAppContainer->iBlenderLite->SetSpecular();
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
        case EDuplicatedObject:
            iAppContainer->iBlenderLite->DuplicatedObject();
            break;       
        case EDuplicatedLinked:
            iAppContainer->iBlenderLite->DuplicatedLinked();
            break; 
        case ESetDiffuse:
            iAppContainer->iBlenderLite->SetDiffuse();
            break;   
        case ESetEmission:
            iAppContainer->iBlenderLite->SetEmission();
            break;  
        case ESetTexture:
            iAppContainer->iBlenderLite->SetTexture();
            break;  
        case ESetActiveTexture:
            iAppContainer->iBlenderLite->ActivarTextura();
            break; 
        case ESetInterpolation:
            iAppContainer->iBlenderLite->SetInterpolation();
            break; 
        case ESetTransparent:
            iAppContainer->iBlenderLite->SetTransparencia();
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
            iAppContainer->iBlenderLite->EnfocarObject();
            break; 
        case EBlenderLiteViewMaterial:
            iAppContainer->iBlenderLite->SetShading(MaterialPreview);
            break; 
        case EBlenderLiteViewSolid:
            iAppContainer->iBlenderLite->SetShading(Solid);
            break; 
        case EBlenderLiteViewWireframe:
            iAppContainer->iBlenderLite->SetShading(Wireframe);
            break; 
        /*case EBlenderLiteViewRendered:
            iAppContainer->iBlenderLite->SetShading(Rendered);
            break; */
        case EBlenderLiteSetTipoVertex:
            iAppContainer->iBlenderLite->SetTipoSelect(vertexSelect);
            break; 
        case EBlenderLiteSetTipoEdge:
            iAppContainer->iBlenderLite->SetTipoSelect(edgeSelect);
            break; 
        case EBlenderLiteSetTipoFace:
            iAppContainer->iBlenderLite->SetTipoSelect(faceSelect);
            break; 
        case EBlenderLiteOrigenToGeometry:
            iAppContainer->iBlenderLite->SetOrigen(0);
            break; 
        case EBlenderLiteOrigenToSelect:
            iAppContainer->iBlenderLite->SetOrigen(1);
            break; 
        case EBlenderLiteOrigenToCursor:
            iAppContainer->iBlenderLite->SetOrigen(2);
            break; 
        case EBlenderLiteCambiarObject:
            iAppContainer->iBlenderLite->changeSelect();
            break; 
        case EBlenderLiteModificadorArray:
            iAppContainer->iBlenderLite->AddModificador(array);
            break; 
        case EBlenderLiteModificadorMirror:
            iAppContainer->iBlenderLite->AddModificador(mirror);
            break; 
        case EBlenderLiteModificadorScrew:
            iAppContainer->iBlenderLite->AddModificador(screw);
            break; 
        case ENewTexture:
            iAppContainer->iBlenderLite->NewTexture();
            break; 
            
            //case EBlenderLiteNavegacion:
            //iAppContainer->iBlenderLite->SetMode();
            //break;            
        default:
            break;
        }
    }

// End of File
