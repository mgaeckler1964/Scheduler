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
#include <fstream>

#include <vcl.h>
#include <registry.hpp>

#include <gak/csv.h>
#include <gak/numericString.h>

#pragma hdrstop

#include "MobileFrm.h"

//---------------------------------------------------------------------------
using namespace gak;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMobileForm *MobileForm;
//---------------------------------------------------------------------------
typedef struct
{
	int	taskId;
	STRING	project;
	STRING	title;
	STRING	comboEntry;
} TASK_ENTRY;

static Array<TASK_ENTRY>	theTaskFile;

//---------------------------------------------------------------------------

#define registryKey		"\\Software\\CRESD\\Scheduler"

//---------------------------------------------------------------------------
static int compareTaskEntries( const void *te1, const void *te2 )
{
	const TASK_ENTRY	*entry1 = (const TASK_ENTRY *)te1;
	const TASK_ENTRY	*entry2 = (const TASK_ENTRY *)te2;

	return strcmp( entry1->comboEntry, entry2->comboEntry );
}

//---------------------------------------------------------------------------
__fastcall TMobileForm::TMobileForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TMobileForm::readTaskFile( void )
{
	ArrayOfStrings	fieldDef;
	FieldSet	  	record;
	TASK_ENTRY	  	taskEntry;
	STRING		  	id;

	std::ifstream	csvFile( TaskFileEdit->Text.c_str() );

	if( csvFile.is_open() )
	{
		readCSVLine( csvFile, &fieldDef );

		theTaskFile.clear();
		while( !csvFile.eof() && !csvFile.fail() )
		{
			readFIELD_SET( csvFile, fieldDef, &record );
			if( csvFile.eof() || csvFile.fail() )
				break;


			id = record["ID"];
			taskEntry.taskId = id.getValueE<unsigned>();
			taskEntry.project = record["Project"];
			taskEntry.title = record["title"];

			if( id[0U] && taskEntry.project[0U] && taskEntry.title[0U] )
			{
				taskEntry.comboEntry = record["Project"];
				taskEntry.comboEntry += " - ";
				taskEntry.comboEntry += STRING(record["Title"]);

				theTaskFile.addElement( taskEntry );
			}
		}

		theTaskFile.qsort( compareTaskEntries );

		TasksComboBox->Items->Clear();
		for(
			Array<TASK_ENTRY>::const_iterator it = theTaskFile.cbegin(),
				endIT = theTaskFile.cend();
			it != endIT;
			++it
		)
		{
			TasksComboBox->Items->Add( static_cast<const char*>(it->comboEntry) );
		}

		TasksComboBox->ItemIndex = 0;
	}
}

//---------------------------------------------------------------------------
void __fastcall TMobileForm::FormShow(TObject *Sender)
{
	StartButton->Enabled = false;
	StopButton->Enabled = false;
	currentTask = 0;

	TRegistry	*registry = new TRegistry;

	if( registry->OpenKey( registryKey, false ) )
	{
		if( registry->ValueExists( "TaskFile" ) )
		{
			TaskFileEdit->Text = registry->ReadString( "TaskFile" );
			readTaskFile();
		}

		if( registry->ValueExists( "ActivityFile" ) )
			ActivityFileEdit->Text = registry->ReadString( "ActivityFile" );

		if( TasksComboBox->Items->Count && ActivityFileEdit->Text > "" )
			StartButton->Enabled = true;

		if( registry->ValueExists( "currentTask" ) )
		{
			int lastTask = registry->ReadInteger( "currentTask" );
			if( lastTask )
			{
				for( size_t i=0; i<theTaskFile.size(); i++ )
				{
					if( theTaskFile[i].taskId == lastTask )
					{
						TasksComboBox->ItemIndex = i;
						StartButtonClick( Sender );
						break;
					}
				}
			}
		}

		registry->CloseKey();
	}
}

//---------------------------------------------------------------------------

void __fastcall TMobileForm::Button1Click(TObject *)
{
	TRegistry	*registry = new TRegistry;

	if( registry->OpenKey( registryKey, false )
	&&  registry->ValueExists( "TaskPath" ) )
	{
		OpenDialog->InitialDir = registry->ReadString( "TaskPath" );
	}
	registry->CloseKey();

	OpenDialog->FileName = "Tasks.csv";
	if( OpenDialog->Execute() )
	{
		TaskFileEdit->Text = OpenDialog->FileName;
		if( registry->OpenKey( registryKey, true ) )
		{
			registry->WriteString( "TaskFile", OpenDialog->FileName );

			AnsiString	TaskPath = OpenDialog->FileName;
			int			lastPos = TaskPath.LastDelimiter( "\\" );

			if( lastPos )
				lastPos--;
			TaskPath.SetLength( lastPos );

			registry->WriteString( "TaskPath", TaskPath );

			registry->CloseKey();
		}

		readTaskFile();

		if( TasksComboBox->Items->Count && ActivityFileEdit->Text > "" )
			StartButton->Enabled = true;
		else
			StartButton->Enabled = false;
	}

	delete registry;
}
//---------------------------------------------------------------------------

void __fastcall TMobileForm::CloseButtonClick(TObject *)
{
	Close();
}
//---------------------------------------------------------------------------


void __fastcall TMobileForm::Button2Click(TObject *)
{
	TRegistry	*registry = new TRegistry;

	if( registry->OpenKey( registryKey, false )
	&&  registry->ValueExists( "ActivityPath" ) )
	{
		SaveDialog->InitialDir = registry->ReadString( "ActivityPath" );
	}
	registry->CloseKey();

	SaveDialog->FileName = "Activity.csv";
	if( SaveDialog->Execute() )
	{
		ActivityFileEdit->Text = SaveDialog->FileName;
		if( registry->OpenKey( registryKey, true ) )
		{
			registry->WriteString( "ActivityFile", SaveDialog->FileName );

			AnsiString	ActivityPath = SaveDialog->FileName;
			int			lastPos = ActivityPath.LastDelimiter( "\\" );

			if( lastPos )
				lastPos--;
			ActivityPath.SetLength( lastPos );

			registry->WriteString( "ActivityPath", ActivityPath );

			registry->CloseKey();
		}

		if( TasksComboBox->Items->Count && ActivityFileEdit->Text > "" )
			StartButton->Enabled = true;
		else
			StartButton->Enabled = false;
	}

	delete registry;
}
//---------------------------------------------------------------------------

void __fastcall TMobileForm::StartButtonClick(TObject *)
{
	if( currentTask != theTaskFile[TasksComboBox->ItemIndex].taskId )
	{
		unsigned short	day, month, year;
		unsigned short	hour, minute, second, mili;
		int				newFile;

		newFile = access( ActivityFileEdit->Text.c_str(), 0 );

		std::ofstream	csvFile( ActivityFileEdit->Text.c_str(), std::ios_base::app );
		TDateTime	now = Now();
		now.DecodeDate( &year, &month, &day );
		now.DecodeTime( &hour, &minute, &second, &mili );

		if( csvFile.is_open() )
		{
			if( newFile )
				csvFile << "ID,PROJECT,TITLE,START_TIME\n";
				
			csvFile <<
				theTaskFile[TasksComboBox->ItemIndex].taskId << ",\"" <<
				theTaskFile[TasksComboBox->ItemIndex].project << "\",\"" <<
				theTaskFile[TasksComboBox->ItemIndex].title << "\"," <<
				day << '.' << month << '.' << year << ' ' <<
				hour << ':' << minute << ':' << second << '\n';


			currentTask = theTaskFile[TasksComboBox->ItemIndex].taskId;
			StopButton->Enabled = true;
			ClockTimer->Enabled = true;
			startTime = clock();

			TRegistry *registry = new TRegistry;
			if( registry->OpenKey( registryKey, true ) )
			{
				registry->WriteInteger( "currentTask", currentTask );
				registry->CloseKey();
			}
			delete registry;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMobileForm::StopButtonClick( TObject *Sender )
{
	if( currentTask )
	{
		unsigned short	day, month, year;
		unsigned short	hour, minute, second, mili;

		std::ofstream	csvFile( ActivityFileEdit->Text.c_str(), std::ios_base::app );
		TDateTime	now = Now();
		now.DecodeDate( &year, &month, &day );
		now.DecodeTime( &hour, &minute, &second, &mili );

		if( csvFile.is_open() )
		{
			csvFile <<
				"0,\"-\",\"-\"," <<
				day << '.' << month << '.' << year << ' ' <<
				hour << ':' << minute << ':' << second << '\n';
		}
		StopButton->Enabled = false;
		ClockTimer->Enabled = false;
		currentTask = 0;
		if( Sender == StopButton )
		{
			TRegistry *registry = new TRegistry;
			if( registry->OpenKey( registryKey, true ) )
			{
				registry->DeleteValue( "currentTask" );
				registry->CloseKey();
			}
			delete registry;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMobileForm::FormClose(TObject *Sender,
	  TCloseAction &)
{
	StopButtonClick( Sender );	
}
//---------------------------------------------------------------------------

void __fastcall TMobileForm::ClockTimerTimer(TObject *)
{
	char	buffer[32];
	clock_t		numSeconds = clock() - startTime;
	numSeconds /= CLK_TCK;

	int		seconds = numSeconds % 60;
	numSeconds /= 60;
	int minutes = numSeconds % 60;
	int hour = numSeconds / 60;

	sprintf( buffer, "%02d:%02d:%02d", hour, minutes, seconds );
	ClockLabel->Caption = buffer;
}
//---------------------------------------------------------------------------
void __fastcall TMobileForm::AppMinimize(TObject *)
{
	IconData.cbSize = sizeof(IconData);
	IconData.hWnd = Handle;
	IconData.uID = 100;
	IconData.uFlags = NIF_MESSAGE + NIF_ICON + NIF_TIP;
	IconData.uCallbackMessage = WM_USER + 1;
	IconData.hIcon = Application->Icon->Handle;

	Shell_NotifyIcon(NIM_ADD, &IconData);

	ShowWindow( Application->Handle, SW_HIDE );
}
//---------------------------------------------------------------------------
void __fastcall TMobileForm::AppWindowProc(TMessage &msg)
{
	if( msg.Msg == WM_USER + 1 )
	{
		if( msg.LParam == WM_LBUTTONUP
		||  msg.LParam == WM_RBUTTONUP )
		{
			Shell_NotifyIcon(NIM_DELETE, &IconData);

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
		StopButtonClick( Application );

/***/	return;
	}

	WndProc( msg );
}

//---------------------------------------------------------------------------

void __fastcall TMobileForm::FormCreate(TObject *)
{
	Application->OnMinimize = AppMinimize;
	WindowProc = AppWindowProc;

	strcpy( IconData.szTip, Application->Title.c_str() );
}
//---------------------------------------------------------------------------

void __fastcall TMobileForm::FormDestroy(TObject *)
{
	Shell_NotifyIcon(NIM_DELETE, &IconData);
}
//---------------------------------------------------------------------------

