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

#include <shellapi.h>

#include <gak/fieldSet.h>
#include <gak/csv.h>
#include <gak/vcl_tools.h>
#include <gak/stringStream.h>
#include <gak/directory.h>

#pragma hdrstop

#include "DataUnit.h"
#include "ActivityFrm.h"
#include "ExportActivitiesDlg.h"
#include "MainFrm.h"
#include "SetupDlg.h"
#include "PermissionsFrm.h"

//---------------------------------------------------------------------------
using namespace gak;
using namespace vcl;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TActivityForm *ActivityForm;
gak::net::SocketServer<SchedulerServer> theActivitiesServer;

//---------------------------------------------------------------------------

static void exportTasksToStream( std::ostream &out, int userID )
{
	long	theId;
	STRING	project, title;
	TQuery	*datesQuery = new TQuery( Application );

	datesQuery->DatabaseName = "SchedulerDB";

	datesQuery->SQL->Clear();
	datesQuery->SQL->Add(	"select schedule.*, project.name as ProjectName "
							"from schedule "
							"left OUTER JOIN Project "
							"on (Project.ID = Schedule.Project) "
							"where (Schedule.recurringid is null or Schedule.recurringid <= 0) and "
									"(Schedule.estEffort is not null and Schedule.estEffort > 0) and "
									"Schedule.userId = :theUser "
							"order by Schedule.enddate, Schedule.startdate" );
	datesQuery->Params->Items[0]->AsInteger = userID;
	datesQuery->Open();

	out << "\"PROJECT\","
				"\"TITLE\","
				"\"ID\"\n";

	for( datesQuery->Open(); !datesQuery->Eof; datesQuery->Next() )
	{
		theId = datesQuery->FieldByName( "ID" )->AsInteger;

		project = datesQuery->FieldByName( "ProjectName" )->AsString.c_str();
		title = datesQuery->FieldByName( "Title" )->AsString.c_str();

		out << '"' << project << "\",\""
				<< title << "\",\""
				<< theId << "\"\n";
	}
	datesQuery->Close();

	delete datesQuery;
}

//---------------------------------------------------------------------------
int SchedulerServer::handleGetRequest( const STRING &url )
{
	T_STRING		tmpUrl = url + (size_t)1;
	STRING			userName = tmpUrl.getFirstToken( "/" );
	STRING			password = tmpUrl.getNextToken();

	UserOrGroup		theUser;

	loginUser(
		"SchedulerDB",
		(const char*)userName,
		(const char*)password,
		&theUser
	);

	if( !theUser.ID )
	{
/*@*/	return 401;
	}
	else
	{
		setStatusCode( 200 );
		setContentType( "application/scheduler.tasks" );

		sendResponse();

		std::ostream out( static_cast<std::streambuf*>(this) );

		out << "\r\n";

		exportTasksToStream( out, theUser.ID );

		out.flush();
	}

	return 0;
}

int SchedulerServer::handlePostRequest( const STRING &url )
{
	T_STRING		tmpUrl = url + (size_t)1;
	STRING			userName = tmpUrl.getFirstToken( "/" );
	STRING			password = tmpUrl.getNextToken();

	UserOrGroup		theUser;

	loginUser(
		"SchedulerDB",
		(const char*)userName,
		(const char*)password,
		&theUser
	);

	if( !theUser.ID )
	{
/*@*/	return 401;
	}
	else
	{
		TActivityForm *ActivityForm		= NULL;

		for( int i=0; i<MainForm->MDIChildCount; i++ )
		{
			if( !ActivityForm )
			{
				ActivityForm = dynamic_cast<TActivityForm *>(MainForm->MDIChildren[i]);
				if( ActivityForm && ActivityForm->getUserId() != theUser.ID )
					ActivityForm = NULL;
			}
		}

		if( !ActivityForm )
		{
/*@*/		return 404;
		}

		std::istream csvFile( static_cast<std::streambuf*>(this) );

		STRING 			theBody;
		ioSTRINGstream	theStream( theBody );
		while( !csvFile.eof() )
		{
			int c = csvFile.get();
			if( c >= 0 )
				theStream.put( c );
		}
		theStream.flush();
		theStream.clear();
		theStream.seekg( 0 );

		STRING fileName;
		STRING fileNameTemplate = getenv( "TEMP" );
		if( !fileNameTemplate.endsWith( DIRECTORY_DELIMITER_STRING ) )
			fileNameTemplate += DIRECTORY_DELIMITER;
		fileNameTemplate += userName;

		size_t i=0;
		while( true )
		{
			fileName = fileNameTemplate;
			fileName += formatNumber( i );
			fileName += ".csv";
			if( !exists( fileName ) )
			{
				break;
			}
			i++;
		}
		std::ofstream	theFile( fileName );
		while( !theStream.eof() )
		{
			int c = theStream.get();
			if( c >= 0 )
				theFile.put( c );
		}
		theFile.close();

		theStream.clear();
		theStream.seekg( 0 );
		STRING stats = ActivityForm->importActivitiesFromStream( theStream );
		stats += "\nDaten gespeichert in ";
		stats += fileName;

		response.puts( stats );

		return 200;
	}
}

//---------------------------------------------------------------------------
class OneDay
{
	public:
	TDateTime	startTime,
				endTime;
	double		workTime;
	FieldSet	theTasks;

	OneDay()
	{
		workTime = 0.0;
	}
};
//---------------------------------------------------------------------------
std::ostream & operator << ( std::ostream &out, OneDay &data )
{
	size_t	i, numTasks;

	numTasks = data.theTasks.getNumFields();

	for( i=0; i<numTasks; i++ )
	{
		if( i )
			out << "; ";

		out << data.theTasks[i].getKey() <<
			" (" << (double)(data.theTasks[i].getValue()) << " h)";
	}
	return out;
}
//---------------------------------------------------------------------------
__fastcall TActivityForm::TActivityForm(TComponent* Owner)
	: TForm(Owner)
{
	userId = 0;
	currentActivityId = -1;
}
//---------------------------------------------------------------------------
void TActivityForm::RestartActivity( bool gotoFirst )
{
	if( gotoFirst )
	{
		ActivityQuery->Close();
		ActivityQuery->Open();
	}
	else
	{
		int	id = ActivityQueryID->AsInteger;
		ActivityQuery->Close();

		for( ActivityQuery->Open();
			id != ActivityQueryID->AsInteger && !ActivityQuery->Eof;
			ActivityQuery->Next() )
			;
	}
}

//---------------------------------------------------------------------------

void TActivityForm::deleteActivities(
	const AnsiString	&selProject,
	TDateTime			minDate,
	TDateTime 			maxDate
)
{
	int			selProjectId = 0;
	TQuery		*delSql = new TQuery( this );

	delSql->DatabaseName = "SchedulerDB";

	if( selProject != "Alle" )
	{
		delSql->SQL->Add(
			"select project.id "
			"from project "
			"where project.name = :selProject"
		);
		delSql->ParamByName( "selProject" )->AsString = selProject;
		delSql->Open();
		if( !delSql->Eof )
			selProjectId = delSql->Fields->Fields[0]->AsInteger;
		delSql->Close();
		delSql->SQL->Clear();
	}

	delSql->SQL->Add(
		"delete from activities "
		"where  activities.startTime >= :minDate "
		"and    activities.endTime < :maxDate "
		"and    activities.task in ( "
			"select schedule.id "
			"from   schedule "
			"where  schedule.userId = :theUser "
	);
	if( selProject != "Alle" )
	{
		delSql->SQL->Add(
			"and schedule.project = :selProject "
		);
	}
	delSql->SQL->Add( ")" );

	delSql->ParamByName( "theUser" )->AsInteger = userId;
	delSql->ParamByName( "minDate" )->AsDateTime = minDate;
	delSql->ParamByName( "maxDate" )->AsDateTime = maxDate;
	if( selProject != "Alle" )
		delSql->ParamByName( "selProject" )->AsInteger = selProjectId;

	delSql->ExecSQL();

	delete delSql;
}
//---------------------------------------------------------------------------
double TActivityForm::doExportProtocol(
	std::ofstream		&fp,
	const AnsiString	&selProject,
	unsigned short		roundValue,
	TDateTime			minDate,
	TDateTime			maxDate
)
{
	bool				transferFlag = ExportActivitiesDialog->TransferCheckBox->Checked;
	bool				showYear, showMonth, needProjectExpectation = false;
	AnsiString			monthStr = ExportActivitiesDialog->MonthCombo->Text,
						Transfered;
	TDateTime			startTime, endTime, lastDate = maxDate-1;
	AnsiString			actProject, task,
						startTimeStr, endTimeStr;
	unsigned short		hour, minute, second, milli;
	Word				year, month, dayOfMonth;
	Word				year2, month2;
	double				previousStart = 0.0;
	int					projectId = 0;
	size_t				dayOfPeriod, numDays = (int)maxDate-(int)minDate;
	OneDay				*thisPeriod = new OneDay[numDays];
	FieldSet			theTasks, theTasksForTransfer;
	double				actualEffort, estimatedEffort, diffEffort,
						totalEstimated, totalNewEstimated, totalEffort,
						totalDifference, totalEffortThisMonth,
						sumTaskTime;

	static const char	*daysOfWeek[] = { "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa" };
	size_t              dayOfWeek = minDate.DayOfWeek()-1;

	static TQuery		*ScheduleQuery = NULL;
	static TQuery		*UpdateTransferedQuery = NULL;

	minDate.DecodeDate( &year, &month, &dayOfMonth );
	lastDate.DecodeDate( &year2, &month2, &dayOfMonth );
	showYear = year != year2;
	showMonth = showYear || month != month2;

	if( !ScheduleQuery )
	{
		ScheduleQuery = new TQuery( this );
		ScheduleQuery->SQL->Add(
			"select PROJECT, TITLE, ACTEFFORT, ESTEFFORT, EFFORTUNIT, PERCENTCOMPLETED "
			"from	schedule "
			"where   (RECURRINGID is null or RECURRINGID <= 0) and "
					"(ESTEFFORT is not null and ESTEFFORT > 0) and "
					"USERID = :theUser" );
		ScheduleQuery->DatabaseName = "SchedulerDB";
		//ScheduleQuery->UniDirectional = true;
		ScheduleQuery->RequestLive = true;
	}

	if( transferFlag && !UpdateTransferedQuery )
	{
		UpdateTransferedQuery = new TQuery( this );
		UpdateTransferedQuery->SQL->Add(
			"update	activities "
			"set	Transfered = :Transfered "
			"where	id = :id "
		);
		UpdateTransferedQuery->DatabaseName = "SchedulerDB";
	}

	fp <<	"\"Stundenabrechnung\"\n"
			"\n"
			"\"Projekt:\"" << ListSeparator << selProject.c_str() << "\n";
	if( showMonth )
		fp << "\"Zeitraum:\"" << ListSeparator <<  minDate.DateString().c_str() << '-' << lastDate.DateString().c_str() << '\n';
	else
		fp <<	"\"Monat:\"" << ListSeparator << monthStr.c_str() << ' ' << ExportActivitiesDialog->YearSelect->Position << '\n';

	/*
		========================================================================
		collect data from activity table
		========================================================================
	*/
	ActivityQuery->First();

	while( !ActivityQuery->Eof )
	{
		startTime = ActivityQueryStartTime->AsDateTime;
		actProject = ActivityQueryprojectName->AsString;
		endTime = ActivityQueryEndTime->AsDateTime;
		Transfered = ActivityQueryTransfered->AsString;

		if( roundValue )
		{
			startTime.DecodeTime( &hour, &minute, &second, &milli );
			startTime.DecodeDate( &year, &month, &dayOfMonth );
			if( minute % roundValue )
			{
				minute /= roundValue;
				minute *= roundValue;
			}
			startTime = EncodeDate(year,month,dayOfMonth) +
						EncodeTime(hour,minute,0,0);

			endTime.DecodeTime( &hour, &minute, &second, &milli );
			endTime.DecodeDate( &year, &month, &dayOfMonth );
			if( second || milli )
				minute++;
			if( minute % roundValue )
			{
				minute /= roundValue;
				minute++;
				minute *= roundValue;
			}
			if( minute >= 60 )
			{
				minute -= (unsigned short)60;
				hour++;
			}
			if( hour >= 24 )
			{
				hour -= (unsigned short)24;
				endTime =	EncodeDate(year,month,dayOfMonth) +
							EncodeTime(hour,minute,0,0);
				endTime += 1;
			}
			else
				endTime =	EncodeDate(year,month,dayOfMonth) +
							EncodeTime(hour,minute,0,0);

			if( previousStart && previousStart < (double)endTime )
				endTime = previousStart;
			previousStart = startTime;
		}

		if( selProject == actProject
		&&  startTime >= minDate
		&&  startTime < maxDate )
		{
			if( !projectId )	// project not yet known
				projectId = ActivityQueryprojectID->AsInteger;

//			startTime.DecodeDate( &year, &month, &dayOfMonth );
//			dayOfMonth--;

			dayOfPeriod = (int)startTime - (int)minDate;

			task = ActivityQuerytitle->AsString;
			if( thisPeriod[dayOfPeriod].workTime <= 0 )
			{
				thisPeriod[dayOfPeriod].startTime = startTime;
				thisPeriod[dayOfPeriod].endTime = endTime;

				thisPeriod[dayOfPeriod].workTime = ((double)endTime-(double)startTime)*24;
			}
			else
			{
				if( thisPeriod[dayOfPeriod].startTime > startTime )
					thisPeriod[dayOfPeriod].startTime = startTime;
				if( thisPeriod[dayOfPeriod].endTime < endTime )
					thisPeriod[dayOfPeriod].endTime = endTime;

				thisPeriod[dayOfPeriod].workTime += ((double)endTime-(double)startTime)*24;
			}

			sumTaskTime = thisPeriod[dayOfPeriod].theTasks[task.c_str()];
			sumTaskTime += ((double)endTime-(double)startTime)*24;
			thisPeriod[dayOfPeriod].theTasks.updateField( task.c_str(), sumTaskTime );

			sumTaskTime = theTasks[task.c_str()];
			sumTaskTime += ((double)endTime-(double)startTime)*24;
			theTasks.updateField( task.c_str(), sumTaskTime );

			if( !Transfered.Length() )
			{
				sumTaskTime = theTasksForTransfer[task.c_str()];
				sumTaskTime += ((double)endTime-(double)startTime)*24;
				theTasksForTransfer.updateField( task.c_str(), sumTaskTime );
			}
			if( transferFlag && !Transfered.Length() )
			{
				UpdateTransferedQuery->ParamByName( "id" )->AsInteger = ActivityQueryID->AsInteger;
				UpdateTransferedQuery->ParamByName( "transfered" )->AsString = "X";

				UpdateTransferedQuery->ExecSQL();
			}
		}
		ActivityQuery->Next();
	}

	/*
		========================================================================
		write day report
		========================================================================
	*/
	fp <<	"\n"
			"\"Tag\"" << ListSeparator << "\"Anfang\"" << ListSeparator << "\"Ende\"" << ListSeparator << "\"Pause(h)\"" << ListSeparator << "\"Zeit(h)\"" << ListSeparator << "\"Tätigkeit\"\n";

	totalEffortThisMonth = 0.0;
	for( dayOfPeriod = 0; dayOfPeriod<numDays; dayOfPeriod++ )
	{
		if( thisPeriod[dayOfPeriod].workTime > 0.0 )
		{
			totalEffortThisMonth += thisPeriod[dayOfPeriod].workTime;

			thisPeriod[dayOfPeriod].startTime.DecodeDate( &year, &month, &dayOfMonth );

			startTimeStr = thisPeriod[dayOfPeriod].startTime.FormatString( "hh:nn" );
			endTimeStr = thisPeriod[dayOfPeriod].endTime.FormatString( "hh:nn" );
			fp	<< '"' << daysOfWeek[dayOfWeek%7] << ' '
				<< dayOfMonth << ". ";
			if( showMonth )
				 fp << month << ". ";
			if( showYear )
				fp << year;

			fp	<< '"' << ListSeparator << '"' << startTimeStr.c_str()
				<< '"' << ListSeparator << '"' << endTimeStr.c_str() << '"' << ListSeparator
				<< (((double)thisPeriod[dayOfPeriod].endTime-(double)thisPeriod[dayOfPeriod].startTime)*24 - thisPeriod[dayOfPeriod].workTime)
				<< ListSeparator
				<< thisPeriod[dayOfPeriod].workTime
				<< ListSeparator << '"' << thisPeriod[dayOfPeriod] << "\"\n";
		}
		dayOfWeek += 1;
	}
	fp << "\"Summe:\"" << ListSeparator << ListSeparator << ListSeparator << ListSeparator << totalEffortThisMonth << "\n";

	totalEstimated =
		totalEffort =
		totalDifference =
		totalEffortThisMonth = 0.0;

	/*
		========================================================================
		write month report
		========================================================================
	*/
	fp <<	"\n"
			"\"Projektanalyse\"\n"
			"\n"
			"\"Soll\"" << ListSeparator << "\"Ist\"" << ListSeparator << "\"Diff.\"" << ListSeparator << "\"Frei(%)\"" << ListSeparator << "\"Monat\"" << ListSeparator << "\"Aufgabe\"\n";

	ScheduleQuery->ParamByName( "theUser" )->AsInteger = userId;
	ScheduleQuery->Open();
	while( !ScheduleQuery->Eof )
	{
		if( ScheduleQuery->Fields->Fields[0]->AsInteger == projectId )
		{
			if( !ScheduleQuery->Fields->Fields[5]->IsNull )
				needProjectExpectation = true;
			AnsiString	taskName = ScheduleQuery->Fields->Fields[1]->AsString;

			actualEffort = ScheduleQuery->Fields->Fields[2]->AsFloat +
				(double)theTasksForTransfer[taskName.c_str()]
			;

			estimatedEffort = ScheduleQuery->Fields->Fields[3]->AsFloat;
			if( ScheduleQuery->Fields->Fields[4]->AsString == "d" )
				estimatedEffort *= 8.0;
			totalEstimated += estimatedEffort;

			diffEffort = estimatedEffort - actualEffort;

			totalEffort += actualEffort;
			totalDifference += diffEffort;
			totalEffortThisMonth += (double)(theTasks[taskName.c_str()]);

			fp << estimatedEffort << ListSeparator
				<< actualEffort << ListSeparator
				<< diffEffort << ListSeparator
				<< (diffEffort/estimatedEffort) *100 << ListSeparator
				<< (double)(theTasks[taskName.c_str()]) << ListSeparator << '"'
				<< taskName.c_str() << "\"\n";

			theTasks.updateField( taskName.c_str(), actualEffort );

			if( transferFlag )
			{
				ScheduleQuery->Edit();
				ScheduleQuery->Fields->Fields[2]->AsFloat = actualEffort;
				ScheduleQuery->Post();
			}
		}
		ScheduleQuery->Next();
	}
	ScheduleQuery->Close();

	fp	<< totalEstimated << ListSeparator
		<< totalEffort << ListSeparator
		<< totalDifference << ListSeparator
		<< (totalDifference/totalEstimated) *100 << ListSeparator
		<< totalEffortThisMonth
		<< ListSeparator << "\"Summe\"\n";

	if( needProjectExpectation )
	{
		totalEstimated = totalNewEstimated = totalEffort = 0.0;

		fp <<	"\n"
				"\"Projekterwartung\"\n"
				"\n"
				"\"Soll\"" << ListSeparator << "\"Ist\"" << ListSeparator << "\"Grad(%)\"" << ListSeparator << "\"Neu\"" << ListSeparator << "\"Veränd.\"" << ListSeparator << "\"Aufgabe\"\n";

		ScheduleQuery->ParamByName( "theUser" )->AsInteger = userId;
		ScheduleQuery->Open();
		while( !ScheduleQuery->Eof )
		{
			if( ScheduleQuery->Fields->Fields[0]->AsInteger == projectId
			&& !ScheduleQuery->Fields->Fields[5]->IsNull )
			{
				AnsiString	taskName = ScheduleQuery->Fields->Fields[1]->AsString;
				double		percentCompleted = ScheduleQuery->Fields->Fields[5]->AsFloat;
				double		actEffort = (double)(theTasks[taskName.c_str()]);
				double		newEffort;

				totalEffort += actEffort;

				estimatedEffort = ScheduleQuery->Fields->Fields[3]->AsFloat;
				if( ScheduleQuery->Fields->Fields[4]->AsString == "d" )
					estimatedEffort *= 8.0;
				totalEstimated += estimatedEffort;

				if( !percentCompleted )
					newEffort = actEffort + estimatedEffort;
				else
					newEffort = actEffort / percentCompleted *100;
				totalNewEstimated += newEffort;

				fp	<< estimatedEffort << ListSeparator
					<< actEffort << ListSeparator
					<< percentCompleted << ListSeparator
					<< newEffort << ListSeparator
					<< (newEffort-estimatedEffort)/estimatedEffort *100 << ListSeparator << '"'
					<< taskName.c_str() << "\"\n";
			}
			ScheduleQuery->Next();
		}
		ScheduleQuery->Close();

		fp	<< totalEstimated << ListSeparator
			<< totalEffort << ListSeparator
			<<(totalEffort/totalNewEstimated)*100 << ListSeparator
			<< totalNewEstimated << ListSeparator
			<< ((totalNewEstimated / totalEstimated) * 100 - 100)
			<<  ListSeparator << "\"Summe\"\n"
			"\n";

		if( totalEstimated < totalNewEstimated )
			fp << "\"Achtung: Sie werden vsl. Ihr Budget um "
			<< ((totalNewEstimated / totalEstimated) * 100 - 100)
			<< "% nicht einhalten.\"\n\n\n";
		else
			fp	<< "\"Hinweis: Sie werden vsl. nur "
				<< (totalNewEstimated / totalEstimated) * 100
				<< "% ihres Budgets benötigen.\"\n\n\n";

	}
	else
		fp << "\n\n";

	delete [] thisPeriod;

	return totalEffortThisMonth;
}

//---------------------------------------------------------------------------
void __fastcall TActivityForm::ExportProtocolClick(TObject *)
{
	TDateTime			minDate, maxDate;
	AnsiString			selProject;

	unsigned short			roundValue;
	static unsigned short	roundValues[] = { 0, 1, 5, 10, 15, 30, 60 };

	TMainForm::	setDateFormats();

	if(
		ExportActivitiesDialog->Show(
			ActivityQueryprojectName->AsString,
			ActivityQueryStartTime->AsDateTime
		) == mrOk
	)
	{
		if( testActivities( false ) )
/*@*/		return;

		roundValue = roundValues[
			ExportActivitiesDialog->RoundComboBox->ItemIndex
		];
		selProject = ExportActivitiesDialog->ProjectCombo->Text;

		ExportActivitiesDialog->GetSelectedDateRange( &minDate, &maxDate );

		std::ofstream	fp(ExportActivitiesDialog->FileEdit->Text.c_str() );
		fp.setf( std::ios_base::fixed, std::ios_base::floatfield );
		fp.precision( 2 );
		fp.imbue( std::locale( "german" ) );
		if( fp.is_open() )
		{
			if( selProject != "Alle" )
				doExportProtocol( fp, selProject, roundValue, minDate, maxDate );
			else
			{
				TQuery	*projectQuery = new TQuery( this );
				double	totalEffortThisMonth = 0.0;

				projectQuery->SQL->Add(
					"select	distinct project.name "
					"from	project, schedule, activities "
					"where	activities.StartTime >= :minDate "
					"and	activities.StartTime < :maxDate "
					"and	activities.task = schedule.id "
					"and	schedule.userId = :theUser "
					"and	schedule.project = project.id " );
				projectQuery->DatabaseName = "SchedulerDB";
				projectQuery->UniDirectional = true;

				projectQuery->ParamByName( "theUser" )->AsInteger = userId;
				projectQuery->ParamByName( "minDate" )->AsDateTime = minDate;
				projectQuery->ParamByName( "maxDate" )->AsDateTime = maxDate;

				projectQuery->Open();
				while( !projectQuery->Eof )
				{
					totalEffortThisMonth += doExportProtocol(
						fp,
						projectQuery->Fields->Fields[0]->AsString,
						roundValue, minDate, maxDate
					);
					projectQuery->Next();
				}

				projectQuery->Close();

				delete projectQuery;

				fp << "Gesamtaufwand:" << ListSeparator << totalEffortThisMonth << "\n\n\n";
			}

			fp <<
			"\"Soll\"" << ListSeparator << "\"Geschätzter Gesamtaufwand für Aufgabe (oder genehmigtes Budget) (h)\"\n"
			"\"Ist\"" << ListSeparator << "\"Tatsächlicher Aufwand für Aufgabe bisher (h)\"\n"
			"\"Diff\"" << ListSeparator << "\"Differenz (oder noch freies Budget) (h)\"\n"
			"\"Frei\"" << ListSeparator << "\"Anteil noch frei (%)\"\n"
			"\"Monat\"" << ListSeparator << "\"Tatsächlicher Aufwand für Aufgabe in diesem Monat (h)\"\n"
			"\"Grad\"" << ListSeparator << "\"Fertigstellungsgrad(%)\"\n"
			"\"Neu\"" << ListSeparator << "\"Neue Schätzung\"\n"
			"\"Veränd.\"" << ListSeparator << "\"Veränderung gegenüber Schätzung (%)\"\n\n";

			fp.close();

			ShellExecute(
				Handle,
				"open",
				ExportActivitiesDialog->FileEdit->Text.c_str(),
				"", ".",
				SW_SHOW
			);
		}

	}

	if( ExportActivitiesDialog->DeleteCheckBox->Checked )
	{
		deleteActivities( selProject, minDate, maxDate );
		RestartActivity( false );
	}
}
//---------------------------------------------------------------------------

void __fastcall TActivityForm::FormActivate(TObject *)
{
	if( userId )
	{
		TaskQuery->Close();
		TaskQuery->Open();
		selectTask( ActivityQueryTask->AsInteger );
	}
}
//---------------------------------------------------------------------------

void __fastcall TActivityForm::RestartButtonClick(TObject *)
{
	ActivityQuery->Close();
	TaskQuery->Close();

	TaskQuery->Open();
	ActivityQuery->Open();

	if( !readOnly )
		StopTimer();
}
//---------------------------------------------------------------------------



bool TActivityForm::testActivities( bool showOk )
{
	TDateTime	startTime, endTime;
	double		previousStart = 0;
	bool		badFound = false;

	ActivityQuery->First();

	while( !ActivityQuery->Eof )
	{
		startTime = ActivityQueryStartTime->AsDateTime;
		endTime = ActivityQueryEndTime->AsDateTime;

		if( previousStart && previousStart < (double)endTime )
		{
			Application->MessageBox(
				"Überschneidung gefunden", "Fehler", MB_OK|MB_ICONERROR
			);
			badFound = true;
			break;
		}
		if( endTime < startTime )
		{
			Application->MessageBox(
				"Endezeit vor Startzeit", "Fehler", MB_OK|MB_ICONERROR
			);
			badFound = true;
			break;
		}
		previousStart = startTime;

		ActivityQuery->Next();
	}

	if( !badFound && showOk )
		Application->MessageBox(
			"Keine Überschneidung gefunden",
			"Terminverwaltung",
			MB_OK|MB_ICONINFORMATION
		);

	return badFound;
}

void __fastcall TActivityForm::ExportCSVClick(TObject *)
{
	TRegistry 	*registry = new TRegistry;

	TMainForm::	setDateFormats();

	if( registry->OpenKey( registryKey, false ) )
	{
		if(  registry->ValueExists( "ExportActivitiesPath" ) )
			SaveDialog->InitialDir = registry->ReadString( "ExportActivitiesPath" );

		registry->CloseKey();
	}

	if( SaveDialog->Execute() )
	{
		AnsiString	ExportActivitiesPath = SaveDialog->FileName;

		std::ofstream	fp( ExportActivitiesPath.c_str() );
		if( fp.is_open() )
		{
			ActivityQuery->First();

			while( !ActivityQuery->Eof )
			{
				fp	<< ActivityQueryStartTime->AsString.c_str() << ','
					<< ActivityQueryEndTime->AsString.c_str() << ",\""
					<< ActivityQuerytitle->AsString.c_str() << "\",\""
					<< ActivityQueryprojectName->AsString.c_str() << "\"\n";
				ActivityQuery->Next();
			}
			fp.close();
		}
		else
		{
			Application->MessageBox(
				"Datei konnte nicht erzeugt werden.",
				"Terminverwaltung",
				MB_ICONERROR|MB_OK
			);
		}

		if( registry->OpenKey( registryKey, true ) )
		{
			int lastPos = ExportActivitiesPath.LastDelimiter( "\\" );

			if( lastPos )
				lastPos--;
			ExportActivitiesPath.SetLength( lastPos );

			registry->WriteString(
				"ExportActivitiesPath", ExportActivitiesPath
			);
			registry->CloseKey();
		}
	}

	delete registry;
}
//---------------------------------------------------------------------------

void __fastcall TActivityForm::ImportCSVClick(TObject *)
{
	int					line;
	ArrayOfStrings	csvLine;
	STRING				doubleActivity, badActivity,
						startDateStr, endDateStr,
						title, project;
	int					day, month, year,
						hour, minute, second;
	TDateTime			startDate, endDate;
	int					projectId, scheduleId;
	bool				activityFound;

	static TQuery		*ProjectScheduleQuery = NULL;
	static TQuery		*ScheduleInsertQuery = NULL;
	int					recordCount;
	size_t				numCreatedTasks, numImportedLines, numRejectedLines;
	STRING				endStat;

	TMainForm::	setDateFormats();

	if( !ProjectScheduleQuery )
	{
		ProjectScheduleQuery = new TQuery( this );
		ProjectScheduleQuery->SQL->Add(
			"select	s.id "
			"from	schedule s, project p "
			"where	s.userId = :theUser and "
					"s.title = :theTitle and "
					"s.project = p.id and "
					"p.name = :theProject" );
		ProjectScheduleQuery->DatabaseName = "SchedulerDB";
		ProjectScheduleQuery->UniDirectional = false;
	}

	if( !ScheduleInsertQuery )
	{
		ScheduleInsertQuery = new TQuery( this );
		ScheduleInsertQuery->SQL->Add(
			"insert into schedule "
			"( userId, title, project, StartDate, EndDate, EstEffort )"
			"values"
			"( :userId, :title, :project, :StartDate, :EndDate, 1 )"
		);
		ScheduleInsertQuery->DatabaseName = "SchedulerDB";
	}

	TRegistry 	*registry = new TRegistry;

	if( registry->OpenKey( registryKey, false ) )
	{
		if(  registry->ValueExists( "ExportActivitiesPath" ) )
			OpenDialog->InitialDir = registry->ReadString( "ExportActivitiesPath" );

		registry->CloseKey();
	}

	if( OpenDialog->Execute() )
	{
		AnsiString	ExportActivitiesPath = OpenDialog->FileName;

		std::ifstream fp( ExportActivitiesPath.c_str() );
		if( fp.is_open() )
		{
			theDataModule->ProjectTable->Open();
			ActivityQuery->DisableControls();

			line = 1;
			numCreatedTasks = numImportedLines = numRejectedLines = 0;

			while( !fp.eof() && !fp.fail() )
			{
				csvLine.clear();
				readCSVLine( fp, &csvLine );

				startDateStr = csvLine[0];
				endDateStr = csvLine[1];
				title = csvLine[2];
				project = csvLine[3];
				if( startDateStr == "" || endDateStr == "" || title == "" || project == "" || fp.eof() )
					break;


				sscanf( startDateStr, "%d.%d.%d %d:%d:%d", &day, &month, &year, &hour, &minute, &second );
				startDate = EncodeDate( (Word)year, (Word)month, (Word)day ) + EncodeTime( (Word)hour, (Word)minute, (Word)second, 0 );
				sscanf( endDateStr, "%d.%d.%d %d:%d:%d", &day, &month, &year, &hour, &minute, &second );
				endDate = EncodeDate( (Word)year, (Word)month, (Word)day ) + EncodeTime( (Word)hour, (Word)minute, (Word)second, 0 );

				ProjectScheduleQuery->Params->Items[0]->AsInteger = userId;
				ProjectScheduleQuery->Params->Items[1]->AsString = (const char *)title;
				ProjectScheduleQuery->Params->Items[2]->AsString = (const char *)project;

				ProjectScheduleQuery->Open();
				recordCount = ProjectScheduleQuery->RecordCount;
				if( recordCount == 0 )
				{
					ProjectScheduleQuery->Close();
					projectId = theDataModule->getProjectId( project );

					ScheduleInsertQuery->Params->Items[0]->AsInteger = userId;
					ScheduleInsertQuery->Params->Items[1]->AsString = (const char *)title;
					ScheduleInsertQuery->Params->Items[2]->AsInteger = projectId;
					ScheduleInsertQuery->Params->Items[3]->AsDateTime = TDateTime::CurrentDateTime();
					ScheduleInsertQuery->Params->Items[4]->AsDateTime = TDateTime::CurrentDateTime();

					ScheduleInsertQuery->ExecSQL();
					ProjectScheduleQuery->Open();
					recordCount = ProjectScheduleQuery->RecordCount;
					numCreatedTasks++;
				}
				if( recordCount == 1 )
				{
					scheduleId = ProjectScheduleQuery->Fields->Fields[0]->AsInteger;

					// check whether there is an activity
					activityFound = false;
					ActivityQuery->First();
					while( !ActivityQuery->Eof && !activityFound )
					{
						if( ActivityQueryStartTime->AsString == (const char *)startDateStr
						&&  ActivityQueryEndTime->AsString == (const char *)endDateStr
						&&  ActivityQueryTask->AsInteger == scheduleId )
						{
							activityFound = true;
							break;
						}
						ActivityQuery->Next();
					}

					if( !activityFound )
					{
						insertQuery->ParamByName( "id" )->AsInteger = theDataModule->getNewActivitiyId();
						insertQuery->ParamByName( "task" )->AsInteger = scheduleId;
						insertQuery->ParamByName( "StartTime" )->AsDateTime = startDate;
						insertQuery->ParamByName( "EndTime" )->AsDateTime = endDate;
						insertQuery->ExecSQL();
						numImportedLines++;
					}
					else
					{
						doubleActivity += formatNumber( line );
						doubleActivity += ": Tätigkeit schon gefunden\n";
						numRejectedLines++;
					}
				}
				else
				{
					badActivity += formatNumber( line );
					badActivity += ": ";
					badActivity += formatNumber(
						ProjectScheduleQuery->RecordCount
					);
					badActivity += " Aufgaben gefunden\n";
					numRejectedLines++;
				}
				ProjectScheduleQuery->Close();

				line++;
			}
			fp.close();

			if( doubleActivity[0U] )
			{
				Application->MessageBox(
					doubleActivity, "Terminverwaltung", MB_OK|MB_ICONWARNING
				);
			}

			if( badActivity[0U] )
			{
				Application->MessageBox(
					badActivity, "Terminverwaltung", MB_OK|MB_ICONWARNING
				);
			}

			endStat += formatNumber( numImportedLines );
			endStat += " Zeile(n) importiert,\n";
			endStat += formatNumber( numRejectedLines );
			endStat += " Zeile(n) abgewiesen,\n";
			endStat += formatNumber( numCreatedTasks );
			endStat += " Aufgabe(n) erzeugt.\n";


			Application->MessageBox(
				endStat, "Terminverwaltung", MB_OK|MB_ICONINFORMATION
			);

			ActivityQuery->EnableControls();
			theDataModule->ProjectTable->Close();

			ActivityQuery->Close();
			TaskQuery->Close();

			TaskQuery->Open();
			ActivityQuery->Open();
		}
		else
		{
			Application->MessageBox(
				"Datei konnte nicht geladen werden.",
				"Terminverwaltung",
				MB_ICONERROR|MB_OK
			);
		}


		if( registry->OpenKey( registryKey, true ) )
		{
			int lastPos = ExportActivitiesPath.LastDelimiter( "\\" );

			if( lastPos )
				lastPos--;
			ExportActivitiesPath.SetLength( lastPos );

			registry->WriteString(
				"ExportActivitiesPath", ExportActivitiesPath
			);
			registry->CloseKey();
		}
	}

	delete registry;
}
//---------------------------------------------------------------------------

void __fastcall TActivityForm::AutoSaveTimerTimer(TObject *)
{
	if( ActivityQueryID->AsInteger == currentActivityId && TaskComboBox->Enabled )
	{
		int			taskId = getSelectedTaskId();

		if( taskId )
		{
			TaskComboBox->Enabled = false;

			updateQuery->ParamByName( "id" )->AsInteger = currentActivityId;
			updateQuery->ParamByName( "task" )->AsInteger = taskId;
			updateQuery->ParamByName( "StartTime" )->AsDateTime = ActivityQueryStartTime->AsDateTime;
			updateQuery->ParamByName( "EndTime" )->AsDateTime = Now();

			updateQuery->ExecSQL();

			RestartActivity( true );
			saveTask( taskId );

			TaskComboBox->Enabled = true;

			unsigned short	dummy, sec, milisec;

			Now().DecodeTime( &dummy, &dummy, &sec, &milisec );
			AutoSaveTimer->Interval = 60000L - sec*1000 - milisec;
		}
	}
}
//---------------------------------------------------------------------------



void __fastcall TActivityForm::StartButtonClick(TObject *Sender )
{
	if( readOnly )
/*@*/	return;

	bool	createNewTask = (currentActivityId <0);

	StopButtonClick( Sender );

	int	lastTask = createNewTask ? getSelectedTaskId() : getLastTask();
	if( lastTask )
	{
		int activityId = theDataModule->getNewActivitiyId();

		insertQuery->ParamByName( "id" )->AsInteger = activityId;
		insertQuery->ParamByName( "task" )->AsInteger = lastTask;
		insertQuery->ParamByName( "StartTime" )->AsDateTime = Now();
		insertQuery->ParamByName( "EndTime" )->AsDateTime = Now();

		insertQuery->ExecSQL();

		RestartActivity( true );

		StartTimer(activityId);
	}
}
//---------------------------------------------------------------------------

void __fastcall TActivityForm::StopButtonClick(TObject *)
{
	if( readOnly || !ActivityQuery->Active )
/*@*/	return;

	int	taskId = getSelectedTaskId();

	if( taskId && ActivityQueryID->AsInteger == currentActivityId )
	{
		updateQuery->ParamByName( "id" )->AsInteger = currentActivityId;
		updateQuery->ParamByName( "task" )->AsInteger = taskId;
		updateQuery->ParamByName( "StartTime" )->AsDateTime = ActivityQueryStartTime->AsDateTime;
		updateQuery->ParamByName( "EndTime" )->AsDateTime = Now();

		updateQuery->ExecSQL();

		RestartActivity( true );

		StopTimer();
		saveTask( taskId );
	}
}
//---------------------------------------------------------------------------

void __fastcall TActivityForm::DeleteButtonClick(TObject *)
{
	if( readOnly )
/*@*/	return;

	deleteQuery->ParamByName( "id" )->AsInteger = ActivityQueryID->AsInteger;

	deleteQuery->ExecSQL();

	RestartActivity( true );

	StopTimer();
}
//---------------------------------------------------------------------------

void __fastcall TActivityForm::SaveButtonClick(TObject *)
{
	if( readOnly || ActivityQueryTransfered->AsString.Length() )
/*@*/	return;

	int	taskId = getSelectedTaskId();

	if( taskId )
	{
		updateQuery->ParamByName( "id" )->AsInteger = ActivityQueryID->AsInteger;
		updateQuery->ParamByName( "task" )->AsInteger = taskId;
		updateQuery->ParamByName( "StartTime" )->AsDateTime = getStart();
		updateQuery->ParamByName( "EndTime" )->AsDateTime = getEnd();

		updateQuery->ExecSQL();

		RestartActivity( false );

		StopTimer();

		saveTask( taskId );
	}
}
//---------------------------------------------------------------------------

void __fastcall TActivityForm::ActivityQueryAfterScroll(TDataSet *)
{

	TDateTime start = ActivityQueryStartTime->AsDateTime;

	if( (int)start )
	{
		StartDatePicker->Date = start;
		StartTimePicker->Time = start;
	}

	TDateTime end = ActivityQueryEndTime->AsDateTime;
	if( (int)end )
	{
		EndDatePicker->Date = end;
		EndTimePicker->Time = end;
	}

	if(TaskComboBox->Enabled)
	{
		TaskComboBox->Enabled = false;

		selectTask( ActivityQueryTask->AsInteger );

		StopTimer();
		TaskComboBox->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TActivityForm::TaskQueryAfterOpen(TDataSet *)
{
	TaskComboBox->Enabled = false;
	TaskComboBox->Items->Clear();
	while( !TaskQuery->Eof )
	{
		TaskComboBox->Items->Add( TaskQuerytitle->AsString );
		TaskQuery->Next();
	}
	TaskComboBox->Enabled = true;
}
//---------------------------------------------------------------------------
int TActivityForm::getSelectedTaskId()
{
	if( TaskQuery->Active )
	{
		if( TaskQuerytitle->AsString != TaskComboBox->Items->Strings[TaskComboBox->ItemIndex] )
		{
			TaskQuery->First();
			while( !TaskQuery->Eof )
			{
				if( TaskQuerytitle->AsString == TaskComboBox->Items->Strings[TaskComboBox->ItemIndex] )
					break;
				TaskQuery->Next();
			}
		}

		return TaskQueryid->AsInteger;
	}
	else
		return 0;
}
//---------------------------------------------------------------------------
void TActivityForm::selectTask( int TaskId )
{
	// find id in list of tasks
	if( TaskQueryid->AsInteger != TaskId )
	{
		TaskQuery->First();
		while( !TaskQuery->Eof )
		{
			if( TaskQueryid->AsInteger == TaskId )
				break;
			TaskQuery->Next();
		}
	}

	// select task in combo box if not yet done
	if( TaskQuerytitle->AsString != TaskComboBox->Items->Strings[TaskComboBox->ItemIndex] )
	{
		size_t	i, count = TaskComboBox->Items->Count;
		for(i=0;i<count; i++ )
		{
			if( TaskComboBox->Items->Strings[i] == TaskQuerytitle->AsString )
			{
				TaskComboBox->ItemIndex = i;
				break;
			}
		}
	}
}
//---------------------------------------------------------------------------
int TActivityForm::getLastTask( void )
{
	int	lastTask = 0;

	static	TQuery	*activeUserQuery = NULL;

	if( !activeUserQuery )
	{
		activeUserQuery = new TQuery( NULL );
		activeUserQuery->SQL->Add(
			"select	LastTask "
			"from	user_tab "
			"where	id = :theID"
		);
		activeUserQuery->DatabaseName = "SchedulerDB";

		activeUserQuery->Prepare();
	}

	activeUserQuery->Params->Items[0]->AsInteger = userId;
	activeUserQuery->Open();
	if( !activeUserQuery->Eof )
		lastTask = activeUserQuery->Fields->Fields[0]->AsInteger;

	activeUserQuery->Close();

	return lastTask;
}
//---------------------------------------------------------------------------
void TActivityForm::saveTask( int TaskId )
{
	static	TQuery	*updateUserSQL = NULL;

	if( !updateUserSQL )
	{
		updateUserSQL = new TQuery( NULL );
		updateUserSQL->SQL->Add(
			"update user_tab "
			"set LastTask = :lastId "
			"where id = :theUser"
		);
		updateUserSQL->DatabaseName = "SchedulerDB";

		updateUserSQL->Prepare();
	}

	updateUserSQL->Params->Items[0]->AsInteger = TaskId;
	updateUserSQL->Params->Items[1]->AsInteger = userId;
	updateUserSQL->ExecSQL();
}

void __fastcall TActivityForm::FormDestroy(TObject *Sender)
{
	StopButtonClick( Sender );
}
//---------------------------------------------------------------------------
void __fastcall TActivityForm::ExportAufgaben1Click(TObject *)
{
	TRegistry	*registry = new TRegistry;

	if( registry->OpenKey( registryKey, false )
	&&  registry->ValueExists( "TaskPath" ) )
	{
		SaveDialog->InitialDir = registry->ReadString( "TaskPath" );
	}
	registry->CloseKey();

	SaveDialog->FileName = "Tasks.csv";
	if( SaveDialog->Execute() )
	{
		char *dbFile = SaveDialog->FileName.c_str();

		if( registry->OpenKey( registryKey, true ) )
		{
			AnsiString	TaskPath = SaveDialog->FileName;
			registry->WriteString( "TaskFile", TaskPath );

			int			lastPos = TaskPath.LastDelimiter( "\\" );

			if( lastPos )
				lastPos--;
			TaskPath.SetLength( lastPos );

			registry->WriteString( "TaskPath", TaskPath );
			registry->CloseKey();
		}

		std::ofstream	csvFile( dbFile );
		if( csvFile.is_open() )
			exportTasksToStream( csvFile, userId );

		csvFile.close();
	}

	delete registry;
}
//---------------------------------------------------------------------------


STRING TActivityForm::importActivitiesFromStream( std::istream &csvFile )
{
	ArrayOfStrings	fieldDef;
	FieldSet	  	record1, record2;
	FieldSet	  	*actual = &record1, *lastSet = NULL;

	int			  	newActivities = 0, dubActivities = 0, numErrors = 0;
	STRING		  	stats = "";

	readCSVLine( csvFile, &fieldDef );

	while( !csvFile.eof() && !csvFile.fail() )
	{
		readFIELD_SET( csvFile, fieldDef, actual );
		if( csvFile.eof() || csvFile.fail() )
			break;

		if( lastSet )
		{
			int	taskId;
			bool errorFlag = false;
			TDateTime startTime, endTime;

			// we must save the last record
			int day, month, year, hour, minute, second;
			try
			{
				sscanf( STRING((*lastSet)["START_TIME"]), "%d.%d.%d %d:%d:%d",
					&day, &month, &year, &hour, &minute, &second );
				startTime = EncodeDate( (Word)year, (Word)month, (Word)day ) + EncodeTime( (Word)hour, (Word)minute, (Word)second, 0 );
				sscanf( STRING((*actual)["START_TIME"]), "%d.%d.%d %d:%d:%d",
					&day, &month, &year, &hour, &minute, &second );
				endTime = EncodeDate( (Word)year, (Word)month, (Word)day ) + EncodeTime( (Word)hour, (Word)minute, (Word)second, 0 );
			}
			catch( Exception &e )
			{
				numErrors++;
				errorFlag = true;
			}

			if( !errorFlag )
			{
				taskId = (*lastSet)["ID"];

				// check whether there is an activity
				bool activityFound = false;
				ActivityQuery->First();
				while( !ActivityQuery->Eof && !activityFound )
				{
					if( ActivityQueryStartTime->AsDateTime == startTime
					&&  ActivityQueryEndTime->AsDateTime == endTime
					&&  ActivityQueryTask->AsInteger == taskId )
					{
						activityFound = true;
						break;
					}
					ActivityQuery->Next();
				}

				if( !activityFound )
				{
					try
					{
						insertQuery->ParamByName( "id" )->AsInteger = theDataModule->getNewActivitiyId();
						insertQuery->ParamByName( "task" )->AsInteger = taskId;
						insertQuery->ParamByName( "StartTime" )->AsDateTime = startTime;
						insertQuery->ParamByName( "EndTime" )->AsDateTime = endTime;
						insertQuery->ExecSQL();
						newActivities++;
					}
					catch( ... )
					{
						numErrors++;
					}
				}
				else
					dubActivities++;
			}

			taskId = (*actual)["ID"];
			if( !taskId )
				lastSet = NULL;
			else
			{
				lastSet = actual;
				actual = (lastSet == &record1 ? &record2 : &record1);
			}
		}
		else
		{
			lastSet = actual;
			actual = (lastSet == &record1 ? &record2 : &record1);
		} // if( lastSet )
	} // while( !csvFile.eof() && !csvFile.fail() )
	if( lastSet )	// no finished activity?
	{
		// we must save the last record
		try
		{
			int day, month, year, hour, minute, second;
			sscanf( STRING((*lastSet)["START_TIME"]), "%d.%d.%d %d:%d:%d",
				&day, &month, &year, &hour, &minute, &second );
			TDateTime startTime = EncodeDate( (Word)year, (Word)month, (Word)day ) + EncodeTime( (Word)hour, (Word)minute, (Word)second, 0 );

			int	taskId;
			taskId = (*lastSet)["ID"];
			insertQuery->ParamByName( "id" )->AsInteger = theDataModule->getNewActivitiyId();
			insertQuery->ParamByName( "task" )->AsInteger = taskId;
			insertQuery->ParamByName( "StartTime" )->AsDateTime = startTime;
			insertQuery->ParamByName( "EndTime" )->AsDateTime = startTime;
			insertQuery->ExecSQL();

			stats += "Letzte Aktivität wurde nicht abgeschlossen.\n";

			newActivities++;
		}
		catch( Exception &e )
		{
			numErrors++;
		}
	}

	RestartButtonClick( NULL );

	// show stats
	stats += formatNumber( newActivities );
	stats += " Tätigkeiten erzeugt.\n";
	stats += formatNumber( dubActivities );
	stats += " Tätigkeiten schon vorhanden.\n";
	stats += formatNumber( numErrors );
	stats += " Fehler\n";

	return stats;
}

//---------------------------------------------------------------------------

void __fastcall TActivityForm::ImportTtigkeiten1Click(TObject *)
{
	TRegistry	*registry = new TRegistry;

	if( registry->OpenKey( registryKey, false )
	&&  registry->ValueExists( "ActivityPath" ) )
	{
		OpenDialog->InitialDir = registry->ReadString( "ActivityPath" );
	}
	registry->CloseKey();

	OpenDialog->FileName = "Activity.csv";
	if( OpenDialog->Execute() )
	{
		AnsiString	ActivityFile = OpenDialog->FileName;
		if( registry->OpenKey( registryKey, true ) )
		{
			registry->WriteString( "ActivityFile", ActivityFile );

			AnsiString	ActivityPath = ActivityFile;
			int			lastPos = ActivityPath.LastDelimiter( "\\" );

			if( lastPos )
				lastPos--;
			ActivityPath.SetLength( lastPos );

			registry->WriteString( "ActivityPath", ActivityPath );

			registry->CloseKey();
		}

		std::ifstream			csvFile( ActivityFile.c_str() );

		if( csvFile.is_open() )
		{
			STRING stats = importActivitiesFromStream( csvFile );

			Application->MessageBox( stats, "Import", MB_OK );

			csvFile.close();
		}
		else
			Application->MessageBox(
				"Datei konnte nicht geöffnet werden.", "Fehler", MB_OK
			);


	} // if( OpenDialog->Execute() )

	delete registry;
}
//---------------------------------------------------------------------------

void __fastcall TActivityForm::TestClick(TObject *)
{
	testActivities( true );
}
//---------------------------------------------------------------------------

void TActivityForm::setUserId( long userId, STRING userName, int permissions )
{
	STRING newCaption = Caption.c_str();
	newCaption += ' ';
	newCaption += userName;
	Caption = (const char*)newCaption;

	readOnly = !(permissions & PERM_ACTIVITY_WRITE);
	this->userId = userId;
	TaskQuery->ParamByName( "theUser" )->AsInteger = userId;
	TaskQuery->Open();

	StartButton->Enabled = !readOnly;
	StopButton->Enabled = !readOnly;
	DeleteButton->Enabled = !readOnly;
	SaveButton->Enabled = !readOnly;

	ActivityQuery->ParamByName( "theUser" )->AsInteger = userId;
	// ActivityQuery->RequestLive = !readOnly;
	ActivityQuery->Open();

	if( !readOnly && SetupDialog->GetAutoActivity() )
	{
		long		lastTask = getLastTask();

		if( lastTask > 0 )
			StartButtonClick( StartButton );
	}

	int tag = this->Tag;
	tag %= 10;
	tag += userId * 10;
	this->Tag = tag;
}

void __fastcall TActivityForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	StopButtonClick( Sender );
	Action = caFree;	
}
//---------------------------------------------------------------------------

void __fastcall TActivityForm::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
	StopButtonClick( Sender );
	CanClose = true;
}
//---------------------------------------------------------------------------

void __fastcall TActivityForm::DBGridActivitiesDrawColumnCell(TObject *,
      const TRect &Rect, int DataCol, TColumn *Column,
      TGridDrawState State)
{
	TGridDrawState	empty;

	if( State == empty )
	{
		if( ActivityQueryTransfered->AsString.Length() )
			DBGridActivities->Canvas->Brush->Color = clRed;
	}
	DBGridActivities->DefaultDrawColumnCell( Rect, DataCol, Column, State);
}
//---------------------------------------------------------------------------

