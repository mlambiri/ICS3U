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

//==== Game Graphics ======
/*
 * @author   mlambiri
 * @date     Dec. 30, 2019
 *  This section contains functions that draw items on screen
 */

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 3, 2019
 @mname   loadPlayerBitmap
 @details
  This function loads a bitmap for the player from a file\n
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
 @date    Dec 3, 2019
 @mname   loadBitmap
 @details
 Loads bitmaps for the non-player objects
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
 @date    Dec 20, 2019
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
 @date    Dec 23, 2019
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
 @date    Dec 29, 2019
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
 @date    Dec 9, 2019
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
   @date    Dec. 27, 2019
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
   @date    Dec. 28, 2019
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
   @date    Dec. 28, 2019
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
 @date    Dec 5, 2019
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
 @date    Dec 5, 2019
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
 @date    Dec 30, 2019
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
 @date   Dec 11, 2019
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
 @date    Dec 3, 2019
 @mname   drawBeginGameScreen
 @details
 Returns false if escape key is pressed
 This function displays the first screen that the user views in the game\n
 --------------------------------------------------------------------------
 */
void drawBeginGameScreen(GameData *gptr) {
	FENTRY();


	int next = drawTextToScreen(gptr, (char*) "Welcome to Car Breaker", gptr->display.width / 2,
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
 @date    Dec 12, 2019
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
 @date    Dec 9, 2019
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
 @date    Dec 21, 2019
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
 @date    Dec 29, 2019
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
	FEXIT();
}//end-of-drawHelpToScreen

/**
 ---------------------------------------------------------------------------
 @author  mlambiri
 @date    Dec 6, 2019
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
 @date    Dec 8, 2019
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
 @date    Dec 2, 2019
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
 @date    Dec 3, 2019
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

