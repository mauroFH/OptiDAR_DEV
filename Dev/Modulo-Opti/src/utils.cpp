/** \file utils.cpp
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of
 * --------------------------------------------------------------------------
 */

#include "../../Modulo-Main/DARH.h"

/**
*  Test utils functions.
*/
void DATTest(void)
{
	int		seconds;
	char		str[8];

	// 
	seconds = 54300;
	DATsec2string(seconds, str);
	cout << "seconds " << seconds << " str " << str << endl;

	seconds = 172800;
	DATsec2string(seconds, str);
	cout << "seconds " << seconds << " str " << str << endl;
}

/**
*  Return the time (format mm:hh) corresponding to the scalar number in input (hour can be greater than 24h).
*  @param seconds seconds
*  @param w string (in output)
*  @return the time (format mm:hh) corresponding to the scalar number in input, < 0 if time in input not valid
*/
int DATsec2string(int seconds, char *w)
{
	int	giorno, ore, min, minuti;
	char s1[8], s2[8];
	char st[8];
	//
	char sep[2] = ":";
	//char sep[2] = "�";
	//
	minuti = seconds / 60;
	strcpy(w, "");
	if ((minuti<0) || (minuti > 4320))
		return -1;

	// Calcola il giorno
	giorno = minuti / 1440;
	minuti = minuti - giorno * 1440;
	ore = minuti / 60;
	min = minuti - ore * 60;

	// String
	//itoa(ore, s1, 10);
	sprintf(s1, "%d", ore);
	//itoa(min, s2, 10);
	sprintf(s2, "%d", min);
	// Result
	if (ore<10)
	{
		strcpy(st, "0");
		strcat(st, s1);
		strcat(st, sep);
		strcpy(s1, st);
	}
	else
		strcat(s1, sep);
	if (min<10)
	{
		strcpy(st, "0");
		strcat(st, s2);
		strcpy(s2, st);
	}
	strcat(s1, s2);
	if (giorno == 0)
		strcat(s1, "|A");
	if (giorno == 1)
		strcat(s1, "|B");
	if (giorno == 2)
		strcat(s1, "|C");

	strcpy(w, s1);
	return 0;
}
