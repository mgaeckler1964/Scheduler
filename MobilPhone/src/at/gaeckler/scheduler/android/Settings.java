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

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class Settings extends Activity
{
	EditText	editTextUrl, editTextUser, editTextPassword;
	Button		buttonSave, buttonCancel;

	@Override
    public void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.settings);

		editTextUrl = (EditText)findViewById( R.id.editTextUrl );
		editTextUser = (EditText)findViewById( R.id.editTextUser );
		editTextPassword = (EditText)findViewById( R.id.editTextPassword );

        SharedPreferences settings = getSharedPreferences(MobileForm.CONFIGURATION, 0);
        editTextUrl.setText( settings.getString("url", "") );
        editTextUser.setText( settings.getString("user", "") );
        editTextPassword.setText( settings.getString("password", "") );

        buttonSave = (Button) findViewById(R.id.buttonSave);
		buttonSave.setOnClickListener
		(
			new View.OnClickListener()
			{
			
				public void onClick(View v)
				{
					saveSettings();
					finish();
				}
			}
		);
		buttonCancel = (Button) findViewById(R.id.buttonCancel);
		buttonCancel.setOnClickListener
		(
			new View.OnClickListener()
			{
			
				public void onClick(View v)
				{
					finish();
				}
			}
		);
        	
	}

	private void saveSettings( )
	{
		SharedPreferences settings = getSharedPreferences(MobileForm.CONFIGURATION, 0);
	    SharedPreferences.Editor editor = settings.edit();
	    editor.putString("url", editTextUrl.getText().toString());
	    editor.putString("user", editTextUser.getText().toString());
	    editor.putString("password", editTextPassword.getText().toString());
	
	    // Commit the edits!
	    editor.commit();
	}

}
