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
#include <vcl/registry.hpp>

#pragma hdrstop

#include "SetupDlg.h"
#include "MainFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSetupDialog *SetupDialog;
//---------------------------------------------------------------------------
__fastcall TSetupDialog::TSetupDialog(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSetupDialog::FormCreate(TObject *)
{
	std::auto_ptr<TRegistry>	reg(new TRegistry());

	if( reg->OpenKey( REGISTRY_KEY, false ) )
	{
		if( reg->ValueExists( "AutoActivity" ) )
			AutoActivityCheckBox->Checked = reg->ReadBool( "AutoActivity" );
		else
			AutoActivityCheckBox->Checked = false;

		if( reg->ValueExists( "Database" ) )
			DatabaseRadioGroup->ItemIndex = reg->ReadInteger( "Database" );
		else
			DatabaseRadioGroup->ItemIndex = 0;

		if( reg->ValueExists( "MobileServer" ) )
			CheckBoxMobileServer->Checked = reg->ReadBool( "MobileServer" );
		else
			CheckBoxMobileServer->Checked = false;

		if( reg->ValueExists( "ServerPort" ) )
		{
			int ServerPort = reg->ReadInteger( "ServerPort" );
			char buffer[32];
			sprintf( buffer, "%d", ServerPort );
			EditServerPort->Text = buffer;
		}
		else
			EditServerPort->Text = "";

		if( reg->ValueExists( "InfoMinimize" ) )
			CheckBoxInfoMinimize->Checked = reg->ReadBool( "InfoMinimize" );
		else
			CheckBoxInfoMinimize->Checked = false;

		reg->CloseKey();
	}
	else
	{
		AutoActivityCheckBox->Checked = false;
		DatabaseRadioGroup->ItemIndex = 0;
		CheckBoxMobileServer->Checked = false;
		EditServerPort->Text = "";
	}
}
//---------------------------------------------------------------------------
void __fastcall TSetupDialog::ButtonOKClick(TObject *)
{
	std::auto_ptr<TRegistry>	reg(new TRegistry());

	reg->OpenKey( REGISTRY_KEY, true );
	reg->WriteBool( "AutoActivity", AutoActivityCheckBox->Checked );
	reg->WriteInteger( "Database", DatabaseRadioGroup->ItemIndex );
	reg->WriteBool( "MobileServer", CheckBoxMobileServer->Checked );

	if( EditServerPort->Text > "" )
	{
		int ServerPort = atoi( EditServerPort->Text.c_str() );
		reg->WriteInteger( "ServerPort", ServerPort );
	}
	else
		reg->DeleteValue( "ServerPort" );

	reg->WriteBool( "InfoMinimize", CheckBoxInfoMinimize->Checked );

	reg->CloseKey();
}
//---------------------------------------------------------------------------
void __fastcall TSetupDialog::ButtonCancelClick(TObject *)
{
	std::auto_ptr<TRegistry>	reg(new TRegistry());

	if( reg->OpenKey( REGISTRY_KEY, false ) )
	{
		if( reg->ValueExists( "AutoActivity" ) )
			AutoActivityCheckBox->Checked = reg->ReadBool( "AutoActivity" );
		else
			AutoActivityCheckBox->Checked = false;

		if( reg->ValueExists( "Database" ) )
			DatabaseRadioGroup->ItemIndex = reg->ReadInteger( "Database" );
		else
			DatabaseRadioGroup->ItemIndex = 0;

		reg->CloseKey();
	}
	else
	{
		AutoActivityCheckBox->Checked = false;
		DatabaseRadioGroup->ItemIndex = 0;
	}
}
//---------------------------------------------------------------------------
