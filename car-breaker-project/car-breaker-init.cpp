//============================================================================
// Name        : car-breaker-graphics.cpp
// Author      : mlambiri
// Version     :
// Copyright   : (c) 2019 Michael C Lambiri
// Description : car breaker game
//============================================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "game-debug.h"
#include "car-breaker-graphics.h"

static const int minballspeed_c = 2;
static const int maxballspeed_c = 7;
static const uint maxPaddleSize_c = 7;
static const uint maxdiff_c = 4;
static const int ballSpeedIncrease_c = 3;
static const int initPaddleSpeed_c = 20;

static const char busPngName[] = "player1.png";
static const char lrtPngName[] = "player2.png";
static const char ballPngName[] = "ball.png";
static const char gasPngName[] = "gascar.png";
static const char electricPngName[] = "ecar.png";
static const char busSound[] = "cardoor.wav";
static const char lrtSound[] = "cardoor.wav";
static const char defaultFont[] = "pirulen.ttf";


//========VARIABLE DECLARATIONS=====
//declaring the main data variable of the game
//usually passed to functions using a pointer

GameData carBreaker = {
		{INITPLAYER, INITPLAYER},
		INITGBB,
		INITDISPLAY, human_c, maxballspeed_c, NULL, FONTSIZE, MAXROUNDS, { 0 }, { 0 },
		FRAMERATE

};

//It is an array that stores several sets of height divisors and player speed multiplier
//The divisors should be in increasing order as they are checked
//starting with the smallest index
// lrt moves faster when the ball is further
BotControlInfo lrtBotArray[pro_c + 1] = {
		{ { 2, 4, 8, 16 }, { 0.8, 0.6, 0.4, 0.2 }},
		{ { 2, 4, 8, 16 }, { 1.25, 1, 0.75, 0.5 }},
		{ { 2, 4, 8, 16 }, { 2, 1.5, 1.25, 1  }},
		{ { 2, 4, 8, 16 }, { 3, 2, 1.5, 1.3 } } };

//It is an array that stores several sets of height divisors and player speed multiplier
//The divisors should be in increasing order as they are checked
//starting with the smallest index
// bus moves faster when the ball is closer
BotControlInfo busBotArray[pro_c + 1] = {
		{ { 2, 3, 4, 8 }, { 0.2, 0.4, 0.6, 0.8 }},
		{ { 2, 3, 4, 8 }, { 0.5, 0.75, 1, 1.25 }},
		{ { 2, 3, 4, 8 }, { 1, 1.25, 1.5, 2 }},
		{ { 2, 3, 4, 8 }, { 1.3, 1.5, 2, 3 } } };




//====== Game Initialization ================
/*
 * @author   mlambiri
 * @date     Dec. 30, 2019
 *  This set of functions reads game assets from files
 *  It loads fonts, bitmaps for various objects, sound files ...
 *  It also reads the car layout from a file
 *  It also reads in the config file
 */


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 2, 2019
 @mname   configureGameData
 @details
 This function gets the game config parameters as read from the config file
 In the same format as the parameters passes to the main file
 argv is an array of character pointers and argc is the is number of entries in the array
 Processing is done in the same style as the main command line arguments\n
 --------------------------------------------------------------------------
 */
void configureGameData(GameData *p, int argc, char **argv) {
	FENTRY();

	//seed random number generator with time
	srand (time(NULL));

	strcpy(p->player[bus_c].name, "Bus");
	strcpy(p->player[lrt_c].name, "LRT");
	strcpy(p->fontFileName, defaultFont);
	strcpy(p->player[bus_c].audioFileName, busSound);
	strcpy(p->player[lrt_c].audioFileName, lrtSound);
	strcpy(p->busBitmapName, busPngName);
	strcpy(p->lrtBitmapName, lrtPngName);
	strcpy(p->ballBitmapName, ballPngName);
	strcpy(p->gasBitmapName, gasPngName);
	strcpy(p->ecarBitmapName, electricPngName);

	// today
	p->year = 2019;

	p->validLayout = false;
	p->penalty = POINTSFORLOSTBALL;
	p->inLayout[0] = 0;
	p->outLayout[0] = 0;

	p->maxRows = MAXCARROWS;
	p->maxColumns = MAXCARCOLUMNS;

	p->gameNumber = 1;
	p->roundNumber = 1;
	p->backgroundColor = &(p->bcolorarray[yellow_c]);

	p->scorePointsPerSmash = 1;
	p->player[bus_c].paddleSpeed = initPaddleSpeed_c;
	p->player[lrt_c].paddleSpeed = initPaddleSpeed_c;

	p->trajectoryDisplay.display = NULL;
	p->path.rec = false;
	p->path.used = 0;
	p->path.separateDisplay = false;

	p->botLevel[bus_c] = pro_c;
	p->botLevel[lrt_c] = pro_c;

	p->helpOn = false;
	p->stats.totalBounce = 0;
	p->stats.firstEmpty = 0;
	p->stats.bounceUntilSmash[p->stats.firstEmpty] = 0;

	p->gamePaused = false;
	p->gameStart = false;
	p->roundWin = false;

	//loop that processes the command line arguments.
	//argc is the size of the argument's array and argv is the array itself
	for (int param = 0; param < argc; param++) {
		if (strcmp(argv[param], "gamemode") == 0) {
			//arcade mode
			//LRT (ie player2) is computer controlled
			if (++param < argc ){
				if (strcmp(argv[param], "arcade") == 0)
					p->gameMode = arcade_c;
				else if (strcmp(argv[param], "auto") == 0)
					p->gameMode = fullbot_c;
				else
					p->gameMode = human_c;
			}
		} else if (strcmp(argv[param], "screenwidth") == 0) {
			//display width
			if (++param < argc)
				p->display.width = atoi(argv[param]);
		} else if (strcmp(argv[param], "screenheight") == 0) {
			//display height
			if (++param < argc)
				p->display.height = atoi(argv[param]);
		}else if (strcmp(argv[param], "year") == 0) {
			//the year of play
			// pre 2000 there are no electric cars
			if (++param < argc)
				p->year = atoi(argv[param]);
		} else if (strcmp(argv[param], "maxrows") == 0) {
			//the number of rows of cars, max is MAXBRICKROWS
			if (++param < argc) {
				p->maxRows = atoi(argv[param]);
				if(p->maxRows > MAXCARROWS)
					p->maxRows = MAXCARROWS;
			}
		}else if (strcmp(argv[param], "maxcolumns") == 0) {
			//the number of rows of cars, max is MAXBRICKCOLUMNS
			if (++param < argc) {
				p->maxColumns = atoi(argv[param]);
				if(p->maxColumns > MAXCARCOLUMNS)
					p->maxColumns = MAXCARCOLUMNS;
			}
		}  else if (strcmp(argv[param], "fontsize") == 0) {
			//font size
			if (++param < argc)
				p->fontsize = atoi(argv[param]);
		} else if (strcmp(argv[param], "maxballspeed") == 0) {
			// maximum number of pixels the ball will move between frames
			if (++param < argc) {
				int maxballspeed = atoi(argv[param]);
				if(maxballspeed <= 0)
					maxballspeed = minballspeed_c;
				if (maxballspeed >= maxballspeed_c)
					maxballspeed = maxballspeed_c;
				p->maxspeed.x = maxballspeed;
				p->maxspeed.y = maxballspeed;
				p->maxballspeed = maxballspeed;
			}
		} else if (strcmp(argv[param], "maxrounds") == 0) {
			//maxscore
			if (++param < argc)
				p->maxRounds = atoi(argv[param]);
		} else if (strcmp(argv[param], "fontfile") == 0) {
			//font file name
			if (++param < argc)
				strcpy(p->fontFileName, argv[param]);
		} else if (strcmp(argv[param], "winSound") == 0) {
			//font file name
			if (++param < argc)
				strcpy(p->winSoundFile, argv[param]);
		} else if (strcmp(argv[param], "busbmp") == 0) {
			//player 1 bitmap file name
			if (++param < argc)
				strcpy(p->busBitmapName, argv[param]);
		} else if (strcmp(argv[param], "lrtbmp") == 0) {
			//player 2 bitmap file name
			if (++param < argc)
				strcpy(p->lrtBitmapName, argv[param]);
		} else if (strcmp(argv[param], "ballbmp") == 0) {
			//ball bitmap file name
			if (++param < argc)
				strcpy(p->ballBitmapName, argv[param]);
		} else if (strcmp(argv[param], "gascarbmp") == 0) {
			//gas car bitmap file name
			if (++param < argc)
				strcpy(p->gasBitmapName, argv[param]);
		} else if (strcmp(argv[param], "ecarbmp") == 0) {
			//ecar bitmap file name
			if (++param < argc)
				strcpy(p->ecarBitmapName, argv[param]);
		} else if (strcmp(argv[param], "bussound") == 0) {
			//player 1 sound file name
			if (++param < argc)
				strcpy(p->player[bus_c].audioFileName, argv[param]);
		} else if (strcmp(argv[param], "lrtsound") == 0) {
			//player 2 sound file name
			if (++param < argc)
				strcpy(p->player[lrt_c].audioFileName, argv[param]);
		} else if (strcmp(argv[param], "pattern") == 0) {
			// car layout
			if (++param < argc) {
				p->validLayout = readCarLayoutFile(p, argv[param]);
				if( p->validLayout) {
					strcpy(p->inLayout, argv[param]);
				}
			}
		} else if (strcmp(argv[param], "busspeed") == 0) {
			//bus paddle speed
			if (++param < argc) {
				p->player[bus_c].paddleSpeed = atoi(argv[param]);
			}
		} else if (strcmp(argv[param], "lrtspeed") == 0) {
			//lrt paddle speed
			if (++param < argc) {
				p->player[lrt_c].paddleSpeed = atoi(argv[param]);
			}
		}else if (strcmp(argv[param], "buslevel") == 0) {
			//player 2 paddle speed
			if (++param < argc) {
				p->botLevel[bus_c] = atoi(argv[param]) - 1;
				if(p->botLevel[bus_c] < 0 )
					p->botLevel[bus_c] = 0;
				if(p->botLevel[bus_c] > 3 )
					p->botLevel[bus_c] = 3;
			}
		}else if (strcmp(argv[param], "lrtlevel") == 0) {
			//player 2 paddle speed
			if (++param < argc) {
				p->botLevel[lrt_c] = atoi(argv[param]) -1;
			}
			if(p->botLevel[lrt_c] < 0 )
				p->botLevel[lrt_c] = 0;
			if(p->botLevel[lrt_c] > 3 )
				p->botLevel[lrt_c] = 3;
		} else if (strcmp(argv[param], "buslength") == 0) {
			//level (controls the paddle size)
			if (++param < argc) {
				p->player[bus_c].paddleSize = atoi(argv[param]);
				if (p->player[bus_c].paddleSize > maxPaddleSize_c)
					p->player[bus_c].paddleSize = maxPaddleSize_c;
				if (p->player[bus_c].paddleSize < 1)
					p->player[bus_c].paddleSize = 1;
			}
		} else if (strcmp(argv[param], "lrtlength") == 0) {
			//level (controls the paddle size)
			if (++param < argc) {
				p->player[lrt_c].paddleSize = atoi(argv[param]);
				if (p->player[lrt_c].paddleSize > maxPaddleSize_c)
					p->player[lrt_c].paddleSize = maxPaddleSize_c;
				if (p->player[lrt_c].paddleSize < 1)
					p->player[lrt_c].paddleSize = 1;
			}
		} else if (strcmp(argv[param], "fps") == 0) {
			//display fps
			if (++param < argc) {
				p->fps = atof(argv[param]);
				if (p->fps < MINFPS)
					p->fps = MINFPS;
				if (p->fps > MAXFPS)
					p->fps = MAXFPS;
			}
		} else if (strcmp(argv[param], "penalty") == 0) {
			//display penalty
			if (++param < argc) {
				p->penalty = atoi(argv[param]);
			}
		} else if (strcmp(argv[param], "trajectorywindow") == 0) {
			//ball bitmap file name
			if (++param < argc)
				if(argv[param][0] == 'y' ) {
					p->path.separateDisplay = true;
				}
		}else if (strcmp(argv[param], "record") == 0) {
			//ball bitmap file name
			if (++param < argc)
				if(argv[param][0] == 'y' ) {
					p->path.rec = true;
				}
		}else if (strcmp(argv[param], "colourscheme") == 0) {
			//player 2 bitmap file name
			if (++param < argc) {
				switch (argv[param][0]) {
				case 'y':
					p->backgroundColor = &(p->bcolorarray[yellow_c]);
					break;
				case 'b':
					p->backgroundColor = &(p->bcolorarray[blue_c]);
					break;
				case 'w':
					p->backgroundColor = &(p->bcolorarray[white_c]);
					break;
				case 'g':
					p->backgroundColor = &(p->bcolorarray[green_c]);
					break;
				case 'q':
					p->backgroundColor = &(p->bcolorarray[grey_c]);
					break;
				default:
					break;
				} //end-switch(argv[param][0])
				p->initcolor = p->backgroundColor;
			}
		}
	} //end-of-for

	p->botControl[bus_c] = &(busBotArray[p->botLevel[bus_c]]);
	p->botControl[lrt_c] = &(lrtBotArray[p->botLevel[lrt_c]]);

	FEXIT();
} // end-of-function configureGame


