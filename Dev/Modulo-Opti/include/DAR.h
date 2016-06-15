/** \file DAR.h  
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of 
 *---------------------------------------------------------------------------
 */

#ifndef __DARDEFS_H
#define __DARDEFS_H

/// type of graph nodes
typedef enum {
	PICKUP,  /// pickup node of a transportation request */
	DELIVERY, /// delivery node of a transportation request */
	VEHICLE_DEPOT,		/// depot * /
	PARKING_AREA /// parking area * /
} node_type;

/// type of constraints
typedef enum {
	CAPACITY,  /// vehicle capacity */
	RIDETIME,  /// ride time */
	TIMEWINDOW, /// time window */
	WAITTIME   /// maximum waiting time */
} constraint_type;

/// type of capacity constraints
typedef enum {
	S,  /// standard case */
	A,  /// case a), see analisys */
	B,	/// cases b),c), see analisys */
} ccons_type;

/// type of user associated with a request
typedef enum {
	ABLE,			/// normal user */
	DISABLED		/// handicapped user */
} request_type;


/// instance dimensions (number of, nof)
struct nof_str{
	int nodes; ///  number of nodes equal to \c 1+requests+depots */
	int vehicles; ///  number of vehicles */
	int vehicle_types; ///  number of vehicle types */
	int requests; ///  number of transportation requests */
	int depots; ///  number of depots */
	short dimensions; ///  number of distinct vehicle dimensions and corresponding distinct request demands */
};

/// indices to structures
struct ind_str{
	int first_depot; ///  index of the first depot in the list of nodes, see \ref nodes_str  */
	int last_request; ///  index of the last request in the list of nodes, see \ref nodes_str  */
	int first_delivery; ///  index of the first delivery in the list of nodes, see \ref nodes_str  */
};

/*! \brief graph nodes.
*
*  Nodes are numbered from 0 to nof.nodes.
*  Node 0 is a dummy node.
*  Nodes from 1 to nof.requests correspond to the origins of the trasportation requests.
*  Nodes from nof.requests+1 to 2*nof.requests correspond to the destrinations of the trasportation requests.
*  Node i and nof.requests+i are origin and destination of request i.
*  Nodes from 2*nof.requests+1 to nof.nodes correspond to depot nodes, \c ind.start_depots is set equal to 2*nof.requests+1
*  Summary:
*  \verbatim
			   0               [1 .. nof.requests]          [nof.requests+1 .. 2*nof.requests]      [ind.start_depots=2*nof.requests+1 .. nof.nodes]
			NOT USED               ORIGINS					       DESTINATIONS									   DEPOTS
			-----------------------------------------------------------------------------------------------------------------------------------------
																	nodes_str
			-----------------------------------------------------------------------------------------------------------------------------------------
								   requests_str																	  depots_str
			----------------------------------------------------------------------------------------------------------------------------------------- 
   \endverbatim
*/

/// time window
struct time_window_str{
	int e_time;		///  earliest time */
	int l_time;		///  latest time */
};

struct nodes_str{
	char		code[CON_MAXNSTRCODE];		///  code */
	node_type	type;					///  type of node:  \c PICKUP or \c DELIVERY or \c DEPOT (subtypes: \c VEHICLE_DEPOT or \c PARKING_AREA) */
	double		latitude;				///  geographic latitude coordinate  */
	double		longitude;				///  geographic longitude coordinate  */
	time_window_str tw;					/// time window associated with the node (derives from the corresponding tw associated with the type of node) */
	float	demand[MAXN_DIMENSIONS];			/// demand associated with the node (derives from the corresponding demand associated with the type of node), positive i pickup node,
								/// negative if delivery, 0 otherwise */
	int s_time[MAXN_VEHICLES];				/// service time associated with the node per vehicle type, computed as a function of the request and the vehile type */
	int max_w_time;						/// maxinum waiting time at the node */
	int ride_time;						/// ride time */
	int priority;						/// priority
};

/// vehicle types
struct vehicle_types_str{
	char	 code[CON_MAXNSTRCODE];		///  code */
};

/// vehicles
struct vehicles_str{
	char	 code[CON_MAXNSTRCODE];		///  code */
	short type; ///  index of the type of vehicle, from \c 0 to  \c nof.vehicle_types */
	int node_origin; ///  node index of the starting node or origin of the vehicle */
	int node_destination; ///  node index of the ending node or destination of the vehicle */
	short v_objection_nodes[MAXN_NODES]; /// \c v_objection_nodes[i]=1 if the vehicle cannot visit node of index \c i, 0 otherwise */
	int nof_slots_tw; /// number of time window slots */
	time_window_str  v_slots_tw[MAXN_VEHICLE_TWSLOTS]; /// array of time window slots */
	// Capacity constraints
	ccons_type type_ccons; /// type of capacity constraints */
	int seated_capacity; /// vehicle seated capacity */
	int standing_capacity; /// vehicle standing capacity */
	int seated_disabled; /// vehicle disabled seated capacity */
	int total_passengers; /// vehicle capacity */
	float v_capacities[MAXN_DIMENSIONS]; /// array of vehicle capacities, one for each dimension, from 0 to \c nof.dimensions */
};

/// depots
struct depots_str{
	char	 code[CON_MAXNSTRCODE];		///  code */
	node_type type;  ///  type of depot: \c VEHICLE_DEPOT or \c PARKING_AREA */
	int parking_capacity;	/// maxinum number of vehicles parked at the depot per time unit */
	time_window_str tw; /// time window at the depot */
	double		latitude;					///  geographic latitude coordinate  */
	double		longitude;					///  geographic longitude coordinate  */
};

/// 
/*! \brief transportation request (either pickup request or delivery request)
*
*  The same data structure is used to model both the pickup request and delivery request.
*  For each request \c i we have:
*  \li nodes \c i (pickup node) and \c nof.requests+i (delivery node)
*  \li pickup data (see below) and delivery data (see below); some redundancy is introduced in the data representation
*  \li the demand of the delivery request is assumed to be minus the demand of the pickup request
*/
struct requests_str{
	char	 code[CON_MAXNSTRCODE];		///  code */
	request_type type; /// type of user, \c ABLE or \c DISABLED */
	time_window_str tw; /// pickup or delivery time window */
	// to be defined properly
	float v_s_times_pickup[MAXN_VEHICLES]; /// array of service times associated to the pair vehicle-request */
	float v_s_times_delivery[MAXN_VEHICLES]; /// array of service times associated to the pair vehicle-request */
	int ride_limit; /// ride limit expressed in time units */
	int priority; /// ride limit expressed in time units */
	short v_objection_requests[MAXN_REQUESTS]; /// \c v_objections_requests[j]=1 if the request cannot be in the same vehicle of the request \c j , 0 otherwise */
	short v_objection_vehicles[MAXN_VEHICLES]; /// \c v_objection_vehicles[k]=1 if the request cannot be served by vehicle \c k , 0 otherwise */
	float demand[MAXN_DIMENSIONS];				/// demand associated with the request */
	bool		fixed;	/// = true iff the request is fixed in the solution, false otherwise */
	short	i_paired_request; /// index of the request paired with thr request, =-1 if not defined */
};

/// Data used to evaluate the insertion of a request into a route
struct eval_str{
	int i_request; /// [in]: index of the request */
	int p_node; /// [in]: node of the request */
	float exm[2]; /// [out]: best and 2nd-best extra-cost */
	int pickup_pos[2]; /// [out]: best and 2nd-best pickup positions */
	int delivery_pos[2]; /// [out]: best and 2nd-best pickup positions */
};

#ifdef MAIN
	const char *node_type_str[] = { "PICKUP", "DELIVERY", "VEHICLE_DEPOT", "PARKING_AREA" }; /// string for enum \c node_type */
#else
	extern const char *node_type_str[]; /// string for enum \c node_type */
#endif

class CDar{
	public:
		CDar();
		~CDar();

		// Flags
		bool flg_instance_loaded;	 ///  true if a complete instance has been loaded */
		bool flg_solution_computed;  ///  true if a solution has been computed */

		// Problem description
		nof_str nof; ///  instance dimensions 
		struct ind_str ind; ///  indices to structures 
		nodes_str v_nodes[MAXN_NODES];							///  array of nodes 
		vehicle_types_str v_vehicle_types[MAXN_VEHICLE_TYPES];	///  array of vehicle types 	
		vehicles_str v_vehicles[MAXN_VEHICLE_TYPES];	///  array of vehicles 
		depots_str v_depots[MAXN_DEPOTS]; ///  array of depots numbered from \c 0 to \c nof.depots, index \c 0 corresponds to index node  
		requests_str v_requests[2*MAXN_REQUESTS]; 	/// array of transportation requests, the entry \c 0 is not used whereas  the entry \c i and \c nof.requests+i
								/// refer to the origin and destination of transportation request \c i */
		float ***m_dist=NULL; /// distance matrix {d(i,j,k)}, distance from i to j for vehicle type k */
		float ***m_time=NULL; /// time matrix {d(i,j,k)}, time from i to j for vehicle type k */
		float cons_max_wtime_stops; /// maxinum waiting time for inserting parking areas or stops

		// Solution
		class CSolution solution;

		// Main functions
		// ...................... Time, Distance and Cost
		float d(int i, int j, int k, int t); // distance function d(i,j,k,t)
		float t(int i, int j, int k, int t); // distance function t(i,j,k,t)
		float t_b(int i, int j, int k, int t); // distance function t(i,j,k,t), backward version
		float c(int i, int j, int k, int t); // cost function c(i,j,k,t)
		// ...................... I/O
		void input(char *Instance);
		void input_01(char *Instance);
		void print_data(char *Instance); // print DAR data
		void out_data(char *Instance); // output of the instance data
		void out_solution(char *Instance); // output of solution
		void out_route(CRoute &route, ofstream &flog); // output of a route
		void out_route_01(CRoute &route, ofstream &flog); // output of a route

		// ...................... Feasibility
		bool feasibility(CRoute &route, constraint_type &constraint); // route feasibility
		bool feasibility(CSolution &solution, int &i_route, constraint_type &constraint); // solution feasibility
		bool feasibility_wt(CRoute &route, constraint_type &constraint);
		bool feasibility_cap(CRoute &route, constraint_type &constraint); // capacity constraints
		bool feasibility_tw(CRoute &route, constraint_type &constraint); // time constraints

		// ...................... General 
		bool eval_insertion(CRoute &route, eval_str &eval_insert); // evaluate the insertion of a request into the route given in input
		bool eval_deletion(CRoute &route, eval_str &eval_deletion); // evaluate the deletion of a request into the route given in input
		bool insert_stops(CRoute &route); // evaluate and insert stops

		// Functions used to test the code
		ofstream exflog; // lof file used by \c ex_ functions
		void ex_main(char *Instance); // main ex_ function
		void ex_input(void); // read \c ex_ data
		void ex_compute_mat(void); // compute distance and time matrices
		void print_latex(CRoute &route, char *Instance); // print \c ex_ latex data
		void print_latex_shp(CRoute &route, char *Instance); // print \c ex_ latex data

		// Utilities
		bool checkdata(void); // check problem dimensions
		int getrequest(int i_node); // return the index of the request associated with the node
		void checkrequests(void); // check requests in input for feasibility
};

#endif /* __DARDEFS_H */
