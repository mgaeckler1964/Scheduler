/*
		Project:		Scheduler
		Module:			
		Description:	
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2024 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Austria, Linz ``AS IS''
		AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
		TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
		PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
		CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
		SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
		LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
		USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
		ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
		OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
		OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
		SUCH DAMAGE.
*/


// TODO:	About

#include <fstream>

#include <Carbon/Carbon.h>

#include <gak/csv.h>

using namespace gak;

typedef struct
{
	int		taskId;
	STRING	project;
	STRING	title;
	STRING	comboEntry;
} TASK_ENTRY;

static Array<TASK_ENTRY>	theTaskEntries;

static int currentTask = -1;
static time_t	startTime;

const ControlID taskFileEditID		= { 'MOBI', 1 };
const ControlID activityFileEditID	= { 'MOBI', 2 };
const ControlID activityPopupID		= { 'MOBI', 3 };
const ControlID timeTextID			= { 'MOBI', 4 };
const ControlID startButtonID		= { 'MOBI', 5 };
const ControlID stopButtonID		= { 'MOBI', 6 };

const CFStringRef taskFileKey		= CFSTR("taskFileName");
const CFStringRef activityFileKey	= CFSTR("activityFileName");
const CFStringRef lastActivityKey	= CFSTR("lastActivity");

static WindowRef	window;

//Global proc ptr used for Files
static NavEventUPP			selectTasksFileEventProc;
static NavEventUPP			selectActivityFileEventProc;
static EventLoopTimerUPP	timerUPP;

static EventLoopTimerRef	theTimer;

static STRING activityFileStr;

static void ShowAbout( void )
{
	WindowRef		aboutWindow;
	IBNibRef 		nibRef;
	OSStatus		err;

	// Create a Nib reference passing the name of the nib file (without the .nib extension)
	// CreateNibReference only searches into the application bundle.
	err = CreateNibReference(CFSTR("mobil"), &nibRef);
	require_noerr( err, CantGetNibRef );
	
	// Then create a window. "MainWindow" is the name of the window object. This name is set in 
	// InterfaceBuilder when the nib is created.
	err = CreateWindowFromNib(nibRef, CFSTR( "AboutBox"), &aboutWindow );
	require_noerr( err, CantCreateWindow );
	
	// We don't need the nib reference anymore.
	DisposeNibReference(nibRef);
	
	// The window was created hidden so show it.
	ShowWindow( aboutWindow );

CantCreateWindow:
CantGetNibRef:

	return;
}

static int compareTaskEntries( const void *te1, const void *te2 )
{
	const TASK_ENTRY	*entry1 = (const TASK_ENTRY *)te1;
	const TASK_ENTRY	*entry2 = (const TASK_ENTRY *)te2;

	return strcmp( entry1->comboEntry, entry2->comboEntry );
}

inline OSStatus GetPopupButtonMenuRef( ControlRef popupButton, MenuRef* outMenu )
{
	OSStatus	err;
	
	err = GetControlData( popupButton, 0, kControlPopupButtonMenuRefTag,
			sizeof( MenuRef ), outMenu, NULL );
	
	return err;
}

inline int GetPopupButtonSelIndex( ControlRef popupButton )
{
	return GetControl32BitValue( popupButton )-1;
}

inline void SetPopupButtonSelIndex( ControlRef popupButton, int newIndex )
{
	SetControl32BitValue( popupButton, newIndex+1 );
	DrawOneControl( popupButton );
}

static CFMutableStringRef GetCompletePath( FSRef theFileRef )
{
	FSCatalogInfo		theFileInfo;
	HFSUniStr255		theUniFileName;
	CFMutableStringRef	thePathRef;
	CFStringRef			theFileNameRef;
	FSRef				theParentRef;
	OSStatus			err;
	
	err = FSGetCatalogInfo( 
		&theFileRef, 
		kFSCatInfoNodeID, &theFileInfo, 
		&theUniFileName, NULL, &theParentRef 
	);
	if( err == noErr && theFileInfo.nodeID != fsRtDirID )
	{
		theFileNameRef = CFStringCreateWithCharacters( 
			NULL, theUniFileName.unicode, theUniFileName.length  
		);
		
		thePathRef = GetCompletePath( theParentRef );

		CFStringAppend( thePathRef, CFSTR( "/" ) );
		CFStringAppend( thePathRef, theFileNameRef );
	}
	else
		thePathRef = CFStringCreateMutable( NULL, 0 );
	
	return thePathRef;
}

static CFStringRef PreferenceGetAppStringValue( CFStringRef key )
{
	CFPropertyListRef propertyListRef = CFPreferencesCopyAppValue (
		key, kCFPreferencesCurrentApplication
	);
	
	CFTypeID propType = CFGetTypeID( propertyListRef );
	if( propType == CFStringGetTypeID() )
		return (CFStringRef)propertyListRef;
	
	return CFSTR( "" );
}

static void EnableDisableButtons()
{
	OSStatus	err;
	ControlRef	startButtonRef, stopButtonRef, popupRef;
	
	err = GetControlByID( window, &startButtonID, &startButtonRef );
	if( err == noErr )
		err = GetControlByID( window, &stopButtonID, &stopButtonRef );
	if( err == noErr )
		err = GetControlByID( window, &activityPopupID, &popupRef );
		
	if( err == noErr )
	{
		if( !activityFileStr.isEmpty() && theTaskEntries.size() > 0 )
		{
			EnableControl( startButtonRef );
		}
		else
		{
			DisableControl( startButtonRef );
		}

		if( theTaskEntries.size() > 0 )
		{
			EnableControl( popupRef );
		}
		else
		{
			DisableControl( popupRef );
		}

		if( currentTask >= 0 )
		{
			EnableControl( stopButtonRef );
		}
		else
		{
			DisableControl( stopButtonRef );
		}
	}
}

static void readTaskFile( const char *fileName )
{
	ArrayOfStrings	fieldDef;
	FieldSet		record;
	TASK_ENTRY		taskEntry;
	int				id;

	std::ifstream	csvFile( fileName );

	if( csvFile.is_open() )
	{
		readCSVLine( csvFile, &fieldDef );

		theTaskEntries.clear();
		while( !csvFile.eof() && !csvFile.fail() )
		{
			readFIELD_SET( csvFile, fieldDef, &record );
			if( csvFile.eof() || csvFile.fail() )
				break;

			id = record["ID"];
			taskEntry.taskId = id;
			taskEntry.project = record["Project"];
			taskEntry.title = record["title"];

			if( id && !taskEntry.project.isEmpty() && !taskEntry.title.isEmpty() )
			{
				taskEntry.comboEntry = record["Project"];
				taskEntry.comboEntry += " - ";
				taskEntry.comboEntry += STRING(record["Title"]);

				theTaskEntries.addElement( taskEntry );
			}
		}

		theTaskEntries.qsort( compareTaskEntries );
	}
}

static void loadTasksFile( const char *filePath )
{
	OSStatus	err;

	readTaskFile( filePath );

	ControlHandle	popupHandle;
		
	err = GetControlByID( window, &activityPopupID, &popupHandle );
	if( err == noErr )
	{
		MenuRef	menu;
			
		err = GetPopupButtonMenuRef( popupHandle, &menu );
		if( err == noErr )
			err = DeleteMenuItems( menu, 1, CountMenuItems( menu ) );

		for( int i=0; err==noErr && i<theTaskEntries.size(); i++ )
		{
				CFStringRef	menuText;
				menuText = CFStringCreateWithCString( 
					NULL, (const char*)theTaskEntries[i].comboEntry, kCFStringEncodingWindowsLatin1 
				);
				err = AppendMenuItemTextWithCFString( 
					menu, menuText, 0, i, NULL 
				);
		}
		DrawOneControl( popupHandle );
	}
	EnableDisableButtons();
}

static void ClearPopupButton( void )
{
	OSStatus	err;

	ControlHandle	popupHandle;
		
	err = GetControlByID( window, &activityPopupID, &popupHandle );
	if( err == noErr )
	{
		MenuRef	menu;
			
		err = GetPopupButtonMenuRef( popupHandle, &menu );
		if( err == noErr )
			err = DeleteMenuItems( menu, 1, CountMenuItems( menu ) );

		DrawOneControl( popupHandle );
	}
}

static void selectTasksFileCallback( NavEventCallbackMessage cbSelector, NavCBRecPtr cbParams, void *cbUD )
{
	OSStatus	err = noErr;
	if( cbSelector == kNavCBUserAction )
	{
		NavReplyRecord	reply;
		NavUserAction	userAction = 0;
		err = NavDialogGetReply( cbParams->context, &reply );
		if( err == noErr )
		{
			userAction = NavDialogGetUserAction( cbParams->context );
			if( userAction == kNavUserActionOpen )
			{
				ControlHandle	objHandle;

				err = GetControlByID( window, &taskFileEditID, &objHandle );
				if( err == noErr )
				{
					AEDesc	actualDesc;
					FSRef	selectedFileRef;
					
					err = AECoerceDesc( &reply.selection, typeFSRef, &actualDesc );
					if( err == noErr )
					{
						err = AEGetDescData( 
							&actualDesc, (void *)(&selectedFileRef), sizeof( FSRef ) 
						);
						if( err == noErr )
						{
							CFMutableStringRef theFileNameRef = GetCompletePath( selectedFileRef );
							SetControlData( 
								objHandle, 0, kControlEditTextCFStringTag, 
								sizeof(CFStringRef), &theFileNameRef 
							);
							CFPreferencesSetAppValue(taskFileKey, theFileNameRef, kCFPreferencesCurrentApplication );
							CFPreferencesAppSynchronize( kCFPreferencesCurrentApplication );
							loadTasksFile( CFStringGetCStringPtr( theFileNameRef, CFStringGetFastestEncoding(theFileNameRef) ) );
							CFRelease( theFileNameRef );
							DrawOneControl( objHandle );
						}
					}
				}
				
			}
			NavDisposeReply( &reply );
		}
	}
	else if( cbSelector == kNavCBTerminate )
	{
		NavDialogDispose( cbParams->context );
		DisposeNavEventUPP( selectTasksFileEventProc );
	}
}

static OSStatus selectTasksFile( void )
{
	OSStatus err = noErr;
	NavDialogRef theOpenDialog;
	NavDialogCreationOptions dialogOptions;

	// Set up a dialog with default options 
	err = NavGetDefaultDialogCreationOptions (&dialogOptions);
	if (err == noErr )
	{
		dialogOptions.modality = kWindowModalityAppModal;	
		selectTasksFileEventProc = NewNavEventUPP (selectTasksFileCallback);	
		/* Note: passing in NULL here causes all files to be displayed in the dialog.
		May want to change this to filter the file types */
		err =  NavCreateGetFileDialog (
			&dialogOptions, NULL, selectTasksFileEventProc, 
			NULL, NULL, NULL, &theOpenDialog 
		);
		if( err == noErr&& theOpenDialog != NULL )
		{
			if ((err = NavDialogRun (theOpenDialog)) != noErr)
			{
				NavDialogDispose (theOpenDialog);
				DisposeNavEventUPP (selectTasksFileEventProc);
			}
		}
	}
	return err;
}


static void selectActivityFileCallback( NavEventCallbackMessage cbSelector, NavCBRecPtr cbParams, void *cbUD )
{
	OSStatus	err = noErr;
	if( cbSelector == kNavCBUserAction )
	{
		NavReplyRecord	reply;
		NavUserAction	userAction = 0;
		err = NavDialogGetReply( cbParams->context, &reply );
		if( err == noErr )
		{
			userAction = NavDialogGetUserAction( cbParams->context );
			if( userAction == kNavUserActionSaveAs )
			{
				ControlHandle	objHandle;

				err = GetControlByID( window, &activityFileEditID, &objHandle );
				if( err == noErr )
				{
					AEDesc	actualDesc;
					FSRef	selectedFileRef;
					
					err = AECoerceDesc( &reply.selection, typeFSRef, &actualDesc );
					if( err == noErr )
					{
						err = AEGetDescData( 
							&actualDesc, (void *)(&selectedFileRef), sizeof( FSRef ) 
						);
						if( err == noErr )
						{
							CFMutableStringRef theFileNameRef = GetCompletePath( selectedFileRef );
							CFStringAppend( theFileNameRef, CFSTR( "/" ) );
							CFStringAppend( theFileNameRef, reply.saveFileName );

							activityFileStr = CFStringGetCStringPtr( theFileNameRef, CFStringGetFastestEncoding(theFileNameRef) );

							SetControlData( 
								objHandle, 0, kControlEditTextCFStringTag, 
								sizeof(CFStringRef), &theFileNameRef 
							);
							CFPreferencesSetAppValue(activityFileKey, theFileNameRef, kCFPreferencesCurrentApplication );
							CFPreferencesAppSynchronize( kCFPreferencesCurrentApplication );

							CFRelease( theFileNameRef );
							DrawOneControl( objHandle );
						}
					}
				}
			}
			NavDisposeReply( &reply );
		}
	}
	else if( cbSelector == kNavCBTerminate )
	{
		NavDialogDispose( cbParams->context );
		DisposeNavEventUPP( selectActivityFileEventProc );
	}

	EnableDisableButtons();
}

static OSStatus selectActivityFile( void )
{
	OSStatus err = noErr;
	NavDialogRef theSaveDialog;
	NavDialogCreationOptions dialogOptions;

	// Set up a dialog with default options 
	err = NavGetDefaultDialogCreationOptions (&dialogOptions);
	if (err == noErr )
	{
		dialogOptions.modality = kWindowModalityAppModal;	
		selectActivityFileEventProc = NewNavEventUPP (selectActivityFileCallback);	
		/* Note: passing in NULL here causes all files to be displayed in the dialog.
		May want to change this to filter the file types */
		err =  NavCreatePutFileDialog (
			&dialogOptions, 'mcsv', kNavGenericSignature, selectActivityFileEventProc, 
			NULL, &theSaveDialog 
		);
		if( err == noErr && theSaveDialog != NULL)
		{
			if ((err = NavDialogRun (theSaveDialog)) != noErr)
			{
				NavDialogDispose (theSaveDialog);
				DisposeNavEventUPP (selectActivityFileEventProc);
			}
		}
	}
	return err;
}

pascal void TimerAction (EventLoopTimerRef  theTimer,
                         void* userData)
{
	time_t		curTimer;
	int			ellapsed, seconds, minutes, hours;
	char		buffer[16];

	time( &curTimer );
	
	ellapsed = curTimer - startTime;
	seconds = ellapsed % 60;
	ellapsed /= 60;
	minutes = ellapsed % 60;
	ellapsed /= 60;
	hours = ellapsed;
	
	sprintf( buffer, "%02d:%02d:%02d", hours, minutes, seconds );
	
	CFStringRef		newTimerStr = CFStringCreateWithFormat( NULL, NULL, CFSTR( "%s" ), buffer );
	ControlRef		timerLabel;
	
	OSStatus	err = GetControlByID( window, &timeTextID, &timerLabel );
	
	if( err == noErr )
	{
		SetControlData( timerLabel, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &newTimerStr );
		DrawOneControl( timerLabel );
	}

	CFRelease( newTimerStr );
}

static void startActivity()
{
	EventLoopRef       mainLoop;
 
	OSStatus		err;
	ControlHandle	popupHandle;
	int				selItemIndex = -1;
		
	err = GetControlByID( window, &activityPopupID, &popupHandle );
	if( err == noErr )
		selItemIndex = GetPopupButtonSelIndex( popupHandle );

	if( selItemIndex >= 0 && currentTask != theTaskEntries[selItemIndex].taskId )
	{
		int				newFile;

		newFile = access( activityFileStr, 0 );

		std::ofstream	csvFile( activityFileStr, std::ios::out|std::ios::app );
		time_t			nowSec;
		time( &nowSec );
		struct tm *now = localtime( &nowSec );
		
		if( csvFile.is_open() )
		{
			if( newFile )
				csvFile << "ID,PROJECT,TITLE,START_TIME\n";
				
			csvFile <<
				theTaskEntries[selItemIndex].taskId << ",\"" <<
				theTaskEntries[selItemIndex].project << "\",\"" <<
				theTaskEntries[selItemIndex].title << "\"," <<
				(now->tm_mday+1) << '.' << (now->tm_mon+1) << '.' << (now->tm_year+1900) << ' ' <<
				now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << '\n';


			currentTask = theTaskEntries[selItemIndex].taskId;

			ControlRef	stopHandle;
			err = GetControlByID( window, &stopButtonID, &stopHandle );
			if( err == noErr )
				EnableControl( stopHandle );

			CFPreferencesSetAppValue(
				lastActivityKey, 
				CFStringCreateWithFormat( 
					NULL, NULL, CFSTR( "%d" ), currentTask 
				), 
				kCFPreferencesCurrentApplication 
			);
			CFPreferencesAppSynchronize( kCFPreferencesCurrentApplication );

			time(&startTime);

			mainLoop = GetMainEventLoop();
			timerUPP = NewEventLoopTimerUPP(TimerAction);
 
			InstallEventLoopTimer( mainLoop,
				1,
				1,
				timerUPP,
				NULL,
				&theTimer
			);		
		}
	}
}

static void stopActivity(bool logStop)
{
	OSStatus		err;

	if( currentTask >= 0 )
	{
		int				newFile;

		newFile = access( activityFileStr, 0 );

		std::ofstream	csvFile( activityFileStr, std::ios::out|std::ios::app );
		time_t			nowSec;
		time( &nowSec );
		struct tm *now = localtime( &nowSec );
		
		if( csvFile.is_open() )
		{
			if( newFile )
				csvFile << "ID,PROJECT,TITLE,START_TIME\n";
				
			csvFile <<
				0 << ",\"" <<
				'-' << "\",\"" <<
				'-' << "\"," <<
				(now->tm_mday+1) << '.' << (now->tm_mon+1) << '.' << (now->tm_year+1900) << ' ' <<
				now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << '\n';


			currentTask = -1;

			ControlRef	stopHandle;
			err = GetControlByID( window, &stopButtonID, &stopHandle );
			if( err == noErr )
				DisableControl( stopHandle );

			if( logStop )
			{
				CFPreferencesSetAppValue( 
					lastActivityKey, NULL, kCFPreferencesCurrentApplication 
				);
				CFPreferencesAppSynchronize( kCFPreferencesCurrentApplication );
			}

			RemoveEventLoopTimer( theTimer );
			DisposeEventLoopTimerUPP( timerUPP );
			CFStringRef		newTimerStr =  CFSTR( "00:00:00" );
			ControlRef		timerLabel;
	
			OSStatus	err = GetControlByID( window, &timeTextID, &timerLabel );
	
			if( err == noErr )
			{
				SetControlData( timerLabel, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &newTimerStr );
				DrawOneControl( timerLabel );
			}
		}
	}
}

static void exitStopActivity( void )
{
	stopActivity( false );
}

static pascal OSStatus MobileEventHandler( EventHandlerCallRef handlerRef, EventRef event, void *userData )
{
	OSStatus	err = noErr;

	if( GetEventClass( event ) == kEventClassCommand )
	{
		HICommand	command;

		GetEventParameter( event, kEventParamDirectObject, typeHICommand, NULL, sizeof(command), NULL, &command );

		switch( command.commandID )
		{
			case 'abou':
				ShowAbout();
				break;
			case 'brtk':
				selectTasksFile();
				break;
			case 'brac':
				selectActivityFile();
				break;
			case 'strt':
				startActivity();
				break;
			case 'stop':
				stopActivity(true);
				break;
			case 'quit':
				stopActivity(false);
			default:
				err = eventNotHandledErr;
		}
	}
	
	return err;
}


int main(int argc, char* argv[])
{
	CFStringRef		taskFile;
	CFStringRef		activityFile;
    IBNibRef 		nibRef;
    int				curActivity;
	Boolean			keyExists;
	
    OSStatus		err;

	EventTypeSpec commSpec[] = 
	{
		{ kEventClassCommand, kEventProcessCommand },
	};

	// Create a Nib reference passing the name of the nib file (without the .nib extension)
	// CreateNibReference only searches into the application bundle.
	err = CreateNibReference(CFSTR("mobil"), &nibRef);
	require_noerr( err, CantGetNibRef );

	// Once the nib reference is created, set the menu bar. "MainMenu" is the name of the menu bar
	// object. This name is set in InterfaceBuilder when the nib is created.
	err = SetMenuBarFromNib(nibRef, CFSTR("MenuBar"));
	require_noerr( err, CantSetMenuBar );

	// Then create a window. "MainWindow" is the name of the window object. This name is set in 
	// InterfaceBuilder when the nib is created.
	err = CreateWindowFromNib(nibRef, CFSTR("MainWindow"), &window);
	require_noerr( err, CantCreateWindow );

	// We don't need the nib reference anymore.
	DisposeNibReference(nibRef);

	// The window was created hidden so show it.
	ShowWindow( window );

	InstallWindowEventHandler( window, NewEventHandlerUPP(MobileEventHandler), 1, commSpec, NULL, NULL );

	ClearPopupButton();

	taskFile = PreferenceGetAppStringValue( taskFileKey );
	if( CFStringGetLength( taskFile ) > 0 )
	{
		ControlRef	objHandle;

		GetControlByID( window, &taskFileEditID, &objHandle );
		SetControlData( 
			objHandle, 0, kControlEditTextCFStringTag, 
			sizeof(CFStringRef), &taskFile 
		);
		loadTasksFile( CFStringGetCStringPtr( taskFile, CFStringGetFastestEncoding(taskFile) ) );
		DrawOneControl( objHandle );
	}
	activityFile = PreferenceGetAppStringValue( activityFileKey );
	if( CFStringGetLength( activityFile ) > 0 )
	{
		ControlRef	objHandle;

		activityFileStr = CFStringGetCStringPtr( activityFile, CFStringGetFastestEncoding(activityFile) );

		GetControlByID( window, &activityFileEditID, &objHandle );
		SetControlData( 
			objHandle, 0, kControlEditTextCFStringTag, 
			sizeof(CFStringRef), &activityFile 
		);
		DrawOneControl( objHandle );
	}
	
	curActivity = CFPreferencesGetAppIntegerValue( 
		lastActivityKey, kCFPreferencesCurrentApplication, &keyExists 
	);
	if( keyExists )
	{
		for( int i=0; i<theTaskEntries.size(); i++ )
		{
			if( theTaskEntries[i].taskId == curActivity )
			{
				ControlRef	objHandle;
				
				GetControlByID( window, &activityPopupID, &objHandle );
				SetPopupButtonSelIndex( objHandle, i );
				startActivity();
				
				break;
			}
		}
	}

	EnableDisableButtons();
	
	atexit( exitStopActivity );

    // Call the event loop
	RunApplicationEventLoop();

CantCreateWindow:
CantSetMenuBar:
CantGetNibRef:
	return err;
}

