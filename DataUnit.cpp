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

#include "DataUnit.h"

//---------------------------------------------------------------------------
using namespace gak;
using namespace vcl;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TtheDataModule *theDataModule;
//---------------------------------------------------------------------------
__fastcall TtheDataModule::TtheDataModule(TComponent* Owner)
	: TDataModule(Owner)
{
}

int TtheDataModule::getProjectId( const char *projectName )
{
	bool	projectFound = false;
	int		projectId;

	ProjectTable->First();
	while( !ProjectTable->Eof )
	{
		if( ProjectTableNAME->AsString == projectName )
		{
			projectFound = true;
			break;
		}
		ProjectTable->Next();
	}

	if( !projectFound )
	{
		ProjectTable->Insert();
		ProjectTableNAME->AsString = projectName;
		ProjectTable->Post();
	}

	projectId = ProjectTableID->AsInteger;

	return projectId;
}

//---------------------------------------------------------------------------
void __fastcall TtheDataModule::ProjectTableBeforeDelete(TDataSet *)
{
	static TQuery	*numTasksQuery = NULL;
	int				numTasks;


	if( !numTasksQuery )
	{
		numTasksQuery = new TQuery( this );
		numTasksQuery->DatabaseName = "SchedulerDB";
		numTasksQuery->SQL->Add(
			"select count(*) from schedule where project = :theProject"
		);
	}

	numTasksQuery->Params->Items[0]->AsInteger = ProjectTableID->AsInteger;
	numTasksQuery->Open();
	numTasks = numTasksQuery->Fields->Fields[0]->AsInteger;
	numTasksQuery->Close();

	if( numTasks )
		throw Exception(
			"Datensatz kann nicht gelöscht werden.\n"
			"Es gibt Aufgaben zu diesem Projekt!"
		);
}
//---------------------------------------------------------------------------
int TtheDataModule::getNewActivitiyId( void )

{
	int	newId = 1;

	ActivityMaxIdQuery->Open();
	if(!ActivityMaxIdQuery->Eof)
		newId =ActivityMaxIdQuery->Fields->Fields[0]->AsInteger + 1;
	ActivityMaxIdQuery->Close();


	return newId;
}
//---------------------------------------------------------------------------

void __fastcall TtheDataModule::ProjectTableAfterInsert(TDataSet *)
{
	setNewMaxValue( ProjectTable, ProjectTableID );
}
//---------------------------------------------------------------------------

