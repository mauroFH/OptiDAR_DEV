/**----------------------------------------------------------------------------
* file ModuloIO.cpp
*
* Version 1.0
*------------------------------------------------------------------------------
* Licensed Materials - Property of
\*---------------------------------------------------------------------------*/

#include "../Modulo-Main/darh.h"

/**----------------------------------------------------------------------------
* In the following we have to place the functions of Modulo-InOut
\*---------------------------------------------------------------------------*/
int stupid_path_search(int s, int t, int * v_SHP_path, C_IST *Ist);

/**----------------------------------------------------------------------------
* Read the input data
* @param argc Number of args (temporary)
* @param argv Args (temporary)
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int ReadInputData(C_SHP *mySHP, C_IST *Ist, C_CSV *myCSV)
{
    char buf1[100], buf2[100];

    // Initializa a log file (temporary: probably we have to share a unique log file)
    //std::ofstream out("out.txt");
    //std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

    // Read input shapefile of the network and build forwars star
	fprintf(Ist->flog, "  Perform SHP_readAndBuildLines... \n");
	fflush(Ist->flog);
    mySHP->SHP_readAndBuildLines(Ist->networkFileName, Ist);
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

    // Read instance data
	fprintf(Ist->flog, "  Perform CSV_readInstance... \n");
	fflush(Ist->flog);
	snprintf(buf1, sizeof(buf1), "ISTANZA%d", Ist->SessionID);
    myCSV->CSV_readInstance(buf1, Ist->networkFileName, mySHP, Ist);
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

    // Save the CSV file for the optimizer
	fprintf(Ist->flog, "  Perform CSV_writeOptiData... \n");
	fflush(Ist->flog);
	snprintf(buf2, sizeof(buf2), "%s//%s_dar_input.csv", INPUTDIR, buf1);
	//myCSV->CSV_writeOptiData(buf2, mySHP, Ist);
	myCSV->CSV_writeOptiData_MS2(buf2, mySHP, Ist);
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

    // creates a shape with the requests
	fprintf(Ist->flog, "  Perform SHP_writeShapeRequests... \n");
	fflush(Ist->flog);
	mySHP->SHP_writeShapeRequests(buf1, Ist );
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

	// create a shapefile with the stoppoints
	fprintf(Ist->flog, "  Perform SHP_writeShapeStopPoints... \n");
	fflush(Ist->flog);
	mySHP->SHP_writeShapeStopPoints(buf1, Ist);
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

	// create a shapefile with the ParkingPoints
	fprintf(Ist->flog, "  Perform SHP_writeShapeParkingPoints... \n");
	fflush(Ist->flog);
	mySHP->SHP_writeShapeParkingPoints(buf1, Ist);
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

    return 0;
}; /* END main */

/**----------------------------------------------------------------------------
* Read the output data (from the optimizer)
* @param argc Number of args (temporary)
* @param argv Args (temporary)
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int ReadOutputData(C_SHP *mySHP, C_IST *Ist, C_CSV *myCSV)
{
    char buf1[CON_MAXNSTR];

    // Read the CSV file containing the soltion computed by the optimizer
	fprintf(Ist->flog, "  Perform CSV_readOptiData... \n");
	fflush(Ist->flog);
	snprintf(buf1, sizeof(buf1), "ISTANZA%d", Ist->SessionID);
    snprintf(buf, sizeof(buf), "%s//%s_dar_solution.csv", OUTPUTDIR, buf1);
	//myCSV->CSV_readOptiData(buf, mySHP, Ist);
	myCSV->CSV_readOptiData_MS2(buf, mySHP, Ist);
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

    // Write the output files

	// Write WayPoints
	fprintf(Ist->flog, "  Perform CVS_writeWaypoints... \n");
	fflush(Ist->flog);
	myCSV->CVS_writeWaypoints(mySHP, Ist, buf1);
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

	// Write Route
	fprintf(Ist->flog, "  Perform CVS_writeRoute... \n");
	fflush(Ist->flog);
	//myCSV->CVS_writeRouteOld(Ist, buf1);
	myCSV->CVS_writeRoute(Ist, buf1);
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

	// Write RoutePoints
	fprintf(Ist->flog, "  Perform CVS_writeRoutePoint... \n");
	fflush(Ist->flog);
	myCSV->CVS_writeRoutePoint(Ist, buf1);
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

	// Write RequestInRoute
	fprintf(Ist->flog, "  Perform CVS_writeRequestInRoute... \n");
	fflush(Ist->flog);
	myCSV->CVS_writeRequestInRoute(Ist, buf1);
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

	// Write Solution
	fprintf(Ist->flog, "  Perform CVS_writeSolution... \n");
	fflush(Ist->flog);
	myCSV->CVS_writeSolution();
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

	// Write Setup
	fprintf(Ist->flog, "  Perform CVS_writeSetup... \n");
	fflush(Ist->flog);
	myCSV->CVS_writeSetup(Ist);
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

	// write shapefiles 

    // Write the shapefile with the waypoints path(s)
	fprintf(Ist->flog, "  Perform SHP_writeShapeFromWaypoints... \n");
	fflush(Ist->flog);
	mySHP->SHP_writeShapeFromWaypoints(buf1);
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

	// creates a shapefile with the path (route))
	fprintf(Ist->flog, "  Perform CVS_writePath... \n");
	fflush(Ist->flog);
	myCSV->CVS_writePath(mySHP, Ist, buf1);
	fprintf(Ist->flog, "  Done! \n\n");
	fflush(Ist->flog);

	return 0;
}