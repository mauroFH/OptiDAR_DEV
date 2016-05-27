/** \file sort.h  
 * Sorting utilities include file.
 * Version 1.0.0
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of 
 *---------------------------------------------------------------------------
 */

#ifndef __SORTDEFS_H
#define __SORTDEFS_H

/** Data structure used in class Sorter (for sorting items)
*/
struct QSData
{
	long Pos; /// Posizione originale del valore da ordinare */
	double Val[8]; /// Valori su cui ordinare in modo lessicografico */
	long Ind[4]; /// Indici da usare per il retrivial del dato originale */
};

/** Sort Value
	@param  nval	number of values 
	@param  val		values 
	@param  ind		index of the i-th ordered value
	@param	sense	-1 increasing; +1 decreasing
*/
int SortValues(int nval, int *val, int *ind, int sense);

/**--------------------------------------------------------------------------
//  Class for sorting items
//---------------------------------------------------------------------------
*/
class Sorter
{
private:

	void QSort(QSData *a, int first, int last, int QSNVal, int sense);
	int Split(QSData *a, int first, int last, int QSNVal, int sense);
	int QSCmp(QSData *pi, QSData *pj, int QSNVal, int sense);

public:
	long NLevel;   /// Numero di livelli da considerare nell'ordinamento */
	long NItem;    /// Numero dei item da ordinare */
	QSData *Item;  /// Item da ordinare */


	/// Constructor and Destructorr
	Sorter(void);
	~Sorter(void);

	/// Inizializza Skyline
	int Init(void);

	// Sort item
	int Sort(int sense);

};

#endif /* __SORTDEFS_H */

