/**----------------------------------------------------------------------------
* file main.cpp
*
* Main for the project "OptiDAR".
*
* Last Update: 05.04.2016
* Version 1.0
*------------------------------------------------------------------------------
* Licensed Materials - Property of
\*---------------------------------------------------------------------------*/

#include "./darh.h"

//#define InterfaceDB

int DB2CSV(int SessionID);
int ExecDB2CSV(int SessionID, char *NameTab);
int CSV2DB(int SessionID, int SolutionID);
int ExecCSV2DB(int SessionID, char *NameTab);
int ExecCSV2DBSol(int SessionID, int SolutionID, char *NameTab);

// Error Logging management
CError error;

/**----------------------------------------------------------------------------
* MAIN usage : argv[1] = input polylines shapefile name without extension;
*              argv[2] = Session ID 
\*---------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    register C_IST Ist;	
    register C_SHP mySHP;
	register C_CSV myCSV;
 
	clock_t t1, t2;
	double dt;

	char buf1[100];

	// To be read from the input!
	Ist.SessionID = atoi(argv[2]);
	Ist.SolutionID = 1; 
	Ist.SolutionNum = 1;
	Ist.RouteID = 1;
	Ist.LogID = 1;

	// Default Parameters
	Ist.Param.PathType = 100;  // Quicker 
	Ist.Param.AvgSpeed = 35. * (1000./3600.);  // 35 km/h

	snprintf(buf1, sizeof(buf1), "ISTANZA%d", Ist.SessionID);
	snprintf(Ist.networkFileName, sizeof(Ist.networkFileName), "%s//%s", INPUTDIR, argv[1]);
	snprintf(Ist.logFileName, sizeof(Ist.logFileName), "%s//%s_log.txt", OUTPUTDIR, buf1);
	snprintf(Ist.logcsvFileName, sizeof(Ist.logcsvFileName), "%s//%s_SessionLog.csv", OUTPUTDIR, buf1);

	// Open log file
	Ist.debug = 1;
	Ist.flog = fopen(Ist.logFileName, "w");
	if (Ist.flog == NULL)
		return 1;

	// Open csv log file 
	Ist.fcsv = fopen(Ist.logcsvFileName, "w");
	if (Ist.fcsv == NULL)
		return 1;

	// Switch the stderr into log file
	error.ferr = Ist.flog;
	error.fcsv = Ist.fcsv;
	error.SessionID = Ist.SessionID;
	error.SolutionID = Ist.SolutionID;
	error.SolutionNum = Ist.SolutionNum;
	error.LogID = Ist.LogID;

	// Start!
	if (Ist.debug) printf("OptiDAR: Start \n\n");
	fprintf(Ist.flog,"OptiDAR: Start \n\n");
	fflush(Ist.flog);

#ifdef InterfaceDB
	// Import DB to CSV
	if (Ist.debug) printf("\n\nOptiDAR: Import DB to CSV \n\n");
	fprintf(Ist.flog, "\n\nOptiDAR: Import DB to CSV \n\n");
	t1 = clock();
	DB2CSV(Ist.SessionID);
	t2 = clock();
	dt = (double)(t2 - t1) / CLOCKS_PER_SEC;
	if (Ist.debug) printf("\n  Computing Time (sec) DB2CSV: %6.3f\n\n", dt);
	fprintf(Ist.flog, "\n  Computing Time (sec) DB2CSV: %6.3f\n\n", dt);
#endif
	
	// Modulo-InOut: 
	// - Read the geographic information (i.e., shapfiles for lines and points) 
	// - Read the problem
	// - Generate the input file for the optimizer
	if (Ist.debug) printf("\nOptiDAR: Read Data \n\n");
	fprintf(Ist.flog, "\nOptiDAR: Read Data \n\n");
	t1 = clock();
	ReadInputData(&mySHP, &Ist, &myCSV);
	t2 = clock();
	dt = (double)(t2 - t1) / CLOCKS_PER_SEC;
	if (Ist.debug) printf("  Computing Time (sec) Read Data: %6.3f\n\n", dt);
	fprintf(Ist.flog, "  Computing Time (sec) Read Data: %6.3f\n\n", dt);

	//
	// Modulo-Opti: Call the optimizer
    //
	if (Ist.debug) printf("\nOptiDAR: Optimization \n\n");
	fprintf(Ist.flog, "\nOptiDAR: Optimization \n\n");
	t1 = clock();
	main_DAR(buf1);
	t2 = clock();
	dt = (double)(t2 - t1) / CLOCKS_PER_SEC;
	if (Ist.debug) printf("  Computing Time (sec) Optimization: %6.3f\n\n", dt);
	fprintf(Ist.flog, "  Computing Time (sec) Optimization: %6.3f\n\n", dt);

	//
	// Modulo-InOut: 
	// - Read the solution (from a CSV file)
	// - Write solution
	// - Write the shape corresponding to the trip(s)
	if (Ist.debug) printf("\nOptiDAR: Write Solution \n\n");
	fprintf(Ist.flog, "\nOptiDAR: Write Solution \n\n");
	t1 = clock();
	ReadOutputData(&mySHP, &Ist, &myCSV);
	t2 = clock();
	dt = (double)(t2 - t1) / CLOCKS_PER_SEC;
	if (Ist.debug) printf("  Computing Time (sec) Write Solution: %6.3f\n\n", dt);
	fprintf(Ist.flog, "  Computing Time (sec) Write Solution: %6.3f\n\n", dt);

	//
    //myCSV.CSV_DeAllocateMatrixDT_and_C();
    //

	// Close log file
	fclose(Ist.flog);
	fclose(Ist.fcsv);

#ifdef InterfaceDB
	// Import CSV to DB
	if (Ist.debug) printf("\nOptiDAR: Export CSV to DB \n\n");
	fprintf(Ist.flog, "\nOptiDAR: Export CSV to DB \n\n");
	t1 = clock();
	CSV2DB(Ist.SessionID, Ist.SolutionID);
	t2 = clock();
	dt = (double)(t2 - t1) / CLOCKS_PER_SEC;
	if (Ist.debug) printf("  Computing Time (sec) CSV2DB: %6.3f\n\n", dt);
	fprintf(Ist.flog, "  Computing Time (sec) CSV2DB: %6.3f\n\n", dt);
#endif

    return 0;
}; 


/**----------------------------------------------------------------------------
*  Import from DB to CSV
\*---------------------------------------------------------------------------*/
int DB2CSV(int SessionID)
{
	ExecDB2CSV(SessionID, (char *)"ParkingPoint");
	ExecDB2CSV(SessionID, (char *)"ParkingPointCapacity");
	ExecDB2CSV(SessionID, (char *)"Point");
	ExecDB2CSV(SessionID, (char *)"Request");
	ExecDB2CSV(SessionID, (char *)"RequestAddress");
	ExecDB2CSV(SessionID, (char *)"RequestInRoute");
	ExecDB2CSV(SessionID, (char *)"Route");
	ExecDB2CSV(SessionID, (char *)"Session");
	ExecDB2CSV(SessionID, (char *)"SessionLog");
	ExecDB2CSV(SessionID, (char *)"Solution");
	ExecDB2CSV(SessionID, (char *)"StopPoint");
	ExecDB2CSV(SessionID, (char *)"Solution");
	ExecDB2CSV(SessionID, (char *)"SystemStatus");
	ExecDB2CSV(SessionID, (char *)"TypeOfVehicle");
	ExecDB2CSV(SessionID, (char *)"Vehicle");
	ExecDB2CSV(SessionID, (char *)"TypeOfVehicle");
	ExecDB2CSV(SessionID, (char *)"VehicleSchedule");
	ExecDB2CSV(SessionID, (char *)"Waypoints");

	return 0;
}


/**----------------------------------------------------------------------------
*  Execute Import from DB to CSV
\*---------------------------------------------------------------------------*/
int ExecDB2CSV(int SessionID, char *NameTab)
{
	char SID[10];
	char Command[255];

	sprintf(Command,"%s\\db2csv.bat ", SQLDIR);
	sprintf(SID, "%d ", SessionID);
	strcat(Command, SID);
	strcat(Command, NameTab);
	system(Command);

	return 0;
}

/**----------------------------------------------------------------------------
*  Export from CSV to DB
\*---------------------------------------------------------------------------*/
int CSV2DB(int SessionID, int SolutionID)
{
	ExecCSV2DBSol(SessionID, SolutionID, (char *)"Waypoint");
	ExecCSV2DBSol(SessionID, SolutionID, (char *)"Route");
	ExecCSV2DBSol(SessionID, SolutionID, (char *)"RoutePoint");
	ExecCSV2DBSol(SessionID, SolutionID, (char *)"RequestInRoute");
	ExecCSV2DB(SessionID, (char *)"Solution");
	ExecCSV2DB(SessionID, (char *)"SessionLog");

	return 0;
}


/**----------------------------------------------------------------------------
*  Execute Export from CSV to DB (using SessionID for deleting)
\*---------------------------------------------------------------------------*/
int ExecCSV2DB(int SessionID, char *NameTab)
{
	char SID[10];
	char Command[255];

	sprintf(Command, "%s\\csv2db.bat ", SQLDIR);
	sprintf(SID, "%d ", SessionID);
	strcat(Command, SID);
	strcat(Command, NameTab);
	system(Command);

	return 0;
}

/**----------------------------------------------------------------------------
*  Execute Export from CSV to DB (using SolutionID for deleting)
\*---------------------------------------------------------------------------*/
int ExecCSV2DBSol(int SessionID, int SolutionID, char *NameTab)
{
	char SID[10];
	char Command[255];

	sprintf(Command, "%s\\csv2db-sol.bat ", SQLDIR);
	sprintf(SID, "%d ", SessionID);
	strcat(Command, SID);
	sprintf(SID, "%d ", SolutionID);
	strcat(Command, SID);
	strcat(Command, NameTab);
	system(Command);

	return 0;
}

/* END main */

