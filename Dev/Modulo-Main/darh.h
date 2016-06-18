/** \file DARH.h  
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of 
 *---------------------------------------------------------------------------
 */
#ifndef __DARHDEFS_H
#define __DARHDEFS_H

using namespace std;

/* Bring in the C library header files */
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <cstring>
#include <time.h>
#include <assert.h>
#include <ctype.h>
#include <algorithm> 
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <math.h>
#include <limits.h>
#include <float.h>

#define	MAXN_NODES					200	/// max. number of nodes 
#define	MAXN_VEHICLES				 10	/// max. number of vehicles 
#define	MAXN_VEHICLE_TYPES			  5	/// max. number of vehicle types 
#define	MAXN_VEHICLE_DESTINATIONS     5	/// max. number destinations for a vehicle 
#define	MAXN_VEHICLE_TWSLOTS		  3	/// max. number of time window slots for a vehicle 
#define	MAXN_REQUESTS				 50	/// max. number of transportation requests 
#define	MAXN_DEPOTS					 50	/// max. number of depots 
#define	MAXN_DIMENSIONS				  3	/// max. number of vehicle dimensions 

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

//#define VS

#ifdef VS
    // and Visual Studio defines
    #define snprintf sprintf_s // Solve a problem with Visual Studio 2013 which does not recognize the function "snprintf" included in the latest ANSI standard)
    #define INPUTDIR  "../input"
	#define OUTPUTDIR "../output"	
	#define SQLDIR "..\\Modulo-SQLServer"
#else
    #define INPUTDIR  "../input"
    #define OUTPUTDIR "../output" 
    #define SQLDIR "../Modulo-SQLServer"
#endif


/* Main header files*/

#include "../Modulo-Main/constants.h"
#include "../Modulo-Main/errors.h"


/* Shapelib header files*/
#include "../Modulo-ShapeLib/shapefil.h"

/* interface */
#include "../Modulo-InOut/C_IST.h"

/* In Out header files*/
#include "../Modulo-InOut/C_SHP.h"
#include "../Modulo-InOut/C_CSV.h"

/* Bring in DAR header files */
#include "../Modulo-Opti/include/random.h"
#include "../Modulo-Opti/include/utils.h"
#include "../Modulo-Opti/include/solution.h"
#include "../Modulo-Opti/include/DAR.h"
#include "../Modulo-Opti/include/hsv.h"
#include "../Modulo-Opti/include/sort.h"

/* ModuloIO header files*/
#include "../Modulo-InOut/ModuloIO.h"

/* Dijkstra header files*/
#include "../Modulo-Dijkstra/DijkstraObj.h"

#define DEBUG /*!< when defined, debug logs will be printed using \c flog stream  */

#ifdef MAIN
	// In SHP_Graph_Utilities.cpp "flog" must be declared, therefore this #ifdef must be removed!
	#ifdef DEBUG
		ofstream flog; /*!<  main log stream */		
	#endif
        char buf[250];
#else
	// In SHP_Graph_Utilities.cpp "flog" must be declared, therefore this #ifdef must be removed!
	#ifdef DEBUG 
		extern ofstream flog; /*!<  main log  stream */
	#endif
        extern char buf[250];
#endif

int main_DAR(char *Instance); // Main function

#endif /* __DARHDEFS_H */

// Error Logging management
extern CError error;

