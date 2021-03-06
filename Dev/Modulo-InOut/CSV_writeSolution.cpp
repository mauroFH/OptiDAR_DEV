    #include "../Modulo-Main/darh.h"

/**
 * Writes the csv file containing the waypoints, and the corresponding shapefile
 * 
 * @param shapeFileName input name of the shapefile with the network
 * @param myG           input the Graph class
 * @param Instance      input name of the instance
 * @return 
 */
int C_CSV::CVS_writeWaypoints(C_SHP *mySHP, C_IST *Ist, char *Instance) {
	SHPHandle shpHandle;
	SHPObject *shpObject = NULL;
	XYPoint_STR v_stop, v_lastWritten, v1, v2;
	int shpType, numElements, i_arc, k_arc, i_stop;
	double padfMinBound[4], padfMaxBound[4];
	double dummy1, dummy2;
	Arc_STR *a;
	IShape_STR *is;
	ofstream fout;
	int SessionId = Ist->SessionID;
	int WaypointId = Ist->RouteID*CON_MAXWAYPOINTSROUTE + 1;
	int SolutionId = Ist->SolutionID;
	int VehicleId = Ist->TRoute.VehicleID;
	int PointOrder = 0;
	int RouteId = Ist->RouteID;
	int RoutePointId = Ist->RouteID*CON_MAXSTOPSROUTE + 0;

	v_lastWritten.X = v_lastWritten.Y = -1;

	// Open Shapefile
	shpHandle = SHPOpen(Ist->networkFileName, "rb");
	if (shpHandle == NULL) {
		snprintf(buf, sizeof(buf), "Error opening  SHP file %s", Ist->networkFileName);
		error.fatal(buf, __FUNCTION__);
	}
	fprintf(Ist->flog, "    Open Shapefile \n");
	fflush(Ist->flog);

	// Get general file info
	SHPGetInfo(shpHandle, &numElements, &shpType, padfMinBound, padfMaxBound);
	if (shpType != 3) {
		snprintf(buf, sizeof(buf), " - wrong shpfile type %d, mut be 3", shpType);
		error.fatal(buf, __FUNCTION__);
	}
	fprintf(Ist->flog, "    Get general file info \n");
	fflush(Ist->flog);

	// Open file
	snprintf(buf, sizeof(buf), "%s//%s_Waypoint.csv", OUTPUTDIR, Instance);
	fout.open(buf, ios::out);
	if (!fout.is_open())
	{
		snprintf(buf, sizeof(buf), "File cannot be opened : %s", buf);
		error.fatal(buf, __FUNCTION__);
	}
	fprintf(Ist->flog, "    Open CSV file \n");
	fflush(Ist->flog);

	// write headings
	// We remove it because CSV2DB does not manage the header
	//CSV_writeWaypointsHeader(&fout);
	//
	// first stop
	i_stop = 0;

	if (Ist->TRoute.v_stop_out[i_stop].StopOrPark == 0) // Stop Point
		v_stop = Ist->v_Points[Ist->v_StopPoints[Ist->TRoute.v_stop_out[i_stop].indexSoP].i_Point].P;
	else // parking point
		v_stop = Ist->v_Points[Ist->v_ParkingPoints[Ist->TRoute.v_stop_out[i_stop].indexSoP].i_Point].P;
	//
	bool FlagFirstStopDone = false;
	fprintf(Ist->flog, "    Number of Points: %d \n", Ist->TRoute.RArcs.npoints);
	fprintf(Ist->flog, "    Loop \n");
	fprintf(Ist->flog, "      Arc: ");
	fflush(Ist->flog);
	for (i_arc = 0; i_arc < Ist->TRoute.RArcs.npoints; i_arc++) {
		fprintf(Ist->flog, "(%d) ",i_arc);
		fflush(Ist->flog);
		//
		if (Ist->TRoute.RArcs.arc[i_arc] < 0) continue;
		k_arc = Ist->v_ArcsFS[Ist->TRoute.RArcs.arc[i_arc]].original_Arc;
		if (k_arc < 0) // dummy arc
			continue;
		a = &(Ist->v_SHP_Arcs_List[k_arc]);     // a is the 'oiginal' arc
		//
		v2.X = v2.Y = -1;
		for (is = a->info.i_shp_first; is != NULL; is = is->next) {    // an arc may contain several polylines
			//
			shpObject = SHPReadObject(shpHandle, (int)is->index);
			if (shpObject == NULL)
			{
				snprintf(buf, sizeof(buf), "Failed to read a shpObject of arc %lld", a->info.id);
				error.fatal(buf, __FUNCTION__);
			}
			// scan a polyline
			for (int ip = 0; ip < shpObject->nParts; ip++) {
				// a polyline may contain parts

				// trick: i use the same block of instructions to evaluate poylines to be considered in the 
				//        forward or revers direction            
				int first, last, i_v1, last_j;
				first = shpObject->panPartStart[ip];
				if (ip < shpObject->nParts - 1)  last = shpObject->panPartStart[ip + 1] - 1;
				else                             last = shpObject->nVertices - 1;
				if (is->direction == 1) {
					i_v1 = first;
					last_j = last;
				}
				else {
					i_v1 = last;
					last_j = first;
				}
				for (int j = i_v1 + is->direction;; j += is->direction) {
					// check part i: it may contain several segments
					int i_v2 = j;
					//
					if (i_v1 != i_v2) {
						//  v1-v2 is a proper segment
						v1.X = shpObject->padfX[i_v1];  v1.Y = shpObject->padfY[i_v1];   // starting point of the segment
						v2.X = shpObject->padfX[i_v2];  v2.Y = shpObject->padfY[i_v2]; // ending point of the segment
						//if (Ist->TRoute.v_stop_out[i_stop].indexArc != i_arc || !mySHP->SHP_isPointOnSegment(v_stop, v1, v2)){
						if (Ist->TRoute.v_stop_out[i_stop].indexArc != i_arc || !mySHP->SHP_isPointOnSegment(v_stop, v1, v2, &dummy1, &dummy2)) {
							//
							// the stop is not on this arc 
							// ** OR **
							// it is on the arc but not on the segment : write the first vertex of the segment
							//
							if (FlagFirstStopDone) { // if the first stop has not been printed do not print thi segment
								if (!mySHP->SHP_PointsCoincide(v_lastWritten, v1)) {
									// write v
									CSV_writeWaypointsRow(&fout, WaypointId, SessionId, SolutionId, VehicleId, PointOrder, -1, RouteId, v1, a->info.id);
									WaypointId++; v_lastWritten = v1;
								}
								if (!mySHP->SHP_PointsCoincide(v_lastWritten, v2)) {
									// write v
									CSV_writeWaypointsRow(&fout, WaypointId, SessionId, SolutionId, VehicleId, PointOrder, -1, RouteId, v2, a->info.id);
									WaypointId++; v_lastWritten = v2;
								}
							}
						}
						else {
							//
							// the stop is on this segment
							//
							if (FlagFirstStopDone && !mySHP->SHP_PointsCoincide(v1, v_stop)) {
								// write v
								if (!mySHP->SHP_PointsCoincide(v_lastWritten, v1)) {
									CSV_writeWaypointsRow(&fout, WaypointId, SessionId, SolutionId, VehicleId, PointOrder, -1, RouteId, v1, a->info.id);
									WaypointId++; v_lastWritten = v1;
								}
							}
							do {
								// write the stop
								FlagFirstStopDone = true;
								CSV_writeWaypointsRow(&fout, WaypointId, SessionId, SolutionId, VehicleId, PointOrder, RoutePointId, RouteId, v_stop, a->info.id);
								PointOrder++; RoutePointId++; WaypointId++; v_lastWritten = v_stop;
								i_stop++;
								if (i_stop >= Ist->TRoute.nstop)   // all stops written 
									goto terminate;
								// next stop
								if (Ist->TRoute.v_stop_out[i_stop].StopOrPark == 0) // Stop Point
									v_stop = Ist->v_Points[Ist->v_StopPoints[Ist->TRoute.v_stop_out[i_stop].indexSoP].i_Point].P;
								else // parking point
									v_stop = Ist->v_Points[Ist->v_ParkingPoints[Ist->TRoute.v_stop_out[i_stop].indexSoP].i_Point].P;
							} while (mySHP->SHP_isPointOnSegment(v_stop, v1, v2, &dummy1, &dummy2));
							//while (mySHP->SHP_isPointOnSegment(v_stop, v1, v2));
							// i_stop is not on segment (v1,v2)
							if (!mySHP->SHP_PointsCoincide(v_lastWritten, v2)) {
								CSV_writeWaypointsRow(&fout, WaypointId, SessionId, SolutionId, VehicleId, PointOrder, -1, RouteId, v2, a->info.id);
								WaypointId++; v_lastWritten = v2;  // PointOrder++; 
							}
						} // end if (isOnSegment)
					}// if i_v1 != i_v2
					v1 = v2; i_v1 = i_v2;
					if (j == last_j) break; // this is the only exit of the for (j)                  
				} //for j
			}// ip
			SHPDestroyObject(shpObject);//if (shpObject != NULL) { delete[] shpObject;  shpObject = NULL;}
			shpObject = NULL;
		}// for is
	}// for i_arc

terminate:

	fprintf(Ist->flog, " \n    Loop Ended!\n");
	fflush(Ist->flog);

	// Free memory
	if (shpObject != NULL) 
		SHPDestroyObject(shpObject); //{   free(shpObject); shpObject = NULL;   }

	// Close files
	fout.close();
	SHPClose(shpHandle);

	return 0;
}

void C_CSV::CSV_writeWaypointsHeader(ofstream *fout){
	static char SEP = CON_CSVFILE_SEPARATOR;

	*fout << "WaypointId" << SEP << "SolutionId" << SEP << "Routeid" << SEP << "PointOrder" << SEP << "RoutePointId" << SEP
		<< "Latitude" << SEP << "Longitude" << SEP << "FeatureId" << "\n";
	(*fout).flush();
}

void C_CSV::CSV_writeWaypointsRow(ofstream *fout, int WaypointId, int SessionId, int SolutionId, int Vehicleid, int PointOrder,
	int RoutePointId, int RouteId, XYPoint_STR v, long long FeatureId){
	static char SEP = CON_CSVFILE_SEPARATOR;

	if (RoutePointId >= 0)
	{
		*fout << WaypointId << SEP << SolutionId << SEP << RouteId << SEP << PointOrder << SEP << RoutePointId << SEP
			<< setprecision(15) << v.X << SEP << setprecision(15) << v.Y << SEP << setprecision(15) << FeatureId << "\n";
		(*fout).flush();
	}
	else
	{
		*fout << WaypointId << SEP << SolutionId << SEP << RouteId << SEP << PointOrder << SEP << "" << SEP
			<< setprecision(15) << v.X << SEP << setprecision(15) << v.Y << SEP << setprecision(15) << FeatureId << "\n";
	}

	(*fout).flush();

}
/*
 int CSV::CSVwriteWaypointsLasttArc( SHPHandle shpHandle, ofstream fout, Point_STR *lastP, C_IST *Ist){
}
*/
void C_CSV::CVS_writePath(C_SHP *mySHP, C_IST *Ist, char *Instance) {
	int i, i_stop, id;
	long narcs, *arc;
	int *id_spezzone;

	narcs = Ist->TRoute.RArcs.npoints - 1;
	arc = new long[narcs];
	id_spezzone = new int[narcs];
	if (arc == NULL || id_spezzone == NULL)
		error.fatal((char *) " allocationg arc or id_spezzone ", __FUNCTION__);
	//
	i_stop = 0; id = 0;
	for (i = 0; i < narcs; i++) {
		arc[i] = Ist->TRoute.RArcs.arc[i];
		id_spezzone[i] = id;
		if (i_stop < Ist->TRoute.nstop - 1 && Ist->TRoute.v_stop_out[i_stop + 1].indexArc <= i) {
			id++; i_stop++;
		}
	}
	mySHP->SHP_writeShapeFromPath(Instance, narcs, arc, id_spezzone, Ist);
	//
	delete[] arc;
}

/**
* Write output CSV file "Route"
* @param Ist istance data
* @param Instance  input name of the instance
* @return
*/
void C_CSV::CVS_writeRoute(C_IST *Ist, char *Instance){
	ofstream fout;
	int RouteId = Ist->RouteID;
	int SolutionId = Ist->SolutionID;
	int VehicleId = Ist->v_Vehicles[Ist->TRoute.VehicleID].VehicleId;
	int RouteNumber = 1;

	// Open file
	snprintf(buf, sizeof(buf), "%s//%s_Route.csv", OUTPUTDIR, Instance);
	fout.open(buf, ios::out);
	if (!fout.is_open())
	{
		snprintf(buf, sizeof(buf), "File cannot be opened %s", buf);
		error.fatal(buf, __FUNCTION__);
	}
	static char SEP = CON_CSVFILE_SEPARATOR;

	// Removed because the CSV2DB does not manage the header 
	//fout  << "RouteId" << SEP << "SolutionId" << SEP <<  "RouteNumber" << SEP << "Vehicleid" << "\n"  ;
	//(fout).flush();

	//
	// Route ID
	fout << RouteId << SEP;
	//fout << Ist->SessionID << SEP;
	fout << SolutionId << SEP;
	fout << RouteNumber << SEP;
	fout << VehicleId << '\n';
	fout.flush();

	// Close file
	fout.close();

}

/**
* Write output CSV file "RoutePoint"
* @param Ist istance data
* @param Instance  input name of the instance
* @return
*/
void C_CSV::CVS_writeRoutePoint(C_IST *Ist, char *Instance){
	int i;
	ofstream fout;
	int RouteId = Ist->RouteID;
	int RoutePointId = RouteId * CON_MAXSTOPSROUTE + 1;
	int SolutionId = Ist->SolutionID;
	int PointOrder = 1;

	// Open file
	snprintf(buf, sizeof(buf), "%s//%s_RoutePoint.csv", OUTPUTDIR, Instance);
	fout.open(buf, ios::out);
	if (!fout.is_open())
	{
		snprintf(buf, sizeof(buf), "File cannot be opened %s", buf);
		error.fatal(buf, __FUNCTION__);
	}
	static char SEP = CON_CSVFILE_SEPARATOR;

	// Removed because the CSV2DB does not manage the header 
	//fout  << "RoutePointId" << SEP << "SolutionId" << SEP <<  "RouteId" << SEP <<"PointOrder" 
	//<< SEP << "RouteActionType" 
	//<< SEP << "EarliestTime" << SEP << "LatestTime" << SEP << "ArriveTime" << SEP << "DepartureTime"     
	//<< SEP << "Latitude" << SEP << "Longitude" << SEP << "PointId" 
	//<< SEP << "RelativeDistance" << "\n"  ;
	//(fout).flush();

	long LastDistance = 0;
	//
	for (i = 0; i < Ist->TRoute.nstop; i++) {
		// Route ID
		fout << RoutePointId++ << SEP;
		//fout << Ist->SessionID << SEP;
		fout << SolutionId << SEP;
		fout << RouteId << SEP;
		fout << PointOrder++ << SEP;
		// RouteActionType
		if (Ist->TRoute.v_stop_out[i].StopOrPark == 0) {
			fout << "Stop" << SEP;
		}
		else{
			if (Ist->v_ParkingPoints[Ist->TRoute.v_stop_out[i].indexSoP].isDepot)
				fout << "Depot" << SEP;
			else
				fout << "Parking" << SEP;
		}
		//ErliestTime and LatestTime
		switch (Ist->TRoute.v_stop_out[i].type) {
		case 1: // pickup
			fout << Ist->v_Requests[Ist->TRoute.v_stop_out[i].indexReq].pickup.StartTime << SEP
				<< Ist->v_Requests[Ist->TRoute.v_stop_out[i].indexReq].pickup.EndTime << SEP;
			break;
		case 2: // dropoff
			fout << Ist->v_Requests[Ist->TRoute.v_stop_out[i].indexReq].dropoff.StartTime << SEP
				<< Ist->v_Requests[Ist->TRoute.v_stop_out[i].indexReq].dropoff.EndTime << SEP;
			break;
		default:
			fout << 0 << SEP << CON_LASTTIME << SEP;
		}
		// ArriveTime
		fout << Ist->TRoute.v_stop_out[i].atime << SEP;
		// DepartureTime
		fout << Ist->TRoute.v_stop_out[i].dtime << SEP;
		// Latitude and Longitude
		if (Ist->TRoute.v_stop_out[i].StopOrPark == 0) {// Stop
			fout << Ist->v_Points[Ist->v_StopPoints[Ist->TRoute.v_stop_out[i].indexSoP].i_Point].P.X << SEP
				<< Ist->v_Points[Ist->v_StopPoints[Ist->TRoute.v_stop_out[i].indexSoP].i_Point].P.Y << SEP;
		}
		else{
			// parking
			fout << setprecision(15) << Ist->v_Points[Ist->v_ParkingPoints[Ist->TRoute.v_stop_out[i].indexSoP].i_Point].P.X << SEP
				<< setprecision(15) << Ist->v_Points[Ist->v_ParkingPoints[Ist->TRoute.v_stop_out[i].indexSoP].i_Point].P.Y << SEP;
		}
		// Point ID
		if (Ist->TRoute.v_stop_out[i].StopOrPark == 0) {// Stop
			fout << Ist->v_Points[Ist->v_StopPoints[Ist->TRoute.v_stop_out[i].indexSoP].i_Point].PointId << SEP;
		}
		else{
			// parking
			fout << Ist->v_Points[Ist->v_ParkingPoints[Ist->TRoute.v_stop_out[i].indexSoP].i_Point].PointId << SEP;
		}
		// RelativeDistance        
		fout << setprecision(2) << fixed << (float)(Ist->TRoute.v_stop_out[i].distance - LastDistance) << "\n";
		//
		LastDistance = Ist->TRoute.v_stop_out[i].distance;
		//
		fout.flush();
	}

	// Close file
	fout.close();

}

/**
* Write output CSV file "Route"
* @param Ist istance data
* @param Instance  input name of the instance
* @return
*/
void C_CSV::CVS_writeRouteOld(C_IST *Ist, char *Instance){
	int i;
	ofstream fout;
	int RouteId = Ist->RouteID;
	int SolutionId = Ist->SolutionID;
	int VehicleId = Ist->TRoute.VehicleID;
	int PointOrder = 0;

	// Open file
	snprintf(buf, sizeof(buf), "%s//%s_Route.csv", OUTPUTDIR, Instance);
	fout.open(buf, ios::out);
	if (!fout.is_open())
	{
		snprintf(buf, sizeof(buf), "File cannot be opened %s", buf);
		error.fatal(buf, __FUNCTION__);
	}
	static char SEP = CON_CSVFILE_SEPARATOR;

	// Removed because the CSV2DB does not manage the header 
	//fout  << "RouteId" << SEP << "SolutionId" << SEP << "Vehicleid" << SEP << "PointOrder" << SEP << "RouteActionType" 
	//<< SEP << "EarliestTime" << SEP << "LatestTime" << SEP << "ArriveTime" << SEP << "DepartureTime"     
	//<< SEP << "Latitude" << SEP << "Longitude" << SEP << "RequestAddressId" << SEP << "PointId" 
	//<< SEP << "RelativeDistance" << SEP << "\n"  ;
	//(fout).flush();

	long LastDistance = 0;
	//
	for (i = 0; i < Ist->TRoute.nstop; i++) {
		// Route ID
		fout << RouteId << SEP;
		//fout << Ist->SessionID << SEP;
		fout << SolutionId << SEP;
		fout << VehicleId << SEP;
		fout << PointOrder++ << SEP;
		// RouteActionType
		if (Ist->TRoute.v_stop_out[i].StopOrPark == 0) {
			fout << "Stop" << SEP;
		}
		else{
			if (Ist->v_ParkingPoints[Ist->TRoute.v_stop_out[i].indexSoP].isDepot)
				fout << "Depot" << SEP;
			else
				fout << "Parking" << SEP;
		}
		//ErliestTime and LatestTime
		switch (Ist->TRoute.v_stop_out[i].type) {
		case 1: // pickup
			fout << Ist->v_Requests[Ist->TRoute.v_stop_out[i].indexReq].pickup.StartTime << SEP
				<< Ist->v_Requests[Ist->TRoute.v_stop_out[i].indexReq].pickup.EndTime << SEP;
			break;
		case 2: // dropoff
			fout << Ist->v_Requests[Ist->TRoute.v_stop_out[i].indexReq].dropoff.StartTime << SEP
				<< Ist->v_Requests[Ist->TRoute.v_stop_out[i].indexReq].dropoff.EndTime << SEP;
			break;
		default:
			fout << 0 << SEP << CON_LASTTIME << SEP;
		}
		// ArriveTime
		fout << Ist->TRoute.v_stop_out[i].atime << SEP;
		// DepartureTime
		fout << Ist->TRoute.v_stop_out[i].dtime << SEP;
		// Latitude and Longitude
		if (Ist->TRoute.v_stop_out[i].StopOrPark == 0) {// Stop
			fout << Ist->v_Points[Ist->v_StopPoints[Ist->TRoute.v_stop_out[i].indexSoP].i_Point].P.X << SEP
				<< Ist->v_Points[Ist->v_StopPoints[Ist->TRoute.v_stop_out[i].indexSoP].i_Point].P.Y << SEP;
		}
		else{
			// parking
			fout << setprecision(15) << Ist->v_Points[Ist->v_ParkingPoints[Ist->TRoute.v_stop_out[i].indexSoP].i_Point].P.X << SEP
				<< setprecision(15) << Ist->v_Points[Ist->v_ParkingPoints[Ist->TRoute.v_stop_out[i].indexSoP].i_Point].P.Y << SEP;
		}
		// RequestAddress ID
		switch (Ist->TRoute.v_stop_out[i].type) {
		case 1: // pickup
			fout << Ist->v_Requests[Ist->TRoute.v_stop_out[i].indexReq].pickup.RequestAddressId << SEP;
			break;
		case 2: // dropoff
			fout << Ist->v_Requests[Ist->TRoute.v_stop_out[i].indexReq].pickup.RequestAddressId << SEP;
			break;
		default:
			fout << -1 << SEP;
		}
		// Point ID
		if (Ist->TRoute.v_stop_out[i].StopOrPark == 0) {// Stop
			fout << Ist->v_Points[Ist->v_StopPoints[Ist->TRoute.v_stop_out[i].indexSoP].i_Point].PointId << SEP;
		}
		else{
			// parking
			fout << Ist->v_Points[Ist->v_ParkingPoints[Ist->TRoute.v_stop_out[i].indexSoP].i_Point].PointId << SEP;
		}
		// RelativeDistance        
		//fout << setprecision(2) << fixed << (float)(Ist->TRoute.v_stop_out[i].distance - LastDistance) << SEP << "\n";          
		fout << setprecision(2) << fixed << (float)(Ist->TRoute.v_stop_out[i].distance - LastDistance) << "\n";
		//
		LastDistance = Ist->TRoute.v_stop_out[i].distance;
		//
		fout.flush();
	}

	// Close file
	fout.close();

}

/**
* Write output CSV file "RequestInRoute"
* @param Ist istance data
* @param Instance  input name of the instance
* @return
*/
void C_CSV::CVS_writeRequestInRoute(C_IST *Ist, char *Instance){
	int i;
	ofstream fout;
	int RouteId = Ist->RouteID;
	int RoutePointId = RouteId * CON_MAXSTOPSROUTE + 1;
	int RequestInRouteId = RoutePointId;
	int SolutionId = Ist->SolutionID;

	// Open file
	snprintf(buf, sizeof(buf), "%s//%s_RequestInRoute.csv", OUTPUTDIR, Instance);
	fout.open(buf, ios::out);
	if (!fout.is_open())
	{
		snprintf(buf, sizeof(buf), "File cannot be opened %s", buf);
		error.fatal(buf, __FUNCTION__);
	}
	static char SEP = CON_CSVFILE_SEPARATOR;

	// Removed because the CSV2DB does not manage the header 
	//fout  << "RequestInRouteId" << SEP << "RoutePointId" << SEP << "SolutionId" << SEP <<  "RequestId"  
	//<< SEP << "ActionType" << "\n"  ;
	//(fout).flush();

	//
	for (i = 0; i < Ist->TRoute.nstop; i++) {

		if (Ist->TRoute.v_stop_out[i].StopOrPark != 0)
		{
			RoutePointId++;
			continue;
		}

		// Route ID
		fout << RequestInRouteId++ << SEP;
		fout << RoutePointId++ << SEP;
		//fout << Ist->SessionID << SEP;
		fout << SolutionId << SEP;
		fout << Ist->TRoute.v_stop_out[i].indexReq + 1 << SEP;  // 
		// RouteActionType
		if (Ist->TRoute.v_stop_out[i].StopOrPark == 0) {
			fout << "Stop";
		}
		else{
			if (Ist->v_ParkingPoints[Ist->TRoute.v_stop_out[i].indexSoP].isDepot)
				fout << "Depot";
			else
				fout << "Parking";
		}
		//
		fout << '\n';

		fout.flush();
	}

	// Close file
	fout.close();

}

/**
* Write output CSV file "Solution"    
*/
void C_CSV::CVS_writeSolution(void){

	ofstream fout;
	char buf1[CON_MAXNSTR];

	// Read the CSV file containing the soltion computed by the optimizer
	snprintf(buf1, sizeof(buf1), "ISTANZA%d", error.SessionID);

	// Open file
	snprintf(buf, sizeof(buf), "%s//%s_Solution.csv", OUTPUTDIR, buf1);
	fout.open(buf, ios::out);
	if (!fout.is_open())
	{
		snprintf(buf, sizeof(buf), "File cannot be opened %s", buf);
		error.fatal(buf, __FUNCTION__);
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

/**
* ReWrite CSV file "Setup"
* @param Ist istance data
* @param Instance  input name of the instance
* @return
*/
int C_CSV::CVS_writeSetup(C_IST *Ist)
{
	ofstream fout;

	// Open file
	snprintf(buf, sizeof(buf), "%s//Setup.csv", INPUTDIR);
	fout.open(buf, ios::out);
	if (!fout.is_open())
	{
		snprintf(buf, sizeof(buf), "File cannot be opened %s", buf);
		error.fatal(buf, __FUNCTION__);
	}
	static char SEP = CON_CSVFILE_SEPARATOR;

	// Header 
	fout << "Parameter" << SEP << "Value" << "\n";
	(fout).flush();

	// Route ID
	fout << "SolutionId" << SEP << Ist->SolutionID + 1 << "\n";
	fout << "RouteId" << SEP << Ist->RouteID + 1 << "\n";
	fout.flush();

	// Close file
	fout.close();

	return 0;

}