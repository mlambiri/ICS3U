/*
 * write-results.cpp
 *
 *  Created on: May 30, 2017
 *      Author: dwlambiri
 */

#include <stdio.h>
#include <time.h>

#include "../car-breaker-project/game-debug.h"


static const char resultsFileName[] = "car-breaker-results.txt";

/**
  ---------------------------------------------------------------------------
   @author  dwlambiri
   @date    May 30, 2017
   @mname   recordResult
   @details
	  After each game is finished I wanted to record the result of the game to
	  a text file.
	  This function opens resultsFileName[] and appends p.
	  Then the textfile is closed. \n
  --------------------------------------------------------------------------
 */
bool
recordResult(char* p) {

	FENTRY();
	TRACE();
	FILE* fptr;
	fptr = fopen(resultsFileName, "a");
	if(fptr == NULL){
		INFO("The file does not exist");
		FEXIT();
		return false;
	}
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];

	rawtime = time (NULL);
	timeinfo = localtime (&rawtime);

	strftime (buffer,80,"%F %T ",timeinfo);

	fprintf(fptr,"%s %s \n", buffer, p);
	fclose(fptr);
	FEXIT();
	return true;
} // end-of-function recordResult

//bool debugon = true;
//bool traceon;
//int  debuglevel = 0;
///**
//  ---------------------------------------------------------------------------
//   @author  dwlambiri
//   @date    May 30, 2017
//   @name    main
//   @param   argc number of command line arguments
//   @param   argv command line arguments
//   @return  the return code
//   @details
//	The main function of the program.
//	\n
//  --------------------------------------------------------------------------
// */
//int
//main() {
//
//	recordResult("ONE");
//	recordResult("TWO");
//	recordResult("THREE");
//	return 0;
//}
