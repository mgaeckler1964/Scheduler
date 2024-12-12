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
#ifndef MainFrmH
#define MainFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <Db.hpp>
#include <DBTables.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <Buttons.hpp>
#include <ImgList.hpp>

#include <shellapi.h>

#include <gak/string.h>
#include <gak/vcl_tools.h>

//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu;
	TMenuItem *Datei1;
	TMenuItem *Ende1;
	TMenuItem *Hilfe1;
	TMenuItem *about;
	TMenuItem *Fenster1;
	TMenuItem *bereinander1;
	TMenuItem *Untereinander1;
	TMenuItem *Nebeneinander1;
	TTimer *theTimer;
	TStatusBar *StatusBar;
	TToolBar *ToolBar1;
	TToolButton *ToolButton1;
	TImageList *ImageList1;
	TMenuItem *FileUser;
	TMenuItem *N1;
	TQuery *activeUserQuery;
	TDatabase *theDatabase;
	TMenuItem *Projekte1;
	TMenuItem *N2;
	TMenuItem *Einstellungen1;
	TMenuItem *FileLogin;
	TMenuItem *Kennwortndern1;
	TMenuItem *N3;
	TMenuItem *Rechte1;
	TMenuItem *N4;
	void __fastcall Ende1Click(TObject *Sender);
	void __fastcall theTimerTimer(TObject *Sender);
	void __fastcall ToolButton1Click(TObject *Sender);
	void __fastcall FileUserClick(TObject *Sender);
	void __fastcall AppMinimize(TObject *Sender);
	void __fastcall AppWindowProc(TMessage &Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Projekte1Click(TObject *Sender);
	void __fastcall Einstellungen1Click(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall Kennwortndern1Click(TObject *Sender);
	void __fastcall FileLoginClick(TObject *Sender);
	void __fastcall StatusBarResize(TObject *Sender);
	void __fastcall Rechte1Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall aboutClick(TObject *Sender);
private:	// User declarations
	const gak::vcl::UserOrGroup		*actUser;
	int								databaseMode;	// auto, local, network
	NOTIFYICONDATA					iconData;

	void __fastcall AppMessage(tagMSG &Msg, bool &Handled);
	void login( void );
	int loadPermissions( int schedulerId );
public:		// User declarations
	__fastcall TMainForm(TComponent* Owner);

	double getWorkingTime( void ) const
	{
		return 40.0;
	}
	short int getWorkDaysPerWeek( void ) const
	{
		return 5;
	}
	double getWorkTimePerDay( void ) const
	{
		return getWorkingTime() / double(getWorkDaysPerWeek());
	}
	static void setDateFormats( void )
	{
		DateSeparator = '.';
		TimeSeparator = ':';
		ShortDateFormat = "dd/mm/yyyy";		// ensure 4 digit year
		LongDateFormat = "dd/mm/yyyy";		// ensure 4 digit year
		ShortTimeFormat = "hh:nn:ss";
		LongTimeFormat = "hh:nn:ss";
	}

	void openUser( int userId );
	const gak::vcl::UserOrGroup *getActUser( void ) const
	{
		return actUser;
	}
	void getUserById( int userId, gak::vcl::UserOrGroup *result ) const
	{
		gak::vcl::getUserById( theDatabase->DatabaseName, userId, result );
	}
};
//---------------------------------------------------------------------------
extern char *registryKey;
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif

