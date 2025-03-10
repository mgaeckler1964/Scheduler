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
#include <vcl.h>
#include <vcl/registry.hpp>
#include <gak/t_string.h>
#include <gak/numericString.h>

#pragma hdrstop
#include "MainFrm.h"

USERES("scheduler.res");
USEFORM("CalendarFrm.cpp", CalendarForm);
USEFORM("MainFrm.cpp", MainForm);
USEFORM("AlarmDlg.cpp", AlarmDialog);
USEFORM("EditDateDlg.cpp", EditDateDialog);
USEFORM("scheduleFrm.cpp", ScheduleForm);
USEFORM("RecurringFrm.cpp", RecurringForm);
USEFORM("..\GAKLIB\Repository\UserDlg.cpp", UserDialog);
USEFORM("ProjectDlg.cpp", ProjectDialog);
USEFORM("ActivityFrm.cpp", ActivityForm);
USEFORM("ExportActivitiesDlg.cpp", ExportActivitiesDialog);
USEFORM("SetupDlg.cpp", SetupDialog);
USEFORM("DataUnit.cpp", theDataModule); /* TDataModule: File Type */
USELIB("..\..\Object\gaklib\gaklib_bcb.lib");
USEFORM("..\GAKLIB\Repository\LoginDlg.cpp", LoginForm);
USEFORM("..\GAKLIB\Repository\changePasswordDlg.cpp", ChangePasswordForm);
USEFORM("PermissionsFrm.cpp", PermissionsForm);
USEFORM("..\GAKLIB\Repository\Config.cpp", ConfigDataModule); /* TDataModule: File Type */
USEFORM("AboutFrm.cpp", AboutBox);
//---------------------------------------------------------------------------
#include "scheduleFrm.h"
#include "ActivityFrm.h"
#include "CalendarFrm.h"
#include "RecurringFrm.h"

//---------------------------------------------------------------------------
using namespace gak;
using namespace vcl;
//---------------------------------------------------------------------------
static void saveActiveChild( void )
{
	TScheduleForm	*ScheduleForm;
	TRecurringForm	*RecurringForm;
	TActivityForm	*ActivityForm;
	TCalendarForm	*CalendarForm;

	int		   	userId;
	ArrayOfInts	users;
	STRING	   	usersToOpen;

	for( int i=0; i<MainForm->MDIChildCount; i++ )
	{
		userId = 0;

		ScheduleForm = dynamic_cast<TScheduleForm *>(MainForm->MDIChildren[i]);
		if( ScheduleForm )
			userId = ScheduleForm->getUserId();

		RecurringForm = dynamic_cast<TRecurringForm *>(MainForm->MDIChildren[i]);
		if( RecurringForm )
			userId = RecurringForm->getUserId();

		ActivityForm = dynamic_cast<TActivityForm *>(MainForm->MDIChildren[i]);
		if( ActivityForm )
			userId = ActivityForm->getUserId();

		CalendarForm = dynamic_cast<TCalendarForm *>(MainForm->MDIChildren[i]);
		if( CalendarForm )
			userId = CalendarForm->getUserId();


		if( userId )
		{
			if( users.findElement( userId ) == (size_t)-1 )
				users.addElement( userId );
		}
	}

	for(
		ArrayOfInts::const_iterator it = users.cbegin(),
			endIT = users.cend();
		it != endIT;
		++it
	)
	{
		if( !usersToOpen.isEmpty() )
			usersToOpen += ';';

		usersToOpen += formatNumber( *it );
	}
	TRegistry	*reg = new TRegistry;

	reg->OpenKey( REGISTRY_KEY, true );
	if( MainForm->ActiveMDIChild )
	{
		reg->WriteInteger( "ActiveChild", MainForm->ActiveMDIChild->Tag );
		reg->WriteInteger( "ActiveState", MainForm->ActiveMDIChild->WindowState );
	}
	reg->WriteString( "usersToOpen", (const char *)usersToOpen );
	reg->CloseKey();

	delete reg;
}

static void reactivateChild( void )
{
	TForm			*theChild;
	int				i;

	int				theChildTag = 0;
	TWindowState	activeState = wsNormal;
	T_STRING		usersToOpen;
	TRegistry		*reg = new TRegistry;

	if( reg->OpenKey( REGISTRY_KEY, false ) )
	{
		if( reg->ValueExists("ActiveChild") )
			theChildTag = reg->ReadInteger( "ActiveChild" );
		if( reg->ValueExists("ActiveState") )
			activeState = (TWindowState)reg->ReadInteger( "ActiveState" );
		if( reg->ValueExists( "usersToOpen" ) )
			usersToOpen = reg->ReadString( "usersToOpen" ).c_str();

		reg->CloseKey();
	}
	delete reg;

	if( usersToOpen.isEmpty() )
		MainForm->openUser( actUser.ID );
	else
	{
		int	userId;
		for( STRING	theUserToOpen = usersToOpen.getFirstToken( ";" );
			!theUserToOpen.isEmpty();
			theUserToOpen = usersToOpen.getNextToken()
		)
		{
			userId = theUserToOpen.getValueE<unsigned>();
			MainForm->openUser( userId );
		}
	}

	for( i=0; i<MainForm->MDIChildCount; i++ )
	{
		theChild = MainForm->MDIChildren[i];
		if( theChild->Tag == theChildTag )
		{
			theChild->BringToFront();
			theChild->SetFocus();
			theChild->WindowState = activeState;
			break;
		}
	}
}

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "Terminverwaltung";
		Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(TtheDataModule), &theDataModule);
		Application->CreateForm(__classid(TSetupDialog), &SetupDialog);
		Application->CreateForm(__classid(TProjectDialog), &ProjectDialog);
		Application->CreateForm(__classid(TUserDialog), &UserDialog);
		Application->CreateForm(__classid(TAlarmDialog), &AlarmDialog);
		Application->CreateForm(__classid(TEditDateDialog), &EditDateDialog);
		Application->CreateForm(__classid(TExportActivitiesDialog), &ExportActivitiesDialog);
		Application->CreateForm(__classid(TLoginForm), &LoginForm);
		Application->CreateForm(__classid(TChangePasswordForm), &ChangePasswordForm);
		Application->CreateForm(__classid(TPermissionsForm), &PermissionsForm);
		Application->CreateForm(__classid(TConfigDataModule), &ConfigDataModule);
		Application->CreateForm(__classid(TAboutBox), &AboutBox);
		if( !Application->Tag )
		{
			MainForm->Show();
			if( MainForm->theDatabase->Connected )
			{
				reactivateChild();
				Application->Run();
				saveActiveChild();
			}
		}
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------






