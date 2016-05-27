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

#define CON_MAXLOAD                  60 /// Maximum number of customer that can be loaded on one vehicle    
#define CON_LASTTIME           60*60*48 /// Maximum time in seconds
    
#define CON_MAXVEHICLETYPES          10 /// Maximum number of vehicle tyoes 

   struct XYPoint_STR{
        double X, Y;
    };

   struct StopPoint_STR{
        int StopPointId;                       /// 
        unsigned char Cod[CON_MAXNCODSTOPPOINT];           /// name which identifies univocally the stop
        unsigned char Descr[CON_MAXNDESCRSTOPPOINT]; /// description of the stop
        int i_Point;                       /// index of the point in v_Points[]]
   }; 
    
   struct Point_STR{
        int PointId;                       /// 
         XYPoint_STR P;                       /// Coordinates X and Y
        int i_arc;                         /// index of the arc of the forward start that must be used to visit the point
        double offset ;                    /// offset of the stop from the initial vertex of i_arc
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
        int i_pickup_stop;      /// index of the pickup stop in v_Stops[]
        int i_dropoff_stop;     /// index of the delivery stop in v_Stops[]   
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
#endif /* DB_H */

