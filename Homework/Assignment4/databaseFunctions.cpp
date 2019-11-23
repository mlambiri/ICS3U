#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>                       // For allegro, must be in compiler search path.
#include <allegro5/allegro_native_dialog.h> 		// for message box
#include "monarchs.h"

/***This function prints the contents of the database to the screen using allegro
Parameters: p[] is the database
counter is the number of entries in the database*/
void printDatabase(ALLEGRO_FONT *font, Person p[], bool highlight[], int counter){

    //creates the print string to be fed into al_draw_text()
    char printString[200] = "";

    for (int i = 0; i < counter; i++){

    	ALLEGRO_COLOR fcolor = (highlight[i]?BLUE:FOREGROUND);
        //prints name to the buffer
        sprintf(printString, "%s %s", p[i].name, p[i].regnal);
        al_draw_text(font, fcolor, COL_1, 50 + i*fontSize_c, ALLEGRO_ALIGN_LEFT, printString);

        //prints birth day to the buffer
        sprintf(printString, "%d %s %d", p[i].birth.year, p[i].birth.month, p[i].birth.day);
        al_draw_text(font, fcolor, COL_2, 50 + i*fontSize_c, ALLEGRO_ALIGN_LEFT, printString);

        //prints death day to the buffer
        sprintf(printString, "%d %s %d", p[i].death.year, p[i].death.month, p[i].death.day);
        al_draw_text(font, fcolor, COL_3, 50 + i*fontSize_c, ALLEGRO_ALIGN_LEFT, printString);

        //prints age to the buffer
        sprintf(printString, "%d", calculateAge(&(p[i])));
        al_draw_text(font, fcolor, COL_4, 50 + i*fontSize_c, ALLEGRO_ALIGN_LEFT, printString);

    }

}

/**prints the title to the top of the screen*/
void printTitle(ALLEGRO_FONT *font){

    char title[200] = "Monarch";



    //print Monarch title
    al_draw_text(font, FOREGROUND, COL_1, 5, ALLEGRO_ALIGN_LEFT, title);

    strcpy(title, "Birth Date");
    //print Monarch title
    al_draw_text(font, FOREGROUND, COL_2, 5, ALLEGRO_ALIGN_LEFT, title);

    strcpy(title, "Death Date");
    //print Monarch title
    al_draw_text(font, FOREGROUND, COL_3, 5, ALLEGRO_ALIGN_LEFT, title);

    strcpy(title, "Age");
    //print Monarch title
    al_draw_text(font, FOREGROUND, COL_4, 5, ALLEGRO_ALIGN_LEFT, title);

}

