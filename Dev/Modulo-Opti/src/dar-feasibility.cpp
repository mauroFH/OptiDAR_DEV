/** \file dar-feasibility.cpp
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of
 * --------------------------------------------------------------------------
 */


#include "../../Modulo-Main/DARH.h"

/**
* @brief Feasibility function (route version) 
* @param[in] route route in input
* @param[out] constraint type of constraint violated (if any)
* @return  true, if the route is feasible
*/
bool CDar::feasibility(CRoute &route, constraint_type &constraint)
{
	// Check if vehicle assigned
	if (route.i_vehicle < 0){
		constraint = CAPACITY;
		return false;
	}
	// Capacity constraints
	if (!feasibility_cap(route, constraint)){
		constraint = CAPACITY;
		return false;
	}
	// Capacity constraints
	if (!feasibility_tw(route, constraint)){
		return false;
	}
	return true;
}

/**
* @brief Feasibility function (route version, time constraints)
* @param[in] route route in input
* @param[out] constraint type of constraint violated (if any)
* @return  true, if the route is feasible
*/
bool CDar::feasibility_tw(CRoute &route, constraint_type &constraint)
{
	int p_node, p_pred, p_pickup;
	CNode *ptr_node, *ptr_pred, *ptr_pickup, *ptr_next;
	int i_node, i_vehicle, i_node_pred, i_node_pickup, i_node_next, p_next;
	int travel_time, service_time, e_time, l_time;
	float distance, cost, next_distance;
	int delta;
	int st_par, st_seq_ABLE, st_seq_DISABLE, i_request, c_st;
	i_vehicle = route.i_vehicle;

	// Initialization
	i_vehicle		= route.i_vehicle;
	route.time		= 0;
	route.distance	= 0;
	route.cost		= 0;
	// Origin (note: must be changed if more than one vehicle is available)
	p_node				= route.p_head;
	ptr_node				= &(route.v_nodes[p_node]);
	i_node				= ptr_node->i_node;// index of the node
	ptr_node->s_time		= 0;
	ptr_node->a_time		= v_nodes[i_node].tw.e_time;
	ptr_node->d_time		= v_nodes[i_node].tw.e_time;
	ptr_node->l_time		= v_nodes[i_node].tw.l_time;
	ptr_node->w_time		= 0;
	// Next node
	p_node			= route.v_nodes[route.p_head].p_next;
	while (p_node != NIL)
	{
		ptr_node		= &(route.v_nodes[p_node]);
		i_node		= ptr_node->i_node;// index of the node
		p_pred		= ptr_node->p_pred;
		ptr_pred		= &(route.v_nodes[p_pred]);
		i_node_pred = ptr_pred->i_node;
		// Check times
		travel_time		= (int)t(i_node_pred, i_node, ptr_pred->d_time, i_vehicle);
		distance			= d(i_node_pred, i_node, ptr_pred->d_time, i_vehicle);
		cost				= c(i_node_pred, i_node, ptr_pred->d_time, i_vehicle);
		service_time		= v_nodes[i_node].s_time[i_vehicle];
		e_time			= v_nodes[i_node].tw.e_time;
		l_time			= v_nodes[i_node].tw.l_time;
		route.time		+= travel_time;
		route.distance	+= distance;
		route.cost		+= cost;
		// 
		ptr_node->a_time = ptr_pred->d_time + travel_time;
		// Check TIME WINDOWS
		if (ptr_node->a_time > l_time){
			constraint = TIMEWINDOW;
			return false;
		}
		ptr_node->w_time = max(e_time - ptr_node->a_time, 0);
		ptr_node->d_time = ptr_node->a_time + ptr_node->w_time + service_time;
		ptr_node->l_time = l_time;
		ptr_node->s_time = service_time;
		// Adjust the departing time from the previous node
		if (p_pred == route.p_head || v_nodes[i_node_pred].type == PARKING_AREA)
		{
			ptr_pred->d_time = max(ptr_pred->d_time, ptr_node->a_time + ptr_node->w_time - travel_time);
			ptr_node->w_time = 0;
			ptr_node->a_time = max(ptr_node->a_time, e_time);
		}
		//
		// Check RIDE TIME
		if (v_nodes[i_node].type == DELIVERY)
		{
			// Node corredponsind to the delivery
			p_pickup			= ptr_node->p_request;
			ptr_pickup		= &(route.v_nodes[p_pickup]);
			i_node_pickup	= ptr_pickup->i_node;
			delta			= (ptr_node->a_time - ptr_pickup->d_time) - v_nodes[i_node].ride_time;
			if (delta > 0){
				constraint = RIDETIME;
				return false;
			}
		}
		// Check if nodes in the same location
		p_next = ptr_node->p_next;
		if (p_next != NIL && p_next != route.p_tail){
			ptr_next			= &(route.v_nodes[p_next]);
			i_node_next		= ptr_next->i_node;
			next_distance	= d(i_node, i_node_next, ptr_node->a_time, i_vehicle);
			if (next_distance < 1)
			{
				st_par		= st_seq_ABLE = st_seq_DISABLE = 0;
				i_request	= getrequest(i_node);
				if (i_request > 0){
					if (v_requests[i_request].type == ABLE)
					{
						if (v_nodes[i_node].type == PICKUP)		st_seq_ABLE = ptr_node->s_time;
						if (v_nodes[i_node].type == DELIVERY)	st_par = ptr_node->s_time;
					}
					if (v_requests[i_request].type == DISABLED)
					{
						if (v_nodes[i_node].type == PICKUP)		st_seq_DISABLE = ptr_node->s_time;
						if (v_nodes[i_node].type == DELIVERY)	st_seq_DISABLE = ptr_node->s_time;
					}
				}
				while ((next_distance < 1) && p_next != NIL){
					ptr_next = &(route.v_nodes[p_next]);
					i_node_next = ptr_next->i_node;
					// Arrival and waiting times at the node
					ptr_next->a_time = ptr_node->a_time;
					ptr_next->w_time = max(v_nodes[i_node_next].tw.e_time - ptr_node->a_time, 0);
					// Check RIDE TIME
					if (v_nodes[i_node_next].type == DELIVERY)
					{
						// Node corredponsind to the delivery
						p_pickup			= ptr_next->p_request;
						ptr_pickup		= &(route.v_nodes[p_pickup]);
						i_node_pickup	= ptr_pickup->i_node;
						delta			= (ptr_next->a_time - ptr_pickup->d_time) - v_nodes[i_node_next].ride_time;
						if (delta > 0){
							constraint = RIDETIME;
							return false;
						}
					}
					// Service time
					service_time = v_nodes[i_node_next].s_time[i_vehicle];
					ptr_next->s_time = service_time;
					i_request = getrequest(i_node_next);
					if (i_request > 0)
					{
						if (v_requests[i_request].type == ABLE)
						{
							if (v_nodes[i_node].type == PICKUP)		st_seq_ABLE = st_seq_ABLE + ptr_node->s_time;
							if (v_nodes[i_node].type == DELIVERY)	st_par = max(ptr_node->s_time, service_time);
						}
						if (v_requests[i_request].type == DISABLED)
						{
							if (v_nodes[i_node].type == PICKUP)		st_seq_DISABLE = st_seq_DISABLE + ptr_node->s_time;
							if (v_nodes[i_node].type == DELIVERY)	st_seq_DISABLE = st_seq_DISABLE + ptr_node->s_time;
						}
					}
					c_st	 = max(st_seq_ABLE, st_par);
					if (c_st < st_seq_DISABLE) c_st = st_seq_DISABLE;
					ptr_next->d_time = ptr_node->a_time + c_st;
					ptr_next->l_time = v_nodes[i_node_next].tw.l_time;
					//
					if (ptr_next->p_next != NIL){
						next_distance = d(i_node_next, route.v_nodes[ptr_next->p_next].i_node, ptr_next->a_time, i_vehicle);
					}
					// Next node
					p_next = ptr_next->p_next;
				}
				p_node = p_next;
			}
		}
		// Next node
		p_node = ptr_node->p_next;
	}

	return true;
}

/**
* @brief Feasibility function (route version, capacity constraints)
* @param[in] route route in input
* @param[out] constraint type of constraint violated (if any)
* @return  true, if the route is feasible
*/
bool CDar::feasibility_cap(CRoute &route, constraint_type &constraint)
{
	int p_node, p_pred;
	CNode *ptr_node, *ptr_pred;
	int i, i_node, i_vehicle, i_node_pred, i_request;
	bool feasible;

	// Check if vehicle assigned
	if (route.i_vehicle < 0){
		constraint = CAPACITY;
		return false;
	}
	// Initialization
	feasible		= true;
	i_vehicle	= route.i_vehicle;
	p_node		= route.p_head;	// initial node
	ptr_node		= &(route.v_nodes[p_node]);
	for (i = 0; i < nof.dimensions; i++)
		ptr_node->q[i] = 0;

	//////////////////////////////////////////////////// case a)
	if ((v_vehicles[i_vehicle].type_ccons == S) || (v_vehicles[i_vehicle].type_ccons == A))
	{
		// Initialization
		p_node	 = route.p_head;	// initial node
		ptr_node = &(route.v_nodes[p_node]);
		// Capacity constraints
		p_node = ptr_node->p_next;
		while ((p_node != NIL) && (p_node != route.p_tail)){
			ptr_node		= &(route.v_nodes[p_node]);
			i_node		= ptr_node->i_node;// index of the node
			p_pred		= ptr_node->p_pred;
			ptr_pred		= &(route.v_nodes[p_pred]);
			i_node_pred = ptr_pred->i_node;
			i_request	= getrequest(i_node);
			// Cunulative demand
			if (i_request < 0){
				// intermediate node
				for (i = 0; i < nof.dimensions; i++)
					ptr_node->q[i] = ptr_pred->q[i];
			}
			else{
				// request node
				for (i = 0; i < nof.dimensions; i++)
				{
					ptr_node->q[i] = ptr_pred->q[i] + v_nodes[i_node].demand[i];
					// Check CAPACITY constrint
					if (ptr_node->q[i] > v_vehicles[i_vehicle].v_capacities[i]){
						constraint = CAPACITY;
						return false;
					}
				}
			}
			// Next node
			p_node = ptr_node->p_next;
		}
	}
	//////////////////////////////////////////////////// cases b) and c)
	if ( (v_vehicles[i_vehicle].type_ccons == B) )
	{
		bool found;
		// Initialization
		p_node	 = route.p_head;	// initial node
		ptr_node = &(route.v_nodes[p_node]);
		// Capacity constraints
		p_node = ptr_node->p_next;
		while ((p_node != NIL) && (p_node != route.p_tail)){
			ptr_node		= &(route.v_nodes[p_node]);
			i_node		= ptr_node->i_node;// index of the node
			p_pred		= ptr_node->p_pred;
			ptr_pred		= &(route.v_nodes[p_pred]);
			i_node_pred = ptr_pred->i_node;
			i_request	= getrequest(i_node);
			if (i_request < 0){
				// intermediate node
				for (i = 0; i < nof.dimensions; i++)
					ptr_node->q[i] = ptr_pred->q[i];
			}
			else
			{
				if (v_requests[i_request].type == DISABLED)
				{
					if (v_nodes[i_node].demand[0] == 0)
					{ // Pure DISABLED
						// Cunulative demand
						for (i = 0; i < nof.dimensions; i++){
							ptr_node->q[i] = ptr_pred->q[i] + v_nodes[i_node].demand[i];
							// Check CAPACITY constraint
							if (ptr_node->q[i] > v_vehicles[i_vehicle].v_capacities[i]){
								constraint = CAPACITY;
								return false;
							}
						}
					}
					else // Combined ABLE + DISABLED
					{
						// Disabled (seated)
						ptr_node->q[1] = ptr_pred->q[1] + v_nodes[i_node].demand[1];
						// Check CAPACITY constraint
						if (ptr_node->q[1] > v_vehicles[i_vehicle].v_capacities[1]){
							constraint = CAPACITY;
							return false;
						}
						// Abled (standing or seated)
						if (v_nodes[i_node].demand[0] > 0)
						{ // PICKUP
							found = false;
							for (i = nof.dimensions - 1; i >= 0; i--) // starts from standing capacity
							{
								ptr_node->q[i] = ptr_pred->q[i] + v_nodes[i_node].demand[0];
								// Check CAPACITY constraint
								if (ptr_node->q[i] <= v_vehicles[i_vehicle].v_capacities[i]){
									found = true;
									break;
								}
								else
									ptr_node->q[i] = ptr_pred->q[i] - v_nodes[i_node].demand[0];
							}
							if (!found){
								constraint = CAPACITY;
								return false;
							}
						}
						else
						{ // DELIVERY
							for (i = nof.dimensions - 1; i >= 0; i--)
							{
								if (ptr_node->q[i] >= v_nodes[i_node].demand[0])
								{
									ptr_node->q[i] = ptr_pred->q[i] - v_nodes[i_node].demand[0];
									break;
								}
							}
						}
					}
				}
				if (v_requests[i_request].type == ABLE){
					if (v_nodes[i_node].demand[0] > 0)
					{ // PICKUP
						found = false;
						for (i = nof.dimensions - 1; i >= 0; i--) // starts from standing capacity
						{
							ptr_node->q[i] = ptr_pred->q[i] + v_nodes[i_node].demand[0];
							// Check CAPACITY constraint
							if (ptr_node->q[i] <= v_vehicles[i_vehicle].v_capacities[i]){
								found = true;
								break;
							}
							else
								ptr_node->q[i] = ptr_pred->q[i] - v_nodes[i_node].demand[0];
						}
						if (!found){
							constraint = CAPACITY;
							return false;
						}
					}
					else
					{ // DELIVERY
						for (i = nof.dimensions - 1; i >= 0; i--)
						{
							if (ptr_node->q[i] >= v_nodes[i_node].demand[0])
							{
								ptr_node->q[i] = ptr_pred->q[i] - v_nodes[i_node].demand[0];
								break;
							}
						}
					}
				}
			}
			// Next node
			p_node = ptr_node->p_next;
		}
	}

	return (feasible);
}

/**
* @brief Feasibility function (solution version)
* @param[in] solution solution in input
* @param[out] i_route index of the non feasible route (if any)
* @param[out] constraint type of constraint violated (if any)
* @return  true, if the solution is feasible
*/
bool CDar::feasibility(CSolution &solution, int &i_route, constraint_type &constraint)
{
	for (i_route = 0; i_route < solution.nof_routes; i_route++){
		if (!feasibility(solution.v_routes[i_route], constraint))
			return (false);
	}
	return true;
}

/**
* @brief Feasibility function (route version) with maximum waiting time
* @param[in] route route in input
* @param[out] constraint type of constraint violated (if any)
* @return  true, if the route is feasible
*/
bool CDar::feasibility_wt(CRoute &route, constraint_type &constraint)
{
	int p_node, p_pred, p_next, p_delivery, p_pickup;
	CNode *ptr_node, *ptr_pred, *ptr_next, *ptr_delivery, *ptr_pickup;
	int i, i_node, i_vehicle, i_node_pred, i_node_delivery, i_node_pickup;
	int travel_time, service_time, max_w_time, e_time, l_time;
	int tot_w_time, i_node_next;
	int delta;
	float distance, cost;

	// Check if vehicle assigned
	if (route.i_vehicle < 0){
		constraint = CAPACITY;
		return false;
	}
	i_vehicle = route.i_vehicle;
	// Initialization
	p_node = route.p_head;	// initial node
	ptr_node = &(route.v_nodes[p_node]);
	i_node = ptr_node->i_node;// index of the node
	ptr_node->a_time = v_nodes[i_node].tw.e_time;
	ptr_node->d_time = v_nodes[i_node].tw.e_time;
	ptr_node->w_time = 0;
	ptr_node->l_time = v_nodes[i_node].tw.l_time;
	for (i = 0; i < nof.dimensions; i++) ptr_node->q[i] = 0;

	// First pass (forward)
	p_node = ptr_node->p_next;
	while (p_node != NIL){
		ptr_node = &(route.v_nodes[p_node]);
		i_node = ptr_node->i_node;// index of the node
		p_pred = ptr_node->p_pred;
		ptr_pred = &(route.v_nodes[p_pred]);
		i_node_pred = ptr_pred->i_node;
		// Cunulative demand
		for (i = 0; i < nof.dimensions; i++){
			ptr_node->q[i] = ptr_pred->q[i] + v_nodes[i_node].demand[i];
			// Check CAPACITY constrint
			if (ptr_node->q[i] > v_vehicles[i_vehicle].v_capacities[i]){
				constraint = CAPACITY;
				return false;
			}
		}
		// Check times
		travel_time = (int)t(i_node_pred, i_node, ptr_pred->d_time, i_vehicle);
		service_time = (int)v_nodes[i_node].s_time[i_vehicle];
		max_w_time = v_nodes[i_node].max_w_time;
		e_time = v_nodes[i_node].tw.e_time;
		l_time = v_nodes[i_node].tw.l_time;
		// 
		ptr_node->l_time = min(ptr_pred->l_time + travel_time + service_time + max_w_time, l_time);
		ptr_node->a_time = ptr_pred->d_time + travel_time;
		ptr_node->w_time = max(e_time - ptr_node->a_time, 0);
		ptr_node->d_time = ptr_node->a_time + ptr_node->w_time + service_time;
		// Check TIME WINDOWS
		if (ptr_node->a_time > l_time){
			constraint = TIMEWINDOW;
			return false;
		}
		// Check WAITING TIMES
		if (ptr_node->l_time < e_time){
			constraint = WAITTIME;
			return false;
		}
		ptr_node->a_time = e_time - max_w_time;
		ptr_node->d_time = e_time + service_time;
		// Next node
		p_node = ptr_node->p_next;
	}

	// Second pass (backward)
	p_node = route.v_nodes[route.p_tail].p_pred;
	tot_w_time = 0;
	while (p_node != NIL){
		ptr_node = &(route.v_nodes[p_node]);
		i_node = ptr_node->i_node;// index of the node
		p_next = ptr_node->p_next;
		ptr_next = &(route.v_nodes[p_next]);
		i_node_next = ptr_next->i_node;
		// Check times
		travel_time = (int)t_b(i_node, i_node_next, i_vehicle, ptr_next->a_time);
		service_time = (int)v_nodes[i_node].s_time[i_vehicle];
		max_w_time = v_nodes[i_node].max_w_time;
		ptr_node->d_time = ptr_next->a_time - travel_time;
		ptr_node->a_time = max(ptr_node->a_time, ptr_node->d_time - max_w_time);
		// Check RIDE TIME
		if (v_nodes[i_node].type == PICKUP){
			// Node corredponsind to the pickup
			p_delivery = ptr_node->p_request;
			ptr_delivery = &(route.v_nodes[p_delivery]);
			i_node_delivery = ptr_delivery->i_node;
			delta = (ptr_delivery->d_time - ptr_node->d_time) - v_nodes[i_node].ride_time;
			if (delta > tot_w_time){
				constraint = RIDETIME;
				return false;
			}
			ptr_node->d_time = ptr_node->d_time + delta;
			ptr_node->a_time = max(ptr_node->a_time, ptr_node->d_time - max_w_time);
			tot_w_time = tot_w_time - delta;
			// Check TIME WINDOWS
			if (ptr_node->d_time > ptr_node->l_time){
				constraint = TIMEWINDOW;
				return false;
			}
		}
		tot_w_time = tot_w_time + (ptr_node->d_time - ptr_node->a_time);
		// pred node
		p_node = ptr_node->p_pred;
	}
	// Initialization
	route.time = 0;
	route.distance = 0;
	route.cost = 0;
	// Third and final pass (forward)
	p_node = route.v_nodes[route.p_head].p_next;
	while (p_node != NIL){
		ptr_node = &(route.v_nodes[p_node]);
		i_node = ptr_node->i_node;// index of the node
		p_pred = ptr_node->p_pred;
		ptr_pred = &(route.v_nodes[p_pred]);
		i_node_pred = ptr_pred->i_node;
		// Check times
		travel_time = (int)t(i_node_pred, i_node, ptr_pred->d_time, i_vehicle);
		distance = d(i_node_pred, i_node, ptr_pred->d_time, i_vehicle);
		cost = c(i_node_pred, i_node, ptr_pred->d_time, i_vehicle);
		service_time = (int)v_nodes[i_node].s_time[i_vehicle];
		e_time = v_nodes[i_node].tw.e_time;
		l_time = v_nodes[i_node].tw.l_time;
		//
		route.time += travel_time;
		route.distance += distance;
		route.cost += cost;
		// 
		ptr_node->a_time = ptr_pred->d_time + travel_time;
		ptr_node->w_time = max(e_time - ptr_node->a_time, 0);
		ptr_node->d_time = max(ptr_node->a_time + ptr_node->w_time + service_time, ptr_node->d_time);
		// Check TIME WINDOWS
		if (ptr_node->d_time > l_time){
			constraint = TIMEWINDOW;
			return false;
		}
		// Check RIDE TIME
		if (v_nodes[i_node].type == DELIVERY)
		{
			// Node corredponsind to the delivery
			p_pickup = ptr_node->p_request;
			ptr_pickup = &(route.v_nodes[p_pickup]);
			i_node_pickup = ptr_pickup->i_node;
			delta = (ptr_node->d_time - ptr_pickup->d_time) - v_nodes[i_node].ride_time;
			if (delta > 0){
				constraint = RIDETIME;
				return false;
			}
		}
		// Next node
		p_node = ptr_node->p_next;
	}
	return true;
}

