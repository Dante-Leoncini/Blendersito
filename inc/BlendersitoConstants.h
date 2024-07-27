#ifndef __AKNEXNOTECONSTANTS_H__
#define __AKNEXNOTECONSTANTS_H__

// CONSTANTS

// All apllications constants are here.
const TInt KBlendersitoGray = 0xaaaaaa;

const TInt KBlendersitoComponentCount = 1;
const TInt KBlendersitoMessageBufLength = 256;
const TInt KBlendersitoRandomNumberRange = 20;
const TInt KBlendersitoProgressbarFinalValue = 200;
const TInt KBlendersitoInitializeTime = 0;
const TInt KBlendersitoInitializeResourceId = 0;
const TInt KBlendersitoTextBufLength = 256;

// Define show time of wait note and permanent note.
// Unit is microsecond. 
const TInt KMaxPermanentNoteShowTime( 6 * 1000000 );

const TUid KViewId = { 1 }; // UID of view
const TInt KBlendersitoTitleBufLength = 256;

const TInt KPermNoteCallBackPriority( CActive::EPriorityStandard );
const TInt KWaitNoteCallBackPriority( CActive::EPriorityStandard );

#endif  //__AKNEXNOTECONSTANTS_H__

// End of File
