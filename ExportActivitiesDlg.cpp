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
#include <vcl.h>
#include <vcl/registry.hpp>
#pragma hdrstop

#include "ExportActivitiesDlg.h"
#include "MainFrm.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TExportActivitiesDialog *ExportActivitiesDialog;
//---------------------------------------------------------------------------
__fastcall TExportActivitiesDialog::TExportActivitiesDialog(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TExportActivitiesDialog::putFileNameAndMonth( const AnsiString &selProject, TDateTime startDate )
{
	unsigned short	year, month, day;
	char			tmpBuff[32];
	AnsiString		fileName;

	startDate.DecodeDate( &year, &month, &day );

	YearSelect->Position = year;
	MonthCombo->ItemIndex = month-1;

	sprintf( tmpBuff, "%hu %02hu", year, month );

	fileName = selProject;
	fileName += tmpBuff;
	fileName += ".csv";

	SaveDialog->FileName = fileName;

	fileName.Insert( "\\", 1 );
	fileName.Insert( SaveDialog->InitialDir, 1 );

	FileEdit->Text = fileName;
}

//---------------------------------------------------------------------------
int __fastcall TExportActivitiesDialog::Show( const AnsiString &selProject, TDateTime startDate )
{
	int		i, selIndex = 0;

	// fill project combo
	ProjectQuery->Open();
	ProjectCombo->Items->Clear();

	ProjectCombo->Items->Add( "Alle" );
	i = 0;
	while( !ProjectQuery->Eof )
	{
		ProjectCombo->Items->Add( ProjectQuery->Fields->Fields[0]->AsString );
		if( selProject == ProjectQuery->Fields->Fields[0]->AsString )
			selIndex = i;
		ProjectQuery->Next();
		i++;
	}
	ProjectQuery->Close();
	ProjectCombo->ItemIndex = selIndex +1;

	// setup other options
	TRegistry		*registry = new TRegistry;

	if( registry->OpenKey( REGISTRY_KEY, false ) )
	{
		if(  registry->ValueExists( "ExportActivitiesPath" ) )
			SaveDialog->InitialDir = registry->ReadString( "ExportActivitiesPath" );

		if( registry->ValueExists( "ExportRound" ) )
			RoundComboBox->ItemIndex = registry->ReadInteger( "ExportRound" );
		else
			RoundComboBox->ItemIndex = 0;

		registry->CloseKey();
	}

	delete registry;

	// determine default filename
	putFileNameAndMonth( selProject, startDate );

	GetDateRange( &startDate );

	// clear option boxes
	DeleteCheckBox->Checked = false;
	TransferCheckBox->Checked = false;

	return ShowModal();
}

//---------------------------------------------------------------------------
void __fastcall TExportActivitiesDialog::ButtonBrowseClick(TObject *)
{
	if( SaveDialog->Execute() )
		FileEdit->Text = SaveDialog->FileName;
}
//---------------------------------------------------------------------------
void __fastcall TExportActivitiesDialog::ButtonOkClick(TObject *)
{
	TRegistry	*registry = new TRegistry;

	if( registry->OpenKey( REGISTRY_KEY, true ) )
	{
		AnsiString	ExportActivitiesPath = FileEdit->Text;
		int			lastPos = ExportActivitiesPath.LastDelimiter( "\\" );

		if( lastPos )
			lastPos--;
		ExportActivitiesPath.SetLength( lastPos );

		registry->WriteString( "ExportActivitiesPath", ExportActivitiesPath );
		registry->WriteInteger( "ExportRound", RoundComboBox->ItemIndex );

		registry->CloseKey();
	}

	delete registry;
}
//---------------------------------------------------------------------------

void TExportActivitiesDialog::GetDateRange( TDateTime *minDate )
{
	TDateTime	maxDate;
	Word		minMonth, minYear;
	Word		maxMonth, maxYear;

	minMonth = (Word)(MonthCombo->ItemIndex + 1);
	minYear = YearSelect->Position;
	if( minMonth < 12 )
	{
		maxMonth = (Word)(minMonth + 1);
		maxYear = minYear;
	}
	else
	{
		maxMonth = 1;
		maxYear = (Word)(minYear + 1);
	}

	*minDate = EncodeDate( minYear, minMonth, 1 );
	maxDate = EncodeDate( maxYear, maxMonth, 1 );
	maxDate -= 1;

	DateTimePickerStart->Date = (int)*minDate;
	DateTimePickerEnd->Date = (int)maxDate;
}

void __fastcall TExportActivitiesDialog::ValueChange( TObject * )
{
	TDateTime	minDate;

	GetDateRange( &minDate );

	putFileNameAndMonth( ProjectCombo->Text, minDate );
}
//---------------------------------------------------------------------------
void TExportActivitiesDialog::GetSelectedDateRange( TDateTime *minDate, TDateTime *maxDate )
{
	*minDate = (int)(DateTimePickerStart->Date);
	*maxDate = (int)(DateTimePickerEnd->Date);
	*maxDate += 1;
}
//---------------------------------------------------------------------------




