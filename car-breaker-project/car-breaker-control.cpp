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
 @date    Dec 16, 2019
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
 @date    Dec 8, 2019
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
   @date    Jan 1, 2020
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
   @date    Dec 6th, 2019
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
 @date    Dec 7, 2019
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
 @date    Dec 6, 2019
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
 @date    Dec 3, 2019
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
		DEBUG("*** KEY DOWN");
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
		DEBUG("*** KEY UP");
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
	}else if (gptr->ev.type == ALLEGRO_EVENT_KEY_CHAR) {
		DEBUG(" *** KEY CHAR EVENT");
	}
	FEXIT();
	return true;
} // end-of-function checkKeyboardAndMouse

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 22, 2019
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
 @date    Dec 18, 2019
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
 @date    Dec 5, 2019
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
							al_flush_event_queue(p->eventqueue);
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
