#ifndef __AKNEXNOTECONSTANTS_H__
#define __AKNEXNOTECONSTANTS_H__

// CONSTANTS

// All apllications constants are here.
const TInt KBlenderLiteGray = 0xaaaaaa;

const TInt KBlenderLiteComponentCount = 1;
const TInt KBlenderLiteMessageBufLength = 256;
const TInt KBlenderLiteRandomNumberRange = 20;
const TInt KBlenderLiteProgressbarFinalValue = 200;
const TInt KBlenderLiteInitializeTime = 0;
const TInt KBlenderLiteInitializeResourceId = 0;
const TInt KBlenderLiteTextBufLength = 256;

// Define show time of wait note and permanent note.
// Unit is microsecond. 
const TInt KMaxPermanentNoteShowTime( 6 * 1000000 );

const TUid KViewId = { 1 }; // UID of view
const TInt KBlenderLiteTitleBufLength = 256;

const TInt KPermNoteCallBackPriority( CActive::EPriorityStandard );
const TInt KWaitNoteCallBackPriority( CActive::EPriorityStandard );

#endif  //__AKNEXNOTECONSTANTS_H__

// End of File
