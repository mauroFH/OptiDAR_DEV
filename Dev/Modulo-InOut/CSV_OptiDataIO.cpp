#include "../Modulo-Main/darh.h"

/**
* Read the solution computed by the optimizer
* @param OptiOutFileName input name of the CSV file containing the solution computed by the optimizer
* @param mySHP shapefiles data
* @param Ist istance data
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_readOptiData(char *OptiOutFileName, C_SHP *mySHP, C_IST *Ist)
{
	FILE *fin;
	//CError error;
	long numRec;
	long konta;
	int numFields;
	unsigned char *p, word[CON_MAXNDESCRSTOPPOINT];
	char word_dummy[CON_MAXNDESCRSTOPPOINT];
	static unsigned char line[512];
	long i, j;
	long k1, k2;
	long i1, i2;
	long j1, j2;
	long v1, v2;
	long konta1;
	long index;
	//long npred;
	long *v_vpred;
	long *v_apred;

	// Open file
	fin = fopen(OptiOutFileName, "r");
	if (fin == NULL)
	{
		snprintf(buf, sizeof(buf), "File %s cannot be opened", OptiOutFileName);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	}

	// Counts the records
	numRec = CSV_contaRec(fin);
	if (numRec < 1)
	{
		snprintf(buf, sizeof(buf), "File %s - No record, the file is empty", OptiOutFileName);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	}

	// Allocate data structure
	Ist->TRoute.nstop = numRec - 1;
	Ist->TRoute.v_stop_out = new struct route_out[Ist->TRoute.nstop];

	// riposiziona il file in testa
	fseek(fin, 0, SEEK_SET);

	// Inizio lettura
	numRec = 0;
	CSV_LeggiRec(fin, &numRec, (unsigned char *)line); // first record contains headings
	konta = 0;
	while (!feof(fin))
	{
		line[0] = '\0';
		CSV_LeggiRec(fin, &numRec, (unsigned char *)line);
		if (line[0] == '\0') continue;

		// Initialize
		p = &line[0];
		numFields = 0;

		//////// node index
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word))	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0)	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		// If it is a depot or a parking area we must align the index (remind that depots and park areas are doubled)
		Ist->TRoute.v_stop_out[konta].indexDT = i;
		if (i >= 2 * Ist->num_Requests)
			i = 2 * (i - 2 * Ist->num_Requests) + 2 * Ist->num_Requests;
		Ist->TRoute.v_stop_out[konta].indexSoP = Ist->v_DT_back[i].indexSoP;
		Ist->TRoute.v_stop_out[konta].indexReq = Ist->v_DT_back[i].indexReq;

		//////// Code
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		memcpy(Ist->TRoute.v_stop_out[konta].code, word, CON_MAXNCODSTOPPOINT);

		//////// Type
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		memcpy(buf, word, CON_MAXNCODSTOPPOINT);
		if (strcmp(buf, "VEHICLE_DEPOT") == 0)
		{
			Ist->TRoute.v_stop_out[konta].type = 0;
			Ist->TRoute.v_stop_out[konta].StopOrPark = true;
		}
		else if (strcmp(buf, "PICKUP") == 0)
		{
			Ist->TRoute.v_stop_out[konta].type = 1;
			Ist->TRoute.v_stop_out[konta].StopOrPark = false;
		}
		else if (strcmp(buf, "DELIVERY") == 0)
		{
			Ist->TRoute.v_stop_out[konta].type = 2;
			Ist->TRoute.v_stop_out[konta].StopOrPark = false;
		}
		else if (strcmp(buf, "PARKING_AREA") == 0)
		{
			Ist->TRoute.v_stop_out[konta].type = 3;
			Ist->TRoute.v_stop_out[konta].StopOrPark = true;
		}
		else
		{
			snprintf(buf, sizeof(buf), "File %s - Type not recognized - Record = %ld field %d", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}

		//////// distance
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word))	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0)	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].distance = i;

		//////// time
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word))	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0)	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].time = i;

		//////// seated
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word))	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0)	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].seated = i;

		//////// standing
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word))	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0)	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].standing = i;

		//////// disable
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word))	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0)	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].disable = i;

		//////// Arrival time
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word))	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0)	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].atime = i;

		//////// Departure time
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word))	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0)	{
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].dtime = i;

		konta++;
	}

	// Generate the stop list for the waypoints data structure
	// and generate the point/arc list for the waypoints data structure

	// Allocate dynamic data structures 
	Ist->TRoute.RArcs.arc = new long[Ist->TRoute.nstop * Ist->MatrixDT.nvert];
	v_vpred = new long[Ist->MatrixDT.nvert];
	v_apred = new long[Ist->MatrixDT.nvert];

	// Starting and ending arcs
	j1 = Ist->TRoute.v_stop_out[0].indexDT;
	j2 = Ist->TRoute.v_stop_out[1].indexDT;
	index = Ist->MatrixDT.m_mapi[j1][j2];
	Ist->TRoute.RArcs.sarc = Ist->v_DT_back[index].karc;
	Ist->TRoute.RArcs.soffset = (float)Ist->v_DT_back[index].offset;
	j1 = Ist->TRoute.v_stop_out[Ist->TRoute.nstop - 2].indexDT;
	j2 = Ist->TRoute.v_stop_out[Ist->TRoute.nstop - 1].indexDT;
	index = Ist->MatrixDT.m_mapj[j1][j2];
	Ist->TRoute.RArcs.earc = Ist->v_DT_back[index].karc;
	Ist->TRoute.RArcs.eoffset = (float)Ist->v_DT_back[index].offset;

	// Build lists
	konta = 0;
	Ist->TRoute.RArcs.npoints = 0;
	Ist->TRoute.RArcs.nstop = Ist->TRoute.nstop;
	Ist->TRoute.RArcs.arc[konta] = Ist->TRoute.RArcs.sarc;
	Ist->TRoute.v_stop_out[0].indexArc = konta;
	konta++;
	for (i = 1; i < Ist->TRoute.nstop; i++)
	{
		// Extract the sequence of arcs for travelling from i-1 to i
		konta1 = 0;
		j1 = Ist->TRoute.v_stop_out[i - 1].indexDT;
		j2 = Ist->TRoute.v_stop_out[i].indexDT;
		i1 = Ist->MatrixDT.m_mapi[j1][j2];
		i2 = Ist->MatrixDT.m_mapj[j1][j2];
		k1 = Ist->v_DT_back[i1].karc;
		k2 = Ist->v_DT_back[i2].karc;
		assert(k1 >= 0);
		assert(k2 >= 0);
		v1 = Ist->v_ArcsFS[k1].to;
		v2 = Ist->v_ArcsFS[k2].from;

		// The intermidiate waypoints are defined by "vpred" of the matrix DT (in reverse order)
		while (1)
		{
			if ((v2 == v1) || (Ist->MatrixDT.m_vpred[i1][v2] == -1))
				break;
			v_vpred[konta1] = Ist->MatrixDT.m_vpred[i1][v2];
			v_apred[konta1] = Ist->MatrixDT.m_apred[i1][v2];
			konta1++;
			v2 = Ist->MatrixDT.m_vpred[i1][v2];
		}

		// The intermidiate waypoints are saved in the correct order 
		for (j = konta1 - 1; j >= 0; j--)
		{
			Ist->TRoute.RArcs.arc[konta] = v_apred[j];
			konta++;
		}

		// Manage the case where two stops are on the same arc 
		if (Ist->v_DT_back[i2].karc != Ist->TRoute.RArcs.arc[konta - 1])
		{
			Ist->TRoute.RArcs.arc[konta] = Ist->v_DT_back[i2].karc;
			konta++;
		}
		Ist->TRoute.v_stop_out[i].indexArc = konta - 1;
	}
	Ist->TRoute.RArcs.npoints = konta;

	//// Perform a quick debug
#ifdef DEBUG
	CSV_OptiIODebug(Ist);
#endif

	// free memory
	delete[] v_vpred;
	delete[] v_apred;

	return 0;
}


/**
* Read the solution computed by the optimizer
* @param OptiOutFileName input name of the CSV file containing the solution computed by the optimizer
* @param mySHP shapefiles data
* @param Ist istance data
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_readOptiData_MS2(char *OptiOutFileName, C_SHP *mySHP, C_IST *Ist)
{
	FILE *fin;
	//CError error;
	long numRec;
	long konta;
	int numFields;
	unsigned char *p, word[CON_MAXNDESCRSTOPPOINT];
	char word_dummy[CON_MAXNDESCRSTOPPOINT];
	static unsigned char line[512];
	long i, j;
	long k1, k2;
	long i1, i2;
	long j1, j2;
	long v1, v2;
	long konta1;
	long index;
	//long npred;
	long *v_vpred;
	long *v_apred;

	// Open file
	fin = fopen(OptiOutFileName, "r");
	if (fin == NULL)
	{
		snprintf(buf, sizeof(buf), "File %s - Cannot be opened", OptiOutFileName);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	}

	// Counts the records
	numRec = CSV_contaRec(fin);
	if (numRec < 1)
	{
		snprintf(buf, sizeof(buf), "File %s - No record, the file is empty", OptiOutFileName);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	}

	// Allocate data structure
	Ist->TRoute.nstop = numRec - 2;  // Eliminate two rows: vehicle assigned and heading
	Ist->TRoute.v_stop_out = new struct route_out[Ist->TRoute.nstop];

	// Set position at the beginning of input file
	fseek(fin, 0, SEEK_SET);

	// Start reading
	// Read vehicle index
	numRec = 0;
	line[0] = '\0';
	CSV_LeggiRec(fin, &numRec, (unsigned char *)line);
	if (line[0] == '\0') 
	{
		snprintf(buf, sizeof(buf), "File %s - No record, the first row is empty", OptiOutFileName);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	}
	// Initialize
	p = &line[0];
	numFields = 0;
	//////// Head of the row
	if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
		snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	};
	//////// Vehicle index
	if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
		snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	};
	if (CSVnonIntero(word)) {
		snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	}
	memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
	sscanf(word_dummy, "%ld", &i);
	if (i < 0) {
		snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	}
	// If it is a depot or a parking area we must align the index (remind that depots and park areas are doubled)
	Ist->TRoute.VehicleID = Ist->VehOpti.ivehicle[i]; // Da correggere con l'ID.

	// Read heding
	numRec = 1;
	CSV_LeggiRec(fin, &numRec, (unsigned char *)line); // Second record contains headings

	// Read stops
	konta = 0;
	while (!feof(fin))
	{
		line[0] = '\0';
		CSV_LeggiRec(fin, &numRec, (unsigned char *)line);
		if (line[0] == '\0') continue;

		// Initialize
		p = &line[0];
		numFields = 0;

		//////// node index
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word)) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		// If it is a depot or a parking area we must align the index (remind that depots and park areas are doubled)
		Ist->TRoute.v_stop_out[konta].indexDT = i;
		if (i >= 2 * Ist->num_Requests)
			i = 2 * (i - 2 * Ist->num_Requests) + 2 * Ist->num_Requests;
		Ist->TRoute.v_stop_out[konta].indexSoP = Ist->v_DT_back[i].indexSoP;
		Ist->TRoute.v_stop_out[konta].indexReq = Ist->v_DT_back[i].indexReq;

		//////// Code
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		memcpy(Ist->TRoute.v_stop_out[konta].code, word, CON_MAXNCODSTOPPOINT);

		//////// Type
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		memcpy(buf, word, CON_MAXNCODSTOPPOINT);
		if (strcmp(buf, "VEHICLE_DEPOT") == 0)
		{
			Ist->TRoute.v_stop_out[konta].type = 0;
			Ist->TRoute.v_stop_out[konta].StopOrPark = true;
		}
		else if (strcmp(buf, "PICKUP") == 0)
		{
			Ist->TRoute.v_stop_out[konta].type = 1;
			Ist->TRoute.v_stop_out[konta].StopOrPark = false;
		}
		else if (strcmp(buf, "DELIVERY") == 0)
		{
			Ist->TRoute.v_stop_out[konta].type = 2;
			Ist->TRoute.v_stop_out[konta].StopOrPark = false;
		}
		else if (strcmp(buf, "PARKING_AREA") == 0)
		{
			Ist->TRoute.v_stop_out[konta].type = 3;
			Ist->TRoute.v_stop_out[konta].StopOrPark = true;
		}
		else
		{
			snprintf(buf, sizeof(buf), "File %s - Type not recognized - Record = %ld field %d", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}

		//////// distance
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word)) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].distance = i;

		//////// time
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word)) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].time = i;

		//////// seated
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word)) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].seated = i;

		//////// standing
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word)) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].standing = i;

		//////// disable
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word)) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].disable = i;

		//////// Arrival time
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word)) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].atime = i;

		//////// Departure time
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not found", OptiOutFileName, numRec, numFields);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		};
		if (CSVnonIntero(word)) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d not an integer = %s", OptiOutFileName, numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0) {
			snprintf(buf, sizeof(buf), "File %s - Record = %ld field %d negative = %ld", OptiOutFileName, numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		Ist->TRoute.v_stop_out[konta].dtime = i;

		konta++;
	}

	// Generate the stop list for the waypoints data structure
	// and generate the point/arc list for the waypoints data structure

	// Allocate dynamic data structures 
	Ist->TRoute.RArcs.arc = new long[Ist->TRoute.nstop * Ist->MatrixDT.nvert];
	v_vpred = new long[Ist->MatrixDT.nvert];
	v_apred = new long[Ist->MatrixDT.nvert];

	// Starting and ending arcs
	j1 = Ist->TRoute.v_stop_out[0].indexDT;
	j2 = Ist->TRoute.v_stop_out[1].indexDT;
	index = Ist->MatrixDT.m_mapi[j1][j2];
	Ist->TRoute.RArcs.sarc = Ist->v_DT_back[index].karc;
	Ist->TRoute.RArcs.soffset = (float)Ist->v_DT_back[index].offset;
	j1 = Ist->TRoute.v_stop_out[Ist->TRoute.nstop - 2].indexDT;
	j2 = Ist->TRoute.v_stop_out[Ist->TRoute.nstop - 1].indexDT;
	index = Ist->MatrixDT.m_mapj[j1][j2];
	Ist->TRoute.RArcs.earc = Ist->v_DT_back[index].karc;
	Ist->TRoute.RArcs.eoffset = (float)Ist->v_DT_back[index].offset;

	// Build lists
	konta = 0;
	Ist->TRoute.RArcs.npoints = 0;
	Ist->TRoute.RArcs.nstop = Ist->TRoute.nstop;
	Ist->TRoute.RArcs.arc[konta] = Ist->TRoute.RArcs.sarc;
	Ist->TRoute.v_stop_out[0].indexArc = konta;
	konta++;
	for (i = 1; i < Ist->TRoute.nstop; i++)
	{
		// Extract the sequence of arcs for travelling from i-1 to i
		konta1 = 0;
		j1 = Ist->TRoute.v_stop_out[i - 1].indexDT;
		j2 = Ist->TRoute.v_stop_out[i].indexDT;
		i1 = Ist->MatrixDT.m_mapi[j1][j2];
		i2 = Ist->MatrixDT.m_mapj[j1][j2];
		k1 = Ist->v_DT_back[i1].karc;
		k2 = Ist->v_DT_back[i2].karc;
		assert(k1 >= 0);
		assert(k2 >= 0);
		v1 = Ist->v_ArcsFS[k1].to;
		v2 = Ist->v_ArcsFS[k2].from;

		// The intermidiate waypoints are defined by "vpred" of the matrix DT (in reverse order)
		while (1)
		{
			if ((v2 == v1) || (Ist->MatrixDT.m_vpred[i1][v2] == -1))
				break;
			v_vpred[konta1] = Ist->MatrixDT.m_vpred[i1][v2];
			v_apred[konta1] = Ist->MatrixDT.m_apred[i1][v2];
			konta1++;
			v2 = Ist->MatrixDT.m_vpred[i1][v2];
		}

		// The intermidiate waypoints are saved in the correct order 
		for (j = konta1 - 1; j >= 0; j--)
		{
			Ist->TRoute.RArcs.arc[konta] = v_apred[j];
			konta++;
		}

		// Manage the case where two stops are on the same arc 
		if (Ist->v_DT_back[i2].karc != Ist->TRoute.RArcs.arc[konta - 1])
		{
			Ist->TRoute.RArcs.arc[konta] = Ist->v_DT_back[i2].karc;
			konta++;
		}
		Ist->TRoute.v_stop_out[i].indexArc = konta - 1;
	}
	Ist->TRoute.RArcs.npoints = konta;

	//// Perform a quick debug
#ifdef DEBUG
	CSV_OptiIODebug(Ist);
#endif

	// free memory
	delete[] v_vpred;
	delete[] v_apred;

	return 0;
}


/**
* Debug the solution computed by the optimizer
* @param Ist istance data
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_OptiIODebug(C_IST *Ist)
{
	long i, j, i1, i2;
	long j1, j2, k1, k2;
	long dist, time;
	long dist1, time1;

	//// Print the waypoints for debugging
	dist = 0;
	time = 0;
	dist1 = 0;
	time1 = 0;
	fprintf(Ist->flog,"\n  WAYPOINTS\n");
	for (i = 0; i < Ist->TRoute.nstop; i++)
	{
		if (i > 0)
		{
			j1 = Ist->TRoute.v_stop_out[i - 1].indexDT;
			j2 = Ist->TRoute.v_stop_out[i].indexDT;
			i1 = Ist->MatrixDT.m_mapi[j1][j2];
			i2 = Ist->MatrixDT.m_mapj[j1][j2];
			//i1 = Ist->TRoute.v_stop_out[i - 1].indexDT;
			//i2 = Ist->TRoute.v_stop_out[i].indexDT;
			dist1 += Ist->MatrixDT.m_length[i1][i2];
			time1 += Ist->MatrixDT.m_time[i1][i2];
		}

		k1 = Ist->TRoute.v_stop_out[i].indexArc;
		k2 = Ist->TRoute.RArcs.arc[k1];
		assert(k1 >= 0);
		assert(k2 >= 0);
		i1 = Ist->v_ArcsFS[k2].from;
		i2 = Ist->v_ArcsFS[k2].to;
		fprintf(Ist->flog, "  Arc stop %ld: indice=%ld  arc=%ld - (%ld,%ld) \n", i, k1, k2, i1, i2);

		if (i == 0)
		{
			j1 = Ist->TRoute.v_stop_out[0].indexDT;
			j2 = Ist->TRoute.v_stop_out[1].indexDT;
			i1 = Ist->MatrixDT.m_mapi[j1][j2];
			//dist -= (long)(Ist->v_DT_back[Ist->TRoute.v_stop_out[i].indexDT].offset * Ist->v_ArcsFS[k2].length);
			//time -= (long)(60.0 * Ist->v_DT_back[Ist->TRoute.v_stop_out[i].indexDT].offset * Ist->v_ArcsFS[k2].minutes);
			dist -= (long)(Ist->v_DT_back[i1].offset * Ist->v_ArcsFS[k2].length);
			time -= (long)(60.0 * Ist->v_DT_back[i1].offset * Ist->v_ArcsFS[k2].minutes);
		}

		if (i < Ist->TRoute.nstop - 1)
		{
			for (j = k1; j < Ist->TRoute.v_stop_out[i + 1].indexArc; j++)
			{
				fprintf(Ist->flog, " [%ld] arc=%ld - (%d,%d) ", j, Ist->TRoute.RArcs.arc[j], Ist->v_ArcsFS[Ist->TRoute.RArcs.arc[j]].from, Ist->v_ArcsFS[Ist->TRoute.RArcs.arc[j]].to);
				dist += Ist->v_ArcsFS[Ist->TRoute.RArcs.arc[j]].length;
				time += (long)(60.0*Ist->v_ArcsFS[Ist->TRoute.RArcs.arc[j]].minutes);
			}
		}
		fprintf(Ist->flog, "\n");

		if (i == Ist->TRoute.nstop - 1)
		{
			for (j = k1; j < Ist->TRoute.RArcs.npoints - 1; j++)
			{
				fprintf(Ist->flog, " [%ld] arc=%ld - (%d,%d) ", j, Ist->TRoute.RArcs.arc[j], Ist->v_ArcsFS[Ist->TRoute.RArcs.arc[j]].from, Ist->v_ArcsFS[Ist->TRoute.RArcs.arc[j]].to);
				dist += Ist->v_ArcsFS[Ist->TRoute.RArcs.arc[j]].length;
				time += (long)(60.0*Ist->v_ArcsFS[Ist->TRoute.RArcs.arc[j]].minutes);
			}
			j1 = Ist->TRoute.v_stop_out[Ist->TRoute.nstop - 2].indexDT;
			j2 = Ist->TRoute.v_stop_out[Ist->TRoute.nstop - 1].indexDT;
			i2 = Ist->MatrixDT.m_mapj[j1][j2];
			dist += (long)(Ist->v_DT_back[i2].offset * Ist->v_ArcsFS[k2].length);
			time += (long)(60.0 * Ist->v_DT_back[i2].offset * Ist->v_ArcsFS[k2].minutes);
		}

		fprintf(Ist->flog, "  Opti_distance = %ld - Opti_time = %ld \n", Ist->TRoute.v_stop_out[i].distance, Ist->TRoute.v_stop_out[i].time);
		fprintf(Ist->flog, "  Check_distance = %ld - Check_time = %ld \n", dist, time);
		fprintf(Ist->flog, "  Check_distance(MAT) = %ld - Check_time(MAT) = %ld \n", dist1, time1);
		fprintf(Ist->flog, "\n");
	}

	// Verifica connessione route
	for (j = 0; j < Ist->TRoute.RArcs.npoints - 1; j++)
	{
		if (Ist->v_ArcsFS[Ist->TRoute.RArcs.arc[j + 1]].from != Ist->v_ArcsFS[Ist->TRoute.RArcs.arc[j]].to)
		{
			fprintf(Ist->flog, "\n  ERROR PATH NOT CONNECTED!!!");
			//return 1;
		}
	}

	return 0;
}

/**
* Check the input data size
* @param Ist istance data
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_CheckInputDataSize(C_IST *Ist)
{
	if (Ist->num_VerticesFS <= 0)
	{
		error.fatal("No vertices in the road network", __FUNCTION__);
		return(-1);
	}

	if (Ist->num_ArcsFS <= 0)
	{
		error.fatal("No arcs are in the road network", __FUNCTION__);
		return(-1);
	}

	if (Ist->num_Points <= 0)
	{
		error.fatal("No points are in the road network", __FUNCTION__);
		return(-1);
	}

	if (Ist->num_Requests <= 0)
	{
		error.fatal("No request are specified", __FUNCTION__);
		return(-1);
	}

	if (Ist->num_Vehicles <= 0)
	{
		error.fatal("No vehicles are available", __FUNCTION__);
		return(-1);
	}

	return 0;
}

/**
* Write the data for the optimizer
* @param OptiInFileName input name of the CSV file containing the information for the optimizer
* @param mySHP shapefiles data
* @param Ist istance data
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_writeOptiData(char *OptiInFileName, C_SHP *mySHP, C_IST *Ist)
{
	long i, j;
	long i1, j1;
	long i2, j2;
	long ndep;
	long nreq, nreq1;
	long stop;
	int i_park;
	long ivehicle;
	long ivehiclesched = 0;
	long blength, btime;
	static char SEP = CON_CSVFILE_SEPARATOR;

	Dijkstra mappa;
	struct DPoint pstart;
	//long *dist, *time;
	//long *pred, *arcs;
	long ns;

	FILE *fin;
	//CError error;

	clock_t t1, t2;
	double dt;

	//cout << "\nWrite Opti Data ...\n"; cout.flush();
	
	// Open file
	fin = fopen(OptiInFileName, "w");
	if (fin==NULL)
	{
		snprintf(buf, sizeof(buf), "File %s - File cannot be opened", OptiInFileName);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	}

	// Check sul numero di richieste
	if (CSV_CheckInputDataSize(Ist))
		return -1;

	// Compute the number of depots
	ndep = Ist->num_ParkingPoints;

	// Allocate the data structure
	nreq = 2 * Ist->num_Requests + 2*ndep;  // We double depot/parking
	Ist->v_DT_back = new struct DPoint[nreq];
	//dist = new long[nreq];
	//time = new long[nreq];
	//pred = new long[Ist->num_VerticesFS];
	//arcs = new long[Ist->num_VerticesFS];
	Ist->MatrixDT.dim = nreq;
	Ist->MatrixDT.nvert  = Ist->num_VerticesFS;
	Ist->MatrixDT.m_length = new long*[Ist->MatrixDT.dim];
	Ist->MatrixDT.m_time = new long*[Ist->MatrixDT.dim];
	Ist->MatrixDT.m_vpred = new long*[Ist->MatrixDT.dim];
	Ist->MatrixDT.m_apred = new long*[Ist->MatrixDT.dim];
	Ist->MatrixDT.m_mapi = new long*[Ist->MatrixDT.dim];
	Ist->MatrixDT.m_mapj = new long*[Ist->MatrixDT.dim];
	for (i = 0; i < Ist->MatrixDT.dim; i++)
	{
		Ist->MatrixDT.m_length[i] = new long[Ist->MatrixDT.dim];
		Ist->MatrixDT.m_time[i] = new long[Ist->MatrixDT.dim];
		Ist->MatrixDT.m_vpred[i] = new long[Ist->MatrixDT.nvert];
		Ist->MatrixDT.m_apred[i] = new long[Ist->MatrixDT.nvert];
		Ist->MatrixDT.m_mapi[i] = new long[Ist->MatrixDT.dim];
		Ist->MatrixDT.m_mapj[i] = new long[Ist->MatrixDT.dim];
	}

	// Write #request and #depots
	fprintf(fin, "%d %c %ld %c \n", Ist->num_Requests, SEP, ndep, SEP);

	// Write vehicle data
	int kk = CSV_AssignVehicle(Ist, &ivehicle, &ivehiclesched); 
	if ( kk== -1)
	{
		snprintf(buf, sizeof(buf), "File %s - Vehicle not found", OptiInFileName);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	}
	Ist->TRoute.VehicleID = ivehicle;
	fprintf(fin, "%s %c %d %c %d %c %d %c %d %c %d %c %d %c %d %c %d %c \n", 
		Ist->v_Vehicles[ivehicle].Callsign, SEP, Ist->v_Vehicles[ivehicle].SeatedCapacity, SEP,
		Ist->v_Vehicles[ivehicle].StandingCapacity, SEP, Ist->v_Vehicles[ivehicle].WheelChairCapacity, SEP,
		Ist->v_Vehicles[ivehicle].TotalCapacity, SEP, 
		Ist->v_VehicleSchedules[ivehiclesched].StartTime, SEP, Ist->v_VehicleSchedules[ivehiclesched].EndTime, SEP,
		Ist->v_VehicleSchedules[ivehiclesched].i_StartDepot + 2 * Ist->num_Requests, SEP, 
		Ist->v_VehicleSchedules[ivehiclesched].i_EndDepot + 2 * Ist->num_Requests, SEP);

	///fprintf(fin, " code ; 3 ; 0 ; 0 ; 3 ; 0 ; 172800 ; 33 ; 33 ; \n");

	// Write pickups 
	nreq = 0;  // The index start from 0!
	for (i = 0; i < Ist->num_Requests; i++)
	{
		// Code Number
		fprintf(fin, "%ld %c ", nreq, SEP);

		// Code
		fprintf(fin, "%s %c ", Ist->v_Requests[i].Cod, SEP);

		// Coordinates (x,y)
		stop = Ist->v_Requests[i].i_pickup_stop;
		fprintf(fin, "%f %c %f %c ", Ist->v_Points[Ist->v_StopPoints[stop].i_Point].P.X, SEP,
                        Ist->v_Points[Ist->v_StopPoints[stop].i_Point].P.Y, SEP);

		// Type
		//fprintf(fin, "%d %c ", Ist->v_requests[i].pickup.type, SEP);

		// Time window
		fprintf(fin, "%d %c ", Ist->v_Requests[i].pickup.StartTime, SEP);
		fprintf(fin, "%d %c ", Ist->v_Requests[i].pickup.EndTime, SEP);

		// Service time
		fprintf(fin, "%d %c ", Ist->v_Requests[i].ServiceTime, SEP);  

		// Ride time
		fprintf(fin, "%d %c ", Ist->v_Requests[i].MaxTransit, SEP);

		// Demand
		// Temporary we read the first two (notice that we remove "type")
		fprintf(fin, "%ld %c ", (long)(Ist->v_Requests[i].LoadNormal), SEP);
		//fprintf(fin, "%d %c ", (long)(Ist->v_requests[i].pickup.v_demands[1]), SEP);
		fprintf(fin, " 0 ;");

		// New line
		fprintf(fin, "\n");

		// Add entry for the matrix computation
		stop = Ist->v_Requests[i].i_pickup_stop;
		Ist->v_DT_back[nreq].StopOrPark = (bool)0; // StopPoint
		Ist->v_DT_back[nreq].offset = Ist->v_Points[Ist->v_StopPoints[stop].i_Point].offset;
		Ist->v_DT_back[nreq].karc = Ist->v_Points[Ist->v_StopPoints[stop].i_Point].i_arc;
		Ist->v_DT_back[nreq].indexSoP = stop;
		Ist->v_DT_back[nreq].indexReq = i;
		Ist->v_DT_back[nreq].type = 1;

		// Update counter
		nreq++;
	}

	// Write deliveries
	for (i = 0; i < Ist->num_Requests; i++)
	{
		// Code Number
		fprintf(fin, "%ld %c ", nreq, SEP);

		// Code
		fprintf(fin, "%s %c ", Ist->v_Requests[i].Cod, SEP);

		// Coordinates (x,y)
		stop = Ist->v_Requests[i].i_dropoff_stop;
		fprintf(fin, "%f %c %f %c ", Ist->v_Points[Ist->v_StopPoints[stop].i_Point].P.X, SEP, 
                        Ist->v_Points[Ist->v_StopPoints[stop].i_Point].P.Y, SEP);

		// Type
		//fprintf(fin, "%d %c ", Ist->v_requests[i].delivery.type, SEP);

		// Time window
		fprintf(fin, "%d %c ", Ist->v_Requests[i].dropoff.StartTime, SEP);
		fprintf(fin, "%d %c ", Ist->v_Requests[i].dropoff.EndTime, SEP);

		// Service time
		fprintf(fin, "%d %c ", Ist->v_Requests[i].ServiceTime, SEP);  

		// Ride time
		fprintf(fin, "%d %c ", Ist->v_Requests[i].MaxTransit, SEP);

		// Demand
		// Temporary we read the first two (notice that we remove "type")
		fprintf(fin, "%d %c ", -(Ist->v_Requests[i].LoadNormal), SEP);
		//fprintf(fin, "%d %c ", -(long)(Ist->v_requests[i].delivery.v_demands[1]), SEP);
		fprintf(fin, " 0 ;");

		// New line
		fprintf(fin, "\n");

		fflush(fin);

		// Add entry for the matrix computation
		stop = Ist->v_Requests[i].i_dropoff_stop;
        Ist->v_DT_back[nreq].StopOrPark = (bool)0; // StopPoint
		Ist->v_DT_back[nreq].offset = Ist->v_Points[Ist->v_StopPoints[stop].i_Point].offset;
		Ist->v_DT_back[nreq].karc = Ist->v_Points[Ist->v_StopPoints[stop].i_Point].i_arc;
		Ist->v_DT_back[nreq].indexSoP = stop;
		Ist->v_DT_back[nreq].indexReq = i;
		Ist->v_DT_back[nreq].type = 2;

		// Update counter
		nreq++;
	}

	// Write depots 
	nreq1 = nreq;
	for (i = 0; i < Ist->num_ParkingPoints; i++)
	{
		// Code Number
		fprintf(fin, "%ld %c ", nreq, SEP);

		// Code
		fprintf(fin, "%s %c ", Ist->v_ParkingPoints[i].Cod, SEP);

		// Coordinates (x,y)
		fprintf(fin, "%f %c %f %c ", Ist->v_Points[Ist->v_ParkingPoints[i].i_Point].P.X, SEP,
			Ist->v_Points[Ist->v_ParkingPoints[i].i_Point].P.Y, SEP);

		// Type
		if (Ist->v_ParkingPoints[i].isDepot)
			fprintf(fin, "%d %c ", 0, SEP);
		else
			fprintf(fin, "%d %c ", 1, SEP);

		// New line
		fprintf(fin, "\n");

		// Add entry for the matrix computation (standard)
		i_park = i;
		Ist->v_DT_back[nreq1].StopOrPark = (bool)1; // ParkingPoint                        
		Ist->v_DT_back[nreq1].offset = Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].offset;
		Ist->v_DT_back[nreq1].karc = Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].i_arc;
		Ist->v_DT_back[nreq1].indexSoP = i_park;
		Ist->v_DT_back[nreq1].indexReq = -1;
		Ist->v_DT_back[nreq1].type = 0;
		nreq1++;

		// Add entry for the matrix computation (optional)
		i_park = i;
		Ist->v_DT_back[nreq1].StopOrPark = (bool)1; // ParkingPoint  
		if (Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].i_arc2 < 0)
		{
			Ist->v_DT_back[nreq1].offset = Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].offset;
			Ist->v_DT_back[nreq1].karc = Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].i_arc;
		}
		else
		{
			Ist->v_DT_back[nreq1].offset = Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].offset2;
			Ist->v_DT_back[nreq1].karc = Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].i_arc2;
		}
		Ist->v_DT_back[nreq1].indexSoP = i_park;
		Ist->v_DT_back[nreq1].indexReq = -1;
		Ist->v_DT_back[nreq1].type = 0;
		nreq1++;

		// Update counter
		nreq++;
	}

	// Write the distance matrix

	// Initialize the object (i.e., allocate data structure, etc.)
	mappa.Initialize(Ist->num_VerticesFS, Ist->num_ArcsFS);

	// Load the Forward Star
	for (i = 0; i < Ist->num_VerticesFS + 1; i++)
		mappa.index[i] = Ist->v_fromFS[i];

	for (j = 0; j < Ist->num_ArcsFS; j++)
	{
		mappa.iver[j] = Ist->v_ArcsFS[j].from;
		mappa.jver[j] = Ist->v_ArcsFS[j].to;
		mappa.darc[j] = Ist->v_ArcsFS[j].length;  // To remove when possible...
		mappa.distanza[j] = Ist->v_ArcsFS[j].length;
		mappa.tempo[j] = (long)(60.*Ist->v_ArcsFS[j].minutes);
		//mappa.costo[j] = (long)(60.*Ist->v_ArcsFS[j].minutes);
		//mappa.costo[j] = Ist->v_ArcsFS[j].length;
		mappa.costo[j] = mappa.DefineCost(Ist->Param.PathType, Ist->Param.AvgSpeed, mappa.distanza[j], mappa.tempo[j]);
	}

	// Compute matrix by rows
	t1 = clock();
	for (i = 0; i < Ist->MatrixDT.dim; i++)
	{
		// Starting point
		pstart.offset = Ist->v_DT_back[i].offset;
		pstart.karc = Ist->v_DT_back[i].karc;

		// Compute the row of the matrix
		ns = Ist->MatrixDT.dim;
		mappa.ComputePaths(pstart, ns, Ist->v_DT_back, Ist->MatrixDT.m_length[i], Ist->MatrixDT.m_time[i], 
                        Ist->MatrixDT.m_vpred[i], Ist->MatrixDT.m_apred[i]);
	}
	t2 = clock();
	dt = (double)(t2 - t1) / CLOCKS_PER_SEC;
	if (Ist->debug)
		printf("\n  Calcolo matrice Time.sec: %6.3f\n", dt);

	// Write Matrix DT
	t1 = clock();
	for (i = 0; i < nreq; i++)
	{  
		// Define the index in the matrix DT
		if (i < 2 * Ist->num_Requests)
		{
			i1 = i;
			i2 = i;
		}
		else
		{
			i2 = i - 2 * Ist->num_Requests;
			i1 = 2 * Ist->num_Requests + 2 * i2;
			i2 = 2 * Ist->num_Requests + 2 * i2 + 1;
		}

		// Write the matrix row entries ( 2 * Ist->num_Requests + 2 * ndep)
		for (j = 0; j < nreq; j++)
		{
			if (i == j)
				continue;

			// Define the index in the matrix DT
			if (j < 2 * Ist->num_Requests)
			{
				j1 = j;
				j2 = j;
			}
			else
			{
				j2 = j - 2 * Ist->num_Requests;
				j1 = 2 * Ist->num_Requests + 2 * j2;
				j2 = 2 * Ist->num_Requests + 2 * j2 + 1;
			}

			// Define the minimum time path
			blength = Ist->MatrixDT.m_length[i1][j1];
			btime = Ist->MatrixDT.m_time[i1][j1];
			Ist->MatrixDT.m_mapi[i][j] = i1;
			Ist->MatrixDT.m_mapj[i][j] = j1;
			if (btime > Ist->MatrixDT.m_time[i1][j2])
			{
				blength = Ist->MatrixDT.m_length[i1][j2];
				btime = Ist->MatrixDT.m_time[i1][j2];
				Ist->MatrixDT.m_mapi[i][j] = i1;
				Ist->MatrixDT.m_mapj[i][j] = j2;
			}
			if (btime > Ist->MatrixDT.m_time[i2][j1])
			{
				blength = Ist->MatrixDT.m_length[i2][j1];
				btime = Ist->MatrixDT.m_time[i2][j1];
				Ist->MatrixDT.m_mapi[i][j] = i2;
				Ist->MatrixDT.m_mapj[i][j] = j1;
			}
			if (btime > Ist->MatrixDT.m_time[i2][j2])
			{
				blength = Ist->MatrixDT.m_length[i2][j2];
				btime = Ist->MatrixDT.m_time[i2][j2];
				Ist->MatrixDT.m_mapi[i][j] = i2;
				Ist->MatrixDT.m_mapj[i][j] = j2;
			}

			// Row entry
			fprintf(fin, "%ld %c %ld %c ", i, SEP, j, SEP);
			fprintf(fin, "%ld %c %ld %c ", blength, SEP, btime, SEP);

			// New line
			fprintf(fin, "\n");
		}
	}
	t2 = clock();
	dt = (double)(t2 - t1) / CLOCKS_PER_SEC;
	if (Ist->debug)
		printf("\n  Scrittura matrice Time.sec: %6.3f\n\n", dt);

//terminate:

	fclose(fin);

	return 0;
}


/**
* Write the data for the optimizer (MileStone 2)
* @param OptiInFileName input name of the CSV file containing the information for the optimizer
* @param mySHP shapefiles data
* @param Ist istance data
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_writeOptiData_MS2(char *OptiInFileName, C_SHP *mySHP, C_IST *Ist)
{
	long i, j;
	long i1, j1;
	long i2, j2;
	long ndep;
	long nreq, nreq1;
	long nvei;
	long stop;
	int i_park;
	long ivehicle;
	long ivehiclesched = 0;
	long blength, btime;
	static char SEP = CON_CSVFILE_SEPARATOR;

	Dijkstra mappa;
	struct DPoint pstart;
	//long *dist, *time;
	//long *pred, *arcs;
	long ns;

	FILE *fin;
	//CError error;

	clock_t t1, t2;
	double dt;

	//cout << "\nWrite Opti Data ...\n"; cout.flush();

	// Open file
	fin = fopen(OptiInFileName, "w");
	if (fin == NULL)
	{
		snprintf(buf, sizeof(buf), "File %s - File cannot be opened", OptiInFileName);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	}

	// Check sul numero di richieste
	if (CSV_CheckInputDataSize(Ist))
		return -1;

	// Compute the number of depots
	ndep = Ist->num_ParkingPoints;

	// Allocate the data structure
	nreq = 2 * Ist->num_Requests + 2 * ndep;  // We double depot/parking
	Ist->v_DT_back = new struct DPoint[nreq];
	//dist = new long[nreq];
	//time = new long[nreq];
	//pred = new long[Ist->num_VerticesFS];
	//arcs = new long[Ist->num_VerticesFS];
	Ist->MatrixDT.dim = nreq;
	Ist->MatrixDT.nvert = Ist->num_VerticesFS;
	Ist->MatrixDT.m_length = new long*[Ist->MatrixDT.dim];
	Ist->MatrixDT.m_time = new long*[Ist->MatrixDT.dim];
	Ist->MatrixDT.m_vpred = new long*[Ist->MatrixDT.dim];
	Ist->MatrixDT.m_apred = new long*[Ist->MatrixDT.dim];
	Ist->MatrixDT.m_mapi = new long*[Ist->MatrixDT.dim];
	Ist->MatrixDT.m_mapj = new long*[Ist->MatrixDT.dim];
	for (i = 0; i < Ist->MatrixDT.dim; i++)
	{
		Ist->MatrixDT.m_length[i] = new long[Ist->MatrixDT.dim];
		Ist->MatrixDT.m_time[i] = new long[Ist->MatrixDT.dim];
		Ist->MatrixDT.m_vpred[i] = new long[Ist->MatrixDT.nvert];
		Ist->MatrixDT.m_apred[i] = new long[Ist->MatrixDT.nvert];
		Ist->MatrixDT.m_mapi[i] = new long[Ist->MatrixDT.dim];
		Ist->MatrixDT.m_mapj[i] = new long[Ist->MatrixDT.dim];
	}
	nvei = Ist->num_Vehicles;
	if (nvei < Ist->num_VehicleSchedules)
		nvei = Ist->num_VehicleSchedules;
	Ist->VehOpti.ivehicle = new long[nvei];
	Ist->VehOpti.ivehiclesched = new long[nvei];

	// Select the available vehicles
	Ist->VehOpti.nveh = 0;
	int kk = CSV_AssignVehicle_MS2(Ist, &Ist->VehOpti.nveh, Ist->VehOpti.ivehicle, Ist->VehOpti.ivehiclesched);
	if (kk == -1)
	{
		snprintf(buf, sizeof(buf), "File %s - Vehicle not found", OptiInFileName);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	}

	// Write #request, #depots, and #vehicles
	nvei = Ist->VehOpti.nveh;
	fprintf(fin, "%d %c %ld %c %ld %c \n", Ist->num_Requests, SEP, ndep, SEP, nvei, SEP);

	// Write vehicle data
	for (i = 0; i < Ist->VehOpti.nveh; i++)
	{
		ivehicle = Ist->VehOpti.ivehicle[i];
		ivehiclesched = Ist->VehOpti.ivehiclesched[i];

		//Ist->TRoute.VehicleID = ivehicle;
		fprintf(fin, "%s %c %d %c %d %c %d %c %d %c %d %c %d %c %d %c %d %c \n",
			Ist->v_Vehicles[ivehicle].Callsign, SEP, Ist->v_Vehicles[ivehicle].SeatedCapacity, SEP,
			Ist->v_Vehicles[ivehicle].StandingCapacity, SEP, Ist->v_Vehicles[ivehicle].WheelChairCapacity, SEP,
			Ist->v_Vehicles[ivehicle].TotalCapacity, SEP,
			Ist->v_VehicleSchedules[ivehiclesched].StartTime, SEP, Ist->v_VehicleSchedules[ivehiclesched].EndTime, SEP,
			Ist->v_VehicleSchedules[ivehiclesched].i_StartDepot + 2 * Ist->num_Requests, SEP,
			Ist->v_VehicleSchedules[ivehiclesched].i_EndDepot + 2 * Ist->num_Requests, SEP);
	}

	// Write pickups 
	nreq = 0;  // The index start from 0!
	for (i = 0; i < Ist->num_Requests; i++)
	{
		// Code Number
		fprintf(fin, "%ld %c ", nreq, SEP);

		// Code
		fprintf(fin, "%s %c ", Ist->v_Requests[i].Cod, SEP);

		// Coordinates (x,y)
		stop = Ist->v_Requests[i].i_pickup_stop;
		fprintf(fin, "%f %c %f %c ", Ist->v_Points[Ist->v_StopPoints[stop].i_Point].P.X, SEP,
			Ist->v_Points[Ist->v_StopPoints[stop].i_Point].P.Y, SEP);

		// Type
		//fprintf(fin, "%d %c ", Ist->v_requests[i].pickup.type, SEP);

		// Time window
		if (Ist->v_Requests[i].ConfirmedActionTypeID == 0)
		{
			fprintf(fin, "%d %c ", Ist->v_Requests[i].ConfirmedTime - Ist->v_Requests[i].ConfirmedRange, SEP);
			fprintf(fin, "%d %c ", Ist->v_Requests[i].ConfirmedTime + Ist->v_Requests[i].ConfirmedRange, SEP);
		}
		else
		{
			fprintf(fin, "%d %c ", Ist->v_Requests[i].pickup.StartTime, SEP);
			fprintf(fin, "%d %c ", Ist->v_Requests[i].pickup.EndTime, SEP);
		}

		// Service time
		fprintf(fin, "%d %c ", Ist->v_Requests[i].ServiceTime, SEP);

		// Ride time
		fprintf(fin, "%d %c ", Ist->v_Requests[i].MaxTransit, SEP);

		// Demand
		// Temporary we read the first two (notice that we remove "type")
		fprintf(fin, "%ld %c ", (long)(Ist->v_Requests[i].LoadNormal), SEP);
		//fprintf(fin, "%d %c ", (long)(Ist->v_requests[i].pickup.v_demands[1]), SEP);
		fprintf(fin, " 0 ;");

		// Fixed
		if (Ist->v_Requests[i].ConfirmedActionTypeID == 0)
			fprintf(fin, " 1 ;");
		else
			fprintf(fin, " 0 ;");

		// Paired Request
		fprintf(fin, " -1 ;");

		// New line
		fprintf(fin, "\n");

		// Add entry for the matrix computation
		stop = Ist->v_Requests[i].i_pickup_stop;
		Ist->v_DT_back[nreq].StopOrPark = (bool)0; // StopPoint
		Ist->v_DT_back[nreq].offset = Ist->v_Points[Ist->v_StopPoints[stop].i_Point].offset;
		Ist->v_DT_back[nreq].karc = Ist->v_Points[Ist->v_StopPoints[stop].i_Point].i_arc;
		Ist->v_DT_back[nreq].indexSoP = stop;
		Ist->v_DT_back[nreq].indexReq = i;
		Ist->v_DT_back[nreq].type = 1;

		// Update counter
		nreq++;
	}

	// Write deliveries
	for (i = 0; i < Ist->num_Requests; i++)
	{
		// Code Number
		fprintf(fin, "%ld %c ", nreq, SEP);

		// Code
		fprintf(fin, "%s %c ", Ist->v_Requests[i].Cod, SEP);

		// Coordinates (x,y)
		stop = Ist->v_Requests[i].i_dropoff_stop;
		fprintf(fin, "%f %c %f %c ", Ist->v_Points[Ist->v_StopPoints[stop].i_Point].P.X, SEP,
			Ist->v_Points[Ist->v_StopPoints[stop].i_Point].P.Y, SEP);

		// Type
		//fprintf(fin, "%d %c ", Ist->v_requests[i].delivery.type, SEP);

		// Time window
		if (Ist->v_Requests[i].ConfirmedActionTypeID == 1)
		{
			fprintf(fin, "%d %c ", Ist->v_Requests[i].ConfirmedTime - Ist->v_Requests[i].ConfirmedRange, SEP);
			fprintf(fin, "%d %c ", Ist->v_Requests[i].ConfirmedTime + Ist->v_Requests[i].ConfirmedRange, SEP);
		}
		else
		{
			fprintf(fin, "%d %c ", Ist->v_Requests[i].dropoff.StartTime, SEP);
			fprintf(fin, "%d %c ", Ist->v_Requests[i].dropoff.EndTime, SEP);
		}

		// Service time
		fprintf(fin, "%d %c ", Ist->v_Requests[i].ServiceTime, SEP);

		// Ride time
		fprintf(fin, "%d %c ", Ist->v_Requests[i].MaxTransit, SEP);

		// Demand
		// Temporary we read the first two (notice that we remove "type")
		fprintf(fin, "%d %c ", -(Ist->v_Requests[i].LoadNormal), SEP);
		//fprintf(fin, "%d %c ", -(long)(Ist->v_requests[i].delivery.v_demands[1]), SEP);
		fprintf(fin, " 0 ;");

		// Fixed
		if (Ist->v_Requests[i].ConfirmedActionTypeID == 1)
			fprintf(fin, " 1 ;");
		else
			fprintf(fin, " 0 ;");

		// Paired Request
		fprintf(fin, " -1 ;");

		// New line
		fprintf(fin, "\n");

		fflush(fin);

		// Add entry for the matrix computation
		stop = Ist->v_Requests[i].i_dropoff_stop;
		Ist->v_DT_back[nreq].StopOrPark = (bool)0; // StopPoint
		Ist->v_DT_back[nreq].offset = Ist->v_Points[Ist->v_StopPoints[stop].i_Point].offset;
		Ist->v_DT_back[nreq].karc = Ist->v_Points[Ist->v_StopPoints[stop].i_Point].i_arc;
		Ist->v_DT_back[nreq].indexSoP = stop;
		Ist->v_DT_back[nreq].indexReq = i;
		Ist->v_DT_back[nreq].type = 2;

		// Update counter
		nreq++;
	}

	// Write depots 
	nreq1 = nreq;
	for (i = 0; i < Ist->num_ParkingPoints; i++)
	{
		// Code Number
		fprintf(fin, "%ld %c ", nreq, SEP);

		// Code
		fprintf(fin, "%s %c ", Ist->v_ParkingPoints[i].Cod, SEP);

		// Coordinates (x,y)
		fprintf(fin, "%f %c %f %c ", Ist->v_Points[Ist->v_ParkingPoints[i].i_Point].P.X, SEP,
			Ist->v_Points[Ist->v_ParkingPoints[i].i_Point].P.Y, SEP);

		// Type
		if (Ist->v_ParkingPoints[i].isDepot)
			fprintf(fin, "%d %c ", 0, SEP);
		else
			fprintf(fin, "%d %c ", 1, SEP);

		// New line
		fprintf(fin, "\n");

		// Add entry for the matrix computation (standard)
		i_park = i;
		Ist->v_DT_back[nreq1].StopOrPark = (bool)1; // ParkingPoint                        
		Ist->v_DT_back[nreq1].offset = Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].offset;
		Ist->v_DT_back[nreq1].karc = Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].i_arc;
		Ist->v_DT_back[nreq1].indexSoP = i_park;
		Ist->v_DT_back[nreq1].indexReq = -1;
		Ist->v_DT_back[nreq1].type = 0;
		nreq1++;

		// Add entry for the matrix computation (optional)
		i_park = i;
		Ist->v_DT_back[nreq1].StopOrPark = (bool)1; // ParkingPoint  
		if (Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].i_arc2 < 0)
		{
			Ist->v_DT_back[nreq1].offset = Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].offset;
			Ist->v_DT_back[nreq1].karc = Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].i_arc;
		}
		else
		{
			Ist->v_DT_back[nreq1].offset = Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].offset2;
			Ist->v_DT_back[nreq1].karc = Ist->v_Points[Ist->v_ParkingPoints[i_park].i_Point].i_arc2;
		}
		Ist->v_DT_back[nreq1].indexSoP = i_park;
		Ist->v_DT_back[nreq1].indexReq = -1;
		Ist->v_DT_back[nreq1].type = 0;
		nreq1++;

		// Update counter
		nreq++;
	}

	// Write the distance matrix

	// Initialize the object (i.e., allocate data structure, etc.)
	mappa.Initialize(Ist->num_VerticesFS, Ist->num_ArcsFS);

	// Load the Forward Star
	for (i = 0; i < Ist->num_VerticesFS + 1; i++)
		mappa.index[i] = Ist->v_fromFS[i];

	for (j = 0; j < Ist->num_ArcsFS; j++)
	{
		mappa.iver[j] = Ist->v_ArcsFS[j].from;
		mappa.jver[j] = Ist->v_ArcsFS[j].to;
		mappa.darc[j] = Ist->v_ArcsFS[j].length;  // To remove when possible...
		mappa.distanza[j] = Ist->v_ArcsFS[j].length;
		mappa.tempo[j] = (long)(60.*Ist->v_ArcsFS[j].minutes);
		//mappa.costo[j] = (long)(60.*Ist->v_ArcsFS[j].minutes);
		//mappa.costo[j] = Ist->v_ArcsFS[j].length;
		mappa.costo[j] = mappa.DefineCost(Ist->Param.PathType, Ist->Param.AvgSpeed, mappa.distanza[j], mappa.tempo[j]);
	}

	// Compute matrix by rows
	t1 = clock();
	for (i = 0; i < Ist->MatrixDT.dim; i++)
	{
		// Starting point
		pstart.offset = Ist->v_DT_back[i].offset;
		pstart.karc = Ist->v_DT_back[i].karc;

		// Compute the row of the matrix
		ns = Ist->MatrixDT.dim;
		mappa.ComputePaths(pstart, ns, Ist->v_DT_back, Ist->MatrixDT.m_length[i], Ist->MatrixDT.m_time[i],
			Ist->MatrixDT.m_vpred[i], Ist->MatrixDT.m_apred[i]);
	}
	t2 = clock();
	dt = (double)(t2 - t1) / CLOCKS_PER_SEC;
	if (Ist->debug)
		printf("\n  Calcolo matrice Time.sec: %6.3f\n", dt);

	// Write Matrix DT
	t1 = clock();
	for (i = 0; i < nreq; i++)
	{
		// Define the index in the matrix DT
		if (i < 2 * Ist->num_Requests)
		{
			i1 = i;
			i2 = i;
		}
		else
		{
			i2 = i - 2 * Ist->num_Requests;
			i1 = 2 * Ist->num_Requests + 2 * i2;
			i2 = 2 * Ist->num_Requests + 2 * i2 + 1;
		}

		// Write the matrix row entries ( 2 * Ist->num_Requests + 2 * ndep)
		for (j = 0; j < nreq; j++)
		{
			if (i == j)
				continue;

			// Define the index in the matrix DT
			if (j < 2 * Ist->num_Requests)
			{
				j1 = j;
				j2 = j;
			}
			else
			{
				j2 = j - 2 * Ist->num_Requests;
				j1 = 2 * Ist->num_Requests + 2 * j2;
				j2 = 2 * Ist->num_Requests + 2 * j2 + 1;
			}

			// Define the minimum time path
			blength = Ist->MatrixDT.m_length[i1][j1];
			btime = Ist->MatrixDT.m_time[i1][j1];
			Ist->MatrixDT.m_mapi[i][j] = i1;
			Ist->MatrixDT.m_mapj[i][j] = j1;
			if (btime > Ist->MatrixDT.m_time[i1][j2])
			{
				blength = Ist->MatrixDT.m_length[i1][j2];
				btime = Ist->MatrixDT.m_time[i1][j2];
				Ist->MatrixDT.m_mapi[i][j] = i1;
				Ist->MatrixDT.m_mapj[i][j] = j2;
			}
			if (btime > Ist->MatrixDT.m_time[i2][j1])
			{
				blength = Ist->MatrixDT.m_length[i2][j1];
				btime = Ist->MatrixDT.m_time[i2][j1];
				Ist->MatrixDT.m_mapi[i][j] = i2;
				Ist->MatrixDT.m_mapj[i][j] = j1;
			}
			if (btime > Ist->MatrixDT.m_time[i2][j2])
			{
				blength = Ist->MatrixDT.m_length[i2][j2];
				btime = Ist->MatrixDT.m_time[i2][j2];
				Ist->MatrixDT.m_mapi[i][j] = i2;
				Ist->MatrixDT.m_mapj[i][j] = j2;
			}

			// Row entry
			fprintf(fin, "%ld %c %ld %c ", i, SEP, j, SEP);
			fprintf(fin, "%ld %c %ld %c ", blength, SEP, btime, SEP);

			// New line
			fprintf(fin, "\n");
		}
	}
	t2 = clock();
	dt = (double)(t2 - t1) / CLOCKS_PER_SEC;
	if (Ist->debug)
		printf("\n  Scrittura matrice Time.sec: %6.3f\n\n", dt);

	//terminate:

	fclose(fin);

	return 0;
}


/**
* Assign the correct vehicle
* @param Ist instance data
* @param ivehicle vehicle index
* @param ivehiclesched vehicle scheduled index
* @param Ist instance data
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_AssignVehicle(C_IST *Ist, long *ivehicle, long *ivehiclesched)
{
	long i;

	// In the request a vehicle is specified?
	if (Ist->v_Requests[0].i_Vheicle >= 0)
	{
		*ivehicle = Ist->v_Requests[0].i_Vheicle;
		*ivehiclesched = -1;
		for (i = 0; i < Ist->num_VehicleSchedules; i++)
			if (Ist->v_VehicleSchedules[i].i_Vehicle == 0)
			{
				*ivehiclesched = i;
				break;
			}
		if (*ivehiclesched >= 0)
			return 0;
	}

	// In the request a vehicle type is specified?
	if (Ist->v_Requests[0].i_TypeOfVehicle >= 0)
	{
		*ivehicle = -1;
		*ivehiclesched = -1;
		for (i = 0; i < Ist->num_Vehicles; i++)
			if (Ist->v_Vehicles[i].i_TypeOfVehicle == Ist->v_TypeOfVehicles[Ist->v_Requests[0].i_TypeOfVehicle].TypeOfVehicleId)
			{
				*ivehicle = i;
				break;
			}
		if (ivehicle >= 0)
		{
			for (i = 0; i < Ist->num_VehicleSchedules; i++)
				if (Ist->v_VehicleSchedules[i].i_Vehicle == 0)
				{
					*ivehiclesched = i;
					break;
				}
		}
		if (*ivehiclesched >= 0)
			return 0;
	}

	// we use the first vehicle available in the table "Vehicle"
	*ivehicle = 0;
	*ivehiclesched = -1;
	for (i = 0; i < Ist->num_VehicleSchedules; i++)
		if (Ist->v_VehicleSchedules[i].i_Vehicle == 0)
		{
			*ivehiclesched = i;
			break;
		}
	if (*ivehiclesched >= 0)
		return 0;

	// Vehicle not available!!!
	return -1;
}

/**
* Assign the correct vehicle
* @param Ist instance data
* @param ivehicle vehicle index
* @param ivehiclesched vehicle scheduled index
* @param Ist instance data
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_AssignVehicle_MS2(C_IST *Ist, long *nveh, long *ivehicle, long *ivehiclesched)
{
	long i, i1;

	// Initialize
	*nveh = 0;

	// In the request a vehicle is specified?
	if (Ist->v_Requests[0].i_Vheicle >= 0)
	{
		i = Ist->v_Requests[0].i_Vheicle;
		for (i1 = 0; i1 < Ist->num_VehicleSchedules; i1++)
			if (Ist->v_VehicleSchedules[i1].i_Vehicle == i)
			{
				ivehicle[*nveh] = i;
				ivehiclesched[*nveh] = i1;
				(*nveh) += 1;
			}

		// If at least one vehicle is found, STOP
		if ((*nveh) > 0)
			return 0;
	}

	// In the request a vehicle type is specified?
	if (Ist->v_Requests[0].i_TypeOfVehicle >= 0)
	{
		for (i = 0; i < Ist->num_Vehicles; i++)
		{
			//if (Ist->v_Vehicles[i].i_TypeOfVehicle == Ist->v_TypeOfVehicles[Ist->v_Requests[0].i_TypeOfVehicle].TypeOfVehicleId)
			if (Ist->v_Vehicles[i].i_TypeOfVehicle == Ist->v_Requests[0].i_TypeOfVehicle)
			{
				for (i1 = 0; i1 < Ist->num_VehicleSchedules; i1++)
					if (Ist->v_VehicleSchedules[i1].i_Vehicle == i)
					{
						ivehicle[*nveh] = i;
						ivehiclesched[*nveh] = i1;
						(*nveh) += 1;
					}
			}
		}

		// If at least one vehicle is found, STOP
		if ((*nveh) > 0)
			return 0;
	}

	// we use the available vehicles in the table "Vehicle"
	for (i = 0; i < Ist->num_Vehicles; i++)
	{
		for (i1 = 0; i1 < Ist->num_VehicleSchedules; i1++)
			if (Ist->v_VehicleSchedules[i1].i_Vehicle == i)
			{
				ivehicle[*nveh] = i;
				ivehiclesched[*nveh] = i1;
				(*nveh) += 1;
			}
	}

	// If at least one vehicle is found, STOP
	if ((*nveh) > 0)
		return 0;

	// Vehicle not available!!!
	return -1;
}
