/** \file constants.h  
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of 
 *---------------------------------------------------------------------------
 */

#ifndef __CONDEFS_H
#define __CONDEFS_H

#define CON_MAXFLOAT	99999		/// Massimo valore float 
#define CON_MAXTIME		 9999		/// Tempo massimo in minuti 
#define CON_MAXINT		99999		/// Massimo numero intero 
#define CON_EPS			0.001       /// smallest number 

#define	CON_CSVFILE_SEPARATOR	';'  /// field separator for TXT/CSV file 
#define CON_BLANK		        ' '  /// Blank separator 
#define	CON_SEPDECIMAL		    '.'  /// Decimal separator 
#define CON_DOPPIOAPICE         '"'
#define CON_PUNTOEVIRGOLA       ';'
#define CON_MENO                '-'

#define	CON_MAXSTRFILENAME	200  /// max. dim. of any string containing a file name including  '\0' 
#define	CON_MAXSTRPROCNAME	100  /// max. dim. of any string containing a function name including  '\0' 
#define	CON_MAXSTRERROR		500  /// max. dim. of any string containing an error description including  '\0' 
#define	CON_MAXSTR		    500  /// max. dim. of a generic string  '\0' 

#define CON_MAXNSTRCODE		50  /// massima dimensione di una stringa contenete un codice 
#define CON_MAXNSTRDENOM	70  /// massima dimensione stringa denominazione 

#define CON_STR_NULL        "NULL"  /// stringa "NULL" per i codici non specificati 
#define CON_NULL                -1  /// "NULL" numerico 

#define	NIL			-1

#endif /* __CONDEFS_H */