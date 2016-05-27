//
// managing the graph
//

#include "../Modulo-Main/darh.h"
//

/**
 * Builds the expanded Forward Star starting from the arcs in Ist->v_SHP_Arcs_List
 * @return 
 */
int C_SHP::SHP_buildFS(C_IST *Ist){
	char   NomeProc[] = "C_SHP::SHPbuildFS";
	/** computes number of arcs entering  (#in) and outgoing (#out) from each vertex i
	 * compute the list of arcs entering end exiting from each vertex.
	 * Each vertex i is expanded in #in + #out vertices (let us call in-vettices and out-vertices) and
	 * #in X #out - v_num_violated[i] arcs among these vertices
	 *
	 */
	int i, k, kk;
	int *v_first_in, *v_first_out, *v_next_in, *v_next_out, *v_num_in, *v_num_out, *v_new_vertex_to;
	/** v_first_in[i]  =  FIRST ARC OUT OF NODE i
	 * v_first_out[k] =  NEXT ARC OUT OF THE STARTING NODE OF ARC k; -1 if it is the last
	 * v_next_in[i]   =  FIRST ARC INTO NODE i,
	 * v_next_out[k]  =  NEXT ARC INTO THE ENDING NODE OF ARC k; -1 if it is the last
	 * v_next_vertex_to[k] = index of the vertex of the expanded graph in which arrives original arc k
	 */
	//CError error;

	v_first_in = new int[Ist->SHP_num_Vertices + 1];
	v_first_out = new int[Ist->SHP_num_Vertices + 1];
	v_next_in = new int[Ist->SHP_num_Arcs];
	v_next_out = new int[Ist->SHP_num_Arcs];
	v_num_in = new int[Ist->SHP_num_Vertices];
	v_num_out = new int[Ist->SHP_num_Vertices];
	v_new_vertex_to = new int[Ist->SHP_num_Arcs];

	if (v_first_in == NULL || v_first_out == NULL || v_next_in == NULL || v_next_out == NULL || v_new_vertex_to == NULL ||
		v_num_in == NULL || v_num_out == NULL) {
		error.fatal("cannot allocate first_in, first_out, next_in, next_out, num_in, num_out new_vertex_to ", __FUNCTION__);
		return -1;
	}
	SHPbuildInOutLists(v_first_in, v_first_out, v_next_in, v_next_out, v_num_in, v_num_out, Ist);

	//
	Ist->num_VerticesFS = 0;
	for (i = 0; i < Ist->SHP_num_Vertices; i++) {
		Ist->num_VerticesFS += (v_num_in[i]) + (v_num_out[i]);
	}
	Ist->v_fromFS = new int[Ist->num_VerticesFS + 1];
	if (Ist->v_fromFS == NULL) {
		error.fatal(" Ist->v_fromFS ", __FUNCTION__);
		return -1;
	}
	Ist->v_original_Vertex = new int[Ist->num_VerticesFS];
	if (Ist->v_original_Vertex == NULL){
		error.fatal("cannot allocate v_original_xxx ", __FUNCTION__);
		return -1;
	}
	Ist->num_ArcsFS = 0;;
	/* the number of arcs computed here is an upper bound since we do not subtract the missing arcs due to the frobidden turns */
	for (i = 0; i < Ist->SHP_num_Vertices; i++) Ist->num_ArcsFS += (v_num_in[i]) * (v_num_out[i]);
	Ist->num_ArcsFS += Ist->SHP_num_Arcs;

	Ist->v_ArcsFS = new arc_FS_STR[Ist->num_ArcsFS];
	if (Ist->v_ArcsFS == NULL) {
		error.fatal(" cannot allocate Ist->v_ArcsFS ", __FUNCTION__);
		return -1;
	}
	// initialize v_new_vertex_to 
	for (k = 0; k < Ist->SHP_num_Arcs; k++)   v_new_vertex_to[k] = Ist->v_SHP_Arcs_List[k].to;
	//
	Ist->num_VerticesFS = Ist->num_ArcsFS = 0;
	// expand one vertex at a time: vertex i belongs to the original (small) graph
	int counter = 0;
	for (i = 0; i < Ist->SHP_num_Vertices; i++){
		for (k = v_first_in[i]; k >= 0; k = v_next_in[k]) {
			Ist->v_original_Vertex[Ist->num_VerticesFS + counter] = i;
			v_new_vertex_to[k] = Ist->num_VerticesFS + counter++;
		}
		for (k = v_first_out[i]; k >= 0; k = v_next_out[k]) {
			Ist->v_original_Vertex[Ist->num_VerticesFS + counter++] = i;
		}
	}
	// generate arcs
	for (i = 0; i < Ist->SHP_num_Vertices; i++){
		/* generates internal arcs from in-vertices to out-vertices*/
		int counter_in = 0;
		for (k = v_first_in[i]; k >= 0; k = v_next_in[k]) {
			Ist->v_fromFS[Ist->num_VerticesFS + counter_in] = Ist->num_ArcsFS;
			int counter_out;
			for (kk = v_first_out[i], counter_out = 0; kk >= 0; kk = v_next_out[kk], counter_out++) {
				//if (Ist->v_SHP_Arcs_List[k].from == Ist->v_SHP_Arcs_List[kk].to) continue; // avoid U turn
				if (SHPsameShape(&Ist->v_SHP_Arcs_List[k], &Ist->v_SHP_Arcs_List[kk])) continue; // avoid U turn
				if (SHPturnForbidden(&Ist->v_SHP_Arcs_List[k], &Ist->v_SHP_Arcs_List[kk])) continue; // no turn from k to kk 
				Ist->v_ArcsFS[Ist->num_ArcsFS].to = Ist->num_VerticesFS + v_num_in[i] + counter_out;
				Ist->v_ArcsFS[Ist->num_ArcsFS].from = Ist->num_VerticesFS + counter_in;
				Ist->v_ArcsFS[Ist->num_ArcsFS].length = 0;
				Ist->v_ArcsFS[Ist->num_ArcsFS].minutes = 0;
				Ist->v_ArcsFS[Ist->num_ArcsFS++].original_Arc = -1;
			}
			counter_in++;
		}

		int counter_out = 0;
		/* generates arcs from out-vertices to vertices differnt from i, ( i in the small graph) */
		for (k = v_first_out[i]; k >= 0; k = v_next_out[k]) {
			Ist->v_fromFS[Ist->num_VerticesFS + v_num_in[i] + counter_out] = Ist->num_ArcsFS;
			Ist->v_ArcsFS[Ist->num_ArcsFS].from = Ist->num_VerticesFS + v_num_in[i] + counter_out;
			counter_out++;
			Ist->v_ArcsFS[Ist->num_ArcsFS].to = v_new_vertex_to[k];
			Ist->v_ArcsFS[Ist->num_ArcsFS].length = Ist->v_SHP_Arcs_List[k].info.length;
			Ist->v_ArcsFS[Ist->num_ArcsFS].minutes = Ist->v_SHP_Arcs_List[k].info.minutes;
			Ist->v_ArcsFS[Ist->num_ArcsFS++].original_Arc = k;
		}
		Ist->num_VerticesFS += v_num_in[i] + v_num_out[i];
	}// for i
	Ist->v_fromFS[Ist->num_VerticesFS] = Ist->num_ArcsFS;
	delete[] v_first_in;
	delete[] v_first_out;
	delete[] v_next_in;
	delete[] v_next_out;
	delete[] v_num_in;
	delete[] v_num_out;
	delete[] v_new_vertex_to;
	return 0;
}

/** check if arcs a1 and a2  corrspond to a same shape
 * @param a1 input fisrt arc
 * @param a2 input second arc
 */
int C_SHP::SHPsameShape(Arc_STR *a1, Arc_STR *a2){
	char   NomeProc[] = "C_SHP::SHPsameShape";
	IShape_STR *is1, *is2;
	for (is1 = a1->info.i_shp_first; is1 != NULL; is1 = is1->next)
		for (is2 = a2->info.i_shp_first; is2 != NULL; is2 = is2->next)
			if (abs(is1->index) == abs(is2->index)) return 1;
	return 0;
}

/**
 * 
 * @param v_first_in = output FIRST ARC OUT OF NODE i 
 * @param v_first_out = output NEXT ARC OUT OF THE STARTING NODE OF ARC j
 * @param v_next_in = output FIRST ARC INTO NODE i,
 * @param v_next_out = output NEXT ARC INTO THE ENDING NODE OF ARC j
 * @param v_num_in = output # of arcs entering  node i
 * @param v_num_out = output # of arcs emanating from node i
 * @return 
 */
int C_SHP::SHPbuildInOutLists(int *v_first_in, int *v_first_out, int *v_next_in, int *v_next_out, int *v_num_in, int *v_num_out,
	C_IST *Ist){
	char   NomeProc[] = "C_SHP::SHPbuildInOutLists";
	/*
	*/
	int i, j, k;
	int *v_last_in, *v_last_out;
	//CError error;

	v_last_in = new int[Ist->SHP_num_Vertices];
	v_last_out = new int[Ist->SHP_num_Vertices];

	if (v_last_in == NULL || v_last_out == NULL) {
		error.fatal("cannot allocate temp_in temp_out ", __FUNCTION__);
		return -1;
	}
	for (i = 0; i < Ist->SHP_num_Vertices; i++)
		v_first_in[i] = v_first_out[i] = v_last_in[i] = v_last_out[i] = -1;
	for (i = 0; i < Ist->SHP_num_Vertices; i++)
		v_num_in[i] = v_num_out[i] = 0;
	//
	for (k = 0; k < Ist->SHP_num_Arcs; k++){
		v_next_in[k] = -1;
		v_next_out[k] = -1;
		i = Ist->v_SHP_Arcs_List[k].from;
		j = Ist->v_SHP_Arcs_List[k].to;
		v_num_out[i]++;
		v_num_in[j]++;
		if (v_first_out[i] >= 0)
			v_next_out[v_last_out[i]] = k;
		else
			v_first_out[i] = k;
		v_last_out[i] = k;
		if (v_first_in[j] >= 0)
			v_next_in[v_last_in[j]] = k;
		else
			v_first_in[j] = k;
		v_last_in[j] = k;
	}
	v_first_out[Ist->SHP_num_Vertices] = Ist->SHP_num_Arcs;
	v_first_in[Ist->SHP_num_Vertices] = Ist->SHP_num_Arcs;
	delete[] v_last_in;
	delete[] v_last_out;
	return 0;
}

//
void C_SHP::SHP_printlists(C_IST *Ist){
	flog << fixed;
	//int i;

	flog << "----\n";
	flog << setw(9) << "vertex" << " " << setw(9) << "X" << ", " << setw(9) << "Y" << "\n";
	for (int i = 0; i < Ist->SHP_num_Vertices; i++) {
		flog << setw(9) << i << " " << setw(9) << Ist->v_SHP_Vertices_List[i].X << ", " << setw(9) << Ist->v_SHP_Vertices_List[i].Y << "\n";
	}
	flog << "\n";
	flog << setw(18) << "Arc" << " " << setw(9) << "from" << ", " << setw(9) << "to" << setw(7) << "length" << " " << setw(8) << "minutes" << "\n";
	for (int i = 0; i < Ist->SHP_num_Arcs; i++) {
		flog << setw(3) << i << " " << setw(15) << Ist->v_SHP_Arcs_List[i].info.id
			<< setw(9) << Ist->v_SHP_Arcs_List[i].from << ", " << setw(4) << Ist->v_SHP_Arcs_List[i].fromold
			<< " " << setw(9) << Ist->v_SHP_Arcs_List[i].to << ", " << setw(4) << Ist->v_SHP_Arcs_List[i].toold
			<< setw(7) << Ist->v_SHP_Arcs_List[i].info.length << " " << setw(8) << Ist->v_SHP_Arcs_List[i].info.minutes << "\n";
	}
}

void C_SHP::SHP_printFS(C_IST *Ist){
    //int i;
    flog << fixed;
    
    flog << "----F O R W A R D     S T A R\n";
    flog <<setw(6)<< "new v" << " "<<setw(6) << "original v"<< "\n";   
    for (int i=0; i < Ist->num_VerticesFS; i++) {
        flog <<setw(6)<< i << " "<<setw(6) << Ist->v_original_Vertex[i]<< "\n";
    }
    flog << "\n";
    flog <<setw(3)<< "arc"<< " "<< setw(15) << "id"<< " "
           << setw(6) << "from" <<setw(6)<< "to" <<setw(8)<< "length" 
           << "  "  << "minutes" << "\n";    
    for (int i=0; i < Ist->num_VerticesFS; i++) {
        for (int k=Ist->v_fromFS[i]; k < Ist->v_fromFS[i+1]; k++)  {
            long long ifid = -1;
            if (Ist->v_ArcsFS[k].original_Arc >= 0) ifid = Ist->v_SHP_Arcs_List[Ist->v_ArcsFS[k].original_Arc].info.id;
            flog <<setw(3)<< k << " "<<setw(15) << ifid 
                << " " <<setw(6)<< Ist->v_ArcsFS[k].from <<setw(6)<< Ist->v_ArcsFS[k].to <<setw(8)<< Ist->v_ArcsFS[k].length 
                  << "  "  << setw(8)<<Ist->v_ArcsFS[k].minutes << "\n";
        }
    }   
}

