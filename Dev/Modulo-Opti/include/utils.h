/** \file utils.h  
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of 
 *---------------------------------------------------------------------------
 */

#ifndef __UTLDEFS_H
#define __UTLDEFS_H

/**
*  Test utils functions.
*/
void DATTest(void);

/**
*  Return the time (format mm:hh) corresponding to the scalar number in input (hour can be greater than 24h).
*  @param seconds seconds
*  @param w string (in output)
*  @return the time (format mm:hh) corresponding to the scalar number in input, < 0 if time in input not valid
*/
int DATsec2string(int seconds, char *w);


#endif /* __UTLDEFS_H */
