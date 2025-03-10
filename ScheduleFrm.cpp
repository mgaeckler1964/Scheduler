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
#include <fstream.h>
#include <stdlib.h>

#include <vcl.h>
#include <vcl/registry.hpp>
#pragma hdrstop

#include <gak/csv.h>

#include "ScheduleFrm.h"
#include "PermissionsFrm.h"
#include "MainFrm.h"
#include "EditDateDlg.h"
#include "AlarmDlg.h"

//---------------------------------------------------------------------------
using namespace gak;
using namespace vcl;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TScheduleForm *ScheduleForm;

void TScheduleForm::setUserId( long userId, const STRING &userName, int permissions )
{
	STRING newCaption = Caption.c_str();
	newCaption += ' ';
	newCaption += userName;
	Caption = (const char*)newCaption;

	this->userId = userId;

	datesQuery->Params->Items[0]->AsInteger = userId;
	datesQuery->Open();

	int tag = this->Tag;
	tag %= 10;
	tag += userId * 10;
	this->Tag = tag;

	readOnly = !(permissions & PERM_SCHEDULE_WRITE);

	BitBtnNew->Enabled = !readOnly;
	NeuerTermin->Enabled = !readOnly;

	BitBtnDelete->Enabled = !readOnly;
	TerminLschen->Enabled = !readOnly;

	BitBtnDeleteAll->Enabled = !readOnly;
	if( readOnly )
		BitBtnChange->Caption = "Anzeigen";

	Import1->Enabled = !readOnly;
	Import2->Enabled = !readOnly;
}
//---------------------------------------------------------------------------
int TScheduleForm::getLastExport( void )
{
	int	lastExport = 0;

	static	TQuery	*activeUserQuery = NULL;

	if( !activeUserQuery )
	{
		activeUserQuery = new TQuery( NULL );
		activeUserQuery->SQL->Add(
			"select	LastExport "
			"from	user_tab "
			"where	id = :theID"
		);
		activeUserQuery->DatabaseName = "SchedulerDB";

		activeUserQuery->Prepare();
	}
	activeUserQuery->Params->Items[0]->AsInteger = userId;
	activeUserQuery->Open();
	if( !activeUserQuery->Eof )
		lastExport	= activeUserQuery->Fields->Fields[0]->AsInteger;

	activeUserQuery->Close();

	return lastExport;
}
//---------------------------------------------------------------------------
void TScheduleForm::ReloadTable( int id )
{
	if( id )
	{
		datesQuery->Close();

		for( datesQuery->Open();
			id != datesQuery->FieldByName( "id" )->AsInteger && !datesQuery->Eof;
			datesQuery->Next()
		)
			;
	}
	else
	{
		TDateTime	endDate = datesQuery->FieldByName( "endDate" )->AsDateTime;
		datesQuery->Close();

		for( datesQuery->Open();
			endDate > datesQuery->FieldByName( "endDate" )->AsDateTime && !datesQuery->Eof;
			datesQuery->Next() )
			;
	}
}
//---------------------------------------------------------------------------
void TScheduleForm::importOutlook( void )
{
	ArrayOfStrings	fieldDef;
	FieldSet	  	record;
	STRING		  	Betreff, Kategorien,
					BeginntAm, BeginntUm,
					EndetAm, EndetUm,
					ErinnerungEinAus,
					ErinnerungAm, ErinnerungUm,
					Beschreibung, Ort;

	int			  	day, month, year,
					hour, minute, second;

	TDateTime	  	startDate, endDate, alarmDate;
	double		  	alarmBefore;

	TRegistry	  	*registry = new TRegistry;

	if( registry->OpenKey( REGISTRY_KEY, false )
	&&  registry->ValueExists( "OutlookPath" ) )
	{
		OpenDialog->InitialDir = registry->ReadString( "OutlookPath" );
	}
	registry->CloseKey();

	OpenDialog->FilterIndex = 2;
	if( OpenDialog->Execute() )
	{
		char	*dbFile = OpenDialog->FileName.c_str();

		if( registry->OpenKey( REGISTRY_KEY, true ) )
		{
			AnsiString	OutlookPath = OpenDialog->FileName;
			int			lastPos = OutlookPath.LastDelimiter( "\\" );

			if( lastPos )
				lastPos--;
			OutlookPath.SetLength( lastPos );

			registry->WriteString( "OutlookPath", OutlookPath );

			registry->CloseKey();
		}

		ifstream		csvFile( dbFile );

		readCSVLine( csvFile, &fieldDef );

		while( !csvFile.eof() && !csvFile.fail() )
		{
			readFIELD_SET( csvFile, fieldDef, &record );
			if( csvFile.eof() || csvFile.fail() )
				break;

			Betreff = record["Betreff"];
			Kategorien = record["Kategorien"];
			if( Kategorien != "Scheduler" )
			{
				BeginntAm = record["Beginnt am"];
				BeginntUm = record["Beginnt um"];
				EndetAm = record["Endet am"];
				EndetUm = record["Endet um"];
				ErinnerungEinAus = record["Erinnerung Ein/Aus"];
				ErinnerungAm = record["Erinnerung am"];
				ErinnerungUm = record["Erinnerung um"];
				Beschreibung = record["Beschreibung"];
				Ort = record["Ort"];

				sscanf( BeginntAm, "%d.%d.%d", &day, &month, &year );
				sscanf( BeginntUm, "%d:%d:%d", &hour, &minute, &second );
				startDate = EncodeDate( (Word)year, (Word)month, (Word)day ) +
							EncodeTime( (Word)hour, (Word)minute, (Word)second, 0 );

				sscanf( EndetAm, "%d.%d.%d", &day, &month, &year );
				sscanf( EndetUm, "%d:%d:%d", &hour, &minute, &second );
				endDate = EncodeDate( (Word)year, (Word)month, (Word)day ) +
							EncodeTime( (Word)hour, (Word)minute, (Word)second, 0 );

				if( !ErinnerungEinAus.compareI( "ein" ) )
				{
					sscanf( ErinnerungAm, "%d.%d.%d", &day, &month, &year );
					sscanf( ErinnerungUm, "%d:%d:%d", &hour, &minute, &second );
					alarmDate = EncodeDate( (Word)year, (Word)month, (Word)day ) +
								EncodeTime( (Word)hour, (Word)minute, (Word)second, 0 );
					alarmBefore = startDate-alarmDate;
					alarmBefore *= 24 * 60;
				}
				else
					alarmBefore = -1;

				insertDateSQL->ParamByName( "UserId" )->AsInteger = userId;
				insertDateSQL->ParamByName( "startDate" )->AsDateTime = startDate;
				insertDateSQL->ParamByName( "endDate" )->AsDateTime = endDate;
				insertDateSQL->ParamByName( "title" )->AsString = Betreff;
				insertDateSQL->ParamByName( "Description" )->AsString = Beschreibung;
				insertDateSQL->ParamByName( "Location" )->AsString = Ort;
				if( alarmBefore >= 0 )
				{
					insertDateSQL->ParamByName( "alarmDate" )->AsDateTime = alarmDate;
					insertDateSQL->ParamByName( "AlarmBefore" )->AsSmallInt = alarmBefore;
				}

				insertDateSQL->ExecSQL();
			}
		}

		csvFile.close();
	}

	delete registry;
}

//---------------------------------------------------------------------------
void TScheduleForm::exportOutlook( TQuery *datesSQL )
{
	long		maxId = getLastExport(), theId;
	AnsiString	startDate, startTime;
	AnsiString	endDate, endTime;
	AnsiString	alarmDate, alarmTime;
	STRING		title, location, description;
	bool		alarmFlag;

	TDateTime	theDate;

	TRegistry	*registry = new TRegistry;

	if( registry->OpenKey( REGISTRY_KEY, false )
	&&  registry->ValueExists( "OutlookPath" ) )
	{
		SaveDialog->InitialDir = registry->ReadString( "OutlookPath" );
	}
	registry->CloseKey();

	SaveDialog->FilterIndex = 2;
	SaveDialog->DefaultExt = "csv";
	if( SaveDialog->Execute() )
	{
		char *dbFile = SaveDialog->FileName.c_str();

		if( registry->OpenKey( REGISTRY_KEY, true ) )
		{
			AnsiString	OutlookPath = SaveDialog->FileName;
			int			lastPos = OutlookPath.LastDelimiter( "\\" );

			if( lastPos )
				lastPos--;
			OutlookPath.SetLength( lastPos );

			registry->WriteString( "OutlookPath", OutlookPath );
			registry->CloseKey();
		}

		gak::ofstream	csvFile( dbFile );
		if( csvFile.is_open() )
		{
			csvFile << "\"Betreff\","
						"\"Beginnt am\","
						"\"Beginnt um\","
						"\"Endet am\",\"Endet um\","
						"\"Erinnerung Ein/Aus\","
						"\"Erinnerung am\","
						"\"Erinnerung um\","
						"\"Ort\","
						"\"Kategorien\","
						"\"Beschreibung\"\n";

			datesSQL->First();
			while( !datesSQL->Eof )
			{
				theId = datesSQL->FieldByName( "ID" )->AsInteger;
				if( theId > maxId )
					maxId = theId;

				theDate = datesSQL->FieldByName( "StartDate" )->AsDateTime;
				startDate = theDate.DateString();
				startTime = theDate.TimeString();
/*
unsigned short hour, minute, sec, msec;
char buffer[128];

theDate.DecodeTime( &hour, &minute, &sec, &msec );
sprintf( buffer, "%u:%u:%u.%u", hour, minute, sec, msec );
startTime = buffer;
*/
				theDate = datesSQL->FieldByName( "EndDate" )->AsDateTime;
				endDate = theDate.DateString();
				endTime = theDate.TimeString();
/*
theDate.DecodeTime( &hour, &minute, &sec, &msec );
sprintf( buffer, "%u:%u:%u.%u", hour, minute, sec, msec );
endTime = buffer;
*/
				alarmFlag = !datesSQL->FieldByName( "AlarmDate" )->IsNull;
				if( alarmFlag )
				{
					theDate = datesSQL->FieldByName( "AlarmDate" )->AsDateTime;
					alarmDate = theDate.DateString();
					alarmTime = theDate.TimeString();
				}

				title = datesSQL->FieldByName( "Title" )->AsString.c_str();

				location = datesSQL->FieldByName( "location" )->AsString.c_str();
				description = datesSQL->FieldByName( "description" )->AsString.c_str();

				csvFile << '"' << title << "\",\""
						<< startDate.c_str() << "\",\""
						<< startTime.c_str() << "\",\""
						<< endDate.c_str() << "\",\""
						<< endTime.c_str() << "\",\""
						<< (alarmFlag ? "Ein" : "Aus") << "\",\""
						<< (alarmFlag ? alarmDate.c_str() : "") << "\",\""
						<< (alarmFlag ? alarmTime.c_str() : "") << "\",\""
						<< location << "\",\""
						<< "Scheduler" << "\",\""
						<< description << "\"\n";

				datesSQL->Next();
			}

			updateUserSQL->Params->Items[0]->AsInteger = maxId;
			updateUserSQL->Params->Items[1]->AsInteger = userId;
			updateUserSQL->ExecSQL();

			csvFile.close();
		}
	}

	delete registry;
}
//---------------------------------------------------------------------------
void TScheduleForm::importCasio( void )
{
	TRegistry	*registry = new TRegistry;

	if( registry->OpenKey( REGISTRY_KEY, false )
	&&  registry->ValueExists( "CasioPath" ) )
	{
		OpenDialog->InitialDir = registry->ReadString( "CasioPath" );
	}
	registry->CloseKey();

	OpenDialog->FilterIndex = 1;
	if( OpenDialog->Execute() )
	{
		char			*dbFile = OpenDialog->FileName.c_str();

		TDateTime		datum;
		int				anfang, ende, alarm;
		AnsiString		text;

		TDateTime		startDate, endDate, alarmDate;
		int				alarmBefore;

		if( registry->OpenKey( REGISTRY_KEY, true ) )
		{
			AnsiString	CasioPath = OpenDialog->FileName;
			int			lastPos = CasioPath.LastDelimiter( "\\" );

			if( lastPos )
				lastPos--;
			CasioPath.SetLength( lastPos );

			registry->WriteString( "CasioPath", CasioPath );
			registry->CloseKey();
		}

		dbaseTable->TableName = dbFile;
		dbaseTable->Open();

		dbaseTable->First();
		while( !dbaseTable->Eof )
		{
			text = dbaseTable->FieldByName( "TEXT" )->AsString;
			if( text[1] != '-' || text[2] != '>' )
			{
				datum = dbaseTable->FieldByName( "DATUM" )->AsDateTime;
				anfang = dbaseTable->FieldByName( "ANFANG" )->AsInteger;
				ende = dbaseTable->FieldByName( "ENDE" )->AsInteger;
				alarm = dbaseTable->FieldByName( "ALARM" )->AsInteger;

				startDate = datum + EncodeTime( (Word)(anfang/100), (Word)(anfang%100), 0, 0 );
				if( ende >= 0 )
					endDate = datum + EncodeTime( (Word)(ende/100), (Word)(ende%100), 0, 0 );
				else
					endDate = startDate;
				if( alarm >= 0 )
				{
					alarmDate = datum + EncodeTime( (Word)(alarm/100), (Word)(alarm%100), 0, 0 );
					alarmBefore = (anfang/100)-(alarm/100);
					alarmBefore *= 60;
					alarmBefore += (anfang%100)-(alarm%100);
				}

				insertDateSQL->ParamByName( "UserId" )->AsInteger = userId;
				insertDateSQL->ParamByName( "startDate" )->AsDateTime = startDate;
				insertDateSQL->ParamByName( "endDate" )->AsDateTime = endDate;
				insertDateSQL->ParamByName( "title" )->AsString = text;
				insertDateSQL->ParamByName( "Description" )->AsString = "";
				if( alarm >= 0 )
				{
					insertDateSQL->ParamByName( "alarmDate" )->AsDateTime = alarmDate;
					insertDateSQL->ParamByName( "AlarmBefore" )->AsSmallInt = alarmBefore;
				}

				insertDateSQL->ExecSQL();
			}
			dbaseTable->Next();
		}

		dbaseTable->Close();
	}

	delete registry;
}

//---------------------------------------------------------------------------
void TScheduleForm::exportCasio( TQuery *datesSQL )
{
	long		maxId = getLastExport(), theId;

	TRegistry	*registry = new TRegistry;

	if( registry->OpenKey( REGISTRY_KEY, false )
	&&  registry->ValueExists( "CasioPath" ) )
	{
		SaveDialog->InitialDir = registry->ReadString( "CasioPath" );
	}
	registry->CloseKey();

	SaveDialog->FilterIndex = 1;
	SaveDialog->DefaultExt = "db";
	if( SaveDialog->Execute() )
	{
		char			*dbFile = SaveDialog->FileName.c_str();
		TDateTime		theDate;
		unsigned short	hour, minute, dummy;

		if( registry->OpenKey( REGISTRY_KEY, true ) )
		{
			AnsiString	CasioPath = SaveDialog->FileName;
			int			lastPos = CasioPath.LastDelimiter( "\\" );

			if( lastPos )
				lastPos--;
			CasioPath.SetLength( lastPos );

			registry->WriteString( "CasioPath", CasioPath );
			registry->CloseKey();
		}

		dbaseTable->TableName = dbFile;
		dbaseTable->CreateTable();
		dbaseTable->Open();

		datesSQL->First();
		while( !datesSQL->Eof )
		{
			dbaseTable->Insert();

			theId = datesSQL->FieldByName( "ID" )->AsInteger;
			if( theId > maxId )
				maxId = theId;

			theDate = datesSQL->FieldByName( "StartDate" )->AsDateTime;
			dbaseTableDATUM->AsDateTime = theDate;
			theDate.DecodeTime( &hour, &minute, &dummy, &dummy );
			dbaseTableANFANG->AsInteger = hour * 100 + minute;

			theDate = datesSQL->FieldByName( "EndDate" )->AsDateTime;
			theDate.DecodeTime( &hour, &minute, &dummy, &dummy );
			dbaseTableENDE->AsInteger = hour * 100 + minute;

			theDate = datesSQL->FieldByName( "AlarmDate" )->AsDateTime;
			theDate.DecodeTime( &hour, &minute, &dummy, &dummy );
			dbaseTableALARM->AsInteger = hour * 100 + minute;

			dbaseTableTEXT->AsString =
				"->" + datesSQL->FieldByName( "Title" )->AsString + "<-";

			dbaseTable->Post();

			datesSQL->Next();
		}

		dbaseTable->Close();

		updateUserSQL->Params->Items[0]->AsInteger = maxId;
		updateUserSQL->Params->Items[1]->AsInteger = userId;
		updateUserSQL->ExecSQL();
	}

	delete registry;
}

//---------------------------------------------------------------------------
__fastcall TScheduleForm::TScheduleForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall TScheduleForm::NeuerTerminClick(TObject *)
{
	if( readOnly )
/*@*/	return;

	EditDateDialog->setNewDate();
	if( EditDateDialog->ShowModal(userId) == mrOk )
		ReloadTable( EditDateDialog->getNewDate() );
}
//---------------------------------------------------------------------------

void __fastcall TScheduleForm::TerminBearbeitenClick(TObject *)
{
	int theID = datesQuery->FieldByName( "id" )->AsInteger;

	if( theID )
	{
		EditDateDialog->setActualDate( theID, readOnly );
		if( EditDateDialog->ShowModal(userId) == mrOk )
			ReloadTable( theID );
	}
}
//---------------------------------------------------------------------------


void __fastcall TScheduleForm::TerminLschenClick(TObject *)
{
	if( readOnly )
/*@*/	return;

	TQuery	*delSQL;

	if( Application->MessageBox(
						"Wollen Sie den Termin löschen?",
						"Achtung",
						MB_YESNO|MB_ICONQUESTION ) == IDYES )
	{
		delSQL = new TQuery( Application );
		if( delSQL )
		{
			delSQL->DatabaseName = "SchedulerDB";
			delSQL->SQL->Add( "delete from schedule where id = :theId" );
			delSQL->Params->Items[0]->AsInteger = datesQuery->FieldByName( "id" )->AsInteger;

			delSQL->ExecSQL();

			delete delSQL;

			ReloadTable();
		}
	}
}

//---------------------------------------------------------------------------


void __fastcall TScheduleForm::ScheduleGridDrawColumnCell(TObject *,
	  const TRect &Rect, int DataCol, TColumn *Column,
	  TGridDrawState State)
{
	TGridDrawState	empty;

	if( State == empty )
	{
		if( datesQuery->FieldByName("endDate")->AsDateTime < Now() )
			ScheduleGrid->Canvas->Brush->Color = clRed;
		else if( datesQuery->FieldByName("startDate")->AsDateTime < Now() )
			ScheduleGrid->Canvas->Brush->Color = clGreen;
	}
	ScheduleGrid->DefaultDrawColumnCell( Rect, DataCol, Column, State);
}
//---------------------------------------------------------------------------


void __fastcall TScheduleForm::ExportAllClick(TObject *)
{
	void	*bookmark;

	datesQuery->DisableControls();
	bookmark = datesQuery->GetBookmark();

	exportCasio( datesQuery );

	datesQuery->GotoBookmark( bookmark );
	datesQuery->FreeBookmark( bookmark );
	datesQuery->EnableControls();
}
//---------------------------------------------------------------------------

void __fastcall TScheduleForm::ExportNewClick(TObject *)
{
	newQuery->Params->Items[0]->AsInteger = userId;
	newQuery->Params->Items[1]->AsInteger = getLastExport();
	newQuery->Open();

	exportCasio( newQuery );

	newQuery->Close();
}
//---------------------------------------------------------------------------


void __fastcall TScheduleForm::Import1Click(TObject *)
{
	if( readOnly )
/*@*/	return;

	void	*bookmark;

	datesQuery->DisableControls();
	bookmark = datesQuery->GetBookmark();

	importCasio();

	datesQuery->Close();
	datesQuery->Open();

	datesQuery->GotoBookmark( bookmark );
	datesQuery->FreeBookmark( bookmark );
	datesQuery->EnableControls();
}
//---------------------------------------------------------------------------

void __fastcall TScheduleForm::ButtonAllClick(TObject *)
{
	datesQuery->Close();
	datesQuery->SQL->Clear();
	datesQuery->SQL->Add(	"select schedule.*, project.name as ProjectName "
							"from schedule "
							"left OUTER JOIN Project "
							"on (Project.ID = Schedule.Project) "
							"where Schedule.userId = :theUser "
							"order by Schedule.enddate, Schedule.startdate, project.name, Schedule.title" );
	datesQuery->Params->Items[0]->AsInteger = userId;
	datesQuery->Open();
}
//---------------------------------------------------------------------------

void __fastcall TScheduleForm::ButtonDatesClick(TObject *)
{
	datesQuery->Close();
	datesQuery->SQL->Clear();
	datesQuery->SQL->Add(	"select schedule.*, project.name as ProjectName "
							"from schedule "
							"left OUTER JOIN Project "
							"on (Project.ID = Schedule.Project) "
							"where ((Schedule.recurringid is not null or Schedule.recurringid > 0) or "
									"(Schedule.estEffort is null or Schedule.estEffort = 0)) and "
									"Schedule.userId = :theUser "
							"order by Schedule.enddate, Schedule.startdate, project.name, Schedule.title" );
	datesQuery->Params->Items[0]->AsInteger = userId;
	datesQuery->Open();
}
//---------------------------------------------------------------------------

void __fastcall TScheduleForm::ButtonTasksClick(TObject *)
{
	datesQuery->Close();
	datesQuery->SQL->Clear();
	datesQuery->SQL->Add(	"select schedule.*, project.name as ProjectName "
							"from schedule "
							"left OUTER JOIN Project "
							"on (Project.ID = Schedule.Project) "
							"where (Schedule.recurringid is null or Schedule.recurringid <= 0) and "
									"(Schedule.estEffort is not null and Schedule.estEffort > 0) and "
									"Schedule.userId = :theUser "
							"order by Schedule.enddate, Schedule.startdate, project.name, Schedule.title" );
	datesQuery->Params->Items[0]->AsInteger = userId;
	datesQuery->Open();
}
//---------------------------------------------------------------------------

void __fastcall TScheduleForm::BitBtnDeleteAllClick(TObject *)
{
	if( readOnly )
/*@*/	return;

	TQuery	*delSQL;

	if( Application->MessageBox(
						"Wollen Sie alle alte Termine löschen?",
						"Achtung",
						MB_YESNO|MB_ICONQUESTION ) == IDYES )
	{
		delSQL = new TQuery( Application );
		if( delSQL )
		{
			delSQL->DatabaseName = "SchedulerDB";
			delSQL->SQL->Add( "delete from schedule s where s.userId = :theUser and s.EndDate < :today" );
			delSQL->SQL->Add( "and not exists ("
									"select * from activities a where a.task = s.id"
								")" );
			delSQL->Params->Items[0]->AsInteger = userId;
			delSQL->Params->Items[1]->AsDateTime = Sysutils::Date();

			delSQL->ExecSQL();

			int			changed = delSQL->RowsAffected;
			AnsiString	changedStr = IntToStr( changed ) + " Zeile(n) gelöscht.";
			Application->MessageBox(
						changedStr.c_str(),
						"Scheduler",
						MB_ICONINFORMATION );

			delete delSQL;

			ReloadTable();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TScheduleForm::ExportAlle1Click(TObject *)
{
	void	*bookmark;

	datesQuery->DisableControls();
	bookmark = datesQuery->GetBookmark();

	exportOutlook( datesQuery );

	datesQuery->GotoBookmark( bookmark );
	datesQuery->FreeBookmark( bookmark );
	datesQuery->EnableControls();
}
//---------------------------------------------------------------------------
void __fastcall TScheduleForm::ExportNeue1Click(TObject *)
{
	newQuery->Params->Items[0]->AsInteger = userId;
	newQuery->Params->Items[1]->AsInteger = getLastExport();
	newQuery->Open();

	exportOutlook( newQuery );

	newQuery->Close();
}
//---------------------------------------------------------------------------
void __fastcall TScheduleForm::Import2Click(TObject *)
{
	if( readOnly )
/*@*/	return;

	void	*bookmark;

	datesQuery->DisableControls();
	bookmark = datesQuery->GetBookmark();

	importOutlook();

	datesQuery->Close();
	datesQuery->Open();

	datesQuery->GotoBookmark( bookmark );
	datesQuery->FreeBookmark( bookmark );
	datesQuery->EnableControls();
}
//---------------------------------------------------------------------------



void __fastcall TScheduleForm::theTimerTimer(TObject *)
{
	TDateTime		now = Now();
	TDateTime		startDate,
					alarmDate;
	double			timeLeft;
	AnsiString		tmp,
					description,
					title;
	unsigned short	hour,
					min,
					sec,
					msec,
					newTimer;
	int				redoTime, alarmButton;
	bool			restartFlag = false;

	theTimer->Enabled = false;
	theTimer->Interval = 0;

	activeDatesQuery->Params->Items[0]->AsInteger = userId;
	activeDatesQuery->Params->Items[1]->AsDateTime = now;

	activeDatesQuery->Open();
	while( !activeDatesQuery->Eof )
	{
		alarmButton = mrOk;

		activeDatesQuery->Edit();

		if( !activeDatesQuery->FieldByName( "alarmDate" )->IsNull )
		{
			title        = activeDatesQuery->FieldByName( "title" )->AsString;
			startDate    = activeDatesQuery->FieldByName( "startDate" )->AsDateTime;
			alarmDate    = activeDatesQuery->FieldByName( "alarmDate" )->AsDateTime;
			description  = startDate;
			description += ":\r\n";
			description += activeDatesQuery->FieldByName( "description" )->AsString;

			ShowWindow( Application->Handle, SW_SHOW );
			Application->Restore();
			Activate();
			SetFocus();
			Show();

			AlarmDialog->RedoEdit->Text = "1";

			timeLeft = (startDate - now);
			if( timeLeft > 7.0 )		// more than one week
			{
				AlarmDialog->RedoUnitCombo->ItemIndex = 3;
				AlarmDialog->RedoEdit->Text = "1";
			}
			else if( timeLeft > 1.0 )	// more than one day
			{
				AlarmDialog->RedoUnitCombo->ItemIndex = 2;
				AlarmDialog->RedoEdit->Text = "1";
			}
			else if( timeLeft > (1.0/24.0) )	// more that one hour
			{
				AlarmDialog->RedoUnitCombo->ItemIndex = 1;
				AlarmDialog->RedoEdit->Text = "1";
			}
			else if( timeLeft > (45.0/(60.0*24.0)) )	// more that 45 minutes
			{
				AlarmDialog->RedoUnitCombo->ItemIndex = 0;
				AlarmDialog->RedoEdit->Text = "45";
			}
			else if( timeLeft > (30.0/(60.0*24.0)) )	// more that 30 minutes
			{
				AlarmDialog->RedoUnitCombo->ItemIndex = 0;
				AlarmDialog->RedoEdit->Text = "30";
			}
			else if( timeLeft > (15.0/(60.0*24.0)) )	// more that 15 minutes
			{
				AlarmDialog->RedoUnitCombo->ItemIndex = 0;
				AlarmDialog->RedoEdit->Text = "15";
			}
			else if( timeLeft > (10.0/(60.0*24.0)) )	// more that 10 minutes
			{
				AlarmDialog->RedoUnitCombo->ItemIndex = 0;
				AlarmDialog->RedoEdit->Text = "10";
			}
			else if( timeLeft > (5.0/(60.0*24.0)) )		// more that 5 minutes
			{
				AlarmDialog->RedoUnitCombo->ItemIndex = 0;
				AlarmDialog->RedoEdit->Text = "5";
			}
			else if( timeLeft > 0 )
			{
				AlarmDialog->RedoUnitCombo->ItemIndex = 0;
				AlarmDialog->RedoEdit->Text = "1";
			}
			else
			{
				TRegistry	*reg = new TRegistry;

				if( reg->OpenKey( REGISTRY_KEY, false ) )
				{
					if( reg->ValueExists( "Redo" ) )
						AlarmDialog->RedoEdit->Text = reg->ReadString( "Redo" );
					else
						AlarmDialog->RedoEdit->Text = "";

					if( reg->ValueExists( "RedoUnit" ) )
						AlarmDialog->RedoUnitCombo->ItemIndex = reg->ReadInteger( "RedoUnit" );
					else
						AlarmDialog->RedoUnitCombo->ItemIndex = 0;

					reg->CloseKey();
				}

				delete reg;

				title.Insert( "Überfällig: ", 1 );
				description.Insert( "Überfällig: ", 1 );
			}

			alarmButton = AlarmDialog->AlarmShow( title.c_str(), description.c_str() );

			redoTime = atol( AlarmDialog->RedoEdit->Text.c_str() );
			if( alarmButton == mrOk && redoTime > 0 )
			{

				TRegistry	*reg = new TRegistry;

				if( reg->OpenKey( REGISTRY_KEY, true ) )
				{
					reg->WriteString( "Redo", AlarmDialog->RedoEdit->Text );
					reg->WriteInteger("RedoUnit", AlarmDialog->RedoUnitCombo->ItemIndex);

					reg->CloseKey();
				}

				delete reg;



				now = Now();
				if( AlarmDialog->RedoUnitCombo->ItemIndex == 0 ) // Minutes
					now += EncodeTime( 0, (Word)redoTime, 0, 0 );
				else if( AlarmDialog->RedoUnitCombo->ItemIndex == 1 ) // hours
					now += EncodeTime( (Word)redoTime, 0, 0, 0 );
				else if( AlarmDialog->RedoUnitCombo->ItemIndex == 2 ) // days
					now += redoTime;
				else if( AlarmDialog->RedoUnitCombo->ItemIndex == 3 ) // weeks
					now += redoTime*7;

				activeDatesQuery->FieldByName( "AlarmDate" )->AsDateTime = now;
			}
			else
			{
				activeDatesQuery->FieldByName( "AlarmDate" )->Clear();
				activeDatesQuery->FieldByName( "AlarmBefore" )->Clear();
			}
			restartFlag = true;
		}
		if( !activeDatesQuery->FieldByName( "Command" )->IsNull )
		{
			if( alarmButton == mrOk )
			{
				WinExec(
					activeDatesQuery->FieldByName(
						"Command"
					)->AsString.c_str(),
					SW_SHOW
				);
			}
			activeDatesQuery->FieldByName( "Command" )->Clear();
		}

		activeDatesQuery->Post();
		activeDatesQuery->Next();
	}
	activeDatesQuery->Close();

	if( restartFlag )
		ReloadTable();

	now = Now();
	now.DecodeTime( &hour, &min, &sec, &msec );

	newTimer = (unsigned short)(60000U - (sec*1000U) - msec);

	theTimer->Interval = newTimer;
	theTimer->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TScheduleForm::FormClose(TObject *,
      TCloseAction &Action)
{
	datesQuery->Close();
	Action = caFree;
}
//---------------------------------------------------------------------------


