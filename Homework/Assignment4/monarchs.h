#ifndef _MONARCH_H_
#define _MONARCH_H_

const int SCREEN_W = 640;       // screen width
const int SCREEN_H = 480;       // screen height

const int COL_1 = 5;    //tabs for chart layout
const int COL_2 = 180;
const int COL_3 = 400;
const int COL_4 = 600;

const int maxMonarchs_c = 20;
const int maxName_c = 14;
const int maxMonth_c = 14;
const int maxRegnal_c = 6;

const int fontSize_c = 18;

//#define BACKGROUND al_map_rgb(0x09, 0x31, 0x45)
#define BACKGROUND al_map_rgb(0xff, 0xff, 0xff)
#define FOREGROUND al_map_rgb(0x3C, 0x64, 0x78)
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
int readFile(Person p[],  int &counter, char* fileName, ALLEGRO_DISPLAY *display);
int writeFile(Person p[], int &counter, char* fileName, ALLEGRO_DISPLAY *display);
void printDatabase(ALLEGRO_FONT *font, Person p[], bool highlight[],  int counter);
int monarchCompareIncreasing(const void * a, const void * b);
int monarchNameCompareDecreasing(const void * a, const void * b);
int ageCompareIncreasing(const void * a, const void * b);
int ageCompareDecreasing(const void * a, const void * b);
int regnalCompareIncreasing(const void * a, const void * b);
int regnalCompareDecreasing(const void * a, const void * b);
int calculateAge(const Person* p);
int strcicmp(char const *a, char const *b);

#endif
