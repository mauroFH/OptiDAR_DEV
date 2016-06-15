/** \file solution.h  
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of 
 *---------------------------------------------------------------------------
 */

#ifndef __SOLDEFS_H
#define __SOLDEFS_H

#define	MAXN_ROUTE_NODES					50	/// max. number of nodes per route */
#define	MAXN_SOLUTION_ROUTES				50	/// max. number of routes forming a solution */

/// Node of a route
class CNode{
public:

	CNode();
	~CNode();

	int i_node; /// node index */
	int p_next; /// next node of the route in the list of free nodes */
	int p_pred; /// head of the list of nodes of the route */
	int p_request; /// if the node is a pickup (delivery) node is the pointer to the pickup (delivery) node, NULL otherwise */

	// The data below are computed during the check performed by the \c feasibility function
	float q[MAXN_DIMENSIONS]; /// cumulative load up to node i (included) */
	int a_time; /// arrival time at node i */
	int d_time; /// departute time from node i */
	int w_time; /// waiting time at node i */
	int l_time; /// latest possible departure time at node i */
	int s_time; /// service time at node i (computed taking into account of pickup/delivery on the same locations */
};

/// Route
class CRoute{

public:

	CRoute();
	~CRoute();

	CNode v_nodes[MAXN_ROUTE_NODES]; /// list of nodes forming the route */

	// Data
	int		nof_nodes;	/// number of nodes visited by the route, including the initial and final nodes */
	int		i_vehicle;	/// index of the associated vehicle, equal to -1 if not defined */


	// The data below are computed during the check performed by the \c feasibility function
	float	cost;		/// route cost */
	float	time;		/// total driving time */
	float	distance;	/// total distance */

	// Linked list of nodes forming the route and of the free node of the route
	int p_head; /// head of the list of nodes of the route */
	int p_tail; /// head of the list of nodes of the route */
	int p_free; /// pointer to the first free node of the route */

	// Functions
	int new_node(void); // allocate a node to the route
	int add_node(int i_node); // add a node to the route
	void delete_node(int p_node); // delete a node from the route
	void remove_node(int i_node); // remove a node from a route
	int add_node_after(int p_node_in, int i_node); // add a node to the route
	int add_node_before(int p_node_in, int i_node); // add a node to the route
	int add_request(int i_pickup, int i_delivery, int p_node_pickup, int p_node_delivery); // add a request to the route
	void remove_request(int p_node_pickup); // remove a request to the route
};

/// Solution
class CSolution{
	public:

		CSolution();
		~CSolution();
		
		// Data
		float   cost; /// solution cost */
		int		nof_routes; /// number of routes in solution */
		CRoute  v_routes[MAXN_SOLUTION_ROUTES];

		// Data associated with the solution
		int  nof_requests_routed; ///  number of requsets in solution */
		bool routed[MAXN_REQUESTS];  ///  =true if the request has been routed */
		bool fixed[MAXN_REQUESTS];  ///  =true if the request is fixed in solution */
		int  totpriority; /// total priority

		// Function
		void init(); // itinialize a solution
		int new_route(int i_node_start, int i_node_end); // add an empty route of type (start,end) to the solution
		void set_request(class CDar &dar, int i_request); // set the solution to add or remove the request in input
};


#endif /* __DARDEFS_H */
