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
#ifndef RecurringFrmH
#define RecurringFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Db.hpp>
#include <DBTables.hpp>
#include <DBCtrls.hpp>
#include <DBGrids.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Mask.hpp>
#include <ComCtrls.hpp>

#include "ScheduleFrm.h"
#include "PermissionsFrm.h"

//---------------------------------------------------------------------------
class TRecurringForm : public TForm
{
__published:	// IDE-managed Components
	TComboBox *RepeatComboBox;
	TComboBox *WeekDayComboBox;
	TComboBox *CounterComboBox;
	TComboBox *MonthComboBox;
	TLabel *Label1;
	TLabel *DayLabel;
	TLabel *MonthLabel;
	TDataSource *RecurringSource;
	TDBGrid *DBGridTitleList;
	TDBEdit *MonthDayEdit;
	TDBNavigator *DBNavigator;
	TBevel *Bevel1;
	TLabel *DayMonthLabel;
	TDBEdit *DBEditLetzterEintrag;
	TLabel *Label11;
	TDBEdit *DBEditTitle;
	TDBMemo *DBMemoDescription;
	TDBEdit *DBEditDauerTage;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TDBEdit *DBEditAlarmBefore;
	TLabel *Label6;
	TDBComboBox *AlarmUnitComboBox;
	TDateTimePicker *sTimePicker;
	TDateTimePicker *eTimePicker;
	TQuery *RecurringQuery;
	TDBEdit *DBEditCommand;
	TLabel *Label7;
	TComboBox *WeekComboBox;
	TLabel *Label8;
	TDBEdit *DBEditLocation;
	void __fastcall RecurringQueryAfterScroll(TDataSet *DataSet);
	void __fastcall RecurringQueryBeforePost(TDataSet *DataSet);
	void __fastcall RecurringQueryAfterInsert(TDataSet *DataSet);
	void __fastcall ComboBoxChange(TObject *Sender);
	void __fastcall RecurringQueryAfterCancel(TDataSet *DataSet);

	void __fastcall RecurringQueryBeforeDelete(TDataSet *DataSet);
	void __fastcall RecurringQueryAfterPost(TDataSet *DataSet);
	void __fastcall sTimePickerChange(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
	long userId;
	bool readOnly;

	void getRecurringSchedules( void );
	void removeSchedules( void );
	void enableDisableControls( void );

	void transfer2Controls( void );
	void transfer2Table( void );
public:		// User declarations
	__fastcall TRecurringForm(TComponent* Owner);
	void setUserId( long userId, const gak::STRING &userName, int permissions )
	{
		gak::STRING newCaption = Caption.c_str();
		newCaption += ' ';
		newCaption += userName;
		Caption = (const char*)newCaption;

		this->userId = userId;
		getRecurringSchedules();

		RecurringQuery->Params->Items[0]->AsInteger = userId;
		readOnly = !(permissions & PERM_RECURRING_WRITE);
		RecurringQuery->RequestLive = !readOnly;
		RecurringQuery->Open();

		int tag = this->Tag;
		tag %= 10;
		tag += userId * 10;
		this->Tag = tag;
	}
	long getUserId( void ) const
	{
		return userId;
	}
};
//---------------------------------------------------------------------------
// extern PACKAGE TRecurringForm *RecurringForm;
//---------------------------------------------------------------------------
#endif
