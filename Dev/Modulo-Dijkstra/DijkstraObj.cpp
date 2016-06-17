/**---------------------------------------------------------------------------- 
* file DijkstraObj.cpp
*
* Class Dijkstra Definition: Path and Matrix Distance/Time computation.
*
* Last Update: 16.03.2016
* Version 1.0
*------------------------------------------------------------------------------
* Licensed Materials - Property of
\*---------------------------------------------------------------------------*/

#include "../Modulo-Main/darh.h"

/**----------------------------------------------------------------------------
* Constructor
\*---------------------------------------------------------------------------*/
Dijkstra::Dijkstra()
{
	// Directory and filename containing the digital map
	MapName[0]='\0';  // Name of the file containing the SFS of the map
	CodName[0]='\0';  // Name of the file containing the codes of the arcs
	TabName[0]='\0';  // Name of the file containing the table

	// Flags
	FlagSFS  = 0;  // Flag SFS: 0 = NULL; 1 = allocated 
	FlagSFSF = 0;  // Flag SFS: 0 = NULL; 1 = allocated 
	FlagCode = 0;  // Flag SFS: 0 = NULL; 1 = allocated 
	FlagTab  = 0;  // Flag SFS: 0 = NULL; 1 = allocated 
	FlagDij  = 0;  // Flag SFS: 0 = NULL; 1 = allocated 

	// SFS definition
	nver=narc=0;    // Number of vertices and arcs
	index  = NULL;  // Gives gamma location for each vertex
	jver = NULL;  // Gamma^{+1}: ending vertex of the arc
	iver = NULL;  // Gamma^{-1}: starting vertex of the arc
	darc = NULL;  // Arc distance 
	codarc = NULL;  // Arc code
	type = NULL;    // Arc type

	// Road code definition
	nkey     = 0;     // Number of different road, i.e. code
	tipo     = NULL;  // Road type 
	distanza = NULL;  // Road length
	tempo    = NULL;  // Time to travel the road
	costo    = NULL;  // Cost to pay fro travelling the corresponding road 

	// Auxiliary data structure used to zip SFS
	tab     = NULL;  // Auxiliary table to save the different road types
	tabkey  = NULL;  // Auxiliary table to save the road codes
	ntab    = NULL;  // Auxiliary vector used filling "tab"
	ntabkey = NULL;  // Auxiliary vector used filling "tabkey"

	// Data structure used by Dijkstra
	dist  = NULL;  // Dijkstra label
	ipred = NULL;  // Dijkstra predecessor: vertex
	kpred = NULL;  // Dijkstra predecessor: arc
	next  = NULL;  // Next item in the same hash bucket 
	last  = NULL;  // Previous item in the same hash bucket 
	head  = NULL;  // Hash bucket head (n.1)
	tail  = NULL;  // Hash bucket tail (n.1)
	head2 = NULL;  // Hash bucket head (n.2)
	tail2 = NULL;  // Hash bucket tail (n.2)
	zsink = NULL;  // Zipped flag: if the i-th bit of zsink[j] is 1 then node (zsink[j]*32+j) is a sink
	bit   = NULL;  // Bit masks: bit[i]=2**i

}


/**----------------------------------------------------------------------------
* Destructor
\*---------------------------------------------------------------------------*/
Dijkstra::~Dijkstra()
{
	FlagSFS  = 0;
	FlagSFSF = 0;
	FlagCode = 0;
	FlagTab  = 0;
	FlagDij  = 0;

	DeleteSFS();
	DeleteSFSF();
	DeleteCode();
	DeleteTab();
	DeleteDijkstra();
}


/**----------------------------------------------------------------------------
* Initialize the data structure
* @param nver number of vertices
* @param narc number of vertices
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::Initialize(long nv, long na)
{
	int status;

	// Initialize the number of vertices and arcs
	nver = nv;
	narc = na;

	// Allocate the basic static forward star data structure
	status = AllocateSFS();
	if (status != 0)
		return status;

	// Allocate the extended static forward star data structure
	status = AllocateSFSF();
	if (status != 0)
		return status;

	// Allocate the "cost" static forward star data structure
	status = AllocateCost();
	if (status != 0)
		return status;

	// There is a need for adding more initialization!

	return 0;
}


/**----------------------------------------------------------------------------
* Set the name of the file containing the SFS
* @param *name ptr to the string of the file name
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::SetMapName(char *name)
{
	MapName[0] = '\0';
	strcpy(MapName, name);

	return 0;
}


/**----------------------------------------------------------------------------
* Set the name of the file containing the codes
* @param *name ptr to the string of the file name
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::SetCodName(char *name)
{
	CodName[0]='\0';
	strcpy(CodName,name);

	return 0;
}


/**----------------------------------------------------------------------------
* Set the name of the file containing the table
* @param *name ptr to the string of the file name
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::SetTabName(char *name)
{
	TabName[0]='\0';
	strcpy(TabName,name);

	return 0;
}


/**----------------------------------------------------------------------------
* Allocate SFS definition
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::AllocateSFS()
{
	if (FlagSFS)
		DeleteSFS();

	FlagSFS = 1;

	index = new long [nver+1];
	if (index==NULL) return 1;

	jver = new long[narc];
	if (jver == NULL) return 2;

	iver = new long[narc];
	if (iver == NULL) return 3;

	codarc = new unsigned short[narc];
	if (codarc == NULL) return 4;

	return 0;
}


/**----------------------------------------------------------------------------
* Allocate SFSF definition
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::AllocateSFSF(void)
{
	if (FlagSFSF)
		DeleteSFSF();

	FlagSFSF = 1;

	darc = new long [narc];
	if (darc==NULL) return 1;

	type = new char [narc];
	if (type==NULL) return 2;

	return 0;
}


/**----------------------------------------------------------------------------
* Allocate Road Code
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::AllocateCode(void)
{
	if (FlagCode)
		DeleteCode();

	FlagCode = 1;

	tipo = new char [MaxCod];
	if (tipo==NULL) return 1;

	distanza = new long [MaxCod];
	if (distanza==NULL) return 2;

	tempo = new long [MaxCod];
	if (tempo==NULL) return 3;

	costo = new long [MaxCod];
	if (costo==NULL) return 4;

	return 0;
}


/**----------------------------------------------------------------------------
* Allocate "cost" vectors for the arc
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::AllocateCost(void)
{
	if (FlagCode)
		DeleteCode();

	FlagCost = 1;

	tipo = new char[narc];
	if (tipo == NULL) return 1;

	distanza = new long[narc];
	if (distanza == NULL) return 2;

	tempo = new long[narc];
	if (tempo == NULL) return 3;

	costo = new long[narc];
	if (costo == NULL) return 4;

	return 0;
}


/**----------------------------------------------------------------------------
* Allocate Auxiliary data structure used to zip SFS
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::AllocateTab(void)
{
	long i;

	if (FlagTab)
		DeleteTab();

	FlagTab = 1;

	tab = new long* [MaxVal];
	if (tab==NULL) return 1;
	for (i=0;i<MaxVal;i++)
	{
		tab[i] = new long [MaxType];
		if (tab[i]==NULL) return 2;
	}

	tabkey = new long* [MaxVal];
	if (tabkey==NULL) return 3;
	for (i=0;i<MaxVal;i++)
	{
		tabkey[i] = new long [MaxType];
		if (tabkey[i]==NULL) return 4;
	}

	ntab = new long [MaxType];
	if (ntab==NULL) return 5;

	ntabkey = new long [MaxType];
	if (ntabkey==NULL) return 6;

	return 0;
}


/**----------------------------------------------------------------------------
* Allocate Data structure used by Dijkstra
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::AllocateDijkstra(void)
{
	if (FlagDij)
		DeleteDijkstra();

	FlagDij = 1;

	dist = new long [nver];
	if (dist==NULL) return 1;

	ipred = new long[nver + 1];
	if (ipred == NULL) return 2;

	kpred = new long[nver + 1];
	if (kpred == NULL) return 2;

	next = new long[nver];
	if (next==NULL) return 3;

	last = new long [nver]; 
	if (last==NULL) return 4;

	head = new long [MaxVal+4];
	if (head==NULL) return 5;

	tail = new long [MaxVal+4];
	if (tail==NULL) return 6;

	head2 = new long [MaxVal2+4];
	if (head2==NULL) return 7;

	tail2 = new long [MaxVal2+4];
	if (tail2==NULL) return 8;

	zsink = new long [znverb];
	if (zsink==NULL) return 9;

	bit = new long [lsize];
	if (bit==NULL) return 10;

	return 0;
}


/**----------------------------------------------------------------------------
* Free SFS definition
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::DeleteSFS(void)
{
	FlagSFS = 0;

	if (index) delete [] index;
	if (jver) delete[] jver;
	if (iver) delete[] iver;
	if (codarc) delete[] codarc;

	index   = NULL;
	jver = NULL;
	iver = NULL;
	codarc = NULL;

	return 0;
}


/**----------------------------------------------------------------------------
* Free SFS definition
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::DeleteSFSF(void)
{
	FlagSFSF = 0;

	if (darc) delete [] darc; 
	if (type) delete [] type; 

	darc = NULL;
	type = NULL;

	return 0;
}


/**----------------------------------------------------------------------------
* Free Road Code 
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::DeleteCode(void)
{
	FlagCode = 0;

	// Road code definition
	if (tipo) delete [] tipo;
	if (distanza) delete [] distanza;
	if (tempo) delete [] tempo;
	if (costo) delete [] costo;  

	tipo     = NULL;  
	distanza = NULL;
	tempo    = NULL;
	costo    = NULL;

	return 0;
}


/**----------------------------------------------------------------------------
* Free Auxiliary data structure used to zip SFS 
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::DeleteTab(void)
{
	long i;

	FlagTab = 0;

	// Auxiliary data structure used to zip SFS
	if (tab)
	{
		for (i=0;i<MaxVal;i++)
            delete [] tab[i];

        delete [] tab;
	}

	if (tabkey)
	{
		for (i=0;i<MaxVal;i++)
            delete [] tabkey[i];

        delete [] tabkey;
	}

	if (ntab) delete [] ntab;
	if (ntabkey) delete [] ntabkey;

	tab     = NULL;
	tabkey  = NULL;
	ntab    = NULL;
	ntabkey = NULL;

	return 0;
}


/**----------------------------------------------------------------------------
* Free Data structure used by Dijkstra
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::DeleteDijkstra(void)
{
	FlagDij = 0;

	// Data structure used by Dijkstra
	if (dist) delete [] dist;
	if (ipred) delete[] ipred;
	if (kpred) delete[] kpred;
	if (next) delete[] next;
	if (last) delete [] last;
	if (head) delete [] head;
	if (tail) delete [] tail;
	if (head2) delete [] head2;
	if (tail2) delete [] tail2;
	if (zsink) delete [] zsink;
	if (bit) delete [] bit;

	dist  = NULL;
	ipred = NULL;
	kpred = NULL;
	next = NULL;
	last  = NULL;
	head  = NULL;
	tail  = NULL;
	head2 = NULL;
	tail2 = NULL;
	zsink = NULL;
	bit   = NULL;

	return 0;
}


/**----------------------------------------------------------------------------
* Read the Full Static Forward Star
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::LeggiFull(void)
{
	int ko;
	long i,ok,err;
	FILE *mappa;
	
	err=0;

	// Open the input file
	mappa = fopen(MapName,"rb");
	if (mappa==NULL) 
	{
		err=99;
		goto error2;
	}

	ok=fread(&nver,sizeof(long),1,mappa);
	if (ok<1) 
	{
		err=1;
		goto error;
	}

	ok=fread(&narc,sizeof(long),1,mappa);
	if (ok<1) 
	{
		err=1;
		goto error;
	}

	ko=AllocateSFS();
	if (ko) 
	{
		err=999;
		goto error;
	}

	ko=AllocateSFSF();
	if (ko) 
	{
		err=999;
		goto error;
	}

	ok=fread(index,sizeof(long),nver,mappa);
	if (ok<nver) 
	{
		err=2;
		goto error;
	}

	ok=fread(jver,sizeof(long),narc,mappa);
	if (ok<narc) 
	{
		err=3;
		goto error;
	}

	ok=fread(darc,sizeof(long),narc,mappa);
	if (ok<narc) 
	{
		err=4;
		goto error;
	}

	ok=fread(type,sizeof(char),narc,mappa);
	if (ok<narc) 
	{
		err=5;
		goto error;
	}

	// Close index
    index[nver]=narc+1;

	// Setup the index
	for (i=0;i<=nver;i++)
		index[i]--;

	for (i=0;i<narc;i++)
		jver[i]--;

error:

	// Close the input file
	fclose(mappa);

error2:

	return err;
}


/**----------------------------------------------------------------------------
* Read the Zipped Static Forward Star
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::LeggiZip(void)
{
	int ko;
	long i,ok,err;
	FILE *mappa;

	err=0;

	// Open the input file
	mappa = fopen(MapName,"rb");
	if (mappa==NULL) 
	{
		err=99;
		goto error2;
	}

	ok=fread(&nver,sizeof(long),1,mappa);
	if (ok<1) 
	{
		err=1;
		goto error;
	}

	ok=fread(&narc,sizeof(long),1,mappa);
	if (ok<1) 
	{
		err=1;
		goto error;
	}

	ko=AllocateSFS();
	if (ko) 
	{
		err=999;
		goto error;
	}

	ok=fread(index,sizeof(long),nver,mappa);
	if (ok<nver) 
	{
		err=2;
		goto error;
	}

	ok=fread(jver,sizeof(long),narc,mappa);
	if (ok<narc) 
	{
		err=3;
		goto error;
	}

	// Close index
    index[nver]=narc+1;

	// Setup the index
	for (i=0;i<=nver;i++)
		index[i]--;

	for (i=0;i<narc;i++)
		jver[i]--;

error:

	// Close the input file
	fclose(mappa);

error2:

	return err;
}


/**----------------------------------------------------------------------------
* Read the zip code associated to each arc
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::LeggiCod(void)
{
	int ko;
	long ok,err;
	FILE *mappa;

	err=0;

	// Open the input file
	mappa = fopen(CodName,"rb");
	if (mappa==NULL) 
	{
		err=99;
		goto error2;
	}
	
	ok=fread(&narc,sizeof(long),1,mappa);
	if (ok<1) 
	{
		err=1;
		goto error;
	}

	if (FlagSFS==0)
	{
		ko=AllocateSFS();
		if (ko)
		{
			err=999;
			goto error;
		}
	}

	ok=fread(codarc,sizeof(short),narc,mappa);
	if (ok<narc) 
	{
		err=2;
		goto error;
	}

error:

	// Close the input file
	fclose(mappa);

error2:

	return err;
	
}


/**----------------------------------------------------------------------------
* Read the table where for each zip code is given: distance, time, cost, type
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::LeggiTable(void)
{
	int ko;
	long ok,err;
	FILE *mappa;

	if (FlagCode==0)
	{
		ko=AllocateCode();
		if (ko) 
			return 999;
	}

	err=0;

	// Open the input file
	mappa = fopen(TabName,"rb");
	if (mappa==NULL) 
	{
		err=99;
		goto error2;
	}
	
	ok=fread(&nkey,sizeof(long),1,mappa);
	if (ok<1) 
	{
		err=1;
		goto error;
	}

	ok=fread(distanza,sizeof(long),nkey,mappa);
	if (ok<nkey) 
	{
		err=2;
		goto error;
	}

	ok=fread(tempo,sizeof(long),nkey,mappa);
	if (ok<nkey) 
	{
		err=3;
		goto error;
	}

	ok=fread(costo,sizeof(long),nkey,mappa);
	if (ok<nkey) 
	{
		err=4;
		goto error;
	}

	ok=fread(tipo,sizeof(char),nkey,mappa);
	if (ok<nkey) 
	{
		err=5;
		goto error;
	}

error:

	// Close the input file
	fclose(mappa);

error2:

	return err;
	
}


/**----------------------------------------------------------------------------
* Write the zip code associated to each arc
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::ScriviCod(void)
{
	long ok,err;
	FILE *mappa;

	if (FlagSFS==0)
		return 99;

	err=0;

	// Open the input file
	mappa = fopen(CodName,"wb");
	if (mappa==NULL) 
	{
		err=99;
		goto error2;
	}
	
	ok=fwrite(&narc,sizeof(long),1,mappa);
	if (ok<1) 
	{
		err=1;
		goto error;
	}

	ok=fwrite(codarc,sizeof(short),narc,mappa);
	if (ok<narc) 
	{
		err=2;
		goto error;
	}

error:

	// Close the input file
	fclose(mappa);

error2:

	return err;
	
}


/**----------------------------------------------------------------------------
* Write the table where for each zip code is given: distance, time, cost, type
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::ScriviTable(void)
{
	long ok,err;
	FILE *mappa;

	if (FlagCode==0)
		return 99;

	err=0;

	// Open the input file
	mappa = fopen(TabName,"wb");
	if (mappa==NULL) 
	{
		err=99;
		goto error2;
	}
	
	ok=fwrite(&nkey,sizeof(long),1,mappa);
	if (ok<1) 
	{
		err=1;
		goto error;
	}

	ok=fwrite(distanza,sizeof(long),nkey,mappa);
	if (ok<nkey) 
	{
		err=2;
		goto error;
	}

	ok=fwrite(tempo,sizeof(long),nkey,mappa);
	if (ok<nkey) 
	{
		err=3;
		goto error;
	}

	ok=fwrite(costo,sizeof(long),nkey,mappa);
	if (ok<nkey) 
	{
		err=4;
		goto error;
	}

	ok=fwrite(tipo,sizeof(char),nkey,mappa);
	if (ok<nkey) 
	{
		err=5;
		goto error;
	}

error:

	// Close the input file
	fclose(mappa);

error2:

	return err;
	
}


/**----------------------------------------------------------------------------
* Return the number of vertices
* @return the function returns the number of vertices, if successful, or -1, 
* otherwise.
\----------------------------------------------------------------------------*/
long Dijkstra::NVer(void)
{
	if (FlagSFS==0)
		return -1;

	return nver;
}


/**----------------------------------------------------------------------------
* Return the number of arcs
* @return the function returns the number of arcs, if successful, or -1, 
* otherwise.
\----------------------------------------------------------------------------*/
long Dijkstra::NArc(void)
{
	if (FlagSFS==0)
		return -1;

	return narc;
}


/**----------------------------------------------------------------------------
* Return the time associated to arc k
* @param k index of the arc
* @return the function returns the time associated to arc k, if successful, 
* or -1, otherwise.
\----------------------------------------------------------------------------*/
long Dijkstra::ArcTime(long k)
{
	long kcode;

	if (FlagCode==0)
		return -1;

	if (k>narc)
		return -2;

	kcode = codarc[k-1];

	return tempo[kcode];
}


/**----------------------------------------------------------------------------
* Return the distance associated to arc k
* @param k index of the arc
* @return the function returns the distance associated to arc k, 
* if successful, or -1, otherwise.
\----------------------------------------------------------------------------*/
long Dijkstra::ArcDistance(long k)
{
	long kcode;

	if (FlagCode==0)
		return -1;

	if (k>narc)
		return -2;

	kcode = codarc[k-1];

	return distanza[kcode];
}


/**----------------------------------------------------------------------------
* Return the cost associated to arc k
* @param k index of the arc
* @return the function returns the cost associated to arc k, if successful, 
* or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
long Dijkstra::ArcCost(long k)
{
	long kcode;

	if (FlagCode==0)
		return -1;

	if (k>narc)
		return -2;

	kcode = codarc[k-1];

	return costo[kcode];
}


/**----------------------------------------------------------------------------
* Return the index of arc (i,j). 
* This function considers the general case where there exists more than one 
* arc (i,j) (i.e. parallel arcs). 
* The returned index corresponds to the least cost arc (i,j).
* @param i starting vertex of the arc
* @param j ending vertex of the arc
* @return the function returns the index of the corresponding arc, 
* if successful, or -1, otherwise.
\----------------------------------------------------------------------------*/
long Dijkstra::ArcIndex(long i, long j)
{
	long k,jj;
	long ipos1,ipos2;
	long costb;
	long kcode,kcb;

	if ((FlagSFS==0)||(FlagCode==0))
		return -2;

	if ((i>nver)||(j>nver))
		return -3;

	i--;
	j--;

	ipos1 = index[i];
	ipos2 = index[i+1] - 1;

	costb=Infl;
	kcb=-1;
	for (k=ipos1;k<=ipos2;k++)
	{
		jj = jver[k];
		if (jj==j)
		{
			kcode = codarc[k];
			if (costo[kcode]<costb)
			{
				kcb=k;
				costb=costo[kcode];
			}
		}
	}

	return kcb+1;

}


/**----------------------------------------------------------------------------
* Return the set Gamma corresponding of arc i. 
* @param i index of the vertex
* @param ng cardinality of set Gamma^{+1}
* @param *vertex list of the vertices in Gamma^{+1}
* @param *arc list of the indices of the corresponding arcs
* @param *cost list of the costs of the corresponding arcs
* @param *dist list of the distances of the corresponding arcs
* @param *time list of the travel times of the corresponding arcs
* @return the function returns the cardinality of Gamma^{+1}, if successful, 
* or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
long Dijkstra::Gamma(long i, long ng, long *vertex, long *arc, long *cost, long *dist, long *time)
{
	long j,k;
	long ipos1,ipos2;
	long kcode;

	if ((FlagSFS==0)||(FlagCode==0))
		return -1;

	if (i>nver)
		return -2;

	i--;

	ng = 0;
	ipos1 = index[i];
	ipos2 = index[i+1] - 1;

	for (k=ipos1;k<=ipos2;k++)
	{
		j = jver[k];
		kcode = codarc[k];

		if (vertex) vertex[ng] = j+1;
		if (arc) arc[ng] = k+1;
		if (cost) cost[ng] = costo[kcode];
		if (dist) dist[ng] = distanza[kcode];
		if (time) time[ng] = tempo[kcode];

		ng++;
	}

	return ng;

}


/**----------------------------------------------------------------------------
* Zip the Static Forward Star
* @param TipoPer 0=shortest; 100=fastest; ]0,100[: a convex combination
* @param nVel: Number of different road types
* @param *Vel: Array containing the speed for each road type
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::Zip(int TipoPer, int nVel, int *Vel)
{
	int ok;

	// Read the SFS
	ok=LeggiFull();
	if (ok) return ok;

	// Zip the data
	ok=Elabora();
	if (ok) return ok;

	// Update the table containing the data for each arc
	ok=UpdateTable(TipoPer,nVel,Vel);
	if (ok) return ok;

	// Write the zipped file
	ok=ScriviCod();
	if (ok) return ok;

	// Write the table file
	ok=ScriviTable();
	if (ok) return ok;

	return 0;
}


/**----------------------------------------------------------------------------
* Read Zipped Static Forward Star
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::ReadZip(void)
{
	int ok;

	// Read the SFS
	ok=LeggiZip();
	if (ok) return ok;

	// Write the zipped file
	ok=LeggiCod();
	if (ok) return ok;

	// Write the table file
	ok=LeggiTable();
	if (ok) return ok;

	return 0;
}


/**----------------------------------------------------------------------------
* Update the table required to zip the Static Forward Star, where Peso is 
* computed considering the parameters.
* @param TipoPer 0=shortest; 100=fastest; ]0,100[: a convex combination
* @param nVel: Number of different road types
* @param *Vel: Array containing the speed for each road type
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int  Dijkstra::UpdateVel(int TipoPer, int nVel, int *Vel)
{
	int ok;

	if ((FlagSFS==0)||(FlagCode==0))
		return 99;

	// Update the table containing the data for each arc
	ok=UpdateTable(TipoPer,nVel,Vel);
	if (ok) return ok;

	// Write the zipped file
	ok=ScriviCod();
	if (ok) return ok;

	// Write the table file
	ok=ScriviTable();
	if (ok) return ok;

	return 0;
}


/**----------------------------------------------------------------------------
* Compute the table required to zip the Static Forward Star.
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::Elabora(void)
{
	long i,j,k;

	long kd,kty;
	int ko1;
	char ko;

	if ((FlagSFS==0)||(FlagSFSF==0))
		return 99;

	ko1=AllocateTab();
	if (ko1) 
		return 99;

	ko1=AllocateCode();
	if (ko1) 
		return 99;

	// Initialize the table
	for (j=0;j<MaxType;j++)
	{
		ntab[j]=0;
		for (i=0;i<MaxVal;i++)
            tab[i][j]=0;
	}

	// Build the first table
	for (k=0;k<narc;k++)
	{
		// To avoid distance less or equal to 0
		kd=darc[k];
		if(kd<=0)
		{
			kd=1;
			darc[k]=1;
		}

		// To avoid wrong road type
		kty=type[k];
		if(kty<=0)
		{
			kty=6;
			type[k]=6;
		}

		if (kd<MaxCod) 
			tab[kd][kty]++;
		else
		{
			ko=0;
			for (i=MaxCod;i<MaxCod+ntab[kty];i++)
				if (tab[i][kty]==kd) 
				{
					ko=1;
					break;
				}

			if (ko==0)
			{
				tab[MaxCod+ntab[kty]][kty]=kd;
				ntab[kty]++;
				if (ntab[kty]>=(MaxVal-MaxCod))
					return 1;
			}
		} 
	} 

	// Revise the table
	nkey=0;
	for (j=0;j<MaxType;j++)
	{
		for (i=0;i<MaxCod;i++)
			if (tab[i][j]>0)
			{
				tipo[nkey]=(char)j;
				distanza[nkey]=i;
				tempo[nkey]=0;
				costo[nkey]=0;
				tabkey[i][j]=nkey;
				nkey++;
			}

		for (i=MaxCod;i<MaxCod+ntab[j];i++)
		{
			tipo[nkey]=(char)j;
			distanza[nkey]=tab[i][j];
			tempo[nkey]=0;
			costo[nkey]=0;
			tabkey[i][j]=nkey;
			nkey++;
		}

		ntabkey[j]=nkey;
	}

	// Replace darc(k) with the key
	for (k=0;k<narc;k++)
	{
		kd=darc[k];
		kty=type[k];
		if (kd<MaxCod)
		{
			codarc[k]=(unsigned short)tabkey[kd][kty];
			if (codarc[k]<0)
				printf("errr...%ld\n",k);
		}	
		else
			for (i=MaxCod;i<MaxCod+ntab[kty];i++)
				if (tab[i][kty]==kd)
				{
					codarc[k]=(unsigned short)tabkey[i][kty];
					if (codarc[k]<0)
						printf("errr...%ld\n",k);
					break;
				}
	}

	DeleteSFSF();
	DeleteTab();

	return 0;

}


/**----------------------------------------------------------------------------
* Update the table required to zip the Static Forward Star, where Peso is 
* computed considering the parameters.
* @param TipoPer 0=shortest; 100=fastest; ]0,100[: a convex combination
* @param nVel: Number of different road types
* @param *Vel: Array containing the speed for each road type
* @return the function returns zero, if successful, or a non-zero, otherwise.
\----------------------------------------------------------------------------*/
int Dijkstra::UpdateTable(int TipoPer, int nVel, int *Vel)
{
	long time;
	long k;

	if (FlagCode==0)
		return 99;

	// For each entry compute tempo and costo
	for (k=0;k<nkey;k++)
	{
		if ((int)tipo[k]>nVel)
			return 1;

		// Compute the time required to travel the arc
		tempo[k] = long( (((float)distanza[k]*3.6) / (float)Vel[(int)tipo[k]]) + 0.5 ) ;

		// Compute the time (comparable with distance) required to travel the arc,
		// where an average speed of AvgVel is assumed
		time = long( (((float)distanza[k]*AvgVel) / (float)Vel[(int)tipo[k]]) + 0.5 ) ;
		// time=tempo[k];

		// Cost of the arc of code k
		costo[k] = long( ((distanza[k] * (100.-TipoPer) + time * TipoPer) / 100.) );
		if (costo[k]<=0)
			costo[k]=1;

	}

	return 0;

}


/**----------------------------------------------------------------------------
* Compute Arc Cost.
* @param TipoPer 0=shortest; 100=fastest; ]0,100[: a convex combination
* @param distance: length of the arc
* @param time: travel time of the arc
* @return the function returns the arc cost.
\----------------------------------------------------------------------------*/
long Dijkstra::DefineCost(int TipoPer, double AvgSpeed, long distance, long time)
{
	long costo;

	// Cost of the arc of code k
	costo = long(((distance * (100. - TipoPer) + time * AvgSpeed * TipoPer) / 100.));
	if (costo <= 0)
		costo = 0;

	return costo;

}


//-----------------------------------------------------------------------------
//  Compute the Shortest Paths from "root" to a "sink" using 
//  Dijkstra with a Double Hash and a Zipped Static Forward Star
//-----------------------------------------------------------------------------
/**----------------------------------------------------------------------------
* Compute the Shortest Paths from "root" to a "sink" using Dijkstra with 
* a Double Hash and a Zipped Static Forward Star
* @param root origin of the path
* @param sink destination of the path
* @param *length travel distance on the computed path
* @param *time travel time on the computed path
* @param **pred predecessor used to reconstruct the computed path
* @return the function returns the number of sinks not reached by the path.
\----------------------------------------------------------------------------*/
long Dijkstra::ComputePath(long root, long sink, long *length, long *time, long **pred)
{
	return ComputePath(root,1,&sink,length,time,pred);
}


/**----------------------------------------------------------------------------
* Compute the Shortest Paths from "root" to a list of locations "sink" using 
* Dijkstra with a Double Hash and a Zipped Static Forward Star
* @param root origin of the path
* @param ns number of destinations
* @param *sink list of destinations
* @param *length list of the travel distances for each computed path
* @param *time list of the travel times for each computed path
* @param **pred predecessor used to reconstruct the computed paths
* @return the function returns the number of sinks not reached by the path.
\----------------------------------------------------------------------------*/
long Dijkstra::ComputePath(long root, long ns, long *sink, long *length, long *time, long **pred)
{
	int ko;
	long i,j,h,k;
	long zns;
	long inew;
	long pt,pt2;
	long maxd,maxd2;
	long large;
	long ipos1,ipos2;
	long kcode,kcost,karcost;
	long jdist;
	long qq;
	long jj,ll,ls,lx;
	long ipt,jnext;
	long val;
	long km,sec;
	long kcb,costb;
	char hash;

	root--;

	if ((FlagSFS==0)||(FlagCode==0))
		return ns;

	if (FlagDij==0)
	{
		ko=AllocateDijkstra();
		if (ko) 
			return ns;
	}

	// Initialize some variables
	maxd=MaxVal;
	maxd2=MaxVal2;
	large=Infl;

	// Initialize the zipped sink vector
	for (i=0;i<=nver/lsize;i++)
		zsink[i]=0;

	val=1;
	bit[0]=1;
	for (i=1;i<lsize;i++)
	{
		val*=2;
		bit[i]=val;
	}
	
	zns=ns;
	for (i=0;i<ns;i++)
	{
		k=(sink[i]-1)/lsize;
		j=(sink[i]-1)%lsize;
		zsink[k]=zsink[k]|bit[j];
	}

	// Initialize the hash n.1 empty
	for (k=0;k<=maxd;k++)
	{
		head[k]=-1;
		tail[k]=-1;
	}

	// Initialize the hash n.2 empty
	for (k=0;k<=maxd2;k++)
	{
		head2[k]=-1;
		tail2[k]=-1;
	}

	// Initialize the graph unexplored
	for (i=0;i<nver;i++)
	{
		dist[i]=large;
		ipred[i]=-1;
	}

	// Begin from "root": label "root" and put "root" in the hash 
	last[root] = -1;
	dist[root] = 0;
	next[root] = -1;
	head[0] = root;
	tail[0] = root;
	inew = root;  // inew is the next vertex to get a permanent label and to be scanned
	val  = zns;

	pt  = 0;
	pt2 = 0;

	// Loop where nodes are expanded as soon as "inew" is equal to the last "sink" remained
	while (1) 
	{
		// Check if a "sink" is reached
		k=inew/lsize;
		j=inew%lsize;
		if (zsink[k]&bit[j])
		{
			zns--;
			if (zns==0)
				break;
		}

		// If the new permanent node "inew" is not the last "sink" then "inew" must be expanded 
		ipos1 = index[inew];
		ipos2 = index[inew+1] - 1;

		// Now, we are going to scan all the arcs between the above two markers
		for (k=ipos1;k<=ipos2;k++)
		{
			kcode = codarc[k];
			karcost = costo[kcode];
			kcost = karcost + dist[inew];
			j     = jver[k];
			jdist = dist[j];

			// Where is j? In hash n.1 or n.2?
			if (jdist<0)  
			{
				// j is in hash n.2
				hash=1;
				jdist=-jdist;
			}
			else if (jdist<large)
			{
				// j is in hash n.1
				hash=0;
			}
			else
			{
				// j is neither in hash n.1 nor in hash n.2
				hash=-1;
			}
			
			// Is the current label of j improved?
			if (kcost>=jdist) 
				continue;  // It is not improved then continue to the next arc

			// The label is improved then j needs to be relabelled
			if (hash==0) // j is the hash n. 1
			{
				// j was already labelled and must therefore be in some sublist
				qq = jdist % maxd;

				// j was in the sublist of qq. Now, we must remove it from there.
				lx = next[j];
				ls = last[j];
				if (lx==-1) 
				{
					if (ls==-1) 
					{
						// j in the top = bottom
						head[qq] = -1;
						tail[qq] = -1;
					}
					else
					{
						// j in the bottom
						tail[qq] = ls;
						next[ls] = -1;
					}
				}
				else
				{
					if (ls==-1)
					{
						// j in the top
						head[qq] = lx;
						last[lx] = -1;
					}
					else
					{
						// j in the middle of the list
						last[lx] = ls;
						next[ls] = lx;
					}
				}
			}
			else if (hash==1)  // j is the hash n. 2
			{
				// j was already labelled and must therefore be in some sublist
				qq = jdist % maxd2;

				// j was in the sublist of qq. Now, we must remove it from there.
				lx = next[j];
				ls = last[j];
				if (lx==-1) 
				{
					if (ls==-1) 
					{
						// j in the top = bottom
						head2[qq] = -1;
						tail2[qq] = -1;
					}
					else
					{
						// j in the bottom
						tail2[qq] = ls;
						next[ls] = -1;
					}
				}
				else
				{
					if (ls==-1)
					{
						// j in the top
						head2[qq] = lx;
						last[lx] = -1;
					}
					else
					{
						// j in the middle of the list
						last[lx] = ls;
						next[ls] = lx;
					}
				}
			}

			// j has been removed from sublist qq. Now, we must put j into its new sublist
			if (karcost<maxd) // j must be put in hash n. 1
			{
				qq = kcost % maxd;

				if (head[qq]==-1)
				{
					// the sublist qq is empty
					head[qq] = j;
					last[j] = -1;
				}
				else
				{
					// some vertex is already in sublist qq
					ll = tail[qq];
					last[j]  = ll;
					next[ll] = j;
				}

				ipred[j] = inew;
				dist[j]  = kcost;
				tail[qq] = j;
				next[j]  = -1;
			}
			else
			{
				qq = kcost % maxd2;

				if (head2[qq]==-1)
				{
					// the sublist qq is empty
					head2[qq] = j;
					last[j] = -1;
				}
				else
				{
					// some vertex is already in sublist qq
					ll = tail2[qq];
					last[j]  = ll;
					next[ll] = j;
				}

				ipred[j]  = inew;
				dist[j]   = -kcost;
				tail2[qq] = j;
				next[j]   = -1;
			}
		}

		// Now, we select a new vertex to make label permanent
		if (next[inew]>=0)
		{
			inew = next[inew];
			continue;
		}

		// sublist pt containing inew is empty, therefore we find
		// the first vertex in the next non empty sublist
		head[pt]=-1;
		inew=-1;
		for (ipt=pt+1;ipt<=maxd;ipt++)
		{
			if (head[ipt]>=0) 
			{
				pt = ipt;
				inew = head[pt];
				break;
			}
		}

		// A no-empty list is found
		if (inew>=0)
			continue;

		// A round is complete without having found an empty list
round2:
		pt2++;
		if (pt2>maxd2) // Hash n.2 exhausted
			break;

		if (head2[pt2]>=0)
		{
			j=head2[pt2];

			while (j>=0)
			{
				jnext = next[j];
				kcost = -dist[j];

				qq = kcost % maxd;

				if (head[qq]==-1)
				{
					// the sublist qq is empty
					head[qq] = j;
					last[j] = -1;
				}
				else
				{
					// some vertex is already in sublist qq
					ll = tail[qq];
					last[j]  = ll;
					next[ll] = j;
				}

				// ipred[j] doesn't change
				dist[j]  = kcost;
				tail[qq] = j;
				next[j]  = -1;

				// next vertex to put in hash n.1
				j = jnext;
			}
		}

		// Try to find a no-empty list 
		for (pt=0;pt<=maxd;pt++)
		{
			if (head[pt]>=0) 
			{
				inew = head[pt];
				break;
			}
		}

		// If no more non-empty lists can be found then
		// the unlabelled vertices can not be reached.
		// The "sink" cannot be reached!!!
		if (inew<0)
			goto round2;

	}

	if (inew<0)
		return zns;  // The unreached sinks

	if ((length)||(time))
	{
		for (h=0;h<ns;h++)
		{
			km=0;
			sec=0;
			j=(sink[h]-1);
			while (ipred[j]>0)
			{
				i=ipred[j];

				ipos1 = index[i];
				ipos2 = index[i+1] - 1;

				// We are now going to scan the arcs between the above two markers
				costb=large;
				kcb=-1;
				for (k=ipos1;k<=ipos2;k++)
				{
					jj = jver[k];
					if (jj==j)
					{
						kcode = codarc[k];
						if (costo[kcode]<costb)
						{
							kcb=kcode;
							costb=costo[kcode];
						}
					}
				}

				if (kcb<0)
					return ns;

				km += distanza[kcb];
				sec += tempo[kcb];

				j=i;
			}

			// If length is required then update
			if (length)
				length[h]=km;

			// If time is required then update
			if (time)
				time[h]=sec;
		}
	}
		
	if (pred!=NULL)
	{
		for (i = nver; i >= 1; i--)
			ipred[i] = ipred[i - 1] + 1;

		*pred=ipred;
	}

	return zns;
}


/**----------------------------------------------------------------------------
* Compute the Shortest Paths from "root" to a list of locations "sink" using
* Dijkstra with a Double Hash and a Zipped Static Forward Star
* @param root origin of the path
* @param ns number of destinations
* @param *sink list of destinations
* @param *length list of the travel distances for each computed path
* @param *time list of the travel times for each computed path
* @param **predi predecessor used to reconstruct the computed paths
* @param **predk arc used in the computed paths
* @return the function returns the number of sinks not reached by the path.
\----------------------------------------------------------------------------*/
long Dijkstra::ComputePaths(long root, long ns, long *sink, long *length, long *time, long *predi, long *predk)
{
	int ko;
	long i, j, h, k;
	long zns;
	long inew;
	long pt, pt2;
	long maxd, maxd2;
	long large;
	long ipos1, ipos2;
	long kcode, kcost, karcost;
	long jdist;
	long qq;
	long jj, ll, ls, lx;
	long ipt, jnext;
	long val;
	long km, sec;
	long kcb, costb;
	char hash;

	if ((FlagSFS == 0) || (FlagCost == 0))
		return ns;

	if (FlagDij == 0)
	{
		ko = AllocateDijkstra();
		if (ko)
			return ns;
	}

	// Initialize some variables
	maxd = MaxVal;
	maxd2 = MaxVal2;
	large = Infl;

	// Initialize the zipped sink vector
	for (i = 0; i <= nver / lsize; i++)
		zsink[i] = 0;

	// Build the bit mask used to check when computation is complete
	val = 1;
	bit[0] = 1;
	for (i = 1; i<lsize; i++)
	{
		val *= 2;
		bit[i] = val;
	}

	//zns = ns;
	zns = 0;
	for (i = 0; i < ns; i++)
	{
		if (sink[i] >= 0)
		{
			k = (sink[i]) / lsize;
			j = (sink[i]) % lsize;

			if (!(zsink[k] & bit[j]))
			{
				zsink[k] = zsink[k] | bit[j];
				zns++;
			}
		}
	}

	// Initialize the hash n.1 empty
	for (k = 0; k <= maxd; k++)
	{
		head[k] = -1;
		tail[k] = -1;
	}

	// Initialize the hash n.2 empty
	for (k = 0; k <= maxd2; k++)
	{
		head2[k] = -1;
		tail2[k] = -1;
	}

	// Initialize the graph unexplored
	for (i = 0; i<nver; i++)
	{
		dist[i] = large;
		ipred[i] = -1;
		kpred[i] = -1;
	}

	// Begin from "root": label "root" and put "root" in the hash 
	last[root] = -1;
	dist[root] = 0;
	next[root] = -1;
	head[0] = root;
	tail[0] = root;
	inew = root;  // inew is the next vertex to get a permanent label and to be scanned
	val = zns;

	pt = 0;
	pt2 = 0;

	// Loop where nodes are expanded as soon as "inew" is equal to the last "sink" remained
	while (1)
	{
		// Check if a "sink" is reached
		k = inew / lsize;
		j = inew % lsize;
		if (zsink[k] & bit[j])
		{
			zns--;
			if (zns == 0)
				break;
		}

		// If the new permanent node "inew" is not the last "sink" then "inew" must be expanded 
		ipos1 = index[inew];
		ipos2 = index[inew + 1] - 1;

		// Now, we are going to scan all the arcs between the above two markers
		for (k = ipos1; k <= ipos2; k++)
		{
			//kcode = codarc[k];
			//karcost = costo[kcode];
			karcost = costo[k];
			kcost = karcost + dist[inew];
			j = jver[k];
			jdist = dist[j];

			// Where is j? In hash n.1 or n.2?
			if (jdist<0)
			{
				// j is in hash n.2
				hash = 1;
				jdist = -jdist;
			}
			else if (jdist<large)
			{
				// j is in hash n.1
				hash = 0;
			}
			else
			{
				// j is neither in hash n.1 nor in hash n.2
				hash = -1;
			}

			// Is the current label of j improved?
			if (kcost >= jdist)
				continue;  // It is not improved then continue to the next arc

			// The label is improved then j needs to be relabelled
			if (hash == 0) // j is the hash n. 1
			{
				// j was already labelled and must therefore be in some sublist
				qq = jdist % maxd;

				// j was in the sublist of qq. Now, we must remove it from there.
				lx = next[j];
				ls = last[j];
				if (lx == -1)
				{
					if (ls == -1)
					{
						// j in the top = bottom
						head[qq] = -1;
						tail[qq] = -1;
					}
					else
					{
						// j in the bottom
						tail[qq] = ls;
						next[ls] = -1;
					}
				}
				else
				{
					if (ls == -1)
					{
						// j in the top
						head[qq] = lx;
						last[lx] = -1;
					}
					else
					{
						// j in the middle of the list
						last[lx] = ls;
						next[ls] = lx;
					}
				}
			}
			else if (hash == 1)  // j is the hash n. 2
			{
				// j was already labelled and must therefore be in some sublist
				qq = jdist % maxd2;

				// j was in the sublist of qq. Now, we must remove it from there.
				lx = next[j];
				ls = last[j];
				if (lx == -1)
				{
					if (ls == -1)
					{
						// j in the top = bottom
						head2[qq] = -1;
						tail2[qq] = -1;
					}
					else
					{
						// j in the bottom
						tail2[qq] = ls;
						next[ls] = -1;
					}
				}
				else
				{
					if (ls == -1)
					{
						// j in the top
						head2[qq] = lx;
						last[lx] = -1;
					}
					else
					{
						// j in the middle of the list
						last[lx] = ls;
						next[ls] = lx;
					}
				}
			}

			// j has been removed from sublist qq. Now, we must put j into its new sublist
			if (karcost<maxd) // j must be put in hash n. 1
			{
				qq = kcost % maxd;

				if (head[qq] == -1)
				{
					// the sublist qq is empty
					head[qq] = j;
					last[j] = -1;
				}
				else
				{
					// some vertex is already in sublist qq
					ll = tail[qq];
					last[j] = ll;
					next[ll] = j;
				}

				ipred[j] = inew;
				kpred[j] = k;
				dist[j] = kcost;
				tail[qq] = j;
				next[j] = -1;
			}
			else
			{
				qq = kcost % maxd2;

				if (head2[qq] == -1)
				{
					// the sublist qq is empty
					head2[qq] = j;
					last[j] = -1;
				}
				else
				{
					// some vertex is already in sublist qq
					ll = tail2[qq];
					last[j] = ll;
					next[ll] = j;
				}

				ipred[j] = inew;
				kpred[j] = k;
				dist[j] = -kcost;
				tail2[qq] = j;
				next[j] = -1;
			}
		}

		// Now, we select a new vertex to make label permanent
		if (next[inew] >= 0)
		{
			inew = next[inew];
			continue;
		}

		// sublist pt containing inew is empty, therefore we find
		// the first vertex in the next non empty sublist
		head[pt] = -1;
		inew = -1;
		for (ipt = pt + 1; ipt <= maxd; ipt++)
		{
			if (head[ipt] >= 0)
			{
				pt = ipt;
				inew = head[pt];
				break;
			}
		}

		// A no-empty list is found
		if (inew >= 0)
			continue;

		// A round is complete without having found an empty list
round2:
		pt2++;
		if (pt2>maxd2) // Hash n.2 exhausted
			break;

		if (head2[pt2] >= 0)
		{
			j = head2[pt2];

			while (j >= 0)
			{
				jnext = next[j];
				kcost = -dist[j];

				qq = kcost % maxd;

				if (head[qq] == -1)
				{
					// the sublist qq is empty
					head[qq] = j;
					last[j] = -1;
				}
				else
				{
					// some vertex is already in sublist qq
					ll = tail[qq];
					last[j] = ll;
					next[ll] = j;
				}

				// ipred[j] and kpred[j] do not change
				dist[j] = kcost;
				tail[qq] = j;
				next[j] = -1;

				// next vertex to put in hash n.1
				j = jnext;
			}
		}

		// Try to find a no-empty list 
		for (pt = 0; pt <= maxd; pt++)
		{
			if (head[pt] >= 0)
			{
				inew = head[pt];
				break;
			}
		}

		// If no more non-empty lists can be found then
		// the unlabelled vertices can not be reached.
		// The "sink" cannot be reached!!!
		if (inew<0)
			goto round2;

	}

	//Mar:Apr2016: Temporary eliminated
	//if (inew<0)
	//	return zns;  // The unreached sinks

	if ((length) || (time))
	{
		for (h = 0; h<ns; h++)
		{
			// Is the sink defined? 
			if (sink[h] < 0)
			{
				km = +CON_MAXINT;
				sec = +CON_MAXINT;
				continue;
			}
			else
			{
				// If the sink is denined, then evaluate time and distance
				km = 0;
				sec = 0;
				j = sink[h];
				while (ipred[j] >= 0)
				{
					i = ipred[j];

					ipos1 = index[i];
					ipos2 = index[i + 1] - 1;

					// We are now going to scan the arcs between the above two markers
					costb = large;
					kcb = -1;
					for (k = ipos1; k <= ipos2; k++)
					{
						jj = jver[k];
						if (jj == j)
						{
							//kcode = codarc[k];  //Marco:Apr2016
							kcode = k;
							if (costo[kcode] < costb)
							{
								kcb = kcode;
								costb = costo[kcode];
							}
						}
					}

					if (kcb < 0)
						return ns;

					km += distanza[kcb];
					sec += tempo[kcb];

					j = i;
				}
			}

			// If length is required then update
			if (length)
				length[h] = km;

			// If time is required then update
			if (time)
				time[h] = sec;
		}
	}

	if (predi != NULL)
	{
		for (i = 0; i <nver; i++)
			predi[i] = ipred[i];
	}

	if (predk != NULL)
	{
		for (i = 0; i <nver; i++)
			predk[i] = kpred[i];
	}

	return zns;
}


/**----------------------------------------------------------------------------
* Compute the Shortest Paths from "root" to a list of locations "sink" using
* Dijkstra with a Double Hash and a Zipped Static Forward Star
* @param root origin of the path
* @param ns number of destinations
* @param *sink list of destinations
* @param *length list of the travel distances for each computed path
* @param *time list of the travel times for each computed path
* @param **predi predecessor used to reconstruct the computed paths
* @param **predk arc used in the computed paths
* @return the function returns the number of sinks not reached by the path.
\----------------------------------------------------------------------------*/
long Dijkstra::ComputePaths(struct DPoint proot, long ns, struct DPoint *psink, long *length, long *time, long *predi, long *predk)
{
	long root; 
	long *sink;
	long i;

	// Is the root defined?
	if (proot.karc < 0)
	{
		// Setup distance, time, and pred 
		for (i = 0; i < ns; i++)
		{
			length[i] = +CON_MAXINT;
			time[i] = +CON_MAXINT;
			if (psink[i].karc >= 0)
			{
				predi[iver[psink[i].karc]] = -1;
				predk[iver[psink[i].karc]] = -1;
			}
		}
		return 0;
	}

	// Allocate the sink nodes
	sink = new long[ns];

	// Setup the starting and ending nodes
	root = jver[proot.karc];  
	for (i = 0; i < ns; i++)
	{
		if (psink[i].karc >= 0)
			sink[i] = iver[psink[i].karc];
		else
			sink[i] = -1;
	}

	// Compute paths
	ComputePaths(root, ns, sink, length, time, predi, predk);
		
	// Add the offsets to "length" and "time" 
	for (i = 0; i < ns; i++)
	{
		// Are points on the same arc?
		if ((proot.karc == psink[i].karc) && (proot.offset < psink[i].offset + 0.00001))
		{
			// Revise pred
			predi[sink[i]] = -1;
			predk[sink[i]] = -1;

			// Update distance with offsets
			length[i] = (long)((psink[i].offset - proot.offset)*distanza[proot.karc]);

			// Update time with offsets
			time[i] = (long)((psink[i].offset - proot.offset)*tempo[proot.karc]);
		}
		else if (psink[i].karc >=0)
		{
			// Update distance with offsets
			length[i] = length[i] + (long)((1. - proot.offset)*distanza[proot.karc]);
			length[i] = length[i] + (long)((psink[i].offset)*distanza[psink[i].karc]);

			// Update time with offsets
			time[i] = time[i] + (long)((1. - proot.offset)*tempo[proot.karc]);
			time[i] = time[i] + (long)((psink[i].offset)*tempo[psink[i].karc]);
		}
		else
		{
			// The arc is not assigned, therefore we cannot reach the "root"
			length[i] = +CON_MAXINT;
			time[i] = +CON_MAXINT;
		}
	}

	return 0;
}








