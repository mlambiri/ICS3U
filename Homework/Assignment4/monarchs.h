/*
 * @author   mlambiri
 * @date     Nov. 23, 2019
 *   the file needs compilation guards
 *   otherwise if we include the file twice
 *   we will get compile errors
 *
 */

#ifndef _MONARCH_H_
#define _MONARCH_H_

const int SCREEN_W = 640;       // screen width
const int SCREEN_H = 480;       // screen height

/*
 * @author   mlambiri
 * @date     Nov. 23, 2019
 *   I have added 2 new columns to the display
 *   first column will now display the line number
 *   last column will display the age
 */

const int COL_1 = 5;    //tabs for chart layout
const int COL_2 = 50;
const int COL_3 = 180;
const int COL_4 = 400;
const int COL_5 = 600;

const int maxMonarchs_c = 20;
const int maxName_c = 14;
const int maxMonth_c = 14;
const int maxRegnal_c = 6;

/*
 * @author   mlambiri
 * @date     Nov. 23, 2019
 *  i made the font smaller so that we can print all
 *  database entries on screen
 *  the previous one was too large
 */

const int fontSize_c = 18;

const int bufferSize_c = 200;

//#define BACKGROUND al_map_rgb(0x09, 0x31, 0x45)
#define BACKGROUND al_map_rgb(0xff, 0xff, 0xff)
#define FOREGROUND al_map_rgb(0x3C, 0x64, 0x78)
/*
 * @author   mlambiri
 * @date     Nov. 23, 2019
 *  I've added the blue color
 */

#define BLUE al_map_rgb(0, 0, 255)

//define structs here
struct Date {
    int year;
    char month[14];
    int day;
};

struct Person {
    char name[14];
    char regnal[6];
    Date birth, death;
};

void initializeAllegro();
int checkSetup(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font);
void printTitle(ALLEGRO_FONT *font);
void printDatabase(ALLEGRO_FONT *font, Person p[], bool highlight[],  int counter);

/*
 * @author   mlambiri
 * @date     Nov. 23, 2019
 *  these are the functions i defined for this assignment
 */

/*
 * @author   mlambiri
 * @date     Nov. 23, 2019
 *   I have added an extra parameter to the readFile and writeFile
 *   the dipslay parameter is useful for creating error dialog boxes
 *   using allegro
 *   there are many possible errors that need to be displayed to the
 *   user. all use the dialogs
 */
int readFile(Person p[],  int &counter, char* fileName, ALLEGRO_DISPLAY *display);
int writeFile(Person p[], int &counter, char* fileName, ALLEGRO_DISPLAY *display);

int calculateAge(const Person* p);
int regnalToNumeral(const Person* p);
bool checkDate(Date* d);
bool causalityCheck(Person* p);
int monthToInt(const Date* d);

/*
 * @author   mlambiri
 * @date     Nov. 23, 2019
 *  these compare functions are given to the qsort function to perform
 *  various types of sorts
 */

int monarchCompareIncreasing(const void * a, const void * b);
int monarchNameCompareDecreasing(const void * a, const void * b);
int ageCompareIncreasing(const void * a, const void * b);
int ageCompareDecreasing(const void * a, const void * b);
int regnalCompareIncreasing(const void * a, const void * b);
int regnalCompareDecreasing(const void * a, const void * b);

#endif
