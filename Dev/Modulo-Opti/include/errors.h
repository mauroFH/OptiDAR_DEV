/** \file errors.h  
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of 
 *---------------------------------------------------------------------------
 */

#ifndef __ERRDEFS_H
#define __ERRDEFS_H

class CError{

public:
	FILE* ferr = NULL;  /// log-file
	FILE* fcsv = NULL;  /// csv-log-file
	int SessionID;   /// ID Session
	int SolutionID;  /// ID Solution
	int SolutionNum; /// Solution Number
	int LogID;       /// ID log
	int Status = 0;  /// Status: 0=OK; 1=Warning; 2=Fatal_Error
	C_CSV *myCSV;    /// Required for accessing the function "CVS_writeSolution"

	CError();
	~CError();

	void fatal(const char* format, ...);
	void warning(const char* format, ...);
	void warning_old(const char* format, ...);
	void warning_opt(const char* format, ...);
};

#endif /* __ERRDEFS_H */
