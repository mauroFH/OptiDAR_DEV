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
    mySHP->SHP_readAndBuildLines(Ist->networkFileName, Ist);

    // Read instance data
    snprintf(buf1, sizeof(buf1), "ISTANZA%d", Ist->SessionID);
    myCSV->CSV_readInstance(buf1, Ist->networkFileName, mySHP, Ist);
    
    // Save the CSV file for the optimizer
    snprintf(buf2, sizeof(buf2), "%s//%s_dar_input.csv", INPUTDIR, buf1);
	//myCSV->CSV_writeOptiData(buf2, mySHP, Ist);
	myCSV->CSV_writeOptiData_MS2(buf2, mySHP, Ist);

    // creates a shape with the requests
    mySHP->SHP_writeShapeRequests(buf1, Ist );
    
	// create a shapefile with the stoppoints
    mySHP->SHP_writeShapeStopPoints(buf1, Ist);
    
	// create a shapefile with the ParkingPoints
    mySHP->SHP_writeShapeParkingPoints(buf1, Ist);    

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
    snprintf(buf1, sizeof(buf1), "ISTANZA%d", Ist->SessionID);
    snprintf(buf, sizeof(buf), "%s//%s_dar_solution.csv", OUTPUTDIR, buf1);
	//myCSV->CSV_readOptiData(buf, mySHP, Ist);
	myCSV->CSV_readOptiData_MS2(buf, mySHP, Ist);

    // Write the output files
    myCSV->CVS_writeWaypoints(mySHP, Ist, buf1);
	//myCSV->CVS_writeRouteOld(Ist, buf1);
	myCSV->CVS_writeRoute(Ist, buf1);
	myCSV->CVS_writeRoutePoint(Ist, buf1);
	myCSV->CVS_writeRequestInRoute(Ist, buf1);
	myCSV->CVS_writeSolution();
	myCSV->CVS_writeSetup(Ist);

	// write shapefiles 
    // Write the shapefile with the waypoints path(s)
    mySHP->SHP_writeShapeFromWaypoints(buf1);        
    // creates a shapefile with the path (route))
    myCSV->CVS_writePath(mySHP, Ist, buf1);
    //

	return 0;
}