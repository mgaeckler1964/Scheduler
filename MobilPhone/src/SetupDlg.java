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

import javax.microedition.lcdui.*;
import javax.microedition.rms.*;

public class SetupDlg extends Form implements CommandListener
{
	public SetupDlg()
	{
		super("Setup");
		try
		{
			jbInit();
			if( Mobil.exists(MobileForm.CONFIGURATION) )
			{
				byte [] rec;
				String line, name, value;
				int		assignmentPos;
				RecordStore	theConfiguration = RecordStore.openRecordStore(
					MobileForm.CONFIGURATION, false
				);
				RecordEnumeration theRecs = theConfiguration.enumerateRecords(
					null, null, false
				);
				for( int i=0; i<theConfiguration.getNumRecords(); i++ )
				{
					rec = theRecs.nextRecord();
					line = new String( rec );
					assignmentPos = line.indexOf('=');
					if( assignmentPos > 0 )
					{
						name = line.substring(0, assignmentPos);
						value = line.substring(assignmentPos+1, line.length());
						if( name.equals( "URL" ) )
							textFieldUrl.setString(value);
						else if( name.equals( "USER" ) )
							textFieldUserName.setString(value);
						else if( name.equals( "PASSWORD" ) )
							textFieldPassword.setString(value);
					}
				}
				theConfiguration.closeRecordStore();
			}
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
	}

	private void jbInit() throws Exception
	{
		// Set up this Displayable to listen to command events
		setCommandListener(this);
		// add the Exit command
		addCommand(new Command("Speichern", Command.OK, 1));
		addCommand(new Command("Abbrechen", Command.CANCEL, 1));
		textFieldUrl.setLabel("Url");
		textFieldUrl.setMaxSize(1024);
		this.append(textFieldUrl);
		this.append(textFieldUserName);
		this.append(textFieldPassword);
		textFieldUserName.setLabel("Benutzer");
		textFieldUserName.setMaxSize(255);
		this.setTitle("Einstellungen");
		textFieldPassword.setLabel("Kennwort");
		textFieldPassword.setConstraints(TextField.ANY | TextField.PASSWORD);
		textFieldPassword.setMaxSize(255);
	}

	public void commandAction(Command command, Displayable displayable)
	{
		if (command.getCommandType() == Command.OK)
		{
			String	line;
			byte [] data;
			try
			{
				if (Mobil.exists(MobileForm.CONFIGURATION))
					RecordStore.deleteRecordStore(MobileForm.CONFIGURATION);

				RecordStore theConfiguration = RecordStore.openRecordStore(
					MobileForm.CONFIGURATION, true
				);

				line ="URL=" + textFieldUrl.getString();
				data = line.getBytes();
				theConfiguration.addRecord( data, 0, data.length );

				line ="USER=" + textFieldUserName.getString();
				data = line.getBytes();
				theConfiguration.addRecord( data, 0, data.length );

				line ="PASSWORD=" + textFieldPassword.getString();
				data = line.getBytes();
				theConfiguration.addRecord( data, 0, data.length );

				theConfiguration.closeRecordStore();
			}
			catch( Throwable e )
			{
				System.out.println(e.getMessage());
			}
			Mobil.instance.mobileForm.loadConfig();
			Display.getDisplay(Mobil.instance).setCurrent(Mobil.instance.mobileForm);
		}
		else if (command.getCommandType() == Command.CANCEL)
		{
			Display.getDisplay(Mobil.instance).setCurrent(Mobil.instance.mobileForm);
		}
	}

	TextField textFieldUrl = new TextField("", "", 15, TextField.URL);
	TextField textFieldUserName = new TextField("", "", 15, TextField.ANY);
	TextField textFieldPassword = new TextField("", "", 15, TextField.ANY);

}
