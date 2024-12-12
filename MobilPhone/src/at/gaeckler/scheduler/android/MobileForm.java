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

package at.gaeckler.scheduler.android;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.Timer;
import java.util.TimerTask;
import java.util.Vector;

import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.rms.InvalidRecordIDException;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreFullException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.RecordStoreNotOpenException;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.Spinner;
import android.widget.TextView;

import at.gaeckler.scheduler.CurrentTime;
import at.gaeckler.scheduler.Task;

/*
 * ============================================================================================================
 * Classes that emulate JavaME
 * ============================================================================================================
 */

class ChoiceGroup
{
	Spinner 				theSpinner;
	ArrayAdapter<String>	dataAdapter;

	ChoiceGroup( Context ctx, Spinner newSpinner )
	{
		theSpinner = newSpinner;

		dataAdapter = new ArrayAdapter<String>(ctx, android.R.layout.simple_spinner_item );
		dataAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		theSpinner.setAdapter( dataAdapter );
	}

	public void append(String label, Object object)
	{
		dataAdapter.add(label);
	}

	public void deleteAll() 
	{
		dataAdapter.clear();
	}

	public void setSelectedIndex(int selectedTaskIdx, boolean b)
	{
		theSpinner.setSelection(selectedTaskIdx);
	}

	public void setVisibility(int visibility)
	{
		theSpinner.setVisibility(visibility);
		theSpinner.requestLayout();
		theSpinner.getParent().requestLayout();
	}

	public int getSelectedIndex()
	{
		// TODO Auto-generated method stub
		return theSpinner.getSelectedItemPosition();
	}

	public String getString(int selectedIndex)
	{
		return dataAdapter.getItem(selectedIndex);
	}
}

class AlertType
{
	public static final int ERROR = 0;
}
class Alert
{
	String	theErrorLabel, theErrorText;

	public Alert(String errorLabel, String errorText, Object object, int alertType )
	{
		theErrorLabel = errorLabel;
		theErrorText = errorText;
	}

	public void show( Context ctx )
	{
    	AlertDialog.Builder builder = new AlertDialog.Builder( ctx );
    	builder.setMessage(theErrorText)
    		   .setTitle(theErrorLabel)
    	       .setCancelable(false)
    	       .setNegativeButton("Fertig", new DialogInterface.OnClickListener() {
    	           public void onClick(DialogInterface dialog, int id) {
    	                dialog.cancel();
    	           }
    	       });
    	AlertDialog alert = builder.create();
    	alert.show();
	}

	public void setTimeout(int i)
	{
	}
}

class Display
{
	Context	ctx;

	Display( Context ctx )
	{
		this.ctx = ctx;
	}
	public static Display getDisplay(Context instance) 
	{
		return new Display( instance );
	}
	public void setCurrent(Alert errAlert)
	{
		errAlert.show( ctx );
	}
};

class StringItem
{
	TextView labelView;
	TextView stringView;
	
	public StringItem(TextView labelView, TextView stringView ) 
	{
		this.labelView = labelView;
		this.stringView = stringView;
	}
	public void setLabel( final String label )
	{
		Handler hndlr = labelView.getHandler();
		
		if( hndlr != null )
		{
			hndlr.post(
				new Runnable()
				{
					public void run()
					{
						if( label == null )
							labelView.setVisibility(View.GONE);
						else
						{
							labelView.setVisibility(View.VISIBLE);
							labelView.setText(label);
						}
					}
				}
			);
		}
	}
	public void setText( final String text )
	{
		Handler hndlr = stringView.getHandler();
		
		if( hndlr != null )
		{
			hndlr.post(
				new Runnable()
				{
					public void run()
					{
						if( text == null )
							stringView.setVisibility(View.GONE);
						else
						{
							stringView.setVisibility(View.VISIBLE);
							stringView.setText(text);
						}
					}
				}
			);
		}
	}
}
/*
 * ============================================================================================================
 * Classes that emulate JavaME END
 * ============================================================================================================
 */

/*
 * Classes from original source
 */

// this is from original Mobil.Java which is the entry point for a midlet, we need only the exists function
class Mobil
{
	public static Context instance;

	static boolean exists( String storeName )
	{
		String[]	stores = RecordStore.listRecordStores();
		if( stores != null )
			for( int i=0; i<stores.length; i++ )
				if( stores[i].equals( storeName ) )
					return true;

		return false;
	}
};

/*
==== From MIDlet =========================================================================================
*/
class mobileThread extends Thread // implements RecordComparator
{
	MobileForm	theForm;
	String		url;

/*
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
*/
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

/* 
 * this source is copied from the MIDlet:
 */
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

//		theForm.delete(1);
//		theForm.insert(1,theForm.timeLabel);
	}
	
	public long getStartTicks()
	{
		return startTicks;
	}
	public void setStartTicks( long newTicks )
	{
		startTicks = newTicks;
	}
}


/*
==== From MIDlet END =========================================================================================
*/

public class MobileForm extends Activity
{
	public static final String TASKS = "Tasks.csv";
	public static final String ACTIVITIES = "activities.csv";
	public static final String CUR_ACTIVITY = "actTask.sav";
	public static final String LAST_ACTIVITY = "lastTask.sav";
	public static final String CONFIGURATION = "mobile.cfg";
	Vector<Task>	taskList = new Vector<Task>();
	ScrollView		scrollView;
	ChoiceGroup		taskChoice;
	TextView		statusLabel;
	Button			startCmd;
	Button			stopCmd;
	StringItem		taskLabel;
	StringItem		timeLabel;
	int				curActivity = -1;
	long			curStartTicks = -1;

	String			url, userName, password;
	Timer			executionTimer = null;
	TaskTimer		myTaskTimer = null;

	void loadConfig()
	{
		SharedPreferences settings = getSharedPreferences(CONFIGURATION, 0);

		url = settings.getString("url", "");
		userName = settings.getString("user", "");
		password = settings.getString("password", "");
	}

	public MobileForm()
	{
		RecordStore.ctx = this;
		Mobil.instance = this;
	}
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        scrollView = (ScrollView)findViewById( R.id.scrollView );
        taskChoice = new ChoiceGroup( this, (Spinner)findViewById( R.id.taskChoice ) );
        statusLabel = (TextView)findViewById( R.id.statusLabel );
        startCmd = (Button)findViewById( R.id.startCmd );
        startCmd.setOnClickListener
		(
			new View.OnClickListener()
			{
			
				public void onClick(View v)
				{
					commandAction( v, null );
				}
			}
		);
        stopCmd = (Button)findViewById( R.id.stopCmd );
        stopCmd.setOnClickListener
		(
			new View.OnClickListener()
			{
			
				public void onClick(View v)
				{
					commandAction( v, null );
				}
			}
		);
		stopCmd.setVisibility( View.GONE );
        
        taskLabel = new StringItem(
            	(TextView)findViewById( R.id.taskLabel ),
            	(TextView)findViewById( R.id.taskText )
        );
        timeLabel = new StringItem(
            	(TextView)findViewById( R.id.timeLabel ),
            	(TextView)findViewById( R.id.timeText )
        );
		loadConfig();
		if(savedInstanceState != null )
			curStartTicks = savedInstanceState.getLong( "startTicks" );
		else
			curStartTicks = 0;

		try
		{
			loadTasksCsv();
		}
		catch( Throwable e )
		{
			e.printStackTrace();
			
		}
		countRecords();
    }
    @Override
    public boolean onCreateOptionsMenu( android.view.Menu menu )
    {
    	MenuInflater inflater = getMenuInflater();
    	inflater.inflate(R.menu.mz_menu, menu);
    	
    	return super.onCreateOptionsMenu(menu);
    }
    @Override
    public boolean onPrepareOptionsMenu (android.view.Menu menu)
    {
		MenuItem	transferMenu = menu.findItem(R.id.itemTransfer);
		MenuItem	settingsMenu = menu.findItem(R.id.itemSettings);

		loadConfig();
		if( url.length() > 0 && userName.length() > 0 && password.length() > 0 && executionTimer == null )
			transferMenu.setVisible(true);
		else
			transferMenu.setVisible(false);
    	
		if( executionTimer == null )
			settingsMenu.setVisible(true);
		else
			settingsMenu.setVisible(false);
    	
    	
    	return true;
    }
    @Override
    public boolean onOptionsItemSelected( MenuItem item )
    {
    	int	itemId = item.getItemId();
    	switch( itemId )
    	{
    	case R.id.itemExit:
    		if( executionTimer != null )
    		{
    			executionTimer.cancel();
    			executionTimer.purge();
    			executionTimer = null;
    			myTaskTimer = null;
    			saveActivity( -1, false );
    		}

    		finish();
    		break;
    		
    	case R.id.itemTransfer:
    		if( curActivity == -1 )
    		{
    			loadConfig();
    			if( url.length() > 0 && userName.length() > 0 && password.length() > 0 )
    				transferData();
    		}
    		break;
    		
    	case R.id.itemSettings:
        	Intent intent = new Intent( this, Settings.class );
        	startActivity( intent );
        	break;
    	}
    	
    	return super.onOptionsItemSelected(item);
    }
	protected void  onSaveInstanceState (Bundle outState)
	{
		if( executionTimer != null && myTaskTimer != null )
		{
			outState.putInt("curActivity", curActivity );
			outState.putLong("startTicks", myTaskTimer.getStartTicks() );
		}
	}
/*
    @Override
    public void onPause()
    {
    	
		if( false && executionTimer != null )
		{
			executionTimer.cancel();
			executionTimer.purge();
			executionTimer = null;
//			saveActivity( -1, true );
			
		}
		super.onPause();
    }
*/
    void commandAction( View button, MobileForm dummy )
    {
    	if( button.equals( startCmd ) )
    	{
    		if( curActivity == -1 )
    		{
				taskLabel.setLabel(null);
				taskLabel.setText(taskChoice.getString(taskChoice.getSelectedIndex()));
				timeLabel.setLabel(null);
				timeLabel.setText("00:00:00");
				taskChoice.setVisibility( View.GONE );
				startCmd.setVisibility( View.GONE );
				stopCmd.setVisibility( View.VISIBLE );
				executionTimer = new Timer();
				myTaskTimer = new TaskTimer( this );
				if( curStartTicks > 0 )
				{
					myTaskTimer.setStartTicks( curStartTicks );
					curStartTicks = 0;
				}
				executionTimer.scheduleAtFixedRate(myTaskTimer, 0, 1000 );
	
				saveActivity( taskChoice.getSelectedIndex(), false );
				scrollView.requestLayout();
    		}
    	}
    	else if( button.equals(stopCmd))
    	{
    		if( curActivity >= 0 )
    		{
				taskLabel.setLabel("Aufgabe:");
				taskLabel.setText("Pause");
				timeLabel.setLabel("Zeit:");
				timeLabel.setText("00:00:00");
	
				taskChoice.setVisibility( View.VISIBLE );
				startCmd.setVisibility( View.VISIBLE );
				stopCmd.setVisibility( View.GONE );
	
				executionTimer.cancel();
				executionTimer.purge();
				executionTimer = null;
				myTaskTimer = null;
				saveActivity( -1, true );
				scrollView.requestLayout();
    		}
    	}
    	
    }
/*
==== From MIDlet =========================================================================================
*/
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

    /* 
     * this source is copied from the MIDlet:
     */
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
	private void transferData()
	{
		try
		{
			mobileThread theThread = new mobileThread( this, url, userName, password );
			/*
			 * this thread adds items to the task list combo box which is not allowed
			 * therefore we start the thread function immediately
			 */
//			theThread.start();
			theThread.run();
		}
		catch( Throwable e )
		{
		}
	}
/*
==== From MIDlet End ========================================================================================
*/
}