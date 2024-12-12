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

#ifndef MobileFrmH
#define MobileFrmH
//---------------------------------------------------------------------------
#include <time.h>

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TMobileForm : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TEdit *TaskFileEdit;
	TLabel *Label2;
	TEdit *ActivityFileEdit;
	TLabel *Label3;
	TComboBox *TasksComboBox;
	TButton *Button1;
	TButton *Button2;
	TButton *StartButton;
	TButton *StopButton;
	TButton *CloseButton;
	TSaveDialog *SaveDialog;
	TOpenDialog *OpenDialog;
	TTimer *ClockTimer;
	TLabel *ClockLabel;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall CloseButtonClick(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall StartButtonClick(TObject *Sender);
	void __fastcall StopButtonClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ClockTimerTimer(TObject *Sender);
	void __fastcall AppMinimize(TObject *Sender);
	void __fastcall AppWindowProc(TMessage &Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
	void				readTaskFile( void );
	int					currentTask;
	clock_t				startTime;
	NOTIFYICONDATA		IconData;
public:		// User declarations
	__fastcall TMobileForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMobileForm *MobileForm;
//---------------------------------------------------------------------------
#endif
