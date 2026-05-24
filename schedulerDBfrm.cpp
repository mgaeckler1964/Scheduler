/*
	Project:		Scheduler
	Module:			schedulerDBfrm.cpp
	Description:	The main form for the database admin program
	Author:			Martin Gäckler
	Address:		Hofmannsthalweg 14, A-4030 Linz
	Web:			https://www.gaeckler.at/

	Copyright:		(c) 1988-2026 Martin Gäckler

	This program is free software: you can redistribute it and/or modify  
	it under the terms of the GNU General Public License as published by  
	the Free Software Foundation, version 3.

	You should have received a copy of the GNU General Public License 
	along with this program. If not, see <http://www.gnu.org/licenses/>.

	THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Linz, Austria ``AS IS''
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
#pragma hdrstop

#include "schedulerDBfrm.h"
//---------------------------------------------------------------------------
using namespace gak;
using namespace vcl;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "dbConvertFrm"
#pragma resource "*.dfm"
TSchedulerDbManagementForm *SchedulerDbManagementForm;
//---------------------------------------------------------------------------
#pragma option -RT-
class SchedulerSchema : public DatabaseSchema
{
	virtual void checkFieldsBeforePost( TDataSet *destTable, const STRING &tableName );
};
#pragma option -RT+
//---------------------------------------------------------------------------
__fastcall TSchedulerDbManagementForm::TSchedulerDbManagementForm(TComponent* Owner)
	: TDbConvertForm(Owner)
{
}
//---------------------------------------------------------------------------
DatabaseSchema *TSchedulerDbManagementForm::createSchema() const
{
	return new SchedulerSchema;
}
//---------------------------------------------------------------------------
const char *TSchedulerDbManagementForm::getAliasName() const
{
	return "SCHEDULE";
}
//---------------------------------------------------------------------------
void SchedulerSchema::checkFieldsBeforePost( TDataSet *destTable, const STRING &tableName )
{
	CI_STRING	myTableName = tableName;

	if( myTableName == "USER_TAB" )
	{
		TField	*theField = destTable->FieldByName( "TYPE" );
		if( theField->IsNull )
			theField->AsInteger = 0;

		theField = destTable->FieldByName( "PERMISSIONS" );
		if( theField->IsNull )
			theField->AsInteger = -1;
	}
}

