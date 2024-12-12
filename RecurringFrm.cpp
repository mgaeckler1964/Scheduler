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
#include <math.h>
#include <vcl.h>
#pragma hdrstop

#include <gak/date.h>
#include <gak/vcl_tools.h>

#include "RecurringFrm.h"
#include "MainFrm.h"
#include "ScheduleFrm.h"

//---------------------------------------------------------------------------
using namespace gak;
using namespace vcl;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRecurringForm *RecurringForm;
//---------------------------------------------------------------------------
static void nextMonth( unsigned short *year, unsigned short *month )
{
	(*month)++;
	if( *month > 12 )
	{
		*month = 1;
		(*year)++;
	}
}
//---------------------------------------------------------------------------
static TDateTime nextMonth( TDateTime date )
{
	unsigned short	year, month, day;

	date.DecodeDate( &year, &month, &day );
	nextMonth( &year, &month );

	bool ok = false;
	while( !ok )
	{
		try
		{
			date = EncodeDate( year, month, day );
			ok = true;
		}
		catch( ... )
		{
			if( day > 27 )
				day--;
			else
				throw;
		}
	}

	return date;
}
//---------------------------------------------------------------------------
static TDateTime nextYear( TDateTime date )
{
	unsigned short	year, month, day;

	date.DecodeDate( &year, &month, &day );
	year++;
	bool ok = false;
	while( !ok )
	{
		try
		{
			date = EncodeDate( year, month, day );
			ok = true;
		}
		catch( ... )
		{
			if( day > 27 )
				day--;
			else
				throw;
		}
	}

	return date;
}
//---------------------------------------------------------------------------
static TDateTime findDayOfMonth( TDateTime date, int dayOfWeek )
{
	unsigned short	year, month, day;
	int				curDow;

	date.DecodeDate( &year, &month, &day );

	date = EncodeDate( year, month, 1 );

	curDow = date.DayOfWeek();
	if( curDow <= dayOfWeek )
		date += dayOfWeek - curDow;
	else
		date += 7 - (curDow-dayOfWeek);

	return date;
}
//---------------------------------------------------------------------------
static TDateTime findDayOfMonth( TDateTime date, int counter, int dayOfWeek )
{
	if( counter > 0 )
	{
		date = findDayOfMonth( date, dayOfWeek );
		if( counter > 1 )
			date += 7*(counter-1);
	}
	else
	{
		date = findDayOfMonth( nextMonth( date ), dayOfWeek );
		date += 7*counter;
	}
	return date;
}
//---------------------------------------------------------------------------
__fastcall TRecurringForm::TRecurringForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TRecurringForm::getRecurringSchedules( void )
{
	TQuery			*repResQuery;
	TQuery			*reservatQuery;
	TDateTime		today = TDateTime::CurrentDate();
	TDateTime		maxDate = today + 366;
	TDateTime		tmpDate, lastDate;
	WORD			AlarmBefore;
	TDateTime		AlarmDate;
	TDateTime		tmp;
	int				i, rc;
	int				repeatMode, WeekDay, MonthDay, Month, counter, dow;
	unsigned short	theDay, theMonth, theYear;

	repResQuery = new TQuery( Application );
	if( repResQuery )
	{
		reservatQuery = new TQuery( Application );
		if( reservatQuery )
		{
			reservatQuery->DatabaseName = "SchedulerDB";
			repResQuery->DatabaseName = "SchedulerDB";

			reservatQuery->RequestLive = true;
			repResQuery->RequestLive = true;

			reservatQuery->SQL->Add( "select * from SCHEDULE "
										"where USERID = :theUser");
			reservatQuery->Params->Items[0]->AsInteger = userId;

			repResQuery->SQL->Add( "select * from RECURRING "
									"where (LETZTEREINTRAG is null or "
									"LETZTEREINTRAG < :maxDate) and "
									"USERID = :theUser" );
			repResQuery->Params->Items[0]->AsDate = maxDate;
			repResQuery->Params->Items[1]->AsInteger = userId;

			//reservatQuery->UniDirectional = true;
			//repResQuery->UniDirectional = true;

			reservatQuery->Open();
			repResQuery->Open();
			rc = repResQuery->RecordCount;
			for( i=0; i<rc; i++ )
			//while( !repResQuery->Eof )		// did not work ?
			{
				if( repResQuery->FieldByName( "LetzterEintrag" )->IsNull )
					lastDate = TDateTime::CurrentDate();
				else
					lastDate = repResQuery->FieldByName( "LetzterEintrag" )->AsDateTime;

				repeatMode	= repResQuery->FieldByName( "Wiederholung" )->AsInteger;
				WeekDay		= repResQuery->FieldByName( "WochenTag" )->AsInteger;
				MonthDay	= repResQuery->FieldByName( "MonatsTag" )->AsInteger;
				Month		= repResQuery->FieldByName( "Monat" )->AsInteger;
				counter		= repResQuery->FieldByName( "Zaehler" )->AsInteger;

				while( lastDate < maxDate )
				{
					// calculate the next ocurence of the reservation
					switch( repeatMode )
					{
						case 0:		// dayly
						{
							lastDate += 1;
							break;
						}
						case 1:		// weekly
						{
							// find next occurence of week day required

							dow = lastDate.DayOfWeek();
							if( dow < WeekDay )
								lastDate += WeekDay - dow;
							else
								lastDate += 7 - (dow-WeekDay);
							break;
						}
						case 2:		// two weekly
						{
							unsigned weekOfYear;

							// find next occurence of week day required
							dow = lastDate.DayOfWeek();
							if( dow < WeekDay )
								lastDate += WeekDay - dow;
							else
								lastDate += 7 - (dow-WeekDay);

							// convert to my own date type
							lastDate.DecodeDate( &theYear, &theMonth, &theDay );
							gak::Date	myDate( theDay, gak::Date::Month(theMonth), theYear );

							// check the week
							weekOfYear = myDate.weekOfYear();

							if( (!counter &&   weekOfYear % 2 )		// even weeks
							||  ( counter && !(weekOfYear % 2)) )	// odd weeks
							{
								lastDate += 7;		// next week
							}
							break;


						}
						case 3:		// monthly
						{
							if( !counter )
							{
								lastDate.DecodeDate( &theYear, &theMonth, &theDay );
								if( theDay >= MonthDay )
								{
									nextMonth( &theYear, &theMonth );
								}
								lastDate = EncodeDate( theYear, theMonth, (unsigned short)MonthDay );
							}
							else
							{
								tmpDate = findDayOfMonth( lastDate, counter, WeekDay );
								while( tmpDate <= lastDate || tmpDate <= today )
									tmpDate = findDayOfMonth( nextMonth( tmpDate ), counter, WeekDay );
								lastDate = tmpDate;
							}
							break;
						}
						case 4:		// jährlich
						{
							lastDate.DecodeDate( &theYear, &theMonth, &theDay );
							if( !counter )
							{
								tmpDate = EncodeDate( theYear, (unsigned short)Month, (unsigned short)MonthDay );
								if( tmpDate <= lastDate )
								{
									theYear++;
									lastDate = EncodeDate( theYear, (unsigned short)Month, (unsigned short)MonthDay );
								}
								else
									lastDate = tmpDate;
							}
							else
							{
								tmpDate = findDayOfMonth( EncodeDate( theYear, (unsigned short)Month, 1 ), counter, WeekDay );
								while( tmpDate <= lastDate || tmpDate <= today )
									tmpDate = findDayOfMonth( nextYear( tmpDate ), counter, WeekDay );
								lastDate = tmpDate;
							}
							break;
						}
					}
					if( lastDate > today )
					{
						reservatQuery->Insert();
						reservatQuery->FieldByName( "ID" )->AsInteger =
							getNewMaxValue(
								"SchedulerDB", "SCHEDULE", "ID"
							)
						;
						reservatQuery->FieldByName( "UserId" )->AsInteger = userId;
						reservatQuery->FieldByName( "StartDate" )->AsDateTime = lastDate + repResQuery->FieldByName( "UhrzeitAnfang" )->AsDateTime;
						reservatQuery->FieldByName( "EndDate" )->AsDateTime = reservatQuery->FieldByName( "StartDate" )->AsDateTime + repResQuery->FieldByName( "DauerTage" )->AsInteger + repResQuery->FieldByName( "DauerStunden" )->AsDateTime;
						reservatQuery->FieldByName( "Title" )->AsString = repResQuery->FieldByName( "Title" )->AsString;
						reservatQuery->FieldByName( "Location" )->AsString = repResQuery->FieldByName( "Location" )->AsString;
						reservatQuery->FieldByName( "Description" )->AsString = repResQuery->FieldByName( "Description" )->AsString;
						reservatQuery->FieldByName( "Command" )->AsString = repResQuery->FieldByName( "Command" )->AsString;
						reservatQuery->FieldByName( "AlarmUnit" )->AsString = repResQuery->FieldByName( "AlarmUnit" )->AsString;
						reservatQuery->FieldByName( "AlarmBefore" )->AsInteger = repResQuery->FieldByName( "AlarmBefore" )->AsInteger;
						reservatQuery->FieldByName( "RecurringId" )->AsInteger = repResQuery->FieldByName( "ID")->AsInteger;

						AlarmBefore = (WORD)repResQuery->FieldByName( "AlarmBefore" )->AsInteger;
						if( AlarmBefore > 0 )
						{
							AlarmDate = reservatQuery->FieldByName( "StartDate" )->AsDateTime;
							if( reservatQuery->FieldByName( "AlarmUnit" )->AsString == "Minuten" )
							{
								tmp = EncodeTime( 0, AlarmBefore, 0, 0 );
								AlarmDate -= tmp;
							}
							else if( reservatQuery->FieldByName( "AlarmUnit" )->AsString == "Stunden" )
							{
								tmp = EncodeTime( AlarmBefore, 0, 0, 0 );
								AlarmDate -= tmp;
							}
							else if( reservatQuery->FieldByName( "AlarmUnit" )->AsString == "Tage" )
							{
								AlarmDate -= AlarmBefore;
							}
							else if( reservatQuery->FieldByName( "AlarmUnit" )->AsString == "Wochen" )
							{
									AlarmDate -= 7*AlarmBefore;
							}
							reservatQuery->FieldByName( "AlarmDate" )->AsDateTime = AlarmDate;
						}
						else
						{
							reservatQuery->FieldByName( "AlarmBefore" )->Clear();
							reservatQuery->FieldByName( "AlarmDate" )->Clear();
						}

						reservatQuery->Post();
					}
				}

				repResQuery->Edit();
				repResQuery->FieldByName( "LetzterEintrag" )->AsDateTime = lastDate;
				repResQuery->Post();

				repResQuery->Next();
			}

			reservatQuery->Close();
			repResQuery->Close();

			delete reservatQuery;
		}
		delete repResQuery;
	}
}
//---------------------------------------------------------------------------
void TRecurringForm::removeSchedules( void )
{
	TQuery	*delSql;

	delSql = new TQuery( Application );
	if( delSql )
	{
		delSql->DatabaseName = "SchedulerDB";
		delSql->SQL->Add( "delete from schedule where recurringId = :theId" );
		delSql->Params->Items[0]->AsInteger = RecurringQuery->FieldByName( "ID" )->AsInteger;
		delSql->ExecSQL();

		delete delSql;
	}
}
//---------------------------------------------------------------------------
void TRecurringForm::enableDisableControls( void )
{
	if( RepeatComboBox->ItemIndex == 0 )	// täglich
	{
		DayLabel->Visible = false;
		CounterComboBox->Visible = false;
		WeekComboBox->Visible = false;
		WeekDayComboBox->Visible = false;

		MonthLabel->Visible = false;
		MonthDayEdit->Visible = false;
		DayMonthLabel->Visible = false;
		MonthComboBox->Visible = false;
	}
	else if( RepeatComboBox->ItemIndex == 1 )	// Wöchentlich
	{
		DayLabel->Visible = true;
		CounterComboBox->Visible = false;
		WeekComboBox->Visible = false;
		WeekDayComboBox->Left = RepeatComboBox->Left;
		WeekDayComboBox->Visible = true;

		MonthLabel->Visible = false;
		MonthDayEdit->Visible = false;
		DayMonthLabel->Visible = false;
		MonthComboBox->Visible = false;
	}
	else if( RepeatComboBox->ItemIndex == 2 )	// Zwei Wochen
	{
		CounterComboBox->Visible = false;
		WeekComboBox->Visible = true;
		WeekDayComboBox->Left = MonthComboBox->Left;
		WeekDayComboBox->Visible = true;

		MonthLabel->Visible = false;
		MonthDayEdit->Visible = false;
		DayMonthLabel->Visible = false;
		MonthComboBox->Visible = false;
	}
	else if( RepeatComboBox->ItemIndex == 3 )	// Monatlich
	{
		DayLabel->Visible = true;
		CounterComboBox->Visible = true;
		WeekComboBox->Visible = false;
		WeekDayComboBox->Left = MonthComboBox->Left;
		WeekDayComboBox->Visible = true;

		MonthLabel->Visible = true;
		MonthDayEdit->Visible = true;
		DayMonthLabel->Caption = "eines Monats.";
		DayMonthLabel->Visible = true;
		MonthComboBox->Visible = false;
	}
	else if( RepeatComboBox->ItemIndex == 4 )	// Jährlich
	{
		DayLabel->Visible = true;
		CounterComboBox->Visible = true;
		WeekComboBox->Visible = false;
		WeekDayComboBox->Left = MonthComboBox->Left;
		WeekDayComboBox->Visible = true;

		MonthLabel->Visible = true;
		MonthDayEdit->Visible = true;
		DayMonthLabel->Caption = "im Monat";
		DayMonthLabel->Visible = true;
		MonthComboBox->Visible = true;
	}
	if( CounterComboBox->Visible )
	{
		if( !CounterComboBox->ItemIndex )
		{
			WeekDayComboBox->Visible = false;
			MonthDayEdit->Visible = true;
		}
		else
		{
			WeekDayComboBox->Visible = true;
			MonthDayEdit->Visible = false;
		}
	}
}
//---------------------------------------------------------------------------
void TRecurringForm::transfer2Controls( void )
{
	int			counter;
	TTime		selTime;

	if( RecurringQuery->RecordCount )
	{
		RepeatComboBox->ItemIndex = RecurringQuery->FieldByName( "Wiederholung" )->AsInteger;
		counter = RecurringQuery->FieldByName( "Zaehler" )->AsInteger;
		if( counter < 0 )
			counter = 4 - counter;

		CounterComboBox->ItemIndex = counter;
		WeekComboBox->ItemIndex = counter;
		WeekDayComboBox->ItemIndex = RecurringQuery->FieldByName( "Wochentag" )->AsInteger-1;
		MonthComboBox->ItemIndex = RecurringQuery->FieldByName( "Monat" )->AsInteger -1;

		selTime = RecurringQuery->FieldByName( "UhrzeitAnfang" )->AsDateTime;
		sTimePicker->Time = selTime;

		selTime = RecurringQuery->FieldByName( "DauerStunden" )->AsDateTime;
		eTimePicker->Time = selTime;
	}
	enableDisableControls();
}
//---------------------------------------------------------------------------
void TRecurringForm::transfer2Table( void )
{
	int				counter;
	TTime			selTime;

	RecurringQuery->FieldByName( "Wiederholung" )->AsInteger = RepeatComboBox->ItemIndex;

	if( CounterComboBox->Visible )
	{
		counter = CounterComboBox->ItemIndex;
		if( counter > 4 )
			counter = 4 - counter;
	}
	else if( WeekComboBox->Visible )
		counter = WeekComboBox->ItemIndex;
	else
		counter = 0;

	RecurringQuery->FieldByName( "Zaehler" )->AsInteger = counter;

	RecurringQuery->FieldByName( "Wochentag" )->AsInteger = WeekDayComboBox->ItemIndex+1;
	RecurringQuery->FieldByName( "Monat" )->AsInteger = MonthComboBox->ItemIndex +1;

	selTime = sTimePicker->Time - floor( sTimePicker->Time );
	RecurringQuery->FieldByName( "UhrzeitAnfang" )->AsDateTime = selTime;

	selTime = eTimePicker->Time - floor( eTimePicker->Time );
	RecurringQuery->FieldByName( "DauerStunden" )->AsDateTime = selTime;
}
//---------------------------------------------------------------------------
void __fastcall TRecurringForm::RecurringQueryAfterScroll(TDataSet *)
{
	transfer2Controls();
}
//---------------------------------------------------------------------------
void __fastcall TRecurringForm::RecurringQueryBeforePost(TDataSet *)
{
	if( MonthDayEdit->Visible
	&& 	(MonthDayEdit->Field->IsNull ||	MonthDayEdit->Field->AsInteger == 0) )
	{
		Application->MessageBox( "Geben Sie einen Monatstag ein", "Fehler", 0 );
		Abort();
	}
	if( WeekDayComboBox->Visible && WeekDayComboBox->ItemIndex < 0 )
	{
		Application->MessageBox( "Geben Sie einen Wochentag ein", "Fehler", 0 );
		Abort();
	}
	if( MonthComboBox->Visible && MonthComboBox->ItemIndex < 0 )
	{
		Application->MessageBox( "Geben Sie einen Monat ein", "Fehler", 0 );
		Abort();
	}

	transfer2Table();

	RecurringQuery->FieldByName( "LetzterEintrag" )->Clear();

	// remove entries in reservation table
	removeSchedules();
}
//---------------------------------------------------------------------------
void __fastcall TRecurringForm::RecurringQueryAfterInsert(TDataSet *)
{
	RecurringQuery->FieldByName( "Wiederholung" )->AsInteger = 0;
	RecurringQuery->FieldByName( "UserId" )->AsInteger = userId;
	RecurringQuery->FieldByName( "ID" )->AsInteger = getNewMaxValue(
		"SchedulerDB", "RECURRING", "ID"
	);
	RepeatComboBox->ItemIndex = 0;
	enableDisableControls();
}
//---------------------------------------------------------------------------
void __fastcall TRecurringForm::RecurringQueryAfterCancel(TDataSet *)
{
	transfer2Controls();
}
//---------------------------------------------------------------------------
void __fastcall TRecurringForm::RecurringQueryBeforeDelete(TDataSet *)
{
	removeSchedules();
}
//---------------------------------------------------------------------------
void __fastcall TRecurringForm::RecurringQueryAfterPost(TDataSet *)
{
	TScheduleForm *ScheduleForm;

	getRecurringSchedules();

	for( int i=0; i<MainForm->MDIChildCount; i++ )
	{
		ScheduleForm = dynamic_cast<TScheduleForm *>(MainForm->MDIChildren[i]);
		if( ScheduleForm && ScheduleForm->getUserId() == userId )
		{
			ScheduleForm->ReloadTable();
			break;
		}
	}

}
//---------------------------------------------------------------------------
void __fastcall TRecurringForm::ComboBoxChange(TObject *)
{
	if( readOnly )
/*@*/	return;

	enableDisableControls();
	RecurringQuery->Edit();
}
//---------------------------------------------------------------------------
void __fastcall TRecurringForm::sTimePickerChange(TObject *)
{
	if( readOnly )
/*@*/	return;

	RecurringQuery->Edit();
	transfer2Table();
}
//---------------------------------------------------------------------------

void __fastcall TRecurringForm::FormClose(TObject *,
	  TCloseAction &Action)
{
	RecurringQuery->Close();
	Action = caFree;
}
//---------------------------------------------------------------------------

