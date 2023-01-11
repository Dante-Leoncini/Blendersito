/*
 * ==============================================================================
 *  Name        : BlenderLiteContainer.h
 *  Part of     : OpenGLEx / BlenderLite
 *
 *  Copyright (c) 2004-2006 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation.
 * ==============================================================================
 */

#ifndef BLENDERLITECONTAINER_H
#define BLENDERLITECONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <GLES/egl.h>
#include <akndef.h>
#include "BlenderLite.h"
#include "BlenderLiteinput.h"

// CLASS DECLARATION

/**
 * Container control class that handles the OpenGL ES initialization and deinitializations.
 * Also uses the CBlenderLite class to do the actual OpenGL ES rendering.
 */
class CBlenderLiteContainer : public CCoeControl, MCoeControlObserver
    {
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
         * Callback function for the CPeriodic. Calculates the current frame, keeps the background
         * light from turning off and orders the CBlenderLite to do the rendering for each frame.
         *@param aInstance Pointer to this instance of CBlenderLiteContainer.
         */
        static int DrawCallBack( TAny* aInstance );

    private: // Functions from base classes

        /**
         * Method from CoeControl that gets called when the display size changes.
         * If OpenGL has been initialized, notifies the renderer class that the screen
         * size has changed.
         */
        void SizeChanged();

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
         * Method from CoeControl. Does nothing in this implementation.
         */
        TInt CountComponentControls() const;

        /**
         * Method from CCoeControl. Does nothing in this implementation.
         */
        CCoeControl* ComponentControl(TInt aIndex) const;

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
