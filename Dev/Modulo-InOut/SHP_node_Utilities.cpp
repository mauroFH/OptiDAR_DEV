#include "../Modulo-Main/darh.h"

/**
 * determine the side of a point p with respect to a segment (v,W) 
 * @param v input firs vertex of the segment
 * @param w input second vertex of the segment
 * @param p input point to be positioned
 * @return 1 if p is on the left, -1 if p is on the right, 0 if p is on the segment 
 */
int C_SHP::SHPpointSide(XYPoint_STR v, XYPoint_STR w, XYPoint_STR p){
	// vertical line
	if (fabs(v.X - w.X) < CON_EPS) {
		if (fabs(v.X - p.X) < CON_EPS) return 0;
		if (p.X < v.X) return  1;
		if (p.X > v.X) return  -1;
	}
	// horizzontal line
	if (fabs(v.Y - w.Y) < CON_EPS) {
		if (fabs(v.Y - p.Y) < CON_EPS) return 0;
		if (p.Y < v.Y) return -1;
		if (p.Y > v.Y) return  1;
	}
	double det = ((w.X - v.X)*(p.Y - v.Y) - (w.Y - v.Y)*(p.X - v.X));

	if (det > 0) return 1; // left 
	if (fabs(det) < CON_EPS) return 0;
	return -1; // right
}

/**
 * Compute the planar distance between two points 
 * @param v input first point
 * @param w input second point
 * @return 
 */
double C_SHP::SHPplanarDistance2P(XYPoint_STR v, XYPoint_STR w)
{
	return sqrt((v.X - w.X)*(v.X - w.X) + (v.Y - w.Y)*(v.Y - w.Y));
}

/**
 * Computes the minimum distance btween a point p and a segment identified by its extremes
 * @param v input first poit of the segment
 * @param w input second point of the segment
 * @param p input point 
 * @param offset output percentage offset with respect to v
 * @return 
/
double  C_SHP::SHPminimum_distanceP2Segment(XYPoint_STR v, XYPoint_STR w, XYPoint_STR p, double *offset) {
	// Return minimum distance between line segment vw and point p
	const double L2 = (v.X - w.X)*(v.X - w.X) + (v.Y - w.Y)*(v.Y - w.Y); //length_squared(v, w);  // i.e. |w-v|^2 -  avoid a sqrt
	if (L2 < CON_EPS) return SHPplanarDistance2P(p, v);   // v == w case
	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	// We clamp t from [0,1] to handle points outside the segment vw.
	double  t = ((p.X - v.X) * (w.X - v.X) + (p.Y - v.Y) * (w.Y - v.Y)) / L2;
	if (t > 1) t = 1;
	if (t < 0) t = 0;
	//t = max(0, min(1, t));//t = max(0, min(1, dot(p - v, w - v) / l2));
	XYPoint_STR projection;// projection = v + t * (w - v);  // Projection falls on the segment
	projection.X = v.X + t * (w.X - v.X);
	projection.Y = v.Y + t * (w.Y - v.Y);
	*offset = t;
	return SHPplanarDistance2P(p, projection);
}
*/
/*int C_SHP::SHPmatchFS(SHPHandle *shpHandle, int numElem, Point p, int *j_arc, int * i_shape  ){
}*/

/**
 * Identifies if a point p is close or inside to the boundin box of a polyline
 * 
 * @param shpObject input handle of the shapefile object to be examined
 * @param i_SHP_index input index of the polyline to be analyzed
 * @param p input Point to be evaluated
 * @return 1 if the point is "far" from the bounding box of the polyline, 0 otherwise
 */
int C_SHP::SHPpointOutsideBox(SHPObject *shpObject, int i_SHP_index, XYPoint_STR p){

	if (p.X < shpObject->dfXMin - shpCLOSE) return 1;
	if (p.X > shpCLOSE + shpObject->dfXMax) return 1;
	if (p.Y  < shpObject->dfYMin - shpCLOSE) return 1;
	if (p.Y > shpCLOSE + shpObject->dfYMax) return 1;
	return 0;
}

/**
 * compute the distance of a point from a polyline
 * 
 * @param shpHandle input handle of the polyline shapefile
 * @param is input pointer to the IShape containing the  polyline to be analyzed
 * @param p input Point to be evaluated
 * @param RightLeft input true: serach an arc having p at its right, false: serach an arc having p at its left 
 * @param SHP_meters input length of the polyline stored in field meters of the DB: 
 * @param l_offset output distance of the projection of the point on the polyline, from its first point if i_SHP_index > 0 or
 *                 from its last point if i_SHP_index < 0 
 * @return -999 no distance computed: far from the polyline
 *         -1   error
 *          0   minimum distance from p to the polyline
 */
double C_SHP::SHPdistancePointPolyline(SHPHandle  shpHandle, DBFHandle dbfHandle, IShape_STR *is, XYPoint_STR p, bool RightLeft, double *l_offset){
	SHPObject *shpObject;
	XYPoint_STR v1, v2;
	double distance, length, offset, segment_length; // working  variables
	double min_distance;     // minimum distance from p to the polyline
	int  i_v1, last_j;

	shpObject = SHPReadObject(shpHandle, is->index);
	if (shpObject == NULL){
		snprintf(buf, sizeof(buf), " cannot  read polyline  %ld ", is->index);
		error.fatal(buf, __FUNCTION__);
	}
	// if the point is far from the polyline return -999
	if (SHPpointOutsideBox(shpObject, is->index, p) > 0)  return -999;
	//
	min_distance = -1;
	v2.X = -1;
	*l_offset = 0;
	length = 0;
	segment_length = 0;
	// if the arc is one of a pair of arcs generated by a bi-directional road and the point is on the left of the arc
	// then we discard this arc, since the point is on the right of its companion !

	for (int ip = 0; ip < shpObject->nParts; ip++){
		// trick: i use the same block of instructions to evaluate poylines to be considered in the 
		//        forward or revers direction
		int first, last;
		first = shpObject->panPartStart[ip];
		if (ip < shpObject->nParts - 1)    last = shpObject->panPartStart[ip + 1] - 1;
		else                             last = shpObject->nVertices - 1;
		if (is->direction == 1) {
			i_v1 = first;
			last_j = last;
		}
		else{
			i_v1 = last;
			last_j = first;
		}
		for (int j = i_v1 + is->direction;; j += is->direction) {
			int i_v2 = j;
			//
			if (i_v1 != i_v2){
                            //  v1-v2 is a proper segment
                            v1.X = shpObject->padfX[i_v1]; v1.Y = shpObject->padfY[i_v1];
                            v2.X = shpObject->padfX[i_v2]; v2.Y = shpObject->padfY[i_v2];
                            // if the arc is one of a pair of arcs generated by a bi-directional road and the point is on the left of the arc
                            // then we discard this arc, since the point is on the right of its companion !
                            int side = SHPpointSide(v1, v2, p); // side < 0 right, side > 0 left
                            if (side == 0 || (side < 0 && RightLeft) || (side > 0 && !RightLeft)) {
                                if (SHP_isPointOnSegment(p, v1, v2, &offset, &distance) ) {
                                    segment_length = SHPplanarDistance2P(v1, v2);
                                    if (min_distance < 0 || distance < min_distance) {
                                        min_distance = distance;
                                        *l_offset = length + offset * segment_length;
                                    }
                                }

                            }
			}

			assert(segment_length >= 0);
			assert(v2.X >= 0);

			length += segment_length;
			v1 = v2; i_v1 = i_v2;
			if (j == last_j) break; // this is the only exit of the for (j)
		}
	}// for ip 
        SHPDestroyObject(shpObject);
	// NB the sum of the geometrical lengths of the parts  can be different from the lenght stored in the DB
	if (*l_offset > is->length) *l_offset = is->length;
	return min_distance;
}

/**
 * compute th distance of a point from an arc of v_SHP_Arcs_List
 * 
 * @param shpHandle input handle of the polyline shapefile
 * @param a input arc  to be analyzed
 * @param p input Point to be evaluated
 * @param RightLeft input true: serach an arc having p at its right, false: serach an arc having p at its left 
 * @param min_distance output minimum distance from p to the arc
 * @param l_offset output distance from the starting vertex of the arc to the the projection of the point on the arc
 * @return -999 no distance computed: far from the arc
 *         -1   error
 *          0   distance computed
 */
double C_SHP::SHPdistancePointArc(SHPHandle  shpHandle, DBFHandle dbfHandle, Arc_STR *a, XYPoint_STR p, bool RightLeft, double *l_offset){
	IShape_STR *is;
	double distance, min_distance, temp_l_offset;
	// double length;
	//
	min_distance = -999;
	*l_offset = 0;
	for (is = a->info.i_shp_first; is != NULL; is = is->next){

		// if the point is far from the polyline skip it
		if ((distance = SHPdistancePointPolyline(shpHandle, dbfHandle, is, p, RightLeft, &temp_l_offset)) < 0) continue;
		//
		if (min_distance < 0 || distance < min_distance) {
			min_distance = distance;
			*l_offset = temp_l_offset;
		}
	}
	return min_distance;
}

/**
 * finds the closest arc to point p and compute its projection on the arc
 * 
 * @param filename input name of the shape file without extension
 * @param p      input point to match on the graph
 * @param RightLeft input true: serach an arc having p at its right, false: serach an arc having p at its left 
 * @param *i_arc  output index of the arc with minimum distance in the Forward Star
 * @param *min_offset output percentage offset of the projection of p on the arc from it starting node
 * @return = 0 arc found; -1 error
 */
double C_SHP::SHP_Point2FS(XYPoint_STR p, bool RightLeft, int *i_arc, double *min_offset, C_IST *Ist){
	SHPHandle  shpHandle;
	DBFHandle  dbfHandle;
	double distance, min_distance, l_offset; // l_offset is a distanced, min_offset is a percentage
	Arc_STR *a;

	if (Ist->num_ArcsFS <= 0) return -1;
	shpHandle = SHPOpen(Ist->networkFileName, "rb");
	if (shpHandle == NULL){
		snprintf(buf, sizeof(buf), "opening  SHP file %s", Ist->networkFileName);
		error.fatal(buf, __FUNCTION__);
	}
	dbfHandle = DBFOpen(Ist->networkFileName, "rb");
	if (dbfHandle == NULL){
		snprintf(buf, sizeof(buf), "opening  DBF file %s", Ist->networkFileName);
		error.fatal(buf, __FUNCTION__);
	}
	min_distance = -1; *min_offset = 0;
	for (int k = 0; k < Ist->num_ArcsFS; k++){
		if (Ist->v_ArcsFS[k].original_Arc < 0) continue; // dummy arcs of the expanded graph
		a = &(Ist->v_SHP_Arcs_List[Ist->v_ArcsFS[k].original_Arc]);
		if ((distance = SHPdistancePointArc(shpHandle, dbfHandle, a, p, RightLeft, &l_offset)) >= 0) {
			if (min_distance < 0 || distance < min_distance) {
				*i_arc = k;
				min_distance = distance;
				*min_offset = l_offset / (double)a->info.length;
			}
		}
	}
	SHPClose(shpHandle);
	if (min_distance < 0) return -1;
	return 0;
}
/**
 * Given a set of stops, creates a shapefile conatining the  corresponding points
 * 
 * @param Instance input instance name
 * @return 
 */
int C_SHP::SHP_writeShapeStopPoints(char *Instance, C_IST *Ist){
	SHPHandle  shpHandleW;
	SHPObject shpObject;
	DBFHandle  dbfHandleW;
	char filename[100];

	shpObject.padfX = new double[1];
	shpObject.padfY = new double[1];
	shpObject.padfZ = new double[1];
	shpObject.padfM = new double[1];

	// open  output shapefiles
	snprintf(filename, sizeof(filename), "%s//%s_StopPoints", OUTPUTDIR, Instance);
	shpHandleW = SHPCreate(filename, SHPT_POINT);
	if (shpHandleW == NULL){
		snprintf(buf, sizeof(buf), "creating SHP file <%s>", filename);
		error.fatal(buf, __FUNCTION__);
	}
	dbfHandleW = DBFCreate(filename);
	if (dbfHandleW == NULL){
		snprintf(buf, sizeof(buf), "creating DBF file <%s>", filename);
		error.fatal(buf, __FUNCTION__);
	}
	DBFAddField(dbfHandleW, (char *) "id", FTInteger, 15, 0);
	DBFAddField(dbfHandleW, (char *) "Cod", FTString, CON_MAXNCODSTOPPOINT, 1);
	//
	for (int i = 0; i < Ist->num_StopPoints; i++){
		// write all the polylines associated with arc k
		shpObject.nSHPType = 1;
		shpObject.nVertices = 1;
		shpObject.nParts = 0;
		shpObject.padfX[0] = shpObject.dfXMin = shpObject.dfXMax = Ist->v_Points[Ist->v_StopPoints[i].i_Point].P.X;
		shpObject.padfY[0] = shpObject.dfYMin = shpObject.dfYMax = Ist->v_Points[Ist->v_StopPoints[i].i_Point].P.Y;
		shpObject.padfZ[0] = shpObject.padfM[0] = 0;
		if (SHPWriteObject(shpHandleW, -1, &shpObject) < 0){
			snprintf(buf, sizeof(buf), "writing point  N. %d in %s", i,filename);
			error.fatal(buf, __FUNCTION__);
		}
		if (DBFWriteIntegerAttribute(dbfHandleW, i, 0, i) < 0){
			snprintf(buf, sizeof(buf), "writing 'id' dbf point  N. %d in %s",i,filename);
			error.fatal(buf, __FUNCTION__);
		}
		if (DBFWriteStringAttribute(dbfHandleW, i, 1, (char *)Ist->v_StopPoints[i].Cod) < 0){
			snprintf(buf, sizeof(buf), "writing 'Cod' in dbf point  N. %d in %s", i, filename);
			error.fatal(buf, __FUNCTION__);
		}
	}// for i
	delete[] shpObject.padfX;
	delete[] shpObject.padfY;
	delete[] shpObject.padfZ;
	delete[] shpObject.padfM;
	SHPClose(shpHandleW);
	DBFClose(dbfHandleW);
	return 0;
}

/**
 * Given a set of Parking Points, creates a shapefile conatining the  corresponding points
 * 
 * @param Instance input instance name
 * @return 
 */
int C_SHP::SHP_writeShapeParkingPoints(char *Instance, C_IST *Ist){
	SHPHandle  shpHandleW;
	SHPObject shpObject;
	DBFHandle  dbfHandleW;
	char filename[100];

	shpObject.padfX = new double[1];
	shpObject.padfY = new double[1];
	shpObject.padfZ = new double[1];
	shpObject.padfM = new double[1];

	// open  output shapefiles
	snprintf(filename, sizeof(filename), "%s//%s_ParkingPoints", OUTPUTDIR, Instance);
	shpHandleW = SHPCreate(filename, SHPT_POINT);
	if (shpHandleW == NULL){
		snprintf(buf, sizeof(buf), "create a SHP file <%s>\n", filename);
		error.fatal(buf, __FUNCTION__);
	}
	dbfHandleW = DBFCreate(filename);
	if (dbfHandleW == NULL){
		snprintf(buf, sizeof(buf), "create a DBF file <%s>\n", filename);
		error.fatal(buf, __FUNCTION__);
	}
	DBFAddField(dbfHandleW, (char *) "id", FTInteger, 15, 0);
	DBFAddField(dbfHandleW, (char *) "Cod", FTString, CON_MAXNCODSTOPPOINT, 1);
	//
	for (int i = 0; i < Ist->num_ParkingPoints; i++){
		// write all the polylines associated with arc k
		shpObject.nSHPType = 1;
		shpObject.nVertices = 1;
		shpObject.nParts = 0;
		shpObject.padfX[0] = shpObject.dfXMin = shpObject.dfXMax = Ist->v_Points[Ist->v_ParkingPoints[i].i_Point].P.X;
		shpObject.padfY[0] = shpObject.dfYMin = shpObject.dfYMax = Ist->v_Points[Ist->v_ParkingPoints[i].i_Point].P.Y;
		shpObject.padfZ[0] = shpObject.padfM[0] = 0;
		if (SHPWriteObject(shpHandleW, -1, &shpObject) < 0){
			snprintf(buf, sizeof(buf), "writing point  N. %d in %s", i,filename);
			error.fatal(buf, __FUNCTION__);
		}
		if (DBFWriteIntegerAttribute(dbfHandleW, i, 0, i) < 0){
			snprintf(buf, sizeof(buf), " writing 'id' dbf point  N. %d in %s", i,filename);
			error.fatal(buf, __FUNCTION__);
		}
		if (DBFWriteStringAttribute(dbfHandleW, i, 1, (char *)Ist->v_ParkingPoints[i].Cod) < 0){
			snprintf(buf, sizeof(buf), "writing 'Cod' in dbf point  N. %d in %s", i, filename);
			error.fatal(buf, __FUNCTION__);

		}
	}// for i
	delete[] shpObject.padfX;
	delete[] shpObject.padfY;
	delete[] shpObject.padfZ;
	delete[] shpObject.padfM;
	SHPClose(shpHandleW);
	DBFClose(dbfHandleW);
	return 0;
}

/**
 * Given a set of waypoints from xxx_waypoints.csv  creates a shapefile conatining the  corresponding points
 * 
 * @param Instance  input instance name
 * @return 
 */
int C_SHP::SHP_writeShapeFromWaypoints(char *Instance){
	SHPHandle  shpHandleW;
	SHPObject  shpObject;
	DBFHandle  dbfHandleW;
	FILE *inp;
	long numRec;
	int i, numFields;
	unsigned char *p, word[CON_MAXNDESCRSTOPPOINT];
	static unsigned char line[512];
        char filename[100];
	XYPoint_STR P;
	register C_CSV myCSV;

	shpObject.padfX = new double[1];
	shpObject.padfY = new double[1];
	shpObject.padfZ = new double[1];
	shpObject.padfM = new double[1];

	// open  output shapefiles
	snprintf(filename, sizeof(filename), "%s//%s_Waypoint", OUTPUTDIR, Instance);
        shpHandleW = NULL;
	shpHandleW = SHPCreate(filename, SHPT_POINT);
	if (shpHandleW == NULL){
		snprintf(buf, sizeof(buf), "Error to create a SHP file <%s>\n", filename);
		error.fatal(buf, __FUNCTION__);
	}
	dbfHandleW = DBFCreate(filename);
	if (dbfHandleW == NULL){
		snprintf(buf, sizeof(buf), "Error to create a DBF file <%s>\n", filename);
		error.fatal(buf, __FUNCTION__);
	}
	DBFAddField(dbfHandleW, (char *) "id", FTInteger, 15, 0);
	DBFAddField(dbfHandleW, (char *) "Cod", FTString, CON_MAXNCODSTOPPOINT, 1);

	// Open ainput file
	snprintf(buf, sizeof(buf), "%s//%s_Waypoint.csv", OUTPUTDIR, Instance);
	inp = myCSV.CSV_ApriFile(buf, (char *) "r");
	// riposiziona il file in testa
	fseek(inp, 0, SEEK_SET);

	// Inizio lettura
	int shpRec = 0;
	numRec = 0;
	//myCSV.CSV_LeggiRec(inp, &numRec, (unsigned char *)line); // first record contains headings
	while (!feof(inp)){
		line[0] = '\0';
		myCSV.CSV_LeggiRec(inp, &numRec, (unsigned char *)line);
		if (line[0] == '\0') continue;
		p = &line[0];
		numFields = 0;
		for (i = 0; i < 5; i++) myCSV.CSV_readString(numRec, &numFields, &p, word);

		P.X = myCSV.CSV_readReal(numRec, &numFields, &p, 0, DBL_MAX);
		P.Y = myCSV.CSV_readReal(numRec, &numFields, &p, 0, DBL_MAX);
		// write the point 
		shpObject.nSHPType = 1;
		shpObject.nVertices = 1;
		shpObject.nParts = 0;
		shpObject.padfX[0] = shpObject.dfXMin = shpObject.dfXMax = P.X;
		shpObject.padfY[0] = shpObject.dfYMin = shpObject.dfYMax = P.Y;
		shpObject.padfZ[0] = shpObject.dfZMin = shpObject.dfZMax = 0;
                shpObject.padfM[0] = shpObject.dfMMin = shpObject.dfMMax = 0;
		if (SHPWriteObject(shpHandleW, -1, &shpObject) < 0){
			snprintf(buf, sizeof(buf), "writing point  N. %d in %s", i,filename);
			error.fatal(buf, __FUNCTION__);
		}
		if (DBFWriteIntegerAttribute(dbfHandleW, shpRec, 0, shpRec) < 0){
			snprintf(buf, sizeof(buf), " writing 'id' dbf point  N. %d in %s", shpRec, filename);
			error.fatal(buf, __FUNCTION__);
		}
		if (DBFWriteStringAttribute(dbfHandleW, shpRec, 1, (char *) "codice ") < 0){
			snprintf(buf, sizeof(buf), " writing 'Cod' in dbf point  N. %d in %s", shpRec,filename);
			error.fatal(buf, __FUNCTION__);
		}
                shpRec++;
	}// while
	delete[] shpObject.padfX;
	delete[] shpObject.padfY;
	delete[] shpObject.padfZ;
	delete[] shpObject.padfM;
	SHPClose(shpHandleW);
	DBFClose(dbfHandleW);
	fclose(inp);
	return 0;
}

/**
 * Given creates a shapefile conatining the   points of the Requests
 * 
 * @param Instance  input instance name
 * @return 
 */
int C_SHP::SHP_writeShapeRequests(char *Instance, C_IST *Ist){
	SHPHandle  shpHandleW;
	SHPObject  shpObject;
	DBFHandle  dbfHandleW;
        char filename[100];
	int i, iField;
	XYPoint_STR P;
	register C_CSV myCSV;


	shpObject.padfX = new double[1];
	shpObject.padfY = new double[1];
	shpObject.padfZ = new double[1];
	shpObject.padfM = new double[1];

	// open  output shapefiles
	snprintf(filename, sizeof(filename), "%s//%s_Requests", OUTPUTDIR, Instance);
	shpHandleW = SHPCreate(filename, SHPT_POINT);
	if (shpHandleW == NULL){
		snprintf(buf, sizeof(buf), "create a SHP file <%s>\n", filename);
		error.fatal(buf, __FUNCTION__);
	}
	dbfHandleW = DBFCreate(filename);
	if (dbfHandleW == NULL){
		snprintf(buf, sizeof(buf), "create a DBF file <%s>\n", filename);
		error.fatal(buf, __FUNCTION__);
	}
	DBFAddField(dbfHandleW, (char *) "id", FTInteger, 15, 0);
	DBFAddField(dbfHandleW, (char *) "Cod", FTString, CON_MAXNCODSTOPPOINT, 1);

	int shpRec = 0;
	shpObject.nSHPType = 1;
	shpObject.nVertices = 1;
	shpObject.nParts = 0;
	for (i = 0; i < Ist->num_Requests; i++){
		P = Ist->v_Points[Ist->v_StopPoints[Ist->v_Requests[i].i_pickup_stop].i_Point].P;
		// pickup
		shpObject.padfX[0] = shpObject.dfXMin = shpObject.dfXMax = P.X;
		shpObject.padfY[0] = shpObject.dfYMin = shpObject.dfYMax = P.Y;
		shpObject.padfZ[0] = shpObject.padfM[0] = 0;
		if (SHPWriteObject(shpHandleW, -1, &shpObject) < 0){
			snprintf(buf, sizeof(buf), "writing point  N. %d in %s", i, filename);
			error.fatal(buf, __FUNCTION__);
		}
		iField = 0;
		if (DBFWriteIntegerAttribute(dbfHandleW, shpRec, iField++, Ist->v_Requests[i].RequestId) < 0){
			snprintf(buf, sizeof(buf), " writing 'id' dbf point  N. %d in %s", 99,filename);
			error.fatal(buf, __FUNCTION__);
		}
		snprintf(buf, sizeof(buf), "Pick%d-%s", Ist->v_Requests[i].RequestId,
			Ist->v_StopPoints[Ist->v_Requests[i].i_pickup_stop].Cod);
		if (DBFWriteStringAttribute(dbfHandleW, shpRec++, iField++, buf) < 0){
			snprintf(buf, sizeof(buf), " writing P 'Cod' in dbf point  N. %d in %s", i,filename);
			error.fatal(buf, __FUNCTION__);
		}
		// dropoff
		P = Ist->v_Points[Ist->v_StopPoints[Ist->v_Requests[i].i_dropoff_stop].i_Point].P;
		shpObject.padfX[0] = shpObject.dfXMin = shpObject.dfXMax = P.X;
		shpObject.padfY[0] = shpObject.dfYMin = shpObject.dfYMax = P.Y;
		shpObject.padfZ[0] = shpObject.padfM[0] = 0;
		if (SHPWriteObject(shpHandleW, -1, &shpObject) < 0){
			snprintf(buf, sizeof(buf), "writing point  N. %d\n", i);
			error.fatal(buf, __FUNCTION__);
		}
		iField = 0;
		if (DBFWriteIntegerAttribute(dbfHandleW, shpRec, iField++, Ist->v_Requests[i].RequestId) < 0){
			snprintf(buf, sizeof(buf), " writing 'id' dbf point  N. %d in %s", 99,filename);
			error.fatal(buf, __FUNCTION__);
		}
		snprintf(buf, sizeof(buf), "Drop%d-%s", Ist->v_Requests[i].RequestId,
			Ist->v_StopPoints[Ist->v_Requests[i].i_dropoff_stop].Cod);
		if (DBFWriteStringAttribute(dbfHandleW, shpRec++, iField++, buf) < 0){
			snprintf(buf, sizeof(buf), " writing D 'Cod' in dbf point  N. %d in %s", i,filename);
			error.fatal(buf, __FUNCTION__);
		}
	}// while
	delete[] shpObject.padfX;
	delete[] shpObject.padfY;
	delete[] shpObject.padfZ;
	delete[] shpObject.padfM;
	SHPClose(shpHandleW);
	DBFClose(dbfHandleW);
	return 0;
}
