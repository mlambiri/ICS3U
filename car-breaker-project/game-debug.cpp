/*
 * game-debug.cpp
 *
 *  Created on: Nov 17, 2017
 *      Author: mlambiri
 */

#include "../car-breaker-project/game-debug.h"

#include <stdio.h>
#include <time.h>

bool debugon = false;
bool traceon = false;
int  debuglevel = debug_c;

static int calldepth = 0;
static const int nospaces_c = 2;

/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 17, 2017
   @mname   printDebugTime
   @details
	  I am trying to print the time when the macros are called.
	  However the function time() has a resolution of seconds.
	  The function clock() returns higher granularity.
	  I tried to print a time with a resolution better than one second.
	  Unfortunately the clock() function does not seem to work on Mac.
	  This function does not print the proper time on my Mac.
	  The function works on Windows machines.
	  I calculated the difference in clock() between the first call of the function
	  and each individual following call.
	  I add this difference to the time() at the moment of the first call.
	  sprintf() formats the result with resolution better than seconds\n
  --------------------------------------------------------------------------
 */
char*
printDebugTime() {

	static clock_t firstClock;
	static char buffer[100];
	static time_t rawtime;
	static bool single = true;
	struct tm * timeinfo;

	if (single == true) {
		single = false;
		rawtime = time (NULL);
		firstClock = clock();

	} //end-of-if(single == true)

	clock_t difftime = clock() - firstClock;
	int seconds = difftime /CLOCKS_PER_SEC ;
	int usec = difftime % CLOCKS_PER_SEC;
	time_t newtime = rawtime + seconds;

	char tb[40];
	timeinfo = localtime (&newtime);
	strftime (tb,40,"%F %T",timeinfo);

	sprintf(buffer, "%s.%06d", tb,  usec);

	return buffer;
} // end-of-function printDebugTime


/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 17, 2017
   @mname   functionEnter
   @details
	  we want to count how many function calls are on top of this one
	  add to a counter at the beggining of the function call\n
  --------------------------------------------------------------------------
 */
void
functionEnter() {

	calldepth += nospaces_c;
} // end-of-function functionEnter


/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 17, 2017
   @mname   functionExit
   @details
	  we want to decrement went we exit a function\n
  --------------------------------------------------------------------------
 */
void
functionExit() {

	calldepth -= nospaces_c;
} // end-of-function functionExit


/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 17, 2017
   @mname   printCallDepth
   @details
	  \n
  --------------------------------------------------------------------------
 */
char*
printCallDepth() {

	static char buffer[200];
	int i;
	for (i = 0; i < calldepth; i++ ) {
		buffer[i] = ' ';
	} //end-of-for
	buffer[i] = 0;

	return buffer;
} // end-of-function printCallDepth





