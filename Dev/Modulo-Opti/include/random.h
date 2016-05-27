/** \file random.h  
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of 
 *---------------------------------------------------------------------------
 */

#ifndef __RANDEFS_H
#define __RANDEFS_H

float f_random(); // Compute a random number in [0,1]
void f_seedran(int k); // Set the random seed
int f_ranval(int a, int b); // Compute a random number in[a, b]

#endif /* __RANDEFS_H */
