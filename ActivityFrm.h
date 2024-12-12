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
#ifndef ActivityFrmH
#define ActivityFrmH
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <Db.hpp>
#include <DBCtrls.hpp>
#include <DBGrids.hpp>
#include <DBTables.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
#include <fstream>
#include <math.h>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Db.hpp>
#include <DBCtrls.hpp>
#include <DBTables.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include <DBGrids.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>

#include <gak/httpBaseServer.h>

//---------------------------------------------------------------------------
class TActivityForm : public TForm
{
__published:	// IDE-managed Components
	TDataSource *TaskSource;
	TQuery *TaskQuery;
	TQuery *ActivityQuery;
	TDataSource *ActivitySqlSource;
	TPanel *Panel1;
	TDBGrid *DBGridActivities;
	TDBNavigator *DBNavigator1;
	TMainMenu *MainMenu1;
	TMenuItem *Ttigkeiten1;
	TMenuItem *ExportProtocol;
	TButton *RestartButton;
	TMenuItem *Test;
	TMenuItem *ExportCSV;
	TMenuItem *ImportCSV;
	TMenuItem *N1;
	TSaveDialog *SaveDialog;
	TOpenDialog *OpenDialog;
	TIntegerField *ActivityQueryTask;
	TDateTimeField *ActivityQueryStartTime;
	TDateTimeField *ActivityQueryEndTime;
	TStringField *ActivityQuerytitle;
	TStringField *ActivityQueryprojectName;
	TIntegerField *ActivityQueryprojectID;
	TTimer *AutoSaveTimer;
	TIntegerField *ActivityQueryID;
	TButton *StartButton;
	TButton *StopButton;
	TButton *DeleteButton;
	TButton *SaveButton;
	TQuery *insertQuery;
	TQuery *updateQuery;
	TIntegerField *TaskQueryid;
	TStringField *TaskQuerytitle;
	TDateTimePicker *StartTimePicker;
	TDateTimePicker *StartDatePicker;
	TDateTimePicker *EndDatePicker;
	TDateTimePicker *EndTimePicker;
	TComboBox *TaskComboBox;
	TMenuItem *N2;
	TMenuItem *N3;
	TMenuItem *ExportAufgaben1;
	TMenuItem *ImportTtigkeiten1;
	TQuery *deleteQuery;
	TStringField *ActivityQueryTransfered;
	TUpdateSQL *ActivityUpdateSQL;
	void __fastcall ExportProtocolClick(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall RestartButtonClick(TObject *Sender);
	void __fastcall ExportCSVClick(TObject *Sender);
	void __fastcall ImportCSVClick(TObject *Sender);
	void __fastcall AutoSaveTimerTimer(TObject *Sender);
	void __fastcall StartButtonClick(TObject *Sender);
	void __fastcall StopButtonClick(TObject *Sender);
	void __fastcall DeleteButtonClick(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall ActivityQueryAfterScroll(TDataSet *DataSet);
	void __fastcall TaskQueryAfterOpen(TDataSet *DataSet);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall ExportAufgaben1Click(TObject *Sender);
	void __fastcall ImportTtigkeiten1Click(TObject *Sender);
	void __fastcall TestClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall DBGridActivitiesDrawColumnCell(TObject *Sender, const TRect &Rect,
          int DataCol, TColumn *Column, TGridDrawState State);
private:	// User declarations
	long userId;
	bool readOnly;

	int	currentActivityId;
	void StartTimer( int activityId )
	{
		currentActivityId = activityId;

		AutoSaveTimer->Interval = 10000;
		AutoSaveTimer->Enabled = true;
		StopButton->Enabled = true;
		DeleteButton->Enabled = false;
		SaveButton->Enabled = false;
		RestartButton->Enabled = false;
		StartDatePicker->Enabled = false;
		StartTimePicker->Enabled = false;
		EndDatePicker->Enabled = false;
		EndTimePicker->Enabled = false;

	}
	void StopTimer()
	{
		AutoSaveTimer->Enabled = false;
		StopButton->Enabled = false;
		DeleteButton->Enabled = true;
		SaveButton->Enabled = true;
		RestartButton->Enabled = true;
		StartDatePicker->Enabled = true;
		StartTimePicker->Enabled = true;
		EndDatePicker->Enabled = true;
		EndTimePicker->Enabled = true;
		currentActivityId = -1;
	}
	void RestartActivity( bool gotoFirst );
	TDateTime getStart()
	{
		return floor(StartDatePicker->Date)+StartTimePicker->Time-floor(StartTimePicker->Time);
	}
	TDateTime getEnd()
	{
		return floor(EndDatePicker->Date)+EndTimePicker->Time -floor(EndTimePicker->Time);
	}

	int getSelectedTaskId();
	void selectTask( int TaskId );
	int getLastTask( void );
	void saveTask( int TaskId );

	void deleteActivities(
		const AnsiString	&selProject,
		TDateTime			minDate,
		TDateTime 			maxDate
	);
	double doExportProtocol(
		std::ofstream		&fp,
		const AnsiString	&selProject,
		unsigned short		roundValue,
		TDateTime			minDate,
		TDateTime			maxDate
	);

	bool testActivities( bool showOk );

public:		// User declarations
	gak::STRING importActivitiesFromStream( std::istream &csvFile );
	__fastcall TActivityForm(TComponent* Owner);
	void setUserId( long userId, gak::STRING userName, int perms );
	long getUserId( void ) const
	{
		return userId;
	}
};
//---------------------------------------------------------------------------
class SchedulerServer : public gak::net::HTTPserverBase
{
	virtual int handleGetRequest( const gak::STRING &url );
	virtual int handlePostRequest( const gak::STRING &url );
	public:
	SchedulerServer() : gak::net::HTTPserverBase()
	{
	}
};
extern gak::net::SocketServer<SchedulerServer> theActivitiesServer;


//---------------------------------------------------------------------------
// extern PACKAGE TActivityForm *ActivityForm;
//---------------------------------------------------------------------------
#endif
