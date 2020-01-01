/*
 * car-breaker-main.cpp
 *
 *  Created on: Nov 11, 2019
 *      Author: mlambiri
 *
 *  this is based on the pong-main.cpp
 *  from dwlambiri: https://github.com/dwlambiri/ping-pong-grade10
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "car-breaker-graphics.h"
#include "game-debug.h"

extern GameData carBreaker;

//===== Public Data =====

#define VERSION "0.9"

/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Dec 1, 2019
   @mname   Usage
   @details
	  If CARDEBUG is defined the function prints the a message saying that this
	  is a debug executable
	  If CARDEBUG is not defined it means that this if a release executable and that
	  all code will remain hidden from the user.
	  The function is used to print a description of the command line parameters
	  to the console. \n
  --------------------------------------------------------------------------
 */
static void
Usage(void) {
#ifdef CARDEBUG
	printf("Version %s [Debug Build %s %s]\n", VERSION, __DATE__, __TIME__);
#else
	printf("Version %s [Release Build %s %s]\n", VERSION, __DATE__, __TIME__);
#endif
	printf("Program Options\n");
	printf("   -c configFileName - selects the config file, if no config file present, default is config.txt	\n");
	printf("   -h - displays command line parameters \n");
#ifdef CARDEBUG
	printf("   -d - enables debug mode \n");
	printf("   -l debuglevel - sets the debug level (higher means less messages) \n");
	printf("   -t - enables trace mode \n");
#endif
	printf("   -v - prints the version of the program and when it was built \n");


} // end-of-function Usage

char** GetParameters(int* , char*);

/**
  ---------------------------------------------------------------------------
   @author  dwlambiri
   @date    May 22, 2017
   @name    main
   @param   argc number of command line arguments
   @param   argv command line arguments
   @return  the return code
   @details
	The main function of the program.

	\n
  --------------------------------------------------------------------------
 */
int
main(int argc, char **argv) {

	char* configFileName = NULL;
	int num = 0;
	bool startTimer = true;
	//WE process the program command line parameters first
	for (int  param = 1; param < argc; param++ ) {
		if (strcmp(argv[param], "-c") == 0) {
			if (++param < argc) {
				configFileName = argv[param];
			} //end-of-if(++param < argc)
		} else if (strcmp(argv[param], "-h") == 0) {
			Usage();
			return 1;
		}else if (strcmp(argv[param], "-n") == 0) {
			startTimer = false;
		}
		//Some command line paramters are available only in the debug executable
#ifdef CARDEBUG
		else if (strcmp(argv[param], "-d") == 0) {
			debugon = true;
		}
		else if (strcmp(argv[param], "-t") == 0) {
			traceon = true;
		}
		else if (strcmp(argv[param], "-l") == 0) {
			if (++param < argc) {
				debuglevel = atoi(argv[param]);
			} //end-of-if(++param < argc)
		}
#endif
		else if (strcmp(argv[param], "-v") == 0) {
#ifdef CARDEBUG
			printf("%s Version %s [Debug Build %s %s]\n", argv[0], VERSION, __DATE__, __TIME__);
#else
			printf("%s Version %s [Release Build %s %s]\n", argv[0], VERSION, __DATE__, __TIME__);
#endif
		}
	} //end-of-for

	// trace is enabled via command line params
	// if placed before it will not print
	TRACE();

	//Process the configuration file and extract all the game configuration
	char** p = GetParameters(&num, configFileName);

	//After the parameters are read from the file they are passed to the game
	//configuration functions to change game variable values
	configureGame(&carBreaker, num, p);

	//We are initializing the game data structures
	if(initializeAllegro(&carBreaker) == false ) {
		//error initializing the game;
		ERROR("cannot initialize graphics engine");
		return 22;
	}

	//All that is left is to run the game
	gameLoop(&carBreaker, startTimer);

	return 0;
} // end-of-function main
