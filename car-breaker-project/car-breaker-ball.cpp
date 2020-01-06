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
 @date    Dec 17, 2019
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
 @date    Dec 17, 2019
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
 @date    Dec 13, 2019
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
 @date    Dec 13, 2019
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
 @date    Dec 9, 2019
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
 @date    Dec 13, 2019
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
 @date    Dec 13, 2019
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
 @date    Dec 13, 2019
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
 @date    Dec 14, 2019
 @mname   areObjectsColliding
 @details
  check if the ball is colliding with another objects
  the side parameter will return the side of the collision
  this side refers to the object not the ball\n
 --------------------------------------------------------------------------
 */
bool areObjectsColliding(GameBasicBlock* ball, GameBasicBlock* obj, COLLISIONSIDE& side){
	FENTRY();

	if(isBallInRegion(ball, obj) == false) {
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
 @date    Dec 15, 2019
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
 @date    Dec 13, 2019
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
 @date    Dec 15, 2019
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
 @date    Dec 13, 2019
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
   @date    Dec 12, 2019
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
   @date    Dec 12, 2019
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
 @date    Dec 12, 2019
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


