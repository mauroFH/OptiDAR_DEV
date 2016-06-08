#include "../Modulo-Main/darh.h"

#define LATLONG

#ifdef LATLONG
#include "LatLong.h"
#endif

/** @name Seach procedures
 */
//@{

/**
 * Search Stop
 * @param stopCode input alphanumerical code to be searched
 * @param Ist      input network clas
 * @return -1 if the code does not exists; position of the stop in v_Stops[], otherwise
 */
int C_CSV::CSVsearchStop(C_IST *Ist, unsigned char *stopCode){
	int i;
	for (i = 0; i < Ist->num_Points; i++){
		//        if (strncmp( (char *)Ist->v_Stops[i].Cod,(char *)stopCode,CON_MAXNCODSTOP) == 0) return i;
	}
	return -1;
}

/**
 * Search PointId
 * @param Id     input  ID to be searched
 * @param Ist    input network clas
 * @return -1    if the code does not exists; position of the Point in v_Points[], otherwise
 */
int C_CSV::CSVsearchPointId(C_IST *Ist, int Id){
	int i;
	for (i = 0; i < Ist->num_Points; i++){
		if (Ist->v_Points[i].PointId == Id) return i;
	}
	return -1;
}

/**
 * Search StopPointId
 * @param Id     input ID to be searched
 * @param Ist    input network clas
 * @return -1    if the code does not exists; position of the Point in v_Points[], otherwise
 */
int C_CSV::CSVsearchStopPointId(C_IST *Ist, int Id){
	int i;
	for (i = 0; i < Ist->num_StopPoints; i++){
		if (Ist->v_StopPoints[i].StopPointId == Id) return i;
	}
	return -1;
}

/**
 * Search ParkingPointId
 * @param Id     input ID to be searched
 * @param Ist    input network clas
 * @return -1    if the code does not exists; position of the Point in v_Points[], otherwise
 */
int C_CSV::CSVsearchParkingPointId(C_IST *Ist, int Id){
	int i;
	for (i = 0; i < Ist->num_ParkingPoints; i++){
		if (Ist->v_ParkingPoints[i].ParkingPointId == Id) return i;
	}
	return -1;
}

/**
 * Search TypeOfVehicleId
 * @param Id     input ID to be searched
 * @param Ist    input network clas
 * @return -1    if the code does not exists; position of the Point in v_Points[], otherwise
 */
int C_CSV::CSVsearchTypeOfVehicleId(C_IST *Ist, int Id){
	int i;
	for (i = 0; i < Ist->num_TypeOfVehicles; i++){
		if (Ist->v_TypeOfVehicles[i].TypeOfVehicleId == Id) return i;
	}
	return -1;
}

/**
 * Search VehicleId
 * @param Id     input ID to be searched
 * @param Ist    input network clas
 * @return -1    if the code does not exists; position of the Point in v_Points[], otherwise
 */
int C_CSV::CSVsearchVehicleId(C_IST *Ist, int Id){
	int i;
	for (i = 0; i < Ist->num_Vehicles; i++){
		if (Ist->v_Vehicles[i].VehicleId == Id) return i;
	}
	return -1;
}

/**
 * Search RequestId
 * @param Id     input ID to be searched
 * @param Ist    input network clas
 * @return -1    if the code does not exists; position of the Point in v_Points[], otherwise
 */
int C_CSV::CSVsearchRequestId(C_IST *Ist, int Id){
	int i;
	for (i = 0; i < Ist->num_Requests; i++){
		if (Ist->v_Requests[i].RequestId == Id) return i;
	}
	return -1;
}

//@}
/** @name	allocate memory
 */
//@{
int C_CSV::CSVallocatePoints(int n, C_IST *Ist){

	Ist->v_Points = new Point_STR[n];
	if (Ist->v_Points == NULL) {
		error.fatal("cannot allocate v_Points ", __FUNCTION__);
	}
	return 0;
}

int C_CSV::CSVallocateStopPoints(int n, C_IST *Ist){

	Ist->v_StopPoints = new StopPoint_STR[n];
	if (Ist->v_StopPoints == NULL) {
		error.fatal("cannot allocate v_Stops ", __FUNCTION__);
	}
	return 0;
}

int C_CSV::CSVallocateParkingPoints(int n, C_IST *Ist){

	Ist->v_ParkingPoints = new ParkingPoint_STR[n];
	if (Ist->v_ParkingPoints == NULL) {
		error.fatal("cannot allocate v_parkingPoints ", __FUNCTION__);
	}
	return 0;
}

int C_CSV::CSVallocateTypeOfVehicles(int n,  C_IST *Ist){
    
	Ist->v_TypeOfVehicles = new TypeOfVehicle_STR[n];
    if (Ist->v_TypeOfVehicles == NULL) {
        error.fatal("cannot allocate v_TypeOfVehicle ", __FUNCTION__);   
    }
    return 0;
}

int C_CSV::CSVallocateVehicles(int n, C_IST *Ist){

	Ist->v_Vehicles = new Vehicle_STR[n];
	if (Ist->v_Vehicles == NULL) {
		error.fatal("cannot allocate v_Vehicle ", __FUNCTION__);
	}
	return 0;
}

int C_CSV::CSVallocateVehicleSchedules(int n, C_IST *Ist){

	Ist->v_VehicleSchedules = new VehicleSchedule_STR[n];
	if (Ist->v_VehicleSchedules == NULL) {
		error.fatal("cannot allocate v_VehicleSchedule ", __FUNCTION__);
	}
	return 0;
}

int C_CSV::CSVallocateRequests(int n, C_IST *Ist){

	Ist->v_Requests = new Request_STR[n];
	if (Ist->v_Requests == NULL) {
		error.fatal("Error allocating v_requests ", __FUNCTION__);
	}
	return 0;
}

//@}
/** @name	Procedures to read the CSV tables 
 */
//@{
int C_CSV::CSV_readInstance(char *Instance, char  *shapeFileName, C_SHP *mySHP, C_IST *Ist){
	char shape[250];
	//
	// N.B. The order of the read is constrained by precedence: do not chanage !
	//
	// Setup
	snprintf(buf, sizeof(buf), "%s//Setup.csv", INPUTDIR);
	CSV_readSetup(buf, Ist);
	// Parameters
	snprintf(buf, sizeof(buf), "%s//Parameters.csv", INPUTDIR);
	CSV_readParameters(buf, Ist);
	// Point
	snprintf(buf, sizeof(buf), "%s//%s_Point.csv", INPUTDIR, Instance);
	snprintf(shape, sizeof(shape), "%s//%s", INPUTDIR, shapeFileName);
	CSV_readPoint(buf, shape, mySHP, Ist);
	// StopPoint
	snprintf(buf, sizeof(buf), "%s//%s_StopPoint.csv", INPUTDIR, Instance);
	CSV_readStopPoint(buf, Ist);
	// ParkingPoint
	snprintf(buf, sizeof(buf), "%s//%s_ParkingPoint.csv", INPUTDIR, Instance);
	CSV_readParkingPoint(buf, mySHP, Ist);
	// Type Of Vehicle
	snprintf(buf, sizeof(buf), "%s//%s_TypeOfVehicle.csv", INPUTDIR, Instance);
	CSV_readTypeOfVehicle(buf, mySHP, Ist);
	// Vehicle
	snprintf(buf, sizeof(buf), "%s//%s_Vehicle.csv", INPUTDIR, Instance);
	CSV_readVehicle(buf, mySHP, Ist);
	// Vehicle Schedule
	snprintf(buf, sizeof(buf), "%s//%s_VehicleSchedule.csv", INPUTDIR, Instance);
	CSV_readVehicleSchedule(buf, mySHP, Ist);
	// ParkingPointCapacity
	snprintf(buf, sizeof(buf), "%s//%s_ParkingPointCapacity.csv", INPUTDIR, Instance);
	CSV_readParkingPointCapacity(buf, mySHP, Ist);
	// Request
	snprintf(buf, sizeof(buf), "%s//%s_Request.csv", INPUTDIR, Instance);
	CSV_readRequest(buf, mySHP, Ist);
	// RequestAddress
	snprintf(buf, sizeof(buf), "%s//%s_RequestAddress.csv", INPUTDIR, Instance);
	CSV_readRequestAddress(buf, mySHP, Ist);

	return 0;
}

/**
 * Read Point CSV file
 * @param pointsFileName  input name of the CSV file containing the information on the physical points
 * @param shapefilename   input name of the shapefile containing the network (polylines) on which we put the stops
 * @param Ist input/output the currente network instance
 * @return 0 = k; -1 = error
 */
int C_CSV::CSV_readPoint(char *pointsFileName, char *shapeFileName, C_SHP *mySHP, C_IST *Ist){
	long numRec, l;
	int i, numFields;
	double f;
	unsigned char *p, word[CON_MAXNDESCRPOINT];
	//char word_dummy[CON_MAXNDESCRPOINT];
	static unsigned char line[512];
	FILE *inp;
	double FLat, FLong;
#ifdef LATLONG
	LatLong Converter;
#endif

	// Open and counts the records
	inp = CSV_ApriFile(pointsFileName, (char *) "r");
	numRec = CSV_contaRec(inp);
	if (numRec < 1)
	{
		snprintf(buf, sizeof(buf), "no point: empty file  %s ", pointsFileName);
		error.fatal(buf, __FUNCTION__);;
	}
	l = (numRec - 1);
	CSVallocatePoints(l, Ist);
	// riposiziona il file in testa
	fseek(inp, 0, SEEK_SET);

	// Inizio lettura
	numRec = 0;
	Ist->num_Points = 0;
	CSV_LeggiRec(inp, &numRec, (unsigned char *)line); // first record contains headings
	while (!feof(inp)){
		line[0] = '\0';
		CSV_LeggiRec(inp, &numRec, (unsigned char *)line);
		if (line[0] == '\0') continue;
		p = &line[0];
		numFields = 0;
		//
		// Point Id
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		Ist->v_Points[Ist->num_Points].PointId = i;
		//
		// Session Id // skipped !!!
		// 
		CSV_readString(numRec, &numFields, &p, word);

		//
		// Longitude
		//
		f = CSV_readReal(numRec, &numFields, &p, 0., DBL_MAX);
		FLong = f;
		//
		// Latitude
		//
		f = CSV_readReal(numRec, &numFields, &p, 0., DBL_MAX);
		FLat = f;
		double XX, YY;
		XX = FLong;
		YY = FLat;
#ifdef LATLONG
		Converter.ConvLL2WGS84(FLat, FLong, &YY, &XX);
#endif		
		Ist->v_Points[Ist->num_Points].P.X = XX;
		Ist->v_Points[Ist->num_Points].P.Y = YY;
		//           
		// find the associated arc in the Forward Star
		//
		int i_arc;
		double offset;
		bool Right = true;

		if (mySHP->SHP_Point2FS(Ist->v_Points[Ist->num_Points].P, Right, &i_arc, &offset, Ist) < 0) {
			Ist->v_Points[Ist->num_Points].i_arc = -1;
			Ist->v_Points[Ist->num_Points].offset = -999;
                        snprintf(buf, sizeof(buf),"Cannot allocate point Id=%d (%lf,%lf) to an arc",
                                Ist->v_Points[Ist->num_Points].PointId,XX,YY);
                        error.warning(buf, __FUNCTION__);
		}
		else{
			Ist->v_Points[Ist->num_Points].i_arc = i_arc;
			Ist->v_Points[Ist->num_Points].offset = offset;
		}
		Ist->v_Points[Ist->num_Points].i_arc2 = -1;
		Ist->v_Points[Ist->num_Points].offset2 = -999;

		Ist->num_Points++;
	}// while
	/*
for (i = 0; i < Ist->num_Stops; i++)
cout << "\n" << setw(15) << " "
<< Ist->v_Points[i].Cod << " " << setw(40) << " "
<< Ist->v_Points[i].Description << " " << setw(3) << " "
<< Ist->v_Points[i].P.X << " " << setw(12) << " "
<< Ist->v_Points[i].P.Y << " " << setw(4) << " "
<< Ist->v_Points[i].i_arc << " " << setw(9) << " "
<< Ist->v_Points[i].offset << " " << setw(4) << " ";
*/
	fclose(inp);
	return 0;
}

/** S T O P P O I N T 
 * Read StopPoint CSV file
 * @param StopsFileName  input name of the CSV file containing the information on the StopPoints
 * @param Ist input/output the currente network instance
 * @return 0 = k; -1 = error
 */
int C_CSV::CSV_readStopPoint(char *FileName, C_IST *Ist){
	long numRec, l;
	int i, id, numFields;
	unsigned char *p, word[CON_MAXNDESCRSTOPPOINT];
	static unsigned char line[512];
	FILE *inp;

	// Open and counts the records
	inp = CSV_ApriFile(FileName, (char *) "r");
	numRec = CSV_contaRec(inp);
	if (numRec < 1)
	{
		snprintf(buf, sizeof(buf), "stop points: empty file %s ", FileName);
		error.fatal(buf, __FUNCTION__);
	}
	l = (numRec - 1);
	CSVallocateStopPoints(l, Ist);
	// riposiziona il file in testa
	fseek(inp, 0, SEEK_SET);

	// Inizio lettura
	numRec = 0;
	Ist->num_StopPoints = 0;
	CSV_LeggiRec(inp, &numRec, (unsigned char *)line); // first record contains headings
	while (!feof(inp)){
		line[0] = '\0';
		CSV_LeggiRec(inp, &numRec, (unsigned char *)line);
		if (line[0] == '\0') continue;
		p = &line[0];
		numFields = 0;
		//
		// StopPoint Id
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		Ist->v_StopPoints[Ist->num_StopPoints].StopPointId = i;
		//
		// Point Id
		//
		id = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		i = CSVsearchPointId(Ist, id);
		if (i < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d Point Id %d not found", numRec, numFields, id);
			error.fatal(buf, __FUNCTION__);
		};
		Ist->v_StopPoints[Ist->num_StopPoints].i_Point = i;
		//
		// Session Id // skipped !!!
		//
		CSV_readString(numRec, &numFields, &p, word);
		//
		// Cod
		//
		CSV_readString(numRec, &numFields, &p, word);
		memcpy(Ist->v_StopPoints[Ist->num_StopPoints].Cod, word, CON_MAXNCODSTOPPOINT);
		//
		// Descr
		//
		CSV_readString(numRec, &numFields, &p, word);
		memcpy(Ist->v_StopPoints[Ist->num_StopPoints].Descr, word, CON_MAXNDESCRSTOPPOINT);
		//
		Ist->num_StopPoints++;
	}
	//                     
	fclose(inp);
	return 0;
}

/** P A R K I N G P O I N T 
* Reads the ParkingPoint table 
* @param FileName  input name of the CSV file containing the information on the ParkingPoints
* @param Ist input/output the currente network instance
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_readParkingPoint(char *FileName, C_SHP * mySHP, C_IST *Ist){
	long numRec, l;
	int i, id, numFields;
	unsigned char *p, word[CON_MAXNDESCRSTOPPOINT];
	static unsigned char line[512];
	FILE *inp;

	// Open and counts the records
	inp = CSV_ApriFile(FileName, (char *) "r");
	numRec = CSV_contaRec(inp);
	if (numRec < 1)
	{
		snprintf(buf, sizeof(buf), "parking points: empty file  %s ", FileName);
		error.fatal(buf, __FUNCTION__);
	}
	l = (numRec - 1);
	CSVallocateParkingPoints(l, Ist);
	// riposiziona il file in testa
	fseek(inp, 0, SEEK_SET);

	// Inizio lettura
	numRec = 0;
	Ist->num_ParkingPoints = 0;
	CSV_LeggiRec(inp, &numRec, (unsigned char *)line); // first record contains headings
	while (!feof(inp)){
		line[0] = '\0';
		CSV_LeggiRec(inp, &numRec, (unsigned char *)line);
		if (line[0] == '\0') continue;
		p = &line[0];
		numFields = 0;
		//
		// ParkingPoint Id
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		Ist->v_ParkingPoints[Ist->num_ParkingPoints].ParkingPointId = i;
		//
		// Session Id // skipped !!!
		//
		CSV_readString(numRec, &numFields, &p, word);
		//
		// Point Id
		//
		id = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		//int i_Point = CSVsearchPointId(Ist, i);
		int i_Point = CSVsearchPointId(Ist, id);  // Marco:Maggio2016
		if (i_Point < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d Point Id %d not found", numRec, numFields, id);
			error.fatal(buf, __FUNCTION__);
		};
		Ist->v_ParkingPoints[Ist->num_ParkingPoints].i_Point = i_Point;
		//
		// Is Depot ?
		//
		i = CSV_readBool(numRec, &numFields, &p);
		Ist->v_ParkingPoints[Ist->num_ParkingPoints].isDepot = i;
		if (i){
			int i_arc;
			double offset;
			bool RightLeft = false;
			if (mySHP->SHP_Point2FS(Ist->v_Points[i_Point].P, RightLeft, &i_arc, &offset, Ist) < 0) {
				//Ist->v_Points[Ist->num_Points].i_arc2 = -1;
				//Ist->v_Points[Ist->num_Points].offset2 = -999;				
				Ist->v_Points[i_Point].i_arc2 = -1;     // Marco:Maggio2016
				Ist->v_Points[i_Point].offset2 = -999;
			}
			else{
				//Ist->v_Points[Ist->num_Points].i_arc2 = i_arc;
				//Ist->v_Points[Ist->num_Points].offset2 = offset;
				Ist->v_Points[i_Point].i_arc2 = i_arc;     // Marco:Maggio2016
				Ist->v_Points[i_Point].offset2 = offset;
			}
		}
		//
		// Cod
		//
		CSV_readString(numRec, &numFields, &p, word);
		memcpy(Ist->v_ParkingPoints[Ist->num_ParkingPoints].Cod, word, CON_MAXNCODPARKINGPOINT);
		//
		// Descr
		//
		CSV_readString(numRec, &numFields, &p, word);
		memcpy(Ist->v_ParkingPoints[Ist->num_ParkingPoints].Descr, word, CON_MAXNDESCRPARKINGPOINT);
		//
		Ist->num_ParkingPoints++;
	}
	//                     
	fclose(inp);
	return 0;
}

/** P A R K I N G P O I N T C A P A C I T Y 
* Reads the ParkingPointsCapacity table and stores the values in v_ParkingPoint[]
* @param FileName input name of the CSV file containing the information on the ParkingPoints Capacity
* @param Ist input/output the currente network instance
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_readParkingPointCapacity(char *FileName, C_SHP *mySHP, C_IST *Ist){
	long numRec;
	int i, id, numFields;
	unsigned char *p, word[CON_MAXNDESCRPARKINGPOINT];
	static unsigned char line[512];
	FILE *inp;


	// Open and counts the records
	inp = CSV_ApriFile(FileName, (char *) "r");
	// riposiziona il file in testa
	fseek(inp, 0, SEEK_SET);

	// Inizio lettura
	numRec = 0;
	CSV_LeggiRec(inp, &numRec, (unsigned char *)line); // first record contains headings
	while (!feof(inp)){
		line[0] = '\0';
		CSV_LeggiRec(inp, &numRec, (unsigned char *)line);
		if (line[0] == '\0') continue;
		p = &line[0];
		numFields = 0;
		//
		// ParkingPointCapacity Id and skip it
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		//
		// Session Id // skipped !!!
		//
		CSV_readString(numRec, &numFields, &p, word);
		//
		// Parking Point Id
		//
		id = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		int i_point = CSVsearchParkingPointId(Ist, id);
		if (i_point < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d Point Id %d not found", numRec, numFields, id);
			error.fatal(buf, __FUNCTION__);
		};
		//
		// Type of Vehicle
		//
		id = CSV_readInt(numRec, &numFields, &p, 0, CON_MAXCODEVALUE - 1);
		int i_vehicletype = CSVsearchTypeOfVehicleId(Ist, id);
		if (i_vehicletype < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d Point Id %d not found", numRec, numFields, id);
			error.fatal(buf, __FUNCTION__);
		};
		//
		// Max Capacity
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		Ist->v_ParkingPoints[i_point].capacity[i_vehicletype] = i;
		//           
	}
	//                     
	fclose(inp);
	return 0;
}

/** T Y P E O F V E H I C L E  
* Reads the TypeOfVehicle table and store the values in v_TypeOfVehicles[]
* @param FileName input name of the CSV file containing the information on the Vehicles
* @param Ist input/output the currente network instance
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_readTypeOfVehicle(char *FileName, C_SHP *mySHP, C_IST *Ist){
	long numRec, l;
	int i, numFields;
	unsigned char *p, word[CON_MAXNDESCRTOV];
	static unsigned char line[512];
	FILE *inp;


	// Open and counts the records
	inp = CSV_ApriFile(FileName, (char *) "r");
	numRec = CSV_contaRec(inp);
	if (numRec < 1)
	{
		snprintf(buf, sizeof(buf), "type of vehicle: Empty file  %s ", FileName);
		error.fatal(buf, __FUNCTION__);
	}
	l = (numRec - 1);
	CSVallocateTypeOfVehicles(l, Ist);
	// riposiziona il file in testa
	fseek(inp, 0, SEEK_SET);

	// Inizio lettura
	numRec = 0;
	Ist->num_TypeOfVehicles = 0;
	CSV_LeggiRec(inp, &numRec, (unsigned char *)line); // first record contains headings
	while (!feof(inp)){
		line[0] = '\0';
		CSV_LeggiRec(inp, &numRec, (unsigned char *)line);
		if (line[0] == '\0') continue;
		p = &line[0];
		numFields = 0;
		//
		// Vehicle ID
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		Ist->v_TypeOfVehicles[Ist->num_TypeOfVehicles].TypeOfVehicleId = i;
		//
		// Session Id // skipped !!!
		//
		CSV_readString(numRec, &numFields, &p, word);
		//
		// CODE
		//
		CSV_readString(numRec, &numFields, &p, word);
		memcpy(Ist->v_TypeOfVehicles[Ist->num_TypeOfVehicles].Cod, word, CON_MAXNCODTOV);
		//
		// Description
		//
		CSV_readString(numRec, &numFields, &p, word);
		memcpy(Ist->v_TypeOfVehicles[Ist->num_TypeOfVehicles].Descr, word, CON_MAXNCODTOV);
		//
		Ist->num_TypeOfVehicles++;
	}
	//                     
	fclose(inp);
	return 0;
}

/** V E H I C L E 
* Reads the Vehicle table and stores the values in v_Vehicles[]
* @param FileName input name of the CSV file containing the information on the Vehicles
* @param Ist input/output the currente network instance
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_readVehicle(char *FileName, C_SHP *mySHP, C_IST *Ist){
	long numRec, l;
	int i, id, numFields;
	unsigned char *p, word[CON_MAXNDESCRVEHICLE];
	static unsigned char line[512];
	FILE *inp;

	// Open and counts the records
	inp = CSV_ApriFile(FileName, (char *) "r");
	numRec = CSV_contaRec(inp);
	if (numRec < 1)
	{
		snprintf(buf, sizeof(buf), "vehicle : empty file  %s ", FileName);
		error.fatal(buf, __FUNCTION__);
	}
	l = (numRec - 1);
	CSVallocateVehicles(l, Ist);
	// riposiziona il file in testa
	fseek(inp, 0, SEEK_SET);

	// Inizio lettura
	numRec = 0;
	Ist->num_Vehicles = 0;
	CSV_LeggiRec(inp, &numRec, (unsigned char *)line); // first record contains headings
	while (!feof(inp)){
		line[0] = '\0';
		CSV_LeggiRec(inp, &numRec, (unsigned char *)line);
		if (line[0] == '\0') continue;
		p = &line[0];
		numFields = 0;
		//
		// Vehicle ID
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		Ist->v_Vehicles[Ist->num_Vehicles].VehicleId = i;
		//
		// Session Id // skipped !!!
		//
		CSV_readString(numRec, &numFields, &p, word);
		//
		// Type of Vehicle
		//
		id = CSV_readInt(numRec, &numFields, &p, 0, CON_MAXCODEVALUE - 1);
		int i_vehicletype = CSVsearchTypeOfVehicleId(Ist, id);
		if (i_vehicletype < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d Type Of Vehicle %d not found", numRec, numFields, id);
			error.fatal(buf, __FUNCTION__);
		}
		Ist->v_Vehicles[Ist->num_Vehicles].i_TypeOfVehicle = i;
		//
		// Callsign
		//
		CSV_readString(numRec, &numFields, &p, word);
		memcpy(Ist->v_Vehicles[Ist->num_Vehicles].Callsign, word, CON_MAXNCODVEHICLE);
		//
		// Latitude // skipped !!!
		//
		CSV_readString(numRec, &numFields, &p, word);
		//
		// Longitude // skipped !!!
		//
		CSV_readString(numRec, &numFields, &p, word);
		//
		// Standing Capacity
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		Ist->v_Vehicles[Ist->num_Vehicles].StandingCapacity = i;
		//
		// Seated Capacity
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		Ist->v_Vehicles[Ist->num_Vehicles].SeatedCapacity = i;
		//
		// Wheel Chair Capacity
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		Ist->v_Vehicles[Ist->num_Vehicles].WheelChairCapacity = i;
		//
		// Total Capacity
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		Ist->v_Vehicles[Ist->num_Vehicles].TotalCapacity = i;
		//
		Ist->num_Vehicles++;
	}
	//                     
	fclose(inp);
	return 0;
}

/** V E H I C L E S C H E D U L E
* Reads the VehicleSchedule table and stores the values in v_VehicleSchedules[]
* @param FileName input name of the CSV file containing the information on the Vehicle Schedules
* @param Ist input/output the currente network instance
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_readVehicleSchedule(char *FileName, C_SHP *mySHP, C_IST *Ist){
	long numRec, l;
	int i, id, numFields;
	unsigned char *p, word[CON_MAXNDESCRVEHICLE];
	static unsigned char line[512];
	FILE *inp;

	// Open and counts the records
	inp = CSV_ApriFile(FileName, (char *) "r");
	numRec = CSV_contaRec(inp);
	if (numRec < 1)
	{
		snprintf(buf, sizeof(buf), "vehicle schedule : empty file  %s ", FileName);
		error.fatal(buf, __FUNCTION__); 
	}
	l = (numRec - 1);
	CSVallocateVehicleSchedules(l, Ist);
	// riposiziona il file in testa
	fseek(inp, 0, SEEK_SET);

	// Inizio lettura
	numRec = 0;
	Ist->num_VehicleSchedules = 0;
	CSV_LeggiRec(inp, &numRec, (unsigned char *)line); // first record contains headings
	while (!feof(inp)){
		line[0] = '\0';
		CSV_LeggiRec(inp, &numRec, (unsigned char *)line);
		if (line[0] == '\0') continue;
		p = &line[0];
		numFields = 0;
		//
		// Vehicle Schedule ID
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		Ist->v_VehicleSchedules[Ist->num_VehicleSchedules].VehicleScheduleId = i;
		//
		// Session Id // skipped !!!
		//
		CSV_readString(numRec, &numFields, &p, word);
		//
		// Vehicle ID
		//
		id = CSV_readInt(numRec, &numFields, &p, 0, CON_MAXCODEVALUE - 1);
		int i_vehicle = CSVsearchVehicleId(Ist, id);
		if (i_vehicle < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d Vehicle Id %d not found", numRec, numFields, id);
			error.fatal(buf, __FUNCTION__);
		}
		Ist->v_VehicleSchedules[Ist->num_VehicleSchedules].i_Vehicle = i_vehicle;
		//
		// StartTime
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, CON_LASTTIME);
		Ist->v_VehicleSchedules[Ist->num_VehicleSchedules].StartTime = i;
		//
		// endTime
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, CON_LASTTIME);
		Ist->v_VehicleSchedules[Ist->num_VehicleSchedules].EndTime = i;
		//
		// Start depot
		//
		id = CSV_readInt(numRec, &numFields, &p, 0, CON_MAXCODEVALUE - 1);
		int i_park = CSVsearchParkingPointId(Ist, id);
		if (i_park < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d Parking Point Id %d not found", numRec, numFields, id);
			error.fatal(buf, __FUNCTION__);
		}
		Ist->v_VehicleSchedules[Ist->num_VehicleSchedules].i_StartDepot = i_park;
		//
		// End depot
		//
		id = CSV_readInt(numRec, &numFields, &p, 0, CON_MAXCODEVALUE - 1);
		i_park = CSVsearchParkingPointId(Ist, id);
		if (i_park < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d Parking Point Id %d not found", numRec, numFields, id);
			error.fatal(buf, __FUNCTION__);
		}
		Ist->v_VehicleSchedules[Ist->num_VehicleSchedules].i_EndDepot = i_park;
		//
		Ist->num_VehicleSchedules++;
	}
	//                     
	fclose(inp);
	return 0;
}

/** R E Q U E S T
* Reads the Request table and stores the values in v_Requestss[]
* @param FileName  input name of the CSV file containing the information on the requests
* @param Ist input/output the currente network instance
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_readRequest(char *FileName, C_SHP *mySHP, C_IST *Ist){
	long numRec, l;
	int i, id, numFields;
	unsigned char *p, word[CON_MAXNDESCRREQUEST];
	static unsigned char line[512];
	FILE *inp;

	// Open and counts the records
	inp = CSV_ApriFile(FileName, (char *) "r");
	numRec = CSV_contaRec(inp);
	if (numRec < 1)
	{
		snprintf(buf, sizeof(buf), "request : empty file = %s ", FileName);
		error.warning(buf, __FUNCTION__);
                return 0;
	}
	l = (numRec - 1);
	CSVallocateRequests(l, Ist);
	// riposiziona il file in testa
	fseek(inp, 0, SEEK_SET);

	// Inizio lettura
	numRec = 0;
	Ist->num_Requests = 0;
	CSV_LeggiRec(inp, &numRec, (unsigned char *)line); // first record contains headings
	while (!feof(inp)){
		line[0] = '\0';
		CSV_LeggiRec(inp, &numRec, (unsigned char *)line);
		if (line[0] == '\0') continue;
		p = &line[0];
		numFields = 0;
		//
		// Request ID
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		Ist->v_Requests[Ist->num_Requests].RequestId = i;
		//
		// Session Id // skipped !!!
		//
		CSV_readString(numRec, &numFields, &p, word);
		//
		// Code
		//
		CSV_readString(numRec, &numFields, &p, word);

		//memcpy(Ist->v_Requests[Ist->num_Requests].Cod, word, MIN(strlen((char *)word), CON_MAXNCODREQUEST));
		strncpy(Ist->v_Requests[Ist->num_Requests].Cod, (char *)word, MIN(strlen((char *)word) + 1, CON_MAXNCODREQUEST));
		//
		// Booked  // skipped !!!
		//
		CSV_readString(numRec, &numFields, &p, word);
		//
		// Priority
		//
		i = CSV_readInt_NULL(numRec, &numFields, &p, 0, INT_MAX, 0);
		Ist->v_Requests[Ist->num_Requests].Priority = i;
		//
		// Type of Vehicle
		//
		id = CSV_readInt_NULL(numRec, &numFields, &p, 0, CON_MAXCODEVALUE - 1, -1);
		if (id == -1)
			Ist->v_Requests[Ist->num_Requests].i_TypeOfVehicle = -1;
		else
		{
			int i_vehicletype = CSVsearchTypeOfVehicleId(Ist, id);
			if (i_vehicletype < 0) {
				snprintf(buf, sizeof(buf), "Record = %ld field %d Type Of Vehicle %d not found", numRec, numFields, id);
				error.fatal(buf, __FUNCTION__);
			}
			Ist->v_Requests[Ist->num_Requests].i_TypeOfVehicle = i;
		}
		//
		// Customer code
		//
		CSV_readString(numRec, &numFields, &p, word);
		memcpy(Ist->v_Requests[Ist->num_Requests].CustomerCode, word, CON_MAXNCUSTOMERCODREQUEST);
		//
		// Max Transit
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, CON_LASTTIME);
		Ist->v_Requests[Ist->num_Requests].MaxTransit = i;
		//
		// Service Time
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, CON_LASTTIME);
		Ist->v_Requests[Ist->num_Requests].ServiceTime = i;
		//
		// Confirmed Time
		//
		i = CSV_readInt_NULL(numRec, &numFields, &p, 0, CON_LASTTIME, -1);
		Ist->v_Requests[Ist->num_Requests].ConfirmedTime = i;
		//
		// Load Normal
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, CON_MAXLOAD);
		Ist->v_Requests[Ist->num_Requests].LoadNormal = i;
		//
		// Load Disabled
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, CON_MAXLOAD);
		Ist->v_Requests[Ist->num_Requests].LoadDisabled = i;
		//
		// Load WheelChair
		//
		i = CSV_readInt(numRec, &numFields, &p, 0, CON_MAXLOAD);
		Ist->v_Requests[Ist->num_Requests].LoadWheelChair = i;
		//
		// VehicleID
		//
		id = CSV_readInt_NULL(numRec, &numFields, &p, 0, INT_MAX, -1);
		if (id==-1)
			Ist->v_Requests[Ist->num_Requests].i_Vheicle = -1;
		else
		{
			int i_vehicle = CSVsearchVehicleId(Ist, id);
			if (i_vehicle < 0) {
				snprintf(buf, sizeof(buf), "Record = %ld field %d VehicleID %d not found", numRec, numFields, id);
				error.fatal(buf, __FUNCTION__);
			}
			Ist->v_Requests[Ist->num_Requests].i_Vheicle = i;
		}
		//
		// Confirmed Range
		//
		i = CSV_readInt_NULL(numRec, &numFields, &p, 0, CON_LASTTIME, -1);
		Ist->v_Requests[Ist->num_Requests].ConfirmedRange = i;
		//
		// Time Window Range
		//
		i = CSV_readInt_NULL(numRec, &numFields, &p, 0, CON_LASTTIME, 0);
		Ist->v_Requests[Ist->num_Requests].TimeWindowRange = i;
		//
		Ist->num_Requests++;
	}
	//                     
	fclose(inp);
	return 0;
}

/** R E Q U E S T A D D R E S S 
* Reads the RequestAddress table and stores the values in v_Requestss[]
* @param FileName input name of the CSV file containing the information on the requests
* @param Ist input/output the currente network instance
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_readRequestAddress(char *FileName, C_SHP *mySHP, C_IST *Ist){
	long numRec;
	int id, numFields;
	unsigned char *p, word[CON_MAXNDESCRREQUEST];
	static unsigned char line[512];
	FILE *inp;

	// Open and counts the records
	inp = CSV_ApriFile(FileName, (char *) "r");
	// riposiziona il file in testa
	fseek(inp, 0, SEEK_SET);

	// Inizio lettura
	numRec = 0;
	CSV_LeggiRec(inp, &numRec, (unsigned char *)line); // first record contains headings
	while (!feof(inp)){
		line[0] = '\0';
		CSV_LeggiRec(inp, &numRec, (unsigned char *)line);
		if (line[0] == '\0') continue;
		p = &line[0];
		numFields = 0;
		//
		// RequestAddress Id 
		//
		int RequestAddressid = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		//
		// Session Id // skipped !!!
		//
		CSV_readString(numRec, &numFields, &p, word);
		//
		// Request ID
		//
		id = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		int i_request = CSVsearchRequestId(Ist, id);
		if (i_request < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d Request ID %d not found", numRec, numFields, id);
			error.fatal(buf, __FUNCTION__);
		}
		//
		// StopPoint ID
		//
		id = CSV_readInt(numRec, &numFields, &p, 0, INT_MAX);
		int i_StopPoint = CSVsearchStopPointId(Ist, id);
		if (i_StopPoint < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d Stop Point ID %d not found", numRec, numFields, id);
			error.fatal(buf, __FUNCTION__);
		}
		//
		// Ordinal // skipped !!!
		//
		CSV_readString(numRec, &numFields, &p, word);
		//
		// Action Type
		//
		CSV_readString(numRec, &numFields, &p, word);
		int i_action = ACTIONTYPEOFPOINTFIND(word);
		if (i_action < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d Action Type invalid  %s", numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
		}
		//
		// StartTime
		//
		int StartTime = CSV_readInt_NULL(numRec, &numFields, &p, 0, CON_LASTTIME, 0);
		//
		// EndTime
		//
		int EndTime = CSV_readInt_NULL(numRec, &numFields, &p, 0, CON_LASTTIME, CON_LASTTIME);
		//
		// EstimatedTime
		//
		int EstimatedTime = CSV_readInt_NULL(numRec, &numFields, &p, 0, CON_LASTTIME, 0);
		//
		if (i_action == 0) {
			// Pickup
			Ist->v_Requests[i_request].pickup.StartTime = StartTime;
			Ist->v_Requests[i_request].pickup.EndTime = EndTime;
			Ist->v_Requests[i_request].pickup.EstimatedTime = EstimatedTime;
			Ist->v_Requests[i_request].pickup.RequestAddressId = RequestAddressid;
			Ist->v_Requests[i_request].i_pickup_stop = i_StopPoint;
		}
		else{
			// dropoff
			Ist->v_Requests[i_request].dropoff.StartTime = StartTime;
			Ist->v_Requests[i_request].dropoff.EndTime = EndTime;
			Ist->v_Requests[i_request].dropoff.EstimatedTime = EstimatedTime;
			Ist->v_Requests[i_request].dropoff.RequestAddressId = RequestAddressid;
			Ist->v_Requests[i_request].i_dropoff_stop = i_StopPoint;
		}
	}
	//                     
	fclose(inp);
	return 0;
}

/**
* Read the setup
* @param Ist instance
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_readSetup(char *FileName, C_IST *Ist)
{
	FILE *fin;
	long numRec;
	long konta;
	int numFields;
	char buf[CON_MAXNSTR];
	unsigned char *p, word[CON_MAXNSTR];
	char word_dummy[CON_MAXNSTR];
	static unsigned char line[512];
	long i;
	long ipar;

	// Open file
	fin = fopen(FileName, "r");
	if (fin == NULL)
	{
            snprintf(buf,sizeof(buf),"File %s cannot be opened",FileName);
            error.fatal(buf, __FUNCTION__);
	}

	// Counts the records
	numRec = CSV_contaRec(fin);
	if (numRec < 1)
	{
            snprintf(buf,sizeof(buf),"setup file %s is empty",FileName);
            error.fatal(buf, __FUNCTION__);
	}

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

		//////// Parameter Name
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d not found", numRec, numFields);
			error.fatal(buf, __FUNCTION__);
		};
		memcpy(buf, word, CON_MAXNSTR);
		if (strncmp(buf, "RouteId", 7) == 0)
		{
			ipar = 0;
		}
		else if (strncmp(buf, "SolutionId", 10) == 0)
		{
			ipar = 1;
		}
		else
		{
			ipar = -1;
			error.fatal("Parameter not recognized", __FUNCTION__);
		}

		//////// Value
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d not found", numRec, numFields);
			error.fatal(buf, __FUNCTION__);
		};
		if (CSVnonIntero(word))	{
			snprintf(buf, sizeof(buf), "Record = %ld field %d not an integer = %s", numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
			return(-1);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0)	{
			snprintf(buf, sizeof(buf), "Record = %ld field %d negative = %ld", numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
		}
		if (ipar == 0)
		{
			Ist->RouteID = i;
		}
		else if (ipar == 1)
		{
			Ist->SolutionID = i;
		}
		else
		{
			error.fatal("Parameter not recognized", __FUNCTION__);
		}

		konta++;
	}

	// Close input file
	fclose(fin);

	return 0;
}


/**
* Read Parameters
* @param Ist instance
* @return 0 = k; -1 = error
*/
int C_CSV::CSV_readParameters(char *FileName, C_IST *Ist)
{
	FILE *fin;
	long numRec;
	long konta;
	int numFields;
	char buf[CON_MAXNSTR];
	unsigned char *p, word[CON_MAXNSTR];
	char word_dummy[CON_MAXNSTR];
	static unsigned char line[512];
	long i;
	long ipar;

	// Open file
	fin = fopen(FileName, "r");
	if (fin == NULL)
	{
            snprintf(buf,sizeof(buf),"File %s cannot be opened",FileName);            
            error.fatal(buf, __FUNCTION__);
	}

	// Counts the records
	numRec = CSV_contaRec(fin);
	if (numRec < 1)
	{
            snprintf(buf,sizeof(buf),"parameter file %s empty",FileName);            
		error.fatal(buf, __FUNCTION__);
	}

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

		//////// Parameter Name
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d not found", numRec, numFields);
			error.fatal(buf, __FUNCTION__);
		};
		memcpy(buf, word, CON_MAXNSTR);
		if (strncmp(buf, "PathType", 8) == 0)
		{
			ipar = 0;
		}
		else if (strncmp(buf, "AvgSpeed", 8) == 0)
		{
			ipar = 1;
		}
		else
		{
			ipar = -1;
			error.fatal("Parameter not recognized", __FUNCTION__);
		}

		//////// Value
		if ((i = CSV_getWord(numRec, &numFields, &p, word)) < 0) {
			snprintf(buf, sizeof(buf), "Record = %ld field %d not found", numRec, numFields);
			error.fatal(buf, __FUNCTION__);
		};
		if (CSVnonIntero(word))	{
			snprintf(buf, sizeof(buf), "Record = %ld field %d not an integer = %s", numRec, numFields, word);
			error.fatal(buf, __FUNCTION__);
		}
		memcpy(word_dummy, word, sizeof(word));// sscanf requires char, not unsigned char
		sscanf(word_dummy, "%ld", &i);
		if (i < 0)	{
			snprintf(buf, sizeof(buf), "Record = %ld field %d negative = %ld", numRec, numFields, i);
			error.fatal(buf, __FUNCTION__);
		}
		if (ipar == 0)
		{
			Ist->Param.PathType = i;
		}
		else if (ipar == 1)
		{
			Ist->Param.AvgSpeed = (double)i * (1000. / 3600.);
		}
		else
		{
			error.fatal("Parameter not recognized", __FUNCTION__);
		}

		konta++;
	}

	// Close input file
	fclose(fin);

	return 0;
}

//@}

