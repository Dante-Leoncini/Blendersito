/*
 * ==============================================================================
 *  Name        : BlendersitoAppUi.cpp
 *  Part of     : OpenGLEx / Blendersito
 * ==============================================================================
 */

// INCLUDE FILES
#include "BlendersitoAppUi.h"
#include "BlendersitoContainer.h"
#include <Blendersito.rsg>
#include "blendersito.hrh"

#include <eikmenup.h>
#include <avkon.hrh>
#include "Mesh.h"

#include <aknconsts.h>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CBlendersitoAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------
//
void CBlendersitoAppUi::ConstructL(){
    BaseConstructL();
    iAppContainer = new (ELeave) CBlendersitoContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );
}

// Destructor
CBlendersitoAppUi::~CBlendersitoAppUi(){
	if ( iAppContainer ){
		RemoveFromStack( iAppContainer );
		delete iAppContainer;
	}
}

// ------------------------------------------------------------------------------
//  CBlendersitoAppUi::DynInitMenuPaneL
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
enum{
	ObjectMode,
	EditMode
};

enum {Solid, MaterialPreview, Wireframe, Rendered};

void CBlendersitoAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane ){
    //para trabajar mas facil
    CBlendersito& BL = *(iAppContainer->iBlendersito); 
    //BL.InteractionMode = ...;

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

		TBlendersitoMenuCommands menuCommand = EBlendersitoOrigenSetOrigen;  // Suponiendo que EBlendersitoCommand es una constante o enumeraci�n v�lida
		
		CEikMenuPaneItem::SData menuItemData;
		menuItemData.iCommandId = menuCommand;  // Asigna el ID de comando de tu men�
		menuItemData.iText = itemText;
				
		aMenuPane->InsertMenuItemL(menuItemData, 2);
        
        // Agrega los nombres de materiales como elementos de men�
		//for (TInt i = 0; i < iAppContainer->iBlendersito->Materials.Count(); ++i){
			//TInt commandId = EAknCmdEmpty + i; // Asigna un ID de comando �nico para cada elemento de men�
		    //_LIT(KTitle, "Selecciona la Textura");
			//aMenuPane->AddMenuItemL(TPtrC(KTitle), EBlendersitoOrigenSetOrigen);
		//}
    }*/
	//oculta el SetOrigen si no esta en modo edicion
    if (aResourceId == R_BLENDERSITO_MENU ) {
        if (BL.InteractionMode == EditMode) {
	        if (BL.Objects.Count() > 0){
                aMenuPane->SetItemDimmed(EViewportObject, EFalse);
                aMenuPane->SetItemDimmed(EUvMappingMenu, EFalse);                
            }
            else {
                aMenuPane->SetItemDimmed(EViewportObject, ETrue);
                aMenuPane->SetItemDimmed(EUvMappingMenu, ETrue);
            }
            aMenuPane->SetItemDimmed(EBlendersitoOrigenSetOrigen, EFalse);
            aMenuPane->SetItemDimmed(EBlendersitoSeleccionar, EFalse);
            aMenuPane->SetItemDimmed(EViewportAdd, ETrue);            
        } 
        else {
            aMenuPane->SetItemDimmed(EBlendersitoOrigenSetOrigen, ETrue);
            aMenuPane->SetItemDimmed(EBlendersitoSeleccionar, ETrue);
            aMenuPane->SetItemDimmed(EViewportAdd, EFalse);

            //si hay objetos            
	        if (BL.Objects.Count() > 0){
                aMenuPane->SetItemDimmed(EViewportObject, EFalse);          
                Object& obj = BL.Objects[BL.SelectActivo];
                //si es una malla 3d
                if (obj.type == mesh && obj.seleccionado){
                    aMenuPane->SetItemDimmed(EMaterial, EFalse);
                    aMenuPane->SetItemDimmed(EBlendersitoModificadores, EFalse);
                }
                else {
                    aMenuPane->SetItemDimmed(EMaterial, ETrue);
                    aMenuPane->SetItemDimmed(EBlendersitoModificadores, ETrue);                    
                }
            }
            else {
                aMenuPane->SetItemDimmed(EMaterial, ETrue);
                aMenuPane->SetItemDimmed(EViewportObject, ETrue);
            }
        }
    }
    else if ( aResourceId == R_TRANSFORM_MENU ){
        //si hay objetos            
        if (BL.Objects.Count() > 0){        
            Object& obj = BL.Objects[BL.SelectActivo];
            //si es una malla 3d
            if (obj.type == mesh && obj.seleccionado){
                aMenuPane->SetItemDimmed(EShrinkFatten, EFalse);
                aMenuPane->SetItemDimmed(EExtrude, EFalse);
            }
            else {
                aMenuPane->SetItemDimmed(EShrinkFatten, ETrue);    
                aMenuPane->SetItemDimmed(EExtrude, ETrue);                  
            }
        }
        else {
            aMenuPane->SetItemDimmed(EShrinkFatten, ETrue);
        }
    }
    else if (aResourceId == R_VIEWPORT_OVERLEY_MENU ) {
        // Texto para setear la pantalla ancha
        if ( iAppContainer->iBlendersito->iWidescreenEnabled ){
            aMenuPane->SetItemTextL( EBlendersitoWidescreen, R_WIDESCREEN_OFF );
            }
        else {
            aMenuPane->SetItemTextL( EBlendersitoWidescreen, R_WIDESCREEN_ON  );
        }
        if ( iAppContainer->iBlendersito->showOverlays ){
            aMenuPane->SetItemTextL( EViewportSetOverlay, R_HIDEOVERLAY );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSetOverlay, R_SHOWOVERLAY  );
        } 
        if ( iAppContainer->iBlendersito->show3DCursor ){
            aMenuPane->SetItemTextL( EViewportSet3DCursor, R_HIDE3DCURSOR );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSet3DCursor, R_SHOW3DCURSOR  );
        } 

        if ( iAppContainer->iBlendersito->showFloor ){
            aMenuPane->SetItemTextL( EViewportSetShowFloor, R_HIDEFLOOR );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSetShowFloor, R_SHOWFLOOR  );
        } 
        if ( iAppContainer->iBlendersito->showYaxis ){
            aMenuPane->SetItemTextL( EViewportSetShowYaxis, R_HIDEYAXIS );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSetShowYaxis, R_SHOWYAXIS  );
        } 
        if ( iAppContainer->iBlendersito->showXaxis ){
            aMenuPane->SetItemTextL( EViewportSetShowXaxis, R_HIDEXAXIS );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSetShowXaxis, R_SHOWXAXIS  );
        } 
        if ( iAppContainer->iBlendersito->showOutlineSelect ){
            aMenuPane->SetItemTextL( EViewportSetOutlineSelect, R_HIDEOUTLINESELECT );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSetOutlineSelect, R_SHOWOUTLINESELECT  );
        } 
        if ( iAppContainer->iBlendersito->showOrigins ){
            aMenuPane->SetItemTextL( EViewportSetOrigins, R_HIDEORIGIN );
        }
        else {
            aMenuPane->SetItemTextL( EViewportSetOrigins, R_SHOWORIGIN  );
        }    
    }  
    else if (aResourceId == R_TIMELINE_MENU ) {
        if ( iAppContainer->iBlendersito->PlayAnimation ){
            aMenuPane->SetItemTextL( ETimelinePlayStop, R_STOP );
        }
        else {
            aMenuPane->SetItemTextL( ETimelinePlayStop, R_PLAY );
        }
        if ( iAppContainer->iBlendersito->ShowTimeline ){
            aMenuPane->SetItemTextL( ETimelineShowTimeline, R_HIDE_TIMELINE );
        }
        else {
            aMenuPane->SetItemTextL( ETimelineShowTimeline, R_SHOW_TIMELINE  );
        }
    }
}

// ----------------------------------------------------
// CBlendersitoAppUi::HandleKeyEventL
// Key event handler
// ----------------------------------------------------
//
TKeyResponse CBlendersitoAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType ){
    if ( iAppContainer->iBlendersito->GetState() == CBlendersito::ERunning ){   
        TUint scan = aKeyEvent.iScanCode;
            
        // Imprimir el código de escaneo para depuración
        /*if (scan != 165){
            HBufC* noteBuf = HBufC::NewLC(100);
            _LIT(KFormatString, "Scan Code: %d\naType: %d");
            noteBuf->Des().Format(KFormatString, scan, aType);			
            iAppContainer->iBlendersito->DialogAlert(noteBuf);
        }*/

        if (aType == EEventKeyDown) {
			switch(scan){                    
                /*case(14): //izquierda
                    //iAppContainer->iBlendersito->Rotar(1);
                    iAppContainer->iBlendersito->Tab();
                    return EKeyWasConsumed;
                case(15): //derecha
                    //iAppContainer->iBlendersito->Rotar(2);
                    iAppContainer->iBlendersito->Tab();
                    return EKeyWasConsumed;
                case(16): //arriba
                    //iAppContainer->iBlendersito->NextPos(0,1);
                    iAppContainer->iBlendersito->Tab();
                    return EKeyWasConsumed;
                case(17): //abajo
                    //iAppContainer->iBlendersito->NextPos(8,1);
                    iAppContainer->iBlendersito->Tab();
                    return EKeyWasConsumed;*/                    
                case(22): //Shift
                case(18): //left Shift
                case(19): //rigth Shift
                    iAppContainer->iBlendersito->iShiftPressed = ETrue;
                    return EKeyWasConsumed;
                case(1): //Delete
                    iAppContainer->iBlendersito->Borrar();
                    return EKeyWasConsumed;
                case(49): //1
                    iAppContainer->iBlendersito->TecladoNumerico(1);
                    return EKeyWasConsumed;
                case(50): //2
                    iAppContainer->iBlendersito->TecladoNumerico(2);
                    return EKeyWasConsumed;
                case(51): //3
                    iAppContainer->iBlendersito->TecladoNumerico(3);
                    return EKeyWasConsumed;
                case(52): //4
                    iAppContainer->iBlendersito->TecladoNumerico(4);
                    return EKeyWasConsumed;
                case(53): //5
                    iAppContainer->iBlendersito->TecladoNumerico(5);
                    return EKeyWasConsumed;
                case(54): //6
                    iAppContainer->iBlendersito->TecladoNumerico(6);
                    return EKeyWasConsumed;
                case(55): //7
                    iAppContainer->iBlendersito->TecladoNumerico(7);
                    return EKeyWasConsumed;
                case(56): //8
                    iAppContainer->iBlendersito->TecladoNumerico(8);
                    return EKeyWasConsumed;
                case(57): //9
                    iAppContainer->iBlendersito->TecladoNumerico(9);
                    return EKeyWasConsumed;
                case(48): //0
                    iAppContainer->iBlendersito->TecladoNumerico(0);
                    return EKeyWasConsumed;
                case(42): //*
                    iAppContainer->iBlendersito->TecladoNumerico(10);
                    return EKeyWasConsumed;
                case(127): //#
                    iAppContainer->iBlendersito->TecladoNumerico(11);
                    return EKeyWasConsumed;
                case(226): //camara
                    iAppContainer->iBlendersito->Extrude();
                    return EKeyWasConsumed;
                case(196): //llamada
                    iAppContainer->iBlendersito->ChangeEje();
                    return EKeyWasConsumed;
                case(65): //A
                    iAppContainer->iBlendersito->SeleccionarTodo();
                    return EKeyWasConsumed;
                case(71): //G
                    iAppContainer->iBlendersito->SetPosicion();
                    return EKeyWasConsumed;
                case(82): //R
                    iAppContainer->iBlendersito->SetRotacion();
                    return EKeyWasConsumed;
                case(83): //S
                    iAppContainer->iBlendersito->SetEscala();
                    return EKeyWasConsumed;
                case(81): //Q
                    iAppContainer->iBlendersito->Cancelar();
                    return EKeyWasConsumed;
                case(88): //X
                    iAppContainer->iBlendersito->EventKeyDown(scan);
                    return EKeyWasConsumed;
                case(3): //Enter
                    iAppContainer->iBlendersito->Aceptar();
                    return EKeyWasConsumed;
                case(167): //OK
                    iAppContainer->iBlendersito->Aceptar();
                    return EKeyWasConsumed;
                case(89): //Y
                    iAppContainer->iBlendersito->SetEje(1);
                    return EKeyWasConsumed;
                case(90): //Z
                    iAppContainer->iBlendersito->SetEje(2);
                    return EKeyWasConsumed;
                case(2): //Tab
                    iAppContainer->iBlendersito->PressTab();
                    return EKeyWasNotConsumed;
                /*case(14): //izquierda
                    iAppContainer->iBlendersito->Tab();
                    return EKeyWasNotConsumed;
                case(15): //derecha
                    iAppContainer->iBlendersito->Tab();
                    return EKeyWasNotConsumed;
                case(16): //arriba
                    iAppContainer->iBlendersito->Tab();
                    return EKeyWasNotConsumed;
                case(17): //abajo
                    iAppContainer->iBlendersito->Tab();
                    return EKeyWasNotConsumed;*/
                default:
                    iAppContainer->iBlendersito->EventKeyDown(scan);
                    return EKeyWasNotConsumed;
			}
		}
        else if (aType == EEventKeyUp) {
            TUint scan = aKeyEvent.iScanCode;
            switch (scan) {                
                /*case(14): //izquierda
                    //if (iAppContainer->iBlendersito->iShiftPressed) {
                    iAppContainer->iBlendersito->Tab();
                    return EKeyWasNotConsumed;
                case(15): //derecha
                    iAppContainer->iBlendersito->Tab();
                    return EKeyWasNotConsumed;
                case(16): //arriba
                    iAppContainer->iBlendersito->Tab();
                    return EKeyWasNotConsumed;
                case(17): //abajo
                    iAppContainer->iBlendersito->Tab();
                    return EKeyWasNotConsumed;*/
                case 22: // Shift
                case 18: // Left Shift
                case 19: // Right Shift
                    if (iAppContainer->iBlendersito->iShiftPressed) {
                        iAppContainer->iBlendersito->iShiftPressed = EFalse;
                    }
                    return EKeyWasConsumed;
                default:
                    return EKeyWasNotConsumed;
            }
        }
	}
    return EKeyWasNotConsumed;
}

enum{
	cubo, esfera, cilindro, plane, vacio, camara,
    cad, luz, monkey,vertice, circle
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
// CBlendersitoAppUi::HandleCommandL
// Command handler
// ----------------------------------------------------
//
void CBlendersitoAppUi::HandleCommandL(TInt aCommand){
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            iAppContainer->iBlendersito->SetMouse();
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
            iAppContainer->iBlendersito->ToggleValue(iAppContainer->iBlendersito->showFloor);
            break;       
        case EViewportSetShowYaxis:
            iAppContainer->iBlendersito->ToggleValue(iAppContainer->iBlendersito->showYaxis);
            break;       
        case EViewportSetShowXaxis:
            iAppContainer->iBlendersito->ToggleValue(iAppContainer->iBlendersito->showXaxis);
            break;       
        case EViewportSetOutlineSelect:
            iAppContainer->iBlendersito->ToggleValue(iAppContainer->iBlendersito->showOutlineSelect);
            break;       
        case EViewportSetOrigins:
            iAppContainer->iBlendersito->ToggleValue(iAppContainer->iBlendersito->showOrigins);
            break;          
        case EViewportSetOverlay:
            iAppContainer->iBlendersito->ToggleValue(iAppContainer->iBlendersito->showOverlays);
            break;       
        case EViewportSet3DCursor:
            iAppContainer->iBlendersito->ToggleValue(iAppContainer->iBlendersito->show3DCursor);
            break;    
        case ETimelinePlayStop:
            iAppContainer->iBlendersito->ToggleValue(iAppContainer->iBlendersito->PlayAnimation);
            break;  
        case ETimelineShowTimeline:
            iAppContainer->iBlendersito->ToggleValue(iAppContainer->iBlendersito->ShowTimeline);
            break;
        case ETimelineSetCurrentFrame:
            iAppContainer->iBlendersito->SetCurrentFrame();
            break;
        case ETimelineSetEndFrame:
            iAppContainer->iBlendersito->SetEndFrame();
            break;
        case ETimelineSetStartFrame:
            iAppContainer->iBlendersito->SetStartFrame();
            break;
        case EImportOBJ:
            iAppContainer->iBlendersito->ImportOBJ();
            break; 
        case EShrinkFatten:
            iAppContainer->iBlendersito->CalcShrinkFattenVectors();
            break; 
        case EExtrude:
            iAppContainer->iBlendersito->Extrude();
            break; 
        case EDuplicate:
            iAppContainer->iBlendersito->Duplicate();
            break; 
        case EOldImportOBJ:
            iAppContainer->iBlendersito->OldImportOBJ();
            break; 
        case EBlendersitoWidescreen:
            iAppContainer->SetWidescreen();
            break;    
        case EViewportCursorToSelect:
            iAppContainer->iBlendersito->CursorToSelect();
            break;
        case EViewportSelectToCursor:
            iAppContainer->iBlendersito->SelectToCursor();
            break;            
        case ECamera:            
            iAppContainer->iBlendersito->AddObject(camera);
            break;   
        case ELight:            
            iAppContainer->iBlendersito->AddObject(light);
            break;       
        case EEmpty:            
            iAppContainer->iBlendersito->AddObject(empty);
            break;
        case ECube:            
            iAppContainer->iBlendersito->AddMesh(cubo);
            break;  
        case ECircle:            
            iAppContainer->iBlendersito->AddMesh(circle);
            break;  
        case EPlane:            
            iAppContainer->iBlendersito->AddMesh(plane);
            break;          
        case EMonkey:
            iAppContainer->iBlendersito->AddMesh(monkey);
            break;  
        case EVertex:
            iAppContainer->iBlendersito->AddMesh(vertice);
            break; 
        case ESetMaterial:
            iAppContainer->iBlendersito->SetMaterial();
            break;  
        case ESetSpecular:
            iAppContainer->iBlendersito->SetSpecular();
            break;   
        case EBlendersitoEditarMesh:
            iAppContainer->iBlendersito->SetEditMode();
            break; 
        case ECylinderProjection:            
            iAppContainer->iBlendersito->UVmapping(0);
            break;   
        case EBlendersitoSetEscala:
            iAppContainer->iBlendersito->SetEscala();
            break;   
        case EBlendersitoSetPosicion:
            iAppContainer->iBlendersito->SetPosicion();
            break;   
        case EBlendersitoSetRotacion:
            iAppContainer->iBlendersito->SetRotacion();
            break;     
        case EDuplicatedObject:
            iAppContainer->iBlendersito->DuplicatedObject();
            break;       
        case EDuplicatedLinked:
            iAppContainer->iBlendersito->DuplicatedLinked();
            break;     
        case ESetParent:
            iAppContainer->iBlendersito->SetParent();
            break;     
        case EClearParent:
            iAppContainer->iBlendersito->ClearParent();
            break;  
        case EFlipNormals:
            iAppContainer->iBlendersito->FlipNormals();
            break;  
        case ERenderUI:
            iAppContainer->iBlendersito->SaveCanvasToImage(false, true);
            break;  
        case ERenderUIanimation:
            iAppContainer->iBlendersito->SaveCanvasToImage(false, true);
            break;  
        case ERenderImage:
            iAppContainer->iBlendersito->SaveCanvasToImage(false, false);
            break; 
        case ERenderAnimation:
            iAppContainer->iBlendersito->SaveCanvasToImage(true, false);
            break; 
        case ESetAmbientLight:
            iAppContainer->iBlendersito->SetAmbientLight();
            break;   
        case ESetDiffuse:
            iAppContainer->iBlendersito->SetDiffuse();
            break;   
        case ESetEmission:
            iAppContainer->iBlendersito->SetEmission();
            break;  
        case ESetTexture:
            iAppContainer->iBlendersito->SetTexture();
            break;  
        case ESetActiveTexture:
            iAppContainer->iBlendersito->ActivarTextura();
            break; 
        case ESetInterpolation:
            iAppContainer->iBlendersito->SetInterpolation();
            break; 
        case ESetTransparent:
            iAppContainer->iBlendersito->SetTransparencia();
            break; 
        case ESetTextureRepeat:
            iAppContainer->iBlendersito->SetTextureRepeat();
            break;             
        case ESetSmooth:
            iAppContainer->iBlendersito->SetSmooth();
            break;  
        case ESetCulling:
            iAppContainer->iBlendersito->SetCulling();
            break;  
        case ESetLighting:
            iAppContainer->iBlendersito->SetLighting();
            break;  
        case ESetVertexColor:
            iAppContainer->iBlendersito->SetVertexColor();
            break;    
        case EBlendersitoInfoObject:
            iAppContainer->iBlendersito->InfoObject(1);
            break; 
        case EBlendersitoBorrar:
            iAppContainer->iBlendersito->Borrar();
            break;  
        case EBlendersitoPerspectiva:
            iAppContainer->iBlendersito->SetPerspectiva();
            break;
        case EBlendersitoTopView:
            iAppContainer->iBlendersito->SetViewpoint(top);
            break;
        case EBlendersitoFrontView:
            iAppContainer->iBlendersito->SetViewpoint(front);
            break;
        case EBlendersitoRightView:
            iAppContainer->iBlendersito->SetViewpoint(right);
            break;
        case EBlendersitoObjFocus:
            iAppContainer->iBlendersito->EnfocarObject();
            break; 
        case EBlendersitoViewMaterial:
            iAppContainer->iBlendersito->SetShading(MaterialPreview);
            break; 
        case EBlendersitoViewSolid:
            iAppContainer->iBlendersito->SetShading(Solid);
            break; 
        case EBlendersitoViewWireframe:
            iAppContainer->iBlendersito->SetShading(Wireframe);
            break; 
        case EViewRendered:
            iAppContainer->iBlendersito->SetShading(Rendered);
            break;
        case EBlendersitoSetTipoVertex:
            iAppContainer->iBlendersito->SetTipoSelect(vertexSelect);
            break; 
        case EBlendersitoSetTipoEdge:
            iAppContainer->iBlendersito->SetTipoSelect(edgeSelect);
            break; 
        case EBlendersitoSetTipoFace:
            iAppContainer->iBlendersito->SetTipoSelect(faceSelect);
            break; 
        case EBlendersitoOrigenToGeometry:
            iAppContainer->iBlendersito->SetOrigen(0);
            break; 
        case EBlendersitoOrigenToSelect:
            iAppContainer->iBlendersito->SetOrigen(1);
            break; 
        case EBlendersitoOrigenToCursor:
            iAppContainer->iBlendersito->SetOrigen(2);
            break; 
        case EBlendersitoCambiarObject:
            iAppContainer->iBlendersito->changeSelect();
            break; 
        case EBlendersitoModificadorArray:
            iAppContainer->iBlendersito->AddModificador(array);
            break; 
        case EBlendersitoModificadorMirror:
            iAppContainer->iBlendersito->AddModificador(mirror);
            break; 
        case EBlendersitoModificadorScrew:
            iAppContainer->iBlendersito->AddModificador(screw);
            break;             
        case ENewMaterial:
            iAppContainer->iBlendersito->NewMaterial();
            break;
        case ENewTexture:
            iAppContainer->iBlendersito->NewTexture();
            break; 
        case ERemoveMaterial:
            iAppContainer->iBlendersito->RemoveMaterial();
            break;  
        case ERemoveTexture:
            iAppContainer->iBlendersito->RemoveTexture();
            break; 
        case EInsertKeyframe:
            iAppContainer->iBlendersito->InsertKeyframe(AnimPosition);
            break; 
        case ERemoveKeyframes:
            iAppContainer->iBlendersito->RemoveKeyframes();
            break; 
        case EClearKeyframes:
            iAppContainer->iBlendersito->ClearKeyframes();
            break; 
        case EBlendersitoNavegacion:
            iAppContainer->iBlendersito->SetNavigation();
            break;            
        default:
            break;
        }
    }

// End of File
