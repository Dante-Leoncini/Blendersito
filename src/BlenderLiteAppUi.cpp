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

#include <aknconsts.h>

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
    //para trabajar mas facil
    CBlenderLite& BL = *(iAppContainer->iBlenderLite); 
    //BL.estado = ...;

    //prepara el menu de materiales    
	/*if (aResourceId == R_MATERIAL_MENU){
	//if (aResourceId == R_LISTMATERIALS_MENU){
        // Eliminar todos los elementos de men� existentes uno por uno
        for (TInt i = aMenuPane->NumberOfItemsInPane() - 1; i >= 0; --i)
        {
            aMenuPane->DeleteMenuItem(aMenuPane->MenuItemCommandId(i));
        }      
        // Definir una cadena de caracteres Unicode de 16 bits
        const TUint16 unicodeText[] = { 'N', 'o', 'm', 'b', 'r', 'e', ' ', 'd', 'e', 'l', ' ', 'e', 'l', 'e', 'm', 'e', 'n', 't', 'o', ' ', 'd', 'e', ' ', 'm', 'e', 'n', '�', '\0' };

        // Inicializar TPtrC16 con la cadena de caracteres Unicode de 16 bits
        //TPtrC16 itemText(unicodeText);
        TPtrC8 itemText = L"Nombre del elemento de men�";

		TBlenderLiteMenuCommands menuCommand = EBlenderLiteOrigenSetOrigen;  // Suponiendo que EBlenderLiteCommand es una constante o enumeraci�n v�lida
		
		CEikMenuPaneItem::SData menuItemData;
		menuItemData.iCommandId = menuCommand;  // Asigna el ID de comando de tu men�
		menuItemData.iText = itemText;
				
		aMenuPane->InsertMenuItemL(menuItemData, 2);
        
        // Agrega los nombres de materiales como elementos de men�
		//for (TInt i = 0; i < iAppContainer->iBlenderLite->Materials.Count(); ++i){
			//TInt commandId = EAknCmdEmpty + i; // Asigna un ID de comando �nico para cada elemento de men�
		    //_LIT(KTitle, "Selecciona la Textura");
			//aMenuPane->AddMenuItemL(TPtrC(KTitle), EBlenderLiteOrigenSetOrigen);
		//}
    }*/
	//oculta el SetOrigen si no esta en modo edicion
    if (aResourceId == R_BLENDERLITE_MENU ) {
        if (BL.estado == edicion) {
	        if (BL.Objects.Count() > 0){
                aMenuPane->SetItemDimmed(EViewportObject, EFalse);
            }
            else {
                aMenuPane->SetItemDimmed(EViewportObject, ETrue);
            }
            aMenuPane->SetItemDimmed(EBlenderLiteOrigenSetOrigen, EFalse);
            aMenuPane->SetItemDimmed(EBlenderLiteSeleccionar, EFalse);
            aMenuPane->SetItemDimmed(EViewportAdd, ETrue);
            
        } else {
            aMenuPane->SetItemDimmed(EBlenderLiteOrigenSetOrigen, ETrue);
            aMenuPane->SetItemDimmed(EBlenderLiteSeleccionar, ETrue);
            aMenuPane->SetItemDimmed(EViewportAdd, EFalse);

            //si hay objetos            
	        if (BL.Objects.Count() > 0){
                aMenuPane->SetItemDimmed(EViewportObject, EFalse);          
                Object& obj = BL.Objects[BL.objSelect];
                //si es una malla 3d
                if (obj.type == mesh){
                    aMenuPane->SetItemDimmed(EMaterial, EFalse);
                    aMenuPane->SetItemDimmed(EBlenderLiteModificadores, EFalse);
                }
                else {
                    aMenuPane->SetItemDimmed(EMaterial, ETrue);
                    aMenuPane->SetItemDimmed(EBlenderLiteModificadores, ETrue);                    
                }
            }
            else {
                aMenuPane->SetItemDimmed(EMaterial, ETrue);
                aMenuPane->SetItemDimmed(EViewportObject, ETrue);
            }
        }
    }
    else if (aResourceId == R_VIEWPORT_OVERLEY_MENU ) {
        // Texto para setear la pantalla ancha
        if ( iAppContainer->iBlenderLite->iWidescreenEnabled ){
            aMenuPane->SetItemTextL( EBlenderLiteWidescreen, R_WIDESCREEN_OFF );
            }
        else {
            aMenuPane->SetItemTextL( EBlenderLiteWidescreen, R_WIDESCREEN_ON  );
        }
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
    else if (aResourceId == R_TIMELINE_MENU ) {
        if ( iAppContainer->iBlenderLite->PlayAnimation ){
            aMenuPane->SetItemTextL( ETimelinePlayStop, R_STOP );
        }
        else {
            aMenuPane->SetItemTextL( ETimelinePlayStop, R_PLAY );
        }
        if ( iAppContainer->iBlenderLite->ShowTimeline ){
            aMenuPane->SetItemTextL( ETimelineShowTimeline, R_HIDE_TIMELINE );
        }
        else {
            aMenuPane->SetItemTextL( ETimelineShowTimeline, R_SHOW_TIMELINE  );
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

typedef enum { AnimPosition, AnimRotation, AnimScale };

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
        //case EMaterial:
        //    OpenMaterialMenuL();
        //    break;
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
        case ETimelinePlayStop:
            iAppContainer->iBlenderLite->ToggleValue(iAppContainer->iBlenderLite->PlayAnimation);
            break;  
        case ETimelineShowTimeline:
            iAppContainer->iBlenderLite->ToggleValue(iAppContainer->iBlenderLite->ShowTimeline);
            break;
        case ETimelineSetCurrentFrame:
            iAppContainer->iBlenderLite->SetCurrentFrame();
            break;
        case ETimelineSetEndFrame:
            iAppContainer->iBlenderLite->SetEndFrame();
            break;
        case ETimelineSetStartFrame:
            iAppContainer->iBlenderLite->SetStartFrame();
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
        case EViewportSelectToCursor:
            iAppContainer->iBlenderLite->SelectToCursor();
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
        case ESetParent:
            iAppContainer->iBlenderLite->SetParent();
            break;     
        case EClearParent:
            iAppContainer->iBlenderLite->ClearParent();
            break;  
        case ERenderUI:
            iAppContainer->iBlenderLite->SaveCanvasToImage(false, true);
            break;  
        case ERenderUIanimation:
            iAppContainer->iBlenderLite->SaveCanvasToImage(false, true);
            break;  
        case ERenderImage:
            iAppContainer->iBlenderLite->SaveCanvasToImage(false, false);
            break; 
        case ERenderAnimation:
            iAppContainer->iBlenderLite->SaveCanvasToImage(true, false);
            break; 
        case ESetAmbientLight:
            iAppContainer->iBlenderLite->SetAmbientLight();
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
        case ESetSmooth:
            iAppContainer->iBlenderLite->SetSmooth();
            break;  
        case ESetCulling:
            iAppContainer->iBlenderLite->SetCulling();
            break;  
        case ESetLighting:
            iAppContainer->iBlenderLite->SetLighting();
            break;  
        case ESetVertexColor:
            iAppContainer->iBlenderLite->SetVertexColor();
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
        case EViewRendered:
            iAppContainer->iBlenderLite->SetShading(Rendered);
            break;
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
        case ENewMaterial:
            iAppContainer->iBlenderLite->NewMaterial();
            break;
        case ENewTexture:
            iAppContainer->iBlenderLite->NewTexture();
            break; 
        case ERemoveMaterial:
            iAppContainer->iBlenderLite->RemoveMaterial();
            break;  
        case ERemoveTexture:
            iAppContainer->iBlenderLite->RemoveTexture();
            break; 
        case EInsertKeyframe:
            iAppContainer->iBlenderLite->InsertKeyframe(AnimPosition);
            break; 
        case ERemoveKeyframes:
            iAppContainer->iBlenderLite->RemoveKeyframes();
            break; 
        case EClearKeyframes:
            iAppContainer->iBlenderLite->ClearKeyframes();
            break; 
            //case EBlenderLiteNavegacion:
            //iAppContainer->iBlenderLite->SetMode();
            //break;            
        default:
            break;
        }
    }

// End of File
