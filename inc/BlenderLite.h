/*
 * ==============================================================================
 *  Name        : BlenderLite.h
 *  Part of     : OpenGLEx / BlenderLite
 * ==============================================================================
 */

#ifndef BLENDERLITE_H
#define BLENDERLITE_H

// INCLUDES
#include <e32base.h> // for CBase definition
#include <aknwaitdialog.h> //dialogo de espera
#include <GLES/gl.h> // OpenGL ES header file
#include "Utils3d.h" // Utilities (texmanager, textures etc.)
#include "Glutils.h" // Misc GLU and GLUT functions
#include "BlenderLiteinput.h"

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

/**
 * Class that does the actual OpenGL ES rendering.
 */
class CBlenderLite : public CFiniteStateMachine, public MTextureLoadingListener
    {
    public:  // Constructors and destructor

        /**
         * Factory method for creating a new CSimpleCube object.
         * @param iInputHandler Input handler that maps keycodes to inputs and stores the current state for each key.
         */
        static CBlenderLite* NewL( TUint aWidth, TUint aHeight, CBlenderLiteInput* aInputHandler);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CBlenderLite();

    public:  // New functions

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

        /**
         * Toggles lighting enabled/disabled.
         */
        void ToggleLighting( void );
        
        //mover al siguiente casillero
        void Rotar( GLfixed aDeltaTimeSecs );
        
        void SetRotacion( void );
        void SetEscala( void );
        void SetPosicion( void );
        void Cancelar( void );
        void Aceptar( void );
        
        void guardarEstado( int indice );
        void ReestablecerEstado( int indice );
        void SetEje( int eje );     
        void CrearObjeto( int modelo );
        void SetTexture(); //int texturaID 
        void ActivarTextura();
        void SetInterpolation();
        void SetTransparencia();
        void SetSpecular();
        void SetSmooth();
        void SetDiffuse();
        void SetEmission();
        void SetEditMode();
        void Extruir();
        void SetPerspectiva( void );
        void EnfocarObjeto();
        void Borrar();
        void changeSelect();
        void InfoObject(TInt opciones);
        void SetViewpoint(TInt opcion);
        void AddModificador(TInt opcion);
        void TecladoNumerico(TInt numero);
        void InsertarValor();
        void ImportOBJ();
        void NewTexture();        
        void CloseWaitNoteL();
        void SetTipoSelect(TInt tipo);
        void SetOrigen( TInt opcion );
        void OpenWaitNoteL( TFileName file );
        void LoadFile(const TFileName& aFileName,
                                       const TSize &aSize,
                                       TInt aSelectedFrame = 0);
        void DisplayWarningL( const TDesC &aDescr, TInt aErr) const;
        void Mensaje(HBufC* noteBuf);
        void MensajeError(HBufC* noteBuf);
        TBool DialogAlert(HBufC* noteBuf);
        TInt DialogNumber( TInt valor, TInt min, TInt max, HBufC* noteBuf); //,TPtrC etiqueta
        void Tab();
        
        //cambiar el shader en el viewport
        void SetShader( TInt valor );
        

        /**
         * Toggles lamp enabled/disabled.
         */
        void ToggleLamp( void );

        /**
         * Toggles spot light enabled/disabled.
         */
        void ToggleSpot( void );

        /**
         * Notifies that the screen size has dynamically changed during execution of
         * this program. Resets the viewport to this new size.
         * @param aWidth New width of the screen.
         * @param aHeight New height of the screen.
         */
        void SetScreenSize( TUint aWidth, TUint aHeight );

    protected: // New functions

        /**
         * Standard constructor that must never Leave.
         * Stores the given screen width and height.
         * @param aWidth Width of the screen.
         * @param aHeight Height of the screen.
         */
        CBlenderLite( TUint aWidth, TUint aHeight, CBlenderLiteInput* aInputHandler );

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

    public:  // Data

		/**
		 * Application states:
		 * ELoadingTextures - indicates that the app. is loading textures.
		 * ERunning - indicates that the app. is running.
		 */
		enum {
			ELoadingTextures,
			ERunning
		};

        /** Whether or not the lighting is enabled. Used when building the menu for the application. */
        TBool iLightingEnabled;

        /** Whether or not the lamp is enabled. Used when building the menu for the application. */
        TBool iLampEnabled;

        /** Whether or not the spotlight is enabled. Used when building the menu for the application. */
        TBool iSpotEnabled;
        
        TInt estado;

    private:  // Data

        	/** Texture manager that is used to load the used textures. */
        	CTextureManager * iTextureManager;
            /** Width of the screen */
            TUint iScreenWidth;
            /** Height of the screen */
            TUint iScreenHeight;
        	/** Texturas */
        	TTexture iBaseColor;  	
        	TTexture iOrigenTextura;
        	TTexture iColorGridTextura;
        	TTexture iMouseTextura;
        	TTexture iLampTextura;
            /** Particle coordinates */
            GLfixed *iParticleCoords;
            // Wait note dialog for indicating refresh operation
            // of the List Model (owned)
    		CAknWaitDialog* iWaitDialog;

            /**
             * Input handler that maps keycodes to inputs and stores the current state
             * for each key. Owned by the C#Name#Container.
             */
            CBlenderLiteInput* iInputHandler;
    };

#endif // BLENDERLITE_H
