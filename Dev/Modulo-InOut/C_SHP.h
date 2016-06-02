#ifndef CSHP_H_
#define CSHP_H_

using namespace std;

//#include "../Modulo-InOut/DB_CSV.h"

#define shpEPS 4.0
#define SWAP(A, B, T) {T C;C=(A);(A)=(B);(B)=C;}
#define R_EARTH 6378000.0
#define PI 3.14159265

class C_SHP{

private:
	int     SHP_num_Vertices_Dim;  /// memory allocated for v_SHP_Verticer_List*/
	int     SHP_num_Arcs_Dim;      /// memory allocated for v_SHP_Arcs_List*/
	int     SHP_num_Points_Dim;    /// memory allocated for array  v_SHP_Points_List */  

	int     SHPaddIShape(Arc_STR *a1, Arc_STR *a2);
	int     SHPattributeDirection(DBFHandle dbfHandle, int numElem);
	double  SHPattributeMeters(DBFHandle dbfHandle, int numElem);
	long long SHPattributeFid(DBFHandle dbfHandle, int numElem);
	long long SHPattributeId(DBFHandle dbfHandle, int numElem);
	long long SHPattributeNoturn(DBFHandle dbfHandle, int numElem, int index);
	int     SHPattributeSpeed(DBFHandle dbfHandle, int numElem);

	int     SHPconnectArcs(Arc_STR *a1, Arc_STR *a2);
	int     SHPcleanLoops(C_IST *);
	int     SHPcleanDegreeZeroAndTwo_function(C_IST *);
	int     SHPcleanMultipleArcs(C_IST *);
	int     SHP_Copy(char *filename);

	int     SHPreadAndStoreAttributes(DBFHandle dbfHandle, int i_SHP_Elem, int i_SHP_Arc, C_IST *);
	int     SHPturnForbidden(Arc_STR *a1, Arc_STR *a2);

	int     SHPsplit2Loops(C_IST *);

	void    quickSortVertices(int *LIST, int left_index, int right_index, double type, C_IST *);

	// Graph utilities
	int     SHPsameShape(Arc_STR *a1, Arc_STR *a2);
	int     SHPbuildInOutLists(int *v_first_in, int *v_first_out, int *v_next_in, int *v_next_out, int *v_num_in, int *v_num_out,
		C_IST *);

	// Node Utilities
	int     SHPpointOutsideBox(SHPObject *shpObject, int i_SHP_index, XYPoint_STR p);
	int     SHPpointSide(XYPoint_STR v, XYPoint_STR w, XYPoint_STR p);

	double  SHPdistancePointArc(SHPHandle  shpHandle, DBFHandle  dbfHandle, Arc_STR *a, XYPoint_STR p, bool RightLeft, double *l_offset);
	double  SHPdistancePointPolyline(SHPHandle  shpHandle, DBFHandle dbfHandle, IShape_STR *is, XYPoint_STR p, bool RightLeft, double *l_offset);
	//double  SHPminimum_distanceP2Segment(XYPoint_STR v, XYPoint_STR w, XYPoint_STR p, double *offset);
	double  SHPplanarDistance2P(XYPoint_STR v, XYPoint_STR w);
	//int     SHPreadPoints(SHPHandle  shpHandle, DBFHandle  dbfHandle);

public:

	//
	C_SHP();
	~C_SHP();
	void clear();

        int SHP_isPointOnSegment(XYPoint_STR p, XYPoint_STR v, XYPoint_STR w, double *offset, double *distance);
	int SHP_PointsCoincide(XYPoint_STR p1, XYPoint_STR v2);
	int SHP_isPointOnArc(SHPHandle  shpHandle, XYPoint_STR p, int i_arc, C_IST *);

	int     SHP_allocateFullLines(int numArcs, C_IST *);

	int     SHP_cleanDegreeZeroAndTwo(C_IST *);
	double  SHP_myopen(char *filename, SHPHandle  *shpHandle, DBFHandle  *dbfHandle, int *shpType, int *numElements, int *numArcs, C_IST *Ist);
	int     SHP_readAndBuildLines(char *filename, C_IST *);
	int     SHP_readPolyline(SHPHandle  shpHandle, DBFHandle  dbfHandle, C_IST *);
	//void Error(char NomeProc[], char message[], int value);
	int     SHP_shrinkNearVertices(C_IST *);
	int     SHP_writeShapeFromPath(char *Instance, long narcs, long *arc, int *id, C_IST *);
	// Graph utilities
	int     SHP_buildFS(C_IST *);

	// Node Utilities
	double  SHP_Point2FS(XYPoint_STR p, bool RightLeft, int *i_arc, double *min_offset, C_IST *);
	int     SHP_readAndBuildPoints(char *filename, C_IST *);
	int     SHP_writeShapeStopPoints(char * Instance, C_IST *);
	int     SHP_writeShapeParkingPoints(char *Instance, C_IST *);
	int     SHP_writeShapeFromWaypoints(char *Instance);
	int     SHP_writeShapeRequests(char *Instance, C_IST *Ist);
	//   
	void    SHP_printlists(C_IST *Ist);
	void    SHP_printFS(C_IST *Ist);
};
#endif