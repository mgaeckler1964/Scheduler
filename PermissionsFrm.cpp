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
#include <gak/vcl_tools.h>
#pragma hdrstop

#include "PermissionsFrm.h"

//---------------------------------------------------------------------------
using namespace gak;
using namespace vcl;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPermissionsForm *PermissionsForm;
//---------------------------------------------------------------------------
__fastcall TPermissionsForm::TPermissionsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TPermissionsForm::FormShow(TObject *)
{
	char filter[128];

	QueryAll->Open();
	TableACLs->Filtered = true;
	sprintf( filter, "SCHEDULER_ID=%d", actUser.ID );
	TableACLs->Filter = filter;
	TableACLs->Open();
}
//---------------------------------------------------------------------------
void __fastcall TPermissionsForm::FormClose(TObject *,
      TCloseAction &)
{
	TableACLs->Close();
	QueryAll->Close();
}
//---------------------------------------------------------------------------
void __fastcall TPermissionsForm::TableACLsAfterInsert(TDataSet *)
{
	TableACLsSCHEDULER_ID->AsInteger = actUser.ID;
	TableACLsPERMISSIONS->AsInteger = PERM_NONE;
}
//---------------------------------------------------------------------------
void __fastcall TPermissionsForm::TableACLsAfterScroll(TDataSet *)
{
	int perms = TableACLsPERMISSIONS->AsInteger;

	if( perms & PERM_SCHEDULE_WRITE )
		RadioGroupSchedules->ItemIndex = 2;
	else if( perms & PERM_SCHEDULE_READ )
		RadioGroupSchedules->ItemIndex = 1;
	else
		RadioGroupSchedules->ItemIndex = 0;

	if( perms & PERM_RECURRING_WRITE )
		RadioGroupRecurring->ItemIndex = 2;
	else if( perms & PERM_RECURRING_READ )
		RadioGroupRecurring->ItemIndex = 1;
	else
		RadioGroupRecurring->ItemIndex = 0;

	if( perms & PERM_ACTIVITY_WRITE )
		RadioGroupActivities->ItemIndex = 2;
	else if( perms & PERM_ACTIVITY_READ )
		RadioGroupActivities->ItemIndex = 1;
	else
		RadioGroupActivities->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TPermissionsForm::RadioGroupSchedulesClick(TObject *)
{
	int perms = TableACLsPERMISSIONS->AsInteger;

	perms &= 0xFF0;
	if( RadioGroupSchedules->ItemIndex == 2 )
		perms |= PERM_SCHEDULE_WRITE|PERM_SCHEDULE_READ;
	else if( RadioGroupSchedules->ItemIndex == 1 )
		perms |= PERM_SCHEDULE_READ;

	if( TableACLsPERMISSIONS->AsInteger != perms )
	{
		if( TableACLs->State == dsBrowse )
			TableACLs->Edit();
		TableACLsPERMISSIONS->AsInteger = perms;
	}
}
//---------------------------------------------------------------------------
void __fastcall TPermissionsForm::RadioGroupRecurringClick(TObject *)
{
	int perms = TableACLsPERMISSIONS->AsInteger;

	perms &= 0xF0F;
	if( RadioGroupRecurring->ItemIndex == 2 )
		perms |= PERM_RECURRING_WRITE|PERM_RECURRING_READ;
	else if( RadioGroupRecurring->ItemIndex == 1 )
		perms |= PERM_RECURRING_READ;

	if( TableACLsPERMISSIONS->AsInteger != perms )
	{
		if( TableACLs->State == dsBrowse )
			TableACLs->Edit();
		TableACLsPERMISSIONS->AsInteger = perms;
	}
}
//---------------------------------------------------------------------------
void __fastcall TPermissionsForm::RadioGroupActivitiesClick(
	  TObject *)
{
	int perms = TableACLsPERMISSIONS->AsInteger;

	perms &= 0x0FF;
	if( RadioGroupActivities->ItemIndex == 2 )
		perms |= PERM_ACTIVITY_WRITE|PERM_ACTIVITY_READ;
	else if( RadioGroupActivities->ItemIndex == 1 )
		perms |= PERM_ACTIVITY_READ;

	if( TableACLsPERMISSIONS->AsInteger != perms )
	{
		if( TableACLs->State == dsBrowse )
			TableACLs->Edit();
		TableACLsPERMISSIONS->AsInteger = perms;
	}
}
//---------------------------------------------------------------------------
