#include "../client/client.h"
#include "symbian_local.h"

#include "hidinputmonitor.h"

extern TPoint currentPosition;

static qboolean shift_down = qfalse;

int hid_to_quake[256] =
    {
    /*  00  */ 000,000,000,000,'a','b','c','d','e','f','g','h','i','j','k','l',
    /*  16  */ 'm','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2',
    /*  32  */ '3','4','5','6','7','8','9','0',K_ENTER,K_ESCAPE,K_BACKSPACE,K_TAB,K_SPACE,'+','´','{',
    /*  48  */ '¨','\'',000,']',000,000,',','.','-',000,000,000,000,000,000,000,
    /*  64  */ 000,000,000,000,000,000,000,000,000,000,000,K_PGUP,K_DEL,000,K_PGDN,K_RIGHTARROW,
    /*  80  */ K_LEFTARROW,K_DOWNARROW,K_UPARROW,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  96  */ 000,000,000,000,'<',000,000,000,000,000,000,000,000,000,000,000,
    /*  112 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  128 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  144 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  160 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  176 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  192 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  208 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  224 */ K_CTRL,K_SHIFT,K_ALT,'~',000,K_SHIFT,000,000,000,000,000,000,000,000,000,000,
    /*  240 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000
    };

int hid_to_quake_shifted[256] = 
    {
    /*  00  */ 000,000,000,000,'A','B','C','D','E','F','G','H','I','J','K','L',
    /*  16  */ 'M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','!','"',
    /*  32  */ '#','¤','%','&','/','(',')','=',K_ENTER,K_ESCAPE,K_BACKSPACE,K_TAB,K_SPACE,'?','\\','}',
    /*  48  */ '^','*',000,'[',000,000,';',':','_',000,000,000,000,000,000,000,
    /*  64  */ 000,000,000,000,000,000,000,000,000,000,000,K_KP_PGUP,K_DEL,000,K_KP_PGDN,K_RIGHTARROW,
    /*  80  */ K_LEFTARROW,K_DOWNARROW,K_UPARROW,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  96  */ 000,000,000,000,'>',000,000,000,000,000,000,000,000,000,000,000,
    /*  112 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  128 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  144 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  160 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  176 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  192 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  208 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  224 */ K_CTRL,K_SHIFT,K_ALT,'~',000,K_SHIFT,000,000,000,000,000,000,000,000,000,000,
    /*  240 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000
    };

//int	hid_to_quake [256] = 
//    {
//    
//    /* 0x00 */	0 /* no event */, 0 /* rollover error */, 0 /* POST fail */,
//    		0 /* undefined error */, 'a', 'b', 'c', 'd',
//    /* 0x08 */	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
//    /* 0x10 */	'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
//    /* 0x18 */	'u', 'v', 'w', 'x', 'y', 'z', '1', '2',
//    /* 0x20 */	'3', '4', '5', '6', '7', '8', '9', '0',
//    /* 0x28 */	K_ENTER, K_ESCAPE, '\b', K_TAB, K_SPACE, '-', '=', '[',
//    /* 0x30 */	']', '\\', '\\', ';',	'"','~', ',', '.', // '~' was '`'
//    /* 0x38 */	'/', 0, 0, 0, 0, 0, 0, 0,
//    /* 0x40 */	0, 0, 0, 0, 0, 0, 0, 0,
//    /* 0x48 */	K_PAUSE, 0, 0, K_KP_PGUP, K_DEL, 0, K_KP_PGDN, K_RIGHTARROW,
//    /* 0x50 */	K_LEFTARROW, K_DOWNARROW, K_UPARROW, 0, 0, 0, 0, 0,
//    /* 0x58 */	0, 0, 0, 0, 0, 0, 0, 0,
//    /* 0x60 */	0, 0, 0, 0, 0, 0, 0, 0,
//    /* pmk!! fill these in, even though my keyboard doesn't use them... */
//    /* 0x68 */	0, 0, 0, 0, 0, 0, 0, 0,
//    /* 0x70 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    /* 0x80 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    /* 0x90 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    /* 0xa0 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    /* 0xb0 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    /* 0xc0 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    /* 0xd0 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    /* 0xe0 */	K_CTRL, K_SHIFT, K_ALT,
//    		0,	/* Would COMPOSE be better? */
//    		K_CTRL, K_SHIFT, K_ALT, 0,
//    /* 0xe8 */	0, 0, 0, 0, 0, 0, 0, 0,
//    /* 0xf0 */	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//    };


void CHIDEventMonitor::RunL()
    {
    THIDEvent hidEvent;
    iHIDClient->GetEvent( hidEvent );
    switch (hidEvent.Type())
        {
        case THIDEvent::EMouseEvent:
            {
            TMouseEvent* mouse = hidEvent.Mouse();
            switch (mouse->Type())
                {
                case EEventRelativeXY:
                    currentPosition += mouse->iPosition;
                    break;
                case EEventButtonDown:
                    switch(mouse->iValue)
                        {
                        case EMouseButtonLeft:
                            Sys_QueEvent(0, SE_KEY, K_MOUSE1, qtrue, 0, NULL);
                            break;
                        case EMouseButtonRight:
                            Sys_QueEvent(0, SE_KEY, K_MOUSE2, qtrue, 0, NULL);
                            break;
                        case EMouseButtonMiddle:
                            Sys_QueEvent(0, SE_KEY, K_MOUSE3, qtrue, 0, NULL);
                            break;
                        default:
                            break;
                        }
                    break;
                case EEventButtonUp:
                    switch(mouse->iValue)
                        {
                        case EMouseButtonLeft:
                            Sys_QueEvent(0, SE_KEY, K_MOUSE1, qfalse, 0, NULL);
                            break;
                        case EMouseButtonRight:
                            Sys_QueEvent(0, SE_KEY, K_MOUSE2, qfalse, 0, NULL);
                            break;
                        case EMouseButtonMiddle:
                            Sys_QueEvent(0, SE_KEY, K_MOUSE3, qfalse, 0, NULL);
                            break;
                        default:
                            break;
                        }
                    break;
                case EEventRelativeWheel:
                    if (mouse->iValue > 0)
                        {
                        Sys_QueEvent(0, SE_KEY, K_MWHEELUP, qtrue, 0, NULL );
                        Sys_QueEvent(0, SE_KEY, K_MWHEELUP, qfalse, 0, NULL );
                        }
                    else
                        {
                        Sys_QueEvent(0, SE_KEY, K_MWHEELDOWN, qtrue, 0, NULL );
                        Sys_QueEvent(0, SE_KEY, K_MWHEELDOWN, qfalse, 0, NULL );
                        }
                    break;
                default:
                    break;
                }
            }
            break;
        case THIDEvent::EKeyEvent:
            {
            THIDKeyEvent* key = hidEvent.Key();
            int hid_code = hid_to_quake[key->ScanCode()];
            switch (key->Type())
                {
                case EEventHIDKeyUp:
                    if (hid_code == K_SHIFT)
                        {
                        shift_down = qfalse;
                        }
                    Sys_QueEvent(0, SE_KEY,  hid_code, qfalse, 0, NULL );
                    
                    break;
                case EEventHIDKeyDown:
//                    Com_Printf( "EEventHIDKeyDown, code %d\n",key->ScanCode());
                    if (hid_code == K_SHIFT)
                        {
                        shift_down = qtrue;
                        }
                    Sys_QueEvent(0, SE_KEY,  hid_code, qtrue, 0, NULL );
                    if (shift_down)
                        {
                        hid_code = hid_to_quake_shifted[key->ScanCode()];
                        }
                    if (hid_code <= 127)
                        {
                        if (hid_code == K_BACKSPACE)
                            {
                            hid_code = '\b';
                            }
                        Sys_QueEvent(0, SE_CHAR, hid_code, 0, 0, NULL );
                        }
                    break;
                default:
                    break;
                }
            }
            break;
        case THIDEvent::EConsumerEvent:
            {
            THIDConsumerEvent* consumer = hidEvent.Consumer();
            Com_Printf( "consumer, code %d\n",consumer->ButtonCode());
            break;
            }
        default:
            break;
        }
    iHIDClient->EventReady( &iStatus );
    SetActive();
    }
    
void CHIDEventMonitor::DoCancel()
    {
    iHIDClient->EventReadyCancel();
    }

CHIDEventMonitor* CHIDEventMonitor::NewL()
    {
    CHIDEventMonitor* self = new (ELeave) CHIDEventMonitor;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CHIDEventMonitor::~CHIDEventMonitor()
    {
    Cancel();
    delete iHIDClient;
    }

CHIDEventMonitor::CHIDEventMonitor() 
        :CActive(CActive::EPriorityIdle)
    {
    }
        
void CHIDEventMonitor::ConstructL()
    {
    User::LeaveIfError(iHidLibrary.Load(_L("hidsrv.dll")));
    TLibraryFunction entry = iHidLibrary.Lookup(1);
    if (entry)
        {
        iHIDClient = (MHIDSrvClient*) entry();
        if (iHIDClient)
            {
            User::LeaveIfError(iHIDClient->Connect());
            iHIDClient->EventReady( &iStatus );
            CActiveScheduler::Add( this );
            SetActive();
            }
        }
    else
        {
        User::Leave(KErrNotFound);
        }
    }
