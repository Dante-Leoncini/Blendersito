/*
 * ==============================================================================
 *  Name        : BlenderLiteContainer.h
 *  Part of     : OpenGLEx / BlenderLite
 * ==============================================================================
 */

#ifndef BLENDERLITECONTAINER_H
#define BLENDERLITECONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <aknnotedialog.h>
#include <AknNotifyStd.h>
#include <apadef.h>
#include <aknprogressdialog.h>
#include <GLES/egl.h>
#include <akndef.h>

#include "BlenderLite.h"
#include "BlenderLiteinput.h"

// CLASS DECLARATION

// FORWARD DECLARATIONS
class CEikLabel;
class CAknExNoteView;

/**
 * Container control class that handles the OpenGL ES initialization and deinitializations.
 * Also uses the CBlenderLite class to do the actual OpenGL ES rendering.
 */

class CBlenderLiteContainer : public CCoeControl, MCoeControlObserver, public MProgressDialogCallback
    {       
	private: // Enumerations
	
		enum TAknExNoteComponentControls
			{
			EAknExNoteComponentLabel,
			EAknExNoteComponentListbox
			}; 
    public: // Constructors and destructor
        /**
         * EPOC default constructor. Initializes the OpenGL ES and creates the rendering context.
         * @param aRect Screen rectangle for container.
         */
        void ConstructL(const TRect& aRect);

        /**
         * Destructor. Destroys the CPeriodic, CBlenderLite and uninitializes OpenGL ES.
         */
        virtual ~CBlenderLiteContainer();

    public: // New functions

        /**
        * ShowGeneralNoteL
        * Indicates general note
        * @param aResourceId
        * @param aTimeout
        * @param aTone
        */
        void ShowGeneralNoteL( TInt aResourceId, 
                               const CAknNoteDialog::TTimeout aTimeout,
                               const CAknNoteDialog::TTone aTone );

        /**
        * ShowGeneralNoteL
        * Indicates general note.
        * @param aResourceId
        * @param aControlId
        * @param aTimeout
        * @param aTone
        * @param aPlural
        */
        void ShowGeneralNoteL( TInt aResourceId,
                               TInt aControlId,
                               const CAknNoteDialog::TTimeout aTimeout,
                               const CAknNoteDialog::TTone aTone,
                               TBool aPlural );

        /**
        * ShowDynamicNumberNoteL
        * Indicates note
        */
        void ShowDynamicNumberNoteL();

        /**
        * ShowWaitNoteL
        * Indicates wait note.
        * @param aResourceId
        * @param aControlId
        */
        void ShowWaitNoteL( TInt aResourceId, TInt aControlId );

        /**
        * ShowProgressNoteUnderSingleProcessL
        * Indicates ProgressNote.
        * @param aResourceId
        * @param aControlId
        */
        void ShowProgressNoteUnderSingleProcessL( TInt aResourceId,   
                                                  TInt aControlId );

        /**
        * ShowShowNoteL
        * Show Note
        * @param aType 
        * @param aResourceId
        */
        void ShowShowNoteL( TAknGlobalNoteType aType, 
                            TInt aResourceId );

        /**
        * CallbackIncrementProgressNoteL
        * Callback function to increment progress note
        * @param aThis
        * @return TInt Return 0 when work is done, otherwise return 1.
        */
        static TInt CallbackIncrementProgressNoteL( TAny* aThis );
        
        /**
        * UpdateProgressNote
        * The entity of ProgressCallBackL() function
        * @return TInt 0 when work is done, otherwise return 1.
        */
        TInt UpdateProgressNote();

        /**
         * Callback function for the CPeriodic. Calculates the current frame, keeps the background
         * light from turning off and orders the CBlenderLite to do the rendering for each frame.
         *@param aInstance Pointer to this instance of CBlenderLiteContainer.
         */
        static int DrawCallBack( TAny* aInstance );

    public: // From MProgressDialogCallback
        
        /**
         * Para cambiar al modo Widescreen
         */
        void SetWidescreen();
        
        /**
        * DialogDismissedL
        * Called when/if the dialog has been dismissed.
        * @param aButtonId
        */
        void DialogDismissedL( TInt aButtonId );    

    private: // Functions from base classes

        /**
         * Method from CoeControl that gets called when the display size changes.
         * If OpenGL has been initialized, notifies the renderer class that the screen
         * size has changed.
         */
        void SizeChanged();

        /**
        * CountComponentControls.
        * Returns number of component.
        * @return Number of controls
        */
        TInt CountComponentControls() const;

        /**
        * ComponentControl.
        * Returns pointer to particular component.
        * @param aIndex Index of the control
        * @return Pointer to Control's component control, identified by index
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         * Handles a change to the control's resources. This method
         * reacts to the KEikDynamicLayoutVariantSwitch event (that notifies of
         * screen size change) by calling the SetExtentToWholeScreen() again so that
         * this control fills the new screen size. This will then trigger a call to the
         * SizeChanged() method.
         * @param aType Message UID value, only KEikDynamicLayoutVariantSwitch is handled by this method.
         */
        void HandleResourceChange(TInt aType);

        /**
         * Method from CCoeControl. Does nothing in this implementation.
         * All rendering is done in the DrawCallBack() method.
         */
        void Draw(const TRect& aRect) const;
        
        /**
		 * Handles joystick movements and other key presses.
		 * @param aKeyEvent Key event.
		 * @param aType Key event type.
		 * @return EKeyWasConsumed if the key event was handled or EKeyWasNotConsumed if the key event was not handled.
		 */
		TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType );

        /**
         * Method from MCoeControlObserver that handles an event from the observed control.
         * Does nothing in this implementation.
		 * @param aControl   Control changing its state.
		 * @param aEventType Type of the control event.
         */
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

    private: //data 
        /**
        * iLabel
        * owned by CAknExNoteContainer object.
        */
        CEikLabel*                      iLabel;

        /**
        * iIdle
        * owned by CAknExNoteContainer object.
        */
        CIdle*                          iIdle;

        /**
        * iProgressDialog
        * Not owned by CAknExNoteContainer object.
        */
        CAknProgressDialog*             iProgressDialog;

        /**
        * iProgressInfo
        * Not owned by CAknExNoteContainer object.
        */
        CEikProgressInfo*               iProgressInfo;

        /**
        * iView
        * Not owned by CAknExNoteContainer object.
        */
        CAknExNoteView*                 iView;

        /** Private Integers for Notes behaviours **/
        TInt                            iGlobalNoteId;
        TInt                            iPermanentNoteId;
        TRequestStatus                  iStatus;

        /** Display where the OpenGL ES window surface resides. */
        EGLDisplay  iEglDisplay;

        /** Window surface that is the target of OpenGL ES graphics rendering. */
        EGLSurface  iEglSurface;

        /** OpenGL ES rendering context. */
        EGLContext  iEglContext;

        /** Active object that is the timing source for the animation. */
        CPeriodic*  iPeriodic;

        /**
         * Flag that indicates if OpenGL ES has been initialized or not.
         * Used to check if SizeChanged() can react to incoming notifications.
         */
        TBool iOpenGlInitialized;

        /** Time (int ticks) when the example started running (rendering frames). */
        TUint iStartTimeTicks;
        /** Time (in seconds) when the previous frame was rendered. */
        GLfloat iLastFrameTimeSecs;
        
        /** Input handler that maps keycodes to inputs and stores the current state for each key. */
        CBlenderLiteInput* iInputHandler;

    public:  //data

        /** Frame counter variable, used in the animation. */
        TInt iFrame;

        /** Used in DrawCallBack() method to do the actual OpenGL ES rendering.  */
        CBlenderLite* iBlenderLite;
    };

#endif

// End of File
