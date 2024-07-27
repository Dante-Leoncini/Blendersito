#ifndef __HIDSRVCLIENT_H__
#define __HIDSRVCLIENT_H__

#include <e32base.h>
#include <e32std.h>
#include <e32cmn.h>

#include <bthidevent.h>

class MHIDSrvClient : public CBase
    {
    public:
        virtual TInt Connect() = 0;
        virtual void Close() = 0;
        virtual void EventReady(TRequestStatus* aStatus) = 0;
        virtual void GetEvent(THIDEvent& aEvent) = 0;
        virtual void EventReadyCancel() = 0;
    };

NONSHARABLE_CLASS(RHIDSrvClient) : public RSessionBase, public MHIDSrvClient
    {
    public:
        TInt Connect();
        void Close();
        void EventReady(TRequestStatus* aStatus);
        void GetEvent(THIDEvent& aEvent);
        void EventReadyCancel();
    };

IMPORT_C MHIDSrvClient* GetHIDClientAPI();

#endif

