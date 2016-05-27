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
	int LogID;       /// ID log

	CError();
	~CError();

	void fatal(const char* format, ...);
	void warning(const char* format, ...);
};

#endif /* __ERRDEFS_H */
