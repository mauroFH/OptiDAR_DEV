/** \file dar-io.cpp
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of
 * --------------------------------------------------------------------------
 */


#include "../../Modulo-Main/DARH.h"

/*
@brief Output of the DAR data.
@param Instance instance name
*/
void CDar::out_data(char *Instance)
{
	int i, i_depot;
	ofstream fout;
	//CError error;
	char sep = CON_CSVFILE_SEPARATOR;

	if (!flg_instance_loaded)
		return;
		snprintf(buf,sizeof(buf),"%s//%s_dar_data.csv",OUTPUTDIR,Instance);
	fout.open(buf, ios::out);
	if (!fout.is_open()) error.fatal("File opening", __FUNCTION__);
	// Main data
	fout << " #nodes" << sep << " #depots" << sep << " #requests" << sep << " #vehicles" << sep << " #vehicles type" << sep << " #dimensions" << endl;
	fout << nof.nodes << sep << nof.depots << sep << nof.requests << sep << nof.vehicles << sep << nof.vehicle_types << sep << nof.dimensions << endl;
	// Depots
	fout << "  depot " << sep << " code " << sep << " e(i) " << sep << "l(i)" << endl;
	for (i = 1; i <= nof.depots; i++)
	{
		i_depot = this->ind.first_depot + i - 1;
		fout << i << sep << v_nodes[i_depot].code << sep << v_nodes[i_depot].tw.e_time << sep << v_nodes[i_depot].tw.l_time << endl;
	}
	// Requests
	fout << "  request " << sep << " code " << sep << " e(i) " << sep << "l(i)" << sep << "ride(i)" << endl;
	for (i = 1; i <= nof.requests; i++)
	{
		fout << i << sep << v_nodes[i].code << sep << v_nodes[i].tw.e_time << sep << v_nodes[i].tw.l_time << sep << v_nodes[i].ride_time << endl;
	}
	fout.flush();
}

/*
@brief Output of the solution.
@param Instance instance name
*/
void CDar::out_solution(char *Instance)
{
	ofstream fout;
	//CError error;
	int k;

	if (!flg_instance_loaded)
		return;
		snprintf(buf,sizeof(buf),"%s//%s_dar_solution.csv",OUTPUTDIR,Instance);
	fout.open(buf, ios::out);
	if (!fout.is_open()) {
			snprintf(buf,sizeof(buf),"File opening %s",buf);           
			error.fatal("File opening", __FUNCTION__);
		}

	// Output of each route
	for (k = 0; k < this->solution.nof_routes; k++){
		out_route(this->solution.v_routes[k], fout);
	}
	fout.flush();
}

/*
@brief Output of a route.
@param route route
#param flog output stream
*/
void CDar::out_route_01(CRoute &route, ofstream &flog)
{
	char sep = CON_CSVFILE_SEPARATOR;
	int p_node, i_node, p_pred;
	CNode *ptr_node, *ptr_pred;
	float cdist;
	int ctime, i, j, time, i_vehicle, load_disabled, load_seated, load_standing;
	bool print_aux_data=true;

	p_node = route.p_head;
	cdist = 0;
	ctime = 0;
	p_pred = NIL;
	i_vehicle = route.i_vehicle;
	assert(i_vehicle >= 0);
	//...
	// Type of capacity constrint
	if (v_vehicles[i_vehicle].type_ccons == S)
		flog << "#node" << sep << "code" << sep << "type" << sep << "distance" << sep << "time" << sep << "load" << sep 
			<< "a.time" << sep << "d.time" << sep;
	if ((v_vehicles[i_vehicle].type_ccons == A) || (v_vehicles[i_vehicle].type_ccons == B))
		flog << "#node" << sep << "code" << sep << "type" << sep << "distance" << sep << "time" << sep << "seated" << sep << "standing" << sep << "disabled" << sep 
			<< "a.time" << sep << "d.time" << sep;
	if (print_aux_data)
	{
		flog << "a.time" << sep << "d.time" << sep << "e.time" << sep << "l.time" << sep;
	}
	flog << endl;
	//...
	while (p_node != NIL)
	{
		// Current node
		ptr_node = &(route.v_nodes[p_node]);
		i_node   = ptr_node->i_node;// index of the node

		if (p_pred != NIL)
		{
			ptr_pred = &(route.v_nodes[p_pred]);
			i = ptr_pred->i_node;
			j = ptr_node->i_node;
			time = ptr_pred->d_time;
			cdist = cdist + d(i, j, time, i_vehicle);
			ctime = ctime + (int)t(i, j, time, i_vehicle);
		}

		//...
		i = ptr_node->i_node;
		// Nodes are numbered from 0
		flog << i_node-1 << sep << v_nodes[i_node].code << sep << node_type_str[v_nodes[i].type] << sep << cdist << sep << ctime << sep;
		// Type of capacity constrint
		if (v_vehicles[i_vehicle].type_ccons == S)
			flog << ptr_node->q[0] << sep;
		if (v_vehicles[i_vehicle].type_ccons == A){
			load_seated   = (int)ptr_node->q[0];
			load_standing = 0;
			if (load_seated > v_vehicles[i_vehicle].seated_capacity){
				load_standing	= load_seated - v_vehicles[i_vehicle].seated_capacity;
				load_seated		= v_vehicles[i_vehicle].seated_capacity;
			}
			load_disabled = (int)ptr_node->q[1];
			//
			flog << load_seated << sep << load_standing << sep << load_disabled << sep;
			//
		}
		if (v_vehicles[i_vehicle].type_ccons == B){
			load_seated		= (int)ptr_node->q[0];
			load_standing	= (int)ptr_node->q[1];
			load_disabled	= 0;
			//
			flog << load_seated << sep << load_standing << sep << load_disabled << sep;
			//
		}
		//
		flog << ptr_node->a_time << sep << ptr_node->d_time << sep;
		//
		//
		if (print_aux_data){
			int ei = v_nodes[i_node].tw.e_time;
			int li = v_nodes[i_node].tw.l_time;
			char str[8];
			DATsec2string(ptr_node->a_time, str);  flog << str << sep;
			DATsec2string(ptr_node->d_time, str);  flog << str << sep;
			DATsec2string(ei, str);  flog << str << sep;
			DATsec2string(li, str);  flog << str << sep;
		}
		//
		flog << endl;
		//...

		// Next node
		p_pred = p_node;
		p_node = ptr_node->p_next;
	}
	flog.flush();
}

/*
@brief Output of a route.
@param route route
#param flog output stream
*/
void CDar::out_route(CRoute &route, ofstream &flog)
{
	char sep = CON_CSVFILE_SEPARATOR;
	int p_node, i_node, p_pred;
	CNode *ptr_node, *ptr_pred;
	float cdist;
	int ctime, i, j, time, i_vehicle, load_disabled, load_seated, load_standing;
	bool print_aux_data = true;

	p_node = route.p_head;
	cdist = 0;
	ctime = 0;
	p_pred = NIL;
	i_vehicle = route.i_vehicle;
	assert(i_vehicle >= 0);
	//...
	// Vehicle index
	flog << "vehicle index" << sep << i_vehicle << sep << endl;
	//
	// Type of capacity constrint
	if (v_vehicles[i_vehicle].type_ccons == S)
		flog << "#node" << sep << "code" << sep << "type" << sep << "distance" << sep << "time" << sep << "load" << sep
		<< "a.time" << sep << "d.time" << sep;
	if ((v_vehicles[i_vehicle].type_ccons == A) || (v_vehicles[i_vehicle].type_ccons == B))
		flog << "#node" << sep << "code" << sep << "type" << sep << "distance" << sep << "time" << sep << "seated" << sep << "standing" << sep << "disabled" << sep
		<< "a.time" << sep << "d.time" << sep;
	if (print_aux_data)
	{
		flog << "a.time" << sep << "d.time" << sep << "e.time" << sep << "l.time" << sep;
	}
	flog << endl;
	//...
	while (p_node != NIL)
	{
		// Current node
		ptr_node = &(route.v_nodes[p_node]);
		i_node = ptr_node->i_node;// index of the node

		if (p_pred != NIL)
		{
			ptr_pred = &(route.v_nodes[p_pred]);
			i = ptr_pred->i_node;
			j = ptr_node->i_node;
			time = ptr_pred->d_time;
			cdist = cdist + d(i, j, time, i_vehicle);
			ctime = ctime + (int)t(i, j, time, i_vehicle);
		}

		//...
		i = ptr_node->i_node;
		// Nodes are numbered from 0
		flog << i_node - 1 << sep << v_nodes[i_node].code << sep << node_type_str[v_nodes[i].type] << sep << cdist << sep << ctime << sep;
		// Type of capacity constrint
		if (v_vehicles[i_vehicle].type_ccons == S)
			flog << ptr_node->q[0] << sep;
		if (v_vehicles[i_vehicle].type_ccons == A){
			load_seated = (int)ptr_node->q[0];
			load_standing = 0;
			if (load_seated > v_vehicles[i_vehicle].seated_capacity){
				load_standing = load_seated - v_vehicles[i_vehicle].seated_capacity;
				load_seated = v_vehicles[i_vehicle].seated_capacity;
			}
			load_disabled = (int)ptr_node->q[1];
			//
			flog << load_seated << sep << load_standing << sep << load_disabled << sep;
			//
		}
		if (v_vehicles[i_vehicle].type_ccons == B){
			load_seated = (int)ptr_node->q[0];
			load_standing = (int)ptr_node->q[1];
			load_disabled = 0;
			//
			flog << load_seated << sep << load_standing << sep << load_disabled << sep;
			//
		}
		//
		flog << ptr_node->a_time << sep << ptr_node->d_time << sep;
		//
		//
		if (print_aux_data){
			int ei = v_nodes[i_node].tw.e_time;
			int li = v_nodes[i_node].tw.l_time;
			char str[8];
			DATsec2string(ptr_node->a_time, str);  flog << str << sep;
			DATsec2string(ptr_node->d_time, str);  flog << str << sep;
			DATsec2string(ei, str);  flog << str << sep;
			DATsec2string(li, str);  flog << str << sep;
		}
		//
		flog << endl;
		//...

		// Next node
		p_pred = p_node;
		p_node = ptr_node->p_next;
	}
	flog.flush();
}

/*
 @brief Print DAR data.
 @param Instance instance name
 */
void CDar::print_data(char *Instance)
{
	if (!flg_instance_loaded)
		return;
#ifdef DEBUG
	int i;
	char str[8];
	flog << "DAR data:" << endl;
	flog << " #nodes..........." << right << setw(4) << nof.nodes << endl;
	flog << " #depots.........." << right << setw(4) << nof.depots << endl;
	flog << " #requests........" << right << setw(4) << nof.requests << endl;
	flog << " #vehicles........" << right << setw(4) << nof.vehicles << endl;
	flog << " #vehicles type..." << right << setw(4) << nof.vehicle_types << endl;
	flog << " #dimensions......" << right << setw(4) << nof.dimensions << endl;
	flog << endl;
	flog << "Nodes:" << endl;
	for (i = 1; i <= nof.nodes; i++){
		flog << "  node " << setw(4) << right << i << " type " << setw(10) << right << node_type_str[v_nodes[i].type] <<
			" time-window [" << v_nodes[i].tw.e_time << "," << v_nodes[i].tw.l_time << "]";
		DATsec2string(v_nodes[i].tw.e_time, str);
		flog << " time-window [" << str << ",";
		DATsec2string(v_nodes[i].tw.l_time, str);
		flog << str << "]" << endl;
	}
	flog.flush();
#endif
}

/**
* Read in DARP data.
*/
void CDar::ex_input(void)
{
	//CError error;
	ifstream	 fin;
	int vehicle_capacity, ride_time, work_time;
	int i, i_dummy, ei, li, j,nc, k;
	float service_time, x, y;
	int demand;

	// Open the log file
	fin.open("../examples/a2-16.txt", ios::out);
	if (!fin.is_open()){
		error.fatal("File opening", __FUNCTION__);
		goto END;
	}

	// #vehicle #requests #work.time #veh.cap. #ride.time
	fin >> nof.vehicles >> nof.requests >> work_time >> vehicle_capacity >> ride_time;
	//
	ride_time		 = 1440;
	vehicle_capacity = 50;
	// 

	nof.depots			= 2;
	nof.vehicle_types	= 1;
	nof.dimensions		= 1;
	nof.nodes = 2 * nof.requests + nof.depots;
	if (nof.nodes >= MAXN_NODES){
		error.fatal("nof.nodes >= MAXN_NODES", __FUNCTION__);
		goto END;
	}
	if (nof.requests >= MAXN_REQUESTS){
		error.fatal("nof.requests >= MAXN_REQUESTS", __FUNCTION__);
		goto END;
	}
	ind.first_depot		= 2 * nof.requests + 1;
	ind.last_request		= 2 * nof.requests;
	ind.first_delivery	= nof.requests + 1;
	// #     x       y     si  qi   ei lt
	nc = 0;
	for (i = 1; i <= nof.nodes; i++){
		// Node i
		fin >> i_dummy >> x >> y >> service_time >> demand >> ei >> li;
		//
		ei = 0;
		li = 1440;
		//
		if (demand == 0)
		{ //DEPOT
			j								= 2 * nof.requests + nc + 1;
			v_depots[nc].parking_capacity	= 0;
			v_depots[nc].type				= VEHICLE_DEPOT;
			v_depots[nc].tw.e_time			= ei;
			v_depots[nc].tw.l_time			= li;
			v_depots[nc].latitude			= x;
			v_depots[nc].longitude			= y;
			strcpy(v_depots[nc].code, "");
			nc++;
		}
		else
		{ // REQUESTS
			if (i <= nof.requests)
			{
				j = i - 2;
				strcpy(v_requests[j].code, "");
				v_requests[j].priority		= 0;
				v_requests[j].ride_limit		= ride_time;
				v_requests[j].v_s_times_pickup[0]	= service_time;
				v_requests[j].v_s_times_delivery[0] = service_time;
				v_requests[j].tw.e_time		= ei;
				v_requests[j].tw.l_time		= li;
				v_requests[j].type			= ABLE;
				v_requests[j].demand	[0]		= (float) demand;
				v_requests[j].fixed = false;
				v_requests[j].i_paired_request = NIL;
				for (k = 0; k < nof.vehicles; k++)
					v_requests[j].v_objection_vehicles[k] = 0;
				for (k = 0; k <= nof.requests; k++)
					v_requests[j].v_objection_requests[k] = 0;
			}
			j = i - 1;
		}
		// Nodes
		strcpy(v_nodes[j].code, "");
		v_nodes[j].latitude  = x;
		v_nodes[j].longitude = y;
		v_nodes[j].tw.e_time = ei;
		v_nodes[j].tw.l_time = li;
		v_nodes[j].demand[0] = (float)demand;
		v_nodes[j].ride_time = ride_time;
		v_nodes[j].priority = 0;
		if (demand == 0)	 v_nodes[j].type = VEHICLE_DEPOT;
		if (demand >= 1) v_nodes[j].type = PICKUP;
		if (demand <= -1) v_nodes[j].type = DELIVERY;
		for (k = 0; k < nof.vehicles; k++)
			v_nodes[j].s_time[k] = (int) service_time;
		v_nodes[j].max_w_time = 0;
	}
	// Vehicles
	for (i = 0; i < nof.vehicles; i++){
		strcpy(v_vehicles[i].code, "");
		v_vehicles[i].type				 = 0;
		v_vehicles[i].node_origin		 = 2 * nof.requests + 1;
		v_vehicles[i].node_destination	 = 2 * nof.requests + 2;
		v_vehicles[i].nof_slots_tw		 = 0;
		v_vehicles[i].type				 = 0;
		v_vehicles[i].v_capacities[0]	 = (float) vehicle_capacity;
		v_vehicles[i].seated_capacity	= 0;
		v_vehicles[i].seated_disabled	= 0;
		v_vehicles[i].standing_capacity = 0;
		v_vehicles[i].total_passengers	= 0;
		v_vehicles[i].type_ccons			= S;
		for (j = 1; j <= nof.nodes; j++)
			v_vehicles[i].v_objection_nodes[j] = 0;
	}
	// Vehicle type
	strcpy(v_vehicle_types[0].code, "");
	// Additional data
	cons_max_wtime_stops = CON_MAXFLOAT;
	//
	flg_instance_loaded = true;
END:;
	fin.close();
}

/**
* Read in DARP data.
@param Instance instance name
*/
void CDar::input_01(char *Instance)
{
	//CError error;
	ifstream	 fin;
	int i, i_dummy, ei, li, j, nc, k, type;
	float x, y;
	int demand_able, demand_disabled;
	char code[CON_MAXNSTRCODE];
	float st, rt;
	float distance, time;
	int n_entries;
	int tot_capacity;
	char sep;
	double scalef = 10000;
	int n_origin, n_destination;
		

	// Open the log file
	snprintf(buf,sizeof(buf),"%s//%s_dar_input.csv",INPUTDIR,Instance);
	fin.open(buf, ios::out);
	if (!fin.is_open()){
				snprintf(buf,sizeof(buf),"File opening %s",buf);            
		error.fatal(buf,__FUNCTION__);
		goto END;
	}

	// SECTION 1
	// #requests, #depots
	fin >> nof.requests >> sep >> nof.depots >> sep;

	// Check dimensions
	nof.nodes			= 2 * nof.requests + nof.depots;
	nof.vehicles			= 1;
	nof.vehicle_types	= 1;
	if (!checkdata())
		error.fatal("Maxinum code dimensions reached", __FUNCTION__);

	// Indices
	ind.first_depot		= 2 * nof.requests + 1;
	ind.last_request		= 2 * nof.requests;
	ind.first_delivery	= nof.requests + 1;
	
	// SECTION 2
	// code, 	# seated capacity, # standing capacity, 	# seated capacity(disabled), # total passengers of the vehicle,	earliest time,	latest time, 	origin, 	destination
	fin >> v_vehicles[0].code >> sep >> v_vehicles[0].seated_capacity >> sep >> v_vehicles[0].standing_capacity >> sep >> 
		v_vehicles[0].seated_disabled >> sep >> v_vehicles[0].total_passengers >> sep >>
		v_vehicles[0].v_slots_tw[0].e_time >> sep >> v_vehicles[0].v_slots_tw[0].l_time >> sep >> n_origin >> sep >> 
		n_destination >> sep;
	v_vehicles[0].nof_slots_tw		= 1;
	v_vehicles[0].node_origin		= n_origin + 1;
	v_vehicles[0].node_destination	= n_destination + 1;


	// Define type of capacity constraints
	// case a)
	//			Vehicle [seated_capacity+standing_capacity, seated_disabled]
	//			Customers: ABLE [x,0], DISABLED [0,x], ABLE+DISABLED [x,x] (request is DISABLED)

	// cases b) and c)
	//			Vehicle [seated_capacity, standing_capacity]
	//			Customers: ABLE [x,0], DISABLED [0,x], ABLE+DISABLED [x,x] (request is DISABLED)
	
	//				
	tot_capacity = v_vehicles[0].seated_capacity + v_vehicles[0].standing_capacity + v_vehicles[0].seated_disabled;
	if (tot_capacity == v_vehicles[0].total_passengers)
	{
		nof.dimensions = 2;
		v_vehicles[0].type_ccons = A;
		v_vehicles[0].v_capacities[0] = float(v_vehicles[0].seated_capacity + v_vehicles[0].standing_capacity);
		v_vehicles[0].v_capacities[1] = float(v_vehicles[0].seated_disabled);
	}
	else
	{
		nof.dimensions = 2;
		v_vehicles[0].type_ccons = B;
		v_vehicles[0].v_capacities[0] = (float)v_vehicles[0].seated_capacity;
		v_vehicles[0].v_capacities[1] = (float)v_vehicles[0].standing_capacity;
	}
	for (j = 1; j <= nof.nodes; j++)
		v_vehicles[0].v_objection_nodes[j] = 0;
	v_vehicles[0].type = 0;

	// Vehicle type
	strcpy(v_vehicle_types[0].code, "");

	// SECTION 3
	// node number,	code, coordinate x,	coordinate y, earliest time, latest time,	service time, ride time, demand_able, demand_disabled
	for (i = 1; i <= 2*nof.requests; i++)
	{
		fin >> i_dummy >> sep >> code >> sep >> x >> sep >> y >> sep >> ei >> sep >> li >> sep >> st >> sep >> rt >> sep >> demand_able >> sep >> demand_disabled >> sep;
		// Define nodes
		strcpy(v_nodes[i].code, code);
		v_nodes[i].latitude = y / scalef;
		v_nodes[i].longitude = x / scalef;
		v_nodes[i].max_w_time	= 0;
		v_nodes[i].s_time[0]		= (int)st;
		v_nodes[i].tw.e_time		= ei;
		v_nodes[i].tw.l_time		= li;
		v_nodes[i].ride_time		= (int) rt;
		v_nodes[i].priority = 0;
		if (i <= nof.requests){
			v_nodes[i].type = PICKUP;
			if (demand_able > 0 && demand_disabled==0)
			{
				v_nodes[i].demand[0] = (float)demand_able;	// ABLE: index 0
				v_nodes[i].demand[1] = 0;
			}
			if (demand_able == 0 && demand_disabled > 0)
			{
				v_nodes[i].demand[1] = (float)demand_disabled;	// DISABLE: index 1
				v_nodes[i].demand[0] = 0;
			}
			if (demand_able > 0 && demand_disabled > 0)
			{
				v_nodes[i].demand[0] = (float)demand_able;	// ABLE: index 0
				v_nodes[i].demand[1] = (float)demand_disabled;		// DISABLED: index 1
			}
		}
		else{
			v_nodes[i].type		 = DELIVERY;
			v_nodes[i].demand[0] = -v_nodes[i - nof.requests].demand[0];
			v_nodes[i].demand[1] = -v_nodes[i - nof.requests].demand[1];
			v_nodes[i].priority = 0;
		}
		// v_nodes[i].demand; to be defined during the feasibility
		// Define the request
		if (i <= nof.requests){
			j = i-1;
			strcpy(v_requests[j].code, code);
			v_requests[j].priority		= 0;
			v_requests[j].ride_limit		= (int)rt;
			v_requests[j].v_s_times_pickup[0]  = st;
			v_requests[j].v_s_times_delivery[0] = st;
			v_requests[j].tw.e_time		= ei;
			v_requests[j].tw.l_time		= li;
			v_requests[j].fixed = false;
			v_requests[j].i_paired_request = NIL;
			if (demand_able > 0 && demand_disabled == 0)
			{
				v_requests[j].type		= ABLE;
				v_requests[j].demand[0] = (float)demand_able;
				v_requests[j].demand[1] = 0;
			}
			if (demand_able == 0 && demand_disabled > 0)
			{
				v_requests[j].type = DISABLED;
				v_requests[j].demand[1] = (float)demand_disabled;
				v_requests[j].demand[0] = 0;
			}
			if (demand_able > 0 && demand_disabled > 0)
			{
				v_requests[j].type = DISABLED;
				v_requests[j].demand[0] = (float)demand_able;
				v_requests[j].demand[1] = (float)demand_disabled;
			}
			for (k = 0; k < nof.vehicles; k++)
				v_requests[j].v_objection_vehicles[k] = 0;
			for (k = 0; k <= nof.requests; k++)
				v_requests[j].v_objection_requests[k] = 0;
		}
	}
	// SECTION 4
	// node number,	code, type
	nc = 0;
	for (i = 2 * nof.requests + 1; i <= nof.nodes; i++)
	{
		fin >> i_dummy >> sep >> code >> sep >> x >> sep >> y >> sep >> type >> sep;
		if (type == 0) v_depots[nc].type = VEHICLE_DEPOT;
		if (type == 1) v_depots[nc].type = PARKING_AREA;
		v_depots[nc].parking_capacity = 0;
		v_depots[nc].tw.e_time = 0;
		v_depots[nc].tw.l_time = 0;
		strcpy(v_depots[nc].code, code);
		// Define nodes
		strcpy(v_nodes[i].code, code);
		v_nodes[i].latitude = y / scalef;
		v_nodes[i].longitude		= x/scalef;
		v_nodes[i].max_w_time	= 0;
		v_nodes[i].s_time[0]		= 0;
		v_nodes[i].tw.e_time		= v_vehicles[0].v_slots_tw[0].e_time;
		v_nodes[i].tw.l_time		= v_vehicles[0].v_slots_tw[0].l_time;
		v_nodes[i].ride_time		= 0;
		v_nodes[i].type			= v_depots[nc].type;
		v_nodes[i].priority		= 0;
	}
	// SECTION 5
	n_entries = (2 * nof.requests + nof.depots)*(2 * nof.requests + nof.depots) - (2 * nof.requests + nof.depots);
	for (k = 1; k <= n_entries; k++){
		fin >> i >> sep >> j >> sep >> distance >> sep >> time >> sep;
		m_dist[i+1][j+1][0] = distance;
		m_time[i+1][j+1][0] = time;
	}

	// Additional data
	cons_max_wtime_stops = 15*60;

	// Instance loaded
	flg_instance_loaded = true;
	//
END:;
	fin.close();
}

/**
* Read in DARP data.
@param Instance instance name
*/
void CDar::input(char *Instance)
{
	//CError error;
	ifstream	 fin;
	int i, i_dummy, ei, li, j, nc, k, type;
	float x, y;
	int demand_able, demand_disabled;
	char code[CON_MAXNSTRCODE];
	float st, rt;
	float distance, time;
	int n_entries;
	int tot_capacity;
	char sep;
	double scalef = 10000;
	int n_origin, n_destination;
	int fixed, i_paired_request;

	// Open the log file
	snprintf(buf, sizeof(buf), "%s//%s_dar_input.csv", INPUTDIR, Instance);
	fin.open(buf, ios::out);
	if (!fin.is_open()){
		snprintf(buf, sizeof(buf), "File opening %s", buf);
		error.fatal(buf, __FUNCTION__);
		goto END;
	}

	// SECTION 1
	// #requests, #depots, #vehicles
	fin >> nof.requests >> sep >> nof.depots >> sep >> nof.vehicles >> sep;

	// Check dimensions
	nof.nodes = 2 * nof.requests + nof.depots;
	nof.vehicle_types = 1;
	if (!checkdata())
		error.fatal("Maxinum code dimensions reached", __FUNCTION__);

	// Indices
	ind.first_depot = 2 * nof.requests + 1;
	ind.last_request = 2 * nof.requests;
	ind.first_delivery = nof.requests + 1;

	// SECTION 2
	for (k = 0; k < nof.vehicles; k++){
		// code, 	# seated capacity, # standing capacity, 	# seated capacity(disabled), # total passengers of the vehicle,	earliest time,	latest time, 	origin, 	destination
		fin >> v_vehicles[k].code >> sep >> v_vehicles[k].seated_capacity >> sep >> v_vehicles[k].standing_capacity >> sep >>
			v_vehicles[k].seated_disabled >> sep >> v_vehicles[k].total_passengers >> sep >>
			v_vehicles[k].v_slots_tw[k].e_time >> sep >> v_vehicles[k].v_slots_tw[k].l_time >> sep >> n_origin >> sep >>
			n_destination >> sep;
		v_vehicles[k].nof_slots_tw = 1;
		v_vehicles[k].node_origin = n_origin + 1;
		v_vehicles[k].node_destination = n_destination + 1;


		// Define type of capacity constraints
		// case a)
		//			Vehicle [seated_capacity+standing_capacity, seated_disabled]
		//			Customers: ABLE [x,0], DISABLED [k,x], ABLE+DISABLED [x,x] (request is DISABLED)

		// cases b) and c)
		//			Vehicle [seated_capacity, standing_capacity]
		//			Customers: ABLE [x,0], DISABLED [k,x], ABLE+DISABLED [x,x] (request is DISABLED)

		//				
		tot_capacity = v_vehicles[k].seated_capacity + v_vehicles[k].standing_capacity + v_vehicles[k].seated_disabled;
		if (tot_capacity == v_vehicles[k].total_passengers)
		{
			nof.dimensions = 2;
			v_vehicles[k].type_ccons = A;
			v_vehicles[k].v_capacities[k] = float(v_vehicles[k].seated_capacity + v_vehicles[k].standing_capacity);
			v_vehicles[k].v_capacities[1] = float(v_vehicles[k].seated_disabled);
		}
		else
		{
			nof.dimensions = 2;
			v_vehicles[k].type_ccons = B;
			v_vehicles[k].v_capacities[k] = (float)v_vehicles[k].seated_capacity;
			v_vehicles[k].v_capacities[1] = (float)v_vehicles[k].standing_capacity;
		}
		for (j = 1; j <= nof.nodes; j++)
			v_vehicles[k].v_objection_nodes[j] = 0;
		v_vehicles[k].type = 0;
	}

	// Vehicle type
	strcpy(v_vehicle_types[0].code, "");

	// SECTION 3
	// node number,	code, coordinate x,	coordinate y, earliest time, latest time,	service time, ride time, demand_able, demand_disabled, fixed, paired_request
	for (i = 1; i <= 2 * nof.requests; i++)
	{
		fin >> i_dummy >> sep >> code >> sep >> x >> sep >> y >> sep >> ei >> sep >> li >> sep >> st >> sep >> rt >> sep >> demand_able >> 
			sep >> demand_disabled >> sep >> fixed >> sep >> i_paired_request >> sep;
		//
		i_paired_request++; // request start from 1, input from 0
		// Define nodes
		strcpy(v_nodes[i].code, code);
		v_nodes[i].latitude = y / scalef;
		v_nodes[i].longitude = x / scalef;
		v_nodes[i].max_w_time = 0;
		v_nodes[i].s_time[0] = (int)st;
		v_nodes[i].tw.e_time = ei;
		v_nodes[i].tw.l_time = li;
		v_nodes[i].ride_time = (int)rt;
		v_nodes[i].priority = 0;
		if (i <= nof.requests){
			v_nodes[i].type = PICKUP;
			if (demand_able > 0 && demand_disabled == 0)
			{
				v_nodes[i].demand[0] = (float)demand_able;	// ABLE: index 0
				v_nodes[i].demand[1] = 0;
			}
			if (demand_able == 0 && demand_disabled > 0)
			{
				v_nodes[i].demand[1] = (float)demand_disabled;	// DISABLE: index 1
				v_nodes[i].demand[0] = 0;
			}
			if (demand_able > 0 && demand_disabled > 0)
			{
				v_nodes[i].demand[0] = (float)demand_able;	// ABLE: index 0
				v_nodes[i].demand[1] = (float)demand_disabled;		// DISABLED: index 1
			}
		}
		else{
			v_nodes[i].type = DELIVERY;
			v_nodes[i].demand[0] = -v_nodes[i - nof.requests].demand[0];
			v_nodes[i].demand[1] = -v_nodes[i - nof.requests].demand[1];
			v_nodes[i].priority = 0;
		}
		// v_nodes[i].demand; to be defined during the feasibility
		// Define the request
		if (i <= nof.requests){
			j = i - 1;
			strcpy(v_requests[j].code, code);
			v_requests[j].priority = 0;
			v_requests[j].ride_limit = (int)rt;
			v_requests[j].v_s_times_pickup[0] = st;
			v_requests[j].v_s_times_delivery[0] = st;
			v_requests[j].tw.e_time = ei;
			v_requests[j].tw.l_time = li;
			if (fixed==0)
				v_requests[j].fixed = false;
			else
				v_requests[j].fixed = true;
			v_requests[j].i_paired_request = i_paired_request;
			if (demand_able > 0 && demand_disabled == 0)
			{
				v_requests[j].type = ABLE;
				v_requests[j].demand[0] = (float)demand_able;
				v_requests[j].demand[1] = 0;
			}
			if (demand_able == 0 && demand_disabled > 0)
			{
				v_requests[j].type = DISABLED;
				v_requests[j].demand[1] = (float)demand_disabled;
				v_requests[j].demand[0] = 0;
			}
			if (demand_able > 0 && demand_disabled > 0)
			{
				v_requests[j].type = DISABLED;
				v_requests[j].demand[0] = (float)demand_able;
				v_requests[j].demand[1] = (float)demand_disabled;
			}
			for (k = 0; k < nof.vehicles; k++)
				v_requests[j].v_objection_vehicles[k] = 0;
			for (k = 0; k <= nof.requests; k++)
				v_requests[j].v_objection_requests[k] = 0;
		}
	}
	// SECTION 4
	// node number,	code, type
	nc = 0;
	for (i = 2 * nof.requests + 1; i <= nof.nodes; i++)
	{
		fin >> i_dummy >> sep >> code >> sep >> x >> sep >> y >> sep >> type >> sep;
		if (type == 0) v_depots[nc].type = VEHICLE_DEPOT;
		if (type == 1) v_depots[nc].type = PARKING_AREA;
		v_depots[nc].parking_capacity = 0;
		v_depots[nc].tw.e_time = 0;
		v_depots[nc].tw.l_time = 0;
		strcpy(v_depots[nc].code, code);
		// Define nodes
		strcpy(v_nodes[i].code, code);
		v_nodes[i].latitude		= y / scalef;
		v_nodes[i].longitude		= x / scalef;
		v_nodes[i].max_w_time	= 0;
		v_nodes[i].s_time[0]		= 0;
		v_nodes[i].tw.e_time		= 0;
		v_nodes[i].tw.l_time		= 0;
		v_nodes[i].ride_time		= 0;
		v_nodes[i].type			= v_depots[nc].type;
		v_nodes[i].priority		= 0;
	}
	// SECTION 5
	n_entries = (2 * nof.requests + nof.depots)*(2 * nof.requests + nof.depots) - (2 * nof.requests + nof.depots);
	for (k = 1; k <= n_entries; k++){
		fin >> i >> sep >> j >> sep >> distance >> sep >> time >> sep;
		m_dist[i + 1][j + 1][0] = distance;
		m_time[i + 1][j + 1][0] = time;
	}

	// Additional data
	cons_max_wtime_stops = 15 * 60;

	// Instance loaded
	flg_instance_loaded = true;
	//
END:;
	fin.close();
}

/**
* Print in outout the latex layout of the instance loaded.
* @param route route
* @param Instance instance name
*/
void CDar::print_latex(CRoute &route, char *Instance)
{
	//CError error;
	double	cxmin, cxmax, cymin, cymax, rxo, ryo, ru, ro;
	int		i, inode, i1, i2;
	ofstream		fp;
	int p_node, p_pred;
	CNode *ptr_node, *ptr_pred;

	// Open the data file
		snprintf(buf,sizeof(buf),"%s//%s_dar.tex",OUTPUTDIR,Instance);
	fp.open(buf, ios::out);
	if (!fp.is_open()){
		error.fatal("File opening", __FUNCTION__);
		goto END;
	}
	// Compute the window dimension
	cxmin = CON_MAXFLOAT;	cxmax = -CON_MAXFLOAT;	cymin = CON_MAXFLOAT;	cymax = -CON_MAXFLOAT;
	for (i = 1; i<=nof.nodes; i++)
	{
		if (v_nodes[i].latitude > cxmax) cxmax = v_nodes[i].longitude;
		if (v_nodes[i].latitude < cxmin) cxmin = v_nodes[i].longitude;
		if (v_nodes[i].longitude > cymax) cymax = v_nodes[i].latitude;
		if (v_nodes[i].longitude < cymin) cymin = v_nodes[i].latitude;
	}
	rxo = (cxmax - cxmin) / 20.;	ryo = (cymax - cymin) / 20.;
	cxmin = cxmin - rxo; 	cxmax = cxmax + rxo; 	cymin = cymin - ryo; 	cymax = cymax + ryo;
	rxo = (cxmax - cxmin);
	ryo = (cymax - cymin);
	if (rxo > ryo) ro = rxo;
	else
		ro = ryo;
	ru = 21. / (ro*1.4);
	// ...................................................................
	fp << "\\documentclass[]{amsart}" << endl;
	fp << "\\usepackage{pst-all} %% From PSTricks" << endl;
	fp << "\\usepackage{lscape}" << endl;
	fp << "%% Layout changes" << endl;
	fp << "\\textheight=23cm" << endl;
	fp << "\\topmargin=1cm" << endl;
	fp << "\\textwidth=16cm" << endl;
	fp << "\\oddsidemargin=0cm" << endl;
	fp << "\\evensidemargin=0cm" << endl;
	fp << "\\begin{document}" << endl;
	//fp << "\\begin{landscape}" << endl;
	fp << "%% Problem data" << endl;
	fp << "\\begin{center}" << endl;;
	fp << "{\\scalebox{0.9}{" << endl;
	fp << "\\psset{unit=" << ru << "}" << endl;
	fp << "\\begin{pspicture}(" << cxmin << "," << cymin << ")(" << cxmax << "," << cymax << ")" << endl;
	// .....................................................................
	// Depots
	for (i = ind.first_depot; i <= nof.nodes; i++){
		fp << "%% Depot" << endl;
		fp << "\\psset{ dotstyle = square, dotsize = 7pt 3, fillcolor=red }" << endl;
		inode = i;
		fp << "\\dotnode(" << v_nodes[i].longitude << "," << v_nodes[i].latitude << "){" << inode << "}" << endl;
	}
	// Request
	fp << "\\psset{fillcolor=white}" << endl;
	for (i = 1; i <= ind.last_request; i++){
		inode = i;
		if (i < ind.first_delivery) // pickup
			fp << "\\dotnode[dotstyle=*](" << v_nodes[i].longitude << "," << v_nodes[i].latitude << "){ " << inode << "} \\nput{0}{" << inode << "}{\\tiny{" << inode << "}}" << endl;
		else // delivery
			fp << "\\dotnode[dotstyle=o](" << v_nodes[i].longitude << "," << v_nodes[i].latitude << "){ " << inode << "} \\nput{0}{" << inode << "}{\\tiny{" << inode << "}}" << endl;
	}
	// Plot requests
	for (i = 1; i <= ind.last_request; i++){
		i1 = i;
		i2 = this->nof.requests + i;
		fp << "\\ncline[linewidth=0.5pt,linestyle=dashed]{->}{" << i1 << "}{" << i2 << "}" << endl;
	}
	// Route
	if (route.nof_nodes > 0){
		p_node = route.p_head;
		p_pred = NIL;
		while (p_node != NIL)
		{
			// Current node
			ptr_node = &(route.v_nodes[p_node]);
			if (p_pred != NIL)
			{
				ptr_pred = &(route.v_nodes[p_pred]);
				i1 = ptr_pred->i_node;
				i2 = ptr_node->i_node;
				fp << "\\ncline[linewidth=1.1pt,linestyle=solid,linecolor=red]{->}{" << i1 << "}{" << i2 << "}" << endl;
			}
			// Next node
			p_pred = p_node;
			p_node = ptr_node->p_next;
		}
	}
	
	// .....................................................................
	fp << "\\end{pspicture}}}" << endl;
	fp << "\\end{center}" << endl;
	//fp << "\\end{landscape}" << endl;
	fp << "\\end{document}" << endl;
	// .....................................................................

END:;
}

/**
* Print in outout the latex layout of the instance loaded.
* @param route route
* @param Instance instance name
*/
void CDar::print_latex_shp(CRoute &route, char *Instance)
{
	//CError error;
	double	cxmin, cxmax, cymin, cymax, rxo, ryo;
	int		i, inode, i1, i2;
	ofstream		fp;
	int p_node, p_pred;
	CNode *ptr_node, *ptr_pred;

	// Open the data file
		snprintf(buf,sizeof(buf),"%s//%s_dar_shp.tex",OUTPUTDIR,Instance);        
	fp.open(buf, ios::out);
	if (!fp.is_open()){
		snprintf(buf,sizeof(buf),"File opening %s//%s",OUTPUTDIR,buf);            
		error.fatal(buf, __FUNCTION__);
		goto END;
	}
	// Compute the window dimension
	cxmin = CON_MAXFLOAT;	cxmax = -CON_MAXFLOAT;	cymin = CON_MAXFLOAT;	cymax = -CON_MAXFLOAT;
	for (i = 1; i <= nof.nodes; i++)
	{
		if (v_nodes[i].latitude > cxmax) cxmax = v_nodes[i].longitude;
		if (v_nodes[i].latitude < cxmin) cxmin = v_nodes[i].longitude;
		if (v_nodes[i].longitude > cymax) cymax = v_nodes[i].latitude;
		if (v_nodes[i].longitude < cymin) cymin = v_nodes[i].latitude;
	}
	rxo = (cxmax - cxmin) / 20.;	ryo = (cymax - cymin) / 20.;
	cxmin = cxmin - rxo; 	cxmax = cxmax + rxo; 	cymin = cymin - ryo; 	cymax = cymax + ryo;
	rxo = (cxmax - cxmin);
	ryo = (cymax - cymin);
	// ...................................................................
	fp << "\\documentclass[]{amsart}" << endl;
	fp << "\\usepackage{pst-all} %% From PSTricks" << endl;
	fp << "\\usepackage{lscape}" << endl;
	fp << "%% Layout changes" << endl;
	fp << "\\textheight=23cm" << endl;
	fp << "\\topmargin=1cm" << endl;
	fp << "\\textwidth=16cm" << endl;
	fp << "\\oddsidemargin=0cm" << endl;
	fp << "\\evensidemargin=0cm" << endl;
	fp << "\\begin{document}" << endl;
	//fp << "\\begin{landscape}" << endl;
	fp << "%% Problem data" << endl;
	fp << "\\begin{center}" << endl;;
	fp << "{\\scalebox{30.0}{" << endl;
	//fp << "\\psset{unit=" << ru << "}" << endl;
	fp << "\\begin{pspicture}(" << cxmin << "," << cymin << ")(" << cxmax << "," << cymax << ")" << endl;
	// .....................................................................
	// Depots
	for (i = ind.first_depot; i <= nof.nodes; i++){
		fp << "%% Depot" << endl;
		fp << "\\psset{ dotstyle = square, dotsize = 0.009, fillcolor=red }" << endl;
		inode = i;
		fp << "\\dotnode(" << v_nodes[i].longitude << "," << v_nodes[i].latitude << "){" << inode << "}" << endl;
	}
	// Request
	fp << "\\psset{fillcolor=white}" << endl;
	for (i = 1; i <= ind.last_request; i++){
		inode = i;
		if (i < ind.first_delivery) // pickup
			fp << "\\dotnode[dotstyle=*](" << v_nodes[i].longitude << "," << v_nodes[i].latitude << "){ " << inode << "}" << endl;
		else // delivery
			fp << "\\dotnode[dotstyle=o](" << v_nodes[i].longitude << "," << v_nodes[i].latitude << "){ " << inode << "}" << endl;
	}
	// Plot requests
	for (i = 1; i <= ind.last_request; i++){
		i1 = i;
		i2 = this->nof.requests + i;
		fp << "\\ncline[linewidth=0.005pt,linestyle=dashed]{-}{" << i1 << "}{" << i2 << "}" << endl;
	}
	// Route
	if (route.nof_nodes > 0){
		p_node = route.p_head;
		p_pred = NIL;
		while (p_node != NIL)
		{
			// Current node
			ptr_node = &(route.v_nodes[p_node]);
			if (p_pred != NIL)
			{
				ptr_pred = &(route.v_nodes[p_pred]);
				i1 = ptr_pred->i_node;
				i2 = ptr_node->i_node;
				fp << "\\ncline[linewidth=0.001pt,linestyle=solid,linecolor=red]{-}{" << i1 << "}{" << i2 << "}" << endl;
			}
			// Next node
			p_pred = p_node;
			p_node = ptr_node->p_next;
		}
	}

	// .....................................................................
	fp << "\\end{pspicture}}}" << endl;
	fp << "\\end{center}" << endl;
	//fp << "\\end{landscape}" << endl;
	fp << "\\end{document}" << endl;
	// .....................................................................

END:;
}