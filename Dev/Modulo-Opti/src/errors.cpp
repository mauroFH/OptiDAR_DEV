/** \file errors.cpp
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of
 * --------------------------------------------------------------------------
 */

#include "../../Modulo-Main/DARH.h"

/**
* Constructor
*/
CError::CError()
{

};

/**
* Destructor
*/
CError::~CError()
{

};

/**
* Fatal error: Prints the message and function name to stderr and terminates the program.
  @param format message string
*/
void CError::fatal(const char* format, ...)
{
	char *text;
	char DateTime[50];
//	char Message[255];
	struct tm *dtime1;
	time_t dtime2;
	static char SEP = CON_CSVFILE_SEPARATOR;

	if (ferr == NULL)
		fprintf(stderr, "FATAL ERROR: ");
	else
		fprintf(ferr, "FATAL ERROR: ");

	va_list args;
	va_start(args, format);
	//sprintf(Message, "%s", format);

	if (ferr == NULL)
	{
		vfprintf(stderr, format, args);
		fprintf(stderr, "--> function ");
	}
	else
	{
		vfprintf(ferr, format, args);
		fprintf(ferr, "--> function ");
	}

	text=va_arg(args, char*);

	if (ferr == NULL)
		vfprintf(stderr, text, args);
	else
		vfprintf(ferr, text, args);

	va_end(args);

	if (ferr == NULL)
		fprintf(stderr, "\n");
	else
		fprintf(ferr, "\n");

	// Write le CSV-Log file
	time(&dtime2);
	dtime1 = localtime(&dtime2);
	sprintf(DateTime, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d", 1900+dtime1->tm_year, dtime1->tm_mon+1, dtime1->tm_mday, dtime1->tm_hour, dtime1->tm_min, dtime1->tm_sec);
	fprintf(error.fcsv, "%d %c %d %c %s %c %s %c %s %c ",
		error.LogID, SEP, error.SessionID, SEP, DateTime, SEP, text, SEP, format, SEP);
	fflush(error.fcsv);

	exit(-1);
}

/**
* Fatal error: Prints the message and function name to stderr and terminates the program.
@param format message string
*/
void CError::warning(const char* format, ...)
{
#ifdef DEBUG
	char *text;
	flog << "WARNING: ";
	va_list args;
	va_start(args, format);
	flog << format;
	text = va_arg(args, char*);
	flog << " " << text;
	va_end(args);
	flog << endl;
#endif
}
