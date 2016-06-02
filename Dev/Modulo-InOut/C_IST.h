    /*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DB.h
 * Author: Mauro
 *
 * Created on 18 aprile 2016, 10.37
 */

#ifndef DB_H
#define DB_H

using namespace std;  

//enum TypeOfPoint{
//    Address,        /// normal pickup or delivery point 
//    ParkingPoint,   /// a parking point 
//};

enum ActionType{
	Pickup,         /// pickup 
	DropOff,        /// drop 
};

#ifdef MAIN
//    const char *TypeOfPointDescr[]{ "Address","ParkingPoint"};
    const char *ActionTypeDescr[]{ "Pickup","DropOff"};
#else
//    extern const char *TypeOfPointDescr[];
    extern const char *ActionTypeDescr[];
#endif
//#define TYPEOFPOINTFIND(A)  strncmp((char*)(A),TypeOfPointDescr[0],strlen(TypeOfPointDescr[0])) == 0 ? 0 :  strncmp((char*)(A),TypeOfPointDescr[1],strlen(TypeOfPointDescr[1])) == 0 ? 1 : -1;
#define ACTIONTYPEOFPOINTFIND(A)  strncmp((char*)(A),ActionTypeDescr[0],strlen(ActionTypeDescr[0])) == 0 ? 0 :  strncmp((char*)(A),ActionTypeDescr[1],strlen(ActionTypeDescr[1])) == 0 ? 1 : -1;

//
// DB interface dimensions
//
#define CON_MAXNOTURN                 4 /// maximum number of fields defining a forbidden turn
#define CON_MAXNCODSTOPPOINT         50 /// StopPoint CODE size
#define CON_MAXNDESCRSTOPPOINT      250 /// StopPoint DESCRIPTION size
#define CON_MAXNCODPOINT             50 /// Point CODE size
#define CON_MAXNDESCRPOINT          250 /// Point DESCRIPTION size  
#define CON_MAXNCODPARKINGPOINT      50 /// ParkingPoint CODE size
#define CON_MAXNDESCRPARKINGPOINT   250 /// ParkingPoint DESCRIPTION size       
#define CON_MAXNCODREQUEST           50 /// Request code not used ???
#define CON_MAXNCUSTOMERCODREQUEST   20 /// Request CUSTOMERCODE size
#define CON_MAXNDESCRREQUEST        100 /// Request DESCRIPTION size
#define CON_MAXNCODTOV               50 /// Type Of Vehicle CODE size
#define CON_MAXNDESCRTOV            250 /// Type Of Vehicle DESCRIPTION size
#define CON_MAXNCODVEHICLE           50 /// Vehicle CODE size
#define CON_MAXNDESCRVEHICLE        250 /// Vehicle DESCRIPTION size
#define CON_MAXNSTR                 250 /// Generic string size
#define CON_MAXCODEVALUE          10000 /// Generic maximum numeric code value

#define CON_MAXLOAD                  60 /// Maximum number of customer that can be loaded on one vehicle    
#define CON_LASTTIME           60*60*48 /// Maximum time in seconds
    
#define CON_MAXVEHICLETYPES          10 /// Maximum number of vehicle tyoes 
#define CON_MAXSTOPSROUTE           200 /// Maximum number of stops per route (used for setting up RoutePointId)
#define CON_MAXWAYPOINTSROUTE    100000 /// Maximum number of waypoints per route (used for setting up WayPointId)

struct IShape_STR {
    long index;    /// index of the shapefile element (polyline) corresponding to this arc; **/
    int  direction;/// if negative the arc is in the reverse direction. This may happen if the line is bidirectional 
                   /// or if the line is tranversed in the opposite direction (attribute ONEWAY = TF)                       
    long long id;  /// id of the shapeobject */
    int length;    /// meters */
    IShape_STR *next;  /// pointer to the next polyline corresponding to the same arc; NULL terminates*/
    IShape_STR *prev;  /// pointer to the previous polyline corresponding to the same arc; NULL terminates*/
};

struct Info_STR{ 
    long long id;           /// id of the first shapeobject associated to this arc
    int  length;            /// meters 
    int  speed;             /// kmh
    float minutes;          /// travel time in minutes  = meters * kmh *1000 / 60
    long long noturn[CON_MAXNOTURN]; /// indices of arcs in which we cannot go directly from thi arc
    struct IShape_STR *i_shp_first;   /// first polyline associated with this arc
    struct IShape_STR *i_shp_last;   /// last polyline associated with this arc
};

struct Arc_STR{
    int from;
    int fromold;  // to be removed when debugging is complete
    int toold;    // to be removed when debugging is complete
    int to;
    Info_STR info;
};

struct arc_FS_STR{
    int from;         /// first vertex of the arc
    int to;           /// second vertex of the arc
    int length;       /// length f the arc
    float minutes;    /// travelling time on the arc
    int original_Arc; /// index of the arc of the not expanded graph in SHP_Arcs_List
};

struct XYPoint_STR{
	double X; 
	double Y;
};

struct StopPoint_STR{
    int StopPointId;                   /// 
    unsigned char Cod[CON_MAXNCODSTOPPOINT];           /// name which identifies univocally the stop
    unsigned char Descr[CON_MAXNDESCRSTOPPOINT]; /// description of the stop
    int i_Point;                       /// index of the point in v_Points[]]
}; 

struct Point_STR{
    int PointId;                       /// 
    XYPoint_STR P;                     /// Coordinates X and Y
    int i_arc;                         /// index of the arc of the forward start that must be used to visit the point
    double offset ;                    /// offset of the stop from the initial vertex of i_arc
    int i_arc2;                        /// if the point is a depot, index of the possible second (reverse) arc of the forward start
                                       /// that can be used to visit the point; -1 otherwise
    double offset2 ;                   /// offset of the stop from the initial vertex of i_arc2     
};     

struct ParkingPoint_STR{
    int ParkingPointId;
    int i_Point;                                    /// index of the point in v_Points[]
    bool isDepot;
    unsigned char Cod[CON_MAXNCODPARKINGPOINT];     
    unsigned char Descr[CON_MAXNDESCRPARKINGPOINT]; 
    int capacity[CON_MAXVEHICLETYPES];              /// parking capacity for each type of vehicle
};

struct RequestDetails_STR{
    ActionType type;        /// pickup or dropoff
    int StartTime;          /// Starting time of the time window
    int EndTime;            /// Ending time of the time window
    int EstimatedTime;      /// Estimated arriva in the associated stop, if the request is confirmed
    int RequestAddressId;   /// id in RequestAddress table
};

struct Request_STR{
    int RequestId;         
    char Cod[CON_MAXNCODREQUEST];
    int Priority;
    int i_TypeOfVehicle;    /// index in v_TypeOfVehicles[]
    char CustomerCode[CON_MAXNCUSTOMERCODREQUEST];     
    int MaxTransit;         /// Maximum transit time (on the vehicle)
    int ServiceTime;        /// time needed to enter or exit the vehicle
    int ConfirmedTime;      /// if > 0 the request must be served at this time
    int LoadNormal;         /// Number of customers without disabilities 
    int LoadDisabled;       /// Number of customers with disabilities
    int LoadWheelChair;     /// Number of customers with wheel chair
    int i_Vheicle;          /// Index of v_Vehicles[]
    int ConfirmedRange;     /// Maximum delay for the confirmed requests
    int TimeWindowRange;    /// Maximum relaxation of the time window limits
    int i_pickup_stop;      /// index of the pickup stop in v_StopPoints[]
    int i_dropoff_stop;     /// index of the delivery stop in v_StopPointss[]   
    struct RequestDetails_STR pickup;
    struct RequestDetails_STR dropoff;
}; 

struct Vehicle_STR{
    int VehicleId;        
    int i_TypeOfVehicle;    /// index in v_TypeOfVehicle[]
    char Callsign[CON_MAXNCODVEHICLE];  /// Code of the Vehicle
    int StandingCapacity;
    int SeatedCapacity;
    int WheelChairCapacity;
    int TotalCapacity;        
};

struct VehicleSchedule_STR{
    int VehicleScheduleId;
    int i_Vehicle;                       /// index in  v_Vehicles[]
    int StartTime;
    int EndTime;
    int i_StartDepot;                    /// index in v_ParkingPoints[] of the starting depot
    int i_EndDepot;                      /// index in v_ParkingPoints[] of the ending depot
};   

struct TypeOfVehicle_STR{
    int TypeOfVehicleId;
    unsigned char Cod[CON_MAXNCODTOV];           
    unsigned char Descr[CON_MAXNDESCRTOV]; 
};   
    
struct route_arcs{
    long npoints;   ///  Number of points (dobbiamo eliminarlo?) 
//	long nvertices; ///  Number of vertices, the number of arcs is also npoints 
    long *arc;      ///  Ordered list (from start to end) of the arcs of v_ArcsFS[] travelled by the route 
    long sarc;      ///  Starting arc (included in "arcs" at the beginning) 
    float soffset;  ///  Offset of the starting point on the starting arc 
    long earc;      ///  Ending arc (included in "arcs" at the end) 
    float eoffset;  ///  Offset of the ending point on the ending arc 
    long nstop;     ///  Number of stops along the path 
};

struct route_out{
    long indexDT;       /// Index in v_DT_back[] 
    long indexReq;      /// Index in the request list v_Request[] 
    bool StopOrPark;    /// 0 if the indexSoP refers to v_StopPoints[]; 1 if refers to v_ParkingPoints[]        
    long indexSoP;      /// Index in  v_StopPoints[] or in v_ParkingPoints []
    char code[CON_MAXNCODREQUEST];  /// Request code 
    long type;          /// Type: 0=DEPOT, 1=PICKUP, 2=DELIVERY, 3=PARK (others?) 
    long distance;      /// Distance travelled (from the beginning) 
    long time;          /// Travel time (from the beginning) 
    long seated;        /// Passenger seated 
    long standing;      /// Passenger stand 
    long disable;       /// Passenger disable 
    long atime;         /// Arrival time 
    long dtime;         /// Departure time 
    long indexArc;      /// Position of the stop in the sequence "arc" available in the route_arcs description of the path 
                        /// (i.e., the index of the arc in RArcs.arcs[] where the current stop is serviced) 
};

struct tab_route{
    long VehicleID;     /// VehicleID
    long nstop;         /// number of elements in v_stop_out
    struct route_out *v_stop_out;
    struct route_arcs RArcs;
};

struct matrice_dt{
    long dim;         /// Number of row/column of the matrix DT
    long nvert;       /// Number of vertex in the corresponding graph
    long **m_length;  /// Length matrix  (size: dim x dim)
    long **m_time;    /// Time matrix (size: dim x dim)
    long **m_vpred;   /// Predecessor vertex matrix (size: dim x nvert)
    long **m_apred;   /// Predecessor arc matrix  (size: dim x nevert)
    long **m_mapi;    /// real starting index in the original matrix  
    long **m_mapj;    /// real ending index in the original matrix  
};

struct parameters{
	double AvgSpeed;  /// Average speed used in the arc cost definition
	long PathType;    /// 0=shortest; 100=fastest; ]0,100[: a convex combination
};

class C_IST{
private:

public:
	C_IST(){
		SessionID = -1;
		SHP_num_Vertices = SHP_num_Arcs = 0;
		v_SHP_Arcs_List = NULL;
		v_SHP_Vertices_List = NULL;
		// v_SHP_Points_List = NULL;
		v_fromFS = NULL;
		v_ArcsFS = NULL;
		v_original_Vertex = NULL;
	}
	~C_IST(){
		delete[] v_SHP_Vertices_List;
		delete[] v_SHP_Arcs_List;
		delete[] v_fromFS;
		delete[] v_ArcsFS;
		delete[] v_original_Vertex;
		v_SHP_Vertices_List = NULL;
		v_fromFS = v_original_Vertex = NULL;
		v_SHP_Arcs_List = NULL;
		v_ArcsFS = NULL;
	};

	parameters Param;  /// Parameters
	int SessionID;     /// ID Session
	int SolutionID;    /// ID Solution
	int SolutionNum;   /// Solution Number
	int RouteID;       /// ID Route
	int LogID;         /// ID log
	char networkFileName[250]; /// shapefile name without extension
	char logFileName[250];     /// log-file name
	char logcsvFileName[250];  /// csv-log-file name
	FILE *flog;                /// file pointer for the log-file
	FILE *fcsv;                /// file pointer for the csv-log-file
	int debug;                 /// 0 = no extra log; 1 = extra log (also on console)

	XYPoint_STR *v_SHP_Vertices_List; /// vector used to store all the vertices read from the shapefile
	Arc_STR     *v_SHP_Arcs_List;     /// vector used to store all the Arcs read from the shapefile

	int SHP_num_Vertices;           /// number of elements currently stored in v_SHP_Vertices_List
	int SHP_num_Arcs;               /// number of elements currently stored in v_SHP_Arcs_List
	/// FORWARD STAR OF THE EXPANDED GRAPH
	int *v_fromFS;                  /// v_fromFS[i] points to the first arc emanating from vertex 
	struct arc_FS_STR *v_ArcsFS;    /// List of arcs : arcs emanating from the same vertex are contiguous 
	int num_VerticesFS, num_ArcsFS; /// self explaining
	/// Points
	struct Point_STR *v_Points;     /// an array containing all the physical points
	int num_Points;
	/// Stops
	struct StopPoint_STR *v_StopPoints; /// an array containing all the stops
	int num_StopPoints;
	/// ParkingPoints
	struct ParkingPoint_STR *v_ParkingPoints; /// an array containing all the parking points
	int num_ParkingPoints;
	/// Requests
	struct Request_STR *v_Requests; /// array containing the requests 
	int num_Requests;
	/// Vehicles
	struct Vehicle_STR *v_Vehicles; /// array containing ...
	int num_Vehicles;
	/// Vehicles schdules
	struct VehicleSchedule_STR *v_VehicleSchedules;  /// array containing ...
	int num_VehicleSchedules;
	/// Types of Vehicles
	struct TypeOfVehicle_STR *v_TypeOfVehicles;  /// array containing ...
	int num_TypeOfVehicles;

	/// used for debugging
	int *v_original_Vertex;    ///  v_original_vertex[i] points to the vertex of the intermediate graph that has been
	/// expanded, generating several vertices among which vertex i 

	struct DPoint *v_DT_back;   /// *v_DT_back returns back from a row of matrixDT to the original structures
	/// v_DT_back[i] gives the informations on the stop and/or request associated with row i of matrixDT 
	struct route_arcs Rarcs;    /// arcs of a route TRoute
	struct tab_route TRoute;    /// one route
	struct matrice_dt MatrixDT; /// che ne so ?
};
    
#endif /* DB_H */

