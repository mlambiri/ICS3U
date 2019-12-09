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

static const char BUSIMAGE[] = "player1.png";
static const char LRTIMAGE[] = "player2.png";
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
void  initBrickLayout(GameData*gptr) {

	FENTRY();
	TRACE();
	gptr->remainingCars = 0;

	/*Checks what year it is before loading the game.
	 * It doesn't check the actual yer, but you can change the year*/

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
			gptr->bricks[i][j].onScreen = (rand() % rNumber)?true:false;
			if(gptr->bricks[i][j].onScreen == true ) {

				// electric cars are 'indestructible'
				// as times goes by more are electric cars
				// but also less cars on road

				totalCount++;
				if(gptr->year >= 2000) {
					if(i == gptr->maxRows - 1) {
						if(topCount-- > 0) {
							gptr->remainingCars++;
							continue;
						}
					}
					gptr->bricks[i][j].indestructible = (totalCount % rNumber)?false:true;
				}
				if(gptr->bricks[i][j].indestructible  == false) {
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
			if(gptr->bricks[i][j].indestructible  == false) {
				// do not allow gas cars to be surrounded by ecars
				// in that case the cars cannot be smashed
				// check if the gas car is surrounded and if it is, make it an ecar
				if((gptr->bricks[i-1][j].indestructible == true)
						&& (gptr->bricks[i][j-1].indestructible == true)
						&& (gptr->bricks[i][j+1].indestructible == true)
						&& (gptr->bricks[i+1][j].indestructible == true)
				){
					gptr->bricks[i][j].indestructible = true;
					if(gptr->bricks[i][j].onScreen == true)
						gptr->remainingCars--;
					else
						gptr->bricks[i][j].onScreen = true;
				}
			}
		} //end-of-for
	} //end-of-for

	gptr->gameNumber++;

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

	p->carArea.speed.x = 0;
	p->carArea.speed.y = 0;
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
			p->bricks[i][j].speed.x = 0;
			p->bricks[i][j].speed.y = 0;
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
 @date    Nov 30, 2019
 @mname   fastBall
 @details
   increase one component of the ball speed
   very useful when we get into an infinite ricochet loop\n
 --------------------------------------------------------------------------
 */
void fastBall(GameData* g) {
	g->maxspeed.x += 2;
	g->maxspeed.y += 2;
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
	g->maxspeed.x -= 2;
	g->maxspeed.y -= 2;
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
	if((x>=b->position.x)
			&& (x <= (b->position.x+b->width))
			&&(y>=b->position.y)
			&& (y<=(b->position.y+b->height))) {
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
	bool result = isPointInObject(obj,ball->position.x,ball->position.y);
	result = result || isPointInObject(obj, ball->position.x+ball->width, ball->position.y);
	result = result || isPointInObject(obj,ball->position.x, ball->position.y+ball->height);
	result = result || isPointInObject(obj, ball->position.x+ball->width, ball->position.y+ball->height);

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
 @date    Dec 3, 2019
 @mname   ballBrickCollision
 @details
   this function returns true if the ball has collided with a brick
   and false otherwise\n
 --------------------------------------------------------------------------
 */
bool isBallBrickCollision(GameData* gptr, int i, int j) {

	FENTRY();
	TRACE();
	if (gptr->bricks[i][j].onScreen == false) {
		FEXIT();
		return false;
	}

	COLLISIONSIDE side;

	if(areObjectsColliding(&(gptr->ball), &(gptr->bricks[i][j]), side)) {

		if(gptr->ball.speed.y == 0) {
			gptr->ball.speed.y = rand() %3 -2;
		}
		if(gptr->ball.speed.x == 0) {
			gptr->ball.speed.x = rand() %3 -2;
		}

		switch(side){
		case top_c:
			gptr->ball.speed.y *= -1;
			gptr->ball.speed.x += rand() % 2;
			gptr->ball.position.y = gptr->bricks[i][j].position.y - gptr->ball.height;
			//printf("top\n");
			break;
		case bottom_c:
			gptr->ball.speed.y *= -1;
			gptr->ball.speed.x += rand() % 2;
			gptr->ball.position.y = gptr->bricks[i][j].position.y + gptr->bricks[i][j].height;
			//printf("bottom\n");
			break;
		case left_c:
			gptr->ball.position.x = gptr->bricks[i][j].position.x - gptr->ball.width;
			gptr->ball.speed.x *= -1;
			//printf("left\n");
			break;
		case right_c:
			gptr->ball.speed.x *= -1;
			gptr->ball.position.x = gptr->bricks[i][j].position.x + gptr->bricks[i][j].width;
			//printf("right\n");
			break;
		default:
			//printf("default\n");
			break;
		}


		if(gptr->bricks[i][j].indestructible == false) {
			gptr->bricks[i][j].onScreen = false;
			gptr->remainingCars--;
			if(gptr->turn)
				gptr->turn->carsSmashed+= gptr->scorePointsPerSmash;
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
	p->ge.width = (al_get_bitmap_width(p->ge.bmap)* (p->paddleSize)) / maxPaddleSize_c;
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
bool loadAudio(GamePlayer *gptr) {
	FENTRY();
	TRACE();
	gptr->sample = al_load_sample(gptr->audioFileName);
	if (gptr->sample == NULL) {
		printf("Audio clip sample %s not loaded!\n", gptr->audioFileName);
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
bool loadAudioWinner(GameData *gptr) {
	FENTRY();
	TRACE();
	gptr->winsample = al_load_sample(gptr->winSoundFile);
	if (gptr->winsample == NULL) {
		printf("Audio clip sample %s not loaded!\n", gptr->winSoundFile);
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
bool loadFont(GameData *gptr, int size) {

	FENTRY();
	TRACE();
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
		printf("Could not load %s.\n", gptr->fontFileName);
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
void  setInitialObjectPositions(GameData *gptr) {

	FENTRY();
	TRACE();
	gptr->ball.speed.y = minballspeed_c
			+ rand() % (gptr->maxballspeed - minballspeed_c);
	if (gptr->roundWinner) {
		gptr->turn = gptr->roundWinner;
		if (gptr->roundWinner == &(gptr->player[bus_c])) {
			gptr->ball.speed.y *= -1;
		} //end-of-if(p->roundWinner == &(p->player[bus_c]))
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

	gptr->player[bus_c].ge.position.x = gptr->display.width / 2 - gptr->player[bus_c].ge.width / 2;
	gptr->player[bus_c].ge.position.y = gptr->display.height - gptr->player[bus_c].ge.height;
	gptr->player[bus_c].ge.speed.x = 0;
	gptr->player[lrt_c].ge.position.x = gptr->display.width / 2 - gptr->player[lrt_c].ge.width / 2;
	gptr->player[lrt_c].ge.position.y = 0;
	gptr->player[lrt_c].ge.speed.x = 0;

	if (gptr->ball.speed.y > 0) {
		gptr->ball.prevposition.y = gptr->ball.position.y;
		gptr->ball.position.y = gptr->player[lrt_c].ge.position.y + gptr->player[lrt_c].ge.height;
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

	int ypos = (gptr->display.height - gptr->bricks[0][0].height*gptr->maxRows)/2;

	for (int i = 0; i < gptr->maxRows; i++) {
		int xpos = (gptr->display.width - gptr->bricks[0][0].width*gptr->maxColumns)/2;
		if(xpos < 0) {
			xpos = 0;
		}

		for (int j = 0; j < gptr->maxColumns; j++) {
			gptr->bricks[i][j].position.y = ypos;
			gptr->bricks[i][j].position.x = xpos;
			xpos += gptr->bricks[i][j].width;
		} //end-of-for
		ypos += gptr->bricks[i][0].height;
	} //end-of-for

	gptr->carArea.position.x = gptr->bricks[0][0].position.x;
	gptr->carArea.position.y = gptr->bricks[0][0].position.y;

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
bool pauseGame(GameData *gptr) {

	//To pause the game we need to stop the timers
	FENTRY();
	TRACE();
	stopTimers(gptr);
	while (true) {
		TRACE();
		//wait for an event
		al_wait_for_event(gptr->eventqueue, &(gptr->ev));
		//check if the event is a key press
		//can be something else as the event queue
		//has other sources
		if (gptr->ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (gptr->ev.keyboard.keycode) {
			case ALLEGRO_KEY_ESCAPE:
				//exit game
				FEXIT();
				return false;
			case ALLEGRO_KEY_P:
				//P was pressed again
				//we want no events in the queue
				//and we want to start the timers
				al_flush_event_queue(gptr->eventqueue);
				startTimers(gptr);
				FEXIT();
				return true;
			}
		}
		//close the display with the mouse
		if (gptr->ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
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
bool isKeyPressEvent(GameData *gptr) {

	FENTRY();
	TRACE();
	if (gptr->ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		switch (gptr->ev.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			gptr->player[bus_c].keyPress[0] = true;
			gptr->player[bus_c].keyPress[1] = false;
			break;
		case ALLEGRO_KEY_RIGHT:
			gptr->player[bus_c].keyPress[0] = false;
			gptr->player[bus_c].keyPress[1] = true;
			break;
		case ALLEGRO_KEY_Q:
			gptr->player[lrt_c].keyPress[0] = true;
			gptr->player[lrt_c].keyPress[1] = false;
			break;
		case ALLEGRO_KEY_E:
			gptr->player[lrt_c].keyPress[0] = false;
			gptr->player[lrt_c].keyPress[1] = true;
			break;
		case ALLEGRO_KEY_P:
			if (pauseGame(gptr) == false) {
				FEXIT();
				return false;
			}
			break;
		case ALLEGRO_KEY_F:
			fastBall(gptr);
			break;
		case ALLEGRO_KEY_S:
			slowBall(gptr);
			break;
		case ALLEGRO_KEY_ESCAPE:
			//exit game
			FEXIT();
			return false;
		}
	} else if (gptr->ev.type == ALLEGRO_EVENT_KEY_UP) {
		switch (gptr->ev.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			gptr->player[bus_c].keyPress[0] = false;
			break;
		case ALLEGRO_KEY_RIGHT:
			gptr->player[bus_c].keyPress[1] = false;
			break;
		case ALLEGRO_KEY_Q:
			gptr->player[lrt_c].keyPress[0] = false;
			break;
		case ALLEGRO_KEY_E:
			gptr->player[lrt_c].keyPress[1] = false;
			break;
		case ALLEGRO_KEY_ESCAPE:
			//exit game
			FEXIT();
			return false;
		}
	} else if (gptr->ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
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
bool pressAnyKeyToBeginGame(GameData *gptr) {

	FENTRY();
	TRACE();
	al_flush_event_queue(gptr->eventqueue);

	while (true) {
		TRACE();
		//wait for an event
		al_wait_for_event(gptr->eventqueue, &(gptr->ev));
		//check if the event is a key press
		//can be something else as the event queue
		//has other sources
		if (gptr->ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			FEXIT();
			//exits either way
			switch (gptr->ev.keyboard.keycode) {
			case ALLEGRO_KEY_ESCAPE:
				//exit game
				return false;
			default:
				return true;
			}
		}
		if (gptr->ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
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
void  movePlayers(GameData *gptr) {

	FENTRY();
	TRACE();

	if (gptr->player[bus_c].keyPress[0] == true) {
		gptr->player[bus_c].ge.position.x -= gptr->player[bus_c].paddleSpeed;
		if (gptr->player[bus_c].ge.position.x < 0)
			gptr->player[bus_c].ge.position.x = 0;
	}
	if (gptr->player[bus_c].keyPress[1] == true) {
		gptr->player[bus_c].ge.position.x += gptr->player[bus_c].paddleSpeed;
		if (gptr->player[bus_c].ge.position.x >= (gptr->display.width - gptr->player[bus_c].ge.width))
			gptr->player[bus_c].ge.position.x = (gptr->display.width - gptr->player[bus_c].ge.width);
	} //end-of-if(p->player[bus_c].keyPress[1] ==true)

	if (gptr->player[lrt_c].keyPress[0] == true) {
		gptr->player[lrt_c].ge.position.x -= gptr->player[lrt_c].paddleSpeed;
		if (gptr->player[lrt_c].ge.position.x < 0)
			gptr->player[lrt_c].ge.position.x = 0;
	} //end-of-if(p->player[lrt_c].keyPress[0] == true)

	if (gptr->player[lrt_c].keyPress[1] == true) {
		gptr->player[lrt_c].ge.position.x += gptr->player[lrt_c].paddleSpeed;
		if (gptr->player[lrt_c].ge.position.x >= (gptr->display.width - gptr->player[lrt_c].ge.width))
			gptr->player[lrt_c].ge.position.x = (gptr->display.width - gptr->player[lrt_c].ge.width);
	} //end-of-if(p->player[lrt_c].keyPress[1] == true)

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
int drawTextOnScreen(GameData *gptr, char *text, int x, int y, int size) {
	FENTRY();
	TRACE();
	al_draw_text(gptr->font[size], gptr->fcolor, x, y, ALLEGRO_ALIGN_CENTRE, text);
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
bool drawTextAndWaitBegin(GameData *gptr) {

	FENTRY();
	TRACE();
	int next = drawTextOnScreen(gptr, (char*) "Welcome to Car Smasher", gptr->display.width / 2,
			gptr->display.height / 4, largeFont_c);
	al_flush_event_queue(gptr->eventqueue);
	drawTextOnScreen(gptr, (char*) "(c) mlambiri 2019", gptr->display.width / 2, next,
			smallFont_c);


	next = drawTextOnScreen(gptr, (char*) "Two Player Mode", gptr->display.width / 2,
			gptr->display.height / 2, regularFont_c);

	char buffer[100];
	sprintf(buffer, "Most points after %d rounds wins!", gptr->maxRounds);
	next = drawTextOnScreen(gptr, buffer, gptr->display.width / 2, next, regularFont_c);
	next = drawTextOnScreen(gptr, (char*) "Press a key to begin", gptr->display.width / 2,
			next, regularFont_c);

	al_flip_display();

	if (pressAnyKeyToBeginGame(gptr) == false) {
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
bool drawTextAndWaitRoundWin(GameData *gptr) {

	FENTRY();
	TRACE();
	char textBuffer[MAXBUFFER];
	if ((gptr->roundNumber == gptr->maxRounds) || (gptr->remainingCars == 0)){
		gptr->roundNumber = 1;
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
		int next = drawTextOnScreen(gptr, textBuffer, gptr->display.width / 2,
				gptr->carArea.position.y - 3*gptr->fontsize, largeFont_c);
		sprintf(textBuffer, "Score: %s %d %s %d", gptr->player[lrt_c].name, gptr->player[lrt_c].carsSmashed,
				gptr->player[bus_c].name, gptr->player[bus_c].carsSmashed);
		next = drawTextOnScreen(gptr, textBuffer, gptr->display.width / 2, next,
				regularFont_c);

		drawTextOnScreen(gptr, (char*) "Press a key to begin or ESC to exit",
				gptr->display.width / 2, gptr->carArea.position.y+gptr->carArea.height, regularFont_c);

		playSound(gptr->winsample);
		sprintf(textBuffer, "[Mode: %s] [Score: %s %s]",
				"Human", gptr->player[lrt_c].name,
				gptr->player[bus_c].name);
		recordResult(textBuffer);
		gptr->bcolor = gptr->initcolor;
		initBrickLayout(gptr);
		setBrickInfo(gptr);

	} else {
		sprintf(textBuffer, "Score: %s %d %s %d",
				gptr->player[lrt_c].name, gptr->player[lrt_c].carsSmashed, gptr->player[bus_c].name,
				gptr->player[bus_c].carsSmashed);
		int next = drawTextOnScreen(gptr, textBuffer, gptr->display.width / 2,
				gptr->carArea.position.y - gptr->fontsize, regularFont_c);
		char buffer[100];
		sprintf(buffer, "Press a key to begin Round %d of %d or ESC to exit", ++gptr->roundNumber, gptr->maxRounds);
		drawTextOnScreen(gptr, buffer, gptr->display.width / 2, gptr->carArea.position.y+gptr->carArea.height, regularFont_c);
		//DEBUG(" =======\n");
	}

	al_flip_display();

	if (pressAnyKeyToBeginGame(gptr) == false) {
		FEXIT();
		return false;
	}

	for (int i = 0; i < 2; i++) {
		gptr->player[bus_c].keyPress[i] = false;
		gptr->player[lrt_c].keyPress[i] = false;
	} //end-of-for
	al_flush_event_queue(gptr->eventqueue);

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
bool displayScore(GameData *gptr) {

	FENTRY();
	TRACE();
	char textBuffer[MAXBUFFER];
	sprintf(textBuffer, "Score: %s %d %s %d",
			gptr->player[lrt_c].name, gptr->player[lrt_c].carsSmashed, gptr->player[bus_c].name,
			gptr->player[bus_c].carsSmashed);
	int next = drawTextOnScreen(gptr, textBuffer, gptr->display.width -100,
			30, smallFont_c);
	sprintf(textBuffer, "Remain: %d",
			gptr->remainingCars);
	next = drawTextOnScreen(gptr, textBuffer, gptr->display.width -100,
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
	al_draw_bitmap(g->bmap, g->position.x, g->position.y, 0);
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
	al_draw_bitmap_region(g->bmap, 0, 0, g->width, g->height, g->position.x,
			g->position.y, 0);
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
void  drawObjects(GameData *gptr) {

	FENTRY();
	TRACE();
	setBackgroundColor(*(gptr->bcolor));
	drawBitmapSection(&(gptr->player[bus_c].ge));
	drawBitmapSection(&(gptr->player[lrt_c].ge));
	drawBitmap(&(gptr->ball));
	for (int i = 0; i < gptr->maxRows; i++) {
		for (int j = 0; j < gptr->maxColumns; j++) {
			if (gptr->bricks[i][j].onScreen == true) {
				drawBitmap(&(gptr->bricks[i][j]));
			}//end-of-if
		} //end-of-for
	} //end-of-for
	displayScore(gptr);
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
bool checkCollisionLeftRight(GameData *gptr) {

	FENTRY();
	TRACE();
	if (gptr->ball.position.x > (gptr->display.width - gptr->ball.width)) {
		gptr->ball.position.x = gptr->display.width - gptr->ball.width;
		if (gptr->ball.speed.x > 0)
			gptr->ball.speed.x *= -1;
		FEXIT();
		return true;
	} else if (gptr->ball.position.x < 0) {
		gptr->ball.position.x = 0;
		if (gptr->ball.speed.x < 0)
			gptr->ball.speed.x *= -1;
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
bool checkCollisionTopAndBottom(GameData *gptr) {

	FENTRY();
	TRACE();
	if ((gptr->ball.position.y >= (gptr->display.height - gptr->ball.height))
			&& (gptr->ball.speed.y > 0)) {
		DEBUG("I hit the bottom line\n");
		gptr->player[lrt_c].carsSmashed += gptr->penalty;
		gptr->roundWinner = &(gptr->player[lrt_c]);
		FEXIT();
		return true;

	} else if ((gptr->ball.position.y <= 0) && (gptr->ball.speed.y < 0)) {
		DEBUG("I hit the top line\n");
		gptr->player[bus_c].carsSmashed += gptr->penalty;
		gptr->roundWinner = &(gptr->player[bus_c]);
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
void  stopTimers(GameData *gptr) {

	FENTRY();
	TRACE();
	al_stop_timer(gptr->timer);
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
void  startTimers(GameData *gptr) {

	FENTRY();
	TRACE();
	al_start_timer(gptr->timer);
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
bool printRoundWinner(GameData *gptr) {

	FENTRY();
	TRACE();
	stopTimers(gptr);
	setInitialObjectPositions(gptr);
	drawObjects(gptr);

	if (drawTextAndWaitRoundWin(gptr) == false) {
		FEXIT();
		return false;
	} else {
		startTimers(gptr);
		playSound(gptr->startsample);
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
	int newspeedx = abs(ball->speed.y) + (rand() % (minballspeed_c / 2));
	if (newspeedx > maxballspeed)
		newspeedx = maxballspeed;
	ball->speed.y = signOfNumber(ball->speed.y) * -1 * newspeedx;

	int zones_c = 8 - playerPtr->paddleSize;
	if (playerPtr->paddleSize == maxPaddleSize_c) {
		ball->speed.x += signOfNumber(ball->speed.x)*rand()%2;
	} else {
		int zonelength = playerPtr->ge.width / zones_c;
		int zonenum = (ball->position.x - playerPtr->ge.position.x) / zonelength;
		if (zonenum < 0) {
			zonenum = 0;
		}
		if (zonenum > zones_c - 1) {
			zonenum = zones_c - 1;
		} //end-of-if(zonenum > zones_c -1)

		ball->speed.x += signOfNumber(ball->speed.x)*rand()%2;
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
bool checkBallCollisionWithPlayers(GameData *gptr) {
	FENTRY();
	TRACE();

	COLLISIONSIDE side;
	// check collision with player 1
	if(areObjectsColliding(&(gptr->ball), &(gptr->player[bus_c].ge), side)) {
		gptr->ball.position.y = gptr->player[bus_c].ge.position.y - gptr->ball.height;
		ballBounceOnPlayer(&(gptr->ball), &(gptr->player[bus_c]), gptr->maxballspeed);
		gptr->turn = &gptr->player[bus_c];
		FEXIT();
		return true;
	}
	//check collision with player 2
	else if(areObjectsColliding(&(gptr->ball), &(gptr->player[lrt_c].ge), side)) {
		gptr->ball.position.y = gptr->player[lrt_c].ge.position.y + gptr->player[lrt_c].ge.height;
		ballBounceOnPlayer(&(gptr->ball), &(gptr->player[lrt_c]), gptr->maxballspeed);
		gptr->turn = &gptr->player[lrt_c];
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
bool updateBallPosition(GameData *gptr) {
	FENTRY();
	TRACE();

	if(gptr->remainingCars == 0) {
		FEXIT();
		return true;
	}


	GameBasicBlock tmpBall = gptr->ball;

	tmpBall.position.x = gptr->ball.position.x + gptr->ball.speed.x;
	tmpBall.position.y = gptr->ball.position.y + gptr->ball.speed.y;

	if(isBallInRegion(&tmpBall, &(gptr->carArea)) == true) {
		if(abs(gptr->ball.speed.x) > gptr->maxspeed.x) {
			gptr->ball.speed.x = signOfNumber(gptr->ball.speed.x)*gptr->maxspeed.x;
		}
		gptr->ball.prevposition.x = gptr->ball.position.x;
		gptr->ball.position.x += gptr->ball.speed.x;

		if(abs(gptr->ball.speed.y) > gptr->maxspeed.y) {
			gptr->ball.speed.y += signOfNumber(gptr->ball.speed.y)*gptr->maxspeed.y;
		}
		gptr->ball.prevposition.y = gptr->ball.position.y;
		gptr->ball.position.y += gptr->ball.speed.y;


		// this collision detection checks all rows and columns
		// it is simple and effective as it checks all cars one by one
		// if the speed is 'negative' (ie the ball moves from the bottom towards top)
		// then we can start checking from (maxRows, maxColumns)
		// this will detect the collisions faster
		bool done = false;
		if(gptr->ball.speed.y <= 0) {
			for (int i = gptr->maxRows-1; i >=0; i--) {
				for (int j = 0; j < gptr->maxColumns; j++) {
					if(isBallBrickCollision(gptr, i, j)) {
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
					if(isBallBrickCollision(gptr, i, j)) {
						done = true;
						break;
					}
				} //end-of-for
				if(done) break;
			} //end-of-for
		}
	}
	else {
		if(abs(gptr->ball.speed.x) > gptr->maxspeed.x) {
			gptr->ball.speed.x = signOfNumber(gptr->ball.speed.x)*gptr->maxspeed.x;
		}
		if(abs(gptr->ball.speed.y) > gptr->maxspeed.y) {
			gptr->ball.speed.y += signOfNumber(gptr->ball.speed.y)*gptr->maxspeed.y;
		}
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
	}
	checkCollisionLeftRight(gptr);
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
bool gameMainLoop(GameData *gptr) {

	FENTRY();
	TRACE();
	startTimers(gptr);

	bool roundwin = false;
	int skipCounter = 0;

	playSound(gptr->startsample);
	//We are waiting for an event from on one of the sources that are linked to the event queue
	//The frame-timer, keyboard, mouse
	//This function blocks until an event is received
	//Therefore if the timers would not be started, this function would return only on a keyboard or mouse event
	while (true) {
		al_wait_for_event(gptr->eventqueue, &(gptr->ev));

		if (gptr->ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			FEXIT();
			return false;
		}
		//If the round is won we need to stop the game for 1 second
		//We do this by counting timer events without processing them which in effect
		//skips frames
		if (roundwin == true) {
			if (gptr->ev.type == ALLEGRO_EVENT_TIMER
					&& gptr->ev.timer.source == gptr->timer) {
				//skip maxSkip frames
				//At the end of each round we want to keep the last frame of the play that shows where the ball exitied the screen
				//for a little longer, so the user can see who won the round
				//We do this by counting frame timer events
				if (skipCounter++ >= (int) gptr->fps) {
					skipCounter = 0;
					roundwin = false;
					if (printRoundWinner(gptr) == false) {
						//user has pressed ESC to end the game
						FEXIT();
						return false;
					}
				}

			} else
				continue;
		} else {
			//check if escape key has been pressed
			if (isKeyPressEvent(gptr) == false) {
				//user has ended game
				FEXIT();
				return false;
			}
			//check if we need to update the frame
			else if (gptr->ev.type == ALLEGRO_EVENT_TIMER
					&& gptr->ev.timer.source == gptr->timer) {
				//If this is a screen update timer event then we have to redraw the screen
				//we have to update the ball position and then draw all objects (players and ball)
				//Calculates next position of the paddles based on the key inputs read above
				movePlayers(gptr);
				roundwin = updateBallPosition(gptr);
				drawObjects(gptr);
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
void  exitGame(GameData *gptr) {
	FENTRY();
	TRACE();
	al_rest(0.0);
	al_destroy_display(gptr->display.display);
	al_destroy_timer(gptr->botTimer);
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

	strcpy(p->player[bus_c].name, "Bus");
	strcpy(p->player[lrt_c].name, "LRT");
	strcpy(p->fontFileName, FONTNAME);
	strcpy(p->player[bus_c].audioFileName, P1SOUND);
	strcpy(p->player[lrt_c].audioFileName, P2SOUND);
	strcpy(p->busBitmapName, BUSIMAGE);
	strcpy(p->lrtBitmapName, LRTIMAGE);
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
	p->player[bus_c].paddleSpeed = initPaddleSpeed_c;
	p->player[lrt_c].paddleSpeed = initPaddleSpeed_c;

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
		}  else if (strcmp(argv[param], "maxrounds") == 0) {
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
		}   else if (strcmp(argv[param], "buslength") == 0) {
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

	if (loadPlayerBitmap(&(p->player[bus_c]), p->busBitmapName) == false) {
		FEXIT();
		return false;
	}
	if (loadPlayerBitmap(&(p->player[lrt_c]), p->lrtBitmapName) == false) {
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

	loadAudio(&(p->player[bus_c]));
	loadAudio(&(p->player[lrt_c]));
	loadAudioWinner(p);

	p->maxspeed.x = p->bricks[0][0].width /4;
	p->maxspeed.y = p->bricks[0][0].height/4;

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
