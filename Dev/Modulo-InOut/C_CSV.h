/* 
 * File:   C_CSV.h
 * Author: Mauro
 *
 * Created on 6 aprile 2016, 15.18
 */
// solo per fare il un test su GitHUB
#ifndef C_CSV_H
#define C_CSV_H

class C_CSV
{
	int     CSVnonIntero(unsigned char *w);
	int     CSVnonInteroNull(unsigned char *w);
	int     CSVnonReale(unsigned char *w);

	int     CSVsearchPointId(C_IST *Ist, int Id);
	int     CSVsearchParkingPointId(C_IST *Ist, int Id);
	int     CSVsearchStop(C_IST *Ist, unsigned char *stopCode);
	int     CSVsearchStopPointId(C_IST *Ist, int Id);
	int     CSVsearchTypeOfVehicleId(C_IST *Ist, int Id);
	int     CSVsearchVehicleId(C_IST *Ist, int Id);
	int     CSVsearchRequestId(C_IST *Ist, int Id);

	int     CSVallocatePoints(int n, C_IST *Ist);
	int     CSVallocateStopPoints(int n, C_IST *Ist);
	int     CSVallocateParkingPoints(int n, C_IST *Ist);
	int     CSVallocateTypeOfVehicles(int n, C_IST *Ist);
	int     CSVallocateVehicles(int n, C_IST *Ist);
	int     CSVallocateVehicleSchedules(int n, C_IST *Ist);
	int     CSVallocateRequests(int n, C_IST *Ist);

public:

	C_CSV();
	~C_CSV();
	void    CSV_AllocateMatrixDT_and_C();
	void    CSV_DeAllocateMatrixDT_and_C(C_IST *Ist);
	FILE    *CSV_ApriFile(char *name, char *modo);
	int     CSV_LeggiRec(FILE *inp, long *numRec, unsigned char *linea);
	long    CSV_contaRec(FILE *inp);
	int     CSV_getWord(int numRec, int *numFields, unsigned char **p, unsigned char *w);
	//
	bool    CSV_readBool(int numRec, int *numFields, unsigned char **p);
	int     CSV_readInt(int numRec, int *numFields, unsigned char **p, int LB, int UB);
	int     CSV_readInt_NULL(int numRec, int *numFields, unsigned char **p, int LB, int UB, int def);
	double  CSV_readReal(int numRec, int *numFields, unsigned char **p, double LB, double UB);
	void    CSV_readString(int numRec, int *numFields, unsigned char **p, unsigned char *word);

	int     CSV_readInstance(char *Instance, char  *shapeFileName, C_SHP *mySHP, C_IST *Ist);

	int     CSV_readSetup(char *FileName, C_IST *Ist);
	int     CSV_readParameters(char *FileName, C_IST *Ist);
	int     CSV_readPoint(char *stopsFileName, char  *shapeFileName, C_SHP *mySHP, C_IST *Ist);
	int     CSV_readStopPoint(char *stopsFileName, C_IST *Ist);
	int     CSV_readParkingPoint(char * filename, C_SHP *mySHP, C_IST *Ist);
	int     CSV_readParkingPointCapacity(char *stopsFileName, C_SHP *mySHP, C_IST *Ist);
	int     CSV_readTypeOfVehicle(char *requestsFileName, C_SHP *mySHP, C_IST *Ist);
	int     CSV_readVehicle(char *requestsFileName, C_SHP *mySHP, C_IST *Ist);
	int     CSV_readVehicleSchedule(char *requestsFileName, C_SHP *mySHP, C_IST *Ist);
	int     CSV_readRequest(char *requestsFileName, C_SHP *mySHP, C_IST *Ist);
	int     CSV_readRequestAddress(char *requestsFileName, C_SHP *mySHP, C_IST *Ist);

	int     CSV_CheckInputDataSize(C_IST *Ist);
	int     CSV_writeOptiData(char *OptiInFileName, C_SHP *mySHP, C_IST *Ist);
	int     CSV_readOptiData(char *OptiOutFileName, C_SHP *mySHP, C_IST *Ist);
	int     CSV_OptiIODebug(C_IST *Ist);
	int     CSV_AssignVehicle(C_IST *Ist, long *ivehicle, long *ivehiclesched);

	int     CVS_writeSetup(C_IST *Ist);
	int     CVS_writeWaypoints(C_SHP *mySHP, C_IST *Ist, char *Instance);
	int     CSV_writeWaypointsFirstArc(SHPHandle shpHandle, ofstream *fout, int *WaypointId, int SolutionId,
		int VehicleID, int *PointOrder, int RoutePointId, int RouteID, XYPoint_STR *p, C_SHP *mySHP, C_IST *Ist);
	void    CSV_writeWaypointsRow(ofstream *fout, int WaypointId, int SessionId, int SolutionId, int Vehicleid, int PointOrder,
		int RoutePointId, int RouteId, XYPoint_STR v, long long FeatureId);
	void    CSV_writeWaypointsHeader(ofstream *fout);
	void    CVS_writePath(C_SHP *mySHP, C_IST *Ist, char *Instance);
	void    CVS_writeRoute(C_IST *Ist, char *Instance);
	void    CVS_writeRoutePoint(C_IST *Ist, char *Instance);
	void    CVS_writeRouteOld(C_IST *Ist, char *Instance);
	void    CVS_writeRequestInRoute(C_IST *Ist, char *Instance);
	void    CVS_writeSolution(C_IST *Ist, char *Instance);
};

#endif /* C_CSV_H */

