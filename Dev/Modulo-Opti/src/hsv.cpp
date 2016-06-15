/** \file hsv.cpp
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of
 * --------------------------------------------------------------------------
 */

#include "../../Modulo-Main/DARH.h"

/**
* Constructor
*/
CHsv::CHsv()
{

};

/**
* Destructor
*/
CHsv::~CHsv()
{

};


/**
* HSV heuristic
@param[in] dar dar class data
@param[out] dar a feasible DAR route and save it in class \c solution as route in position \c 0
*/
void CHsv::solve(class CDar &dar)
{
	int k;
	CSolution s_best;

	// Preprocessing
	preproc(dar);

	for (k = 0; k < dar.nof.vehicles; k++){
		// Inizialize the solution
		dar.solution.init();
		// Apply main heuristic
		main(dar,k);
		// Compare the solution found with the best current solution
		if (compare_solution_vehicle(dar.solution, s_best)){
			s_best = dar.solution;
		}
	}
	// Best solution found
	dar.solution = s_best;
	// If solutoin found, local optimize it
	if (dar.solution.nof_routes > 0){
		main(dar, dar.solution.v_routes[0].i_vehicle);
		// Insert stops in the best solution found
		dar.insert_stops(dar.solution.v_routes[0]);
	}
}

/**
* Main function
  @param dar dar class data
  @param i_vehicle vehicle index
*/
void CHsv::main(class CDar &dar, int i_vehicle)
{
	CSolution s_current, s_best, s_old;
	double z_current, z_best, z_old;
	int iter, n_worse, max_iter, n_drop, max_nworse;
	double drop_t, cool_t, accept_t;
	int i_node_start, i_node_end,i,j;
	bool initial_solution;

	#ifdef DEBUG
		flog << endl << " HSV heuristic" << endl;
	#endif

	// Initialization
	initial_solution = false;
	// Check if solution in input
	if (dar.solution.nof_routes > 0)
		initial_solution = true;

	// Parameter settings
	hsv_algp.noise		  = 0;
	hsv_algp.objf_penalty = 100000;
	if (initial_solution)
	{
		// Solution given in input
		drop_t		= 0.6;
		cool_t		= 0.8;
		max_iter		= 100;
		max_nworse	= 10;
	}
	else
	{
		// Empty solution in input
		drop_t			= 0.6;
		cool_t			= 0.99975;
		max_iter			= 500;
		max_nworse		= 20;
	}
	// For each unrouted pair
	for (i = 1; i < dar.nof.requests; i++){
		for (j = i + 1; j <= dar.nof.requests; j++){
			if (hsv_algp.noise < dar.c(i, j, -1, 0))
				hsv_algp.noise = dar.c(i, j, -1, 0);
		}
	}
	// Random seed
	f_seedran(1000);

	// Check if solution in input
	if (initial_solution)
	{
		s_current			= dar.solution;
		z_current			= compute_fitness(dar, s_current);
	}
	else
	{
		// Vehicle assiged is the first vehicle
		s_current.v_routes[0].i_vehicle = i_vehicle;
		// Create a new empty route, route of index 0 is used to store the solution
		i_node_start = dar.v_vehicles[0].node_origin;
		i_node_end	 = dar.v_vehicles[0].node_destination;
		s_current.new_route(i_node_start, i_node_end);
		// Find an initial feasible solution (if any)
		// Insert "fixed" requests
		greedy_insertion(dar, s_current, true, false);
		// Complete the solution
		if (!greedy_insertion(dar, s_current, false, false)) goto END;
		z_current = compute_fitness(dar, s_current);
	}
	
	// Parameter settings
	accept_t = z_current*0.2;
	// dar.out_route(s_current.v_routes[0], flog);
	// Initialization
	iter		= 0; n_worse = 0;
	s_best	= s_current; z_best = z_current;
	// Main loop
	while (iter < max_iter){
		// Update previous solution
		s_old = s_current; z_old = z_current;
		// Compute the number of requests to be removed
		n_drop = (int)(2 + f_random()*((double)(dar.nof.requests - 2)*drop_t));
		// Remove from the current solution n_drop request
		greedy_remove(dar, s_current, n_drop, false);
		z_current = compute_fitness(dar, s_current);
		// Apply regret heuristic
		greedy_insertion(dar, s_current, false, false);
		// Apply local optimization
		lopt_swaps(dar, s_current);
		// Check the current solution
		z_current = compute_fitness(dar, s_current);
		if (compare_solution(s_current, z_current, s_best, z_best)){
			#ifdef DEBUG
				flog << "    HSV " << " iter " << right << setw(5) << iter << "  z_current " << right << setw(10) << z_current << "  z_best " << right << setw(10) << z_best << endl;
			#endif
			z_best = z_current;
			s_best = s_current;
		}
		// Check if the current solution must be accepted
		if (accept_solution(z_current, z_best, accept_t))
		{
			z_old = z_current; s_old = s_current;
		}
		else
		{
			z_current = z_old; s_current = s_old;
		}
		// Check number of non-improvement iterations
		if (z_old <= z_current) n_worse++;
		else n_worse = 0;
		if (n_worse > max_nworse){
			z_current = z_best; s_current = s_best;
			// Compute the number of requests to be removed
			n_drop = (int)(f_random()*(double)(dar.nof.requests));
			// Remove from the current solution n_drop request
			greedy_remove(dar, s_current, n_drop, true);
			// Apply regret heuristic
			greedy_insertion(dar, s_current, false, true);
			n_worse = 0;
		}
		// Update heuristic parameters
		accept_t = accept_t * cool_t;
		iter++;
	}
	// Finall ocaloptimization
	lopt_swaps(dar, s_best);
	// Update the DAR solution with the best solution found
	dar.solution = s_best;

	#ifdef DEBUG
		//dar.out_route(dar.solution.v_routes[0], flog);
		flog << " ...terminated." << endl;
	#endif

END:;
}

/**
* Data proprocessing
@param dar dar class data
*/
void CHsv::preproc(class CDar &dar)
{
	Sorter data;
	int k;
	bool priority=false;

	for (k = 0; k < dar.nof.requests; k++){
		if (dar.v_requests[k].priority > 0){
			priority = true;
			break;
		}		
	}
	// Sorting
	data.NLevel = 1;
	// Number of positions
	data.NItem = dar.nof.requests;
	data.Init();
	for (k = 0; k < dar.nof.requests; k++)
	{
		data.Item[k].Pos = k;								 // Posizione
		if (priority)
			data.Item[k].Val[0] = dar.v_requests[k].priority;  // valore
		else{
			data.Item[k].Val[0] = dar.v_requests[k].tw.e_time;
		}
	}
	// Sort in increasing order
	data.Sort(-1);
	v_sort_requests[0] = -1;
	for (k = 0; k < dar.nof.requests; k++)
		v_sort_requests[k + 1] = (short)data.Item[k].Pos + 1;
}

/**
* Compute a feasible single-vehicle DAR solution using a greedy insertion heuristic
@param dar dar class data
@param sol solution
@param fixed = true if only fixed request must be considered, false otherwise
@param noise =true if noise activated, false otherwise
@return true if feasible solution computed, false otherwise
*/
bool CHsv::greedy_insertion(class CDar &dar, class CSolution &sol, bool fixed, bool noise)
{
	bool computed, inserted;
	int i,ii;
	eval_str eval_insert, eval_best;
	CRoute *ptr_route;
	int i_pickup, i_delivery, p_node_pickup, p_node_delivery;
	constraint_type constraint;
	bool feasible;
	float rp, noise_value;
	int p_node_new;

	// Initialization
	ptr_route = &sol.v_routes[0];
	computed = false;
	do
	{
		eval_best.i_request = NIL;
		eval_best.exm[0] = eval_best.exm[1] = CON_MAXFLOAT;
		inserted = false;
		// For each unrouted pair
		for (ii = 1; ii <= dar.nof.requests; ii++){
			i = v_sort_requests[ii];
			assert(i >= 1 && i <= dar.nof.requests);
			if (fixed && !(dar.v_requests[i-1].fixed))
				continue;
			// Check if i is routed
			if (!sol.routed[i]){
				// Request i
				eval_insert.i_request = i;
				// Evaluate the insertion of request i into the current route
				if (dar.eval_insertion(*ptr_route, eval_insert))
				{
					// Noise
					noise_value = 0;
					if (noise){
						rp			= f_random();
						noise_value = (float)(hsv_algp.noise*(rp - 0.5));
					}
					// Minimum extra-mileage
					if (eval_insert.exm[0] + noise_value < eval_best.exm[0]){
						eval_best = eval_insert;
					}
					// Maxinum regret
				}
			}
		}
		// Make the insertion (if any)
		if (eval_best.i_request >= 0){
			inserted = true;
			computed = true;
			// Insert request i into the route
			// Pickup and delivery indices
			i_pickup			= eval_best.i_request;
			i_delivery		= i_pickup + dar.nof.requests;
			p_node_pickup	= eval_best.pickup_pos[0];
			p_node_delivery = eval_best.delivery_pos[0];
			p_node_new=ptr_route->add_request(i_pickup, i_delivery, p_node_pickup, p_node_delivery);
			assert(p_node_new >= 0);
			// Apply feasibility
			feasible = dar.feasibility(*ptr_route, constraint);
			assert(feasible);
			// Update the solution
			sol.cost = ptr_route->cost;
			sol.set_request(dar,i_pickup);
			sol.totpriority += dar.v_nodes[i_pickup].priority;
			if (fixed) // the request is fixed in the solution and cannot be removed
				sol.fixed[i_pickup] = true;
		}
		// Repeat until an insertion has been done
	} while (inserted);

	return computed;
}

/**
* Remove from the solution in input n_drop requests
@param dar dar class data
@param sol solution
@param n_drop number of request to be removed
@param noise =true if noise activated, false otherwise
*/
void CHsv::greedy_remove(class CDar &dar, class CSolution &sol, int n_drop, bool noise)
{
	bool removed;
	int  n_removed;
	int p_node, i_node;
	CRoute *ptr_route;
	CNode *ptr_node;
	eval_str eval, eval_best;
	bool feasible;
	constraint_type constraint;
	float rp, noise_value;

	n_removed = 0;
	ptr_route = &sol.v_routes[0];
	do
	{
		// Initialization
		removed = false;
		eval_best.i_request = NIL;
		eval_best.exm[0] = eval_best.exm[1] = CON_MAXFLOAT;
		// First node after the initial node
		p_node = ptr_route->v_nodes[ptr_route->p_head].p_next;
		while ((p_node != NIL) && (p_node != ptr_route->p_tail)){
			ptr_node		= &(ptr_route->v_nodes[p_node]);
			i_node		= ptr_node->i_node;// index of the node
			// Check if pickup node
			// Check if request associated with the node is fixed
			if ((i_node <= dar.nof.requests) && (!sol.fixed[i_node]))
			{
				// Evaluate the deletion of request i from the route
				eval.i_request	= i_node;
				eval.p_node		= p_node;
				if (dar.eval_deletion(*ptr_route, eval))
				{
					// Noise
					noise_value = 0;
					if (noise){
						rp = f_random();
						noise_value = float (hsv_algp.noise*(rp - 0.5));
					}
					if (eval.exm[0] + noise_value < eval_best.exm[0]){
						eval_best = eval;
					}
				}
			}
			// Next node
			p_node = ptr_node->p_next;
		}
		// Make the deletion (if any)
		if (eval_best.i_request >= 0){
			removed = true;
			n_removed++;
			// Remove the request
			ptr_route->remove_request(eval_best.p_node);
			// Apply feasibility
			feasible = dar.feasibility(*ptr_route, constraint);
			assert(feasible);
			// Update the solution
			sol.cost = ptr_route->cost;
			sol.set_request(dar, eval_best.i_request);
			sol.totpriority -= dar.v_nodes[eval.i_request].priority;
		}

	} while (removed && (n_removed < n_drop));
}

/**
* Compute a feasible single-vehicle DAR solution using a greedy insertion heuristic
@param dar dar class data
@param sol solution
@return the value of the fitness function corresponding to the solution in input
*/
double CHsv::compute_fitness(class CDar &dar, class CSolution &sol)
{
	double fitness = 0;
	fitness = hsv_algp.objf_penalty*(dar.nof.requests - sol.nof_requests_routed) + sol.cost;
	return fitness;
}

/**
* Compare the two solutions in input
@param sol1 first solution
@param z1 fitness of the first solution
@param sol2 second solution
@param z2 fitness of the second solution
@return true sol1 is better than sol2, false otherwise
*/
bool CHsv::compare_solution(class CSolution &sol1, double z1, class CSolution &sol2, double z2)
{
	bool best = false;
	if (z1 < z2)
		best = true;
	else
		if (z1 == z2){
			if (sol1.totpriority < sol2.totpriority)
				best = true;
		}
	return (best);
}

/**
* Compare the two solutions in input (vehicle version)
@param sol1 first solution
@param sol2 second solution
@return true sol1 is better than sol2, false otherwise
*/
bool CHsv::compare_solution_vehicle(class CSolution &sol1, class CSolution &sol2)
{
	bool best = false;
	if (sol1.nof_requests_routed > sol2.nof_requests_routed)
		best = true;
	else 
		if (sol1.nof_requests_routed == sol2.nof_requests_routed){
			if (sol1.cost < sol2.cost)
				best = true;
		}
		
	return (best);
}

/**
* Check if the current solution must be accepted
@param z_current fitness of the current solution
@param z_best fitness of the current best solution
@param accept_t parameter
@return true if solution accepted, false otherwise
*/
bool CHsv::accept_solution(double z_current, double z_best, double accept_t)
{
	bool accepted = false;
	double slack,exp,rp;

	slack	= z_current - z_best;
	exp		= -slack / accept_t;
	rp		= pow(2.71828, exp);
	if (f_random() < rp)
		accepted = true;
	else
		accepted = false;
	return accepted;
}

/**
* Performs a local-optimization procedure based on request swaps
@param dar dar class data
@param sol solution
*/
void CHsv::lopt_swaps(class CDar &dar, class CSolution &sol)
{
	int p_node, i_node;
	CRoute *ptr_route;
	CNode *ptr_node;
	eval_str eval;
	bool feasible;
	constraint_type constraint;
	int i_pickup, i_delivery, p_node_pickup, p_node_delivery;
	float z_old;
	int p_node_new;

	// Route
	ptr_route = &sol.v_routes[0];
	// First node after the initial node
	p_node	 = ptr_route->v_nodes[ptr_route->p_head].p_next;
	ptr_node = &(ptr_route->v_nodes[p_node]);
	z_old = ptr_route->cost;
	while ((p_node != NIL) && (ptr_node->p_next != ptr_route->p_tail))
	{
		i_node = ptr_node->i_node;// index of the node
		// Check if pickup node
		// Check if request associated with the node is fixed
		if ((i_node <= dar.nof.requests) && (!sol.fixed[i_node]))
		{
			// Remove the request from the route
			ptr_route->remove_request(p_node);
			// Re-do feasibility
			feasible = dar.feasibility(*ptr_route, constraint);
			// Re-insert the request
			// Request i
			eval.i_request = i_node;
			feasible=dar.eval_insertion(*ptr_route, eval);
			assert(feasible);
			i_pickup				= i_node;
			i_delivery			= i_pickup + dar.nof.requests;
			p_node_pickup		= eval.pickup_pos[0];
			p_node_delivery		= eval.delivery_pos[0];
			p_node_new=ptr_route->add_request(i_node, i_delivery, p_node_pickup, p_node_delivery);
			assert(p_node_new >= 0);
			// Apply feasibility
			feasible = dar.feasibility(*ptr_route, constraint);
			assert(feasible);
			// Update the solution
			sol.cost = ptr_route->cost;
		}
		// Next node
		p_node = ptr_node->p_next;
		ptr_node = &(ptr_route->v_nodes[p_node]);
	}
	assert(z_old >= ptr_route->cost);
}
