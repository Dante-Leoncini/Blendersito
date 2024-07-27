#include <coemain.h>

#include "eventmonitor.h"


void CQuakeEventMonitor::RunL()
    {
    TWsEvent wsEvent;
    iWsSession.GetEvent( wsEvent );
    HandleWsEvent( wsEvent );
    iWsSession.EventReady( &iStatus );
    
    SetActive();
    }
    
void CQuakeEventMonitor::DoCancel()
    {
    iWsSession.EventReadyCancel();
    }

CQuakeEventMonitor* CQuakeEventMonitor::NewL( )
    {
    CQuakeEventMonitor* self = new (ELeave) CQuakeEventMonitor;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CQuakeEventMonitor::~CQuakeEventMonitor()
    {
    Cancel();
    }

CQuakeEventMonitor::CQuakeEventMonitor() 
        :CActive(CActive::EPriorityIdle)
    {
    }
        
void CQuakeEventMonitor::ConstructL()
    {
    iWsSession = CCoeEnv::Static()->WsSession();
    iWsSession.EventReady( &iStatus );
    CActiveScheduler::Add( this );
    SetActive();
    }

