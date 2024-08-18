/*
 * ==============================================================================
 *  Name        : Blendersito.h
 *  Part of     : OpenGLEx / Blendersito
 * ==============================================================================
 */

#ifndef BLENDERSITO_H
#define BLENDERSITO_H

// INCLUDES
#include <e32base.h> // for CBase definition
#include <aknwaitdialog.h> //dialogo de espera
#include <GLES/gl.h> // OpenGL ES header file
#include "Utils3d.h" // Utilities (texmanager, textures etc.)
#include "Glutils.h" // Misc GLU and GLUT functions
#include "Blendersitoinput.h"

//para el cuadro de wait
#include <AknWaitDialog.h>

// Forward declaration de Object
//class CBlendersitoContainer;
class Object;

// MACROS
#define TEX_WIDTH     256 // Texture size must be power of two and max size
#define TEX_HEIGHT    256 // for textures is 256 x 256

#define FRUSTUM_LEFT   -1.f     //left vertical clipping plane
#define FRUSTUM_RIGHT   1.f     //right vertical clipping plane
#define FRUSTUM_BOTTOM -1.f     //bottom horizontal clipping plane
#define FRUSTUM_TOP     1.f     //top horizontal clipping plane
#define FRUSTUM_NEAR    3.f     //near depth clipping plane
#define FRUSTUM_FAR  1000.f     //far depth clipping plane

// CLASS DECLARATION
class Mesh;

/**
 * Class that does the actual OpenGL ES rendering.
 */
class CBlendersito : public CFiniteStateMachine, public MTextureLoadingListener
    {
    public:  // Constructors and destructor

        /**
         * Factory method for creating a new CSimpleCube object.
         * @param iInputHandler Input handler that maps keycodes to inputs and stores the current state for each key.
         */
        static CBlendersito* NewL( TUint aWidth, TUint aHeight, CBlendersitoInput* aInputHandler);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CBlendersito();
            
    public:  // New functions
        
        //RArray<Material> Materials;
        /**
		* CallbackIncrementProgressNoteL
		* Callback function to increment progress note
		* @param aThis
		* @return TInt Return 0 when work is done, otherwise return 1.
		*/
		static TInt CallbackIncrementProgressNoteL( TAny* aThis );

        /**
         * Initializes OpenGL ES, sets the vertex and color
         * arrays and pointers. Also selects the shading mode.
         */
        void AppInit( void );

        /**
         * Called upon application exit. Does nothing.
         */
        void AppExit( void );

        /**
         * Renders one frame.
         * @param aFrame Number of the frame to be rendered.
         */
        void AppCycle( TInt aFrame, GLfloat aTimeSecs, GLfloat aDeltaTimeSecs );

    	/**
    	 * Called when the finite state machine enters a new state.
    	 * Does nothing in this implementation.
    	 * @param aState State that is about to be entered.
    	 */
    	void OnEnterStateL( TInt aState );
        
        /**
		 * Called when texture manager starts loading the textures.
		 * Sets the current state to "loading textures".
		 */
		void OnStartLoadingTexturesL();

		/**
		 * Called when texture manager has completed texture loading.
		 * Changes the current state to "running".
		 */
		void OnEndLoadingTexturesL();
        
        //mover al siguiente casillero
        void InputUsuario( GLfixed aDeltaTimeSecs );
        
        GLfloat GradosARadianes(TInt grados);
        void SetRotacion( void );
        void SetEscala( void );
        void SetPosicion( void );
        void ChangeEje ( void );
        void Cancelar( void );
        void Aceptar( void );
        void SetTransformPivotPoint(void);
        void UVmapping(TInt valor);
        void PressTab( void );

        void SetTranslacionVertices(TInt valor);
        void SetTranslacionObjetos(TInt valor);
        void SetRotacion(TInt valor);
        void BorrarObjeto(TInt indice);
        void BorrarMesh(TInt indice);
        void BorrarAnimaciones(TInt indice);
        void SeleccionarAnterior();
        void SeleccionarProximo();
        
        void dibujarUI();
        void guardarEstado();
        void ReestablecerEstado();
        void SeleccionarTodo();
        void SetEje( TInt eje );     
        void AddMesh( TInt modelo );
        void NewMaterial();
        void AddObject( TInt tipo );
        void SetTexture(); //int textureID 
        void SetMaterial();
        void DuplicatedObject();
        void DuplicatedLinked();
        void ActivarTextura();
        void SetInterpolation();
        void SetTransparencia();
        void SetAmbientLight();

        //nueva forma de editar vertices
        void CalcShrinkFattenVectors();
        void SetShrinkFatten(TInt fuerza);
        void CalcScaleVectors();
        void SetScale(TInt fuerza);
        void Duplicate();

        void FlipNormals();
        void SetSmooth();
        void SetCulling();
        void SetLighting();
        void SetVertexColor();
        void SetSpecular();
        void SetDiffuse();
        void InsertKeyframe(TInt propertySelect);
        void RemoveKeyframes();
        void ClearKeyframes();
        void SetEmission();
        void SetEditMode();
        void Extrude();
        void SetPerspectiva( TBool redibuja = true );
        void EnfocarObject();
        void CursorToSelect();
        void SelectToCursor();
        void ShowWaitDialogL();
        void CloseWaitDialog();
        void DialogWait(HBufC* noteBuf);
        void Borrar();
        void changeSelect();
        void LeerMTL(const TFileName& aFile);
        void ReloadAnimation();
        void SetMouse();
        void ToggleValue(TBool& valueToUpdate);
        void InfoObject(TInt opciones);
        void SetUvSprite(GLshort x, GLshort y, GLshort ancho, GLshort alto);
        void SetSpriteSize(GLshort ancho, GLshort alto);
        void DrawnRectangle();
        void SetTextureRepeat();
        void IconSelect(TBool activo);
        void UiMoveTo(GLshort x, GLshort y);
        void DrawnLines(TInt LineWidth, TInt cantidad, GLshort* vertexlines, GLushort* lineasIndices);
        void DrawnLines(TInt LineWidth, TInt cantidad, const GLshort* vertexlines, const GLushort* lineasIndices);
        void SetUvBordes(GLshort origenX, GLshort origenY, GLshort ancho, GLshort alto, GLshort top, GLshort right, GLshort bottom, GLshort left);
        void DibujarRectanguloBordes(GLshort ancho, GLshort alto, GLshort top, GLshort right, GLshort bottom, GLshort left );
        void SearchSelectObj(Object& obj, TInt objIndex, TBool& found);
        void DrawTransformAxis(Object& obj);
        void SetViewpoint(TInt opcion);
        TFileName GetRootDirectory(const TDesC& aFilePath);        
        void SaveCanvasToImage(TBool secuencia, TBool showUi);
        void DrawnTimeline();
        void SaveAsBMP(TInt width, TInt height, const GLubyte* pixels, const TDesC& fileName);
        void AddModificador(TInt opcion);
        void RenderMeshAndChildren(Object& obj, TInt indice);
        void RenderObjectAndChildrens(TInt objId);
        void TecladoNumerico(TInt numero);
        void InsertarValor();
        void ImportOBJ();
        void DeseleccionarTodo();
        void OldImportOBJ();
        TBool LeerOBJ(RFs* fsSession, RFile* rFile, TFileName* file, TInt64* startPos, 
                        TInt* acumuladoVertices,
                        TInt* acumuladoNormales,
                        TInt* acumuladoUVs);
        void NewTexture();   
        void SetNavigation();
        void RemoveMaterial();
		void RemoveTexture();
        void RenderMesh( Object& obj, TInt indice );
        void EventKeyDown(TInt scan);
        void VerOpciones();

        void calculateReflectionUVs(Mesh& pMesh);
        void createRotationMatrix(GLfloat pitch, GLfloat yaw, GLfloat* matrix);
        void multiplyMatrixVec(const GLfloat* matrix, const GLfloat* vec, GLfloat* result);

        void RenderObject( TInt objId );
        void applyBlur(GLubyte* pixels, int width, int height, int radius);
        int clamp(int value, int min, int max);
        void DibujarOrigen(Object& obj, TInt objIndex);
        void SetCurrentFrame();
        void SetStartFrame();
        void SetEndFrame();
        void SetParent();
        void ClearParent();
        void ObtenerUbicacionInstalacionL(TDes& aUbicacion);
        void SetTipoSelect(TInt tipo);
        void SetOrigen( TInt opcion );
        void LoadTextureFromFile(const TDesC& aFileName);
        void LoadFile(const TFileName& aFileName,
                                       const TSize &aSize,
                                       TInt aSelectedFrame = 0);
        void Mensaje(HBufC* noteBuf);
        void MensajeError(HBufC* noteBuf);
        TBool DialogAlert(HBufC* noteBuf);
        TBool DialogAlert(const TDesC& noteBuf);
        TInt DialogNumber( TInt valor, TInt min, TInt max, HBufC* noteBuf); //,TPtrC etiqueta
        TPtr DialogText(HBufC* textBuf, HBufC* noteBuf);
        //void ShowProgressNoteUnderSingleProcessL( TInt aResourceId, TInt aControlId );
        //TInt DialogSelectOption(const TDesC& aPrompt, CDesCArray& aOptions);
        TInt ShowOptionsDialogL();
        void Tab();
        
        //cambiar el shader en el viewport
        void SetShading( TInt valor );        

        /**
         * Notifies that the screen size has dynamically changed during execution of
         * this program. Resets the viewport to this new size.
         * @param aWidth New width of the screen.
         * @param aHeight New height of the screen.
         */
        void SetScreenSize( TUint aWidth, TUint aHeight );
        void SetScreenSize( TUint aWidth, TUint aHeight, TBool widescreen );
        void SetWidescreen();

    protected: // New functions

        /**
         * Standard constructor that must never Leave.
         * Stores the given screen width and height.
         * @param aWidth Width of the screen.
         * @param aHeight Height of the screen.
         */
        CBlendersito( TUint aWidth, TUint aHeight, CBlendersitoInput* aInputHandler );

        /**
         * Second phase contructor. Does nothing.
         */
        void ConstructL( void );

        /**
         * Helper function that is used to make the duck 'quak'.
         * Calculates the sine for the given angle. Returns 0 if any
         * errors occur while calling the Math::Sin() method.
         * @param aRad Radian angle whose sine is to be calculated.
         * @return The sine of the given angle or 0 if error occured while calculating the sine.
         */
        GLfloat sin( GLfloat aRad );        
        GLfloat aSin(GLfloat value);
        GLfloat atan2(GLfloat y, GLfloat x);

    public:  // Data
        /** para el menu de la app */
        TBool iWidescreenEnabled;
        TBool showOverlays;
        TBool show3DCursor;
        TBool showFloor;
        TBool showYaxis;
        TBool showXaxis;
        TBool showOutlineSelect;
        TBool showOrigins;
        TBool ShowTimeline;
        TBool PlayAnimation;    
        TBool iShiftPressed; 
        RArray<Object> Objects;
        TInt SelectActivo;

		/**
		 * Application states:
		 * ELoadingTextures - indicates that the app. is loading textures.
		 * ERunning - indicates that the app. is running.
		 */
		enum {
			ELoadingTextures,
			ERunning
		};
        
        TInt estado;
        TInt InteractionMode;
        TInt navegacionMode;

    private:  // Data
        CAknWaitDialog* iWaitDialog;

        //CBlendersitoContainer*    iContainer;

		/** Texture manager that is used to load the used textures. */
		CTextureManager * iTextureManager;
		/** Width of the screen */
		TUint iScreenWidth;
		/** Height of the screen */
		TUint iScreenHeight;
        //el alto de la pantalla dividida a la mitad
        GLfloat iScreenHeightSplit;
		/** Texturas */
		/*TTexture iBaseColor;  	
		TTexture iOrigenTextura;
		TTexture iColorGridTextura;
		TTexture iMouseTextura;
		TTexture iLampTextura;
        TTexture iCursor3dTextura;*/
        
        //RArray<TTexture> Textures;

        TTexture iShaderMaterialPreview;
        TTexture iShaderRendered;
        TTexture iShaderSolid;
        TTexture iShaderWireframe;

		/**
		 * Input handler that maps keycodes to inputs and stores the current state
		 * for each key. Owned by the C#Name#Container.
		 */
		CBlendersitoInput* iInputHandler;
    };

#endif // BLENDERSITO_H
