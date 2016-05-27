/** \file hsv.h  
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of 
 *---------------------------------------------------------------------------
 */

#ifndef __HSVDEFS_H
#define __HSVDEFS_H

/// indices to structures
struct hsv_algp_str{
	float objf_penalty; ///  Penalty used in the fitness  function */
	float noise; ///  noise value */
};

class CHsv{

public:

	CHsv();
	~CHsv();

	short v_sort_requests[MAXN_REQUESTS + 1]; /*!<  array of sorted requests */

	// Algorithm parameters
	hsv_algp_str hsv_algp;

	// Functions
	void solve(class CDar &dar); // hsv heuristic	 
	void main(class CDar &dar); // main function hsv heuristic	 
	bool greedy_insertion(class CDar &dar, class CSolution &sol, bool noise); // greedy insertion heuristic
	double compute_fitness(class CDar &dar, class CSolution &sol); // compute the fitness function of a given solution
	void greedy_remove(class CDar &dar, class CSolution &sol, int n_drop, bool noise); // greedy remove heuristic
	void lopt_swaps(class CDar &dar, class CSolution &sol); // local optimization based on request swaps
	bool accept_solution(double z_current, double z_best, double accept_t); // check if the current solution must be accepted
	bool compare_solution(class CSolution &sol1, double z1, class CSolution &sol2, double z2); // compare two solutions
	void preproc(class CDar &dar); // data preprocessing
};

#endif /* __HSVDEFS_H */
