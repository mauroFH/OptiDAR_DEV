/** \file sort.cpp 
 * Sorting utilities.
 * Version 1.0.0
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of 
 *---------------------------------------------------------------------------
 */

#include "../../Modulo-Main/DARH.h"

/**----------------------------------------------------------------------------
// Sort values (one-dmensional)
//   nval: number of values
//   val[i]: i-th value
//   ind[i]: index of the i-th ordered value 
//   sense: -1 increasing; +1 decreasing
//-----------------------------------------------------------------------------
*/
int SortValues(int nval, int *val, int *ind, int sense)
{
	int i;
	Sorter Data;

	Data.NLevel = 1;
	Data.NItem = nval;
	Data.Init();

	for (i=0; i<nval; i++)
	{
		Data.Item[i].Pos = i;
		Data.Item[i].Val[0] = val[i];
	}

	Data.Sort(sense);

	for (i=0; i<nval; i++)
	{
		ind[i] = Data.Item[i].Pos;
	}

	return 0;
}


/**----------------------------------------------------------------------------
//  Sorter: Constructor                                    
//-----------------------------------------------------------------------------
*/
Sorter::Sorter(void)
{
	NLevel = 0;
	NItem = 0;;
	Item = NULL;
}


/**----------------------------------------------------------------------------
//  Sorter: Destructor                                    
//-----------------------------------------------------------------------------
*/
Sorter::~Sorter(void)
{
	NLevel = 0;
	NItem = 0;
	if (Item != NULL)
		delete [] Item;
}


/**----------------------------------------------------------------------------
//  Sorter: Initialize
//-----------------------------------------------------------------------------
*/
int Sorter::Init(void)
{
	Item = new QSData [NItem];

	return 0;
}


/**----------------------------------------------------------------------------
//  Sort item (sense: -1 increasing; +1 decreasing)
//-----------------------------------------------------------------------------
*/
int Sorter::Sort(int sense)
{
	QSort(Item,0,NItem-1,NLevel,sense);

	return 0;
}


/**----------------------------------------------------------------------------
//  Quick sort for Double Varibles 
//  (sense: -1 increasing; +1 decreasing)
//-----------------------------------------------------------------------------
*/
void Sorter::QSort(QSData *a, int first, int last, int QSNVal, int sense)
{
	int mid;
  
	if (last>=first+1)
	{
		mid=Split(a,first,last,QSNVal,sense);
		QSort(a,first,mid-1,QSNVal,sense);
		QSort(a,mid+1,last,QSNVal,sense);
	}
}


/**----------------------------------------------------------------------------
//  Quick sort for Double Varibles 
//  Fuction for perfoming the splitting
//-----------------------------------------------------------------------------
*/
int Sorter::Split(QSData *a, int first, int last, int QSNVal, int sense)
{
	int left,right;
	QSData temp;

	left=first+1;
	right=last;
	
	for (;;)
	{
		while ((left<=last)&&(QSCmp(&a[left],&a[first],QSNVal,sense)<0)) 
			left++;
		while ((QSCmp(&a[right],&a[first],QSNVal,sense)>0))
			right--;

		if (left<right)
		{
			temp=a[left];
			a[left]=a[right];
			a[right]=temp;
			left++;
			right--;
		}
		else
		{
			temp=a[first];
			a[first]=a[right];
			a[right]=temp;
			return right;
		}
	}
}


/**----------------------------------------------------------------------------
//  Quick sort for Double Varibles 
//  Fuction for comparing two items
//-----------------------------------------------------------------------------
*/
int Sorter::QSCmp(QSData *pi, QSData *pj, int QSNVal, int sense)
{
	int i;

	for (i=0;i<QSNVal;i++)
	{
		if (pi->Val[i]>pj->Val[i])
			return -sense;
		if (pi->Val[i]<pj->Val[i])
			return sense;
	}
	
	return 0;
}


