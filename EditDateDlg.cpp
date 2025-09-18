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
#include <math.h>

#include <gak/date.h>
#pragma hdrstop

#include "EditDateDlg.h"
#include "MainFrm.h"

//---------------------------------------------------------------------------
using namespace gak;
using namespace vcl;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditDateDialog *EditDateDialog;
//---------------------------------------------------------------------------
__fastcall TEditDateDialog::TEditDateDialog(TComponent* Owner)
	: TForm(Owner)
{
	m_actualDateId = 0;
}
//---------------------------------------------------------------------------
static inline TDateTime GetDate( TDateTime theDate )
{
	return floor( theDate );
}

static inline TDateTime GetTime( TDateTime theDate )
{
	unsigned short hour, minute, sec, msec;
	theDate.DecodeTime( &hour, &minute, &sec, &msec );

	return EncodeTime( hour, minute, sec, 0 );
}

void __fastcall TEditDateDialog::ButtonOkClick(TObject *)
{
	if( m_readOnly )
/*@*/	return;

	TDateTime	theDate;
	theDate = GetDate(startDatePicker->Date) + GetTime(startTimePicker->Time);
	ScheduleTable->FieldByName( "StartDate" )->AsDateTime = theDate;

	theDate = GetDate(endDatePicker->Date) + GetTime(endTimePicker->Time);
	ScheduleTable->FieldByName( "EndDate" )->AsDateTime = theDate;

	if( !m_actualDateId )
	{
		m_actualDateId = getNewMaxValue(
			"SchedulerDB", "SCHEDULE", "ID"
		);
		ScheduleTableID->AsInteger = m_actualDateId;
	}

	ScheduleTable->Post();
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TEditDateDialog::ButtonCancelClick(TObject *)
{
	ScheduleTable->Cancel();
	ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TEditDateDialog::FormClose(TObject *, TCloseAction &)
{
	ScheduleTable->Close();
	RequiredQuery->Close();
	ProjectQuery->Close();
}
//---------------------------------------------------------------------------
void __fastcall TEditDateDialog::FormShow(TObject *)
{
	TDateTime	theDate;

	ProjectQuery->Open();

	RequiredQuery->ParamByName( "theUser" )->AsInteger = m_userId;
	RequiredQuery->Open();

	if( m_actualDateId )
	{
		AnsiString	filter;

		filter.printf( "ID = %ld", m_actualDateId );
		ScheduleTable->Filter = filter;
		ScheduleTable->Filtered = true;
	}
	else
		ScheduleTable->Filtered = false;


	ButtonOk->Enabled = !m_readOnly;
	ButtonNoTask->Enabled = !m_readOnly;

	ScheduleTable->Open();
	if( m_actualDateId )
	{
		if( !m_readOnly )
			ScheduleTable->Edit();
		theDate = ScheduleTable->FieldByName( "StartDate" )->AsDateTime;
		startDatePicker->Date = theDate;
		startTimePicker->Time = theDate;

		theDate = ScheduleTable->FieldByName( "EndDate" )->AsDateTime;
		endDatePicker->Date = theDate;
		endTimePicker->Time = theDate;

		Caption = "Termin bearbeiten";
	}
	else
	{
		theDate = Now();
		startDatePicker->Date = theDate;
		startTimePicker->Time = theDate;
		endDatePicker->Date = theDate;
		endTimePicker->Time = theDate;

		ScheduleTable->Insert();
		ScheduleTable->FieldByName( "userid" )->AsInteger = m_userId;
		AlarmUnitComboBox->ItemIndex = -1;
		//EffortUnitComboBox->ItemIndex = 0;
		ScheduleTable->FieldByName( "EffortUnit" )->AsString = _EffortUnitComboBox->Items->Strings[0];

		Caption = "Neuer Termin";
	}
}
//---------------------------------------------------------------------------
void __fastcall TEditDateDialog::ScheduleTableBeforePost(TDataSet *)
{
	double		effort;

	effort = ScheduleTable->FieldByName( "EstEffort" )->AsFloat;
	if( effort > 0 )
	{
		TDateTime	startDate = ScheduleTable->FieldByName( "StartDate" )->AsDateTime;
		TDateTime	endDate = ScheduleTable->FieldByName( "EndDate" )->AsDateTime;
		double		daysLeft =	endDate - startDate;
		double		partDay = daysLeft - floor( daysLeft );

		daysLeft = gak::getNumWorkDays(
			gak::Date( startDate ), gak::Date( endDate )
		);
		daysLeft += partDay;

		if( ScheduleTable->FieldByName( "EFFORTUNIT" )->AsString != "d" )
		{
			effort /= MainForm->getWorkTimePerDay();
		}

		if( daysLeft < effort )
		{
			Application->MessageBox(
				"Zu wenig Zeit für den Aufwand reserviert.",
				"Fehler",
				MB_ICONERROR
			);
			Abort();
		}
	}

	int	AlarmBefore = ScheduleTable->FieldByName( "AlarmBefore" )->AsInteger;
	if( AlarmBefore > 0 )
	{
		TDateTime	theDate;
		TDateTime	AlarmDate = ScheduleTable->FieldByName(
			"StartDate"
		)->AsDateTime;

		switch( AlarmUnitComboBox->ItemIndex )
		{
			case 0:			// Minuten
				theDate = EncodeTime( 0, (Word)AlarmBefore, 0, 0 );
				AlarmDate -= theDate;
				break;
			case 1:			// Stunden
				theDate = EncodeTime( (Word)AlarmBefore, 0, 0, 0 );
				AlarmDate -= theDate;
				break;
			case 2:			// Tage
				AlarmDate -= AlarmBefore;
				break;
			case 3:			// Wochen
				AlarmDate -= 7*AlarmBefore;
				break;
		}
		ScheduleTable->FieldByName( "AlarmDate" )->AsDateTime = AlarmDate;
	}
	else
	{
		ScheduleTable->FieldByName( "AlarmBefore" )->Clear();
		ScheduleTable->FieldByName( "AlarmDate" )->Clear();
		ScheduleTable->FieldByName( "ALARMUNIT" )->Clear();
	}
}
//---------------------------------------------------------------------------




void __fastcall TEditDateDialog::ButtonNoTaskClick(TObject *)
{
	ScheduleTable->FieldByName("RequiredTask")->AsInteger = 0;	
}
//---------------------------------------------------------------------------



void __fastcall TEditDateDialog::AlarmBeforeDBEditChange(TObject *)
{
	if( AlarmBeforeDBEdit->Text > "" )
	{
		if( ScheduleTable->FieldByName( "ALARMUNIT" )->IsNull )
			ScheduleTable->FieldByName( "ALARMUNIT" )->AsString =
			AlarmUnitComboBox->Items->Strings[0];
	}
	else
	{
		if( !ScheduleTable->FieldByName( "ALARMUNIT" )->IsNull )
			ScheduleTable->FieldByName( "ALARMUNIT" )->Clear();
	}
}
//---------------------------------------------------------------------------

void __fastcall TEditDateDialog::FinishBitBtnClick(TObject *Sender)
{
	AlarmBeforeDBEdit->Text = "";
}
//---------------------------------------------------------------------------

