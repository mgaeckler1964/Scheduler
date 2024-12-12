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

import java.io.*;
import java.util.*;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.Connector;

import at.gaeckler.scheduler.Task;
import at.gaeckler.scheduler.CurrentTime;

class mobileThread extends Thread implements RecordComparator
{
	MobileForm	theForm;
	String		url;

	public int compare( byte[] line1, byte[] line2 )
	{
		if( line1[0] < line2[0] )
			return RecordComparator.PRECEDES;
		if( line1[0] > line2[0] )
			return RecordComparator.FOLLOWS;

		if( line1[1] < line2[1] )
			return RecordComparator.PRECEDES;
		if( line1[1] > line2[1] )
			return RecordComparator.FOLLOWS;

		if( line1[2] < line2[2] )
			return RecordComparator.PRECEDES;
		if( line1[2] > line2[2] )
			return RecordComparator.FOLLOWS;

		return RecordComparator.EQUIVALENT;
	}
	mobileThread( MobileForm theForm, String url, String userName, String password )
	{
		super();
		this.theForm = theForm;
		this.url = url;
		if( !url.endsWith( "/" ) )
			this.url += '/';
		this.url += userName;
		this.url += '/';
		this.url += password;
	}
	byte [] concat( byte [] src1, byte [] src2, int offset )
	{
		int	i, j;
		int src1length = src1 != null ? src1.length : 0;
		byte [] newArray = new byte[src1length + src2.length ];

		if( src1 != null )
		{
			for( i = 0; i < src1.length; i++ )
				newArray[i] = src1[i];
		}
		for( j=0, i=offset; i<src2.length; j++, i++ )
			newArray[src1length+j] = src2[i];

		return newArray;
	}
	public void run()
	{
		RecordStore	theTasksStore = null;
		RecordStore	theActivitiesStore = null;
		String		errorLabel = "Transfer";
		String		errorText = "";
		String		userAgent = "Mobile Scheduler 1.0(";
		
		if( System.getProperty("java.vm.name") != null )
			userAgent += "VM:" + System.getProperty("java.vm.name") +";"; 
		if( System.getProperty("java.vm.vendor") != null )
			userAgent += "Vendor:" + System.getProperty("java.vm.vendor") +";"; 
		if( System.getProperty("java.vm.version") != null )
			userAgent += "Version:" + System.getProperty("java.vm.version") +";"; 

		if( System.getProperty("microedition.profiles") != null )
			userAgent += "Profile:" + System.getProperty("microedition.profiles") +";"; 
		if( System.getProperty("microedition.configuration") != null )
			userAgent += "Configuration:" + System.getProperty("microedition.configuration") +";"; 

		userAgent += ")";

		HttpConnection con = null;
		int	rc;
		try
		{
			errorLabel = "Hole Aufgaben: open http";
			con = (HttpConnection)Connector.open( url );
			if( con != null)
			{
				con.setRequestProperty( "User-Agent", userAgent );
				con.setRequestProperty( "Connection", "close" );

				errorLabel = "Hole Aufgaben: get Response";
				rc = con.getResponseCode();
				if (rc != HttpConnection.HTTP_OK)
				{
					errorText += "HTTP-Fehler beim Empfangen\n";
					errorText += con.getResponseMessage();
					errorText += "\n";
				}
				else
				{
					errorLabel = "Hole Aufgaben: delete store";
					if( Mobil.exists( MobileForm.TASKS ) )
						RecordStore.deleteRecordStore( MobileForm.TASKS );

					errorLabel = "Hole Aufgaben: open store";
					theTasksStore = RecordStore.openRecordStore( MobileForm.TASKS, true );

					errorLabel = "Hole Aufgaben: read data";
					boolean	first = true;
//					String	contentType = con.getType();
					String	line;

					InputStream	is = con.openInputStream();

					line = "";
					while( true )
					{
						int data = is.read();
						if( data != 10 && data != 13 && data >= 0 )
							line += (char)data;
						else if( !line.equals("") )
						{
							if( !first )
								theTasksStore.addRecord( line.getBytes(), 0, line.length() );
							first = false;
							line = "";
						}
						if( data < 0 )
							break;
					}

					theTasksStore.closeRecordStore();
					theTasksStore = null;
				}

				theForm.loadTasksCsv();
			}
			if( Mobil.exists( MobileForm.ACTIVITIES ) )
			{
				errorLabel = "Sende Tätigkeiten";
				con = (HttpConnection)Connector.open( url );
				if( con != null)
				{
					con.setRequestMethod(HttpConnection.POST);
					con.setRequestProperty("User-Agent", userAgent);
					con.setRequestProperty("Connection", "close");
					con.setRequestProperty("Content-Type",
										   "application/scheduler.activities");

					theActivitiesStore = RecordStore.openRecordStore( MobileForm.ACTIVITIES, false );
					RecordEnumeration theRecords =
						theActivitiesStore.enumerateRecords(null,this, false)
					;
					byte [] outBuffer = null;
					for( int i=0; i<theRecords.numRecords(); i++ )
					{
						byte[] rec = theRecords.nextRecord();

						outBuffer = concat( outBuffer, rec, 3 );
					}
					theActivitiesStore.closeRecordStore();
					theActivitiesStore = null;

					con.setRequestProperty(
						"Content-Length",
						Integer.toString(outBuffer.length)
					 );
					OutputStream os = con.openOutputStream();
					os.write( outBuffer );
					os.close();

					rc = con.getResponseCode();
					if (rc != HttpConnection.HTTP_OK)
					{
						errorText += "HTTP-Fehler beim Senden\n";
						errorText += con.getResponseMessage();
						errorText += "\n";
					}
					else
						RecordStore.deleteRecordStore( MobileForm.ACTIVITIES );

					InputStream	is = con.openInputStream();

					errorLabel = "Sendebericht";
					errorText = userAgent;
					errorText += "\n";
					String line = "";
					while( true )
					{
						int data = is.read();
						if( data != 10 && data != 13 && data >= 0 )
							line += (char)data;
						else if( !line.equals("") )
						{
							errorText += line;
							errorText += "\n";
							line = "";
						}
						if( data < 0 )
							break;
					}
				}
			}
			else
			{
				errorLabel = "Sendebericht";
				errorText = "Nichts zum Senden";
			}
		}
		catch( IOException e )
		{
			errorText = "Allgemeiner I/O Fehler\n";
			errorText += e.getMessage();

			System.err.println( errorText );
		}
		catch( SecurityException e )
		{
			errorText = "Keine Berechtigung\n";
			errorText += e.getMessage();
		}
		catch( RecordStoreFullException e )
		{
			errorText = "Record Store Voll Exception\n";
			errorText += e.getMessage();
		}
		catch( RecordStoreNotFoundException e )
		{
			errorText = "Record Store nicht gefunden.\n";
			errorText += e.getMessage();
		}
		catch( RecordStoreNotOpenException e )
		{
			errorText = "Record Store nicht geöffnet.\n";
			errorText += e.getMessage();
		}
		catch( InvalidRecordIDException e )
		{
			errorText = "Falsche ID im Record Store.\n";
			errorText += e.getMessage();
		}
		catch( RecordStoreException e )
		{
			errorText = "Record Store Exception.\n";
			errorText += e.getMessage();
		}
		theForm.countRecords();
		try
		{
			if( theTasksStore != null )
				theTasksStore.closeRecordStore();
			if( theActivitiesStore != null )
				theActivitiesStore.closeRecordStore();
		}
		catch( Throwable e )
		{
		}
		if( (errorText != null) && (!errorText.equals( "" )) )
		{
			System.err.println( errorText );
			Alert errAlert = new Alert( errorLabel, errorText, null, AlertType.ERROR );
			errAlert.setTimeout( 10000 );
			Display.getDisplay( Mobil.instance ).setCurrent( errAlert );
		}
	}
}

class TaskTimer extends TimerTask
{
	MobileForm	theForm;
	long		startTicks;

	TaskTimer( MobileForm mobileFrm )
	{
		super();
		theForm = mobileFrm;
		startTicks = System.currentTimeMillis();
	}
	public void run()
	{
		int		tmpTimer = (int)(System.currentTimeMillis()-startTicks)/1000;
		int seconds = tmpTimer % 60;
		tmpTimer /= 60;
		int minute = tmpTimer % 60;
		int hour = tmpTimer / 60;

		String newTimer =
			CurrentTime.formatInt( hour ) +
			":" +
			CurrentTime.formatInt( minute ) +
			":" +
			CurrentTime.formatInt( seconds ) +
			" - " +
			new CurrentTime().getLocaleTime();

		theForm.timeLabel.setText( null );
		theForm.timeLabel.setText( newTimer );

		theForm.delete(1);
		theForm.insert(1,theForm.timeLabel);
	}
}

public class MobileForm extends Form implements CommandListener
{
	public static final String TASKS = "Tasks.csv";
	public static final String ACTIVITIES = "activities.csv";
	public static final String CUR_ACTIVITY = "actTask.sav";
	public static final String LAST_ACTIVITY = "lastTask.sav";
	public static final String CONFIGURATION = "mobile.cfg";

	StringItem	taskLabel = new StringItem("", "");
	StringItem	timeLabel = new StringItem("", "");
	ChoiceGroup	taskChoice = new ChoiceGroup("", ChoiceGroup.EXCLUSIVE);
	Command		startCmd = new Command("Start", Command.OK, 1);
	Command		stopCmd = new Command("Stop", Command.STOP, 1);
	Command		transferCmd = new Command("Transfer", Command.SCREEN, 1);
	Command		setupCmd = new Command("Einstellungen", Command.ITEM, 1);

	Timer	executionTimer;
	Vector	taskList = new Vector();
	int		curActivity = -1;
	StringItem statusLabel = new StringItem("", "");

	String url, userName, password;

	public void loadConfig()
	{
		url = "";
		userName = "";
		password = "";

		try
		{
			if( Mobil.exists(CONFIGURATION) )
			{
				byte[] rec;
				String line, name, value;
				int assignmentPos;
				RecordStore theConfiguration = RecordStore.openRecordStore(
					MobileForm.CONFIGURATION, false
					);
				RecordEnumeration theRecs = theConfiguration.enumerateRecords(null, null, false);
				for (int i = 0; i < theConfiguration.getNumRecords(); i++)
				{
					rec = theRecs.nextRecord();
					line = new String(rec);
					assignmentPos = line.indexOf('=');
					if (assignmentPos > 0)
					{
						name = line.substring(0, assignmentPos);
						value = line.substring(assignmentPos + 1, line.length());
						if (name.equals("URL"))
							url = value;
						else if (name.equals("USER"))
							userName = value;
						else if (name.equals("PASSWORD"))
							password = value;
					}
				}
				theConfiguration.closeRecordStore();
			}

			if( url.equals("") || userName.equals("")  || password.equals("") )
				removeCommand( transferCmd );
			else if( executionTimer == null )
				addCommand( transferCmd );
		}
		catch( Throwable e )
		{
			e.printStackTrace();
		}
	}

	public MobileForm()
	{
		super("Mobile Zeiterfassung");

		try
		{
			jbInit();
			loadConfig();
			loadTasksCsv();
			countRecords();
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
		// add the command
		addCommand(transferCmd);
		addCommand(startCmd);
		addCommand(setupCmd);
		addCommand(new Command("Beenden", Command.EXIT, 1));
		taskLabel.setLabel("Aufgabe:");
		taskLabel.setLayout(Item.LAYOUT_2);
		taskLabel.setText("Pause");
		timeLabel.setLabel("Zeit:");
		timeLabel.setLayout(Item.LAYOUT_NEWLINE_BEFORE | Item.LAYOUT_2);
		timeLabel.setText("00:00:00");
		taskChoice.setLabel("Aufgaben: ");
		this.append(taskLabel);
		this.append(timeLabel);
		this.append(statusLabel);
		this.append(taskChoice);
		statusLabel.setLabel("Status");
		statusLabel.setLayout(Item.LAYOUT_NEWLINE_BEFORE | Item.LAYOUT_2);
		statusLabel.setText("xxx");
		statusLabel.setFont(Font.getFont(Font.FONT_STATIC_TEXT));
	}

	public void countRecords( )
	{
		int numTasks = 0;
		int numActivities = 0;
		try
		{
			RecordStore tasks = RecordStore.openRecordStore(TASKS, false);
			numTasks = tasks.getNumRecords();
			tasks.closeRecordStore();
		}
		catch( Throwable e )
		{
			e.printStackTrace();
		}
		try
		{
			RecordStore activities = RecordStore.openRecordStore(ACTIVITIES, false);
			numActivities = activities.getNumRecords();
			activities.closeRecordStore();
		}
		catch( Throwable e )
		{
			e.printStackTrace();
		}
		statusLabel.setText( Integer.toString(numTasks) + "/" + Integer.toString(numActivities) );
	}
	private void saveActivity( int newActivity, boolean stopClicked )
	{
		try
		{
			if( curActivity != newActivity )
			{
				RecordStore theActivitiesStore = RecordStore.openRecordStore(ACTIVITIES, true);
				int numRecs = theActivitiesStore.getNumRecords();
				String actTaskId;
				byte[] outBuf;
				String outLine;

				if( numRecs == 0 )
				{
					outBuf = "000ID,PROJECT,TITLE,START_TIME\n".getBytes();
					theActivitiesStore.addRecord(outBuf, 0, outBuf.length);
					numRecs++;
				}

				if (newActivity >= 0)
				{
					Task actTask = (Task) taskList.elementAt(newActivity);
					outLine = actTask.ID + ",\"" + actTask.project + "\",\"" +
						actTask.title + "\"";
					actTaskId = actTask.ID;
				}
				else
				{
					outLine = "0,-,-";
					actTaskId = null;
				}
				CurrentTime now = new CurrentTime();
				outLine += "," + now.getLocaleDateTime() + "\n";

				String	tmp = Integer.toHexString(numRecs);
				while( tmp.length() < 3 )
					tmp = "0" + tmp;
				outLine = tmp + outLine;

				outBuf = outLine.getBytes();
				theActivitiesStore.addRecord(outBuf, 0, outBuf.length);
				theActivitiesStore.closeRecordStore();

				if (actTaskId != null)
				{
					outBuf = actTaskId.getBytes();

					if( Mobil.exists(CUR_ACTIVITY) )
						RecordStore.deleteRecordStore(CUR_ACTIVITY);
					if( Mobil.exists(LAST_ACTIVITY) )
						RecordStore.deleteRecordStore(LAST_ACTIVITY);

					RecordStore theCurStore = RecordStore.openRecordStore(CUR_ACTIVITY, true);
					theCurStore.addRecord(outBuf, 0, outBuf.length);
					theCurStore.closeRecordStore();

					theCurStore = RecordStore.openRecordStore(LAST_ACTIVITY, true);
					theCurStore.addRecord(outBuf, 0, outBuf.length);
					theCurStore.closeRecordStore();
				}
				curActivity = newActivity;
			}
			if( stopClicked && Mobil.exists(CUR_ACTIVITY) )
			{
				RecordStore.deleteRecordStore(CUR_ACTIVITY);
			}
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		countRecords();
	}

	private void transferData()
	{
		try
		{
			mobileThread theThread = new mobileThread( this, url, userName, password );
			theThread.start();
		}
		catch( Throwable e )
		{
		}
	}
	public void commandAction(Command command, Displayable displayable)
	{
		if (command.getCommandType() == Command.EXIT)
		{
			saveActivity( -1, false );
			Mobil.quitApp();
		}
		else if (command.getCommandType() == Command.OK)
		{
			taskLabel.setLabel(null);
			taskLabel.setText(taskChoice.getString(taskChoice.getSelectedIndex()));
			timeLabel.setLabel(null);
			timeLabel.setText("00:00:00");
			delete(3);		// task choice
			removeCommand( startCmd );
			removeCommand( transferCmd );
			removeCommand( setupCmd );
			addCommand( stopCmd );
			executionTimer = new Timer();
			executionTimer.scheduleAtFixedRate(new TaskTimer( this ), 0, 1000 );

			saveActivity( taskChoice.getSelectedIndex(), false );
		}
		else if (command.getCommandType() == Command.STOP)
		{
			taskLabel.setLabel("Aufgabe:");
			taskLabel.setText("Pause");
			timeLabel.setLabel("Zeit:");
			timeLabel.setText("00:00:00");
			append(taskChoice);
			removeCommand( stopCmd );
			addCommand( startCmd );
			if( !url.equals("") && !userName.equals("")  && !password.equals("") )
				addCommand( transferCmd );
			addCommand( setupCmd );
			executionTimer.cancel();
			executionTimer = null;
			saveActivity( -1, true );
		}
		else if( command.getCommandType() == Command.SCREEN )
		{
			transferData();
		}
		else if( command.getCommandType() == Command.ITEM )
		{
			SetupDlg setup = new SetupDlg();
			Display.getDisplay(Mobil.instance).setCurrent(setup);
		}
	}

	public void loadTasksCsv() throws RecordStoreException, IOException
	{
		String	actTaskId;
		boolean	active = false;

		if( Mobil.exists( CUR_ACTIVITY ) )
		{
			RecordStore	theCurActivityStore = RecordStore.openRecordStore( CUR_ACTIVITY, true );
			RecordEnumeration	theRecs = theCurActivityStore.enumerateRecords(null, null, false );
			byte[]	inputBuffer;

			inputBuffer = theRecs.nextRecord();
			theCurActivityStore.closeRecordStore();

			actTaskId = new String( inputBuffer );

			active = true;
		}
		else if( Mobil.exists( LAST_ACTIVITY ) )
		{
			RecordStore	theLastActivityStore = RecordStore.openRecordStore( LAST_ACTIVITY, true );
			RecordEnumeration	theRecs = theLastActivityStore.enumerateRecords(null, null, false );
			byte[]	inputBuffer;

			inputBuffer = theRecs.nextRecord();
			theLastActivityStore.closeRecordStore();

			actTaskId = new String( inputBuffer );
		}
		else
			actTaskId = null;

		if( Mobil.exists(TASKS) )
		{
			RecordStore	theTasksStore = RecordStore.openRecordStore( TASKS, false );
			int		numRecs = theTasksStore.getNumRecords();
			int		length;
			String	theFieldValue, project, title, ID;
			byte[]	byteInputBuffer;
			char[]	charInputBuffer;
			char	curByte;

			taskList.removeAllElements();
			RecordEnumeration theRecs = theTasksStore.enumerateRecords( null, null, false );
			for( int i=0; i<numRecs; i++ )
			{
				byteInputBuffer = theRecs.nextRecord();
				length = byteInputBuffer.length;

				InputStreamReader isr = new InputStreamReader(
					new ByteArrayInputStream(byteInputBuffer), "ISO8859_1"
				);

				charInputBuffer = new char[length];
				isr.read(charInputBuffer,0,length);
				theFieldValue = project = title = ID = "";
				boolean	inString = false;
				int j=0;
				while( j<length )
				{
					curByte = charInputBuffer[j];
					if( (char)curByte == ',' && !inString )
					{
						if( project.equals("") )
							project = theFieldValue;
						else if( title.equals( "" ) )
							title = theFieldValue;
						else
							ID = theFieldValue;
						theFieldValue = "";
					}
					else if( (char)curByte == '"' )
						inString = !inString;
					else
						theFieldValue += (char)curByte;

					j++;
				}
				if( !theFieldValue.equals( "" ) )
				{
					if (project.equals(""))
						project = theFieldValue;
					else if (title.equals(""))
						title = theFieldValue;
					else
						ID = theFieldValue;
					theFieldValue = "";
				}
				if( !project.equals("")
				&& !title.equals("")
				&& !ID.equals("") )
				{
					taskList.addElement(new Task(project, title, ID));
				}
				theFieldValue = project = title = ID = "";
			}
			theTasksStore.closeRecordStore();
			int		i, selectedTaskIdx;

			taskChoice.deleteAll();
			selectedTaskIdx = -1;
			length = taskList.size();
			for( i=0; i<length; i++ )
			{
				Task curTask = (Task)taskList.elementAt(i);
				taskChoice.append(curTask.getLabel(), null );
				if( actTaskId != null && curTask.ID.equals(actTaskId) )
					selectedTaskIdx = i;
			}
			if( selectedTaskIdx >= 0 )
			{
				taskChoice.setSelectedIndex(selectedTaskIdx, true);
				if( active )
					commandAction( startCmd, this );
			}
		}
	}

}
