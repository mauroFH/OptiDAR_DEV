/** \file solution.cpp
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of
 * --------------------------------------------------------------------------
 */

#include "../../Modulo-Main/DARH.h"

/**
* Constructor
*/
CSolution::CSolution()
{
	int i;
	this->cost = 0;
	this->nof_routes = 0;
	this->nof_requests_routed = 0;
	this->totpriority = 0;
	for (i = 0; i < MAXN_REQUESTS; i++)
	{
		this->routed[i] = false;
		this->fixed[i] = false;
	}
};

/**
* Destructor
*/
CSolution::~CSolution()
{

};

/**
* Allocate a new empty route for the solution of type (i_node_start,i_node_end).
* @param i_node_start node index
* @param i_node_end node index
* @return index to the new route, -1 if route not allocated.
*/
int CSolution::new_route(int i_node_start, int i_node_end)
{
	CRoute *p_route;
	if (nof_routes >= MAXN_SOLUTION_ROUTES)
		return -1;
	// New route
	p_route = &v_routes[nof_routes];
	p_route->add_node(i_node_end); // End node
	p_route->add_node(i_node_start); // Start node
	nof_routes++;
	return (nof_routes - 1);
}

/**
* Constructor
*/
CRoute::CRoute()
{
	int k;
	int p_node;
	this->nof_nodes = 0;
	this->i_vehicle = -1;
	this->cost = 0;
	this->time = 0;
	this->distance = 0;
	p_head = NIL;
	p_tail = NIL;
	p_free = 0;
	p_node = p_free;
	for (k = 1; k < MAXN_ROUTE_NODES; k++)
	{
		v_nodes[p_node].p_next = k;
		p_node = k;
		v_nodes[p_node].p_pred = k-1;
	}
};

/**
* Destructor
*/
CRoute::~CRoute()
{

};

/**
* Constructor
*/
CNode::CNode()
{
	// Note: update also delete_node()
	int k;
	p_next		= NIL;
	p_pred		= NIL;
	p_request	= NIL;
	i_node		= NIL;
	for (k = 0; k < MAXN_DIMENSIONS; k++)
		this->q[k] = 0;
	this->a_time = 0;
	this->d_time = 0;
	this->w_time = 0;
	this->l_time = 0;
	this->s_time = 0;
};

/**
* Destructor
*/
CNode::~CNode()
{

};


/**
* Allocate a new node for the route.
* @return a pointer to a new node of a route, NIL if node not allocated.
*/
int CRoute::new_node(void)
{
	int p_node;
	if (p_free == NIL){
		return NIL;
	}
	p_node = p_free;
	p_free = v_nodes[p_node].p_next;
	v_nodes[p_free].p_pred = NIL;
	v_nodes[p_node].p_next = v_nodes[p_node].p_pred = v_nodes[p_node].p_request = NIL;
	v_nodes[p_node].i_node = NIL;
	return (p_node);
}

/**
* Add a new node for the route, the node is added as fist node
* @return a pointer to the new node of a route, NIL if node not allocated.
*/
int CRoute::add_node(int i_node)
{
	int p_node;
	// Allocate a new node
	p_node = new_node();
	if (p_node == NIL){
		return NIL;
	}
	if (p_head == NIL){
		p_head = p_node;
		p_tail = p_node;
	}
	else{
		v_nodes[p_head].p_pred = p_node;
		v_nodes[p_node].p_next = p_head;
		p_head = p_node;
	}
	v_nodes[p_node].i_node = i_node;
	nof_nodes++;

	return (p_node);
}

/**
* Add a new node for the route, the node is added after the node in input
* @param p_node_in node preceeding the new node
* @param i_node index of the new node
* @return a pointer to the new node of a route, NIL if node not allocated.
*/
int CRoute::add_node_after(int p_node_in, int i_node)
{
	int p_node;
	if (p_node_in == NIL)
		return NIL;
	// Allocate a new node
	p_node = new_node();
	if (p_node == NIL)
		return NIL;
	v_nodes[p_node].p_next					= v_nodes[p_node_in].p_next;
	v_nodes[p_node].p_pred					= p_node_in;
	v_nodes[p_node_in].p_next				= p_node;
	if (!(p_node_in==p_tail))
		v_nodes[v_nodes[p_node].p_next].p_pred = p_node;
	else
		p_tail = p_node;
	v_nodes[p_node].i_node = i_node;
	nof_nodes++;

	return (p_node);
}

/**
* Add a new node for the route, the node is added before the node in input
* @param p_node_in node following the new node
* @param i_node index of the new node
* @return a pointer to the new node of a route, NIL if node not allocated.
*/
int CRoute::add_node_before(int p_node_in, int i_node)
{
	int p_node;
	if (p_node_in == NIL)
		return NIL;
	// Allocate a new node
	p_node = new_node();
	if (p_node == NIL)
		return NIL;
	v_nodes[p_node].p_next		= p_node_in;
	v_nodes[p_node].p_pred		= v_nodes[p_node_in].p_pred;
	v_nodes[p_node_in].p_pred	= p_node;
	if (!(p_node_in == p_head))
		v_nodes[v_nodes[p_node].p_pred].p_next = p_node;
	else
		p_head = p_node;
	v_nodes[p_node].i_node = i_node;
	nof_nodes++;
	return (p_node);
}

/**
* Add a new node for the route, the node is added before the node in input
* @param i_pickup pickup node index
* @param i_delivery delivery node index
* @param p_node_pickup index of the node preeceding the pickup node
* @param p_node_delivery index of the node following the delivery node
* @return a pointer to the new node pickup node of the route, NIL if node not allocated.
*/
int CRoute::add_request(int i_pickup, int i_delivery, int p_node_pickup, int p_node_delivery)
{
	int p_node_p, p_node_d;
	if (p_node_pickup == NIL || p_node_delivery == NIL)
		return NIL;
	// Add pickup node
	p_node_p = add_node_after(p_node_pickup, i_pickup);
	if (p_node_p == NIL) return NIL;
	// Add delivery node
	p_node_d = add_node_before(p_node_delivery, i_delivery);
	if (p_node_d == NIL){
		remove_node(p_node_p);
		return NIL;
	}
	// Update p_request
	v_nodes[p_node_p].p_request = p_node_d;
	v_nodes[p_node_d].p_request = p_node_p;
	return (p_node_pickup);
}

/**
* Add a new node for the route, the node is added before the node in input
* @param p_node_pickup pickup node
*/
void CRoute::remove_request(int p_node_pickup)
{
	int p_node_delivery;
	if (p_node_pickup == NIL) return;
	p_node_delivery = v_nodes[p_node_pickup].p_request;
	// Remove pickup
	remove_node(p_node_pickup);
	// Renove delivery
	remove_node(p_node_delivery);
}

/**
* Delete a node from the route.
* @param p_node pointer to the node
*/
void CRoute::delete_node(int p_node)
{
	int k;
	if (p_node == NIL) return;
	v_nodes[p_node].p_next = p_free;
	v_nodes[p_free].p_pred = p_node;
	v_nodes[p_node].p_pred = NIL;
	v_nodes[p_node].p_request = NIL;
	v_nodes[p_node].i_node = -1;

	v_nodes[p_node].i_node = NIL;
	for (k = 0; k < MAXN_DIMENSIONS; k++)
		v_nodes[p_node].q[k] = 0;
	v_nodes[p_node].a_time = 0;
	v_nodes[p_node].d_time = 0;
	v_nodes[p_node].w_time = 0;
	v_nodes[p_node].l_time = 0;
	v_nodes[p_node].s_time = 0;
	
	p_free = p_node;
}

/**
* Remove a node from the route.
* @param p_node pointer to the node
*/
void CRoute::remove_node(int p_node)
{
	if (p_node == p_head)
	{
		p_head = v_nodes[p_head].p_next;
		v_nodes[p_head].p_pred = NIL;
	}
	else
		if (p_node == p_tail)
		{
			p_tail = v_nodes[p_tail].p_pred;
			v_nodes[p_tail].p_next = NIL;
		}
		else
		{
			v_nodes[v_nodes[p_node].p_pred].p_next = v_nodes[p_node].p_next;
			v_nodes[v_nodes[p_node].p_next].p_pred = v_nodes[p_node].p_pred;
		}
	delete_node(p_node);
	nof_nodes--;
}

/**
* Set the solution depending on the request in input
* @param i_request request index
*/
void CSolution::set_request(int i_request)
{
	bool routed;

	if (i_request < 0) return;
	routed = this->routed[i_request];
	if (routed){
		this->routed[i_request] = false;
		this->nof_requests_routed--;
	}
	else
	{
		this->routed[i_request] = true;
		this->nof_requests_routed++;
	}
}