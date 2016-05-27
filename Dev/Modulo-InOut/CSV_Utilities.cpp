#include "../Modulo-Main/darh.h"

/** @name	Routine generali di lettura
 */
//@{
/** Apre un file
	@param name nome del file
	@param modo modalit\'a di apertura
*/

/**----------------------------------------------------------------------------
* Constructor
\*---------------------------------------------------------------------------*/
C_CSV::C_CSV()
{

}

/**----------------------------------------------------------------------------
* Destructor
\*---------------------------------------------------------------------------*/
C_CSV::~C_CSV()
{

}

/**----------------------------------------------------------------------------
* * Deallocate structures used for input/output to the optimizator
\*---------------------------------------------------------------------------*/
void C_CSV::CSV_AllocateMatrixDT_and_C()
{

}

/**----------------------------------------------------------------------------
* Deallocate structures used for input/output to the optimizator
\*---------------------------------------------------------------------------*/
void C_CSV::CSV_DeAllocateMatrixDT_and_C(C_IST *Ist)
{
	long i;

	if (Ist->v_DT_back != NULL) delete[] Ist->v_DT_back;

	for (i = 0; i < Ist->MatrixDT.dim; i++)
	{
		if (Ist->MatrixDT.m_length[i] != NULL) delete[] Ist->MatrixDT.m_length[i];
		if (Ist->MatrixDT.m_time[i] != NULL) delete[] Ist->MatrixDT.m_time[i];
		if (Ist->MatrixDT.m_vpred[i] != NULL) delete[] Ist->MatrixDT.m_vpred[i];
		if (Ist->MatrixDT.m_apred[i] != NULL) delete[] Ist->MatrixDT.m_apred[i];
	}
	if (Ist->MatrixDT.m_length != NULL) delete[] Ist->MatrixDT.m_length;
	if (Ist->MatrixDT.m_time != NULL) delete[] Ist->MatrixDT.m_time;
	if (Ist->MatrixDT.m_vpred != NULL) delete[] Ist->MatrixDT.m_vpred;
	if (Ist->MatrixDT.m_apred != NULL) delete[] Ist->MatrixDT.m_apred;

	if (Ist->TRoute.RArcs.arc != NULL) delete[] Ist->TRoute.RArcs.arc;
	//if (WPoints.vertex != NULL) delete [] WPoints.vertex;
	//if (WPoints.wstops != NULL) delete [] WPoints.wstops;
}

FILE *C_CSV::CSV_ApriFile(char *name, char *modo)
{
	//CError error;
	FILE *dummy;
	char *NomeProc = (char *) "CSV_ApriFile";
	char buf[100];
	// commento interno (a due barre) che non viene riportato da doc++
	if ((dummy = fopen(name, modo)) == NULL) {
		snprintf(buf, sizeof(buf), "%s: Error opening file = %s", NomeProc, name);
		error.fatal(buf, __FUNCTION__);
	}
	return dummy;
}

/** Legge un record
	la funzione legge sequenzialmente i record di un file in input e 
	aggiorna il contatore dei record
	@param input:        puntatore a FILE di input 
	@param input/output: contatore del record corrente in lettura
	@param output:       stringa contenente il record letto
*/
int C_CSV::CSV_LeggiRec(FILE *inp, long *numRec, unsigned char *linea)
{
	int ch;
	int numc = 0;
	linea[numc] = '\0';
	ch = fgetc(inp);
	while (ch != '\r' && ch != '\n' && !feof(inp))  {
		linea[numc] = (char)ch;
		numc++;
		ch = fgetc(inp);
	}
	linea[numc] = '\0';
	(*numRec)++;
	return(0);
}

/** Conta i record di un file di ingresso
	@param  input: puntatore a FILE di input 
	@return numero dei record del file
*/
long C_CSV::CSV_contaRec(FILE *inp)
{
	long nrec = 0;
	static unsigned char line[512];

	while (!feof(inp)) {
		line[0] = '\0';
		CSV_LeggiRec(inp, &nrec, line);
		if (line[0] == '\0') nrec--;
	}
	return(nrec);
}

/** Legge il prossimo campo di un record
	@param  input:			contatore del record corrente
	@param  input/output:	contatore del campo
	@param  input/output:	primo carattere da leggere nel record, aggiornato in uscita
	@param  output:			stringa contenente il campo letto
	@return -1 se errore, altrimenti numero di caratteri letti 
*/
int C_CSV::CSV_getWord(int numRec, int *numFields, unsigned char **p, unsigned char *w)
{
	int i;
	char *NomeProc = (char *) "CSV_getWord";
	char buf[100];
	//CError error;
	(*numFields)++;
	/*

		************ riattivare se i campi sono compresi in doppi apici

		if (**p != DOPPIOAPICE) {
		sprintf(buf, "record %d campo %d",numRec,*numFields);
		CSV_Errore(NomeProc, NO2APICI, buf);
		return(-1);
		}
		(*p)++;
		i = 0;
		while (**p != DOPPIOAPICE && **p != '\0')   {w[i++] = **p; (*p)++;}
		w[i] = '\0';
		if (**p != DOPPIOAPICE) {
		sprintf(buf, "record %d campo %d",numRec,*numFields);
		CSV_Errore(NomeProc, NO2APICI, buf);
		return(-1);
		}
		(*p)++;
		if (**p != PUNTOEVIRGOLA && **p != '\0') {
		sprintf(buf, "record %d campo %d",numRec,*numFields);
		CSV_Errore(NomeProc, NOSEPARATORE, buf);
		return(-1);
		}
		(*p)++;
		return(i);
		*/
	i = 0;
	while (**p != CON_PUNTOEVIRGOLA && **p != '\0')   { w[i++] = **p; (*p)++; }
	w[i] = '\0';
	if (**p != CON_PUNTOEVIRGOLA && **p != '\0') {
		snprintf(buf, sizeof(buf), "Error %s : record %d campo %d", NomeProc, numRec, *numFields);
		error.fatal(buf, __FUNCTION__);
		return(-1);
	}
	(*p)++;
	return(i);
}

//@}
/** @name	Funzioni controllo sul tipo del contenuto di una stringa
 */
//@{
/** Verifica se la stringa in input rappresenta un intero
	@param  input:	stringa da controllare
	@return true se non e' intero, false altrimenti
*/
int C_CSV::CSVnonIntero(unsigned char *w)
{
	int i = 0;
	char *NomeProc = (char *)   "CSV_NNONINTERO";

	while (w[i] == CON_BLANK) i++;
	if (w[i] == '-') i++;
	if (!isdigit(w[i])) return(true);
	while (isdigit(w[i])) i++;
	while (w[i] == CON_BLANK) i++;
	if (w[i] != '\0') return(true);
	return(false);
}

/** Verifica se la stringa in input rappresenta un intero o NULL
@param  input:	stringa da controllare
@return true se non e' intero, false altrimenti
*/
int C_CSV::CSVnonInteroNull(unsigned char *w)
{
	int i = 0;

	if (w[0] == '\0') return(false);

	while (w[i] == CON_BLANK) i++;
	if (w[i] == '-') i++;
	if (!isdigit(w[i])) return(true);
	while (isdigit(w[i])) i++;
	while (w[i] == CON_BLANK) i++;
	if (w[i] != '\0') return(true);

	return(false);
}

/** Verifica se la stringa in input rappresenta un reale
	@param  input:	stringa da controllare
	@param output:	valore del numero reale (if any)
	@return true se non e' reale, false altrimenti
*/
int C_CSV::CSVnonReale(unsigned char *w)
{
    char *NomeProc = (char *)   "CSV_NonEReale";
    int i=0,FLAGDEC = 0, PosSepDec = -1 ;

    while (w[i] == CON_BLANK ) i++;
    if (!isdigit(w[i]) && w[i] != CON_SEPDECIMAL) return(true);
    for(;;i++)
        if ( !isdigit(w[i]) )
            if ( w[i] == CON_SEPDECIMAL ) { FLAGDEC++; PosSepDec = i; }
			else break;

    while (w[i] == CON_BLANK ) i++;
    if ( w[i] != '\0') return(true);
    if (FLAGDEC > 1) return(true);
    if (PosSepDec >= 0) w[PosSepDec] = '.';
    //sscanf(w,"%f",f);
	return(false);
}

//@}
/** @name	Functions to read a field 
 */
//@{
/**
* Reads a boolean
* @param numRec     input number of the record scanned (used only for error messages)
* @param numFields  input/output pointer to the number of the field, it is increased by one in output
* @param p          pointer to the first character of the line to be scanned
* @return           the value of the boolean read
*/
bool C_CSV::CSV_readBool(int numRec, int *numFields, unsigned char **p)
{
	unsigned char word[CON_MAXNDESCRSTOPPOINT];
	char word_dummy[CON_MAXNDESCRSTOPPOINT];
	int i;
	bool b;
	//CError error;

	if ((i = CSV_getWord(numRec, numFields, p, word)) < 0) {
		snprintf(buf, sizeof(buf), "Record = %d field %d not found", numRec, *numFields);
		error.fatal(buf, __FUNCTION__);
	};
	if (CSVnonIntero(word))	{
		snprintf(buf, sizeof(buf), "Record = %d field %d not an integer = %s", numRec, *numFields, word);
		error.fatal(buf, __FUNCTION__);
	}
	strcpy(word_dummy, (char *)word);// sscanf requires char, not unsigned char
	sscanf(word_dummy, "%d", &i);
	if (i != 0 && i != 1)	{
		snprintf(buf, sizeof(buf), "Record = %d field %d  value %d is not a boolean", numRec, *numFields, i);
		error.fatal(buf, __FUNCTION__);
	}
	b = i;
	return b;
}

/**
* Reads an integer number
* @param numRec     input number of the record scanned (used only for error messages)
* @param numFields  input/output pointer to the number of the field, it is increased by one in output
* @param p          pointer to the first character of the line to be scanned
* @param LB         minimum value allowd for the field
* @param UB         maximum value allowd for the field
* @return           the value of the integer read
*/
int C_CSV::CSV_readInt(int numRec, int *numFields, unsigned char **p, int LB, int UB)
{
	unsigned char word[CON_MAXNDESCRSTOPPOINT];
	char word_dummy[CON_MAXNDESCRSTOPPOINT];
	int i;
	//CError error;

	if ((i = CSV_getWord(numRec, numFields, p, word)) < 0) {
		snprintf(buf, sizeof(buf), "Record = %d field %d not found", numRec, *numFields);
		error.fatal(buf, __FUNCTION__);
	};
	if (CSVnonIntero(word))	{
		snprintf(buf, sizeof(buf), "Record = %d field %d not an integer = %s", numRec, *numFields, word);
		error.fatal(buf, __FUNCTION__);
	}
	strcpy(word_dummy, (char *)word);// sscanf requires char, not unsigned char
	sscanf(word_dummy, "%d", &i);
	if (i < LB)	{
		snprintf(buf, sizeof(buf), "Record = %d field %d  value %d smaller of %d", numRec, *numFields, i, LB);
		error.fatal(buf, __FUNCTION__);
	}
	if (i > UB)	{
		snprintf(buf, sizeof(buf), "Record = %d field %d  value %d greater of %d", numRec, *numFields, i, UB);
		error.fatal(buf, __FUNCTION__);
	}
	return i;
}

/**
* Reads an integer number or NULL
* @param numRec     input number of the record scanned (used only for error messages)
* @param numFields  input/output pointer to the number of the field, it is increased by one in output
* @param p          pointer to the first character of the line to be scanned
* @param LB         minimum value allowd for the field
* @param UB         maximum value allowd for the field
* @param default    value returned when the field is NULL
* @return           the value of the integer read
*/
int C_CSV::CSV_readInt_NULL(int numRec, int *numFields, unsigned char **p, int LB, int UB, int def)
{
	unsigned char word[CON_MAXNDESCRSTOPPOINT];
	char word_dummy[CON_MAXNDESCRSTOPPOINT];
	int i;
	//CError error;

	if ((i = CSV_getWord(numRec, numFields, p, word)) < 0) {
		snprintf(buf, sizeof(buf), "Record = %d field %d not found", numRec, *numFields);
		error.fatal(buf, __FUNCTION__);
	};
	if (CSVnonInteroNull(word))	{
		snprintf(buf, sizeof(buf), "Record = %d field %d not an integer = %s", numRec, *numFields, word);
		error.fatal(buf, __FUNCTION__);
	}
	if (word[0] == '\0')
		return def;

	strcpy(word_dummy, (char *)word);// sscanf requires char, not unsigned char
	sscanf(word_dummy, "%d", &i);
	if (i < LB)	{
		snprintf(buf, sizeof(buf), "Record = %d field %d  value %d smaller of %d", numRec, *numFields, i, LB);
		error.fatal(buf, __FUNCTION__);
	}
	if (i > UB)	{
		snprintf(buf, sizeof(buf), "Record = %d field %d  value %d greater of %d", numRec, *numFields, i, UB);
		error.fatal(buf, __FUNCTION__);
	}
	return i;
}

/**
* Reads a real number (double)
* @param numRec     input number of the record scanned (used only for error messages)
* @param numFields  input/output pointer to the number of the field, it is increased by one in output
* @param p          pointer to the first character of the line to be scanned
* @param LB         minimum value allowd for the field
* @param UB         maximum value allowd for the field
* @return           the value of the real number  read
*/
double C_CSV::CSV_readReal(int numRec, int *numFields, unsigned char **p, double LB, double UB)
{
	unsigned char word[CON_MAXNDESCRSTOPPOINT];
	char word_dummy[CON_MAXNDESCRSTOPPOINT];
	int i;
	double d;

	//CError error;

	if ((i = CSV_getWord(numRec, numFields, p, word)) < 0) {
		snprintf(buf, sizeof(buf), "Record = %d field %d not found", numRec, *numFields);
		error.fatal(buf, __FUNCTION__);
	};
	if (CSVnonReale(word))	{
		snprintf(buf, sizeof(buf), "Record = %d field %d not a real number = %s", numRec, *numFields, word);
		error.fatal(buf, __FUNCTION__);
	}
	strcpy(word_dummy, (char *)word);// sscanf requires char, not unsigned char
	sscanf(word_dummy, "%lf", &d);
	if (d < LB)	{
		snprintf(buf, sizeof(buf), "Record = %d field %d  value %f smaller of %lf", numRec, *numFields, d, LB);
		error.fatal(buf, __FUNCTION__);
	}
	if (d > UB)	{
		snprintf(buf, sizeof(buf), "Record = %d field %d  value %f greater of %lf", numRec, *numFields, d, UB);
		error.fatal(buf, __FUNCTION__);
	}
	return d;
}

/**
* Reads a string
* @param numRec     input number of the record scanned (used only for error messages)
* @param numFields  input/output pointer to the number of the field, it is increased by one in output
* @param p          input pointer to the first character of the line to be scanned
* @param word       output string read
*/
void C_CSV::CSV_readString(int numRec, int *numFields, unsigned char **p, unsigned char *word)
{
	int i;
	//CError error;

	if ((i = CSV_getWord(numRec, numFields, p, word)) < 0) {
		snprintf(buf, sizeof(buf), "Record = %d field %d not found", numRec, *numFields);
		error.fatal(buf, __FUNCTION__);
	};
}

//@}
