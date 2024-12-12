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
import java.io.*;
import java.util.*;
import com.siemens.mp.io.File;

class CurrentTime extends Calendar
{
	CurrentTime()
	{
		super();
		this.setTimeInMillis(System.currentTimeMillis());
	}
	String getLocaleDateTime()
	{
		String	localDateTime;

		localDateTime = getLocaleDate() +
			" " +
			getLocaleTime()
		;

		return localDateTime;
	}
	String getLocaleDate()
	{
		String	localDate;

		localDate = new Integer( this.get(DAY_OF_MONTH) ).toString() +
			"." +
			new Integer( this.get(MONTH)+1 ).toString() +
			"." +
			new Integer( this.get(YEAR) ).toString()
		;

		return localDate;
	}
	String getLocaleTime()
	{
		String	localTime;

		localTime =
			new Integer( this.get(HOUR_OF_DAY) ).toString() +
			":" +
			new Integer( this.get(MINUTE) ).toString() +
			":" +
			new Integer( this.get(SECOND) ).toString()
		;

		return localTime;
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
		Integer seconds = new Integer( tmpTimer % 60);
		tmpTimer /= 60;
		Integer minute = new Integer( tmpTimer % 60 );
		Integer hour = new Integer( tmpTimer / 60 );

		String newTimer =
			hour.toString() +
			":" +
			minute.toString() +
			":" +
			seconds.toString() +
			" - " +
			new CurrentTime().getLocaleTime();

		theForm.timeLabel.setText( null );
		theForm.timeLabel.setText( newTimer );

		theForm.delete(1);
		theForm.insert(1,theForm.timeLabel);
	}
}
class Task
{
	String	project;
	String	title;
	String	ID;

	public Task( String theProject, String theTitle, String theID )
	{
		project = theProject;
		title = theTitle;
		ID = theID;
	}
	public String  getLabel()
	{
		String	theLabel = project + ": " + title;
/*
		if( theLabel.length() > 13 )
			theLabel = theLabel.substring(0,13);
*/
		return theLabel;
	}
}

public class MobileForm extends Form implements CommandListener
{
	Vector	taskList = new Vector();
	int		curActivity = -1;

	public MobileForm()
	{
		super("Mobile Zeiterfassung");
		try
		{
			jbInit();
			loadTasksCsv();
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
		addCommand(new Command("Beenden", Command.EXIT, 1));
		addCommand(startCmd);
		taskLabel.setLabel("Aufgabe:");
		taskLabel.setText("Pause");
		timeLabel.setLabel("Zeit:");
		timeLabel.setText("00:00:00");
		taskChoice.setLabel("Aufgaben: ");
		this.append(taskLabel);
		this.append(timeLabel);
		this.append(taskChoice);
	}

	private void saveActivity( int newActivity, boolean stopClicked )
	{
		try
		{
			if( curActivity != newActivity )
			{
				File file = new File();
				int fd = file.open("Activity.csv");
				int length = file.length(fd);
				String actTaskId;
				byte[] outBuf;
				String outLine;

				if (length == 0)
				{
					outBuf = "ID,PROJECT,TITLE,START_TIME\r\n".getBytes();
					file.write(fd, outBuf, 0, outBuf.length);
				}
				else
					file.seek(fd, length);

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
				outLine += "," + now.getLocaleDateTime() + "\r\n";
				outBuf = outLine.getBytes();
				file.write(fd, outBuf, 0, outBuf.length);
				file.close(fd);
				if (actTaskId != null)
				{
					fd = file.open("actTask.sav");
					outBuf = actTaskId.getBytes();
					file.write(fd, outBuf, 0, outBuf.length);
					file.close(fd);
				}
				curActivity = newActivity;
			}
			if( stopClicked && File.exists("actTask.sav") >= 0 )
				File.delete("actTask.sav");
		}
		catch(Exception e)
		{
			e.printStackTrace();
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
			delete(2);		// task choice
			removeCommand( startCmd );
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
			executionTimer.cancel();
			executionTimer = null;
			saveActivity( -1, true );
		}
	}

	private void loadTasksCsv() throws IOException
	{
		String	actTaskId;
		if( File.exists( "actTask.sav" ) >= 0 )
		{
			File	file = new File();
			int		fd = file.open( "actTask.sav" );
			int		length = file.length( fd );
			byte[]	inputBuffer = new byte[length];

			file.read( fd, inputBuffer, 0, length );
			file.close( fd );

			actTaskId = new String( inputBuffer );
		}
		else
			actTaskId = null;

		if (File.exists("Tasks.csv") >= 0)
		{
			File	file = new File();
			int		fd = file.open( "Tasks.csv" );
			int		length = file.length(fd);
			byte[]	byteInputBuffer = new byte[length];
			char[]	charInputBuffer = new char[length];
			String	theFieldValue, project, title, ID;
			char	curByte;

			file.read( fd, byteInputBuffer, 0, length );
			file.close( fd );
			int		i, selectedTaskIdx;

			InputStreamReader isr = new InputStreamReader(
				new ByteArrayInputStream(byteInputBuffer), "ISO8859_1"
			);

			isr.read(charInputBuffer,0,length);

			// skip first line
			i=0;
			while( i<length )
			{
				if( charInputBuffer[i] == 13 || charInputBuffer[i] == 10 )
					break;
				i++;
			}

			theFieldValue = project = title = ID = "";
			boolean	inString = false;
			while( i<length )
			{
				curByte = charInputBuffer[i];
				if( curByte == 13 || curByte == 10 )	// line delimiter
				{
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
						theFieldValue = project = title = ID = "";
					}
				}
				else if( (char)curByte == ',' && !inString )
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

				i++;
			}

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
				commandAction( startCmd, this );
			}
		}
	}

	StringItem	taskLabel = new StringItem("", "");
	StringItem	timeLabel = new StringItem("", "");
	ChoiceGroup	taskChoice = new ChoiceGroup("", ChoiceGroup.EXCLUSIVE);
	Command		startCmd = new Command("Start", Command.OK, 1);
	Command		stopCmd = new Command("Stop", Command.STOP, 1);

	Timer	executionTimer;
}
