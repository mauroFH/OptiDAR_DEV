/** \file errors.cpp
* --------------------------------------------------------------------------
* Licensed Materials - Property of
* --------------------------------------------------------------------------
*/

#include "../Modulo-Main/darh.h"

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
* Fatal error: Prints the message and function name to stderr, ferr, and csv and terminate the program.
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

	// Setup Status
	Status = 2;

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

	text = va_arg(args, char*);

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
	sprintf(DateTime, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d", 1900 + dtime1->tm_year, dtime1->tm_mon + 1, dtime1->tm_mday, dtime1->tm_hour, dtime1->tm_min, dtime1->tm_sec);
	fprintf(error.fcsv, "%d %c %d %c %s %c %s %c %s %c \n",
		error.LogID, SEP, error.SessionID, SEP, DateTime, SEP, text, SEP, format, SEP);
	fflush(error.fcsv);

	// Prima di uscire e' necessario scrivere la Tabella "Solution.CSV"
	writeSolution();

	fclose(error.fcsv);
	
	exit(-1);
}


/**
* Warning: Prints the message and function name to stderr, ferr, and csv and go on with the program.
@param format message string
*/
void CError::warning(const char* format, ...)
{
	char *text;
	char DateTime[50];
	//	char Message[255];
	struct tm *dtime1;
	time_t dtime2;
	static char SEP = CON_CSVFILE_SEPARATOR;

	// Setup Status
	Status = 1;

	if (ferr == NULL)
		fprintf(stderr, "WARNING: ");
	else
		fprintf(ferr, "WARNING: ");

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

	text = va_arg(args, char*);

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
	sprintf(DateTime, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d", 1900 + dtime1->tm_year, dtime1->tm_mon + 1, dtime1->tm_mday, dtime1->tm_hour, dtime1->tm_min, dtime1->tm_sec);
	fprintf(error.fcsv, "%d %c %d %c %s %c %s %c %s %c \n",
		error.LogID, SEP, error.SessionID, SEP, DateTime, SEP, text, SEP, format, SEP);
	fflush(error.fcsv);

}


/**
* Warning: Prints the message and function name to stderr and go on with the program.
@param format message string
*/
void CError::warning_old(const char* format, ...)
{
#ifdef DEBUG
	char *text;
	
	fprintf(ferr, "WARNING: ");
	va_list args;
	va_start(args, format);
	fprintf(ferr, "%s", format);
	text = va_arg(args, char*);
	fprintf(ferr, " %s\n", text);
	va_end(args);

	/*
	flog << "WARNING: ";
	va_list args;
	va_start(args, format);
	flog << format;
	text = va_arg(args, char*);
	flog << " " << text;
	va_end(args);
	flog << endl;
	*/

#endif
}

/**
* Fatal error: Prints the message and function name to stderr and terminates the program.
@param format message string
*/
void CError::warning_opt(const char* format, ...)
{
	char *text;
#ifdef DEBUG
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
/**
* Write output CSV file "Solution"    
*/
void CError::writeSolution(void){

	ofstream fout;
	char buf1[CON_MAXNSTR];

	// Instance number
	snprintf(buf1, sizeof(buf1), "ISTANZA%d", error.SessionID);

	// Open file CSV 
	snprintf(buf, sizeof(buf), "%s//%s_Solution.csv", OUTPUTDIR, buf1);
	fout.open(buf, ios::out);
	if (!fout.is_open())
	{
		snprintf(buf, sizeof(buf), "CError::writeSolution: File cannot be opened %s", buf);
		perror(buf);
                return;
                //error.fatal(buf, __FUNCTION__);
	}
	static char SEP = CON_CSVFILE_SEPARATOR;

	// Removed because the CSV2DB does not manage the header 
	//fout  << "SolutionId" << SEP << "SessionId" << SEP << "SolutionNum" << "\n"  ;
	//(fout).flush();

	// Route ID
	fout << error.SolutionID << SEP;
	fout << error.SessionID << SEP;
	fout << error.SolutionNum << SEP;
	if (error.Status == 0)
		fout << " OK ";
	else if (error.Status == 1)
		fout << " WARNING ";
	else if (error.Status == 2)
		fout << " FAILED ";
	else
		fout << " UNKNOWN ";

	fout.flush();

	// Close file
	fout.close();

}