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
 @date    Nov 17, 2019
 @mname   configureGame
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
	p->cAlgoSelector = false;
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
		}else if (strcmp(argv[param], "calgo") == 0) {
			//algorithm selector
			int v = 0;
			if (++param < argc)
				v = atoi(argv[param]);
			if(v <= 1)
				p->cAlgoSelector = false;
			else
				p->cAlgoSelector = true;
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



//==== Game Graphics ======
/*
 * @author   mlambiri
 * @date     Dec. 30, 2019
 *  This section contains functions that draw items on screen
 */

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 28, 2019
 @mname   loadPlayerBitmap
 @details
   this function loads a bitmap for the player from a file\n
 --------------------------------------------------------------------------
 */
bool loadPlayerImage(GamePlayer *p, char* fname) {
	FENTRY();

	if ((p->ge.bmap = al_load_bitmap(fname)) == NULL) {
		ERROR2("cannot load player bitmap", fname);
		FEXIT();
		return false;
	}
	p->ge.width = (al_get_bitmap_width(p->ge.bmap)* (p->paddleSize))/ maxPaddleSize_c;
	p->ge.height = al_get_bitmap_height(p->ge.bmap);
	FEXIT();
	return true;
} // end-of-function loadPlayerBitmap

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   loadBitmap
 @details
 return true if ok false otherwise\n
 --------------------------------------------------------------------------
 */
bool loadBitmap(GameBasicBlock *g, char* fname) {
	FENTRY();

	if ((g->bmap = al_load_bitmap(fname)) == NULL) {
		ERROR2("cannot load bitmap", fname);
		FEXIT();
		return false;
	}
	g->width = al_get_bitmap_width(g->bmap);
	g->height = al_get_bitmap_height(g->bmap);

	FEXIT();
	return true;
} // end-of-function loadBitmap


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 27, 2019
 @mname   loadAudio
 @details
 \n
 --------------------------------------------------------------------------
 */
bool loadAudio(GamePlayer *gptr) {
	FENTRY();

	gptr->sample = al_load_sample(gptr->audioFileName);
	if (gptr->sample == NULL) {
		ERROR2("Audio clip sample not loaded: ", gptr->audioFileName);
		FEXIT();
		return false;
	}

	FEXIT();
	return true;
} // end-of-function loadAudio


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 23, 2019
 @mname   PlaySound
 @details
 \n
 --------------------------------------------------------------------------
 */
void  playSound(ALLEGRO_SAMPLE *s) {
	if (s) {
		al_play_sample(s, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
} // end-of-function PlaySound


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 28, 2019
 @mname   loadWinnerSound
 @details
 \n
 --------------------------------------------------------------------------
 */
bool loadWinnerSound(GameData *gptr) {
	FENTRY();

	gptr->winsample = al_load_sample(gptr->winSoundFile);
	if (gptr->winsample == NULL) {
		ERROR2("Audio clip sample not loaded: ", gptr->winSoundFile);
		FEXIT();
		return false;
	}
	FEXIT();
	return true;
} // end-of-function loadWinnerSound

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 27, 2019
 @mname   loadFont
 @details
   this function will load fonts for the text used to print
   the various graphics\n
 --------------------------------------------------------------------------
 */
bool loadFont(GameData *gptr, int size) {
	FENTRY();

	int fontSize = gptr->fontsize;
	switch (size) {
	case smallFont_c:
		fontSize /= 2;
		break;
	case largeFont_c:
		fontSize *= 2;
		break;
	default:
		break;
	} //end-switch(size)
	gptr->font[size] = al_load_ttf_font(gptr->fontFileName, fontSize, 0);

	//error message if the font file is NULL
	if (gptr->font[size] == NULL) {
		ERROR2("Could not load: ", gptr->fontFileName);
		FEXIT();
		return false;
	}
	FEXIT();
	return true;
} // end-of-function loadFont

/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Dec. 1, 2019
   @mname   createTrajectoryDisplay
   @details
	  \n
  --------------------------------------------------------------------------
 */
void createTrajectoryDisplay(GameData* g) {
	FENTRY();

	if(g->trajectoryDisplay.display != NULL) {
		FEXIT();
		return;
	}

	g->trajectoryDisplay.width = g->display.width/2;
	g->trajectoryDisplay.height = g->display.height/2;
	int x = 100;
	int y = 200;

	g->trajectoryDisplay.display = al_create_display(g->trajectoryDisplay.width, g->trajectoryDisplay.height);
	if(g->display.display) {
		al_get_window_position(g->display.display, &x, &y);
		x += g->display.width;
	}

	al_set_window_title(g->trajectoryDisplay.display, "Ball Trajectory");
	al_register_event_source(g->eventqueue, al_get_display_event_source(g->trajectoryDisplay.display));
	writeTrajectoryToWindow(g);
	FEXIT();
} // end-of-method createTrajectoryDisplay


/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Dec. 4, 2019
   @mname   writeTrajectoryToWindow
   @details
	  \n
  --------------------------------------------------------------------------
 */
void writeTrajectoryToWindow(GameData* g) {
	FENTRY();


	if((g->trajectoryDisplay.display  == NULL)  || (g->path.separateDisplay == false)){
		FEXIT();
		return;
	}

	al_set_target_backbuffer(g->trajectoryDisplay.display );
	al_clear_to_color(al_map_rgb(255,255,255));

	if(g->path.rec == true) {
		for (int i = 1; i < g->path.used; i++ ) {
			al_draw_line(g->path.point[i-1].x/2, g->path.point[i-1].y/2, g->path.point[i].x/2, g->path.point[i].y/2, al_map_rgb(255, 0,0), 1.0);
		} //end-of-for
		al_draw_line(g->path.point[g->path.used-1].x/2, g->path.point[g->path.used-1].y/2, g->ball.position.x/2, g->ball.position.y/2, al_map_rgb(255, 0,0), 1.0);
	}

	al_set_target_backbuffer(g->display.display);
	FEXIT();
} // end-of-method writeTrajectoryToWindow



/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Dec. 4, 2019
   @mname   flipAllDisplays
   @details
	  \n
  --------------------------------------------------------------------------
 */
void flipAllDisplays(GameData* g) {
	FENTRY();

	if(g->trajectoryDisplay.display) {
		al_set_target_backbuffer(g->trajectoryDisplay.display );
		al_flip_display();
	}
	if(g->display.display) {
		al_set_target_backbuffer(g->display.display);
		al_flip_display();
	}
	FEXIT();
} // end-of-method flipAllDisplays




/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 24, 2019
 @mname   setCarGraphics
 @details
    This function will set the appropriate bitmaps for each car type
    this allows to set ecars with different bitmaps than gas cars\n
 --------------------------------------------------------------------------
 */
void  setCarGraphics(GameData* p) {
	FENTRY();


	p->carArea.speed.x = 0;
	p->carArea.speed.y = 0;
	p->carArea.onScreen = false;
	p->carArea.width = 0;
	p->carArea.height = 0;

	for (int i = 0; i < p->maxRows; i++) {
		for (int j = 0; j < p->maxColumns; j++) {
			if(p->cars[i][j].indestructible == false) {
				setBitmap(&(p->cars[i][j]), p->gasBitmap);
			}
			else {
				setBitmap(&(p->cars[i][j]), p->ecarBitmap);
			}
			p->cars[i][j].speed.x = 0;
			p->cars[i][j].speed.y = 0;
		} //end-of-for
	} //end-of-for

	for (int j = 0; j < p->maxColumns; j++) {
		p->carArea.width += p->cars[0][j].width;
	}
	for (int i = 0; i < p->maxRows; i++) {
		p->carArea.height += p->cars[i][0].height;
	}


	/*
	 * @author   mlambiri
	 * @date     Jan. 1, 2020
	 *  this section checks if the car area is wider than the display
	 *  in that case we need to mark all cars of display as 'not on screen'
	 *
	 *  if we do not do that there can be gas cars that that not on screen
	 *  in this case the game might not terminate as the players will
	 *  always have gas cars to destroy
	 */

	if(p->carArea.width > p->display.width) {
		int c1 = p->display.width / p->cars[0][0].width;
		for (int i = c1; i < p->maxColumns; i++ ) {
			for (int j = 0; j < p->maxRows; j++ ) {
				if((p->cars[j][i].onScreen == true) && (p->cars[j][i].indestructible == false)) {
					p->remainingCars--;
				}
				p->cars[j][i].onScreen = false;
			} //end-of-for
		} //end-of-for
		p->carArea.width  = p->display.width;
		p->maxColumns = c1;
	}


	int ypos = (p->display.height - p->cars[0][0].height*p->maxRows)/2;
	for (int i = 0; i < p->maxRows; i++) {
		int xpos = (p->display.width - p->cars[0][0].width*p->maxColumns)/2;
		if(xpos < 0) {
			xpos = 0;
		}
		for (int j = 0; j < p->maxColumns; j++) {
			p->cars[i][j].position.y = ypos;
			p->cars[i][j].position.x = xpos;
			xpos += p->cars[i][j].width;
		} //end-of-for
		ypos += p->cars[i][0].height;
	} //end-of-for

	p->carArea.position.x = p->cars[0][0].position.x;
	p->carArea.position.y = p->cars[0][0].position.y;

	FEXIT();
	return;
}

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   setBitmap
 @details
 --------------------------------------------------------------------------
 */
void setBitmap(GameBasicBlock *g, ALLEGRO_BITMAP* b) {
	FENTRY();

	g->bmap = b;
	g->width = al_get_bitmap_width(g->bmap);
	g->height = al_get_bitmap_height(g->bmap);
	FEXIT();
} // end-of-function setBitmap


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   centerBallAndPlayerPositions
 @details
 This function sets the players in the middle of the Y axis and provides
 the ball to one of the players
 If it is the first round, the player who has the ball is chosen at random.
 After a round win the round winner gets the serve.\n
 --------------------------------------------------------------------------
 */
void centerBallAndPlayerPositions(GameData *gptr) {
	FENTRY();

	gptr->ball.speed.y = minballspeed_c + rand() % 3;
	if (gptr->roundWin) {
		gptr->turn = gptr->roundWinner;
		if (gptr->roundWinner == &(gptr->player[bus_c])) {
			gptr->ball.speed.y *= -1;
		} //end-of-if(p->roundWinner == &(p->player[0]))
	} else {
		//if there is no roundwinnner, it is the first serve of the game
		//we need to pick at random a starting player
		switch (rand() % 2) {
		case 0:
			// player 1
			gptr->ball.speed.y *= -1;
			gptr->turn = &gptr->player[bus_c];
			break;
		default:
			//player 2
			gptr->turn = &gptr->player[lrt_c];
			break;
		} //end-switch(rand() %2)
	} //end-of-if(p->roundWinner)

	gptr->ball.speed.x = rand() % 5;
	if (gptr->ball.speed.x == 0)
		gptr->ball.speed.x = 3;
	switch (rand() % 2) {
	case 0:
		//serve up
		gptr->ball.speed.x *= -1;
		break;
	default:
		break;
	} //end-switch(rand() %2)

	gptr->player[bus_c].ge.position.x = gptr->display.width / 2 - gptr->player[0].ge.width / 2;
	gptr->player[bus_c].ge.position.y = gptr->display.height - gptr->player[0].ge.height;
	gptr->player[bus_c].ge.speed.x = 0;
	gptr->player[lrt_c].ge.position.x = gptr->display.width / 2 - gptr->player[1].ge.width / 2;
	gptr->player[lrt_c].ge.position.y = 0;
	gptr->player[lrt_c].ge.speed.x = 0;

	if (gptr->ball.speed.y > 0) {
		gptr->ball.prevposition.y = gptr->ball.position.y;
		gptr->ball.position.y = gptr->player[lrt_c].ge.position.y + gptr->player[1].ge.height;
	}else {
		gptr->ball.prevposition.y = gptr->ball.position.y;
		gptr->ball.position.y = gptr->player[bus_c].ge.position.y - gptr->ball.height;
	}
	gptr->ball.prevposition.x = gptr->ball.position.x;
	gptr->ball.position.x = gptr->display.width / 2 - (gptr->ball.width / 2);
	if (gptr->ball.speed.y > 0) {
		gptr->startsample = gptr->player[lrt_c].sample;
	} else {
		gptr->startsample = gptr->player[bus_c].sample;
	}

	FEXIT();

} // end-of-function centerBallAndPlayerPositions


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   drawTextToScreen
 @details
 Displays text on screen using allegro
 Declared an enumeration of text sizes
 Different text sizes are used for different messages \n
 --------------------------------------------------------------------------
 */
int drawTextToScreen(GameData *gptr, char *text, int x, int y, int size) {
	FENTRY();

	al_draw_text(gptr->font[size], gptr->fontColor, x, y, ALLEGRO_ALIGN_CENTRE, text);
	int fsize = gptr->fontsize;
	switch (size) {
	case smallFont_c:
		fsize /= 2;
		break;
	case largeFont_c:
		fsize *= 2;
		break;
	default:
		break;
	} //end-switch(size)
	FEXIT();
	return y + fsize + 10;
} // end-of-function drawTextToScreen

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   drawBeginGameScreen
 @details
 Returns false if escape key is pressed
 This function displays the first screen that the user views in the game\n
 --------------------------------------------------------------------------
 */
void drawBeginGameScreen(GameData *gptr) {
	FENTRY();


	int next = drawTextToScreen(gptr, (char*) "Welcome to Car Smasher", gptr->display.width / 2,
			gptr->display.height / 4, largeFont_c);
	al_flush_event_queue(gptr->eventqueue);
	drawTextToScreen(gptr, (char*) "(c) mlambiri 2019", gptr->display.width / 2, next,
			smallFont_c);

	if(gptr->gameMode == fullbot_c) {
		next = drawTextToScreen(gptr, (char*) "Full Auto Mode (Bot v Bot)", gptr->display.width / 2,
				gptr->display.height / 2, regularFont_c);
	}
	else if (gptr->gameMode == arcade_c) {
		next = drawTextToScreen(gptr, (char*) "Arcade Mode (Bot Controls LRT)",
				gptr->display.width / 2, gptr->display.height / 2, regularFont_c);
	} else {
		next = drawTextToScreen(gptr, (char*) "Two Player Mode", gptr->display.width / 2,
				gptr->display.height / 2, regularFont_c);
	}
	char buffer[100];
	sprintf(buffer, "Most points after %d rounds wins!", gptr->maxRounds);
	next = drawTextToScreen(gptr, buffer, gptr->display.width / 2, next, regularFont_c);
	next = drawTextToScreen(gptr, (char*) "Press SPACE to begin", gptr->display.width / 2,
			next, regularFont_c);

	FEXIT();
} // end-of-function drawBeginGameScreen

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   drawRoundWinText
 @details
 --------------------------------------------------------------------------
 */
void drawRoundWinText(GameData *gptr) {
	FENTRY();

	char textBuffer[MAXBUFFER];

	sprintf(textBuffer, "Score: %s %d %s %d",
			gptr->player[lrt_c].name, gptr->player[lrt_c].carsSmashed, gptr->player[bus_c].name,
			gptr->player[bus_c].carsSmashed);
	int next = drawTextToScreen(gptr, textBuffer, gptr->display.width / 2,
			gptr->carArea.position.y - gptr->fontsize, regularFont_c);
	char buffer[100];
	sprintf(buffer, "Press SPACE to begin Round %d of %d or ESC to exit", gptr->roundNumber, gptr->maxRounds);
	drawTextToScreen(gptr, buffer, gptr->display.width / 2, gptr->carArea.position.y+gptr->carArea.height, regularFont_c);
	//DEBUG(" =======\n");

	for (int i = 0; i < 2; i++) {
		gptr->player[bus_c].keyPress[i] = false;
		gptr->player[lrt_c].keyPress[i] = false;
	} //end-of-for

	FEXIT();
} // end-of-function drawRoundWinText

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   drawGameWinText
 @details
 	 return false false for round win and true for game win
 --------------------------------------------------------------------------
 */
void drawGameWinText(GameData *gptr) {

	FENTRY();

	char textBuffer[MAXBUFFER];

	GamePlayer* ptr;
	if(gptr->player[bus_c].carsSmashed > gptr->player[lrt_c].carsSmashed) {
		ptr = &gptr->player[bus_c];
	}
	else if(gptr->player[bus_c].carsSmashed < gptr->player[lrt_c].carsSmashed) {
		ptr = &gptr->player[lrt_c];
	}
	else {
		ptr= NULL;
	}
	if(ptr == NULL) {
		sprintf(textBuffer, "It's a Draw!!!!");
	}
	else {
		sprintf(textBuffer, "%s Wins The Game!!", ptr->name);
	}
	int next = drawTextToScreen(gptr, textBuffer, gptr->display.width / 2,
			gptr->carArea.position.y - 3*gptr->fontsize, largeFont_c);
	sprintf(textBuffer, "Score: %s %d %s %d", gptr->player[lrt_c].name, gptr->player[lrt_c].carsSmashed,
			gptr->player[bus_c].name, gptr->player[bus_c].carsSmashed);
	next = drawTextToScreen(gptr, textBuffer, gptr->display.width / 2, next,
			regularFont_c);

	drawTextToScreen(gptr, (char*) "Press SPACE to begin or ESC to exit",
			gptr->display.width / 2, gptr->carArea.position.y+gptr->carArea.height, regularFont_c);

	playSound(gptr->winsample);

	for (int i = 0; i < 2; i++) {
		gptr->player[bus_c].keyPress[i] = false;
		gptr->player[lrt_c].keyPress[i] = false;
	} //end-of-for

	FEXIT();
} // end-of-function drawGameWinText


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 18, 2019
 @mname   drawScoreText
 @details
 \n
 --------------------------------------------------------------------------
 */
void drawScoreText(GameData *gptr) {
	FENTRY();

	char textBuffer[MAXBUFFER];
	sprintf(textBuffer, "Score: %s %d %s %d",
			gptr->player[lrt_c].name, gptr->player[lrt_c].carsSmashed, gptr->player[bus_c].name,
			gptr->player[bus_c].carsSmashed);
	int next = drawTextToScreen(gptr, textBuffer, gptr->display.width -100,
			30, smallFont_c);
	sprintf(textBuffer, "Remain: %d",
			gptr->remainingCars);
	next = drawTextToScreen(gptr, textBuffer, gptr->display.width -100,
			next, smallFont_c);
	sprintf(textBuffer, "%d Points Per Smash",
			gptr->scorePointsPerSmash);
	next = drawTextToScreen(gptr, textBuffer, gptr->display.width -100,
			next, smallFont_c);
	sprintf(textBuffer, "%d Bounces since Last Smash",
			gptr->stats.bounceUntilSmash[gptr->stats.firstEmpty]);
	next = drawTextToScreen(gptr, textBuffer, gptr->display.width -100,
			next, smallFont_c);
	sprintf(textBuffer, "%d Total Bounces",
			gptr->stats.totalBounce);
	next = drawTextToScreen(gptr, textBuffer, gptr->display.width -100,
			next, smallFont_c);
	if(gptr->gamePaused == true) {
		sprintf(textBuffer, "** Game is Paused! Press P to resume **");
		next = drawTextToScreen(gptr, textBuffer, gptr->display.width -100,
				next, smallFont_c);
	}
	FEXIT();
}//end-of-drawScoreText

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 18, 2019
 @mname drawHelpToScreen
 @details
 \n
 --------------------------------------------------------------------------
 */
void drawHelpText(GameData *gptr) {
	FENTRY();

	char textBuffer[MAXBUFFER];
	const int xpos_c = 100;
	sprintf(textBuffer, "Level: %s %d %s %d",
			gptr->player[lrt_c].name,
			gptr->botLevel[lrt_c]+1,
			gptr->player[bus_c].name,
			gptr->botLevel[bus_c]+1);
	int next = drawTextToScreen(gptr, textBuffer, xpos_c,
			30, smallFont_c);

	sprintf(textBuffer, "Collision Algo: %d (C)",
			gptr->cAlgoSelector?2:1);
	next = drawTextToScreen(gptr, textBuffer, xpos_c,
			next, smallFont_c);

	sprintf(textBuffer, "Game Mode: %d",
			gptr->gameMode);
	next = drawTextToScreen(gptr, textBuffer, xpos_c,
			next, smallFont_c);

	sprintf(textBuffer, "New Window: %s (G)",
			gptr->path.separateDisplay?"Y":"N");
	next = drawTextToScreen(gptr, textBuffer, xpos_c,
			next, smallFont_c);

	sprintf(textBuffer, "Trajectory On: %s (T)",
			gptr->path.rec?"Y":"N");
	next = drawTextToScreen(gptr, textBuffer, xpos_c,
			next, smallFont_c);
}//end-of-drawHelpToScreen

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 28, 2019
 @mname   drawPlayerBitmap
 @details
 Draws only a selected portion of a bitmap.
 It is used to change the length of the pallete depending on the game level.\n
 --------------------------------------------------------------------------
 */
void  drawPlayerBitmap(GameBasicBlock *g) {
	al_draw_bitmap_region(g->bmap, 0, 0, g->width, g->height, g->position.x,
			g->position.y, 0);
} // end-of-function drawBitmapSection

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   drawMainGameScreen
 @details
 This function draws the players, ball, score data, and help (if help is on)
 Has to be called every time we want to refresh the display during gameplay\n
 --------------------------------------------------------------------------
 */
void  drawMainGameScreen(GameData *gptr) {
	FENTRY();

	drawPlayerBitmap(&(gptr->player[bus_c].ge));
	drawPlayerBitmap(&(gptr->player[lrt_c].ge));
	al_draw_bitmap(gptr->ball.bmap, gptr->ball.position.x, gptr->ball.position.y, 0);
	for (int i = 0; i < gptr->maxRows; i++) {
		for (int j = 0; j < gptr->maxColumns; j++) {
			if (gptr->cars[i][j].onScreen == true) {
				al_draw_bitmap(gptr->cars[i][j].bmap, gptr->cars[i][j].position.x, gptr->cars[i][j].position.y, 0);
			}//end-of-if
		} //end-of-for
	} //end-of-for
	drawScoreText(gptr);
	if(gptr->helpOn) drawHelpText(gptr);
	if(gptr->path.rec == true && gptr->path.separateDisplay == false) {
		for (int i = 1; i < gptr->path.used; i++ ) {
			al_draw_line(gptr->path.point[i-1].x, gptr->path.point[i-1].y, gptr->path.point[i].x, gptr->path.point[i].y, al_map_rgb(255, 0,0), 1.0);
		} //end-of-for
		al_draw_line(gptr->path.point[gptr->path.used-1].x, gptr->path.point[gptr->path.used-1].y, gptr->ball.position.x, gptr->ball.position.y, al_map_rgb(255, 0,0), 1.0);
	}
	FEXIT();
} // end-of-function drawMainGameScreen


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   initializeAllegro
 @details
 returns 1 if init ok, 0 otherwise
 This function does the following:
 1. Initializes all allegro resources
 2. Loads all game resources (fonts, bitmaps, sounds)
 --------------------------------------------------------------------------
 */
bool initializeAllegro(GameData *p) {
	FENTRY();

	//initiallises allegro libraries
	if(al_init() == 0) {
		ERROR("Cannot init allegro");
		FEXIT();
		return false;
	} //end-of-if(al_init() == 0)

	al_init_primitives_addon();
	al_init_image_addon();
	al_install_keyboard();
	al_install_mouse();
	al_init_font_addon(); // initialize the font addon
	al_init_ttf_addon(); // initialize the ttf (True Type Font) addon
	al_install_audio();
	al_init_acodec_addon();
	//al_reserve_samples(2);


	//tries to load font file
	if (loadFont(p, smallFont_c) == false) {
		FEXIT();
		return false;
	} //end-of-if(LoadFont(p, smallFont_c) == false)


	if (loadFont(p, regularFont_c) == false) {
		FEXIT();
		return false;
	} //end-of-if(LoadFont(p, regularFont_c) == false)


	if (loadFont(p, largeFont_c) == false) {
		FEXIT();
		return false;
	} //end-of-if(LoadFont(p, largeFont_c) == false)


	if ((p->display.display = al_create_display(p->display.width,
			p->display.height)) == NULL) {
		ERROR("Cannot init display");
		FEXIT();
		return false;
	}

	//TRACE();
	p->bcolorarray[yellow_c] = al_map_rgb(255, 255, 0);
	p->bcolorarray[blue_c] = al_map_rgb(200, 200, 255);
	p->bcolorarray[grey_c] = al_map_rgb(180, 180, 180);
	p->bcolorarray[white_c] = al_map_rgb(255, 255, 255);
	p->bcolorarray[green_c] = al_map_rgb(0, 180, 0);

	p->fontColor = al_map_rgb(0, 100, 0);
	p->timer = al_create_timer(1.0 / p->fps);
	p->eventqueue = al_create_event_queue();
	if (al_is_event_queue_empty(p->eventqueue) == false) {
		ERROR("Event queue not empty after creation");
	}

	al_register_event_source(p->eventqueue, al_get_keyboard_event_source());
	al_register_event_source(p->eventqueue,
			al_get_display_event_source(p->display.display));
	al_register_event_source(p->eventqueue,
			al_get_timer_event_source(p->timer));

	if (loadPlayerImage(&(p->player[bus_c]), p->busBitmapName) == false) {
		FEXIT();
		return false;
	}
	if (loadPlayerImage(&(p->player[lrt_c]), p->lrtBitmapName) == false) {
		FEXIT();
		return false;
	}
	if (loadBitmap(&(p->ball), p->ballBitmapName) == false) {
		FEXIT();
		return false;
	}

	if ((p->gasBitmap = al_load_bitmap(p->gasBitmapName)) == NULL) {
		ERROR2("cannot load", p->gasBitmapName);
		FEXIT();
		return false;
	}

	if ((p->ecarBitmap = al_load_bitmap(p->ecarBitmapName)) == NULL) {
		ERROR2("cannot load", p->ecarBitmapName);
		FEXIT();
		return false;
	}

	loadAudio(&(p->player[bus_c]));
	loadAudio(&(p->player[lrt_c]));
	loadWinnerSound(p);

	FEXIT();
	return true;
} // end-of-function initializeGraphics

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   allegroCleanup
 @details
 This function is called when the game terminates and it destroys all allegro resources
 \n
 --------------------------------------------------------------------------
 */
void  allegroCleanup(GameData *gptr) {
	FENTRY();

	al_rest(0.0);
	al_destroy_display(gptr->display.display);
	al_destroy_timer(gptr->timer);
	al_destroy_event_queue(gptr->eventqueue);
	for (int i = 0; i < MAXFONTS; i++) {
		if (gptr->font[i]) {
			al_destroy_font(gptr->font[i]);
		} //end-of-if(p->font[i])
	} //end-of-for

	al_destroy_bitmap(gptr->player[bus_c].ge.bmap);
	al_destroy_bitmap(gptr->player[lrt_c].ge.bmap);
	al_destroy_bitmap(gptr->ball.bmap);
	FEXIT();
} // end-of-function graphicsCleanup


//===== Ball Movement and Collisions  ('Physics') ===========
/*
 * @author   mlambiri
 * @date     Dec. 30, 2019
 *  This section contains the functions that deal with the
 *  ball movement and collisions
 *
 */


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 30, 2019
 @mname   increaseBallSpeed
 @details
   increase one component of the ball speed
   very useful when we get into an infinite ricochet loop\n
 --------------------------------------------------------------------------
 */
void increaseBallSpeed(GameData* g) {
	g->maxspeed.x += 2;
	g->maxspeed.y += 2;
}


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 30, 2019
 @mname   decreaseBallSpeed
 @details
    decrease the largest component of speed
    very useful to get out of infinite ricochet loops\n
 --------------------------------------------------------------------------
 */
void decreaseBallSpeed(GameData* g) {
	g->maxspeed.x -= 2;
	g->maxspeed.y -= 2;
}


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 17, 2019
 @mname   checkCollisionLeftRight
 @details
 true if there is a collision with top or bottom\n
 --------------------------------------------------------------------------
 */
bool checkCollisionLeftRight(GameData *gptr) {
	FENTRY();

	bool collision = false;
	if (gptr->ball.position.x > (gptr->display.width - gptr->ball.width)) {
		gptr->ball.position.x = gptr->display.width - gptr->ball.width;
		if (gptr->ball.speed.x > 0)
			gptr->ball.speed.x *= -1;
		recordPoint(&(gptr->path), &(gptr->ball.position));
		gptr->stats.totalBounce++;
		gptr->stats.bounceUntilSmash[gptr->stats.firstEmpty]++;
		collision = true;
	} else if (gptr->ball.position.x < 0) {
		gptr->ball.position.x = 0;
		if (gptr->ball.speed.x < 0)
			gptr->ball.speed.x *= -1;
		recordPoint(&(gptr->path), &(gptr->ball.position));
		gptr->stats.totalBounce++;
		gptr->stats.bounceUntilSmash[gptr->stats.firstEmpty]++;
		collision = true;
	}

	FEXIT();
	return collision;
} // end-of-function checkCollisionLeftRight

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 17, 2019
 @mname   checkCollisionTopAndBottom
 @details
 Checks if the ball hits either player's side of the field and grants a roundwin\n
 --------------------------------------------------------------------------
 */
bool checkCollisionTopAndBottom(GameData *gptr) {
	FENTRY();

	bool collision = false;
	if (((gptr->ball.position.y + gptr->ball.height )>= gptr->display.height )
			&& (gptr->ball.speed.y > 0)) {
		gptr->player[1].carsSmashed += gptr->penalty;
		gptr->roundWinner = &(gptr->player[lrt_c]);
		recordPoint(&(gptr->path), &(gptr->ball.position));
		gptr->stats.totalBounce++;
		gptr->stats.bounceUntilSmash[gptr->stats.firstEmpty]++;
		collision =  true;

	} else if ((gptr->ball.position.y <= 0) && (gptr->ball.speed.y < 0)) {
		TRACE();
		gptr->player[bus_c].carsSmashed += gptr->penalty;
		gptr->roundWinner = &(gptr->player[bus_c]);
		recordPoint(&(gptr->path), &(gptr->ball.position));
		gptr->stats.totalBounce++;
		gptr->stats.bounceUntilSmash[gptr->stats.firstEmpty]++;
		collision =  true;
	}
	FEXIT();
	return collision;
} // end-of-function checkCollisionTopAndBottom


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 28, 2019
 @mname   signOfNumber
 @details
 \n
 --------------------------------------------------------------------------
 */
int signOfNumber(int value) {

	if (value >= 0) {
		return 1;
	} //end-of-if(value > 0)
	return -1;
} // end-of-function signOfNumber

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 19, 2019
 @mname   isPointInObject
 @details
  check if a point of the ball is in another object\n
 --------------------------------------------------------------------------
 */
bool isPointInObject(GameBasicBlock* b, int x, int y){
	FENTRY();

	bool isin = false;
	if((x>=b->position.x)
			&& (x <= (b->position.x+b->width))
			&&(y>=b->position.y)
			&& (y<=(b->position.y+b->height))) {
		isin = true;
	}

	FEXIT();
	return isin;
}


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 30, 2019
 @mname   isPointInAnyCar
 @details
  return false if the point denoted by coordinates (x,y) is
  not in any car.
  row and column parameters will return the coordinate
  of the car in case the point is in one of them\n
 --------------------------------------------------------------------------
 */
bool isPointInAnyCar(GameData* g,  int x, int y, int&row, int&column){
	FENTRY();


	// check if the point is in the car region
	GameBasicBlock* obj = &(g->carArea);
	bool result = isPointInObject(obj,x, y);

	if(result == false) {
		// not in car area, we can return
		FEXIT();
		return false;
	}

	column = (x - g->cars[0][0].position.x) / g->cars[0][0].width;
	row = (y - g->cars[0][0].position.y) / g->cars[0][0].height;

	if(row >= g->maxRows) {
		FEXIT();
		return false;
	}

	if(column >= g->maxColumns) {
		FEXIT();
		return false;
	}

	if( isPointInObject(&(g->cars[row][column]), x, y) == false) {
		FEXIT();
		return false;
	}

	if(g->cars[row][column].onScreen == false) {
		// this is an empty space on screen
		FEXIT();
		return false;
	}
	else {
		FEXIT();
		return true;
	}
}


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 19, 2019
 @mname   isBallInRegion
 @details
   check if the ball is in a particular rectangle
 --------------------------------------------------------------------------
 */
bool isBallInRegion(GameBasicBlock* ball, GameBasicBlock* obj){
	FENTRY();

	bool result = isPointInObject(obj,ball->position.x,ball->position.y);
	result = result || isPointInObject(obj, ball->position.x+ball->width, ball->position.y);
	result = result || isPointInObject(obj,ball->position.x, ball->position.y+ball->height);
	result = result || isPointInObject(obj, ball->position.x+ball->width, ball->position.y+ball->height);

	FEXIT();
	return result;

}

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 19, 2019
 @mname   areObjectsColliding
 @details
  check if the ball is colliding with another objects
  the side parameter will return the side of the collision
  this side refers to the object not the ball\n
 --------------------------------------------------------------------------
 */
bool areObjectsColliding(GameBasicBlock* ball, GameBasicBlock* obj, COLLISIONSIDE& side){
	FENTRY();

	bool result = isPointInObject(obj,ball->position.x,ball->position.y);
	result = result || isPointInObject(obj, ball->position.x+ball->width, ball->position.y);
	result = result || isPointInObject(obj,ball->position.x, ball->position.y+ball->height);
	result = result || isPointInObject(obj, ball->position.x+ball->width, ball->position.y+ball->height);

	if(result == false) {
		FEXIT();
		return false;
	}

	float ballXCenter = ball->prevposition.x + (float) ball->width/2;
	float ballYCenter = ball->prevposition.y + (float) ball->height/2;

	float objXCenter = obj->position.x + (float) obj->width/2;
	float objYCenter = obj->position.y + (float) obj->height/2;

	if(ball->prevposition.x == (obj->position.x+obj->width)) {
		side = right_c;
		FEXIT();
		return true;
	}

	if((ball->prevposition.x + ball->width) == obj->position.x) {
		side = left_c;
		FEXIT();
		return true;
	}

	if(ball->prevposition.y == (obj->position.y+obj->height)) {
		side = bottom_c;
		FEXIT();
		return true;
	}

	if((ball->prevposition.y + ball->height) == obj->position.y) {
		side = top_c;
		FEXIT();
		return true;
	}

	float o1a = atan((float)obj->width/(float)obj->height);

	// above left
	if(ballYCenter < objYCenter) {
		if(ballXCenter <= objXCenter) {
			float bAngle = atan((float)(objXCenter - ballXCenter)/(objYCenter - ballYCenter));
			if(bAngle < o1a) side = top_c;
			else side = left_c;
		}
		else {
			float bAngle = atan((float)( ballXCenter - objXCenter)/(objYCenter - ballYCenter));
			if(bAngle < o1a) side = top_c;
			else side = right_c;
		}
	} else if(ballYCenter == objYCenter) {
		if(ballXCenter <= objXCenter) {
			side = left_c;
		}
		else {
			side = right_c;
		}
	} else if(ballYCenter > objYCenter) {
		if(ballXCenter <= objXCenter) {
			float bAngle = atan((float)(objXCenter - ballXCenter)/(ballYCenter - objYCenter));
			if(bAngle < o1a) side = bottom_c;
			else side = left_c;
		}
		else {
			float bAngle = atan((float)( ballXCenter - objXCenter)/(ballYCenter - objYCenter));
			if(bAngle < o1a) side = bottom_c;
			else side = right_c;
		}
	}

	FEXIT();
	return true;
}

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 19, 2019
 @mname   whenCollisionOccur
 @details
   this function returns true if the ball has collided with a brick
   and false otherwise\n
 --------------------------------------------------------------------------
 */
bool whenCollisionOccurs(GameData* gptr, int i, int j) {
	FENTRY();

	if (gptr->cars[i][j].onScreen == false) {
		FEXIT();
		return false;
	}

	COLLISIONSIDE side;

	if(areObjectsColliding(&(gptr->ball), &(gptr->cars[i][j]), side)) {

		if(gptr->ball.speed.y == 0) {
			gptr->ball.speed.y = rand() %5 -2;
		}
		if(gptr->ball.speed.x == 0) {
			gptr->ball.speed.x = rand() %5 -2;
		}

		switch(side){
		case top_c:
			gptr->ball.speed.y *= -1;
			gptr->ball.speed.x += rand() % 2;
			gptr->ball.position.y = gptr->cars[i][j].position.y - gptr->ball.height;
			DEBUG("top");
			break;
		case bottom_c:
			gptr->ball.speed.y *= -1;
			gptr->ball.speed.x += rand() % 2;
			gptr->ball.position.y = gptr->cars[i][j].position.y + gptr->cars[i][j].height;
			DEBUG("bottom");
			break;
		case left_c:
			gptr->ball.position.x = gptr->cars[i][j].position.x - gptr->ball.width;
			gptr->ball.speed.x *= -1;
			DEBUG("left");
			break;
		case right_c:
			gptr->ball.speed.x *= -1;
			gptr->ball.position.x = gptr->cars[i][j].position.x + gptr->cars[i][j].width;
			DEBUG("right");
			break;
		default:
			DEBUG("default");
			break;
		}

		ballSpeedLimiter(gptr);

		recordPoint(&(gptr->path), &(gptr->ball.position));
		gptr->stats.totalBounce++;
		gptr->stats.bounceUntilSmash[gptr->stats.firstEmpty]++;

		if(gptr->cars[i][j].indestructible == false) {
			gptr->cars[i][j].onScreen = false;
			gptr->remainingCars--;
			gptr->stats.firstEmpty++;
			gptr->stats.bounceUntilSmash[gptr->stats.firstEmpty] = 0;
			if(gptr->turn)
				gptr->turn->carsSmashed+= gptr->scorePointsPerSmash;
			setPointsPerSmash(gptr);
		}
		if(gptr->remainingCars < level5_c ) {
			gptr->backgroundColor = &(gptr->bcolorarray[green_c]);
		}else if(gptr->remainingCars < level4_c ) {
			gptr->backgroundColor = &(gptr->bcolorarray[blue_c]);
		}

		FEXIT();
		return true;
	}

	FEXIT();
	return false;
}


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 19, 2019
 @mname   isBallBrickCollisionPossible
 @details
   this function returns true if the ball has collided with a brick
   and false otherwise\n
 --------------------------------------------------------------------------
 */
bool isBallCarCollisionPossible(GameData* gptr, GameBasicBlock* tmpBall, int i, int j) {
	FENTRY();

	if (gptr->cars[i][j].onScreen == false) {
		FEXIT();
		return false;
	}

	COLLISIONSIDE side;

	if(areObjectsColliding(tmpBall, &(gptr->cars[i][j]), side)) {

		switch(side){
		case top_c:
			if(i > 0) {
				// check is there is another car on top
				if(gptr->cars[i-1][j].onScreen == true ) {
					FEXIT();
					return false;
				}
			}
			tmpBall->position.y = gptr->cars[i][j].position.y - tmpBall->height;
			if(tmpBall->speed.y < 0) {
				FEXIT();
				return false;
			}
			DEBUG("p-top");
			break;
		case bottom_c:
			if(i < gptr->maxRows-1) {
				// check is there is another car underneath
				if(gptr->cars[i+1][j].onScreen == true ) {
					FEXIT();
					return false;
				}
			}
			tmpBall->position.y = gptr->cars[i][j].position.y + gptr->cars[i][j].height;
			if(tmpBall->speed.y > 0) {
				FEXIT();
				return false;
			}
			DEBUG("p-bottom");
			break;
		case left_c:
			if(j >0) {
				// check is there is another car to the left
				if(gptr->cars[i][j-1].onScreen == true ) {
					FEXIT();
					return false;
				}
			}
			tmpBall->position.x = gptr->cars[i][j].position.x - tmpBall->width;
			if(tmpBall->speed.x < 0) {
				FEXIT();
				return false;
			}
			DEBUG("p-left");
			break;
		case right_c:
			if(j < gptr->maxColumns-1) {
				// check is there is another car underneath
				if(gptr->cars[i][j+1].onScreen == true ) {
					FEXIT();
					return false;
				}
			}
			tmpBall->position.x = gptr->cars[i][j].position.x + gptr->cars[i][j].width;
			if(tmpBall->speed.x > 0) {
				FEXIT();
				return false;
			}
			DEBUG("p-right");
			break;
		default:
			break;
		}

		FEXIT();
		return true;
	}

	FEXIT();
	return false;
}

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 28, 2019
 @mname   ballBounceOnPlayer
 @details
 This function changes the direction of the ball after a collision with the
 pallete.
 The pallete height is divided into zones
 Depending on which zone makes contact the ball will react in a different way
 This is to increase the unpredictability of the game and make it more fun\n
 --------------------------------------------------------------------------
 */
void  ballBounceOnPlayer(GameBasicBlock *ball, GamePlayer *playerPtr,
		int maxballspeed) {

	FENTRY();

	int newspeedy = abs(ball->speed.y) + (rand() % minballspeed_c);
	if (newspeedy > maxballspeed)
		newspeedy = maxballspeed;
	ball->speed.y = signOfNumber(ball->speed.y) * -1 * newspeedy;

	int paddleZones = playerPtr->paddleSize;
	int zonelength = playerPtr->ge.width / paddleZones;
	int zoneHitByBall = (ball->position.x - playerPtr->ge.position.x) / zonelength;

	if (zoneHitByBall <= 0) {
		zoneHitByBall = 0;
	}
	else if (zoneHitByBall > paddleZones - 1) {
		zoneHitByBall = paddleZones - 1;
	}

	switch (paddleZones) {
	case 1:
		ball->speed.x = (-5)* signOfNumber(ball->speed.x)*(rand()%(zoneHitByBall+1));
		break;
	case 2:
		ball->speed.x = (-5)* signOfNumber(ball->speed.x)*(rand()%(zoneHitByBall+1) - paddleZones/2);
		break;
	case 3:
		ball->speed.x = (-4)* signOfNumber(ball->speed.x)*(rand()%(zoneHitByBall+1) - paddleZones/2);
		break;
	case 4:
		ball->speed.x = (-3)* signOfNumber(ball->speed.x)*(rand()%(zoneHitByBall+1) - paddleZones/2);
		break;
	default:
		ball->speed.x = (-2)* signOfNumber(ball->speed.x)*(rand()%(zoneHitByBall+1) - paddleZones/2);
		break;
	} //end-switch(paddleZones)

	DEBUG2("speed.x", ball->speed.x);
	//	ball->speed.x += playerPtr->ge.speed.x;

	playSound(playerPtr->sample);
	FEXIT();

} // end-of-function ballBounceOnPlayer

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 17, 2019
 @mname   checkBallCollisionWithPlayers
 @details
 true if there is a collision false otherwise
 Player one is the *bottom* edge
 Player two it is the *top* edge.

 --------------------------------------------------------------------------
 */
bool checkBallCollisionWithPlayers(GameData *gptr) {
	FENTRY();

	COLLISIONSIDE side;
	// check collision with bus
	if(areObjectsColliding(&(gptr->ball), &(gptr->player[bus_c].ge), side)) {
		if(side == left_c || side == right_c) {
			FEXIT();
			return false;
		}
		gptr->ball.position.y = gptr->player[bus_c].ge.position.y - gptr->ball.height;
		ballBounceOnPlayer(&(gptr->ball), &(gptr->player[bus_c]), gptr->maxballspeed);
		gptr->turn = &gptr->player[bus_c];
		recordPoint(&(gptr->path), &(gptr->ball.position));
		gptr->stats.totalBounce++;
		gptr->stats.bounceUntilSmash[gptr->stats.firstEmpty]++;
		FEXIT();
		return true;
	}
	//check collision with lrt
	else if(areObjectsColliding(&(gptr->ball), &(gptr->player[lrt_c].ge), side)) {
		if(side == left_c || side == right_c) {
			FEXIT();
			return false;
		}
		gptr->ball.position.y = gptr->player[lrt_c].ge.position.y + gptr->player[lrt_c].ge.height;
		ballBounceOnPlayer(&(gptr->ball), &(gptr->player[lrt_c]), gptr->maxballspeed);
		gptr->turn = &gptr->player[lrt_c];
		recordPoint(&(gptr->path), &(gptr->ball.position));
		gptr->stats.totalBounce++;
		gptr->stats.bounceUntilSmash[gptr->stats.firstEmpty]++;
		FEXIT();
		return true;
	}

	FEXIT();
	return false;
} // end-of-function checkCollisionWithPlayers

/**
  --------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov. 30, 2019
   @name    max
   @param   uint, uint
   @return  uint
   @details
	\n
  -------------------------------------------------------------------------
 */
uint max(uint a, uint b) {
	if(a>b) return a;
	else return b;
}

/**
  --------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov. 30, 2019
   @name    min
   @param   a, b
   @return  int
   @details
	\n
  -------------------------------------------------------------------------
 */
uint min(uint a, uint b) {
	if(a>b) return b;
	else return a;
}

/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Dec. 30, 2019
   @mname   ballSpeedLimiter
   @details
	  \n
  --------------------------------------------------------------------------
 */
void
ballSpeedLimiter(GameData* gptr) {
	FENTRY();

	if(abs(gptr->ball.speed.x) > gptr->maxspeed.x) {
		gptr->ball.speed.x = signOfNumber(gptr->ball.speed.x)*gptr->maxspeed.x;
	}
	if(abs(gptr->ball.speed.y) > gptr->maxspeed.y) {
		gptr->ball.speed.y += signOfNumber(gptr->ball.speed.y)*gptr->maxspeed.y;
	}
	FEXIT();
} // end-of-method ballSpeedLimiter



/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   updateBallPosition
 @details
 return true if no more gas cars remain to be smashed
 This function checks if there is a collision between the ball and an object
  as well as for collisions with the top and bottom of the 'field'
  Because x and y speeds can be larger than the size of the car
  we can have a 'tunnelling' effect (described as 'bullet through
  paper' at page YYY in the Game Architecture book.\n
 --------------------------------------------------------------------------
 */
bool updateBallPosition(GameData *gptr) {
	FENTRY();

	if(gptr->remainingCars == 0) {
		FEXIT();
		return true;
	}

	bool ballUpdated = false;

	// there are 2 algorithms coded
	// i could not get rid of the tunnelling effect with either of them
	// it happens in some cases in both but the conditions are
	// different. the algo selection is done through the
	// configuration file using the 'calgo' variable
	// when 'calgo = 1' then cAlgoSelector is set to 'false'
	// when 'calgo = 2' then cAlgoSelector is set to 'true'
	if(gptr->cAlgoSelector == false) { // 'calgo = 1'

		// use a temporary ball object
		GameBasicBlock tmpBall = gptr->ball;

		// update the temporary ball object and use that to check new location
		tmpBall.position.x = gptr->ball.position.x + gptr->ball.speed.x;
		tmpBall.position.y = gptr->ball.position.y + gptr->ball.speed.y;

		if(isBallInRegion(&tmpBall, &(gptr->carArea)) == true) {
			// check collision with cars only if ball will be in the car area
			// update the actual ball position and speed
			ballSpeedLimiter(gptr);
			gptr->ball.prevposition.x = gptr->ball.position.x;
			gptr->ball.prevposition.y = gptr->ball.position.y;
			gptr->ball.position.x += gptr->ball.speed.x;
			gptr->ball.position.y += gptr->ball.speed.y;
			ballUpdated = true;


			// this collision detection checks all rows and columns
			// it is simple and effective as it checks all cars one by one
			// if the speed is 'negative' (ie the ball moves from the bottom towards top)
			// then we can start checking from (maxRows, maxColumns)
			// this will detect the collisions faster
			bool done = false;
			if(gptr->ball.speed.y <= 0) {
				for (int i = gptr->maxRows-1; i >=0; i--) {
					for (int j = 0; j < gptr->maxColumns; j++) {
						if(whenCollisionOccurs(gptr, i, j)) {
							done = true;
							break;
						}
					} //end-of-for
					if(done) break;
				} //end-of-for
			}
			else if(gptr->ball.speed.y > 0) {
				// if the speed is 'positive' (ie ball moves from top to bottom)
				// we can to the check from (0,0) to (maxRow, maxColumn)
				for (int i = 0; i < gptr->maxRows; i++) {
					for (int j = 0; j < gptr->maxColumns; j++) {
						if(whenCollisionOccurs(gptr, i, j)) {
							done = true;
							break;
						}
					} //end-of-for
					if(done) break;
				} //end-of-for

				// if a collision with a car occured we can exit
				// there is no need to check for collisions with the rest of the items
				if(done) {
					FEXIT();
					return false;
				}
			}
		}
	}
	else { // 'calgo = 2'
		ballSpeedLimiter(gptr);
#if 0
		uint maxOfxy = max(abs(gptr->ball.speed.x), abs(gptr->ball.speed.y));
		float xplus = (float) gptr->ball.speed.x / maxOfxy;
		float yplus = (float)  gptr->ball.speed.y / maxOfxy;
#else
		uint maxOfxy = 2;
		float xplus = (float) gptr->ball.speed.x ;
		float yplus = (float)  gptr->ball.speed.y;
#endif
		bool collision = false;
		// use a temporary ball to do the checks
		GameBasicBlock tmpBall = gptr->ball;

		for (int i = 1; i < maxOfxy; i++ ) {

			float txf = tmpBall.position.x + i*xplus;
			float tyf = tmpBall.position.y + i*yplus;
			tmpBall.prevposition.x = gptr->ball.position.x;
			tmpBall.prevposition.y = gptr->ball.position.y;
			tmpBall.position.x = (int) txf ;
			tmpBall.position.y = (int) tyf;

			if(tmpBall.position.x < 0) tmpBall.position.x = 0;
			if(tmpBall.position.x > gptr->display.width) tmpBall.position.x = gptr->display.width;
			if(tmpBall.position.y < 0) tmpBall.position.y = 0;
			if(tmpBall.position.y > gptr->display.height) tmpBall.position.y = gptr->display.height;

			int xdistance = tmpBall.position.x - gptr->cars[0][0].position.x;
			int ydistance = tmpBall.position.y - gptr->cars[0][0].position.y ;
			int row = 0;
			int column = 0;

			if( xdistance > 0) {
				column = xdistance / gptr->cars[0][0].width;
				if(column > gptr->maxColumns) column = gptr->maxColumns;
			}
			if( ydistance > 0) {
				row = ydistance / gptr->cars[0][0].height;
				if(row > gptr->maxRows) row = gptr->maxRows;
			}

			const int d_c = 2;
			int minRow = (row-d_c)<0?0:row-d_c;
			int maxRow = (row+d_c) >gptr->maxRows?gptr->maxRows:row+d_c;
			int minColumn = (column-d_c)<0?0:column-d_c;
			int maxColumn = (column+d_c) >gptr->maxColumns?gptr->maxColumns:column+d_c;
			bool possible = false;

			for (int i = minRow; i < maxRow; i++) {
				for (int j = minColumn; j < maxColumn; j++) {
					if(isBallCarCollisionPossible(gptr, &tmpBall, i, j)) {
						possible = true;
						row = i;
						column = j;
						break;
					}
				} //end-of-for
				if(possible) break;
			} //end-of-for

			if(possible == true) {
				gptr->ball.prevposition.x = gptr->ball.position.x;
				gptr->ball.prevposition.y = gptr->ball.position.y;
				gptr->ball.position.x = tmpBall.position.x;
				gptr->ball.position.y = tmpBall.position.y;
				ballUpdated = true;

				if(whenCollisionOccurs(gptr, row, column)) {
					collision = true;
					break;
				}
				else {
					ballSpeedLimiter(gptr);
				}
			}
		} //end-of-for

		if(collision == true) {
			FEXIT();
			return false;
		}
	} // end-of-collision algorithms

	if(ballUpdated == false) {
		ballSpeedLimiter(gptr);
		// update the ball position
		gptr->ball.prevposition.x = gptr->ball.position.x;
		gptr->ball.prevposition.y = gptr->ball.position.y;
		gptr->ball.position.x +=  gptr->ball.speed.x;
		gptr->ball.position.y += gptr->ball.speed.y;
	}

	if (checkBallCollisionWithPlayers(gptr) == false) {
		if (checkCollisionTopAndBottom(gptr) == true) {
			FEXIT();
			return true;
		}
		else {
			checkCollisionLeftRight(gptr);
		}
	}
	else {
		checkCollisionLeftRight(gptr);
	}
	FEXIT();
	return false;
} // end-of-function updateBallPosition



//=========== Game Controls =======================
/*
 * @author   mlambiri
 * @date     Dec. 30, 2019
 *  This section groups the functions that deal with the control of
 *  the game by the users (either bots or human)
 *  This section contains the main game loop, timers, keyboard event
 *  processing, writing to files, game termination, etc
 */


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 19, 2019
 @mname   setPointsPerSmash
 @details
   set an amount of points for each car smashed
   this will change based on the number of cars that
   are still present on screen\n
 --------------------------------------------------------------------------
 */
void  setPointsPerSmash(GameData*gptr) {
	FENTRY();

	if(gptr->remainingCars<= level6_c){
		gptr->scorePointsPerSmash = 10;
	}else if (gptr->remainingCars< level5_c) {
		gptr->scorePointsPerSmash = 3;
	}else if (gptr->remainingCars< level4_c) {
		gptr->scorePointsPerSmash = 2;
	}else {
		gptr->scorePointsPerSmash = 1;
	}

	FEXIT();
} //end of setPointsPerSmash

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 25, 2019
 @mname   writeCarLayoutToFile
 @details
   writes the car layout to a file\n
 --------------------------------------------------------------------------
 */
bool writeCarLayoutToFile(GameData* g) {
	FENTRY();

	char text[MAXBUFFER];

	FILE* fptr = NULL;

	char filename[MAXBUFFER];
	sprintf(filename, "layout%d.txt", rand()%10000);
	fptr = fopen( filename, "w");
	if (fptr == NULL) {
		FEXIT();
		return false;
	} //end-of-if(fptr == NULL)

	for (int i = 0; i < g->maxRows; i++ ) {
		for (int j = 0; j < g->maxColumns; j++ ) {
			if( g->cars[i][j].onScreen == false) {
				fprintf(fptr, "x");
			}else if(g->cars[i][j].indestructible == false) {
				fprintf(fptr, "g");
			} else {
				fprintf(fptr, "e");
			}
		} //end-of-for
		fprintf(fptr, "\n");
	} //end-of-for

	fclose(fptr);
	FEXIT();
	return true;
} //end-of-writeCarLayoutToFile

/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Dec. 2, 2019
   @mname   recordPoint
   @details
	  \n
  --------------------------------------------------------------------------
 */
bool recordPoint(DataRecorder* r, Point* p) {
	// recorder not on
	if(r->rec == false) return false;
	// recorder full
	if(r->used >= MAXRECORDING-1) return false;
	r->point[r->used++] = *p;

	return true;
} // end-of-method recordPoint


/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Jan. 1, 2020
   @mname   startRound
   @details
	  \n
  --------------------------------------------------------------------------
 */
void startRound(GameData* gptr) {
	FENTRY();
	if(gptr->gameStart == false) {
		writeCarLayoutToFile(gptr);
		if(gptr->path.rec == true && gptr->path.separateDisplay) {
			createTrajectoryDisplay(gptr);
		}
		gptr->gameStart = true;
	}
	else if(gptr->roundWin == true) {
		gptr->roundWin = false;
		if(gptr->gameWin == true ) {
			gptr->roundNumber = 1;
			const char* mode;
			char textBuffer[MAXBUFFER];
			if(gptr->gameMode == fullbot_c) {
				mode = "Full Auto";
			}
			else if (gptr->gameMode == arcade_c) {
				mode = "Arcade";
			} else {
				mode = "Human";
			}
			sprintf(textBuffer, "[Mode: %s] [Score: %s %d %s %d]", mode, gptr->player[lrt_c].name, gptr->player[lrt_c].carsSmashed,
					gptr->player[bus_c].name, gptr->player[bus_c].carsSmashed);
			writeGameResultToFile(textBuffer, &(gptr->stats));
			generateNewCarLayout(gptr);
			gptr->gameWin = false;
		}
		al_flush_event_queue(gptr->eventqueue);
		playSound(gptr->startsample);
	}
	FEXIT();
} // end-of-method startRound



/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Jan. 1, 2020
   @mname   generateNewCarLayout
   @details
	  \n
  --------------------------------------------------------------------------
 */
void generateNewCarLayout(GameData* gptr) {
	FENTRY();
	gptr->backgroundColor = gptr->initcolor;
	createCarLayout(gptr);
	setCarGraphics(gptr);
	writeCarLayoutToFile(gptr);
	gptr->player[bus_c].carsSmashed = 0;
	gptr->player[lrt_c].carsSmashed = 0;
	//gptr->path.rec = false;
	gptr->path.used = 0;
	gptr->stats.totalBounce = 0;
	gptr->stats.firstEmpty = 0;
	gptr->stats.bounceUntilSmash[gptr->stats.firstEmpty] = 0;
	centerBallAndPlayerPositions(gptr);
	recordPoint(&(gptr->path), &(gptr->ball.position));
	FEXIT();
} // end-of-method generateNewCarLayout


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 25, 2019
 @mname   readCarLayoutFile
 @details
    reads the brick layout from a file and stores it as an
    array of characters \n
 --------------------------------------------------------------------------
 */
bool readCarLayoutFile(GameData* g, char* fileName) {

	FENTRY();

	char text[MAXBUFFER];

	FILE* fptr = NULL;
	if (fileName == NULL) {
		DEBUG("Null layout file name");
		FEXIT();
		return false;
	} else {
		fptr = fopen(fileName, "r");
		if (fptr == NULL) {
			DEBUG("Layout file not found");
			FEXIT();
			return false;
		} //end-of-if(fptr == NULL)
	}

	DEBUG("Layout file found. Loading data ....");
	char* buffer = text;
	int i = 0;

	while(fgets(buffer, MAXBUFFER, fptr)) {
		for (int j = 0; j < g->maxColumns; j++ ) {
			if(buffer[j] == 0) {
				FEXIT();
				return false;
			}
			switch(buffer[j]) {
			case 'x':
			case 'X':
			case 'g':
			case 'G':
			case 'e':
			case 'E':
				g->layout[i][j] = buffer[j];
				break;
			default:
				FEXIT();
				return false;
			}
		} //end-of-for
		i++;
		if(i>= g->maxRows) break;
	}

	if(i < g->maxRows) {
		FEXIT();
		return false;
	}

	fclose(fptr);
	FEXIT();
	return true;
} //end-readCarLayoutFile




/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 18, 2019
 @mname    createCarLayout
 @details
   transforms the text layout read from a file to the car setup
   if no text setup is present generate a random layout\n
 --------------------------------------------------------------------------
 */
void  createCarLayout(GameData*gptr) {
	FENTRY();

	gptr->remainingCars = 0;

	if(gptr->validLayout == true) {
		// use it once
		gptr->validLayout = false;
		for(int i = 0; i < gptr->maxRows; i++) {
			for (int j = 0; j < gptr->maxColumns; j++) {
				switch(gptr->layout[i][j]) {
				case 'x':
				case 'X':
					gptr->cars[i][j].onScreen = false;
					gptr->cars[i][j].indestructible = false;
					break;
				case 'e':
				case 'E':
					gptr->cars[i][j].onScreen = true;
					gptr->cars[i][j].indestructible = true;
					break;
				case 'g':
				case 'G':
					gptr->cars[i][j].onScreen = true;
					gptr->cars[i][j].indestructible = false;
					gptr->remainingCars++;
					break;
				default:
					break;
				}
			}
		}
	}
	else {

		int year = (gptr->year > 2080)?2080:gptr->year;
		uint rNumber = (year > 2020)? (2100- year )/10:10;
		uint maxCars = gptr->maxRows * gptr->maxColumns;

		if(year > 2020) {
			maxCars -= (2080-year)*maxCars/160;
		}


		// number of smashable cars in top and bottom row must be similar
		// othewise there is in an advantage to one player
		int topCount = 0;

		int totalCount = 0;

		for(int i = 0; i < gptr->maxRows; i++) {
			for (int j = 0; j < gptr->maxColumns; j++) {
				gptr->cars[i][j].onScreen = (rand() % rNumber)?true:false;
				if(gptr->cars[i][j].onScreen == true ) {
					// ecars are 'indestructible'
					// as times goes by more are ecars
					// but also less cars on road
					totalCount++;
					if(gptr->year >= 2000) {
						if(i == gptr->maxRows - 1) {
							if(topCount-- > 0) {
								gptr->remainingCars++;
								continue;
							}
						}
						gptr->cars[i][j].indestructible = (totalCount % rNumber)?false:true;
					}
					if(gptr->cars[i][j].indestructible  == false) {
						gptr->remainingCars++;
						if(i == 0) {
							topCount++;
						}
					}
				}
			} //end-of-for
		} //end-of-for

		for(int i = 1; i < gptr->maxRows-1; i++) {
			for (int j = 1; j < gptr->maxColumns-1; j++) {
				if(gptr->cars[i][j].indestructible  == false) {
					// do not allow gas cars to be surrounded by ecars
					// in that case the cars cannot be smashed
					// check if the gas car is surrounded and if it is, make it an ecar
					if((gptr->cars[i-1][j].indestructible == true)
							&& (gptr->cars[i][j-1].indestructible == true)
							&& (gptr->cars[i][j+1].indestructible == true)
							&& (gptr->cars[i+1][j].indestructible == true)
					){
						gptr->cars[i][j].indestructible = true;
						if(gptr->cars[i][j].onScreen == true)
							gptr->remainingCars--;
						else
							gptr->cars[i][j].onScreen = true;
					}
				}
			} //end-of-for
		} //end-of-for
	}
	gptr->gameNumber++;

	FEXIT();
} //end-createCarLayout


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   checkKeyboardAndMouse
 @details
 This function checks for keyboard input
 This function reacts to both keydown events and keyup events
 When a key is pushed down a boolean is set to keep the keep down as it is pressed\n
 --------------------------------------------------------------------------
 */
bool checkKeyboardAndMouse(GameData *gptr) {
	FENTRY();

	if (gptr->ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		if(gptr->ev.any.source == al_get_display_event_source(gptr->trajectoryDisplay.display)) {
			al_destroy_display(gptr->trajectoryDisplay.display);
			gptr->trajectoryDisplay.display = NULL;
			FEXIT();
			return true;
		} else {
			FEXIT();
			return false;
		}
	}

	if (gptr->ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		switch (gptr->ev.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			if (gptr->gameMode != fullbot_c)
				gptr->player[bus_c].keyPress[0] = true;
			else
				gptr->player[bus_c].keyPress[0] = false;
			gptr->player[bus_c].keyPress[1] = false;
			break;
		case ALLEGRO_KEY_RIGHT:
			if (gptr->gameMode != fullbot_c)
				gptr->player[bus_c].keyPress[1] = true;
			else
				gptr->player[bus_c].keyPress[1] = false;
			gptr->player[bus_c].keyPress[0] = false;
			break;
		case ALLEGRO_KEY_Q:
			if (gptr->gameMode == human_c)
				gptr->player[lrt_c].keyPress[0] = true;
			else
				gptr->player[lrt_c].keyPress[0] = false;
			gptr->player[lrt_c].keyPress[1] = false;
			break;
		case ALLEGRO_KEY_E:
			if (gptr->gameMode == human_c)
				gptr->player[lrt_c].keyPress[1] = true;
			else
				gptr->player[lrt_c].keyPress[1] = false;
			gptr->player[lrt_c].keyPress[0] = false;
			break;
		case ALLEGRO_KEY_P:
			if(gptr->gamePaused == false ) {
				gptr->gamePaused = true;
				al_stop_timer(gptr->timer);
			}
			else {
				gptr->gamePaused = false;
				al_flush_event_queue(gptr->eventqueue);
				al_start_timer(gptr->timer);
			}
			break;
		case ALLEGRO_KEY_H:
			gptr->helpOn = !gptr->helpOn;
			break;
		case ALLEGRO_KEY_C:
			gptr->cAlgoSelector = !gptr->cAlgoSelector;
			break;
		case ALLEGRO_KEY_G:
			gptr->path.separateDisplay = !gptr->path.separateDisplay;
			if(gptr->path.rec == true) {
				if(gptr->path.separateDisplay) {
					createTrajectoryDisplay(gptr);
				}
				else {
					al_destroy_display(gptr->trajectoryDisplay.display);
					gptr->trajectoryDisplay.display = NULL;
				}
			}
			break;
		case ALLEGRO_KEY_T:
			if(gptr->path.rec == false) {
				if(gptr->path.separateDisplay) {
					createTrajectoryDisplay(gptr);
				}
				gptr->path.rec = true;
			}
			else {
				al_destroy_display(gptr->trajectoryDisplay.display);
				gptr->trajectoryDisplay.display = NULL;
				gptr->path.rec = false;
				gptr->path.used = 0;
			}
			break;
		case ALLEGRO_KEY_F:
			increaseBallSpeed(gptr);
			break;
		case ALLEGRO_KEY_S:
			decreaseBallSpeed(gptr);
			break;
		case ALLEGRO_KEY_N:
			generateNewCarLayout(gptr);
			break;
		case ALLEGRO_KEY_ESCAPE:
			//exit game
			FEXIT();
			return false;
		case ALLEGRO_KEY_SPACE:
			startRound(gptr);
			break;
		}
	} else if (gptr->ev.type == ALLEGRO_EVENT_KEY_UP) {
		switch (gptr->ev.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			if (gptr->gameMode != fullbot_c) {
				gptr->player[bus_c].keyPress[0] = false;
				gptr->player[bus_c].ge.speed.x = 0;
			}
			break;
		case ALLEGRO_KEY_RIGHT:
			if (gptr->gameMode != fullbot_c) {
				gptr->player[bus_c].keyPress[1] = false;
				gptr->player[bus_c].ge.speed.x = 0;
			}
			break;
		case ALLEGRO_KEY_Q:
			if (gptr->gameMode == human_c) {
				gptr->player[lrt_c].keyPress[0] = false;
				gptr->player[lrt_c].ge.speed.x = 0;
			}
			break;
		case ALLEGRO_KEY_E:
			if (gptr->gameMode == human_c) {
				gptr->player[lrt_c].keyPress[1] = false;
				gptr->player[lrt_c].ge.speed.x = 0;
			}
			break;
		case ALLEGRO_KEY_W:
			writeCarLayoutToFile(gptr);
			break;
		case ALLEGRO_KEY_ESCAPE:
			//exit game
			FEXIT();
			return false;
		}
	}
	FEXIT();
	return true;
} // end-of-function checkKeyboardAndMouse

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 27, 2019
 @mname   userControl
 @details
 This function calculates the new positions of the paddles after the keys are pressed\n
 --------------------------------------------------------------------------
 */
void  userControl(GameData *gptr, PLAYERS player ) {
	FENTRY();

	if (gptr->player[player].keyPress[0] == true) {
		gptr->player[player].ge.position.x -= gptr->player[player].paddleSpeed;
		gptr->player[player].ge.speed.x = (-1)*gptr->player[player].paddleSpeed;
		if (gptr->player[player].ge.position.x < 0)
			gptr->player[player].ge.position.x = 0;
	}
	if (gptr->player[player].keyPress[1] == true) {
		gptr->player[player].ge.position.x += gptr->player[player].paddleSpeed;
		gptr->player[player].ge.speed.x = gptr->player[player].paddleSpeed;
		if (gptr->player[player].ge.position.x >= (gptr->display.width - gptr->player[player].ge.width))
			gptr->player[player].ge.position.x = (gptr->display.width - gptr->player[player].ge.width);
	}

	FEXIT();
} // end-of-function userControl


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 14, 2019
 @mname   botControl
 @details
 This function controls the the bus and lrt bots.\n
 --------------------------------------------------------------------------
 */
void  botControl(GameData *gptr, uint botNumber) {
	FENTRY();

	//update only when ball moves towards the player
	//We decide to move up based on the ball Y speed
	// if Y speed > 0 it means the ball is moving downward
	//If the Y speed < 0 it means the ball is moving upwards
	//bot 1 is at the top
	//bot 0 is at the bottom

	if ((botNumber == lrt_c) && gptr->ball.speed.y > 0) {
		FEXIT();
		return;
	}

	if ((botNumber == bus_c) && gptr->ball.speed.y < 0) {
		FEXIT();
		return;
	}

	float mult = 1;
	if(botNumber == lrt_c) {
		if (gptr->ball.position.y <= gptr->display.height / gptr->botControl[botNumber]->heightDivisor[0])
			mult = gptr->botControl[botNumber]->speedMultiplier[0];
		if (gptr->ball.position.y <= gptr->display.height / gptr->botControl[botNumber]->heightDivisor[1])
			mult = gptr->botControl[botNumber]->speedMultiplier[1];
		if (gptr->ball.position.y <= gptr->display.height / gptr->botControl[botNumber]->heightDivisor[2])
			mult = gptr->botControl[botNumber]->speedMultiplier[2];
		if (gptr->ball.position.y <= gptr->display.height / gptr->botControl[botNumber]->heightDivisor[3])
			mult = gptr->botControl[botNumber]->speedMultiplier[3];

		if(gptr->ball.position.y > gptr->display.height/gptr->botControl[botNumber]->heightDivisor[0])
			mult = 0;
	}
	else {
		if ((gptr->display.height - gptr->ball.position.y) <= gptr->display.height / gptr->botControl[botNumber]->heightDivisor[0])
			mult = gptr->botControl[botNumber]->speedMultiplier[0];
		if ((gptr->display.height - gptr->ball.position.y) <= gptr->display.height / gptr->botControl[botNumber]->heightDivisor[1])
			mult = gptr->botControl[botNumber]->speedMultiplier[1];
		if ((gptr->display.height - gptr->ball.position.y) <= gptr->display.height / gptr->botControl[botNumber]->heightDivisor[2])
			mult = gptr->botControl[botNumber]->speedMultiplier[2];
		if ((gptr->display.height - gptr->ball.position.y) <= gptr->display.height / gptr->botControl[botNumber]->heightDivisor[3])
			mult = gptr->botControl[botNumber]->speedMultiplier[3];

		if(gptr->ball.position.y< gptr->display.height/gptr->botControl[botNumber]->heightDivisor[0])
			mult = 0;
	}


	float f = gptr->player[botNumber].paddleSpeed * mult;
	if (gptr->ball.speed.x > 0) {
		if (gptr->ball.position.x > (gptr->player[botNumber].ge.position.x + gptr->player[botNumber].ge.width)) {
			//gptr->ball.prevposition.x = gptr->ball.position.x;
			gptr->player[botNumber].ge.position.x += (int) f;
			gptr->player[botNumber].ge.speed.x = (int) f;
		} else if (gptr->ball.position.x < gptr->player[botNumber].ge.position.x) {
			gptr->ball.prevposition.x = gptr->ball.position.x;
			gptr->player[botNumber].ge.position.x -= (int) f;
			gptr->player[botNumber].ge.speed.x = (int) (-1)*f;
		}
	} else {
		if (gptr->ball.position.x < gptr->player[botNumber].ge.position.x) {
			//gptr->ball.prevposition.x = gptr->ball.position.x;
			gptr->player[botNumber].ge.position.x -= (int) f;
			gptr->player[botNumber].ge.speed.x = (int) (-1)*f;
		} else if (gptr->ball.position.x  > (gptr->player[botNumber].ge.position.x + gptr->player[botNumber].ge.width)) {
			//gptr->ball.prevposition.x = gptr->ball.position.x;
			gptr->player[botNumber].ge.position.x += (int) f;
			gptr->player[botNumber].ge.speed.x = (int) f;
		}
	}

	//end of display to the left
	if (gptr->player[botNumber].ge.position.x < 0) {
		gptr->player[botNumber].ge.position.x = 0;
		gptr->player[botNumber].ge.speed.x = 0;
	}
	//end of display to the right
	if (gptr->player[botNumber].ge.position.x >= (gptr->display.width - gptr->player[botNumber].ge.width)) {
		gptr->player[botNumber].ge.position.x = (gptr->display.width - gptr->player[botNumber].ge.width);
		gptr->player[botNumber].ge.speed.x = 0;
	}
	FEXIT();
} // end-of-function botControl


// === Game Loop ====

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 25, 2019
 @mname   gameLoop
 @details
 This is the function called from the main function
 1. Displays the initial screen
 2. Calls the game loop
 Two dimensional games process events and screen updates in a continuous loop
 usually this loop is called a game loop
 The loop processes events from p->eventqueue.
 Events come from the two game timers (one is for screen refresh,
 the other is for Bot AI) as well as keyboard and mouse events.
 The loop waits for an event to be fired and then processes the event
 If the event is of the type refresh display, all objects are written to a display buffer
 Then that buffer is shown on the screen. \n
 --------------------------------------------------------------------------
 */
void gameLoop(GameData *p, bool startTimer) {
	FENTRY();

	long skipCounter = 0;
	bool quit = false;


	p->maxspeed.x = maxballspeed_c;
	p->maxspeed.y = maxballspeed_c;

	generateNewCarLayout(p);

	if(startTimer) {
		al_start_timer(p->timer);
	}

	playSound(p->startsample);

	//We are waiting for an event from on one of the sources that are linked to the event queue
	//The frame-timer, keyboard, mouse, and bot timer if in arcade mode
	//This function blocks until an event is received
	//Therefore if the timers would not be started,
	//this function would return only on a keyboard or mouse event
	while (!quit) {

		al_wait_for_event(p->eventqueue, &(p->ev));
		//check if escape key has been pressed
		quit = !checkKeyboardAndMouse(p);

		if(p->gameStart == false) {
			//al_clear_to_color writes the graphics buffer in the provided color
			//this is used to set the background color for the display
			al_clear_to_color(*(p->backgroundColor));
			drawBeginGameScreen(p);
			flipAllDisplays(p);
		}
		else {
			//we redraw the screen only on TIMER events
			//however if we dont start the timers
			//we will draw the screen on keyboard events
			//this will allow to debug in manual mode
			//as we can see the game playing frame by frame
			if ((p->ev.type == ALLEGRO_EVENT_TIMER
					&& p->ev.timer.source == p->timer) || (startTimer == false)){
				//for round wins we want to display overlay messages
				if (p->roundWin == true) {
					//At the end of each round we want to keep the last frame of the play that shows where the ball exitied the screen
					//for a little longer, so the user can see who won the round
					//We do this by counting frame timer events
					//and redrawing the last frame when the 'win' occurred
					if (skipCounter++ >= (int) p->fps) {
						if ((p->roundNumber > p->maxRounds) || (p->remainingCars == 0)){
							p->gameWin = true;
							centerBallAndPlayerPositions(p);
							//al_clear_to_color writes the graphics buffer in the provided color
							//this is used to set the background color for the display
							al_clear_to_color(*(p->backgroundColor));
							drawGameWinText(p);
							drawMainGameScreen(p);
						}
						else {
							centerBallAndPlayerPositions(p);
							//al_clear_to_color writes the graphics buffer in the provided color
							//this is used to set the background color for the display
							al_clear_to_color(*(p->backgroundColor));
							drawRoundWinText(p);
							drawMainGameScreen(p);
						}
						writeTrajectoryToWindow(p);
					}else {
						al_flush_event_queue(p->eventqueue);
						//al_clear_to_color writes the graphics buffer in the provided color
						//this is used to set the background color for the display
						al_clear_to_color(*(p->backgroundColor));
						drawMainGameScreen(p);
						writeTrajectoryToWindow(p);
					}
				}
				else {
					// this is the regular play mode
					//1. we update the player positions (either due to key inputs or bot decisions)
					//2. we update ball position
					//3. draw all objects
					//4. draw the trajectory window
					skipCounter = 0;
					//we need to run the bot logic
					if (p->gameMode == fullbot_c) {
						// We have to run the bot control logic
						botControl(p, lrt_c);
						botControl(p, bus_c);
					}else if(p->gameMode == arcade_c) {
						botControl(p, lrt_c);
						userControl(p, bus_c);
					} else {
						userControl(p, bus_c);
						userControl(p, lrt_c);
					}
					//If this is a screen update timer event then we have to redraw the screen
					//we have to update the ball position and then draw all objects (players and ball)
					//Calculates next position of the paddles based on the key inputs read above

					p->roundWin = updateBallPosition(p);
					if(p->roundWin) p->roundNumber++;
					//al_clear_to_color writes the graphics buffer in the provided color
					//this is used to set the background color for the display
					al_clear_to_color(*(p->backgroundColor));
					drawMainGameScreen(p);
					writeTrajectoryToWindow(p);
				}
				//in case of timer event, the screen has new information
				//this sends the information to screen (allegro has double buffering)
				flipAllDisplays(p);
			} //end-of[p->ev.type == ALLEGRO_EVENT_TIMER]
		} //end-of [p->gameStart == false]
	}//end-of-while(!quit)

	//before returning to main, we need to clean up the allegro engine
	allegroCleanup(p);
	FEXIT();
} // end-of-function runGame
