/**----------------------------------------------------------------------------
* file DijkstraObj.h
*
* Class Dijkstra Definition: Path and Matrix Distance/Time computation.
*
* Last Update: 16.03.2016
* Version 1.0
*------------------------------------------------------------------------------
* Licensed Materials - Property of
\*---------------------------------------------------------------------------*/

#ifndef __DIJKSTRA_H
#define __DIJKSTRA_H

/**----------------------------------------------------------------------------
* Definitions
\*---------------------------------------------------------------------------*/
#define  MaxCod         65000
#define  MaxVal          1000  // MaxVal must be sufficiently greater than MaxCod
#define  MaxVal2          100
#define  MaxType           40
#define  AvgVel            60
#define  nverb        5600000
#define  narcb       13000000
#define  Infl      1000000000
#define  lsize             32
#define  znverb 1+nverb/lsize

/**----------------------------------------------------------------------------
* Location of a point into the map: (arc, offset)
\*---------------------------------------------------------------------------*/
struct DPoint
{
    bool StopOrPark;    /// 0 if the indexSoP refers to v_StopPoints[]; if refers to v_ParkingPoints[]
	long indexSoP;      /// index in the original list v_StopPoints[] or v_ParkingPoints[] depending on StopOrPark 
	long indexReq;      /// original index of the request v_Requests[], if ir corresponds to a request, -1 otherwise 
	long type;          /// type: 0=depot; 1=pickup; 2=delivery; 3=parking 
	long karc;          /// index of the arc of the Forward Star where the point is located 
	double offset;      /// offset with respect to the from vertex of the arc where the point is located 
};

/**----------------------------------------------------------------------------
* Class Dijkstra
\*---------------------------------------------------------------------------*/
class Dijkstra
{
	// Global Variables
	char MapName[256];  /// Name of the file containing the SFS of the map */
	char CodName[256];  /// Name of the file containing the codes of the arcs */
	char TabName[256];  /// Name of the file containing the table */

	char FlagSFS;   /// Flag SFS: 0 = NULL; 1 = allocated */
	char FlagSFSF;  /// Flag SFSF: 0 = NULL; 1 = allocated */
	char FlagCode;  /// Flag Code: 0 = NULL; 1 = allocated */
	char FlagCost;  /// Flag Cost: 0 = NULL; 1 = allocated */
	char FlagTab;   /// Flag Tab: 0 = NULL; 1 = allocated */
	char FlagDij;   /// Flag Dijkstra: 0 = NULL; 1 = allocated */ 

	long **tab;	    /// Auxiliary table to save the different road types */
	long **tabkey;  /// Auxiliary table to save the road codes */
	long *ntab;     /// Auxiliary vector used filling "tab" */
	long *ntabkey;  /// Auxiliary vector used filling "tabkey" */

	long *dist;     /// Dijkstra label */
	long *ipred;    /// Dijkstra predecessor: vertex */
	long *kpred;    /// Dijkstra predecessor: arc */
	long *next;     /// Next item in the same hash bucket */
	long *last;     /// Previous item in the same hash bucket */ 
	long *head;     /// Hash bucket head (n.1) */
	long *tail;     /// Hash bucket tail (n.1) */
	long *head2;    /// Hash bucket head (n.2) */
	long *tail2;    /// Hash bucket tail (n.2) */
	long *zsink;    /// Zipped flag: if the i-th bit of zsink[j] is 1 then node (zsink[j]*32+j) is a sink */
	long *bit;      /// Bit masks: bit[i]=2**i */

	/**----------------------------------------------------------------------------
	* Private functions
	\*---------------------------------------------------------------------------*/	
	int AllocateSFS(void);
	int AllocateSFSF(void);
	int AllocateCode(void);
	int AllocateCost(void);
	int AllocateTab(void);
	int AllocateDijkstra(void);

	int DeleteSFS(void);
	int DeleteSFSF(void);
	int DeleteCode(void);
	int DeleteTab(void);
	int DeleteDijkstra(void);

	int LeggiFull(void);
	int LeggiZip(void);
	int LeggiCod(void);
	int LeggiTable(void);
	int ScriviCod(void);
	int ScriviTable(void);

	int Elabora(void);
	int UpdateTable(int TipoPer, int nVel, int *Vel);

public:

	long nver, narc;         /// Number of vertices and arcs */
	long *index;             /// Gives gamma location for each vertex */
	long *jver;              /// Gamma^{+1}: ending vertex of the arc */
	long *iver;              /// Gamma^{-1}: starting vertex of the arc */
	long *darc;              /// Arc distance */
	unsigned short *codarc;  /// Arc code */
	char *type;              /// Arc type */

	long nkey;      /// Number of different road, i.e. code */
	char *tipo;     /// Road type */
	long *distanza; /// Road length */
	long *tempo;    /// Travelling time */
	long *costo;    /// Cost for travelling the corresponding road */

	/**----------------------------------------------------------------------------
	* Constructor and destructor
	\*---------------------------------------------------------------------------*/
	Dijkstra();
	~Dijkstra();

	/**----------------------------------------------------------------------------
	* Functions
	\*---------------------------------------------------------------------------*/
	int  Initialize(long nver, long narc);
	int  SetMapName(char *name);
	int  SetCodName(char *name);
	int  SetTabName(char *name);
	long NVer(void);
	long NArc(void);
	long ArcTime(long k);
	long ArcDistance(long k);
	long ArcCost(long k);
	long ArcIndex(long i, long j);
	long Gamma(long i, long ng, long *vertex, long *arc, long *cost, long *dist, long *time);
	int  Zip(int TipoPer, int nVel, int *Vel);
	int  ReadZip(void);
	int  UpdateVel(int TipoPer, int nVel, int *Vel);
	long ComputePath(long root, long sink, long *length, long *time, long **pred);
	long ComputePath(long root, long ns, long *sink, long *length, long *time, long **pred);
	long ComputePaths(long root, long ns, long *sink, long *length, long *time, long *predi, long *predk);
	long ComputePaths(struct DPoint root, long ns, struct DPoint *sink, long *length, long *time, long *predi, long *predk);
	long DefineCost(int TipoPer, double AvgSpeed, long distance, long time);

};

#endif /* DIJKSTRA_H */


