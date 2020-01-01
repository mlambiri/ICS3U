/*
 * car-breaker-graphics.h
 *
 *  Created on: Dec 1, 2019
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


#define MAXNAME 200
#define MAXBUFFER 200
#define MAXFONTS 3
#define MAXBITMAPS 2

#define MAXBRICKROWS 10
#define MAXBRICKCOLUMNS 30

#define SCREEN_W  1600
#define SCREEN_H  1200
#define MINFPS    5
#define MAXFPS    240
#define FRAMERATE  60
#define PLAYERSPEED 30.0
#define FONTSIZE    24
#define MAXROUNDS    10

#define POINTSFORLOSTBALL 5

enum PLAYERS {
	bus_c = 0, lrt_c = 1
};

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

/*
 * @author   mlambiri
 * @date     Dec. 2, 2019
 *   stores x, y coordinates
 */

typedef struct Point {
	int x;
	int y;
} Point;


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
     onScreen is true if the object is displayed and false if it is not
     indestructible is true if the object cannot be removed by ball collision
     and false otherwise.
    \n
  ---------------------------------------------------------------------------
 */
typedef struct GameBasicBlock {
	bool onScreen;
	bool indestructible;
	Point position;
	Point prevposition;
	int width;
	int height;
	Point speed;
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
	uint games;
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

#define MAXRECORDING 3000

typedef struct DataRecorder {
	Point  point[MAXRECORDING];
	int used;
	bool rec;
	bool separateDisplay;
} DataRecorder;

typedef struct BounceStatistics {
	uint bounceUntilSmash[MAXBRICKCOLUMNS*MAXBRICKROWS];
	uint totalBounce;
	uint firstEmpty;
} BounceStatistics;

#define screenHeightZones_c  4

/*
 * @author   mlambiri
 * @date     Dec 1, 2019
 *  The display is split in zones obtained by dividing display.height
 *  by one of the heightDivisor values
 *  A larger divisor denotes a zone closes to the player
 *
 *  The divisors should be in increasing order
 *  as they are checked starting with the smallest
 *  array index
 *
 *  The speed multiplier will enhance or decrease the
 *  mobility of the player
 *
 *  Setting the multiplier to zero for a zone will make
 *  the player immobile when the ball is in that zone
 *
 *  If the multipliers are larger the player moves more
 *  when the ball is in that zone
 *
 */
typedef struct BotControlInfo {
	//first array represents where in the field Bot will start to move
	int heightDivisor[screenHeightZones_c];
	//This array is a multiplier to determine how much Bot should move
	//setting an entry to zero will prevent Bot from moving when
	//the ball is in the corresponding region
	float speedMultiplier[screenHeightZones_c];
} BotControlInfo;

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
	uint maxRounds;
	char fontFileName[MAXNAME];
	char winSoundFile[MAXNAME];
	float  fps;

	ALLEGRO_EVENT ev;
	ALLEGRO_EVENT_QUEUE *eventqueue;
	ALLEGRO_TIMER *timer;
	ALLEGRO_FONT *font[MAXFONTS];
	ALLEGRO_COLOR bcolorarray[maxColors_c];
	ALLEGRO_COLOR* backgroundColor;
	ALLEGRO_COLOR fontColor;
	ALLEGRO_SAMPLE *startsample;
	ALLEGRO_SAMPLE *winsample;

	GameBasicBlock bricks[MAXBRICKROWS][MAXBRICKCOLUMNS];
	uint gameNumber;
	GamePlayer* turn;
	uint roundNumber;
	uint remainingCars;
	ALLEGRO_COLOR* initcolor;
	uint scorePointsPerSmash;

	char busBitmapName[MAXNAME];
	char lrtBitmapName[MAXNAME];
	char ballBitmapName[MAXNAME];
	char gasBitmapName[MAXNAME];
	char ecarBitmapName[MAXNAME];

	ALLEGRO_BITMAP* gasBitmap;
	ALLEGRO_BITMAP* ecarBitmap;

	int year;

	char layout[MAXBRICKROWS][MAXBRICKCOLUMNS];
	bool validLayout;
	int penalty;

	char inLayout[MAXNAME];
	char outLayout[MAXNAME];

	int maxRows;
	int maxColumns;
	GameBasicBlock carArea;
	bool cAlgoSelector;
	Point maxspeed;
	GameDisplay trajectoryDisplay;
	DataRecorder path;
	int botLevel[2];
	BotControlInfo *botControl[2];
	bool helpOn;
	BounceStatistics stats;
	bool gameStart;
	bool gamePaused;
	bool roundWin;
	bool gameWin;

} GameData;


#define INITGBB {true, true, {0, 0}, {0,0}, 0, 0, {0, 0}, NULL}
#define INITPLAYER {  0, 0, INITGBB, {0}, {0}, NULL, {false, false}, 0}
#define INITDISPLAY {SCREEN_W, SCREEN_H, NULL}

//======= FUNCTION DECLARATIONS =====
// === Initialization ====
bool initializeGameData(GameData *p, int argc, char **argv);
bool loadAudio(GamePlayer *gptr);
bool loadBitmap(GameBasicBlock *g, char* fname);
bool loadFont(GameData *gptr, int size);
bool loadPlayerImage(GamePlayer *p, char* fname);
bool loadWinnerSound(GameData *gptr);
bool readCarLayoutFromFile(GameData* g, char* fileName);

// === Game Graphics ======
bool displayHelp(GameData *gptr);
bool displayScore(GameData *gptr);
bool drawBeginGameScreen(GameData *gptr);
bool roundWinOverlay(GameData *gptr);
bool initializeGraphics(GameData *p);
bool printRoundWinner(GameData *gptr);
bool setBitmap(GameBasicBlock *g, ALLEGRO_BITMAP*);
int   drawTextOnScreen(GameData *gptr, char *text, int x, int y, int size);
void createTrajectoryDisplay(GameData* g);
void drawBitmap(GameBasicBlock *g);
void drawBitmapSection(GameBasicBlock *g);
void drawBitmapSection(GameBasicBlock *g);
void drawObjects(GameData *gptr);
void flipAllDisplays(GameData* g);
void initializeCarLayout(GameData*gptr);
void setCarInfo(GameData* p);
void setInitialObjectPositions(GameData *gptr);
void writeTrajectoryToWindow(GameData* g);
void graphicsCleanup(GameData *gptr);


//===== Ball Movement and Collisions ('Physics') ===========
bool areObjectsColliding(GameBasicBlock* ball, GameBasicBlock* obj, COLLISIONSIDE& side);
bool checkBallCollisionWithPlayers(GameData *gptr);
bool checkCollisionLeftRight(GameData *gptr);
bool checkCollisionTopAndBottom(GameData *gptr);
bool isBallBrickCollisionPossible(GameData* gptr, GameBasicBlock* tmpBall, int i, int j);
bool isBallInRegion(GameBasicBlock* ball, GameBasicBlock* obj);
bool isPointInAnyCar(GameData* g,  int x, int y, int&row, int&column);
bool isPointInObject(GameBasicBlock* b, int x, int y);
bool updateBallPosition(GameData *gptr);
bool whenCollisionOccurs(GameData* gptr, int i, int j) ;
int   signOfNumber(int value);
void ballBounceOnPlayer(GameBasicBlock *ball, GamePlayer *playerPtr, int);
void ballSpeedLimiter(GameData* gptr);
void decreaseBallSpeed(GameData* g);
void increaseBallSpeed(GameData* g);

// ==== Game Control ========
bool checkKeyboardAndMouse(GameData *gptr);
bool recordPoint(DataRecorder* r, Point* p);
bool recordResult(char *p, BounceStatistics* stats);
bool writeCarLayoutToFile(GameData* g);
void botControl(GameData *gptr, uint botNumber);
void userControl(GameData *gptr);
void playSound(ALLEGRO_SAMPLE *s);
void setCarInfo(GameData* p);
void setPointsPerSmash(GameData*gptr) ;
void startTimers(GameData *gptr);
void stopTimers(GameData *gptr);

// === Game Loop ====
void gameLoop(GameData *p, bool startTimer);

//============================


#endif /* CARBREAKERGRAPHICS_H_ */
