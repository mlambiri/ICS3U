#ifndef _MONARCH_H_
#define _MONARCH_H_

const int SCREEN_W = 640;       // screen width
const int SCREEN_H = 480;       // screen height

const int COL_1 = 5;    //tabs for chart layout
const int COL_2 = 180;
const int COL_3 = 400;
const int COL_4 = 620;

const int maxMonarchs_c = 20;
const int maxName_c = 14;
const int maxMonth_c = 14;
const int maxRegnal_c = 6;

//#define BACKGROUND al_map_rgb(0x09, 0x31, 0x45)
#define BACKGROUND al_map_rgb(0xff, 0xff, 0xff)
#define FOREGROUND al_map_rgb(0x3C, 0x64, 0x78)

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
int readFile(Person p[], int monarchAges[], int &counter, char* fileName, ALLEGRO_DISPLAY *display);
void printDatabase(ALLEGRO_FONT *font, Person p[], int age[], int counter);

#endif
