/*
 * config-manager.cpp
 *
 *  Created on: May 26, 2017
 *      Author: dwlambiri
 */
#include <stdio.h>
#include <string.h>

#include "../car-breaker-project/game-debug.h"

static const char configName[] = "config.txt";
static const int bufferSize_c = 1000;
static char text[bufferSize_c];
static const int numberParams_c = 100;
static char* params[numberParams_c];
/**
  ---------------------------------------------------------------------------
   @author  dwlambiri
   @date    May 26, 2017
   @mname   GetParameters
   @details
	  Originally the game was getting the config parameters from the command
	  line, however as the program grew more complicated I moved the config processing
	  to a file. GetParameters tries to open configFileName file and if not present
	  it opens configName file.
	  Lines starting with the # sign are comments and are skipped.
	  The parameters and their values are put in an array similar to the one passed
	  to the main function.
	  We use text[] as a buffer in which we tokenize (transforms a character array
	  into multiple strings by placing zeros inside the character array).
	  Pointers to the strings are stored in params[], while *n is equal to the number of entries
	  placed in params[].
	  \n
  --------------------------------------------------------------------------
 */
char**
GetParameters(int* n, char* configFileName) {

	FENTRY();
	TRACE();
	FILE* fptr = NULL;
	if (configFileName == NULL) {
		fptr = fopen(configName, "r");
	} else {
		fptr = fopen(configFileName, "r");
		if (fptr == NULL) {
			fptr = fopen(configName, "r");
		} //end-of-if(fptr == NULL)
	}

	int counter = 0;
	char* buffer = text;
	const char* delim = "\t\r\n =";
	if(fptr == NULL){
		INFO("The file does not exist");
		*n = 0;
		FEXIT();
		return params;
	}
	int amountleft = bufferSize_c;
	while(fgets(buffer, amountleft, fptr)) {
		 if(buffer[0] == '#') {
			 int size = strlen(buffer);
			 buffer += size;
			 amountleft -= size;
			 continue;
		 }
		 int size = strlen(buffer);
		 char * pch = strtok (buffer, delim);
		 if (pch != NULL) {
			 //printf ("%s\n",pch);
			 params[counter++] = pch;
			 if (counter >= numberParams_c) {
				break;
			 } //end-of-if(counter >= numberParams_c)
			 pch = strtok (NULL, delim);
			 params[counter++] = pch;
			 if (counter >= numberParams_c) {
				break;
			 } //end-of-if(counter >= numberParams_c)

			 if (pch != NULL) {
				 //printf ("%s\n",pch);
			} //end-of-if(pch != NULL)
		 } //end-of-if(pch == NULL)
		 buffer += size;
		 amountleft -= size;
	} //end-of-while(fgets(buffer, bufferSize_c, fptr))
	fclose(fptr);
	*n = counter;
	FEXIT();
	return params;
} // end-of-function GetParameters

 /**
  ---------------------------------------------------------------------------
   @author  dwlambiri
   @date    May 26, 2017
   @name    main
   @param   argc number of command line arguments
   @param   argv command line arguments
   @return  the return code
   @details
	The main function of the program.
	\n
  --------------------------------------------------------------------------
 */
//int
//main(int argc, char **argv) {
//	int parameterSize = GetParameters();
//	printf("------------%d\n", parameterSize);
//	for (int  i = 0; i < parameterSize; i++ ) {
//		printf("%s\n", params[i]);
//	} //end-of-for
//	return 0;
//}

