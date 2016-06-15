/** \file main_DAR.cpp
 * DAR Console application.
 * Version 1.0.0
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of
 * --------------------------------------------------------------------------
 */

#define MAIN
//#define CONSOLE

#include "../../Modulo-Main/DARH.h"

/**
* Main function (integrated project)
*/
#ifdef CONSOLE
	// Error Logging management
	CError error;

	void main(void)
	{									
		char Instance[30] = "ISTANZA33";	// to be excluded for the integrated project
#else
	int main_DAR(char *Instance) // to be activated for the integrated project
	{
#endif
	int status = 0;
	//CError error;	// Error handling
	CDar dar;	// DAR
	
	// Open the log output file
#ifdef DEBUG
	clock_t tstart, tend;
	double ttime;
	tstart = clock();
	snprintf(buf,sizeof(buf),"%s//%s_dar.log",OUTPUTDIR,Instance);
	flog.open(buf, ios::out);
	if (!flog.is_open()) error.fatal("File opening", __FUNCTION__);
	flog << "HSV heuristic" << endl;
#endif
#ifdef DEBUG
		cout << "\nDAR input..."; cout << "\nDAR print...";        
#endif
	// Load an example and populate the DAR instance
	dar.input(Instance);
	// Print instance data
#ifdef DEBUG
	cout << "\nDAR print..."; cout << "\nDAR print...";        
	dar.print_data(Instance);
#endif
#ifdef DEBUG
	dar.out_data(Instance);
#endif
	// Single-vehicle heuristic
#ifdef DEBUG
		cout << "\nDAR solve..."; cout.flush();
#endif
	if (dar.flg_instance_loaded){
		// Perfors preliminary checks
		dar.checkrequests();
		// Applu heuristic
		CHsv heuristic;
		heuristic.solve(dar); // the solution is saved in DAR
		dar.out_solution(Instance);
	}
	else
		error.fatal("Solution not loaded", __FUNCTION__);
#ifdef DEBUG
	tend = clock();
	ttime = (double)(tend - tstart) / CLOCKS_PER_SEC;
	flog << endl << "Total computing time " << ttime << endl;
	flog.close();
#endif

#ifndef CONSOLE
	return (status);
#endif
}
