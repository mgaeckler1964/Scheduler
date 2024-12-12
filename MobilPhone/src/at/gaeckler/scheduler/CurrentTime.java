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

package at.gaeckler.scheduler;

import java.util.Calendar;

public class CurrentTime
{
	Calendar	currentDateTime;

	public CurrentTime()
	{
		currentDateTime = Calendar.getInstance();
	}
	static public String formatInt( int value )
	{
		String str = Integer.toString( value );

		if( value < 10 )
			str = "0" + str;

		return str;
	}
	public String getLocaleDateTime()
	{
		String	localDateTime;

		localDateTime = getLocaleDate() +
			" " +
			getLocaleTime()
		;

		return localDateTime;
	}
	public String getLocaleDate()
	{
		String	localDate;

		localDate = formatInt( currentDateTime.get(Calendar.DAY_OF_MONTH) ) +
			"." +
			formatInt( currentDateTime.get(Calendar.MONTH)+1 ) +
			"." +
			formatInt( currentDateTime.get(Calendar.YEAR) )
		;

		return localDate;
	}
	public String getLocaleTime()
	{
		String	localTime;

		localTime =
			formatInt(currentDateTime.get(Calendar.HOUR_OF_DAY) ) +
			":" +
			formatInt(currentDateTime.get(Calendar.MINUTE) ) +
			":" +
			formatInt(currentDateTime.get(Calendar.SECOND) )
		;

		return localTime;
	}
}

