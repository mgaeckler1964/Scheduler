/*
		Project:		Scheduler
		Module:			
		Description:	
		Author:			Martin G�ckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2024 Martin G�ckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin G�ckler, Austria, Linz ``AS IS''
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

#ifndef PermissionsFrmH
#define PermissionsFrmH
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
//---------------------------------------------------------------------------
const int PERM_NONE				= 0x000;

const int PERM_SCHEDULE_READ	= 0x001;
const int PERM_SCHEDULE_WRITE	= 0x002;

const int PERM_RECURRING_READ	= 0x010;
const int PERM_RECURRING_WRITE	= 0x020;

const int PERM_ACTIVITY_READ	= 0x100;
const int PERM_ACTIVITY_WRITE	= 0x200;

//---------------------------------------------------------------------------
class TPermissionsForm : public TForm
{
__published:	// IDE-managed Components
	TQuery *QueryAll;
	TDataSource *DataSourceACLs;
	TDBGrid *DBGrid1;
	TDBNavigator *DBNavigator;
	TTable *TableACLs;
	TIntegerField *TableACLsUSER_ID;
	TIntegerField *TableACLsPERMISSIONS;
	TStringField *TableACLsUSERNAME;
	TIntegerField *TableACLsSCHEDULER_ID;
	TRadioGroup *RadioGroupSchedules;
	TRadioGroup *RadioGroupRecurring;
	TRadioGroup *RadioGroupActivities;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall TableACLsAfterInsert(TDataSet *DataSet);
	void __fastcall TableACLsAfterScroll(TDataSet *DataSet);
	void __fastcall RadioGroupSchedulesClick(TObject *Sender);
	void __fastcall RadioGroupRecurringClick(TObject *Sender);
	void __fastcall RadioGroupActivitiesClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TPermissionsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TPermissionsForm *PermissionsForm;
//---------------------------------------------------------------------------
#endif
