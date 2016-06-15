/** \file random.cpp
* --------------------------------------------------------------------------
* Licensed Materials - Property of
* --------------------------------------------------------------------------
*/

#define	IA		  7141	/// random parameter */
#define	IC		 54733  /// random parameter */
#define	IM		259200  /// random parameter */


/**
* @brief Compute a random number in [0,1]
* @return  Random number in [0,1]
*/
float f_random()
{
	static int	seed;

   seed   = seed*IA + IC;
   seed   = seed - seed/IM * IM;
   return (float)( seed + 0.0 ) / (float)IM;
}

/**
* @brief Set the random seed
* @param k seed
*/
void f_seedran(int k)
{
	int			i;

	if (k <= 0)
		return;
	for (i=0; i<k; i++)
		f_random();
}

/**
* @brief Compute a random number in [a,b]
* @param a first number
* @param b second number
* @return  Random number in [a,b]
*/
int f_ranval(int a, int b)
{
	return (int) (a + f_random()*(b-a+1));
}

