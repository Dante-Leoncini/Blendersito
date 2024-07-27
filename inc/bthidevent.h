#ifndef __HIDEVENT_H__
#define __HIDEVENT_H__

#include <e32cmn.h>

enum TMouseButtonID
    {
    EMouseButtonNull,
    EMouseButtonLeft,
	EMouseButtonRight,
	EMouseButtonMiddle,
	EMouseButtonSide,
	EMouseButtonForward,
	EMouseButtonBack,   
    };

enum TMouseEventType
    {
    EEventRelativeXY,
    EEventAbsoluteXY,
    EEventButtonUp,
    EEventButtonDown,
    EEventRelativeWheel
    };

enum TKeyEventType
    {
    EEventHIDKeyUp,
    EEventHIDKeyDown
    };
    
enum TConsumerEventCode
    {
    EConsumerNone,
    };
    
class THIDKeyEvent
    {
    public:
        inline TKeyEventType Type();
        inline TInt ScanCode();
        
        TKeyEventType iType;
        TInt iScanCode;
    };
    
TKeyEventType THIDKeyEvent::Type()
    {
    return iType;
    }
    
TInt THIDKeyEvent::ScanCode()
    {
    return iScanCode;
    }
    
class TMouseEvent
    {
    public:
        inline TMouseEventType Type();
        inline TInt Value();
        inline const TPoint& Position();

        TMouseEventType iType;
        TInt iValue;
        TPoint iPosition;
    };
    
TMouseEventType TMouseEvent::Type()
    {
    return iType;
    }
    
TInt TMouseEvent::Value()
    {
    return iValue;
    }
    
const TPoint& TMouseEvent::Position()
    {
    return iPosition;
    }

class THIDConsumerEvent
    {
    public:
        inline TKeyEventType Type();
        inline TConsumerEventCode ButtonCode();
        
        TKeyEventType iType;
        TConsumerEventCode iButtonCode;
    };

TKeyEventType THIDConsumerEvent::Type()
    {
    return iType;
    }
    
TConsumerEventCode THIDConsumerEvent::ButtonCode()
    {
    return iButtonCode;
    }


class THIDEvent
    {
    public:
    enum THIDEventType
        {
        ENullEvent,
        EMouseEvent,
        EKeyEvent,
        EConsumerEvent
        };
        
    public:
        inline THIDEvent();
        inline void SetType(THIDEventType aType);
        inline THIDEventType Type() const;
        inline TMouseEvent* Mouse() const;
        inline THIDKeyEvent* Key() const;
        inline THIDConsumerEvent* Consumer() const;
    private:
        TUint8 iEventData[32];
    };

THIDEvent::THIDEvent()
    {
    SetType(ENullEvent);
    }

void THIDEvent::SetType(THIDEvent::THIDEventType aType)
    {
    *((TInt*)(iEventData)) = aType;
    }

THIDEvent::THIDEventType THIDEvent::Type() const
    {
    return (THIDEvent::THIDEventType)(*((TInt*)(iEventData)));
    }

TMouseEvent* THIDEvent::Mouse() const
    {
    return (TMouseEvent*)(iEventData+4);
    }

THIDKeyEvent* THIDEvent::Key() const
    {
    return (THIDKeyEvent*)(iEventData+4);
    }
    
THIDConsumerEvent* THIDEvent::Consumer() const
    {
    return (THIDConsumerEvent*)(iEventData+4);
    }

#endif
