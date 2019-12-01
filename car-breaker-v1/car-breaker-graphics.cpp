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

static const int minballspeed_c = 3;
static const int maxballspeed_c = minballspeed_c + 2;
static const uint maxPaddleSize_c = 7;
static const uint maxdiff_c = 4;
static const uint botArrays_c = 5;
static const int ballSpeedIncrease_c = 3;
static const int initPaddleSpeed_c = 20;

static const char P1FNAME[] = "player1.png";
static const char P2FNAME[] = "player2.png";
static const char BALLFNAME[] = "ball.png";
static const char GASCARFNAME[] = "gascar.png";
static const char ECARFNAME[] = "ecar.png";
static const char P1SOUND[] = "cardoor.wav";
static const char P2SOUND[] = "cardoor.wav";
static const char FONTNAME[] = "pirulen.ttf";

//========VARIABLE DECLARATIONS=====
//declaring the main data variable of the game
//usually passed to functions using a pointer
GameData carBreaker = {
		{INITPLAYER, INITPLAYER},
		INITGBB,
		INITDISPLAY, human_c, maxballspeed_c, NULL, FONTSIZE, MAXROUNDS, { 0 }, { 0 },
		FRAMERATE

};

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 2, 2019
 @mname   initBrickLAyout
 @details
   initializes the brick layout\n
 --------------------------------------------------------------------------
 */
void  initBrickLayout(GameData*gamePtr) {

	FENTRY();
	TRACE();
	gamePtr->remainingCars = 0;



	int year = (gamePtr->year > 2080)?2080:gamePtr->year;
	uint rNumber = (year > 2020)? (2100- year )/10:10;
	uint maxCars = gamePtr->maxRows * gamePtr->maxColumns;

	if(year > 2020) {
		maxCars -= (2080-year)*maxCars/160;
	}


	// number of smashable cars in top and bottom row must be similar
	// othewise there is in an advantage to one player
	int topCount = 0;

	int totalCount = 0;

	for(int i = 0; i < gamePtr->maxRows; i++) {
		for (int j = 0; j < gamePtr->maxColumns; j++) {
			gamePtr->bricks[i][j].onScreen = (rand() % rNumber)?true:false;
			if(gamePtr->bricks[i][j].onScreen == true ) {
				// ecars are 'indestructible'
				// as times goes by more are ecars
				// but also less cars on road
				totalCount++;
				if(gamePtr->year >= 2000) {
					if(i == gamePtr->maxRows - 1) {
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

	for(int i = 1; i < gamePtr->maxRows-1; i++) {
		for (int j = 1; j < gamePtr->maxColumns-1; j++) {
			if(gamePtr->bricks[i][j].indestructible  == false) {
				// do not allow gas cars to be surrounded by ecars
				// in that case the cars cannot be smashed
				// check if the gas car is surrounded and if it is, make it an ecar
				if((gamePtr->bricks[i-1][j].indestructible == true)
						&& (gamePtr->bricks[i][j-1].indestructible == true)
						&& (gamePtr->bricks[i][j+1].indestructible == true)
						&& (gamePtr->bricks[i+1][j].indestructible == true)
				){
					gamePtr->bricks[i][j].indestructible = true;
					if(gamePtr->bricks[i][j].onScreen == true)
						gamePtr->remainingCars--;
					else
						gamePtr->bricks[i][j].onScreen = true;
				}
			}
		} //end-of-for
	} //end-of-for

	gamePtr->gameNumber++;

	FEXIT();
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
void  setBrickInfo(GameData* p) {
	FENTRY();
	TRACE();

	p->carArea.xspeed = 0;
	p->carArea.yspeed = 0;
	p->carArea.onScreen = false;
	p->carArea.width = 0;
	p->carArea.height = 0;

	for (int i = 0; i < p->maxRows; i++) {
		for (int j = 0; j < p->maxColumns; j++) {
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

	for (int j = 0; j < p->maxColumns; j++) {
		p->carArea.width += p->bricks[0][j].width;
	}
	for (int i = 0; i < p->maxRows; i++) {
		p->carArea.height += p->bricks[i][0].height;
	}

	if(p->carArea.width > p->display.width) {
		int c1 = p->display.width / p->bricks[0][0].width;
		for (int i = c1; i < p->maxColumns; i++ ) {
			for (int j = 0; j < p->maxRows; j++ ) {
				if((p->bricks[j][i].onScreen == true) && (p->bricks[j][i].indestructible == false)) {
					p->remainingCars--;
				}
				p->bricks[j][i].onScreen = false;
			} //end-of-for
		} //end-of-for
		p->carArea.width  = p->display.width;
		p->maxColumns = c1;
	}
	FEXIT();
	return;
}

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 3, 2019
 @mname   setPointsPerCarSmashed
 @details
   set an amount of points for each car smashed
   this will change based on the number of cars that
   are still present on screen\n
 --------------------------------------------------------------------------
 */
void  setPointsPerSmash(GameData*gamePtr) {
	FENTRY();
	TRACE();
	if(gamePtr->remainingCars<= level6_c){
		gamePtr->scorePointsPerSmash = 10;
	}else if (gamePtr->remainingCars< level5_c) {
		gamePtr->scorePointsPerSmash = 3;
	}else if (gamePtr->remainingCars< level4_c) {
		gamePtr->scorePointsPerSmash = 2;
	}else {
		gamePtr->scorePointsPerSmash = 1;
	}

	FEXIT();
}


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 30, 2019
 @mname   fastBall
 @details
   increase one component of the ball speed
   very useful when we get into an infinite ricochet loop\n
 --------------------------------------------------------------------------
 */
void fastBall(GameData* g) {
	g->maxXSpeed += 2;
	g->maxYSpeed += 2;
}


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Nov 30, 2019
 @mname   slowBall
 @details
    decrease the largest component of speed
    very useful to get out of infinite ricochet loops\n
 --------------------------------------------------------------------------
 */
void slowBall(GameData* g) {
	g->maxXSpeed -= 2;
	g->maxYSpeed -= 2;
}


/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 3, 2019
 @mname   isPointInObject
 @details
  check if a point of the ball is in another object\n
 --------------------------------------------------------------------------
 */
bool isPointInObject(GameBasicBlock* b, int x, int y){
	FENTRY();
	TRACE();
	if((x>=b->xposition)
			&& (x <= (b->xposition+b->width))
			&&(y>=b->yposition)
			&& (y<=(b->yposition+b->height))) {
		FEXIT();
		return true;
	}
	else {
		FEXIT();
		return false;
	}
}



/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 3, 2019
 @mname   isBallInArea
 @details
   check if the ball is in a particular rectangle
 --------------------------------------------------------------------------
 */
bool isBallInRegion(GameBasicBlock* ball, GameBasicBlock* obj){

	FENTRY();
	TRACE();
	bool result = isPointInObject(obj,ball->xposition,ball->yposition);
	result = result || isPointInObject(obj, ball->xposition+ball->width, ball->yposition);
	result = result || isPointInObject(obj,ball->xposition, ball->yposition+ball->height);
	result = result || isPointInObject(obj, ball->xposition+ball->width, ball->yposition+ball->height);

	if(result == false) {
		FEXIT();
		return false;
	}

	FEXIT();
	return true;

}

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 3, 2019
 @mname   areObjectsColliding
 @details
  check if the ball is colliding with another objects
  the side parameter will return the side of the collision
  this side refers to the object not the ball\n
 --------------------------------------------------------------------------
 */
bool areObjectsColliding(GameBasicBlock* ball, GameBasicBlock* obj, COLLISIONSIDE& side){

	FENTRY();
	TRACE();
	bool result = isPointInObject(obj,ball->xposition,ball->yposition);
	result = result || isPointInObject(obj, ball->xposition+ball->width, ball->yposition);
	result = result || isPointInObject(obj,ball->xposition, ball->yposition+ball->height);
	result = result || isPointInObject(obj, ball->xposition+ball->width, ball->yposition+ball->height);

	if(result == false) {
		FEXIT();
		return false;
	}

	float ballXCenter = ball->xprevposition + (float) ball->width/2;
	float ballYCenter = ball->yprevposition + (float) ball->height/2;

	float objXCenter = obj->xposition + (float) obj->width/2;
	float objYCenter = obj->yposition + (float) obj->height/2;

	if(ball->xprevposition == (obj->xposition+obj->width)) {
		side = right_c;
		FEXIT();
		return true;
	}

	if((ball->xprevposition + ball->width) == obj->xposition) {
		side = left_c;
		FEXIT();
		return true;
	}

	if(ball->yprevposition == (obj->yposition+obj->height)) {
		side = bottom_c;
		FEXIT();
		return true;
	}

	if((ball->yprevposition + ball->height) == obj->yposition) {
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
 @date    Dec 3, 2019
 @mname   ballBrickCollision
 @details
   this function returns true if the ball has collided with a brick
   and false otherwise\n
 --------------------------------------------------------------------------
 */
bool isBallBrickCollision(GameData* gamePtr, int i, int j) {

	FENTRY();
	TRACE();
	if (gamePtr->bricks[i][j].onScreen == false) {
		FEXIT();
		return false;
	}

	COLLISIONSIDE side;

	if(areObjectsColliding(&(gamePtr->ball), &(gamePtr->bricks[i][j]), side)) {

		if(gamePtr->ball.yspeed == 0) {
			gamePtr->ball.yspeed = rand() %3 -2;
		}
		if(gamePtr->ball.xspeed == 0) {
			gamePtr->ball.xspeed = rand() %3 -2;
		}

		switch(side){
		case top_c:
			gamePtr->ball.yspeed *= -1;
			gamePtr->ball.xspeed += rand() % 2;
			gamePtr->ball.yposition = gamePtr->bricks[i][j].yposition - gamePtr->ball.height;
			//printf("top\n");
			break;
		case bottom_c:
			gamePtr->ball.yspeed *= -1;
			gamePtr->ball.xspeed += rand() % 2;
			gamePtr->ball.yposition = gamePtr->bricks[i][j].yposition + gamePtr->bricks[i][j].height;
			//printf("bottom\n");
			break;
		case left_c:
			gamePtr->ball.xposition = gamePtr->bricks[i][j].xposition - gamePtr->ball.width;
			gamePtr->ball.xspeed *= -1;
			//printf("left\n");
			break;
		case right_c:
			gamePtr->ball.xspeed *= -1;
			gamePtr->ball.xposition = gamePtr->bricks[i][j].xposition + gamePtr->bricks[i][j].width;
			//printf("right\n");
			break;
		default:
			//printf("default\n");
			break;
		}


		if(gamePtr->bricks[i][j].indestructible == false) {
			gamePtr->bricks[i][j].onScreen = false;
			gamePtr->remainingCars--;
			if(gamePtr->turn)
				gamePtr->turn->carsSmashed+= gamePtr->scorePointsPerSmash;
			setPointsPerSmash(gamePtr);
		}
		if(gamePtr->remainingCars < level5_c ) {
			gamePtr->bcolor = &(gamePtr->bcolorarray[green_c]);
		}else if(gamePtr->remainingCars < level4_c ) {
			gamePtr->bcolor = &(gamePtr->bcolorarray[blue_c]);
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
 @date    Dec 6, 2019
 @mname   setBackgroundColor
 @details
   this function sets the background color for the display\n
 --------------------------------------------------------------------------
 */
void  setBackgroundColor(ALLEGRO_COLOR color) {
	FENTRY();
	TRACE();
	al_clear_to_color(color);
	FEXIT();
} // end-of-function SetBackgroundColor

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 1, 2019
 @mname   LoadPlayerBitmap
 @details
   this function loads a bitmap for the player from a file\n
 --------------------------------------------------------------------------
 */
bool loadPlayerBitmap(GamePlayer *p, char* fname) {
	FENTRY();
	TRACE();
	if ((p->ge.bmap = al_load_bitmap(fname)) == NULL) {
		printf("cannot load %s\n ", fname);
		FEXIT();
		return false;
	}
	p->ge.width = (al_get_bitmap_width(p->ge.bmap)* (maxPaddleSize_c + 1 - p->paddleSize)) / maxPaddleSize_c;
	p->ge.height = al_get_bitmap_height(p->ge.bmap);
	FEXIT();
	return true;
} // end-of-function LoadPlayerBitmap

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 6, 2019
 @mname   LoadBitmap
 @details
 return true if ok false otherwise\n
 --------------------------------------------------------------------------
 */
bool loadBitmap(GameBasicBlock *g, char* fname) {
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
 @date    Dec 6, 2019
 @mname   LoadBitmap
 @details
 return true if ok false otherwise\n
 --------------------------------------------------------------------------
 */
bool setBitmap(GameBasicBlock *g, ALLEGRO_BITMAP* b) {
	//FENTRY();
	//TRACE();
	g->bmap = b;
	g->width = al_get_bitmap_width(g->bmap);
	g->height = al_get_bitmap_height(g->bmap);
	//FEXIT();
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
bool loadAudio(GamePlayer *gamePtr) {
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
 @date    Dec 1, 2019
 @mname   LoadWinAudio
 @details
 \n
 --------------------------------------------------------------------------
 */
bool loadAudioWinner(GameData *gamePtr) {
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
   this function will load fonts for the text used to print
   the various graphics\n
 --------------------------------------------------------------------------
 */
bool loadFont(GameData *gamePtr, int size) {

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
 @date    Dec 6, 2019
 @mname   InitialPosition
 @details
 This function sets the players in the middle of the Y axis and provides
 the ball to one of the players
 If it is the first round, the player who has the ball is chosen at random.
 After a round win the round winner gets the serve.\n
 --------------------------------------------------------------------------
 */
void  setInitialObjectPositions(GameData *gamePtr) {

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

	if (gamePtr->ball.yspeed > 0) {
		gamePtr->ball.yprevposition = gamePtr->ball.yposition;
		gamePtr->ball.yposition = gamePtr->player[1].ge.yposition + gamePtr->player[1].ge.height;
	}else {
		gamePtr->ball.yprevposition = gamePtr->ball.yposition;
		gamePtr->ball.yposition = gamePtr->player[0].ge.yposition - gamePtr->ball.height;
	}
	gamePtr->ball.xprevposition = gamePtr->ball.xposition;
	gamePtr->ball.xposition = gamePtr->display.width / 2 - (gamePtr->ball.width / 2);
	if (gamePtr->ball.yspeed > 0) {
		gamePtr->startsample = gamePtr->player[1].sample;
	} else {
		gamePtr->startsample = gamePtr->player[0].sample;
	}

	int ypos = (gamePtr->display.height - gamePtr->bricks[0][0].height*gamePtr->maxRows)/2;

	for (int i = 0; i < gamePtr->maxRows; i++) {
		int xpos = (gamePtr->display.width - gamePtr->bricks[0][0].width*gamePtr->maxColumns)/2;
		if(xpos < 0) {
			xpos = 0;
		}

		for (int j = 0; j < gamePtr->maxColumns; j++) {
			gamePtr->bricks[i][j].yposition = ypos;
			gamePtr->bricks[i][j].xposition = xpos;
			xpos += gamePtr->bricks[i][j].width;
		} //end-of-for
		ypos += gamePtr->bricks[i][0].height;
	} //end-of-for

	gamePtr->carArea.xposition = gamePtr->bricks[0][0].xposition;
	gamePtr->carArea.yposition = gamePtr->bricks[0][0].yposition;

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
bool pauseGame(GameData *gamePtr) {

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
 @date    Dec 6, 2019
 @mname   isKeyPressEvent
 @details
 This function checks for keyboard input
 This function reacts to both keydown events and keyup events
 When a key is pushed down a boolean is set to keep the keep down as it is pressed\n
 --------------------------------------------------------------------------
 */
bool isKeyPressEvent(GameData *gamePtr) {

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
		case ALLEGRO_KEY_Q:
			gamePtr->player[1].keyPress[0] = true;
			gamePtr->player[1].keyPress[1] = false;
			break;
		case ALLEGRO_KEY_E:
			gamePtr->player[1].keyPress[0] = false;
			gamePtr->player[1].keyPress[1] = true;
			break;
		case ALLEGRO_KEY_P:
			if (pauseGame(gamePtr) == false) {
				FEXIT();
				return false;
			}
			break;
		case ALLEGRO_KEY_F:
			fastBall(gamePtr);
			break;
		case ALLEGRO_KEY_S:
			slowBall(gamePtr);
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
		case ALLEGRO_KEY_Q:
			gamePtr->player[1].keyPress[0] = false;
			break;
		case ALLEGRO_KEY_E:
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
} // end-of-function isKeyPressEvent

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 3, 2019
 @mname   pressAnyKeyToBegin
 @details
 \n
 --------------------------------------------------------------------------
 */
bool pressAnyKeyToBeginGame(GameData *gamePtr) {

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
 @mname   movePlayers
 @details
 This function calculates the new positions of the paddles after the keys are pressed\n
 --------------------------------------------------------------------------
 */
void  movePlayers(GameData *gamePtr) {

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
} // end-of-function movePlayers

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 6, 2019
 @mname   DrawText
 @details
 Displays text on screen using allegro
 Declared an enumeration of text sizes
 Different text sizes are used for different messages \n
 --------------------------------------------------------------------------
 */
int drawTextOnScreen(GameData *gamePtr, char *text, int x, int y, int size) {
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
 @date    Dec 6, 2019
 @mname   DisplayTextAndWaitBegin
 @details
 Returns false if escape key is pressed
 This function displays the first screen that the user views in the game\n
 --------------------------------------------------------------------------
 */
bool drawTextAndWaitBegin(GameData *gamePtr) {

	FENTRY();
	TRACE();
	int next = drawTextOnScreen(gamePtr, (char*) "Welcome to Car Smasher", gamePtr->display.width / 2,
			gamePtr->display.height / 4, largeFont_c);
	al_flush_event_queue(gamePtr->eventqueue);
	drawTextOnScreen(gamePtr, (char*) "(c) mlambiri 2019", gamePtr->display.width / 2, next,
			smallFont_c);


	next = drawTextOnScreen(gamePtr, (char*) "Two Player Mode", gamePtr->display.width / 2,
			gamePtr->display.height / 2, regularFont_c);

	char buffer[100];
	sprintf(buffer, "Most points after %d rounds wins!", gamePtr->maxRounds);
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
 @date    Dec 6, 2019
 @mname   DisplayTextAndWaitBegin
 @details
 Returns false if escape key is pressed
 This function displays a screen when a round or game is won
 The text for the two conditions will be different
 We declare the temporary variable next to position text messages one on top of another
 We do this by adding a value to the y coordinate of the message\n
 --------------------------------------------------------------------------
 */
bool drawTextAndWaitRoundWin(GameData *gamePtr) {

	FENTRY();
	TRACE();
	char textBuffer[MAXBUFFER];
	if ((gamePtr->roundNumber == gamePtr->maxRounds) || (gamePtr->remainingCars == 0)){
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
				gamePtr->carArea.yposition - 3*gamePtr->fontsize, largeFont_c);
		sprintf(textBuffer, "Score: %s %d %s %d", gamePtr->player[1].name, gamePtr->player[1].carsSmashed,
				gamePtr->player[0].name, gamePtr->player[0].carsSmashed);
		next = drawTextOnScreen(gamePtr, textBuffer, gamePtr->display.width / 2, next,
				regularFont_c);

		drawTextOnScreen(gamePtr, (char*) "Press a key to begin or ESC to exit",
				gamePtr->display.width / 2, gamePtr->carArea.yposition+gamePtr->carArea.height, regularFont_c);

		playSound(gamePtr->winsample);
		sprintf(textBuffer, "[Mode: %s] [Score: %s %s]",
				"Human", gamePtr->player[1].name,
				gamePtr->player[0].name);
		recordResult(textBuffer);
		gamePtr->bcolor = gamePtr->initcolor;
		initBrickLayout(gamePtr);
		setBrickInfo(gamePtr);

	} else {
		sprintf(textBuffer, "Score: %s %d %s %d",
				gamePtr->player[1].name, gamePtr->player[1].carsSmashed, gamePtr->player[0].name,
				gamePtr->player[0].carsSmashed);
		int next = drawTextOnScreen(gamePtr, textBuffer, gamePtr->display.width / 2,
				gamePtr->carArea.yposition - gamePtr->fontsize, regularFont_c);
		char buffer[100];
		sprintf(buffer, "Press a key to begin Round %d of %d or ESC to exit", ++gamePtr->roundNumber, gamePtr->maxRounds);
		drawTextOnScreen(gamePtr, buffer, gamePtr->display.width / 2, gamePtr->carArea.yposition+gamePtr->carArea.height, regularFont_c);
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
 @date    Dec 2, 2019
 @mname   displayScore
 @details
 \n
 --------------------------------------------------------------------------
 */
bool displayScore(GameData *gamePtr) {

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
}

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 6, 2019
 @mname   DrawBitmap
 @details
 \n
 --------------------------------------------------------------------------
 */
void  drawBitmap(GameBasicBlock *g) {
	FENTRY();
	TRACE();
	al_draw_bitmap(g->bmap, g->xposition, g->yposition, 0);
	FEXIT();

} // end-of-function DrawBitmap

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 1, 2019
 @mname   DrawBitmapSection
 @details
 Draws only a selected portion of a bitmap.
 It is used to change the length of the pallete depending on the game level.\n
 --------------------------------------------------------------------------
 */
void  drawBitmapSection(GameBasicBlock *g) {
	FENTRY();
	TRACE();
	al_draw_bitmap_region(g->bmap, 0, 0, g->width, g->height, g->xposition,
			g->yposition, 0);
	FEXIT();
} // end-of-function DrawBitmapSection

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 6, 2019
 @mname   DrawObjects
 @details
 This function sets the background color and draws the players and the ball
 Has to be called every time we want to refresh the display during gameplay\n
 --------------------------------------------------------------------------
 */
void  drawObjects(GameData *gamePtr) {

	FENTRY();
	TRACE();
	setBackgroundColor(*(gamePtr->bcolor));
	drawBitmapSection(&(gamePtr->player[0].ge));
	drawBitmapSection(&(gamePtr->player[1].ge));
	drawBitmap(&(gamePtr->ball));
	for (int i = 0; i < gamePtr->maxRows; i++) {
		for (int j = 0; j < gamePtr->maxColumns; j++) {
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
 @date    Dec 1, 2019
 @mname   CheckSideCollision
 @details
 true if there is a collision with top or bottom\n
 --------------------------------------------------------------------------
 */
bool checkCollisionLeftRight(GameData *gamePtr) {

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
 @date    Dec 1, 2019
 @mname   CheckTopBottomCollision
 @details
 Checks if the ball hits either player's side of the field and grants a roundwin\n
 --------------------------------------------------------------------------
 */
bool checkCollisionTopAndBottom(GameData *gamePtr) {

	FENTRY();
	TRACE();
	if ((gamePtr->ball.yposition >= (gamePtr->display.height - gamePtr->ball.height))
			&& (gamePtr->ball.yspeed > 0)) {
		gamePtr->player[1].carsSmashed += gamePtr->penalty;
		gamePtr->roundWinner = &(gamePtr->player[1]);
		FEXIT();
		return true;

	} else if ((gamePtr->ball.yposition <= 0) && (gamePtr->ball.yspeed < 0)) {
		TRACE();
		gamePtr->player[0].carsSmashed += gamePtr->penalty;
		gamePtr->roundWinner = &(gamePtr->player[0]);
		FEXIT();
		return true;
	}FEXIT();
	return false;
} // end-of-function CheckTopBottomCollision

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 1, 2019
 @mname   PlaySound
 @details
 \n
 --------------------------------------------------------------------------
 */
void  playSound(ALLEGRO_SAMPLE *s) {
	FENTRY();
	TRACE();
	if (s)
		al_play_sample(s, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	FEXIT();
} // end-of-function PlaySound

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 2, 2019
 @mname   StopTimers
 @details
 Stops all game timers \n
 --------------------------------------------------------------------------
 */
void  stopTimers(GameData *gamePtr) {

	FENTRY();
	TRACE();
	al_stop_timer(gamePtr->timer);
	FEXIT();

} // end-of-function StopTimers

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 2, 2019
 @mname   StartTimers
 @details
 \n
 --------------------------------------------------------------------------
 */
void  startTimers(GameData *gamePtr) {

	FENTRY();
	TRACE();
	al_start_timer(gamePtr->timer);
	FEXIT();
} // end-of-function StartTimers

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 6, 2019
 @mname   PrintRoundWinner
 @details
 When the round ends, we need to stop the timers from firing unwanted events
 We do that at the beginning of the function
 Prints a message and play a sound
 Then we wait for user input to restart the game\n
 --------------------------------------------------------------------------
 */
bool printRoundWinner(GameData *gamePtr) {

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
 @date    Dec 1, 2019
 @mname   SignOfNumber
 @details
 \n
 --------------------------------------------------------------------------
 */
int signOfNumber(int value) {

	if (value >= 0) {
		return 1;
	} //end-of-if(value > 0)
	return -1;
} // end-of-function SignOfNumber

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 1, 2019
 @mname   PaletteBounceCalc
 @details
 This function changes the direction of the ball after a collision with the
 pallete.
 The pallete height is divided into vone_c zones
 Depending on which zone makes contact the ball will react in a different way
 This is to increase the unpredictability of the game and make it more fun and cBotlenging\n
 --------------------------------------------------------------------------
 */
void  ballBounceOnPlayer(GameBasicBlock *ball, GamePlayer *playerPtr,
		int maxballspeed) {

	FENTRY();
	TRACE();
	int newxspeed = abs(ball->yspeed) + (rand() % (minballspeed_c / 2));
	if (newxspeed > maxballspeed)
		newxspeed = maxballspeed;
	ball->yspeed = signOfNumber(ball->yspeed) * -1 * newxspeed;

	int zones_c = 8 - playerPtr->paddleSize;
	if (playerPtr->paddleSize == maxPaddleSize_c) {
		ball->xspeed += signOfNumber(ball->xspeed)*rand()%2;
	} else {
		int zonelength = playerPtr->ge.width / zones_c;
		int zonenum = (ball->xposition - playerPtr->ge.xposition) / zonelength;
		if (zonenum < 0) {
			zonenum = 0;
		}
		if (zonenum > zones_c - 1) {
			zonenum = zones_c - 1;
		} //end-of-if(zonenum > zones_c -1)

		ball->xspeed += signOfNumber(ball->xspeed)*rand()%2;
	}

	playSound(playerPtr->sample);
	FEXIT();

} // end-of-function PaletteBounceCalc

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 1, 2019
 @mname   checkBallCollisionWithPlayers
 @details
 true if there is a collision false otherwise
 Player one is the *bottom* edge
 Player two it is the *top* edge.

 --------------------------------------------------------------------------
 */
bool checkBallCollisionWithPlayers(GameData *gamePtr) {
	FENTRY();
	TRACE();

	COLLISIONSIDE side;
	// check collision with player 1
	if(areObjectsColliding(&(gamePtr->ball), &(gamePtr->player[0].ge), side)) {
		gamePtr->ball.yposition = gamePtr->player[0].ge.yposition - gamePtr->ball.height;
		ballBounceOnPlayer(&(gamePtr->ball), &(gamePtr->player[0]), gamePtr->maxballspeed);
		gamePtr->turn = &gamePtr->player[0];
		FEXIT();
		return true;
	}
	//check collision with player 2
	else if(areObjectsColliding(&(gamePtr->ball), &(gamePtr->player[1].ge), side)) {
		gamePtr->ball.yposition = gamePtr->player[1].ge.yposition + gamePtr->player[1].ge.height;
		ballBounceOnPlayer(&(gamePtr->ball), &(gamePtr->player[1]), gamePtr->maxballspeed);
		gamePtr->turn = &gamePtr->player[1];
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
 @date    Dec 6, 2019
 @mname   UpdateBallPosition
 @details
 return true if no more gas cars remain to be smashed
 This function checks if there is a collision between the ball and an object
  as well as for collisions with the top and bottom of the 'field'
  Because x and y speeds can be larger than the size of the car
  we can have a 'tunnelling' effect (described as 'bullet through
  paper' at page YYY in the Game Architecture book.\n
 --------------------------------------------------------------------------
 */
bool updateBallPosition(GameData *gamePtr) {
	FENTRY();
	TRACE();

	if(gamePtr->remainingCars == 0) {
		FEXIT();
		return true;
	}


	GameBasicBlock tmpBall = gamePtr->ball;

	tmpBall.xposition = gamePtr->ball.xposition + gamePtr->ball.xspeed;
	tmpBall.yposition = gamePtr->ball.yposition + gamePtr->ball.yspeed;

	if(isBallInRegion(&tmpBall, &(gamePtr->carArea)) == true) {
		if(abs(gamePtr->ball.xspeed) > gamePtr->maxXSpeed) {
			gamePtr->ball.xspeed = signOfNumber(gamePtr->ball.xspeed)*gamePtr->maxXSpeed;
		}
		gamePtr->ball.xprevposition = gamePtr->ball.xposition;
		gamePtr->ball.xposition += gamePtr->ball.xspeed;

		if(abs(gamePtr->ball.yspeed) > gamePtr->maxYSpeed) {
			gamePtr->ball.yspeed += signOfNumber(gamePtr->ball.yspeed)*gamePtr->maxYSpeed;
		}
		gamePtr->ball.yprevposition = gamePtr->ball.yposition;
		gamePtr->ball.yposition += gamePtr->ball.yspeed;


		// this collision detection checks all rows and columns
		// it is simple and effective as it checks all cars one by one
		// if the speed is 'negative' (ie the ball moves from the bottom towards top)
		// then we can start checking from (maxRows, maxColumns)
		// this will detect the collisions faster
		bool done = false;
		if(gamePtr->ball.yspeed <= 0) {
			for (int i = gamePtr->maxRows-1; i >=0; i--) {
				for (int j = 0; j < gamePtr->maxColumns; j++) {
					if(isBallBrickCollision(gamePtr, i, j)) {
						done = true;
						break;
					}
				} //end-of-for
				if(done) break;
			} //end-of-for
		}
		else if(gamePtr->ball.yspeed > 0) {
			// if the speed is 'positive' (ie ball moves from top to bottom)
			// we can to the check from (0,0) to (maxRow, maxColumn)
			for (int i = 0; i < gamePtr->maxRows; i++) {
				for (int j = 0; j < gamePtr->maxColumns; j++) {
					if(isBallBrickCollision(gamePtr, i, j)) {
						done = true;
						break;
					}
				} //end-of-for
				if(done) break;
			} //end-of-for
		}
	}
	else {
		if(abs(gamePtr->ball.xspeed) > gamePtr->maxXSpeed) {
			gamePtr->ball.xspeed = signOfNumber(gamePtr->ball.xspeed)*gamePtr->maxXSpeed;
		}
		if(abs(gamePtr->ball.yspeed) > gamePtr->maxYSpeed) {
			gamePtr->ball.yspeed += signOfNumber(gamePtr->ball.yspeed)*gamePtr->maxYSpeed;
		}
		gamePtr->ball.xprevposition = gamePtr->ball.xposition;
		gamePtr->ball.yprevposition = gamePtr->ball.yposition;
		gamePtr->ball.xposition +=  gamePtr->ball.xspeed;
		gamePtr->ball.yposition += gamePtr->ball.yspeed;
	}


	if (checkBallCollisionWithPlayers(gamePtr) == false) {
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
 @date    Dec 6, 2019
 @mname   GameMainLoop
 @details
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
bool gameMainLoop(GameData *gamePtr) {

	FENTRY();
	TRACE();
	startTimers(gamePtr);

	bool roundwin = false;
	int skipCounter = 0;

	playSound(gamePtr->startsample);
	//We are waiting for an event from on one of the sources that are linked to the event queue
	//The frame-timer, keyboard, mouse
	//This function blocks until an event is received
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
			//check if escape key has been pressed
			if (isKeyPressEvent(gamePtr) == false) {
				//user has ended game
				FEXIT();
				return false;
			}
			//check if we need to update the frame
			else if (gamePtr->ev.type == ALLEGRO_EVENT_TIMER
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
 @date    Dec 6, 2019
 @mname   GameExit
 @details
 This function is called when the game terminates and it destroys all allegro resources
 \n
 --------------------------------------------------------------------------
 */
void  exitGame(GameData *gamePtr) {
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
 @date    Dec 1, 2019
 @mname   CreateGameData
 @details
 This function gets the game config parameters as read from the config file
 In the same format as the parameters passes to the main file
 argv is an array of character pointers and argc is the is number of entries in the array
 Processing is done in the same style as the main command line arguments\n
 --------------------------------------------------------------------------
 */
bool initializeGameData(GameData *p, int argc, char **argv) {

	FENTRY();
	TRACE();

	srand(time(0));

	strcpy(p->player[0].name, "Bus");
	strcpy(p->player[1].name, "LRT");
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

	p->validLayout = false;
	p->penalty = POINTSFORLOSTBALL;

	p->maxRows = MAXBRICKROWS;
	p->maxColumns = MAXBRICKCOLUMNS;

	p->gameNumber = 1;
	p->roundNumber = 1;
	p->bcolor = &(p->bcolorarray[yellow_c]);

	p->scorePointsPerSmash = 1;
	p->player[0].paddleSpeed = initPaddleSpeed_c;
	p->player[1].paddleSpeed = initPaddleSpeed_c;

	//loop that processes the command line arguments.
	//argc is the size of the argument's array and argv is the array itself
	for (int param = 0; param < argc; param++) {
		if (strcmp(argv[param], "screenwidth") == 0) {
			TRACE();
			//display width
			if (++param < argc)
				p->display.width = atoi(argv[param]);
		} else if (strcmp(argv[param], "screenheight") == 0) {
			//display height
			if (++param < argc)
				p->display.height = atoi(argv[param]);
		}else if (strcmp(argv[param], "year") == 0) {
			TRACE();
			//the year of play
			// pre 2000 there are no electric cars
			if (++param < argc) {
				p->year = atoi(argv[param]);
				//printf("Year of play is: %d\n", p->year);
			}
		} else if (strcmp(argv[param], "maxrows") == 0) {
			//the number of rows of cars, max is MAXBRICKROWS
			if (++param < argc) {
				p->maxRows = atoi(argv[param]);
				if(p->maxRows > MAXBRICKROWS)
					p->maxRows = MAXBRICKROWS;
			}
		}else if (strcmp(argv[param], "maxcolumns") == 0) {
			//the number of rows of cars, max is MAXBRICKCOLUMNS
			if (++param < argc) {
				p->maxColumns = atoi(argv[param]);
				if(p->maxColumns > MAXBRICKCOLUMNS)
					p->maxColumns = MAXBRICKCOLUMNS;
			}
		}  else if (strcmp(argv[param], "fontsize") == 0) {
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
		}   else if (strcmp(argv[param], "p1level") == 0) {
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
		} else if (strcmp(argv[param], "penalty") == 0) {
			//display penalty
			if (++param < argc) {
				p->penalty = atoi(argv[param]);
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

	//printf("Year of play is: %d\n", p->year);

	initBrickLayout(p);
	FEXIT();
	return true;
} // end-of-function CreateGameData

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 6, 2019
 @mname   InitGame
 @details
 returns 1 if init ok, 0 otherwise
 This function does the following:
 1. Initializes all allegro resources
 2. Loads all game resources (fonts, bitmaps, sounds)
 --------------------------------------------------------------------------
 */
bool initializeGraphics(GameData *p) {
	FENTRY();
	TRACE();
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

	//TRACE();
	//tries to load font file
	if (loadFont(p, smallFont_c) == false) {
		FEXIT();
		return false;
	} //end-of-if(LoadFont(p, smallFont_c) == false)

	//TRACE();
	if (loadFont(p, regularFont_c) == false) {
		FEXIT();
		return false;
	} //end-of-if(LoadFont(p, regularFont_c) == false)

	//TRACE();
	if (loadFont(p, largeFont_c) == false) {
		FEXIT();
		return false;
	} //end-of-if(LoadFont(p, largeFont_c) == false)

	//TRACE();
	if ((p->display.display = al_create_display(p->display.width,
			p->display.height)) == NULL) {
		ERROR("Cannot init display");FEXIT();
		return false;
	}

	//TRACE();
	p->bcolorarray[yellow_c] = al_map_rgb(255, 255, 0);
	p->bcolorarray[blue_c] = al_map_rgb(200, 200, 255);
	p->bcolorarray[grey_c] = al_map_rgb(180, 180, 180);
	p->bcolorarray[white_c] = al_map_rgb(255, 255, 255);
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

	p->maxXSpeed = p->bricks[0][0].width /4;
	p->maxYSpeed = p->bricks[0][0].height/4;

	setInitialObjectPositions(p);

	setBackgroundColor(*(p->bcolor));
	FEXIT();
	return true;
} // end-of-function InitGame

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 7, 2019
 @mname   GameRun
 @details
 This is the function called from the main function
 1. Displays the initial screen
 2. Calls the game loop
 --------------------------------------------------------------------------
 */
void runGame(GameData *p) {
	FENTRY();
	TRACE();
	setBackgroundColor(*(p->bcolor));
	if (drawTextAndWaitBegin(p) == true) {
		gameMainLoop(p);
	}

	exitGame(p);
	FEXIT();
} // end-of-function GameRun
