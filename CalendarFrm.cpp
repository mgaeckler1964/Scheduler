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
#include <stdio.h>
#include <math.h>

#include <vcl.h>
#include <gak/date.h>

#pragma hdrstop

#include "CalendarFrm.h"
// #include "MainFrm.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ccalendr"
#pragma link "cspin"
#pragma link "Scheduler"
#pragma link "DayView"
#pragma link "CCALENDR"
#pragma link "CSPIN"
#pragma resource "*.dfm"
TCalendarForm *CalendarForm;
//---------------------------------------------------------------------------
__fastcall TCalendarForm::TCalendarForm(TComponent* Owner)
	: TForm(Owner)
{
	lastDay = lastMonth = lastYear = -1;
}
//---------------------------------------------------------------------------
void __fastcall TCalendarForm::YearSpinChange(TObject *)
{
	Scheduler->Year = YearSpin->Value;
}
//---------------------------------------------------------------------------
void __fastcall TCalendarForm::MonthComboChange(TObject *)
{
	int	selMonth = MonthCombo->ItemIndex;
	if( selMonth >= 0 )
	{
		selMonth++;
		gak::Date	newDay(
			(unsigned short)Scheduler->Day,
			gak::Date::Month(selMonth),
			(unsigned short)Scheduler->Year
		);
		if( !newDay.isValid() )
		{
			Scheduler->Day = 1;
		}
		Scheduler->Month = selMonth;
	}
}
//---------------------------------------------------------------------------

void __fastcall TCalendarForm::FormShow(TObject *)
{
	int		i;
	char	timeStr[16];

	YearSpin->Value = Scheduler->Year;
	MonthCombo->ItemIndex = Scheduler->Month -1;

	for( i=0; i<DayView->RowCount; i++ )
	{
		sprintf( timeStr, "%02d:%02d", i/2, (i%2)*30 );
		DayView->Cells[0][i] = timeStr;
	}
}
//---------------------------------------------------------------------------



void __fastcall TCalendarForm::SchedulerChange(TObject *)
{
	gak::Date	theDate(
		(unsigned short)Scheduler->Day,
		gak::Date::Month(Scheduler->Month),
		(unsigned short)Scheduler->Year );
	AnsiString	weekDay;
	char		tmp[32];
	const char	*holiDay;

	weekDay = theDate.weekDayName();

	sprintf(
		tmp,
		"%d. %d. %d",
		(int)Scheduler->Day, (int)Scheduler->Month, (int)Scheduler->Year
	);
	weekDay += ", ";
	weekDay += tmp;

	holiDay = theDate.holiday();
	if( holiDay && *holiDay )
	{
		weekDay += ", ";
		weekDay += holiDay;
	}

	DayLabel->Caption = weekDay;


	unsigned short	tmpMonth = (unsigned short)Scheduler->Month;
	unsigned short	tmpYear = (unsigned short)Scheduler->Year;
	unsigned short	tmpDay = (unsigned short)Scheduler->Day;

	if( lastDay != tmpDay || lastMonth != tmpMonth || lastYear != tmpYear )
	{
		TDateTime 		firstDay = EncodeDate( tmpYear, tmpMonth, tmpDay );
		TDateTime 		lastDay = firstDay + 1;

		this->lastDay = tmpDay;

		SchedulesQuery->ParamByName( "theUser" )->AsInteger = userId;
		SchedulesQuery->ParamByName( "firstDay" )->AsDateTime = firstDay;
		SchedulesQuery->ParamByName( "lastDay" )->AsDateTime = lastDay;

		DayView->ClearAppointments();
		SchedulesQuery->Open();
		while( !SchedulesQuery->Eof )
		{
			TDateTime startDate = SchedulesQuery->FieldByName( "startDate" )->AsDateTime;
			TDateTime endDate = SchedulesQuery->FieldByName( "endDate" )->AsDateTime;
			AnsiString title = SchedulesQuery->FieldByName( "title" )->AsString;

			double rowDbl =(double)startDate;
			rowDbl -= floor( rowDbl );
			rowDbl *= 48.0;
			int row1 = (rowDbl + 1.0/600.0);
			rowDbl =(double)endDate;
			rowDbl -= floor( rowDbl );
			rowDbl *= 48.0;
			int row2 = (rowDbl + 1.0/600.0) -1;

			if( row2 < row1 )
				row2 = row1;

			DayView->SetAppointment( row1, row2, title.c_str() );
			SchedulesQuery->Next();
		}
		SchedulesQuery->Close();
	}

	if( lastMonth != tmpMonth || lastYear != tmpYear )
	{
		TDateTime 		firstDay = EncodeDate( tmpYear, tmpMonth, 1 );

		lastMonth = tmpMonth;
		lastYear = tmpYear;

		tmpMonth++;
		if( tmpMonth > 12 )
		{
			tmpMonth = 1;
			tmpYear++;
		}
		TDateTime lastDay = EncodeDate( tmpYear, tmpMonth, 1 );


		SchedulesQuery->ParamByName( "theUser" )->AsInteger = userId;
		SchedulesQuery->ParamByName( "firstDay" )->AsDateTime = firstDay;
		SchedulesQuery->ParamByName( "lastDay" )->AsDateTime = lastDay;

		Scheduler->ClearAppointments();
		SchedulesQuery->Open();
		while( !SchedulesQuery->Eof )
		{
			TDateTime startDate = SchedulesQuery->FieldByName( "startDate" )->AsDateTime;
			startDate.DecodeDate( &tmpYear, &tmpMonth, &tmpDay );
			Scheduler->SetAppointment( tmpDay, true );
			SchedulesQuery->Next();
		}
		SchedulesQuery->Close();
	}
}
//---------------------------------------------------------------------------

void __fastcall TCalendarForm::FormClose(TObject *,
      TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------------

