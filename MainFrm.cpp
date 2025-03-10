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

//---------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

#include <dir.h>
#include <vcl.h>
#include <vcl/registry.hpp>
#include <gak/vcl_tools.h>
#pragma hdrstop

#include "LoginDlg.h"
#include "UserDlg.h"
#include "ChangePasswordDlg.h"

#include "MainFrm.h"
#include "PermissionsFrm.h"
#include "ProjectDlg.h"
#include "SetupDlg.h"
#include "scheduleFrm.h"
#include "ActivityFrm.h"
#include "CalendarFrm.h"
#include "RecurringFrm.h"

#include "config.h"
#include "AboutFrm.h"

//---------------------------------------------------------------------------
using namespace gak;
using namespace vcl;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------


char REGISTRY_KEY[] = "\\Software\\gak\\Scheduler";
TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
int TMainForm::loadPermissions( int schedulerId )
{
	doEnterFunction( "TMainForm::loadPermissions()" );

	int	userPermissions = 0;

	if( actUser->permissions & USER_SYSTEM_PERM || actUser->ID == schedulerId )
		userPermissions = -1;
	else
	{
		TQuery *theQuery = new TQuery( NULL );
		theQuery->DatabaseName = theDatabase->DatabaseName;
		theQuery->SQL->Add(
			"select permissions "
			"from acls "
			"where scheduler_id = :theID "
			"and (user_id = :theUser1 "
				"or user_id in ("
					"select group_id "
					"from u_rights "
					"where user_id = :theUser2"
				")"
			")"
		);
		theQuery->Params->Items[0]->AsInteger = schedulerId;
		theQuery->Params->Items[1]->AsInteger = actUser->ID;
		theQuery->Params->Items[2]->AsInteger = actUser->ID;

		for( theQuery->Open(); !theQuery->Eof; theQuery->Next() )
			userPermissions |= theQuery->Fields->Fields[0]->AsInteger;
		theQuery->Close();

		delete theQuery;
	}
	return userPermissions;
}

//---------------------------------------------------------------------------
void TMainForm::login( void )
{
	bool	showLogin;

	if( !actUser )	// first call
	{
		actUser = &::getActUser( theDatabase->DatabaseName );
		showLogin = false;
	}
	else			// call from menu
	{
		showLogin = true;
	}

	do
	{
		if( showLogin )
		{
			if( LoginForm->ShowModal( "Scheduler" ) == mrOk )
			{
				actUser = &::loginUser(
					theDatabase->DatabaseName,
					LoginForm->EditUserName->Text,
					LoginForm->EditPassword->Text
				);
			}
			else
			{
/*v*/			break;
			}
		}

		if( !actUser->ID )
			Application->MessageBox( "Unbekannter Benutzer oder Kennwort", "Fehler", MB_ICONSTOP );

		if( !actUser->permissions & USER_LOGIN_PERM )
		{
			Application->MessageBox( "Anmeldung abgelehnt", "Error", MB_ICONSTOP );
			actUser = NULL;
		}

		showLogin = true;
	} while( !actUser || actUser->ID == 0 );

	if( actUser->ID )
		StatusBar->Panels->Items[2]->Text = (const char *)actUser->userName;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Ende1Click(TObject *)
{
	doEnterFunction("TMainForm::Ende1Click");
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::theTimerTimer(TObject *)
{
	TDateTime		now = Now();
	unsigned short	hour,
					min,
					sec,
					msec,
					newTimer;
	char			theTime[32];

	theTimer->Enabled = false;
	theTimer->Interval = 0;

	now.DecodeTime( &hour, &min, &sec, &msec );

	newTimer = (unsigned short)(60000U - (sec*1000U) - msec);

	theTimer->Interval = newTimer;
	theTimer->Enabled = true;

	sprintf( theTime, "%02u:%02u", (unsigned short)hour, (unsigned short)min );
	StatusBar->Panels->Items[3]->Text = theTime;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ToolButton1Click(TObject *)
{
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileUserClick(TObject *)
{
	if( UserDialog->ShowModalWithDB( theDatabase->DatabaseName ) == mrOk )
		openUser( UserDialog->selectedUser );

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AppMinimize(TObject *)
{
	if( SetupDialog->GetInfoMinimize() )
	{
		iconData.cbSize = sizeof(iconData);
		iconData.hWnd = Handle;
		iconData.uID = 100;
		iconData.uFlags = NIF_MESSAGE + NIF_ICON + NIF_TIP;
		iconData.uCallbackMessage = WM_USER + 1;
		iconData.hIcon = Application->Icon->Handle;

		Shell_NotifyIcon(NIM_ADD, &iconData);

		ShowWindow( Application->Handle, SW_HIDE );
	}
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::AppWindowProc(TMessage &msg)
{
	doEnterFunction("TMainForm::AppWindowProc");
	doLogValue( msg.Msg );

	if( msg.Msg == WM_USER + 1 )
	{
		if( msg.LParam == WM_LBUTTONUP
		||  msg.LParam == WM_RBUTTONUP )
		{
			Shell_NotifyIcon(NIM_DELETE, &iconData);

			ShowWindow( Application->Handle, SW_SHOW );
			Application->Restore();
			Show();
			Activate();
			SetFocus();
		}

/***/	return;
	}
	else if( msg.Msg == WM_ENDSESSION )
	{
		theDatabase->Close();
		Session->Close();
/***/	exit( EXIT_SUCCESS );
	}

	WndProc( msg );
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::FormDestroy(TObject *)
{
	doEnterFunction("TMainForm::FormDestroy");
	Shell_NotifyIcon(NIM_DELETE, &iconData);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Projekte1Click(TObject *)
{
	ProjectDialog->ShowModal();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::Einstellungen1Click(TObject *)
{
	SetupDialog->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormActivate(TObject *)
{
	setDateFormats();
}
//---------------------------------------------------------------------------

void TMainForm::openUser( int userId )
{
	int perms = loadPermissions( userId );
	if( !perms )
	{
		Application->MessageBox( "Keine Rechte", "Fehler", MB_ICONERROR );
	}
	else
	{
		UserOrGroup		theUser;

		TScheduleForm	*ScheduleForm		= NULL;
		TRecurringForm	*RecurringForm	= NULL;
		TActivityForm	*ActivityForm		= NULL;
		TCalendarForm	*CalendarForm		= NULL;

		getUserById( userId, &theUser );
		if( theUser.ID )
		{
			for( int i=0; i<MDIChildCount; i++ )
			{
				if( !ScheduleForm )
				{
					ScheduleForm = dynamic_cast<TScheduleForm *>(MainForm->MDIChildren[i]);
					if( ScheduleForm && ScheduleForm->getUserId() == userId )
					{
						ScheduleForm->BringToFront();
						ScheduleForm->SetFocus();
					}
					else
						ScheduleForm = NULL;
				}
				if( !RecurringForm )
				{
					RecurringForm = dynamic_cast<TRecurringForm *>(MainForm->MDIChildren[i]);
					if( RecurringForm && RecurringForm->getUserId() != userId )
						RecurringForm = NULL;
				}
				if( !ActivityForm )
				{
					ActivityForm = dynamic_cast<TActivityForm *>(MainForm->MDIChildren[i]);
					if( ActivityForm && ActivityForm->getUserId() != userId )
						ActivityForm = NULL;
				}
				if( !CalendarForm )
				{
					CalendarForm = dynamic_cast<TCalendarForm *>(MainForm->MDIChildren[i]);
					if( CalendarForm && CalendarForm->getUserId() != userId )
						CalendarForm = NULL;
				}
			}

			if( perms & PERM_SCHEDULE_READ )
			{
				if( !ScheduleForm )
				{
					Application->CreateForm(__classid(TScheduleForm), &ScheduleForm);
					ScheduleForm->setUserId( userId, theUser.userName, perms );
				}
				if( !CalendarForm )
				{
					Application->CreateForm(__classid(TCalendarForm), &CalendarForm);
					CalendarForm->setUserId( userId, theUser.userName );
				}
			}

			if( !RecurringForm && (perms & PERM_RECURRING_READ) )
			{
				Application->CreateForm(__classid(TRecurringForm), &RecurringForm);
				RecurringForm->setUserId( userId, theUser.userName, perms );
			}

			if( !ActivityForm && (perms & PERM_ACTIVITY_READ) )
			{
				Application->CreateForm(__classid(TActivityForm), &ActivityForm);
				ActivityForm->setUserId( userId, theUser.userName, perms );
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject *)
{
	int		dbType = 0;
	bool	MobileServer = false;
	int		ServerPort = 0;
	STRING	theTitle;

	char	ntUserName[128];
	DWORD	size = sizeof( ntUserName );

	STRING	privateDir;
	char	*tempDir;

	HWND	theWindow;

	Application->Tag = 0;

	setDateFormats();

	TRegistry	*reg = new TRegistry;

	if( reg->OpenKey( REGISTRY_KEY, false ) )
	{
		if( reg->ValueExists( "Database" ) )
			dbType = reg->ReadInteger( "Database" );

		if( reg->ValueExists( "MobileServer" ) )
			MobileServer = reg->ReadBool( "MobileServer" );

		if( reg->ValueExists( "ServerPort" ) )
			ServerPort = reg->ReadInteger( "ServerPort" );

		reg->CloseKey();
	}

	delete reg;

	GetUserName( ntUserName, &size );

	if( *ntUserName == 0 )
	{
		Application->MessageBox("Kein Benutzer angemeldet", "Fehler", 0);
		Application->Tag = 1;
		Application->Terminate();
/*@*/	return;
	}

	/*
		if a scheduler allready runs with selected account
		activate the existing application an terminate
	*/
	theTitle = "Terminverwaltung ";
	theTitle += ntUserName;

	if( (theWindow = FindWindow( "TApplication", theTitle )) != 0 )
	{
		Application->MessageBox(
			"Scheduler darf nicht zweimal auf dem gleichen Rechner "
			"mit der gleichen Kennung gestartet werden.",
			"Scheduler", MB_OK|MB_ICONWARNING
		);

		ShowWindow( theWindow, SW_RESTORE );
		SendMessage( theWindow, WM_USER+1, 0, WM_LBUTTONUP );

//		exit(1);
		Application->Tag = 1;
		Application->Terminate();
//		throw EAbort("Kein zweites mal");
/*@*/	return;
	}

	/*
		create the BDE temp directory
	*/
	tempDir = getenv( "TEMP" );

	if( tempDir )
	{
		privateDir = tempDir;
		privateDir += "\\Scheduler";
	}
	else
	{
		privateDir += "C:\\Scheduler";
	}

	privateDir += ntUserName;
	mkdir( privateDir );

	Session->PrivateDir = (const char*)privateDir;

	/*
		open the database
	*/
	try
	{
		if( dbType == 0 )	// automatic selection
		{
			try
			{
				Session->Open();

				theDatabase->Open();
				activeUserQuery->Params->Items[0]->AsString = "GAK";
				activeUserQuery->Open();
			}
			catch( ... )
			{
				theDatabase->Close();
				Session->Close();

				Session->NetFileDir = tempDir;
				theDatabase->AliasName = "SCHEDULE_LOCAL";

				Session->Open();

				theDatabase->Open();
				activeUserQuery->Params->Items[0]->AsString = "GAK";
				activeUserQuery->Open();
			}
			activeUserQuery->Close();
		}
		else if( dbType == 1 )	// local database
		{
			Session->NetFileDir = tempDir;
			theDatabase->AliasName = "SCHEDULE_LOCAL";

			Session->Open();

			theDatabase->Open();
			activeUserQuery->Params->Items[0]->AsString = "GAK";
			activeUserQuery->Open();
			activeUserQuery->Close();
		}
		else if( dbType == 2 )	// network database
		{
			Session->Open();

			theDatabase->Open();
			activeUserQuery->Params->Items[0]->AsString = "GAK";
			activeUserQuery->Open();
			activeUserQuery->Close();
		}

		// logon user
		login();
	}
	catch( Exception &e )
	{
		AnsiString error = "Fehler beim Öffnen der Datenbank";
		error += ":\n";
		error += e.Message;
		Application->MessageBox(
			error.c_str(), "Fehler", 0
		);
		Application->Tag = 1;
		Application->Terminate();
/*@*/	return;
	}
	catch( ... )
	{
		Application->MessageBox(
			"Unbekannter Fehler beim Öffnen der Datenbank", "Fehler", 0
		);
		Application->Tag = 1;
		Application->Terminate();
/*@*/	return;
	}

	if( actUser->ID == 0 )
	{
		Application->MessageBox("Unbekannter Benutzer", "Fehler", 0);
		Application->Tag = 1;
		Application->Terminate();
/*@*/	return;
	}
	else
	{
		Caption = static_cast<const char *>(theTitle);
		Application->Title = static_cast<const char *>(theTitle);
		strcpy( iconData.szTip, theTitle );

		StatusBar->Panels->Items[1]->Text = ntUserName;
	}


	int		sessionCount;

	Application->OnMinimize = AppMinimize;
	WindowProc = AppWindowProc;


	// run the server if required
	if( MobileServer
	&& ServerPort > 0
	&& !theActivitiesServer.isRunning() )
	{
		theActivitiesServer.startServer( ServerPort, 10 );
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Kennwortndern1Click(TObject *)
{
	if( ChangePasswordForm->ShowModal() == mrOk )
	{
		changePassword(
			theDatabase->DatabaseName,
			ChangePasswordForm->EditOldPassword->Text,
			ChangePasswordForm->EditNewPassword->Text
		);
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileLoginClick(TObject *)
{
	for( int i=MDIChildCount-1; i>=0; i-- )
	{
		TForm *theChild = MDIChildren[i];
		theChild->Close();
	}
	login();
	if( actUser->ID )
		openUser( actUser->ID );
	else								// sorry
		Application->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::StatusBarResize(TObject *)
{
	StatusBar->Panels->Items[0]->Width = StatusBar->Width - 150;	
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Rechte1Click(TObject *)
{
	PermissionsForm->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *, TCloseAction &)
{
	doEnterFunction("TMainForm::FormClose");
	theDatabase->Close();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::aboutClick(TObject *)
{
	AboutBox->ShowModal();
}
//---------------------------------------------------------------------------

