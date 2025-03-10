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
#ifndef EditDateDlgH
#define EditDateDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Db.hpp>
#include <DBCtrls.hpp>
#include <DBTables.hpp>
#include <Mask.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TEditDateDialog : public TForm
{
__published:	// IDE-managed Components
	TDataSource *ScheduleSource;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TDBEdit *DBEditTitle;
	TLabel *Label4;
	TDBMemo *DBMemoDescription;
	TButton *ButtonOk;
	TButton *ButtonCancel;
	TLabel *Label5;
	TDBEdit *AlarmBeforeDBEdit;
	TDBComboBox *AlarmUnitComboBox;
	TDBEdit *ProgramDBEdit;
	TLabel *Label6;
	TDateTimePicker *startDatePicker;
	TDateTimePicker *startTimePicker;
	TDateTimePicker *endTimePicker;
	TDateTimePicker *endDatePicker;
	TLabel *Label7;
	TDBEdit *EstEffortDBEdit;
	TDBEdit *ActEffortDBEdit;
	TDBEdit *PercentCompletedDBEdit;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TDBLookupComboBox *RequiredDBLookupComboBox;
	TQuery *RequiredQuery;
	TDataSource *RequiredSource;
	TButton *ButtonNoTask;
	TDBComboBox *_EffortUnitComboBox;
	TLabel *Label12;
	TDataSource *ProjectSource;
	TQuery *ProjectQuery;
	TDBLookupComboBox *ProjectDBLookupComboBox;
	TLabel *Label13;
	TDBEdit *LocationDBEdit;
	TTable *ScheduleTable;
	TIntegerField *ScheduleTableID;
	TIntegerField *ScheduleTableUSERID;
	TDateTimeField *ScheduleTableSTARTDATE;
	TDateTimeField *ScheduleTableENDDATE;
	TDateTimeField *ScheduleTableALARMDATE;
	TIntegerField *ScheduleTablePROJECT;
	TFloatField *ScheduleTableESTEFFORT;
	TFloatField *ScheduleTableACTEFFORT;
	TStringField *ScheduleTableEFFORTUNIT;
	TIntegerField *ScheduleTablePERCENTCOMPLETED;
	TIntegerField *ScheduleTableREQUIREDTASK;
	TStringField *ScheduleTableTITLE;
	TStringField *ScheduleTableDESCRIPTION;
	TStringField *ScheduleTableLOCATION;
	TStringField *ScheduleTableCOMMAND;
	TIntegerField *ScheduleTableALARMBEFORE;
	TStringField *ScheduleTableALARMUNIT;
	TIntegerField *ScheduleTableRECURRINGID;
	void __fastcall ButtonOkClick(TObject *Sender);
	void __fastcall ButtonCancelClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ScheduleTableBeforePost(TDataSet *DataSet);
	void __fastcall ButtonNoTaskClick(TObject *Sender);
	void __fastcall AlarmBeforeDBEditChange(TObject *Sender);
private:	// User declarations
	long	m_actualDateId;
	bool	m_readOnly;
	long	m_userId;
public:		// User declarations
	__fastcall TEditDateDialog(TComponent* Owner);
	void setActualDate( long theDateId, bool readOnly )
	{
		m_actualDateId = theDateId;
		m_readOnly = readOnly;
	}
	void setNewDate( void )
	{
		m_actualDateId = 0;
	}
	long getNewDate( void ) const
	{
		return m_actualDateId;
	}
	int ShowModal( long userId )
	{
		m_userId = userId;
		return TForm::ShowModal();
	}
};
//---------------------------------------------------------------------------
extern PACKAGE TEditDateDialog *EditDateDialog;
//---------------------------------------------------------------------------
#endif
