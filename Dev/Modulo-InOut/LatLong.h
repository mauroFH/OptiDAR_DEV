/*
* File:   LatLong.h
* Author : Mauro
*
* Created on 6 aprile 2016, 15.18
*/

#ifndef LATLONG_H
#define LATLONG_H

#include <cmath>			//2010-08-11: was <math.h>	  
#include <cstdio>			//2010-08-11: was <stdio.h>	  
#include <cstdlib>			//2010-08-11: was <stdlib.h>  
#include <cstring>			//2010-08-11: was <string.h>  
#include <cctype>			//2010-08-11: was <ctype.h>  
#include <iostream>			//2010-08-11: was <iostream.h>
#include <iomanip>			//2010-08-11: was <iomanip.h> 

using namespace std;			//2010-08-11: added

//#define	eClarke1866	5		//names for ellipsoidId's
//#define	eGRS80		19
//#define	eWGS72		21
//#define	eWGS84		22
//
//#define	dNAD27_MB_ON	6		//names for datumId's
//#define	dNAD27_Canada	15
//#define	dNAD83_Canada	22
//#define	dNAD83_ConUS	23
//#define	dWGS84		27

class LatLong{

	//const double PI = 4 * atan(1);	//Gantz used: PI=3.14159265;
	//const double deg2rad = PI / 180;
	//const double rad2deg = 180 / PI;
	//const double k0 = 0.9996;

	void LLtoUTM(int eId, double Lat, double Long, double *Northing, double *Easting, int *Zone);
	void UTMtoLL(int eId, double Northing, double Easting, int Zone, double& Lat, double& Long);

public:

	void ConvLL2WGS84(double pLat, double pLong, double *pY, double *pX);
};

#endif /* LATLONG_H */

// 2002-December:  by Eugene Reimer, ereimer@shaw.ca
// started with 2 routines (LLtoUTM, UTMtoLL) by Chuck Gantz chuck.gantz@globalstar.com;
//	Gantz cites his source as:  USGS Bulletin#1532;
// dropped the "LatZone" letters (aka Latitude-Bands);
// reworked handling "origin 10million" for northings in Southern-hemisphere -- his method needed "LatZone" letters which are even sillier than ten-million convention;
// better value for PI:
//	made 6mm difference in calculated UTM coords (in Gantz example);
// ellipsoid constants:  replaced Gantz's constants, which he said were from PeterDana website, with data from PeterDana website (to my surprise they differ):
//	http://www.colorado.edu/geography/gcraft/notes/datum/edlist.html
//	Dana cites his sources as:  NIMA-8350.2-1977july4  and  MADTRAN-1996october1
//	-- better Clarke1866 ellipsoid made 360mm difference in calculated UTM coords in MB;  NAD-27 example in Manitoba now within 1mm of NR-Canada online-converter!!
//	-- computed UTM-coords for AustinDomeStar differ from Dana's by roughly 100mm; but agree to the mm with NR-Canada online-converter (nrcan.gc.ca)!!
// datum constants & datum conversion routine:  using formulae from PeterDana website;  the dX,dY,dZ constants are for the conversion formulae;
//	have defined only those datums that apply to NorthAmerica (28 out of several hundred);
//	Dana cites:  Bowring,B. 1976. Transformations from spatial to geographical coordinates. Survey Review XXIII; pg323-327.
// added support for input from a file;  am tempted to scrap cmdline-args in which case the Usage-Examples would become:  echo "..." | LatLong-UTM
// considered scrapping Testcases routine and using a bash-script of testcases instead;  ==keeping it for non-WGS84 testcases since my UI has no datum-conversion...

// 2010-08-11:  in 2002 this compiled just fine,  but the current g++ insists main must return int;  and warns about deprecated header-files;
//	fixed by revising void main() --> int main();  adding return 0;  revising #include's as shown above eg: <math.h> --> <cmath>;  adding: using namespace std;

// 2010-08-12:  debugging DatumConvert:
//	my 1st testD eg, Dana-eg1, going TO-XYZ;
//		according to Dana it is:	X:-742507.1    Y:-5462738.5    Z:3196706.5		(Dana calls it ECEF XYZ)
//		I was getting:			X:-743130.2424 Y:-5467322.9359 Z:3199389.2544
//		I'm now getting (sin-sqred):	X:-742507.1145 Y:-5462738.4892 Z:3196706.5101		<==AGREES after sin->sin-sqred fix!!
//		GEOTRANS (to Geocentric) gets:	X:-742507      Y:-5462738      Z:3196707		(Geotrans calls it Geocentric)
//		ergo: GEOTRANS agrees with Dana, my code based on Dana-equations does too after fixing;
//	my 2nd testD eg, converting Lat:51.5 Long:101.5 from NAD27 to NAD83:
//		NTv2 (online):		Lat:51:29:59.96845  Long:101:30:1.65354W			<==using http://www.geod.nrcan.gc.ca/apps/ntv2/ntv2_geo_e.php
//					Lat:51.49999124     Long:101.5004593W				<--converted to degrees on my calculator (10-digit precision)
//		NTv2 (exe under Wine):  Lat:51.499991236    Long:101.500459317W				<==AUTHORITATIVE ANSWER
//		NADCON gets:		Lat:==NO LUCK, NORTH-AMERICA==USA==!!==				<==http://www.ngs.noaa.gov/cgi-bin/nadcon.prl
//		GEOTRANS gets:		Lat:51.50004        Long:-101.50059      Ht:-25			<--inaccurate;  its UI truncates Datum-name => unsure which??
//		I'm getting:		Lat:51.500036202    Long:-101.500594748  Ht:-24.919516822	<--From:NAD27_Canada;  same as GEOTRANS but not good enough
//		I'm getting:		Lat:51.500013926    Long:-101.500577765  Ht:-26.781441874    	<--From:NAD27_MB_ON;   slightly closer but still not good enough
//		Paper http://www.h-e.com/pdfs/de_sbe94.pdf:  shows MB has approx no change in Lat,  from -1.0 in E-MB to -1.6seconds in W-MB change in Long;
//			same info in degrees:  Longitude from -0.00027 in Eastern-MB to -0.00044degrees in Western-MB;  <--my eg in W-MB changes by -0.00046degrees;
//			Error on that map-corner-eg to 5-place-fractional-degrees:  Lat-error: 0.00001  Long-error: 00012;
//	==Conclusions after some reading:  (1) NADCON & NTv2(NRCAN) are the reasonably accurate methods;  (2) my (and GEOTRANS's) formula approach is a waste of time;
//	PROJ (http://trac.osgeo.org/proj/) -- it needs grid shift file, eg: NTV2_0.GSB from http://www.geod.nrcan.gc.ca/online_data_e.php
//	NTv2, DOS-program that comes with the "grid shift file" NTV2_0.GSB from NRCAN, runs under Wine;  http://www.geod.nrcan.gc.ca/online_data_e.php
//	note: NTv2 is Canadian;  Australia, NewZealand, Germany, Switzerland, Spain also use its file-format;  and unofficially GreatBritain, France, Portugal;
//	NADCON is yank;  they call it the NORTH-AMERICAN converter but it only supports locations within-USA:-)
//	TRANSDAT: Windows program installed via Wine, into c:\Program Files\transdat;  	needs MFC42.DLL, installed into /home/ereimer/.wine/drive_c/windows/system32
// NTv2:  have found OpenSource programs with NTv2 support;  also found freely downloadable NTv2 Data-files;  and the NTv2 Developer's Guide;
// Notes on NTv2 doc & tools are now in my Datum-Conversion rant:  http://ereimer.net/rants/datum-conversion.htm
//	[For me: copies of NTv2 doc & tools are in:  /pix/er-GIS-DatumConversion-NAD83-WGS84-NTv2-etc-data;  see 00-NOTES... in that dir]
// those tools offer UTM<->LatLong as well as Datum-shift conversions;  may abandon this program as hardly needed, except for more flexible outputformat==??==
// one thing that is needed:  a NAD27->NAD83 conversion-table (in NTv2 format) that handles all of North America==??==

// 2010-08-30:
// put up a web-interface:  http://ereimer.net/cgi-bin/coordinatecvt;
// nbL:  accept N|S E|W at either end of a  Lat/Long coordinate;  (letter-up-front style is not offered for output);
// CvtLine:  accept UTM-4-word input eg: 14 N 501000 5678901 (same as 14 501000 N5678901);  Prt: fUT4 is Fmt bit for UTM-4-word output, replaces fUT3 as on-by-default;
// Usage: added Examples and reworded Instructions for the 4-word UTM style now supported on input & output;
// Consider:
//	doing space->colon fixups to get Lat Long pair might be possible, since valid UTM-Easting is never less than 60...
//	adding some checking for better errmsgs;  eg: each "word" must start with digit|minus-sign|N|S|E|W|n|s|e|w;

