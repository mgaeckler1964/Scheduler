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
#ifndef CalendarFrmH
#define CalendarFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ccalendr.h"
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include "cspin.h"
#include "Scheduler.h"
#include <ExtCtrls.hpp>
#include "DayView.h"
#include "CCALENDR.h"
#include "CSPIN.h"
#include <Db.hpp>
#include <DBTables.hpp>

#include <gak/string.h>

//---------------------------------------------------------------------------
class TCalendarForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TComboBox *MonthCombo;
	TCSpinEdit *YearSpin;
	TScheduler *Scheduler;
	TDayView *DayView;
	TLabel *DayLabel;
	TQuery *SchedulesQuery;
	void __fastcall YearSpinChange(TObject *Sender);

	void __fastcall MonthComboChange(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall SchedulerChange(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	
private:	// User declarations
	long userId;
	unsigned short	lastDay, lastMonth, lastYear;

	void showAppointments( void );
public:		// User declarations
	void setUserId( long userId, const gak::STRING &userName )
	{
		gak::STRING newCaption = Caption.c_str();
		newCaption += ' ';
		newCaption += userName;
		Caption = (const char*)newCaption;

		this->userId = userId;
		SchedulerChange( NULL );

		int tag = this->Tag;
		tag %= 10;
		tag += userId * 10;
		this->Tag = tag;
	}
	long getUserId( void ) const
	{
		return userId;
	}
	__fastcall TCalendarForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
// extern PACKAGE TCalendarForm *CalendarForm;
//---------------------------------------------------------------------------
#endif

