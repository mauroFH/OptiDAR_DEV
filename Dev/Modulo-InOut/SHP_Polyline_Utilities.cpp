#include "../Modulo-Main/darh.h"

C_SHP::C_SHP()
{
	SHP_num_Vertices_Dim = SHP_num_Arcs_Dim = 0;

	clear();
};

C_SHP::~C_SHP()
{
}

/** to be implemented*/
void C_SHP::clear(){};

/**
 * Read a shapefile containing the polylines and builds the arc list v_SHP_Arcs_List and the forward star
 * 
 * @param *filename input = name of the shapefile without extension
 * @param *MM       input = pointer to class M used only for printiing in debugging (to be modified) 
 */
int C_SHP::SHP_readAndBuildLines(char *filename, C_IST *Ist){

	SHPHandle  shpHandle;
	DBFHandle  dbfHandle;
	int shpType, shpNumElements, shpNumArcs;

	SHP_myopen(filename, &shpHandle, &dbfHandle, &shpType, &shpNumElements, &shpNumArcs, Ist);
	if (shpType != 3) {
		error.fatal(" - this procedure read only type 3 shapefiles (polyline)", __FUNCTION__);
	}
	SHP_allocateFullLines(shpNumArcs, Ist);
	SHP_readPolyline(shpHandle, dbfHandle, Ist);
	SHP_printlists(Ist);
	//
	//cout << "shrink nodes close enough\n";
	SHP_shrinkNearVertices(Ist);
	SHP_printlists(Ist);
	//cout << "removes nodes with degree 2";
	SHP_cleanDegreeZeroAndTwo(Ist);
	// MM->printlists(this);
	SHP_buildFS(Ist);
	SHP_printFS(Ist);
	SHPClose(shpHandle);
	DBFClose(dbfHandle);
	//
	return 0;
}

/**
 * 
 * @param shpNumArcs number of directed polylines in the shape file
 */
int C_SHP::SHP_allocateFullLines(int shpNumArcs, C_IST *Ist){

	Ist->v_SHP_Vertices_List = new XYPoint_STR[shpNumArcs * 2];
	if (Ist->v_SHP_Vertices_List == NULL) {
		snprintf(buf, sizeof(buf), "- too much vertices %d; cannnot allocate shpVerticesList ", 2 * shpNumArcs);
		error.fatal(buf, __FUNCTION__);
	}
	SHP_num_Vertices_Dim = shpNumArcs * 2;
	Ist->v_SHP_Arcs_List = new Arc_STR[shpNumArcs];
	if (Ist->v_SHP_Arcs_List == NULL) {
		snprintf(buf, sizeof(buf), "- too much vertices %d; cannot allocate shpArcsList ", shpNumArcs);
		error.fatal(buf, __FUNCTION__);
	}
	SHP_num_Arcs_Dim = shpNumArcs;
	for (int i = 0; i < SHP_num_Arcs_Dim; i++) Ist->v_SHP_Arcs_List[i].info.i_shp_first = NULL;
	for (int i = 0; i < SHP_num_Arcs_Dim; i++) Ist->v_SHP_Arcs_List[i].info.i_shp_last = NULL;
	for (int i = 0; i < SHP_num_Arcs_Dim; i++) for (int j = 0; j < CON_MAXNOTURN; j++) Ist->v_SHP_Arcs_List[i].info.noturn[j] = -1;

	return 0;
}

/**   Open a Shapefile and its associated Database, reading general attributes
 *  @param *filename input : name of the files (.shp and .dbf) without extension
 *  @param *shpHandle : output handle of the shape file
 *  @param *dbfHandle : output handle of the databesa file
 *  @param *shpType         : type of the shape file
 *  @param *shpNumElements  : output number of elements in the shape file
 *  @param *shpNumArcs      : output number of arcs obtained by considering two arcs for the bidirectional polylines (streets)
 */
double C_SHP::SHP_myopen(char *filename, SHPHandle  *shpHandle, DBFHandle  *dbfHandle, int *shpType, int *numElements, int *shpNumArcs, C_IST *Ist){
	double padfMinBound[4], padfMaxBound[4]; // not reported in output
	int i;

	*shpHandle = SHPOpen(filename, "rb");
	if (*shpHandle == NULL){
		snprintf(buf, sizeof(buf), "opening  SHP file %s\n", filename);
		error.fatal(buf, __FUNCTION__);;
	}
	*dbfHandle = DBFOpen(filename, "rb");
	if (*dbfHandle == NULL){
		snprintf(buf, sizeof(buf), "opening  DBF file %s\n", filename);
		error.fatal(buf, __FUNCTION__);
	}
	// get general file info
	SHPGetInfo(*shpHandle, numElements, shpType, padfMinBound, padfMaxBound);
	//
	*shpNumArcs = 0;
	for (i = 0; i < *numElements; i++){
		int dir = SHPattributeDirection(*dbfHandle, i);
		if (dir == 0) *shpNumArcs += 2; // bidirectional
		if (dir == 1 || dir == -1) *shpNumArcs += 1;  // one way   
	}
	return 0;
}

/**
 * Determine the way of tranversing a street (forward, backward, both, no one)
 * 
 * @param dbfHandle input database handle
 * @param numElem   input number of the element to analyze
 * @return 1 = forward; -1 = backward; 0 = both directions; 99 = no transit 
 */
int C_SHP::SHPattributeDirection(DBFHandle dbfHandle, int numElem){
	int idOneway;
	char field[3];

	idOneway = DBFGetFieldIndex(dbfHandle, "ONEWAY");

	if (DBFIsAttributeNULL(dbfHandle, numElem, idOneway)) return 0;
	strncpy(field, DBFReadStringAttribute(dbfHandle, numElem, idOneway), 3);
	if (strcmp(field, " ") == 0) return 0;
	if (strcmp(field, "") == 0) return 0;
	if (strcmp(field, "FT") == 0) return 1;
	if (strcmp(field, "TF") == 0) return -1;
	if (strcmp(field, "N") == 0) return 99;
	snprintf(buf, sizeof(buf), " - wrong code '%s' for element %d", field, numElem);
	error.fatal(buf, __FUNCTION__);
	return -1; // useless, but required for the compiler if strict warning are set
}

/**
 * 
 * @param dbfHandle input data base handle
 * @param numElem input element to be read
 * @return the attribute METERS
 */
double C_SHP::SHPattributeMeters(DBFHandle dbfHandle, int numElem){
	int id;
	double meters;
	id = DBFGetFieldIndex(dbfHandle, "METERS");

	if (DBFIsAttributeNULL(dbfHandle, numElem, id)) return 0;
	meters = DBFReadDoubleAttribute(dbfHandle, numElem, id);
	return meters;
}

/**
 * reads the attribute "speed" 
 * @param dbfHandle input data base handle
 * @param numElem input element to be read
 * @return the attribute "speed"
 */
int C_SHP::SHPattributeSpeed(DBFHandle dbfHandle, int numElem){
	int id;
	int  speed;
	id = DBFGetFieldIndex(dbfHandle, "KPH");

	if (DBFIsAttributeNULL(dbfHandle, numElem, id)) return 0;
	speed = (int)DBFReadDoubleAttribute(dbfHandle, numElem, id);  // Marco:Apr2016 (cast)
	return speed;
}

/**
 * 
 * @param dbfHandle input data base handle
 * @param numElem input element to be read
 * @param index input position x of the NOTURNx field to be read
 * @return 
 */
long long C_SHP::SHPattributeNoturn(DBFHandle dbfHandle, int numElem, int index){
	int id;
	char buf[8];
	snprintf(buf, 8, "NOTURN%1d", index + 1);
	id = DBFGetFieldIndex(dbfHandle, buf);

	if (DBFIsAttributeNULL(dbfHandle, numElem, id)) return -1;
	//int forbidden_id = DBFReadIntegerAttribute(dbfHandle, numElem, id);
	long long forbidden_id = DBFReadLongLongAttribute(dbfHandle, numElem, id);
	return forbidden_id;
}

/**
 * reads the attribute "ID" 
 * @param dbfHandle input data base handle
 * @param numElem input element to be read
 * @return the attribute "speed"
 */
long long C_SHP::SHPattributeId(DBFHandle dbfHandle, int numElem){
	int i;
	long long  fid;
	//double dfid;
	i = DBFGetFieldIndex(dbfHandle, "ID");

	if (DBFIsAttributeNULL(dbfHandle, numElem, i)) return -1;
	//fid = DBFReadLongAttribute(dbfHandle, numElem, i);
	fid = DBFReadLongLongAttribute(dbfHandle, numElem, i);
	if (fid < 0)
		fid = fid;
	return fid;
}

/**
 * reads the attribute "FID" 
 * @param dbfHandle input data base handle
 * @param numElem input element to be read
 * @return the attribute "speed"
 */
long long C_SHP::SHPattributeFid(DBFHandle dbfHandle, int numElem){
	int id;
	long long  fid;
	id = DBFGetFieldIndex(dbfHandle, "FID");

	if (DBFIsAttributeNULL(dbfHandle, numElem, id)) return -1;
	fid = DBFReadIntegerAttribute(dbfHandle, numElem, id);
	return fid;
}

/**
 * Reads the attributes of an element of a shapefile (line) and store them in the arc list
 * 
 * @param dbfHandle input data base handle
 * @param i_SHP_Elem number of the element of the shapefile for reading the attributes
 * @param i_SHP_Arc number of the element oh v_SHP_Arcs_List where the attributes are stored
 * @return 
 */
int C_SHP::SHPreadAndStoreAttributes(DBFHandle dbfHandle, int i_SHP_Elem, int i_SHP_Arc, C_IST *Ist){

	long long id = C_SHP::SHPattributeId(dbfHandle, i_SHP_Elem);
	int meters = (int)C_SHP::SHPattributeMeters(dbfHandle, i_SHP_Elem);  // Marco:Apr2016 (cast)
	int velocita = C_SHP::SHPattributeSpeed(dbfHandle, i_SHP_Elem);
	double minutes = (double)meters / velocita * 0.06;
	//
	Ist->v_SHP_Arcs_List[i_SHP_Arc].info.id = id;
	Ist->v_SHP_Arcs_List[i_SHP_Arc].info.length = meters;
	Ist->v_SHP_Arcs_List[i_SHP_Arc].info.minutes = (float)minutes;
	Ist->v_SHP_Arcs_List[i_SHP_Arc].info.speed = velocita;
	for (int i = 0; i < CON_MAXNOTURN; i++) {
		Ist->v_SHP_Arcs_List[i_SHP_Arc].info.noturn[i] = SHPattributeNoturn(dbfHandle, i_SHP_Elem, i);
	}
	Ist->v_SHP_Arcs_List[i_SHP_Arc].info.i_shp_first = new struct IShape_STR;
	Ist->v_SHP_Arcs_List[i_SHP_Arc].info.i_shp_last = Ist->v_SHP_Arcs_List[i_SHP_Arc].info.i_shp_first;
	Ist->v_SHP_Arcs_List[i_SHP_Arc].info.i_shp_first->index = i_SHP_Elem;
	Ist->v_SHP_Arcs_List[i_SHP_Arc].info.i_shp_first->id = id;
	Ist->v_SHP_Arcs_List[i_SHP_Arc].info.i_shp_first->length = meters;
	int i = DBFGetFieldIndex(dbfHandle, "ONEWAY");
	if (strcmp(DBFReadStringAttribute(dbfHandle, i_SHP_Elem, i), "TF") == 0)
		Ist->v_SHP_Arcs_List[i_SHP_Arc].info.i_shp_first->direction = -1;
	else
		if (strcmp(DBFReadStringAttribute(dbfHandle, i_SHP_Elem, i), "FT") == 0)
			Ist->v_SHP_Arcs_List[i_SHP_Arc].info.i_shp_first->direction = 1;
		else
			Ist->v_SHP_Arcs_List[i_SHP_Arc].info.i_shp_first->direction = 0;
	//
	Ist->v_SHP_Arcs_List[i_SHP_Arc].info.i_shp_first->next = NULL;
	//

	return 0;
}

/**
 *  Reads a Polyline Shapefile (type 3)
 * @param shpHandle : input handle of the shape file
 * @param dbfHandle : input  handle of the database file
 * @param numElements : number of elements in the shapefile
 * @return 
 */
int C_SHP::SHP_readPolyline(SHPHandle  shpHandle, DBFHandle  dbfHandle, C_IST *Ist){
	double padfMinBound[4], padfMaxBound[4]; // dummy
	int i, shpType, numElements, nv, na;
	SHPObject *shpObject;


	// get general file info
	SHPGetInfo(shpHandle, &numElements, &shpType, padfMinBound, padfMaxBound);
	if (shpType != 3){
		snprintf(buf, sizeof(buf), " - wrong shpfile type %d", shpType);
		error.fatal(buf, __FUNCTION__);
	}
	nv = na = 0;
	for (i = 0; i < numElements; i++){
		int dir = C_SHP::SHPattributeDirection(dbfHandle, i);
		if (dir == 99) continue; // street with no traffic flow
		// the Obj type is a polyline (id = 3))
		shpObject = SHPReadObject(shpHandle, i);
		if ((dir != 0 && nv + 1 >= SHP_num_Vertices_Dim) || (dir == 0 && nv + 3 >= SHP_num_Vertices_Dim)){
			snprintf(buf, sizeof(buf), " - too much vertices reading element  %d ", i);
			error.fatal(buf, __FUNCTION__);
		}
		if ((dir != 0 && na >= SHP_num_Arcs_Dim) || (dir == 0 && na + 1 >= SHP_num_Arcs_Dim)){
			snprintf(buf, sizeof(buf), " - too much arcs reading element  %d ", i);
			error.fatal(buf, __FUNCTION__);
		}
		//
		// stores the atributes of a first arc. for bidirectional arcs the same attributes must be stored in the switch
		SHPreadAndStoreAttributes(dbfHandle, i, na, Ist);
		switch (dir) {
		case 1:
			// first point
			Ist->v_SHP_Arcs_List[na].from = nv;
			Ist->v_SHP_Vertices_List[nv].X = shpObject->padfX[0];
			Ist->v_SHP_Vertices_List[nv++].Y = shpObject->padfY[0];
			// last point
			Ist->v_SHP_Arcs_List[na++].to = nv;
			Ist->v_SHP_Vertices_List[nv].X = shpObject->padfX[shpObject->nVertices - 1];
			Ist->v_SHP_Vertices_List[nv++].Y = shpObject->padfY[shpObject->nVertices - 1];
			break;
		case -1:
			// first point
			Ist->v_SHP_Arcs_List[na].from = nv;
			Ist->v_SHP_Vertices_List[nv].X = shpObject->padfX[shpObject->nVertices - 1];
			Ist->v_SHP_Vertices_List[nv++].Y = shpObject->padfY[shpObject->nVertices - 1];
			// last point
			Ist->v_SHP_Arcs_List[na++].to = nv;
			Ist->v_SHP_Vertices_List[nv].X = shpObject->padfX[0];
			Ist->v_SHP_Vertices_List[nv++].Y = shpObject->padfY[0];
			break;
		case 0:
			// forward direction
			Ist->v_SHP_Arcs_List[na].info.i_shp_first->direction = 1;
			// first point
			Ist->v_SHP_Arcs_List[na].from = nv;
			Ist->v_SHP_Vertices_List[nv].X = shpObject->padfX[0];
			Ist->v_SHP_Vertices_List[nv].Y = shpObject->padfY[0];
			// last point
			Ist->v_SHP_Arcs_List[na++].to = nv + 1;
			Ist->v_SHP_Vertices_List[nv + 1].X = shpObject->padfX[shpObject->nVertices - 1];
			Ist->v_SHP_Vertices_List[nv + 1].Y = shpObject->padfY[shpObject->nVertices - 1];
			// backward direction 
			// first point
			Ist->v_SHP_Arcs_List[na].from = nv + 1;
			// last point
			SHPreadAndStoreAttributes(dbfHandle, i, na, Ist);
			Ist->v_SHP_Arcs_List[na].info.i_shp_first->direction = -1;
			Ist->v_SHP_Arcs_List[na++].to = nv;
			nv += 2;
		}
		SHPDestroyObject(shpObject);
	}
	// set global variables
	Ist->SHP_num_Vertices = nv;
	Ist->SHP_num_Arcs = na;

	return 0;
}

/**
 * shrinks vertices with small distance
 * @return 
 */
int C_SHP::SHP_shrinkNearVertices(C_IST *Ist){

	int *v_sorted_vertices, *v_equal_vertex, i, j, nv;

	v_sorted_vertices = new int[Ist->SHP_num_Vertices];
	v_equal_vertex = new int[Ist->SHP_num_Vertices];
	if (v_sorted_vertices == NULL || v_equal_vertex == NULL) {
		error.fatal("error allocating vectors ", __FUNCTION__);
	}
	for (i = 0; i < Ist->SHP_num_Vertices; i++) { v_sorted_vertices[i] = v_equal_vertex[i] = i; }
	/* sort the vertex indices vpointers by increasing X and Y coordinates
	 * this is used to speed up the search of 'equal' vertices to be shrinked
	 */
	quickSortVertices(v_sorted_vertices, 0, Ist->SHP_num_Vertices - 1, -1, Ist);
	/* I am not able to use qsort() allowing to have in the scope myG and its variables */
	//qsort (vpointers, numVertices, sizeof(int), C_SHP::compare1);

	//check the ordering: remove the comments for testing the correctness of the sort
	// sort test begin ----
	for (i = 0; i < Ist->SHP_num_Vertices - 1; i++) {
		if (Ist->v_SHP_Vertices_List[v_sorted_vertices[i]].X > Ist->v_SHP_Vertices_List[v_sorted_vertices[i + 1]].X){//||
			//fabs(shpVerticesList[vpointers[i]].X - shpVerticesList[vpointers[i+1]].X) < EPS &&
			//shpVerticesList[vpointers[i]].Y > shpVerticesList[vpointers[i+1]].Y ){
			error.fatal("internal vertices sorting error", __FUNCTION__);
		}
	}
	// sort test end ----
	/* look for vertices with 'distance' smaller or equal to shpEPS
	 * (two vertices are 'equal' if the second is in a square centered on the first vertex and having edge length 2 * shpEPS
	 * a vertex vj 'equal' to a previous vertex vi is marked by assiging vequal[vj] = vi
	 */
	for (i = 0; i < Ist->SHP_num_Vertices - 1; i++) {
		if (v_equal_vertex[v_sorted_vertices[i]] != v_sorted_vertices[i]) continue;
		for (j = i + 1; j < Ist->SHP_num_Vertices; j++) {
			if (Ist->v_SHP_Vertices_List[v_sorted_vertices[j]].X > Ist->v_SHP_Vertices_List[v_sorted_vertices[i]].X + shpEPS) break; // next vertices have larger distance
			// here X coordinate is inside the shpEPS distance
			if (fabs(Ist->v_SHP_Vertices_List[v_sorted_vertices[j]].Y - Ist->v_SHP_Vertices_List[v_sorted_vertices[i]].Y) < shpEPS) {
				// 'equal' vertices i and j (inside shpEPS distance)
				v_equal_vertex[v_sorted_vertices[j]] = v_sorted_vertices[i];
			}
		}
	}
	// delete 'equal' vertices 
	// remove equal vertices from Arc List
	for (i = 0; i < Ist->SHP_num_Arcs; i++) {
		Ist->v_SHP_Arcs_List[i].fromold = Ist->v_SHP_Arcs_List[i].from;
		Ist->v_SHP_Arcs_List[i].toold = Ist->v_SHP_Arcs_List[i].to;
		Ist->v_SHP_Arcs_List[i].from = v_equal_vertex[Ist->v_SHP_Arcs_List[i].from];
		Ist->v_SHP_Arcs_List[i].to = v_equal_vertex[Ist->v_SHP_Arcs_List[i].to];
	}
	// physically remove equal vertices
	nv = 0;
	for (i = 0; i < Ist->SHP_num_Vertices; i++) {
		if (v_equal_vertex[i] == i) {
			// not equal to another
			Ist->v_SHP_Vertices_List[nv] = Ist->v_SHP_Vertices_List[i];
			v_equal_vertex[i] = nv++; // new position of i
		}
		else{
			v_equal_vertex[i] = -999; // just for check, this position must not be used in the following 
		}
	}
	Ist->SHP_num_Vertices = nv; // now some memory allocated to shpVerticesList is no longer used
	// relocates vertices in Arc List
	for (i = 0; i < Ist->SHP_num_Arcs; i++) {
		Ist->v_SHP_Arcs_List[i].from = v_equal_vertex[Ist->v_SHP_Arcs_List[i].from];
		Ist->v_SHP_Arcs_List[i].to = v_equal_vertex[Ist->v_SHP_Arcs_List[i].to];
	}
	// eliminate loops
	SHPcleanLoops(Ist);
	//  eliminates multiple arcs
	SHPcleanMultipleArcs(Ist);
	//
	delete[] v_equal_vertex;
	delete[] v_sorted_vertices;

	return 0;
}

int C_SHP::SHPcleanLoops(C_IST *Ist){
	// delete loops
	for (int k = 0; k < Ist->SHP_num_Arcs; k++) {
		if (Ist->v_SHP_Arcs_List[k].from == Ist->v_SHP_Arcs_List[k].to){
			Ist->v_SHP_Arcs_List[k] = Ist->v_SHP_Arcs_List[Ist->SHP_num_Arcs - 1];
			Ist->SHP_num_Arcs--;
		}
	}
	return 0;
}

/**
 * 
 */
int C_SHP::SHPcleanMultipleArcs(C_IST *Ist){

	int i, k;
	int *v_first_FS; /// vertex pointers in the forward star
	int *v_arcs_FS;  /// arcs pointers in tge forward star

	v_first_FS = new int[Ist->SHP_num_Vertices + 1];
	v_arcs_FS = new int[Ist->SHP_num_Arcs];

	if (v_first_FS == NULL || v_arcs_FS == NULL) {
		error.fatal("error allocating Forward Star  ", __FUNCTION__);
	}

	// builds a forward star to eliminate multiple arcs
	for (i = 0; i < Ist->SHP_num_Vertices + 1; i++)
		v_first_FS[i] = 0;

	for (i = 0; i < Ist->SHP_num_Arcs; i++)
		v_first_FS[Ist->v_SHP_Arcs_List[i].from + 1]++;

	// here v_first_FS[i+1] is the number of arcs outgoing from vertex i 
	for (i = 1; i < Ist->SHP_num_Vertices; i++)
		v_first_FS[i] += v_first_FS[i - 1];

	// here v_first_FS[i] is the pointer to the first arc emanating from i
	for (i = 0; i < Ist->SHP_num_Arcs; i++) {
		v_arcs_FS[v_first_FS[Ist->v_SHP_Arcs_List[i].from]] = i;
		v_first_FS[Ist->v_SHP_Arcs_List[i].from]++;
	}

	// here v_first_FS[i] is the pointer to the last arc emanating from i plus 1
	for (i = Ist->SHP_num_Vertices; i > 0; i--)
		v_first_FS[i] = v_first_FS[i - 1];
	v_first_FS[0] = 0;

	for (i = 0; i < Ist->SHP_num_Vertices; i++) {
		for (int h = v_first_FS[i]; h < v_first_FS[i + 1]; h++)
			for (int k = h + 1; k < v_first_FS[i + 1]; k++) {
				if (Ist->v_SHP_Arcs_List[v_arcs_FS[h]].to == Ist->v_SHP_Arcs_List[v_arcs_FS[k]].to) {
					// **** IMPORTANT ***
					// multiple arcs: delete the slowest one ... other criteria can be adopted
					if (Ist->v_SHP_Arcs_List[v_arcs_FS[k]].info.minutes >= Ist->v_SHP_Arcs_List[v_arcs_FS[h]].info.minutes){
						Ist->v_SHP_Arcs_List[v_arcs_FS[k]].from = -1; // mark as deleted 
					}else{
						Ist->v_SHP_Arcs_List[v_arcs_FS[h]].from = -1; // mark as deleted    
                                        }
                                }
			}
	}

	// remove the arcs marked to be deleted
	int na = 0;
	for (k = 0; k < Ist->SHP_num_Arcs; k++){
		if (Ist->v_SHP_Arcs_List[k].from < 0) continue;
		Ist->v_SHP_Arcs_List[na++] = Ist->v_SHP_Arcs_List[k];
	}
	Ist->SHP_num_Arcs = na;
	//
	delete[] v_first_FS;
	delete[] v_arcs_FS;

	return 0;
}

/* Deleted code: 
 * I am not able to use qsort() allowing to have in the scope myG and its variables
 * int C_SHP::compare1 (const void * a, const void * b)
{
    int i,j;
    i = *(int*)a;
    j = *(int*)b;
    
    //if ( shpVerticesList[i].X  <  shpVerticesList[j].X - EPS ) return -1;
    //if ( shpVerticesList[i].X  >  shpVerticesList[j].X + EPS ) return 1;
    // equal X coordinate
    //if ( shpVerticesList[i].Y  <  shpVerticesList[j].Y ) return -1;
    //if ( shpVerticesList[i].Y  >  shpVerticesList[j].Y + EPS) return 1;
    
    return 0;
}
 * */

/**
 * check if it is forbidden to turn from a1 to  a2 
 * @param a1 entering arc
 * @param a2 leaving arc
 * @param dffHandle input database handle
 * @return 1 = forbidden 0 otherwise
 */
int C_SHP::SHPturnForbidden(Arc_STR *a1, Arc_STR *a2){

	IShape_STR *is;

	for (int i = 0; i < CON_MAXNOTURN; i++) {
		for (is = a2->info.i_shp_first; is != NULL; is = is->next) {
			if (is->id == a1->info.noturn[i]) return 1;
		}
	}
	return 0;
}

/**
 * concatenate arc a2 after  arc a1 giving an upodated arc a1 =  (a1 -> a2)
 * a1 is given the "sum" of the properties of both arcs
 * @param a1 input arc to which we add a2
 * @param a2 input arc
 * @return 
 */
int C_SHP::SHPconnectArcs(Arc_STR *a1, Arc_STR *a2){
	a1->info.length += a2->info.length;
	a1->info.minutes += a2->info.minutes;
	a1->to = a2->to;
	SHPaddIShape(a1, a2);
	/* a1 enter into a vertex that will be removed, so it should not have noturn[] properties */
	for (int i = 0; i < CON_MAXNOTURN; i++) a1->info.noturn[i] = a2->info.noturn[i];
	return 0;

}

int C_SHP::SHPaddIShape(Arc_STR *a1, Arc_STR *a2){
	IShape_STR *p1_is, *p2_is;
	p2_is = a2->info.i_shp_first;
	if (p2_is == NULL) return 0;
	if (a1->info.i_shp_first == NULL) {
		a1->info.i_shp_first = a1->info.i_shp_last = p2_is;
		p2_is->prev = NULL;
	}
	else{
		p1_is = a1->info.i_shp_last;
		p1_is->next = p2_is;
		p2_is->prev = p1_is;
		a1->info.i_shp_last = a2->info.i_shp_last;
	}
	return 0;
}

/**
 *  eliminates vertices with degree 0 or 2
 * 
 * @return 0 or < 0 if an error occurs
 */
int C_SHP::SHP_cleanDegreeZeroAndTwo(C_IST *Ist){
	int i_ret;
	do {
		i_ret = SHPcleanDegreeZeroAndTwo_function(Ist);
	} while (i_ret == 1);
	//
	return i_ret;
}

/**
 * eliminates vertices with degree 0 or 2
 * @return 1 if some vertex have beeen removed, 0 if no vertex have been removed, -1 if an error occurs
 */
int C_SHP::SHPcleanDegreeZeroAndTwo_function(C_IST *Ist){
	int *v_num_in;  /// v_num_in[i] = number of arcs entering  vertex i
	int *v_num_out; /// v_num_out[i] = number of arcs exiting  vertex i
	int *v_first_in;  /// v_first_in[i] = index of the first arc entering  vertex i
	int *v_first_out; /// v_first_out[i] = index of the first arc exiting   vertex i
	int *v_second_in;  /// v_second_in[i] = index of the second arc entering  vertex i
	int *v_second_out; /// v_second_out[i] = index of the second arc exiting   vertex i
	int *v_vertex_new_pos; /// v_vertex_new_pos[i] = new position in SHP_Vertices_List of vertex originally in position i   
	int *v_arc_new_pos; /// v_arc_new_pos[i] = new position in SHP_Arcs_List of arc originally in position i]
	int i, k, nv, na, modified = 0;

	v_num_in = new int[Ist->SHP_num_Vertices];
	v_num_out = new int[Ist->SHP_num_Vertices];
	v_first_in = new int[Ist->SHP_num_Vertices + 1];
	v_first_out = new int[Ist->SHP_num_Vertices + 1];
	v_second_in = new int[Ist->SHP_num_Vertices];
	v_second_out = new int[Ist->SHP_num_Vertices];
	v_vertex_new_pos = new int[Ist->SHP_num_Vertices];
	v_arc_new_pos = new int[Ist->SHP_num_Arcs];

	if (v_num_in == NULL || v_num_out == NULL || v_first_in == NULL || v_first_out == NULL ||
		v_second_in == NULL || v_second_out == NULL || v_vertex_new_pos == NULL || v_arc_new_pos == NULL) {
		error.fatal("error allocating v_xxx ", __FUNCTION__);
	}
	for (i = 0; i < Ist->SHP_num_Vertices; i++) v_num_in[i] = v_num_out[i] = 0;
	for (i = 0; i < Ist->SHP_num_Vertices; i++) v_first_in[i] = v_first_out[i] = -1;
	for (i = 0; i < Ist->SHP_num_Vertices; i++) v_second_in[i] = v_second_out[i] = -1;
	// compute degree of the vertices
	for (k = 0; k < Ist->SHP_num_Arcs; k++){
		v_num_in[Ist->v_SHP_Arcs_List[k].to]++;
		v_num_out[Ist->v_SHP_Arcs_List[k].from]++;
		v_arc_new_pos[k] = -1;
		/* store first and second  arcs entering or exiting the vertex:
		 * N.B. does not matter if it is not the very second,
		 * since we use the information only for vertices with degree 2
		 */
		if (v_first_in[Ist->v_SHP_Arcs_List[k].to] < 0) {
			v_first_in[Ist->v_SHP_Arcs_List[k].to] = k;
		}
		else{
			v_second_in[Ist->v_SHP_Arcs_List[k].to] = k;
		}
		if (v_first_out[Ist->v_SHP_Arcs_List[k].from] < 0) {
			v_first_out[Ist->v_SHP_Arcs_List[k].from] = k;
		}
		else{
			v_second_out[Ist->v_SHP_Arcs_List[k].from] = k;
		}
	}
	// looks for leaves (a single arc entering or exiting)
	for (i = 0; i < Ist->SHP_num_Vertices; i++){
		if (v_num_in[i] == 1 && v_num_out[i] == 0){
			snprintf(buf, sizeof(buf), "isolated arc in Vertex=%d Arc=%ld %d %d from %f %f  to %f %f", i, Ist->v_SHP_Arcs_List[v_first_in[i]].info.i_shp_first->index,
				Ist->v_SHP_Arcs_List[v_first_in[i]].from, Ist->v_SHP_Arcs_List[v_first_in[i]].to,
				Ist->v_SHP_Vertices_List[Ist->v_SHP_Arcs_List[v_first_in[i]].from].X,
				Ist->v_SHP_Vertices_List[Ist->v_SHP_Arcs_List[v_first_in[i]].from].Y,
				Ist->v_SHP_Vertices_List[Ist->v_SHP_Arcs_List[v_first_in[i]].to].X,
				Ist->v_SHP_Vertices_List[Ist->v_SHP_Arcs_List[v_first_in[i]].to].Y
				);
			error.warning(buf, __FUNCTION__);
		}
		if (v_num_in[i] == 0 && v_num_out[i] == 1){
			snprintf(buf, sizeof(buf), "isolated arc out of vertex #%ld, (%lf,%lf)", 
                                Ist->v_SHP_Arcs_List[v_first_out[i]].info.i_shp_first->index,
				Ist->v_SHP_Vertices_List[Ist->v_SHP_Arcs_List[v_first_out[i]].from].X,
				Ist->v_SHP_Vertices_List[Ist->v_SHP_Arcs_List[v_first_out[i]].from].Y);                                
			error.fatal(buf, __FUNCTION__);
		}
		if (v_num_in[i] == 1 && v_num_out[i] == 1 && Ist->v_SHP_Arcs_List[v_first_in[i]].from == Ist->v_SHP_Arcs_List[v_first_out[i]].to){
			// 2 loop. Tho arcs with opposite direction derived from a bidirectional polyline
			snprintf(buf, sizeof(buf), "isolated line #%ld in the shapefile (%lf,%lf) /%lf,%lf)",
                                    Ist->v_SHP_Arcs_List[v_first_out[i]].info.i_shp_first->index,
                                    Ist->v_SHP_Vertices_List[Ist->v_SHP_Arcs_List[v_first_out[i]].from].X,
                                    Ist->v_SHP_Vertices_List[Ist->v_SHP_Arcs_List[v_first_out[i]].from].Y,                                
                                    Ist->v_SHP_Vertices_List[Ist->v_SHP_Arcs_List[v_first_out[i]].to].X,
                                    Ist->v_SHP_Vertices_List[Ist->v_SHP_Arcs_List[v_first_out[i]].to].Y);                                
			error.fatal(buf, __FUNCTION__);
		}
	}
	// finds and deletes vertices with degree 2
	for (i = 0; i < Ist->SHP_num_Vertices; i++) v_vertex_new_pos[i] = -777; // sanity check
	nv = 0;
	for (i = 0; i < Ist->SHP_num_Vertices; i++){
		int NoChange = 1;
		if (v_num_in[i] == 0 && v_num_out[i] == 0) {
			NoChange = 0;
		}

		if (v_num_in[i] == 1 && v_num_out[i] == 1) {
			// remove one arc
			SHPconnectArcs(&Ist->v_SHP_Arcs_List[v_first_in[i]], &Ist->v_SHP_Arcs_List[v_first_out[i]]);
			Ist->v_SHP_Arcs_List[v_first_out[i]].from = -1;  // mark arc to be deleted
			int b = Ist->v_SHP_Arcs_List[v_first_out[i]].to;
			if (v_first_in[b] == v_first_out[i]) v_first_in[b] = v_first_in[i];
			if (v_second_in[b] == v_first_out[i]) v_second_in[b] = v_first_in[i];
			NoChange = 0; modified = 1;
		}
		if (v_num_in[i] == 2 && v_num_out[i] == 2) {
			/**
			 * two possible situations for bidirectional links
			 * a -- first in --> (i) -- second out --> b
			 * a <- first out--  (i) <- second in  --  b
			 *
			 * a -- first in --> (i) -- first  out --> b
			 * a <- second out-- (i) <- second in  --  b
			 */
			if (Ist->v_SHP_Arcs_List[v_first_in[i]].info.i_shp_last->index == Ist->v_SHP_Arcs_List[v_first_out[i]].info.i_shp_first->index &&
				Ist->v_SHP_Arcs_List[v_second_in[i]].info.i_shp_last->index == Ist->v_SHP_Arcs_List[v_second_out[i]].info.i_shp_first->index) {
				// remove one arc
				SHPconnectArcs(&Ist->v_SHP_Arcs_List[v_first_in[i]], &Ist->v_SHP_Arcs_List[v_second_out[i]]);
				Ist->v_SHP_Arcs_List[v_second_out[i]].from = -1;  // mark arc to be deleted 
				// updates first and second of vertex b
				int b = Ist->v_SHP_Arcs_List[v_second_out[i]].to;
				if (v_first_in[b] == v_second_out[i]) v_first_in[b] = v_first_in[i];
				if (v_second_in[b] == v_second_out[i]) v_second_in[b] = v_first_in[i];
				SHPconnectArcs(&Ist->v_SHP_Arcs_List[v_second_in[i]], &Ist->v_SHP_Arcs_List[v_first_out[i]]);
				Ist->v_SHP_Arcs_List[v_first_out[i]].from = -1;  // mark arc to be deleted  
				// update first and second of vertex a
				int a = Ist->v_SHP_Arcs_List[v_first_out[i]].to;
				if (v_first_in[a] == v_first_out[i]) v_first_in[a] = v_second_in[i];
				if (v_second_in[a] == v_first_out[i]) v_second_in[a] = v_second_in[i];
				NoChange = 0; modified = 1;
			}
			else{
				if (Ist->v_SHP_Arcs_List[v_first_in[i]].info.i_shp_last->index == Ist->v_SHP_Arcs_List[v_second_out[i]].info.i_shp_first->index &&
					Ist->v_SHP_Arcs_List[v_first_out[i]].info.i_shp_first->index == Ist->v_SHP_Arcs_List[v_second_in[i]].info.i_shp_last->index) {
					// remove one arc
					SHPconnectArcs(&Ist->v_SHP_Arcs_List[v_first_in[i]], &Ist->v_SHP_Arcs_List[v_first_out[i]]);
					Ist->v_SHP_Arcs_List[v_first_out[i]].from = -1;  // mark arc to be deleted
					// update first and second of vertex b              
					int b = Ist->v_SHP_Arcs_List[v_first_out[i]].to;
					if (v_first_in[b] == v_first_out[i]) v_first_in[b] = v_first_in[i];
					if (v_second_in[b] == v_first_out[i]) v_second_in[b] = v_first_in[i];
					SHPconnectArcs(&Ist->v_SHP_Arcs_List[v_second_in[i]], &Ist->v_SHP_Arcs_List[v_second_out[i]]);
					Ist->v_SHP_Arcs_List[v_second_out[i]].from = -1;  // mark arc to be deleted    
					// update first and second of vertex a
					int a = Ist->v_SHP_Arcs_List[v_second_out[i]].to;
					if (v_first_in[a] == v_second_out[i]) v_first_in[a] = v_second_in[i];
					if (v_second_in[a] == v_second_out[i]) v_second_in[a] = v_second_in[i];
					NoChange = 0; modified = 1;
				}
			}
		}
		if (NoChange == 1)  {
			// nothing to do, but i is repositioned in the list 
			Ist->v_SHP_Vertices_List[nv] = Ist->v_SHP_Vertices_List[i];
			v_vertex_new_pos[i] = nv++;
		}
	}
	Ist->SHP_num_Vertices = nv;

	// remove the arcs marked to be deleted
	na = 0;
	for (k = 0; k < Ist->SHP_num_Arcs; k++){
		if (Ist->v_SHP_Arcs_List[k].from < 0) continue;
		v_arc_new_pos[k] = na++;
	}
	na = 0;
	for (k = 0; k < Ist->SHP_num_Arcs; k++){
		if (Ist->v_SHP_Arcs_List[k].from < 0) continue;
		Ist->v_SHP_Arcs_List[na] = Ist->v_SHP_Arcs_List[k];
		if (v_vertex_new_pos[Ist->v_SHP_Arcs_List[k].from] < 0){
			char buf[100];
			snprintf(buf, sizeof(buf), "\n new vertex pos negative ! Arc=%d from=%d new =%d", k, Ist->v_SHP_Arcs_List[k].from, v_vertex_new_pos[Ist->v_SHP_Arcs_List[k].from]);
			error.fatal(buf, __FUNCTION__);
		}
		Ist->v_SHP_Arcs_List[na].from = v_vertex_new_pos[Ist->v_SHP_Arcs_List[na].from];
		if (v_vertex_new_pos[Ist->v_SHP_Arcs_List[k].to] < 0){
			char buf[100];
			snprintf(buf, sizeof(buf), "\n new pos negative ! Arc=%d to=%d new =%d", i, Ist->v_SHP_Arcs_List[i].to, v_vertex_new_pos[Ist->v_SHP_Arcs_List[i].to]);
			error.fatal(buf, __FUNCTION__);
		}
		Ist->v_SHP_Arcs_List[na].to = v_vertex_new_pos[Ist->v_SHP_Arcs_List[na].to];
		na++;
	}
	Ist->SHP_num_Arcs = na;
	//
	/*cout << " \n --prima di cleanmultiple\n";

	cout << fixed;

	cout << "----\n";
	cout <<setw(9)<< "vertex" << " "<<setw(9) << "X"<<", "<<setw(9)<<"Y" << "\n";
	for (int i=0; i < SHP_num_Vertices; i++) {
	cout <<setw(9)<< i << " "<<setw(9) << v_SHP_Vertices_List[i].X<<", "<<setw(9)<<v_SHP_Vertices_List[i].Y << "\n";
	}
	cout << "\n";
	cout <<setw(9)<< "Arc" << " " <<setw(9)<< "from" <<", "<<setw(9)<<"to" <<setw(7)<<"length"<<" "<<setw(8) <<"minutes" << "\n";
	for (int i=0; i < SHP_num_Arcs; i++) {
	cout <<setw(9)<< i << " " <<setw(9)<< Ist->v_SHP_Arcs_List[i].from <<", "<<setw(9)<<Ist->v_SHP_Arcs_List[i].to
	<<setw(7)<<Ist->v_SHP_Arcs_List[i].info.length <<" "<<setw(8) <<Ist->v_SHP_Arcs_List[i].info.minutes << "\n";
	}
	*/

	SHPcleanMultipleArcs(Ist);
	//
	delete[] v_num_in;
	delete[] v_num_out;
	delete[] v_first_in;
	delete[] v_first_out;
	delete[] v_second_in;
	delete[] v_second_out;
	delete[] v_vertex_new_pos;
	delete[] v_arc_new_pos;

	return modified;
}

/**
 * Split 2 arcs loops (i --> j; j --> i) by defining 2 vertices for each endpoint (i', i", j', j")
 * 
 */
int C_SHP::SHPsplit2Loops(C_IST *Ist){
	int i, j, ia, ia2;

	for (ia = 0; ia < Ist->SHP_num_Arcs - 1; ia++){
		i = Ist->v_SHP_Arcs_List[ia].from;
		j = Ist->v_SHP_Arcs_List[ia].to;
		for (ia2 = ia + 1; ia2 < Ist->SHP_num_Arcs; ia2++){
			if (i != Ist->v_SHP_Arcs_List[ia2].to) continue;
			if (j != Ist->v_SHP_Arcs_List[ia2].from) continue;
			// adds 2 vertices
			if (Ist->SHP_num_Vertices >= SHP_num_Vertices_Dim) {
				error.fatal("exceeded SHP_num_Vertices_Dim", __FUNCTION__);
			}
			// i prime
			Ist->v_SHP_Vertices_List[Ist->SHP_num_Vertices] = Ist->v_SHP_Vertices_List[j];
		}
	}
	return 0;
}

/***********************************************************************************************
 *  QUICK SORT
 *  LIST		input/output  array: LIST[i]
 *  left_index	input         int  : left  position in range list to sort (0 <= left < right) 
 *  right_index	input         int  : right position in range list to sort (right < lenght)
 *  type		input         int  : type =  1 'LIST[i]' is sorted non-increasing
 *                                   type = -1 'LIST[i]' is sorted increasing
 ***********************************************************************************************/
void C_SHP::quickSortVertices(int *LIST, int left_index, int right_index, double type, C_IST *Ist)
{
	int l = left_index;
	int r = right_index;
	int i, j;
	double pivX, pivY;
	do{
		i = l;
		j = r;
		//piv = LIST[(l + r) >> 1];
		pivX = Ist->v_SHP_Vertices_List[LIST[(l + r) >> 1]].X;
		pivY = Ist->v_SHP_Vertices_List[LIST[(l + r) >> 1]].Y;
		do{
			while ((type * (Ist->v_SHP_Vertices_List[LIST[i]].X - pivX) > CON_EPS ||
				(fabs(Ist->v_SHP_Vertices_List[LIST[i]].X - pivX) < CON_EPS &&
				type *(Ist->v_SHP_Vertices_List[LIST[i]].Y - pivY) > CON_EPS) )&& i < r) i++;
			while ((type * (Ist->v_SHP_Vertices_List[LIST[j]].X - pivX) < -CON_EPS ||
				(fabs(Ist->v_SHP_Vertices_List[LIST[j]].X - pivX) < CON_EPS &&
				type*(Ist->v_SHP_Vertices_List[LIST[j]].Y - pivY) < -CON_EPS)) && j > l) j--;
			if (i < j) SWAP(LIST[i], LIST[j], int);
			if (i <= j){ i++; j--; }
		} while (i <= j);
		if (l < j) quickSortVertices(LIST, l, j, type, Ist);
		l = i;
	} while (i < r);
};

/**
 * Given a path in TRoute, creates a shapefile conatining the polylines corresponding to these arcs
 * 
 * @param Instance  input instance name
 * @param narcs     input number of arcs in the path
 * @param arc       input vector containing the index of the arcs in the path for v_ArcFS[]
 * *param id        input id[i] = number of the segment of trip between two stops at which belongs arc i
 * @return 
 */
int C_SHP::SHP_writeShapeFromPath(char *Instance, long narcs, long *arc, int  *id, C_IST *Ist){
	SHPHandle  shpHandleI, shpHandleW;
	SHPObject *shpObject;
	DBFHandle  dbfHandleI, dbfHandleW;
	int numElements, shpType, iField;
	double padfMinBound[4], padfMaxBound[4];
	char filename[100];
	struct IShape_STR *is;

	// open input and output shapefiles
	shpHandleI = SHPOpen(Ist->networkFileName, "rb");
	if (shpHandleI == NULL){
		snprintf(buf, sizeof(buf), "opening  SHP file %s\n", Ist->networkFileName);
		error.fatal(buf, __FUNCTION__);
	}
	dbfHandleI = DBFOpen(Ist->networkFileName, "rb");
	if (dbfHandleI == NULL){
		snprintf(buf, sizeof(buf), "opening  DBF file %s\n", Ist->networkFileName);
		error.fatal(buf, __FUNCTION__);
	}
	snprintf(filename, sizeof(filename), "%s//%s_Path", OUTPUTDIR, Instance);
	shpHandleW = SHPCreate(filename, SHPT_ARC);
	if (shpHandleW == NULL){
		snprintf(buf, sizeof(buf), "create a SHP file <%s>\n", filename);
		error.fatal(buf, __FUNCTION__);
	}
	dbfHandleW = DBFCloneEmpty(dbfHandleI, buf);
	if (dbfHandleW == NULL){
		snprintf(buf, sizeof(buf), "create a DBF file <%s>\n", filename);
		error.fatal(buf, __FUNCTION__);
	}
	DBFAddField(dbfHandleW, (char *) "SEGMENT", FTInteger, 5, 0);
	iField = DBFGetFieldIndex(dbfHandleW, "SEGMENT");
	// get general file info
	SHPGetInfo(shpHandleI, &numElements, &shpType, padfMinBound, padfMaxBound);
	//
	char record[500];
	const char *cc; cc = &record[0];
	int n_rec = 0;
	for (int i = 0; i < narcs; i++){
		int k = Ist->v_ArcsFS[arc[i]].original_Arc;
		if (k < 0) continue; // dummy arc of the expanded graph
		// write all the polylines associated with arc k
		for (is = Ist->v_SHP_Arcs_List[k].info.i_shp_first; is != NULL; is = is->next){
			int i_shp = is->index;
			shpObject = SHPReadObject(shpHandleI, i_shp);
			if (SHPWriteObject(shpHandleW, -1, shpObject) < 0){
				snprintf(buf, sizeof(buf), "Error writing polyline N. %d in %s", i_shp,filename);
				error.fatal(buf, __FUNCTION__);
			}
			cc = DBFReadTuple(dbfHandleI, i_shp);
			if (DBFWriteTuple(dbfHandleW, n_rec, (void *)cc) < 0){
				snprintf(buf, sizeof(buf), "Error writing attributes of polyline N. %d  in %s", i_shp,filename);
				error.fatal(buf, __FUNCTION__);
			}
			if (DBFWriteIntegerAttribute(dbfHandleW, n_rec, iField, id[i]) < 0){
				snprintf(buf, sizeof(buf), "Error writing attributes of SEGMENT polyline N. %d in %s", i_shp,filename);
				error.fatal(buf, __FUNCTION__);
			}
			n_rec++;
                         SHPDestroyObject(shpObject);
		}// for is
	}
	SHPClose(shpHandleI);
	DBFClose(dbfHandleI);
	SHPClose(shpHandleW);
	DBFClose(dbfHandleW);
	return 0;
}

/* creates a copy of a shapefile : IT IS AN EXAMPLE : DO NOT USE
 * @param filename input name of the file without extension
 */
int C_SHP::SHP_Copy(char *filename){
	SHPHandle  shpHandle, shpHandleW;
	SHPObject *shpObject;
	DBFHandle  dbfHandle, dbfHandleW;
	int numElements, shpType;
	double padfMinBound[4], padfMaxBound[4];

	shpHandle = SHPOpen(filename, "rb");
	if (shpHandle == NULL){
		snprintf(buf, sizeof(buf), "Error opening  SHP file %s\n", filename);
		error.fatal(buf, __FUNCTION__);
	}
	dbfHandle = DBFOpen(filename, "rb");
	if (dbfHandle == NULL){
		snprintf(buf, sizeof(buf), "Error opening  DBF file %s\n", filename);
		error.fatal(buf, __FUNCTION__);;
	}
	shpHandleW = SHPCreate("input\\pippo", SHPT_ARC);
	if (shpHandle == NULL){
		printf("Error to create a SHP file Write\n");
	}
	dbfHandleW = DBFCloneEmpty(dbfHandle, "input\\pippo");
	if (dbfHandleW == NULL){
		printf("Error to create a DBF file Write\n");
	}
	// get general file info
	SHPGetInfo(shpHandle, &numElements, &shpType, padfMinBound, padfMaxBound);
	//
	char record[500];
	const char *cc; cc = &record[0];
	int n_rec = 0;
	for (int i = 0; i < numElements; i++){
		shpObject = SHPReadObject(shpHandle, i);
		cc = DBFReadTuple(dbfHandle, i);
		DBFWriteTuple(dbfHandleW, n_rec++, (void *)cc);
                SHPDestroyObject(shpObject);
	}
	SHPClose(shpHandleW);
	DBFClose(dbfHandleW);
	return 0;
}

/**
 * Checks if an arc of the forward star 'traverse' a point
 * @param p input = a point
 * @param i_arc   = the index of an arc of the forward star
 * @return 1 if the point or its projection fall on one of the  segments of the polylines associated with the arc
 *         0 if the point or the projecton is outside
 *        -1 if an error occurs
 */
int C_SHP::SHP_isPointOnArc(SHPHandle  shpHandle, XYPoint_STR p, int i_arc, C_IST *Ist){

	SHPObject  *shpObject;
	XYPoint_STR v, w;
	int i, shpType, numElements, rvalue;
	double padfMinBound[4], padfMaxBound[4];
	char buf[50];
	Arc_STR *a;
	IShape_STR *is;

	rvalue = 0;
	// get general file info
	SHPGetInfo(shpHandle, &numElements, &shpType, padfMinBound, padfMaxBound);
	if (shpType != 3){
		snprintf(buf, sizeof(buf), " - wrong shpfile type %d, mut be 3", shpType);
		error.fatal(buf, __FUNCTION__);
	}
	if (p.X < padfMinBound[0] || p.X > padfMaxBound[0] ||
		p.Y < padfMinBound[1] || p.Y > padfMaxBound[1]) {
		// point outside the map
		rvalue = 0; goto terminate;
	}
	//
	a = &Ist->v_SHP_Arcs_List[Ist->v_ArcsFS[i_arc].original_Arc]; // a is the 'oiginal' arc 
	for (is = a->info.i_shp_first; is != NULL; is = is->next) {
		shpObject = SHPReadObject(shpHandle, (int)is->index);
		if (p.X < shpObject->dfXMin || p.X > shpObject->dfXMax ||
			p.Y < shpObject->dfYMin || p.Y > shpObject->dfYMax)      continue; // outside the polyline bounding box
		for (i = 0; i < shpObject->nVertices - 1; i++) {
			// check part i 
			v.X = shpObject->padfX[i]; v.Y = shpObject->padfY[i];     // starting point of the segment
			w.X = shpObject->padfX[i + 1]; w.Y = shpObject->padfY[i + 1]; // ending point of the segment
			const double L2 = (v.X - w.X)*(v.X - w.X) + (v.Y - w.Y)*(v.Y - w.Y); //length_squared(v, w); i.e. |w-v|^2              
			double  t = ((p.X - v.X) * (w.X - v.X) + (p.Y - v.Y) * (w.Y - v.Y)) / L2;
			if (t > 1 || t < 0) continue; // projection of p outside the segment
			// here the projection of p is inside the segment !
			rvalue = 1;
			goto terminate;
		}
                SHPDestroyObject(shpObject);
	}
	rvalue = 0;
terminate:
	SHPClose(shpHandle);
	return rvalue;
}

/**
* Check if a point or its projection are on a segment
* @param p input = point to be evaluated
* @param v input = first point of the segment
* @param w input = second point of the segment
* @param offset output = percentage offset with respect to v
* @param distance output = distance of p from the segment (-1  if the point is not on the segment)
* @return 1 if the point  is on the segment, 0 otherwise
*/
int C_SHP::SHP_isPointOnSegment(XYPoint_STR p, XYPoint_STR v, XYPoint_STR w, double *offset, double *distance){


	static double L2;
	double  t;
        XYPoint_STR projection;
        *distance = *offset = -1;

	L2 = (v.X - w.X)*(v.X - w.X) + (v.Y - w.Y)*(v.Y - w.Y); //length_squared(v, w); i.e. |w-v|^2              

	if (L2 < CON_EPS){
		t = 2; projection.X = v.X; projection.Y = v.Y;
	}else{
		t = ((p.X - v.X) * (w.X - v.X) + (p.Y - v.Y) * (w.Y - v.Y)) / L2;
                projection.X = v.X + t * (w.X - v.X);
                projection.Y = v.Y + t * (w.Y - v.Y);
	}

	if (t > 1 || t < 0)
		return 0; // projection of p outside the segment

        *offset = t;
	// here the projection of p is inside the segment !
        *distance = SHPplanarDistance2P(p, projection);
        if (*distance < shpCLOSE )  return 1;
        else              return 0;
}

/**
 * Check if two points coincide
 * @param p1 input = first point
 * @param v2 input = second point
 * @return 1 if the two points coincide, 0 otherwise
 */
int C_SHP::SHP_PointsCoincide(XYPoint_STR p1, XYPoint_STR p2){
	const double L2 = (p1.X - p2.X)*(p1.X - p2.X) + (p1.Y - p2.Y)*(p1.Y - p2.Y);
	if (L2 < CON_EPS) return 1;
	return 0;
}

