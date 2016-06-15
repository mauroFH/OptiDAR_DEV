/** \file dar.cpp
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of
 * --------------------------------------------------------------------------
 */


#include "../../Modulo-Main/DARH.h"

/**
* Constructor.
* Initialize the data structure of the class
*/
CDar::CDar()
{
	//CError error;
	int i, j,k;
	flg_instance_loaded		= false;
	flg_solution_computed	= false;

	nof.depots = 0;
	nof.dimensions = 0;
	nof.nodes = 0;
	nof.requests = 0;
	nof.vehicles = 0;
	nof.vehicle_types = 0;

	// Distance matrix
	m_dist = new float **[MAXN_NODES];
	if (m_dist == NULL) error.fatal("Out of memory", __FUNCTION__);
	for (i = 0; i < MAXN_NODES; i++)
	{
		m_dist[i] = new float *[MAXN_NODES];
		if (m_dist[i] == NULL) error.fatal("Out of memory", __FUNCTION__);
	}
	for (i = 0; i < MAXN_NODES; i++){
		for (j = 0; j < MAXN_NODES; j++){
			m_dist[i][j] = new float [MAXN_VEHICLE_TYPES];
			if (m_dist[i][j] == NULL) error.fatal("Out of memory", __FUNCTION__);
		}
	}
	// time matrix
	m_time = new float **[MAXN_NODES];
	if (m_time == NULL) error.fatal("Out of memory", __FUNCTION__);
	for (i = 0; i < MAXN_NODES; i++)
	{
		m_time[i] = new float *[MAXN_NODES];
		if (m_time[i] == NULL) error.fatal("Out of memory", __FUNCTION__);
	}
	for (i = 0; i < MAXN_NODES; i++){
		for (j = 0; j < MAXN_NODES; j++){
			m_time[i][j] = new float[MAXN_VEHICLE_TYPES];
			if (m_time[i][j] == NULL) error.fatal("Out of memory", __FUNCTION__);
		}
	}
	for (i = 0; i < MAXN_NODES; i++)
		for (j = 0; j < MAXN_NODES; j++)
			for (k = 0; k < MAXN_VEHICLE_TYPES; k++){
				m_dist[i][j][k] = 0;
				m_time[i][j][k] = 0;
			}
};

/**
* Destructor
*/
CDar::~CDar()
{
	int i, j;

	if (m_dist != NULL){
		for (i = 0; i < MAXN_NODES; i++){
			for (j = 0; j < MAXN_NODES; j++){
				if (m_dist[i][j] != NULL) delete m_dist[i][j];
			}
		}
		for (i = 0; i < MAXN_NODES; i++){
			if (m_dist[i] != NULL) delete m_dist[i];
		}
		if (m_dist != NULL) delete m_dist;
	}
	if (m_time != NULL){
		for (i = 0; i < MAXN_NODES; i++){
			for (j = 0; j < MAXN_NODES; j++){
				if (m_time[i][j] != NULL) delete m_time[i][j];
			}
		}
		for (i = 0; i < MAXN_NODES; i++){
			if (m_time[i] != NULL) delete m_time[i];
		}
		if (m_time != NULL) delete m_time;
	}
};

/**
* @brief Check problem dimensions
* @return true if check terminated correctly, false otherwise
*/
bool CDar::checkdata(void)
{
	bool		feasible=true;
	if (nof.depots >= MAXN_DEPOTS)					feasible = false;
	if (nof.requests >= MAXN_REQUESTS)				feasible = false;
	if (nof.nodes >= MAXN_NODES)						feasible = false;
	if (nof.vehicles >= MAXN_VEHICLES)				feasible = false;
	if (nof.vehicle_types >= MAXN_VEHICLE_TYPES)		feasible = false;
	return (feasible);
}

/**
* @brief Distance function
  @param i departing node
  @param j arrival node
  @param t departin time at node \c i
  @param k vehile type
  @return  distance from node \c i to node \c j for vehicle type \c k departing from node \c i at time \c t
  */
float CDar::d(int i, int j, int t, int k)
{
	float dist=0;
	dist = m_dist[i][j][k];
	return (dist);
}

/**
* @brief Cost function
@param i departing node
@param j arrival node
@param t departin time at node \c i
@param k vehile type
@return  distance from node \c i to node \c j for vehicle type \c k departing from node \c i at time \c t
*/
float CDar::c(int i, int j, int t, int k)
{
	float cost = 0;
	cost = m_dist[i][j][k];
	return (cost);
}

/**
* @brief Time function
@param i departing node
@param j arrival node
@param t departin time at node \c i
@param k vehile type
@return  time from node \c i to node \c j for vehicle type \c k departing from node \c i at time \c t
*/
float CDar::t(int i, int j, int t, int k)
{
	float dist = 0;
	dist = m_time[i][j][k];
	return (dist);
}

/**
* @brief Time function
@param i departing node
@param j arrival node
@param t arrival time at node \c i
@param k vehile type
@return  time from node \c i to node \c j for vehicle type \c k arriving at node \c j at time \c t
*/
float CDar::t_b(int i, int j, int t, int k)
{
	float dist = 0;
	dist = m_time[i][j][k];
	return (dist);
}

/**
* Return the index of the request associated with the node
* @param i_node node index
* @return index of the request associated with the node, -1 if not defined
*/
int CDar::getrequest(int i_node)
{
	int i_request=-1;
	if ((i_node >= 1) && (i_node <= nof.requests))
		i_request = i_node - 1;
	if ((i_node >= nof.requests+1) && (i_node <= 2 * nof.requests))
		i_request = i_node - nof.requests-1;
	return(i_request);
}

/**
* Main function examples.
*/
void CDar::ex_main(char *Instance)
{
	//CError error;
	CRoute route;

	// Open the log file
	exflog.open("../output/example.log", ios::out);
	if (!exflog.is_open()){
		error.fatal("File opening", __FUNCTION__);
		goto END;
	}

	// Read in the instance
	ex_input();

	// Plot the instance layout
	print_latex(route, Instance);

	// Cost and time matrices
	ex_compute_mat();


END:;
	// File closing
	exflog.close();
};

/**
* Compute distance and time matrices.
*/
void CDar::ex_compute_mat(void)
{
	int i, j;
	for (i = 1; i <= nof.nodes; i++){
			m_dist[0][i][0] = m_dist[i][0][0] = 0;
			m_time[0][i][0] = m_time[i][0][0] = 0;
			for (j = 1; j <= nof.nodes; j++){
				float xi = (float) v_nodes[i].latitude;
				float yi = (float) v_nodes[i].longitude;
				float xj = (float) v_nodes[j].latitude;
				float yj = (float) v_nodes[j].longitude;
				float euc = sqrtf((xi - xj)*(xi - xj) + (yi - yj)*(yi - yj));
				int	  euc_i = int (100.0*euc);
				m_dist[i][j][0] = (float) euc_i / 100;
				m_time[i][j][0] = (float)euc_i / 100;
			}
	}
}

/**
* @brief Evaluate the insertion of a request into the route given in input
@param route route in input
@param eval_insert see \c eval_insert_str
@return  true if feasible to insert the request into the route, false otherwise
*/
bool CDar::eval_insertion(CRoute &route, eval_str &eval_insert)
{
	bool feasible;
	int p_node, p_next;
	CNode *ptr_node, *ptr_next;
	float z_old;
	CRoute route_tmp;
	int i_pickup, i_delivery, i_node, i_next, p_node_new, i_vehicle, i_before, p_before, p_after, i_after;
	constraint_type constraint;
	float exm, distance;
	bool todo;

	// Initialization
	feasible = false;
	// Current solution cost
	z_old = route.cost;
	// Copy the current route
	route_tmp = route;
	// Pickup and delivery indices
	i_pickup		= eval_insert.i_request;
	i_delivery  = i_pickup + this->nof.requests;
	i_vehicle	= route.i_vehicle;

	// Initialization
	eval_insert.pickup_pos[0] = NIL; eval_insert.pickup_pos[1] = NIL;
	eval_insert.delivery_pos[0] = NIL; eval_insert.delivery_pos[1] = NIL;
	eval_insert.exm[0] = CON_MAXFLOAT; eval_insert.exm[1] = CON_MAXFLOAT;
	// First node of the route
	p_node	 = route_tmp.p_head;
	ptr_node = &(route_tmp.v_nodes[p_node]);
	while ((p_node != NIL) && (ptr_node->p_next != NIL))
	{
		// Next node
		p_next		= ptr_node->p_next;
		ptr_next		= &(route_tmp.v_nodes[p_next]);
		while ((p_next != NIL))
		{
			todo = true;
			// Filters
			i_node = ptr_node->i_node;
			i_next = ptr_next->i_node;
			if ( (v_nodes[i_pickup].tw.l_time < v_nodes[i_node].tw.e_time) ||
				(v_nodes[i_next].tw.l_time < v_nodes[i_delivery].tw.e_time)
				)
				todo = false;
			// Node after i_node
			p_after = ptr_node->p_next;
			if (p_after != NIL){
				i_after = route_tmp.v_nodes[p_after].i_node;
				if (i_after != i_next){
					distance = d(i_pickup, i_after, 0, i_vehicle);
					if ((distance < CON_EPS) && (v_nodes[i_after].type == DELIVERY))
						todo = false;
				}
			}
			// Node before node i_next
			p_before = ptr_next->p_pred;
			if (p_before != NIL)
			{
				i_before = route_tmp.v_nodes[p_before].i_node;
				if (i_before != i_node){
					// Check if delivery is in the same location of node i_node and i_node is a pickup
					distance = d(i_before, i_delivery, 0, i_vehicle);
					if ((distance < CON_EPS) && (v_nodes[i_before].type == PICKUP))
						todo = false;
				}
			}
			if (todo){
				// Insert the pickup after p_node and the delivery before p_next
				p_node_new = route_tmp.add_request(i_pickup, i_delivery, p_node, p_next);
				assert(p_node_new >= 0);
				// Apply feasibility
				if (this->feasibility(route_tmp, constraint))
				{
					feasible = true;
					// Compute extra-cost
					exm = route_tmp.cost - z_old;
					if (exm < eval_insert.exm[0]){
						feasible = true;
						// Update 2nd-best
						eval_insert.exm[1] = eval_insert.exm[0];
						eval_insert.pickup_pos[1] = eval_insert.pickup_pos[0];
						eval_insert.delivery_pos[1] = eval_insert.delivery_pos[0];
						// New best
						eval_insert.exm[0] = exm;
						eval_insert.pickup_pos[0] = p_node;
						eval_insert.delivery_pos[0] = p_next;
					}
					else
						if (exm < eval_insert.exm[1]){
							// Update 2nd-best
							eval_insert.exm[1] = exm;
							eval_insert.pickup_pos[1] = p_node;
							eval_insert.delivery_pos[1] = p_next;

						}
				}
				// Remove the request from the route (ptr_node->p_next is the node corresponding to the delivery node)
				route_tmp.remove_request(route_tmp.v_nodes[p_node].p_next);
			}
			// Next node
			p_next = ptr_next->p_next;
			if (p_next != NIL) ptr_next = &(route.v_nodes[p_next]);
		}

		// Next node
		p_node	 = ptr_node->p_next;
		ptr_node = &(route.v_nodes[p_node]);
	}

	return(feasible);
}

/**
* @brief Evaluate the deletion of a request into the route given in input
@param route route in input
@param eval see \c eval_insert_str
@return  true if feasible to insert the request into the route, false otherwise
*/
bool CDar::eval_deletion(CRoute &route, eval_str &eval)
{
	bool feasible;
	CRoute route_tmp;
	float z_old;
	int p_node;
	constraint_type constraint;

	// Initialization
	feasible = false;
	// Current solution cost
	z_old = route.cost;
	// Copy the current route
	route_tmp = route;
	// Initialization
	eval.pickup_pos[0] = NIL; eval.pickup_pos[1] = NIL;
	eval.delivery_pos[0] = NIL; eval.delivery_pos[1] = NIL;
	eval.exm[0] = CON_MAXFLOAT; eval.exm[1] = CON_MAXFLOAT;

	// 
	p_node = eval.p_node;

	// Remove the node from the route
	route_tmp.remove_request(p_node);
	// Check feasibility
	if (this->feasibility(route_tmp, constraint)){
		feasible = true;
		eval.exm[0] = z_old - route_tmp.cost;
	}
	return(feasible);
}

/**
* @brief Evaluate the deletion of a request into the route given in input
*/
void CDar::checkrequests(void)
{
	CSolution	s_current;
	//CError		error;
	int			i_node_start, i_node_end, i;
	eval_str		eval_insert;
	CRoute		*ptr_route;
	char			buf[200];
	int			i_pickup, i_delivery, travel_time, i_request;

	// Vehicle assiged is the first vehicle of index 0
	s_current.v_routes[0].i_vehicle = 0;
	// Create a new empty route, route of index 0 is used to store the solution
	i_node_start		= v_vehicles[0].node_origin;
	i_node_end		= v_vehicles[0].node_destination;
	// Create a new route
	s_current.new_route(i_node_start, i_node_end);
	// Ptr to the nuew route
	ptr_route = &s_current.v_routes[0];
	// For each unrouted pair
	for (i = 1; i <= nof.requests; i++)
	{
		// Request i
		eval_insert.i_request = i;
		// Evaluate the insertion of request i into the current route
		if (!eval_insertion(*ptr_route, eval_insert))
		{
			snprintf(buf, sizeof(buf), "Request %d: infeasible", i);
			error.warning_opt(buf, __FUNCTION__);
			// Check ride time
			i_pickup		= i; i_delivery = i + nof.requests;
			travel_time = (int)t(i_pickup, i_delivery, 0, 0);
			i_request	= getrequest(i);
			if (v_requests[i_request].ride_limit < travel_time){
				snprintf(buf, sizeof(buf), "   -->Request infeasible for ride time: %d < %d", v_requests[i_request].ride_limit, travel_time);
				error.warning_opt(buf, __FUNCTION__);
			}
		}
	}
}


/**
* @brief Evaluate and insert parking point or stops along a route
@param route route in input
@return  true if feasible route updated, false otherwise
*/
bool CDar::insert_stops(CRoute &route)
{
	bool updated = false;
	constraint_type constraint;
	int p_node, p_pred;
	CNode *ptr_node, *ptr_pred;
	int i_node, i_node_pred, i, k, i_vehicle;
	float extratime, extratime_best, maxwtime;
	bool tobedone, feasible, done;
	CRoute route_tmp;
	int	k_best;

	// Check if vehicle assigned and number of nodes
	if (route.i_vehicle < 0 || route.nof_nodes <=4)
		return false;
	i_vehicle = route.i_vehicle;
	// Apply feasibility
	if (!feasibility(route, constraint))
		return false;
	// Maxinum waiting time at a pickup point
	maxwtime = cons_max_wtime_stops;

	// Initialization
	do
	{
		done = false;
		p_node = route.p_head;	// initial node
		ptr_node = &(route.v_nodes[p_node]);
		p_node = ptr_node->p_next;
		ptr_node = &(route.v_nodes[p_node]);
		p_node = ptr_node->p_next;
		while (p_node != NIL){
			ptr_node = &(route.v_nodes[p_node]);
			i_node = ptr_node->i_node;// index of the node
			p_pred = ptr_node->p_pred;
			ptr_pred = &(route.v_nodes[p_pred]);
			i_node_pred = ptr_pred->i_node;
			// Check for stop insertion
			tobedone = true;
			for (i = 0; i < nof.dimensions; i++)
				if (ptr_pred->q[i] > 0) tobedone = false;
			if (v_nodes[i_node_pred].type == PARKING_AREA || v_nodes[i_node_pred].type == VEHICLE_DEPOT)
				tobedone = false;
			if (v_nodes[i_node].type == PICKUP && tobedone && ptr_node->w_time > maxwtime)
			{
				// Check for insertion
				// Select the "best" stop point
				k_best = -1;
				extratime_best = CON_MAXFLOAT;
				for (k = ind.first_depot; k <= nof.nodes; k++)
				{
					// Parking point or aree
					if (v_nodes[k].type == PARKING_AREA)
					{
						// Compute extra-time
						extratime = t(i_node_pred, k, 0, i_vehicle) + t(k, i_node, 0, i_vehicle) - t(i_node_pred, i_node, 0, i_vehicle);
						// Insert the node
						route_tmp = route;
						// Add the stop before node p_node
						if (route_tmp.add_node_before(p_node, k) != NIL)
							if (feasibility(route_tmp, constraint))
							{
								if (extratime < extratime_best)
								{
									extratime_best = extratime;
									k_best = k;
								}
							}
					}
				}
				// Make insertion
				if (k_best >= 0)
				{
					// Add the stop before node p_node
					route.add_node_before(p_node, k_best);
					feasible = feasibility(route, constraint);
					assert(feasible);
					done = true;
					updated = true;
				}
			}
			if (done)
				break;
			// Next node
			p_node = ptr_node->p_next;
		}
	} while (done);
	return(updated);
}