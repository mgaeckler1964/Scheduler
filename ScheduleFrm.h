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
#ifndef ScheduleFrmH
#define ScheduleFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Db.hpp>
#include <DBCtrls.hpp>
#include <DBGrids.hpp>
#include <DBTables.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>

#include <gak/string.h>

//---------------------------------------------------------------------------
class TScheduleForm : public TForm
{
__published:	// IDE-managed Components
	TDataSource *ScheduleSource;
	TDBGrid *ScheduleGrid;
	TDBMemo *DBMemo1;
	TQuery *datesQuery;
	TMainMenu *MainMenu1;
	TMenuItem *Termin1;
	TMenuItem *NeuerTermin;
	TMenuItem *TerminBearbeiten;
	TPanel *Panel1;
	TBitBtn *BitBtnNew;
	TBitBtn *BitBtnChange;
	TBitBtn *BitBtnDelete;
	TMenuItem *TerminLschen;
	TTable *dbaseTable;
	TSaveDialog *SaveDialog;
	TMenuItem *ExportAll;
	TMenuItem *N1;
	TDateField *dbaseTableDATUM;
	TFloatField *dbaseTableANFANG;
	TFloatField *dbaseTableENDE;
	TFloatField *dbaseTableALARM;
	TStringField *dbaseTableTEXT;
	TQuery *newQuery;
	TMenuItem *Export2;
	TMenuItem *ExportNew;
	TQuery *updateUserSQL;
	TMenuItem *N2;
	TMenuItem *Import1;
	TOpenDialog *OpenDialog;
	TQuery *insertDateSQL;
	TBitBtn *BitBtnDeleteAll;
	TMenuItem *ImExportOutlook1;
	TMenuItem *Import2;
	TMenuItem *N3;
	TMenuItem *ExportAlle1;
	TMenuItem *ExportNeue1;
	TQuery *activeDatesQuery;
	TTimer *theTimer;
	TDBNavigator *DBNavigator;
	TRadioButton *RadioAll;
	TRadioButton *DatesRadio;
	TRadioButton *TasksRadio;
	TBitBtn *FinishBitBtn;
	void __fastcall NeuerTerminClick(TObject *Sender);
	void __fastcall TerminBearbeitenClick(TObject *Sender);
	void __fastcall TerminLschenClick(TObject *Sender);
	void __fastcall ScheduleGridDrawColumnCell(TObject *Sender,
		  const TRect &Rect, int DataCol, TColumn *Column,
		  TGridDrawState State);
	void __fastcall ExportAllClick(TObject *Sender);
	void __fastcall ExportNewClick(TObject *Sender);
	void __fastcall Import1Click(TObject *Sender);
	void __fastcall ButtonAllClick(TObject *Sender);
	void __fastcall ButtonDatesClick(TObject *Sender);
	void __fastcall ButtonTasksClick(TObject *Sender);
	void __fastcall BitBtnDeleteAllClick(TObject *Sender);
	void __fastcall ExportAlle1Click(TObject *Sender);
	void __fastcall ExportNeue1Click(TObject *Sender);
	void __fastcall Import2Click(TObject *Sender);
	void __fastcall theTimerTimer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FinishBitBtnClick(TObject *Sender);
private:	// User declarations
	long	userId;
	bool	readOnly;

	int getLastExport( void );
	void exportOutlook( TQuery *datesSQL );
	void importOutlook( void );

	void exportCasio( TQuery *datesSQL );
	void importCasio( void );
public:		// User declarations
	__fastcall TScheduleForm(TComponent* Owner);
	void ReloadTable( int id=0 );
	void setUserId( long userId, const gak::STRING &userName, int permissions );
	long getUserId( void ) const
	{
		return userId;
	}
};
//---------------------------------------------------------------------------
// extern PACKAGE TScheduleForm *ScheduleForm;
//---------------------------------------------------------------------------
#endif

