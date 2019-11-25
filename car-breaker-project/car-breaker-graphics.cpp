//============================================================================
// Name        : car-breaker-graphics.cpp
// Author      : mlambiri
// Version     :
// Copyright   : (c) 2019 Michael C Lambiri
// Description : car breaker game
//============================================================================

#include "car-breaker-graphics.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "game-debug.h"


static const int minballspeed_c = 3;
static const int maxballspeed_c = minballspeed_c + 2;
static const uint maxPaddleSize_c = 7;
static const uint maxdiff_c = 4;
static const uint botArrays_c = 5;

static const char P1FNAME[] = "player1.png";
static const char P2FNAME[] = "player2.png";
static const char BALLFNAME[] = "ball.png";
static const char GASCARFNAME[] = "gascar.png";
static const char ECARFNAME[] = "ecar.png";
static const char P1SOUND[] = "p1sound.ogg";
static const char P2SOUND[] = "p2sound.ogg";
static const char FONTNAME[] = "pirulen.ttf";


//========VARIABLE DECLARATIONS=====
//declaring the main data variable of the game
//usually passed to functions using a pointer
static GameData carBreaker = {
				{INITPLAYER, INITPLAYER},
				INITGBB,
				INITDISPLAY, human_c, maxballspeed_c, NULL, FONTSIZE, MAXROUNDS, { 0 }, { 0 },
				FRAMERATE

};


/*
 * @author   mlambiri
 * @date     Jun 2, 2019
 *  HAL needs two arrays of values to decide when and how fast to move
 *  This structure stores them in one group both arrays.
 *  HAL adapts his own skill to match the level of skill of the human player
 *  This is done by keeping several sets of variables for HAL's AI.
 *  cond is a divisor of the field length
 *  Because HAL plays the side x = 0 The greater the divisor the closer the ball is to him
 *  val is a multiplier that will make HAL move faster or slower between two frames
 *  If the value of n == 0 HAL will not move
 *  Therefore placing all zeroes in HAL will render him immobile at the center
 *
 */
typedef struct BotControlArray {
	//first array represents where in the field HAL will start to move
	int cond[botArrays_c];
	//This array is a multiplier to determine how much HAL should move
	//setting an entry to zero will prevent HAL from moving
	float val[botArrays_c];
	int paddlespeed;
} BotControlArray;

//It is an array that stores several sets of conditions and values
//The values make the Bot more responsive as the index in the array increases
BotControlArray lrtBotArray[pro_c + 1] = {
		{ { 2, 2, 3, 4, 8 }, { 0.25, 0.25, 0.25, 0.25,0.25 }, (int) PLAYERSPEED },
		{ { 2, 2, 3, 4, 8 }, { 0.5, 0.5, 0.5, 0.5, 0.5 }, (int) PLAYERSPEED },
		{ { 2, 2, 3, 4, 8 }, { 1, 1, 1, 2, 2 }, (int) PLAYERSPEED },
		{ { 2, 2, 3, 4, 8 }, { 1, 1, 1.5, 2, 3 }, 40 } };

//It is an array that stores several sets of conditions and values
//The values make the Bot more responsive as the index in the array increases
BotControlArray busBotArray[pro_c + 1] = {
		{ { 2, 2, 3, 4, 8 }, { 0.25, 0.25, 0.25, 0.25,0.25 }, (int) PLAYERSPEED },
		{ { 2, 2, 3, 4, 8 }, { 0.5, 0.5, 0.5, 0.5, 0.5 }, (int) PLAYERSPEED },
		{ { 2, 2, 3, 4, 8 }, { 1, 1, 1, 2, 2 }, (int) PLAYERSPEED },
		{ { 2, 2, 3, 4, 8 }, { 1, 1, 1.5, 2, 3 }, 40 } };

static int lrtBotPlayingLevel = pro_c;
static BotControlArray *lrtBotPtr = &(lrtBotArray[lrtBotPlayingLevel]);

static int busBotPlayingLevel = pro_c;
static BotControlArray *busBotPtr = &(busBotArray[busBotPlayingLevel]);

//======= EXTERNAL FUNCTION DECLARATION=====//
bool recordResult(char *p);

//======= FUNCTION DECLARATIONS =====
static bool checkCollisionLeftRight(GameData *gamePtr);
static bool checkCollisionTopAndBottom(GameData *gamePtr);
static bool checkBallCollisionWithObjects(GameData *gamePtr);
static bool displayScore(GameData *gamePtr);
static bool drawTextAndWaitBegin(GameData *gamePtr);
static bool drawTextAndWaitRoundWin(GameData *gamePtr);
static bool gameMainLoop(GameData *gamePtr);
static bool loadAudio(GamePlayer *gamePtr);
static bool loadAudioWinner(GameData *gamePtr);
static bool loadBitmap(GameBasicBlock *g, char* fname);
static bool setBitmap(GameBasicBlock *g, ALLEGRO_BITMAP*);
static bool loadFont(GameData *gamePtr, int size);
static bool loadPlayerBitmap(GamePlayer *p, char* fname);
static bool pauseGame(GameData *gamePtr);
static bool pressAnyKeyToBeginGame(GameData *gamePtr);
static bool printRoundWinner(GameData *gamePtr);
static bool processKeyPressEvent(GameData *gamePtr);
static bool updateBallPosition(GameData *gamePtr);
static int drawTextOnScreen(GameData *gamePtr, char *text, int x, int y, int size);
static int signOfNumber(int value);
static void ballBounceOnPlayer(GameBasicBlock *ball, GamePlayer *playerPtr, int);
static void lrtBotControl(GameData *gamePtr, uint botNumber);
static void drawBitmap(GameBasicBlock *g);
static void drawBitmapSection(GameBasicBlock *g);
static void drawObjects(GameData *gamePtr);
static void exitGame(GameData *gamePtr);
static void initBrickLayout(GameData*gamePtr);
static void movePlayers(GameData *gamePtr);
static void playSound(ALLEGRO_SAMPLE *s);
static void setBackgroundColor(ALLEGRO_COLOR color);
static void setInitialObjectPositions(GameData *gamePtr);
static void startTimers(GameData *gamePtr);
static void stopTimers(GameData *gamePtr);
static void setBrickInfo(GameData* p);


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 18, 2019
 @mname   initBrickLAyout
 @details
 \n
 --------------------------------------------------------------------------
 */
static void initBrickLayout(GameData*gamePtr) {

	gamePtr->remainingCars = 0;
	int year = (gamePtr->year > 2080)?2080:gamePtr->year;
	uint rNumber = (year > 2020)? (2100- year )/10:10;
	uint maxCars = MAXBRICKCOLUMNS*MAXBRICKROWS;

	if(year > 2020) {
		maxCars -= (2080-year)*MAXBRICKCOLUMNS*MAXBRICKROWS/160;
	}


	// number of smashable cars in top and bottom row must be similar
	// othewise there is in an advantage to one player
	int topCount = 0;

	int totalCount = 0;

	for(int i = 0; i < MAXBRICKROWS; i++) {
		for (int j = 0; j < MAXBRICKCOLUMNS; j++) {
			gamePtr->bricks[i][j].onScreen = (rand() % rNumber)?true:false;
			if(gamePtr->bricks[i][j].onScreen == true ) {
				// ecars are 'indestructible'
				// as times goes by more are ecars
				// but also less cars on road
				totalCount++;
				if(gamePtr->year >= 2000) {
					if(i == MAXBRICKROWS - 1) {
						if(topCount-- > 0) {
							gamePtr->remainingCars++;
							continue;
						}
					}
					gamePtr->bricks[i][j].indestructible = (totalCount % rNumber)?false:true;
				}
				if(gamePtr->bricks[i][j].indestructible  == false) {
					gamePtr->remainingCars++;
					if(i == 0) {
						topCount++;
					}
				}
			}
		} //end-of-for
	} //end-of-for
	gamePtr->gameNumber++;
}

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 24, 2019
 @mname   setBrickInfo
 @details
 \n
 --------------------------------------------------------------------------
 */
static void setBrickInfo(GameData* p) {
	for (int i = 0; i < MAXBRICKROWS; i++) {
		for (int j = 0; j < MAXBRICKCOLUMNS; j++) {
			if(p->bricks[i][j].indestructible == false) {
				if (setBitmap(&(p->bricks[i][j]), p->gasBitmap) == false) {
					FEXIT();
					return;
				}
			}
			else {
				if (setBitmap(&(p->bricks[i][j]), p->ecarBitmap) == false) {
					FEXIT();
					return;
				}
			}
			p->bricks[i][j].xspeed = 0;
			p->bricks[i][j].yspeed = 0;
		} //end-of-for
	} //end-of-for
}

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 19, 2019
 @mname   setPointsPerCarSmashed
 @details
 \n
 --------------------------------------------------------------------------
 */
static void setPointsPerSmash(GameData*gamePtr) {

	if(gamePtr->remainingCars<= level6_c){
		gamePtr->scorePointsPerSmash = 10;
	}else if (gamePtr->remainingCars< level5_c) {
		gamePtr->scorePointsPerSmash = 3;
	}else if (gamePtr->remainingCars< level4_c) {
		gamePtr->scorePointsPerSmash = 2;
	}else {
		gamePtr->scorePointsPerSmash = 1;
	}
}

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 19, 2019
 @mname   isPointInObject
 @details
 \n
 --------------------------------------------------------------------------
 */
static bool isPointInObject(GameBasicBlock* b, int x, int y){

	if((x>=b->xposition)
		&& (x <= (b->xposition+b->width))
		&&(y>=b->yposition)
		&& (y<=(b->yposition+b->height))) {
		return true;
	}
	else {
		return false;
	}
}

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 19, 2019
 @mname   areObjectsColliding
 @details
  side returns the side of the object hit by the ball\n
 --------------------------------------------------------------------------
 */
static bool areObjectsColliding(GameBasicBlock* ball, GameBasicBlock* obj, COLLISIONSIDE& side){

	bool result = isPointInObject(obj,ball->xposition,ball->yposition);
	result = result || isPointInObject(obj, ball->xposition+ball->width, ball->yposition);
	result = result || isPointInObject(obj,ball->xposition, ball->yposition+ball->height);
	result = result || isPointInObject(obj, ball->xposition+ball->width, ball->yposition+ball->height);

	if(result == false) return false;

	float ballXCenter = ball->xposition + (float) ball->width/2;
	float ballYCenter = ball->yposition + (float) ball->height/2;

	float objXCenter = obj->xposition + (float) obj->width/2;
	float objYCenter = obj->yposition + (float) obj->height/2;


	float o1a = atan((float)obj->width/(float)obj->height);

	if((ball-> yspeed > 0) && (ballYCenter < objYCenter)) {
		side = top_c;
	}
	if((ball-> yspeed < 0) && (ballYCenter > objYCenter)) {
		side = bottom_c;
	}

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
}

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 19, 2019
 @mname   ballBrickCollision
 @details
 \n
 --------------------------------------------------------------------------
 */
static bool isBallBrickCollision(GameData* gamePtr, int i, int j) {

	FENTRY();
	TRACE();
	if (gamePtr->bricks[i][j].onScreen == false) {
		FEXIT();
		return false;
	}

	COLLISIONSIDE side;

	if(areObjectsColliding(&(gamePtr->ball), &(gamePtr->bricks[i][j]), side)) {

		switch(side){
		case top_c:
			gamePtr->ball.yspeed *= -1;
			gamePtr->ball.yposition = gamePtr->bricks[i][j].yposition - gamePtr->ball.height;
			break;
		case bottom_c:
			gamePtr->ball.yspeed *= -1;
			gamePtr->ball.yposition = gamePtr->bricks[i][j].yposition + gamePtr->bricks[i][j].height;
			break;
		case left_c:
			gamePtr->ball.xposition = gamePtr->bricks[i][j].xposition - gamePtr->ball.width;
			gamePtr->ball.xspeed *= -1;
			break;
		case right_c:
			gamePtr->ball.xspeed *= -1;
			gamePtr->ball.xposition = gamePtr->bricks[i][j].xposition + gamePtr->bricks[i][j].width;
			break;
		default:
			break;
		}


		if(gamePtr->bricks[i][j].indestructible == false) {
			gamePtr->bricks[i][j].onScreen = false;
			gamePtr->remainingCars--;
			setPointsPerSmash(gamePtr);
		}
		if(gamePtr->remainingCars < level5_c ) {
			gamePtr->bcolor = &(gamePtr->bcolorarray[green_c]);
		}else if(gamePtr->remainingCars < level4_c ) {
			gamePtr->bcolor = &(gamePtr->bcolorarray[blue_c]);
		}

		if(gamePtr->bricks[i][j].indestructible == false && gamePtr->turn) {
			gamePtr->turn->carsSmashed+= gamePtr->scorePointsPerSmash;
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
 @date    Nov 22, 2019
 @mname   SetBackgroundColor
 @details
 \n
 --------------------------------------------------------------------------
 */
static void setBackgroundColor(ALLEGRO_COLOR color) {
	FENTRY();
	TRACE();
	al_clear_to_color(color);
	FEXIT();
} // end-of-function SetBackgroundColor

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 28, 2019
 @mname   LoadPlayerBitmap
 @details
 \n
 --------------------------------------------------------------------------
 */
static bool loadPlayerBitmap(GamePlayer *p, char* fname) {
	FENTRY();
	TRACE();
	if ((p->ge.bmap = al_load_bitmap(fname)) == NULL) {
		printf("cannot load %s\n ", fname);
		FEXIT();
		return false;
	}
	p->ge.width = (al_get_bitmap_width(p->ge.bmap)* (maxPaddleSize_c + 1 - p->paddleSize))
                        														/ maxPaddleSize_c;
	p->ge.height = al_get_bitmap_height(p->ge.bmap);
	FEXIT();
	return true;
} // end-of-function LoadPlayerBitmap

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   LoadBitmap
 @details
 return true if ok false otherwise\n
 --------------------------------------------------------------------------
 */
static bool loadBitmap(GameBasicBlock *g, char* fname) {
	FENTRY();
	TRACE();
	if ((g->bmap = al_load_bitmap(fname)) == NULL) {
		printf("cannot load %s\n ", fname);
		FEXIT();
		return false;
	}
	g->width = al_get_bitmap_width(g->bmap);
	g->height = al_get_bitmap_height(g->bmap);

	FEXIT();
	return true;
} // end-of-function LoadBitmap


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   LoadBitmap
 @details
 return true if ok false otherwise\n
 --------------------------------------------------------------------------
 */
static bool setBitmap(GameBasicBlock *g, ALLEGRO_BITMAP* b) {
	FENTRY();
	TRACE();
	g->bmap = b;
	g->width = al_get_bitmap_width(g->bmap);
	g->height = al_get_bitmap_height(g->bmap);
	FEXIT();
	return true;
} // end-of-function setBitmap


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 27, 2019
 @mname   LoadAudio
 @details
 \n
 --------------------------------------------------------------------------
 */
static bool loadAudio(GamePlayer *gamePtr) {
	FENTRY();
	TRACE();
	gamePtr->sample = al_load_sample(gamePtr->audioFileName);
	if (gamePtr->sample == NULL) {
		printf("Audio clip sample %s not loaded!\n", gamePtr->audioFileName);
		FEXIT();
		return false;
	}FEXIT();
	return true;
} // end-of-function LoadAudio

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 28, 2019
 @mname   LoadWinAudio
 @details
 \n
 --------------------------------------------------------------------------
 */
static bool loadAudioWinner(GameData *gamePtr) {
	FENTRY();
	TRACE();
	gamePtr->winsample = al_load_sample(gamePtr->winSoundFile);
	if (gamePtr->winsample == NULL) {
		printf("Audio clip sample %s not loaded!\n", gamePtr->winSoundFile);
		FEXIT();
		return false;
	}FEXIT();
	return true;
} // end-of-function LoadWinAudio

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 27, 2019
 @mname   LoadFont
 @details
 \n
 --------------------------------------------------------------------------
 */
static bool loadFont(GameData *gamePtr, int size) {

	FENTRY();
	TRACE();
	int fontSize = gamePtr->fontsize;
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
	gamePtr->font[size] = al_load_ttf_font(gamePtr->fontFileName, fontSize, 0);

	//error message if the font file is NULL
	if (gamePtr->font[size] == NULL) {
		printf("Could not load %s.\n", gamePtr->fontFileName);
		FEXIT();
		return false;
	}FEXIT();
	return true;
} // end-of-function LoadFont

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   InitialPosition
 @details
 This function sets the players in the middle of the Y axis and provides
 the ball to one of the players
 If it is the first round, the player who has the ball is chosen at random.
 After a round win the round winner gets the serve.\n
 --------------------------------------------------------------------------
 */
static void setInitialObjectPositions(GameData *gamePtr) {

	FENTRY();
	TRACE();
	gamePtr->ball.yspeed = minballspeed_c
			+ rand() % (gamePtr->maxballspeed - minballspeed_c);
	if (gamePtr->roundWinner) {
		gamePtr->turn = gamePtr->roundWinner;
		if (gamePtr->roundWinner == &(gamePtr->player[0])) {
			gamePtr->ball.yspeed *= -1;
		} //end-of-if(p->roundWinner == &(p->player[0]))
	} else {
		//if there is no roundwinnner, it is the first serve of the game
		//we need to pick at random a starting player
		switch (rand() % 2) {
		case 0:
			// player 1
			gamePtr->ball.yspeed *= -1;
			gamePtr->turn = &gamePtr->player[0];
			break;
		default:
			//player 2
			gamePtr->turn = &gamePtr->player[1];
			break;
		} //end-switch(rand() %2)
	} //end-of-if(p->roundWinner)

	float ratio_c = (float) gamePtr->display.height / (2 * gamePtr->display.width);
	float maxxspeed = ratio_c * abs(gamePtr->ball.yspeed);

	gamePtr->ball.xspeed = rand() % (int) maxxspeed;
	if (gamePtr->ball.xspeed == 0)
		gamePtr->ball.xspeed = 3;
	switch (rand() % 2) {
	case 0:
		//serve up
		gamePtr->ball.xspeed *= -1;
		break;
	default:
		break;
	} //end-switch(rand() %2)

	gamePtr->player[0].ge.xposition = gamePtr->display.width / 2 - gamePtr->player[0].ge.width / 2;
	gamePtr->player[0].ge.yposition = gamePtr->display.height - gamePtr->player[0].ge.height;
	gamePtr->player[0].ge.xspeed = 0;
	gamePtr->player[1].ge.xposition = gamePtr->display.width / 2 - gamePtr->player[1].ge.width / 2;
	gamePtr->player[1].ge.yposition = 0;
	gamePtr->player[1].ge.xspeed = 0;

	if (gamePtr->ball.yspeed > 0)
		gamePtr->ball.yposition = gamePtr->player[1].ge.yposition + gamePtr->player[1].ge.height;
	else
		gamePtr->ball.yposition = gamePtr->player[0].ge.yposition - gamePtr->ball.height;
	gamePtr->ball.xposition = gamePtr->display.width / 2 - (gamePtr->ball.width / 2);
	if (gamePtr->ball.yspeed > 0) {
		gamePtr->startsample = gamePtr->player[1].sample;
	} else {
		gamePtr->startsample = gamePtr->player[0].sample;
	}

	int ypos = (gamePtr->display.height - gamePtr->bricks[0][0].height*MAXBRICKROWS)/2;
	for (int i = 0; i < MAXBRICKROWS; i++) {
		int xpos = (gamePtr->display.width - gamePtr->bricks[0][0].width*MAXBRICKCOLUMNS)/2;
		if(xpos < 0) {
			xpos = 0;
		}
		for (int j = 0; j < MAXBRICKCOLUMNS; j++) {
			gamePtr->bricks[i][j].yposition = ypos;
			gamePtr->bricks[i][j].xposition = xpos;
			xpos += gamePtr->bricks[i][j].width;
		} //end-of-for
		ypos += gamePtr->bricks[i][0].height;
	} //end-of-for

	FEXIT();

} // end-of-function InitialPosition

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Jun 3, 2019
 @mname   PauseGame
 @details
 Wait for P or ESC to be pressed again\n
 --------------------------------------------------------------------------
 */
static bool pauseGame(GameData *gamePtr) {

	//To pause the game we need to stop the timers
	FENTRY();
	TRACE();
	stopTimers(gamePtr);
	while (true) {
		TRACE();
		//wait for an event
		al_wait_for_event(gamePtr->eventqueue, &(gamePtr->ev));
		//check if the event is a key press
		//can be something else as the event queue
		//has other sources
		if (gamePtr->ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (gamePtr->ev.keyboard.keycode) {
			case ALLEGRO_KEY_ESCAPE:
				//exit game
				FEXIT();
				return false;
			case ALLEGRO_KEY_P:
				//P was pressed again
				//we want no events in the queue
				//and we want to start the timers
				al_flush_event_queue(gamePtr->eventqueue);
				startTimers(gamePtr);
				FEXIT();
				return true;
			}
		}
		//close the display with the mouse
		if (gamePtr->ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			FEXIT();
			return false;
		}
	}FEXIT();
	return true;

} // end-of-function PauseGame

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   ProcessKeyPress
 @details
 This function checks for keyboard input
 This function reacts to both keydown events and keyup events
 When a key is pushed down a boolean is set to keep the keep down as it is pressed\n
 --------------------------------------------------------------------------
 */
static bool processKeyPressEvent(GameData *gamePtr) {

	FENTRY();
	TRACE();
	if (gamePtr->ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		switch (gamePtr->ev.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			gamePtr->player[0].keyPress[0] = true;
			gamePtr->player[0].keyPress[1] = false;
			break;
		case ALLEGRO_KEY_RIGHT:
			gamePtr->player[0].keyPress[0] = false;
			gamePtr->player[0].keyPress[1] = true;
			break;
		case ALLEGRO_KEY_W:
			if (gamePtr->gameMode == human_c)
				gamePtr->player[1].keyPress[0] = true;
			else
				gamePtr->player[1].keyPress[0] = false;
			gamePtr->player[1].keyPress[1] = false;
			break;
		case ALLEGRO_KEY_S:
			if (gamePtr->gameMode == human_c)
				gamePtr->player[1].keyPress[1] = true;
			else
				gamePtr->player[1].keyPress[1] = false;
			gamePtr->player[0].keyPress[0] = false;
			break;
		case ALLEGRO_KEY_P:
			if (pauseGame(gamePtr) == false) {
				FEXIT();
				return false;
			}
			break;
		case ALLEGRO_KEY_ESCAPE:
			//exit game
			FEXIT();
			return false;
		}
	} else if (gamePtr->ev.type == ALLEGRO_EVENT_KEY_UP) {
		switch (gamePtr->ev.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			gamePtr->player[0].keyPress[0] = false;
			break;
		case ALLEGRO_KEY_RIGHT:
			gamePtr->player[0].keyPress[1] = false;
			break;
		case ALLEGRO_KEY_W:
			if (gamePtr->gameMode == human_c)
				gamePtr->player[1].keyPress[0] = false;
			break;
		case ALLEGRO_KEY_S:
			if (gamePtr->gameMode == human_c)
				gamePtr->player[1].keyPress[1] = false;
			break;
		case ALLEGRO_KEY_ESCAPE:
			//exit game
			FEXIT();
			return false;
		}
	} else if (gamePtr->ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		FEXIT();
		return false;
	}FEXIT();
	return true;
} // end-of-function ProcessKeyPress

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Jun 3, 2019
 @mname   PressAnyKeyToBegin
 @details
 \n
 --------------------------------------------------------------------------
 */
static bool pressAnyKeyToBeginGame(GameData *gamePtr) {

	FENTRY();
	TRACE();
	al_flush_event_queue(gamePtr->eventqueue);

	while (true) {
		TRACE();
		//wait for an event
		al_wait_for_event(gamePtr->eventqueue, &(gamePtr->ev));
		//check if the event is a key press
		//can be something else as the event queue
		//has other sources
		if (gamePtr->ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			FEXIT();
			//exits either way
			switch (gamePtr->ev.keyboard.keycode) {
			case ALLEGRO_KEY_ESCAPE:
				//exit game
				return false;
			default:
				return true;
			}
		}
		if (gamePtr->ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			FEXIT();
			return false;
		}
	}FEXIT();
	return true;
} // end-of-function PressAnyKeyToBegin

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 27, 2019
 @mname   MovePaddles
 @details
 This function calculates the new positions of the paddles after the keys are pressed\n
 --------------------------------------------------------------------------
 */
static void movePlayers(GameData *gamePtr) {

	FENTRY();
	TRACE();
	if (gamePtr->player[0].keyPress[0] == true) {
		gamePtr->player[0].ge.xposition -= gamePtr->player[0].paddleSpeed;
		if (gamePtr->player[0].ge.xposition < 0)
			gamePtr->player[0].ge.xposition = 0;
	}
	if (gamePtr->player[0].keyPress[1] == true) {
		gamePtr->player[0].ge.xposition += gamePtr->player[0].paddleSpeed;
		if (gamePtr->player[0].ge.xposition >= (gamePtr->display.width - gamePtr->player[0].ge.width))
			gamePtr->player[0].ge.xposition = (gamePtr->display.width - gamePtr->player[0].ge.width);
	} //end-of-if(p->player[0].keyPress[1] ==true)

	if (gamePtr->player[1].keyPress[0] == true) {
		gamePtr->player[1].ge.xposition -= gamePtr->player[1].paddleSpeed;
		if (gamePtr->player[1].ge.xposition < 0)
			gamePtr->player[1].ge.xposition = 0;
	} //end-of-if(p->player[1].keyPress[0] == true)

	if (gamePtr->player[1].keyPress[1] == true) {
		gamePtr->player[1].ge.xposition += gamePtr->player[1].paddleSpeed;
		if (gamePtr->player[1].ge.xposition >= (gamePtr->display.width - gamePtr->player[1].ge.width))
			gamePtr->player[1].ge.xposition = (gamePtr->display.width - gamePtr->player[1].ge.width);
	} //end-of-if(p->player[1].keyPress[1] == true)

	FEXIT();
} // end-of-function MovePaddles

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   DrawText
 @details
 Displays text on screen using allegro
 Declared an enumeration of text sizes
 Different text sizes are used for different messages \n
 --------------------------------------------------------------------------
 */
static int drawTextOnScreen(GameData *gamePtr, char *text, int x, int y, int size) {
	FENTRY();
	TRACE();
	al_draw_text(gamePtr->font[size], gamePtr->fcolor, x, y, ALLEGRO_ALIGN_CENTRE, text);
	int fsize = gamePtr->fontsize;
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
} // end-of-function DrawText

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   DisplayTextAndWaitBegin
 @details
 Returns false if escape key is pressed
 This function displays the first screen that the user views in the game\n
 --------------------------------------------------------------------------
 */
static bool drawTextAndWaitBegin(GameData *gamePtr) {

	FENTRY();
	TRACE();
	int next = drawTextOnScreen(gamePtr, (char*) "Welcome to Car Smasher", gamePtr->display.width / 2,
			gamePtr->display.height / 4, largeFont_c);
	al_flush_event_queue(gamePtr->eventqueue);
	drawTextOnScreen(gamePtr, (char*) "(c) mlambiri 2019", gamePtr->display.width / 2, next,
			smallFont_c);

	if(gamePtr->gameMode == fullbot_c) {
		next = drawTextOnScreen(gamePtr, (char*) "Full Auto Mode (Bot v Bot)", gamePtr->display.width / 2,
				gamePtr->display.height / 2, regularFont_c);
	}
	else if (gamePtr->gameMode == arcade_c) {
		next = drawTextOnScreen(gamePtr, (char*) "Arcade Mode (Bot Controls LRT)",
				gamePtr->display.width / 2, gamePtr->display.height / 2, regularFont_c);
	} else {
		next = drawTextOnScreen(gamePtr, (char*) "Two Player Mode", gamePtr->display.width / 2,
				gamePtr->display.height / 2, regularFont_c);
	}
	char buffer[100];
	sprintf(buffer, "First to %d Wins!", gamePtr->maxscore);
	next = drawTextOnScreen(gamePtr, buffer, gamePtr->display.width / 2, next, regularFont_c);
	next = drawTextOnScreen(gamePtr, (char*) "Press a key to begin", gamePtr->display.width / 2,
			next, regularFont_c);

	al_flip_display();

	if (pressAnyKeyToBeginGame(gamePtr) == false) {
		FEXIT();
		return false;
	}
	FEXIT();
	return true;
} // end-of-function DisplayTextAndWaitBegin

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   DisplayTextAndWaitBegin
 @details
 Returns false if escape key is pressed
 This function displays a screen when a round or game is won
 The text for the two conditions will be different
 We declare the temporary variable next to position text messages one on top of another
 We do this by adding a value to the y coordinate of the message\n
 --------------------------------------------------------------------------
 */
static bool drawTextAndWaitRoundWin(GameData *gamePtr) {

	FENTRY();
	TRACE();
	char textBuffer[MAXBUFFER];
	if ((gamePtr->roundNumber == gamePtr->maxscore) || (gamePtr->remainingCars == 0)){
		gamePtr->roundNumber = 1;
		GamePlayer* ptr;
		if(gamePtr->player[0].carsSmashed > gamePtr->player[1].carsSmashed) {
			ptr = &gamePtr->player[0];
		}
		else if(gamePtr->player[0].carsSmashed < gamePtr->player[1].carsSmashed) {
			ptr = &gamePtr->player[1];
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
		int next = drawTextOnScreen(gamePtr, textBuffer, gamePtr->display.width / 2,
				gamePtr->display.height / 4, largeFont_c);
		sprintf(textBuffer, "Score: %s %d %s %d", gamePtr->player[1].name, gamePtr->player[1].carsSmashed,
				gamePtr->player[0].name, gamePtr->player[0].carsSmashed);
		next = drawTextOnScreen(gamePtr, textBuffer, gamePtr->display.width / 2, next,
				regularFont_c);

		drawTextOnScreen(gamePtr, (char*) "Press a key to begin or ESC to exit",
				gamePtr->display.width / 2, next + 100, regularFont_c);

		playSound(gamePtr->winsample);
		sprintf(textBuffer, "[Mode: %s] [Score: %s %d %s %d]",
				(gamePtr->gameMode ? "Arcade" : "Human"), gamePtr->player[1].name,
				gamePtr->player[1].score, gamePtr->player[0].name, gamePtr->player[0].score);
		recordResult(textBuffer);
		gamePtr->player[1].score = 0;
		gamePtr->player[0].score = 0;
		gamePtr->bcolor = gamePtr->initcolor;
		initBrickLayout(gamePtr);
		setBrickInfo(gamePtr);

	} else {
		sprintf(textBuffer, "Score: %s %d %s %d",
				gamePtr->player[1].name, gamePtr->player[1].carsSmashed, gamePtr->player[0].name,
				gamePtr->player[0].carsSmashed);
		int next = drawTextOnScreen(gamePtr, textBuffer, gamePtr->display.width / 2,
				gamePtr->display.height / 4, regularFont_c);
		char buffer[100];
		sprintf(buffer, "Press a key to begin Round %d of %d or ESC to exit", ++gamePtr->roundNumber, gamePtr->maxscore);
		drawTextOnScreen(gamePtr, buffer, gamePtr->display.width / 2, gamePtr->display.height / 2+ 100, regularFont_c);
		//DEBUG(" =======\n");
	}

	al_flip_display();

	if (pressAnyKeyToBeginGame(gamePtr) == false) {
		FEXIT();
		return false;
	}

	for (int i = 0; i < 2; i++) {
		gamePtr->player[0].keyPress[i] = false;
		gamePtr->player[1].keyPress[i] = false;
	} //end-of-for
	al_flush_event_queue(gamePtr->eventqueue);

	FEXIT();
	return true;
} // end-of-function DisplayTextAndWaitBegin

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 18, 2019
 @mname   displayScore
 @details
 \n
 --------------------------------------------------------------------------
 */
static bool displayScore(GameData *gamePtr) {

	FENTRY();
	TRACE();
	char textBuffer[MAXBUFFER];
	sprintf(textBuffer, "Score: %s %d %s %d",
			gamePtr->player[1].name, gamePtr->player[1].carsSmashed, gamePtr->player[0].name,
			gamePtr->player[0].carsSmashed);
	int next = drawTextOnScreen(gamePtr, textBuffer, gamePtr->display.width -100,
			30, smallFont_c);
	sprintf(textBuffer, "Remain: %d",
			gamePtr->remainingCars);
	next = drawTextOnScreen(gamePtr, textBuffer, gamePtr->display.width -100,
			next, smallFont_c);
	sprintf(textBuffer, "%d Points Per Smash",
			gamePtr->scorePointsPerSmash);
	next = drawTextOnScreen(gamePtr, textBuffer, gamePtr->display.width -100,
			next, smallFont_c);
}

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   DrawBitmap
 @details
 \n
 --------------------------------------------------------------------------
 */
static void drawBitmap(GameBasicBlock *g) {
	FENTRY();
	TRACE();
	al_draw_bitmap(g->bmap, g->xposition, g->yposition, 0);
	FEXIT();

} // end-of-function DrawBitmap

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 28, 2019
 @mname   DrawBitmapSection
 @details
 Draws only a selected portion of a bitmap.
 It is used to change the length of the pallete depending on the game level.\n
 --------------------------------------------------------------------------
 */
static void drawBitmapSection(GameBasicBlock *g) {
	FENTRY();
	TRACE();
	al_draw_bitmap_region(g->bmap, 0, 0, g->width, g->height, g->xposition,
			g->yposition, 0);
	FEXIT();
} // end-of-function DrawBitmapSection

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   DrawObjects
 @details
 This function sets the background color and draws the players and the ball
 Has to be called every time we want to refresh the display during gameplay\n
 --------------------------------------------------------------------------
 */
static void drawObjects(GameData *gamePtr) {

	FENTRY();
	TRACE();
	setBackgroundColor(*(gamePtr->bcolor));
	drawBitmapSection(&(gamePtr->player[0].ge));
	drawBitmapSection(&(gamePtr->player[1].ge));
	drawBitmap(&(gamePtr->ball));
	for (int i = 0; i < MAXBRICKROWS; i++) {
		for (int j = 0; j < MAXBRICKCOLUMNS; j++) {
			if (gamePtr->bricks[i][j].onScreen == true) {
				drawBitmap(&(gamePtr->bricks[i][j]));
			}//end-of-if
		} //end-of-for
	} //end-of-for
	displayScore(gamePtr);
	FEXIT();
} // end-of-function DrawObjects

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 17, 2019
 @mname   CheckSideCollision
 @details
 true if there is a collision with top or bottom\n
 --------------------------------------------------------------------------
 */
static bool checkCollisionLeftRight(GameData *gamePtr) {

	FENTRY();
	TRACE();
	if (gamePtr->ball.xposition > (gamePtr->display.width - gamePtr->ball.width)) {
		gamePtr->ball.xposition = gamePtr->display.width - gamePtr->ball.width;
		if (gamePtr->ball.xspeed > 0)
			gamePtr->ball.xspeed *= -1;
		FEXIT();
		return true;
	} else if (gamePtr->ball.xposition < 0) {
		gamePtr->ball.xposition = 0;
		if (gamePtr->ball.xspeed < 0)
			gamePtr->ball.xspeed *= -1;
		FEXIT();
		return true;
	}

	FEXIT();
	return false;
} // end-of-function CheckSideCollision

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 17, 2019
 @mname   CheckTopBottomCollision
 @details
 Checks if the ball hits either player's side of the field and grants a roundwin\n
 --------------------------------------------------------------------------
 */
static bool checkCollisionTopAndBottom(GameData *gamePtr) {

	FENTRY();
	TRACE();
	if ((gamePtr->ball.yposition >= (gamePtr->display.height - gamePtr->ball.height))
			&& (gamePtr->ball.yspeed > 0)) {
		gamePtr->player[1].carsSmashed += POINTSFORLOSTBALL;
		//gamePtr->player[1].totalpoints++;
		gamePtr->roundWinner = &(gamePtr->player[1]);
		FEXIT();
		return true;

	} else if ((gamePtr->ball.yposition <= 0) && (gamePtr->ball.yspeed < 0)) {
		TRACE();
		gamePtr->player[0].carsSmashed += POINTSFORLOSTBALL;
		//gamePtr->player[0].totalpoints++;
		gamePtr->roundWinner = &(gamePtr->player[0]);
		FEXIT();
		return true;
	}FEXIT();
	return false;
} // end-of-function CheckTopBottomCollision

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 23, 2019
 @mname   PlaySound
 @details
 \n
 --------------------------------------------------------------------------
 */
static void playSound(ALLEGRO_SAMPLE *s) {
	FENTRY();
	TRACE();
	if (s)
		al_play_sample(s, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	FEXIT();
} // end-of-function PlaySound

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Jun 2, 2019
 @mname   StopTimers
 @details
 Stops all game timers \n
 --------------------------------------------------------------------------
 */
static void stopTimers(GameData *gamePtr) {

	FENTRY();
	TRACE();
	al_stop_timer(gamePtr->timer);
	if (gamePtr->gameMode != human_c)
		al_stop_timer(gamePtr->botTimer);
	FEXIT();

} // end-of-function StopTimers

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Jun 2, 2019
 @mname   StartTimers
 @details
 \n
 --------------------------------------------------------------------------
 */
static void startTimers(GameData *gamePtr) {

	FENTRY();
	TRACE();
	al_start_timer(gamePtr->timer);
	if (gamePtr->gameMode != human_c)
		al_start_timer(gamePtr->botTimer);
	FEXIT();
} // end-of-function StartTimers

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   PrintRoundWinner
 @details
 When the round ends, we need to stop the timers from firing unwanted events
 We do that at the beginning of the function
 Prints a message and play a sound
 Then we wait for user input to restart the game\n
 --------------------------------------------------------------------------
 */
static bool printRoundWinner(GameData *gamePtr) {

	FENTRY();
	TRACE();
	stopTimers(gamePtr);
	setInitialObjectPositions(gamePtr);
	drawObjects(gamePtr);

	if (drawTextAndWaitRoundWin(gamePtr) == false) {
		FEXIT();
		return false;
	} else {
		startTimers(gamePtr);
		playSound(gamePtr->startsample);
	}FEXIT();
	return true;
} // end-of-function PrintRoundWinner

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 28, 2019
 @mname   SignOfNumber
 @details
 \n
 --------------------------------------------------------------------------
 */
static int signOfNumber(int value) {

	if (value >= 0) {
		return 1;
	} //end-of-if(value > 0)
	return -1;
} // end-of-function SignOfNumber

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 28, 2019
 @mname   PaletteBounceCalc
 @details
 This function changes the direction of the ball after a collision with the
 pallete.
 The pallete height is divided into vone_c zones
 Depending on which zone makes contact the ball will react in a different way
 This is to increase the unpredictability of the game and make it more fun and challenging\n
 --------------------------------------------------------------------------
 */
static void ballBounceOnPlayer(GameBasicBlock *ball, GamePlayer *playerPtr,
		int maxballspeed) {

	FENTRY();
	TRACE();
	int newxspeed = abs(ball->yspeed) + (rand() % (minballspeed_c / 2));
	if (newxspeed > maxballspeed)
		newxspeed = maxballspeed;
	ball->yspeed = signOfNumber(ball->yspeed) * -1 * newxspeed;

	int zones_c = 8 - playerPtr->paddleSize;
	if (playerPtr->paddleSize == maxPaddleSize_c) {
		ball->xspeed += rand()%2;
	} else {
		int zonelength = playerPtr->ge.width / zones_c;
		int zonenum = (ball->xposition - playerPtr->ge.xposition) / zonelength;
		if (zonenum < 0) {
			zonenum = 0;
		}
		if (zonenum > zones_c - 1) {
			zonenum = zones_c - 1;
		} //end-of-if(zonenum > zones_c -1)

		ball->xspeed += rand()%2;
	}

	playSound(playerPtr->sample);
	FEXIT();

} // end-of-function PaletteBounceCalc

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 17, 2019
 @mname   checkBallCollisionWithObjects
 @details
 true if there is a collision false otherwise
 This function checks if the ball touches the play edge of the pallet
 Player one is the *bottom* edge
 Player two it is the *top* edge.

 We are using inequalities because we update the positions in non-multiples of the field
 length and width because of that it is possible that the ball and pallete Nov slightly superpose
 That condition is a valid collision \n
 --------------------------------------------------------------------------
 */
static bool checkBallCollisionWithObjects(GameData *gamePtr) {
	FENTRY();
	TRACE();

	COLLISIONSIDE side;
	if(areObjectsColliding(&(gamePtr->ball), &(gamePtr->player[0].ge), side)) {
		gamePtr->ball.yposition = gamePtr->player[0].ge.yposition - gamePtr->ball.height;
		ballBounceOnPlayer(&(gamePtr->ball), &(gamePtr->player[0]), gamePtr->maxballspeed);
		gamePtr->turn = &gamePtr->player[0];
		FEXIT();
		return true;
	}

	else if(areObjectsColliding(&(gamePtr->ball), &(gamePtr->player[1].ge), side)) {
		gamePtr->ball.yposition = gamePtr->player[1].ge.yposition + gamePtr->player[1].ge.height;
		ballBounceOnPlayer(&(gamePtr->ball), &(gamePtr->player[1]), gamePtr->maxballspeed);
		gamePtr->turn = &gamePtr->player[1];
		FEXIT();
		return true;
	}

	if(gamePtr->ball.yspeed <= 0) {
		for (int i = MAXBRICKROWS-1; i >=0; i--) {
			for (int j = 0; j < MAXBRICKCOLUMNS; j++) {
				if(isBallBrickCollision(gamePtr, i, j)) {
					FEXIT();
					return true;
				}
			} //end-of-for
		} //end-of-for
	}
	else if(gamePtr->ball.yspeed > 0) {
		for (int i = 0; i < MAXBRICKROWS; i++) {
			for (int j = 0; j < MAXBRICKCOLUMNS; j++) {
				if(isBallBrickCollision(gamePtr, i, j)) {
					FEXIT();
					return true;
				}
			} //end-of-for
		} //end-of-for
	}

	FEXIT();
	return false;
} // end-of-function CheckPaletteCollision

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   UpdateBallPosition
 @details
 return true if round is finished
 This function checks if there is a collision between the ball and the pallet
 If that is not the case, than it checks if there is a collision with the left and right field edges
 This signifies a round win
 If none of the conditions above happen, we need to check a collision with the top and bottom
 edges of the field\n
 --------------------------------------------------------------------------
 */
static bool updateBallPosition(GameData *gamePtr) {
	FENTRY();
	TRACE();

	if(gamePtr->remainingCars == 0) {
		FEXIT();
		return true;
	}
	gamePtr->ball.xposition = gamePtr->ball.xposition + gamePtr->ball.xspeed;
	gamePtr->ball.yposition = gamePtr->ball.yposition + gamePtr->ball.yspeed;

	if (checkBallCollisionWithObjects(gamePtr) == false) {
		if (checkCollisionTopAndBottom(gamePtr) == true) {
			FEXIT();
			return true;
		}
	}
	checkCollisionLeftRight(gamePtr);
	FEXIT();
	return false;
} // end-of-function UpdateBallPosition

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 14, 2019
 @mname   lrtBotControl
 @details
 This function controls the LRT bot.\n
 --------------------------------------------------------------------------
 */
static void lrtBotControl(GameData *gamePtr) {

	FENTRY();
	TRACE();
	//update only when ball moves towards the player
	//We decide to move up based on the ball Y speed
	// if Y speed > 0 it means the ball is moving downward
	//If the Y speed < 0 it means the ball is moving upwards
	//bot 1 is at the top
	uint botNumber = 1;

	if (gamePtr->ball.yspeed > 0) {
		FEXIT();
		return;
	}

	float mult = 1;
	if (gamePtr->ball.yposition > gamePtr->display.height / lrtBotPtr->cond[0])
		mult = lrtBotPtr->val[0];
	if (gamePtr->ball.yposition <= gamePtr->display.height / lrtBotPtr->cond[1])
		mult = lrtBotPtr->val[1];
	if (gamePtr->ball.yposition <= gamePtr->display.height / lrtBotPtr->cond[2])
		mult = lrtBotPtr->val[2];
	if (gamePtr->ball.yposition <= gamePtr->display.height / lrtBotPtr->cond[3])
		mult = lrtBotPtr->val[3];
	if (gamePtr->ball.yposition <= gamePtr->display.height / lrtBotPtr->cond[4])
		mult = lrtBotPtr->val[4];

	if(gamePtr->ball.yposition > gamePtr->display.height/2)
		mult = 0;


	float f = gamePtr->player[botNumber].paddleSpeed * mult;
	if (gamePtr->ball.xspeed > 0) {
		if (gamePtr->ball.xposition > (gamePtr->player[botNumber].ge.xposition + gamePtr->player[botNumber].ge.width)) {
			gamePtr->player[botNumber].ge.xposition += (int) f;
		} else if (gamePtr->ball.xposition < gamePtr->player[botNumber].ge.xposition) {
			gamePtr->player[botNumber].ge.xposition -= (int) f;
		}
	} else {
		if (gamePtr->ball.xposition < gamePtr->player[botNumber].ge.xposition) {
			gamePtr->player[botNumber].ge.xposition -= (int) f;
		} else if (gamePtr->ball.xposition  > (gamePtr->player[botNumber].ge.xposition + gamePtr->player[botNumber].ge.width)) {
			gamePtr->player[botNumber].ge.xposition += (int) f;
		}
	}

	//end of display to the left
	if (gamePtr->player[botNumber].ge.xposition < 0)
		gamePtr->player[botNumber].ge.xposition = 0;
	//end of display to the right
	if (gamePtr->player[botNumber].ge.xposition >= (gamePtr->display.width - gamePtr->player[botNumber].ge.width))
		gamePtr->player[botNumber].ge.xposition = (gamePtr->display.width - gamePtr->player[botNumber].ge.width);

	FEXIT();
} // end-of-function botControl


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 19, 2019
 @mname   busBotControl
 @details
  This function controls the bus Bot\n
 --------------------------------------------------------------------------
 */
static void busBotControl(GameData *gamePtr) {

	FENTRY();
	TRACE();
	//update only when ball moves towards the player
	//We decide to move up based on the ball Y speed
	// if Y speed > 0 it means the ball is moving downward
	//If the Y speed < 0 it means the ball is moving upwards
	uint botNumber = 0;

	//bot 0 is at the bottom
	if (gamePtr->ball.yspeed < 0) {
		FEXIT();
		return;
	}

	float mult = 1;
	if ((gamePtr->display.height - gamePtr->ball.yposition) > gamePtr->display.height / busBotPtr->cond[0])
		mult = busBotPtr->val[0];
	if ((gamePtr->display.height - gamePtr->ball.yposition) <= gamePtr->display.height / busBotPtr->cond[1])
		mult = busBotPtr->val[1];
	if ((gamePtr->display.height - gamePtr->ball.yposition) <= gamePtr->display.height / busBotPtr->cond[2])
		mult = busBotPtr->val[2];
	if ((gamePtr->display.height - gamePtr->ball.yposition) <= gamePtr->display.height / busBotPtr->cond[3])
		mult = busBotPtr->val[3];
	if ((gamePtr->display.height - gamePtr->ball.yposition) <= gamePtr->display.height / busBotPtr->cond[4])
		mult = busBotPtr->val[4];

	if(gamePtr->ball.yposition< gamePtr->display.height/2)
		mult = 0;


	float f = gamePtr->player[botNumber].paddleSpeed * mult;
	if (gamePtr->ball.xspeed > 0) {
		if (gamePtr->ball.xposition > (gamePtr->player[botNumber].ge.xposition + gamePtr->player[botNumber].ge.width)) {
			gamePtr->player[botNumber].ge.xposition += (int) f;
		} else if (gamePtr->ball.xposition < gamePtr->player[botNumber].ge.xposition) {
			gamePtr->player[botNumber].ge.xposition -= (int) f;
		}
	} else {
		if (gamePtr->ball.xposition < gamePtr->player[botNumber].ge.xposition) {
			gamePtr->player[botNumber].ge.xposition -= (int) f;
		} else if (gamePtr->ball.xposition  > (gamePtr->player[botNumber].ge.xposition + gamePtr->player[botNumber].ge.width)) {
			gamePtr->player[botNumber].ge.xposition += (int) f;
		}
	}

	//end of display to the left
	if (gamePtr->player[botNumber].ge.xposition < 0)
		gamePtr->player[botNumber].ge.xposition = 0;
	//end of display to the right
	if (gamePtr->player[botNumber].ge.xposition >= (gamePtr->display.width - gamePtr->player[botNumber].ge.width))
		gamePtr->player[botNumber].ge.xposition = (gamePtr->display.width - gamePtr->player[botNumber].ge.width);

	FEXIT();
} // end-of-function botControl


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   GameLoop
 @details
 Two dimensional games process events and screen updates in a continuous loop
 usually this loop is called a game loop
 The loop processes events from p->eventqueue.
 Events come from the two game timers (one is for screen refresh,
 the other is for HAL AI) as well as keyboard and mouse events.
 The loop waits for an event to be fired and then processes the event
 If the event is of the type refresh display, all objects are written to a display buffer
 Then that buffer is shown on the screen. \n
 --------------------------------------------------------------------------
 */
static bool gameMainLoop(GameData *gamePtr) {

	FENTRY();
	TRACE();
	startTimers(gamePtr);

	bool roundwin = false;
	int skipCounter = 0;

	playSound(gamePtr->startsample);
	//We are waiting for an event from on one of the sources that are linked to the event queue
	//The frame-timer, keyboard, mouse, and HAL timer if in arcade mode
	//This function blocks until an event is recieved
	//Therefore if the timers would not be started, this function would return only on a keyboard or mouse event
	while (true) {
		al_wait_for_event(gamePtr->eventqueue, &(gamePtr->ev));

		if (gamePtr->ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			FEXIT();
			return false;
		}
		//If the round is won we need to stop the game for 1 second
		//We do this by counting timer events without processing them which in effect
		//skips frames
		if (roundwin == true) {
			TRACE();
			if (gamePtr->ev.type == ALLEGRO_EVENT_TIMER
					&& gamePtr->ev.timer.source == gamePtr->timer) {
				//skip maxSkip frames
				//At the end of each round we want to keep the last frame of the play that shows where the ball exitied the screen
				//for a little longer, so the user can see who won the round
				//We do this by counting frame timer events
				if (skipCounter++ >= (int) gamePtr->fps) {
					skipCounter = 0;
					roundwin = false;
					if (printRoundWinner(gamePtr) == false) {
						//user has pressed ESC to end the game
						FEXIT();
						return false;
					}
				}

			} else
				continue;
		} else {
			TRACE();

			//if this is a hal logic event we need to let hal work
			if (gamePtr->gameMode != human_c && gamePtr->ev.type == ALLEGRO_EVENT_TIMER
					&& gamePtr->ev.timer.source == gamePtr->botTimer) {
				//if we are in arcade mode and the timer event belongs to the hal timer then
				// we have to run the bot control logic
				lrtBotControl(gamePtr);
				busBotControl(gamePtr);
			} else {
				//check if escape key has been pressed
				if (processKeyPressEvent(gamePtr) == false) {
					//user has ended game
					FEXIT();
					return false;
				}
			}
			//check if we need to update the frame
			if (gamePtr->ev.type == ALLEGRO_EVENT_TIMER
					&& gamePtr->ev.timer.source == gamePtr->timer) {
				//If this is a screen update timer event then we have to redraw the screen
				//we have to update the ball position and then draw all objects (players and ball)
				//Calculates next position of the paddles based on the key inputs read above
				movePlayers(gamePtr);
				roundwin = updateBallPosition(gamePtr);
				drawObjects(gamePtr);
				//This function shows the content of the display buffer on the screen.
				al_flip_display();
			}
		}
	}

	FEXIT();
	return true;
} // end-of-function gameMainLoop

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   GameExit
 @details
 This function is called when the game terminates and it destroys all allegro resources
 \n
 --------------------------------------------------------------------------
 */
static void exitGame(GameData *gamePtr) {
	FENTRY();
	TRACE();
	al_rest(0.0);
	al_destroy_display(gamePtr->display.display);
	al_destroy_timer(gamePtr->botTimer);
	al_destroy_timer(gamePtr->timer);
	al_destroy_event_queue(gamePtr->eventqueue);
	for (int i = 0; i < MAXFONTS; i++) {
		if (gamePtr->font[i]) {
			al_destroy_font(gamePtr->font[i]);
		} //end-of-if(p->font[i])
	} //end-of-for

	al_destroy_bitmap(gamePtr->player[0].ge.bmap);
	al_destroy_bitmap(gamePtr->player[1].ge.bmap);
	al_destroy_bitmap(gamePtr->ball.bmap);
	FEXIT();
} // end-of-function GameExit

//======== PUBLIC FUNCTIONS ===========
//The functions below are called from the main function

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 17, 2019
 @mname   CreateGameData
 @details
 This function gets the game config parameters as read from the config file
 In the same format as the parameters passes to the main file
 argv is an array of character pointers and argc is the is number of entries in the array
 Processing is done in the same style as the main command line arguments\n
 --------------------------------------------------------------------------
 */
bool initializeGameData(int argc, char **argv) {
	//sets the default player 1 and player 2 names
	FENTRY();
	TRACE();
	GameData *p = &carBreaker;

	srand(time(0));

	strcpy(p->player[0].name, "Player1");
	strcpy(p->player[1].name, "Player2");
	strcpy(p->fontFileName, FONTNAME);
	strcpy(p->player[0].audioFileName, P1SOUND);
	strcpy(p->player[1].audioFileName, P2SOUND);
	strcpy(p->p1BitmapName, P1FNAME);
	strcpy(p->p2BitmapName, P2FNAME);
	strcpy(p->ballBitmapName, BALLFNAME);
	strcpy(p->gasBitmapName, GASCARFNAME);
	strcpy(p->ecarBitmapName, ECARFNAME);

	// today
	p->year = 2019;


	p->gameNumber = 1;
	p->roundNumber = 1;
	p->bcolor = &(p->bcolorarray[yellow_c]);

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
		} else if (strcmp(argv[param], "fontsize") == 0) {
			//font size
			if (++param < argc)
				p->fontsize = atoi(argv[param]);
		} else if (strcmp(argv[param], "maxballspeed") == 0) {
			// maximum number of pixels the ball will move between frames
			if (++param < argc) {
				p->maxballspeed = atoi(argv[param]);
				if (p->maxballspeed <= maxballspeed_c)
					p->maxballspeed = maxballspeed_c;
			}
		} else if (strcmp(argv[param], "p1name") == 0) {
			//player1 name
			if (++param < argc)
				strcpy(p->player[0].name, argv[param]);
		} else if (strcmp(argv[param], "p2name") == 0) {
			//player2 name
			if (++param < argc)
				strcpy(p->player[1].name, argv[param]);
		} else if (strcmp(argv[param], "maxscore") == 0) {
			//maxscore
			if (++param < argc)
				p->maxscore = atoi(argv[param]);
		} else if (strcmp(argv[param], "fontfile") == 0) {
			//font file name
			if (++param < argc)
				strcpy(p->fontFileName, argv[param]);
		} else if (strcmp(argv[param], "winSound") == 0) {
			//font file name
			if (++param < argc)
				strcpy(p->winSoundFile, argv[param]);
		} else if (strcmp(argv[param], "player1bmp") == 0) {
			//player 1 bitmap file name
			if (++param < argc)
				strcpy(p->p1BitmapName, argv[param]);
		} else if (strcmp(argv[param], "player2bmp") == 0) {
			//player 2 bitmap file name
			if (++param < argc)
				strcpy(p->p2BitmapName, argv[param]);
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
		} else if (strcmp(argv[param], "player1sound") == 0) {
			//player 1 sound file name
			if (++param < argc)
				strcpy(p->player[0].audioFileName, argv[param]);
		} else if (strcmp(argv[param], "player2sound") == 0) {
			//player 2 sound file name
			if (++param < argc)
				strcpy(p->player[1].audioFileName, argv[param]);
		} else if (strcmp(argv[param], "p1paddleSpeed") == 0) {
			//player 1 paddle speed
			if (++param < argc) {
				p->player[0].paddleSpeed = atoi(argv[param]);
				busBotArray[0].paddlespeed = p->player[1].paddleSpeed / 2;
				busBotArray[1].paddlespeed = p->player[1].paddleSpeed;
				busBotArray[2].paddlespeed = (3 * p->player[1].paddleSpeed) / 2;
				busBotArray[3].paddlespeed = p->player[1].paddleSpeed * 2;
			}
		} else if (strcmp(argv[param], "p2paddleSpeed") == 0) {
			//player 2 paddle speed
			if (++param < argc) {
				p->player[1].paddleSpeed = atoi(argv[param]);
				lrtBotArray[0].paddlespeed = p->player[1].paddleSpeed / 2;
				lrtBotArray[1].paddlespeed = p->player[1].paddleSpeed;
				lrtBotArray[2].paddlespeed = (3 * p->player[1].paddleSpeed) / 2;
				lrtBotArray[3].paddlespeed = p->player[1].paddleSpeed * 2;
			}
		} else if (strcmp(argv[param], "p1level") == 0) {
			//level (controls the paddle size)
			if (++param < argc) {
				p->player[0].paddleSize = atoi(argv[param]);
				if (p->player[0].paddleSize > maxPaddleSize_c)
					p->player[0].paddleSize = maxPaddleSize_c;
			}
		} else if (strcmp(argv[param], "p2level") == 0) {
			//level (controls the paddle size)
			if (++param < argc) {
				p->player[1].paddleSize = atoi(argv[param]);
				if (p->player[1].paddleSize > maxPaddleSize_c)
					p->player[1].paddleSize = maxPaddleSize_c;
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
		} else if (strcmp(argv[param], "colourscheme") == 0) {
			//player 2 bitmap file name
			if (++param < argc) {
				switch (argv[param][0]) {
				case 'y':
					p->bcolor = &(p->bcolorarray[yellow_c]);
					break;
				case 'b':
					p->bcolor = &(p->bcolorarray[blue_c]);
					break;
				case 'w':
					p->bcolor = &(p->bcolorarray[white_c]);
					break;
				case 'g':
					p->bcolor = &(p->bcolorarray[green_c]);
					break;
				case 'q':
					p->bcolor = &(p->bcolorarray[grey_c]);
					break;
				default:
					break;
				} //end-switch(argv[param][0])
				p->initcolor = p->bcolor;
			}
		}
	} //end-of-for

	initBrickLayout(p);
	FEXIT();
	return true;
} // end-of-function CreateGameData

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 22, 2019
 @mname   InitGame
 @details
 returns 1 if init ok, 0 otherwise
 This function does the following:
 1. Initialises all allegro resources
 2. Loads all game resources (fonts, bitmaps, sounds)
 --------------------------------------------------------------------------
 */
bool initializeGraphics() {
	FENTRY();
	TRACE();
	GameData *p = &carBreaker;
	//seed random number generator with time
	srand (time(NULL));
	//initiallises allegro libraries
	if(al_init() == 0) {
		ERROR("Cannot init allegro");
		FEXIT();
		return false;
	} //end-of-if(al_init() == 0)
	TRACE();
	al_init_primitives_addon();
	al_init_image_addon();
	al_install_keyboard();
	al_install_mouse();
	al_init_font_addon(); // initialize the font addon
	al_init_ttf_addon(); // initialize the ttf (True Type Font) addon
	al_install_audio();
	al_init_acodec_addon();
	//al_reserve_samples(2);

	TRACE();
	//tries to load font file
	if (loadFont(p, smallFont_c) == false) {
		FEXIT();
		return false;
	} //end-of-if(LoadFont(p, smallFont_c) == false)

	TRACE();
	if (loadFont(p, regularFont_c) == false) {
		FEXIT();
		return false;
	} //end-of-if(LoadFont(p, regularFont_c) == false)

	TRACE();
	if (loadFont(p, largeFont_c) == false) {
		FEXIT();
		return false;
	} //end-of-if(LoadFont(p, largeFont_c) == false)

	TRACE();
	if ((p->display.display = al_create_display(p->display.width,
			p->display.height)) == NULL) {
		ERROR("Cannot init display");FEXIT();
		return false;
	}

	TRACE();
	p->bcolorarray[yellow_c] = al_map_rgb(255, 255, 0);
	p->bcolorarray[blue_c] = al_map_rgb(200, 200, 255);
	p->bcolorarray[grey_c] = al_map_rgb(180, 180, 180);
	p->bcolorarray[white_c] = al_map_rgb(0, 0, 0);
	p->bcolorarray[green_c] = al_map_rgb(0, 180, 0);

	p->fcolor = al_map_rgb(0, 100, 0);
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
	if (p->gameMode == arcade_c || p->gameMode == fullbot_c) {
		INFO("Arcade/Full Auto Modes Detected\n");
		p->botTimer = al_create_timer(1.0 / (float) p->player[1].paddleSpeed);
		al_register_event_source(p->eventqueue,
				al_get_timer_event_source(p->botTimer));
	} else
		p->botTimer = NULL;

	if (loadPlayerBitmap(&(p->player[0]), p->p1BitmapName) == false) {
		FEXIT();
		return false;
	}
	if (loadPlayerBitmap(&(p->player[1]), p->p2BitmapName) == false) {
		FEXIT();
		return false;
	}
	if (loadBitmap(&(p->ball), p->ballBitmapName) == false) {
		FEXIT();
		return false;
	}

	if ((p->gasBitmap = al_load_bitmap(p->gasBitmapName)) == NULL) {
		printf("cannot load %s\n ", p->gasBitmapName);
		FEXIT();
		return false;
	}

	if ((p->ecarBitmap = al_load_bitmap(p->ecarBitmapName)) == NULL) {
		printf("cannot load %s\n ", p->ecarBitmapName);
		FEXIT();
		return false;
	}

	setBrickInfo(p);

	loadAudio(&(p->player[0]));
	loadAudio(&(p->player[1]));
	loadAudioWinner(p);

	setInitialObjectPositions(p);

	setBackgroundColor(*(p->bcolor));
	FEXIT();
	return true;
} // end-of-function InitGame

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 25, 2019
 @mname   GameRun
 @details
 This is the function called from the main function
 1. Displays the initial screen
 2. Calls the game loop
 --------------------------------------------------------------------------
 */
void runGame() {
	FENTRY();
	TRACE();
	GameData *p = &carBreaker;
	setBackgroundColor(*(p->bcolor));
	if (drawTextAndWaitBegin(p) == true) {
		gameMainLoop(p);
	}

	exitGame(p);
	FEXIT();
} // end-of-function GameRun
