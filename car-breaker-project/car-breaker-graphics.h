/*
 * car-breaker-graphics.h
 *
 *  Created on: Nov 17, 2019
 *      Author: mlambiri
 */

#ifndef CARBREAKERGRAPHICS_H_
#define CARBREAKERGRAPHICS_H_

#include <allegro5/allegro.h>
#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro5.h>
#include <allegro5/color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


typedef unsigned int uint;


bool initializeGraphics();
void runGame();
bool initializeGameData(int argc, char **argv);

#define MAXNAME 200
#define MAXBUFFER 200
#define MAXFONTS 3
#define MAXBITMAPS 2

#define MAXBRICKROWS 3
#define MAXBRICKCOLUMNS 20

#define SCREEN_W  1600
#define SCREEN_H  1200
#define MINFPS    5
#define MAXFPS    240
#define FRAMERATE  60
#define PLAYERSPEED 30.0
#define FONTSIZE    24
#define MAXROUNDS    10

enum FONTSIZES {
    smallFont_c = 0, regularFont_c = 1, largeFont_c = 2
};
enum COLOURS {
    yellow_c = 0, blue_c = 1, white_c = 2, green_c = 3, grey_c = 4, maxColors_c
};
enum BOTPLAYINGABILITY {
    novice_c = 0, intermediate_c, expert_c, pro_c
};

enum SCREENCOLORCHANGE {
	level0_c = 50, level1_c = 40, level2_c = 30, level4_c = 20, level5_c = 10, level6_c=1
};

enum GAMEMODE {
	human_c = 0, arcade_c = 1, fullbot_c = 2
};

enum COLLISIONSIDE {
	top_c, bottom_c, left_c, right_c
};

//======= GAME STRUCTURE DEFINITIONS =========

/**
  ---------------------------------------------------------------------------
   @author     mlambiri
   @date       Nov 17, 2019
   @name       GameBasicBlock
   @details
     The GameBasicBlock structure is used for the information and
     logic of game entities (player, ball, cars, trees).
     Allegro 5 uses a system of coordinates that has (0,0) in the top left corner of the screen
     x values increase from left to right and y values increase from top to bottom
     A GameBasicBlock location on the screen is defined by the position of that bitmap's top left corner
     xposition represents the distance from the edge of the screen of the top left corner of the bitmap
     yposition represents the distance from the top of screen to the top left corner of the bitmap.
     xspeed represents the number of pixels that the game entitiy will move on the x axis during one frame.
     yspeed represents the number of pixels the game entity moves up and down in one refresh of the screen
     The players have an xspeed of 0 (cannot move left to right).
     The width is the number of pixels on the x dimension of the bitmap
     The height is the number of pixels in the y dimension of the bitmap
    \n
  ---------------------------------------------------------------------------
 */
typedef struct GameBasicBlock {
    bool onScreen;
    bool indestructible;
    int xposition;
    int yposition;
    int width;
    int height;
    int xspeed;
    int yspeed;
    ALLEGRO_BITMAP* bmap;
}GameBasicBlock;

/**
  ---------------------------------------------------------------------------
   @author     mlambiri
   @date       Nov 17, 2019
   @name       GamePlayer
   @details     contains all player information
        Player is a GameBasicBlock that has several extra variables
        Scores and sounds that are player when a the ball hits a pallete
        Each player may have their own sound\n
  ---------------------------------------------------------------------------
 */
typedef struct GamePlayer {
    uint score;
    uint games;
    uint totalpoints;
    uint carsSmashed;
    GameBasicBlock ge;
    char name[MAXNAME];
    char audioFileName[MAXNAME];
    ALLEGRO_SAMPLE *sample;
    bool keyPress[2];
    int paddleSpeed;
    int paddleSize;
}GamePlayer;

/**
  ---------------------------------------------------------------------------
   @author     mlambiri
   @date       Nov 17, 2019
   @name       GameDisplay
   @details     Contains Display information, height width, and pointer to
   the allegro display
    \n
  ---------------------------------------------------------------------------
 */
typedef struct GameDisplay {
    int width;
    int height;
    ALLEGRO_DISPLAY *display;
} GameDisplay;


/**
  ---------------------------------------------------------------------------
   @author     mlambiri
   @date       Nov 13, 2019
   @name       GameData
   @details
    This is the main data structure
    It aggregates all the required variables that make the game run \n
  ---------------------------------------------------------------------------
 */
typedef struct GameData {
    GamePlayer player[2];
    GameBasicBlock   ball;
    GameDisplay display;
    GAMEMODE   gameMode;
    int    maxballspeed;
    GamePlayer* roundWinner;
    int    fontsize;
    uint maxscore;
    char fontFileName[MAXNAME];
    char winSoundFile[MAXNAME];
    float  fps;

    ALLEGRO_EVENT ev;
    ALLEGRO_EVENT_QUEUE *eventqueue;
    ALLEGRO_TIMER *timer;
    ALLEGRO_TIMER *botTimer;
    ALLEGRO_FONT *font[MAXFONTS];
    ALLEGRO_COLOR bcolorarray[maxColors_c];
    ALLEGRO_COLOR* bcolor;
    ALLEGRO_COLOR fcolor;
    ALLEGRO_SAMPLE *startsample;
    ALLEGRO_SAMPLE *winsample;

    GameBasicBlock bricks[MAXBRICKROWS][MAXBRICKCOLUMNS];
    uint gameNumber;
    GamePlayer* turn;
    uint roundNumber;
    uint remainingCars;
    ALLEGRO_COLOR* initcolor;
    uint scorePointsPerSmash;

    char p1BitmapName[MAXNAME];
    char p2BitmapName[MAXNAME];
    char ballBitmapName[MAXNAME];
    char gasBitmapName[MAXNAME];
    char ecarBitmapName[MAXNAME];

    ALLEGRO_BITMAP* gasBitmap;
    ALLEGRO_BITMAP* ecarBitmap;

    int year;

} GameData;


#define INITGBB {true, true, 0, 0, 0, 0, 0, 0, NULL}
#define INITPLAYER { 0, 0, 0, 0, INITGBB, {0}, {0}, NULL, {false, false}, 0}
#define INITDISPLAY {SCREEN_W, SCREEN_H, NULL}


#endif /* CARBREAKERGRAPHICS_H_ */
