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

/*
 * @author   mlambiri
 * @date     Nov. 23, 2019
 *   I have added the highlight parameter to the function
 *   this way we can display some lines in a different color
 *   this is very useful for searching in the database
 *   and displaying results
 *
 *   Also we now have 5 columns:
 *   - first is the line number, this is used to delete entries
 *   - last is the age of the monarch
 */


void printDatabase(ALLEGRO_FONT *font, Person p[], bool highlight[], int counter){

    //creates the print string to be fed into al_draw_text()

	/*
	 * @author   mlambiri
	 * @date     Nov. 23, 2019
	 *  I replaced all 200 numbers with bufferSize_c (which is defined as 200)
	 *
	 */

    char printString[bufferSize_c] = "";

    for (int i = 0; i < counter; i++){

    	/*
		 * @author   mlambiri
		 * @date     Nov. 23, 2019
		 *   the foreground color used depends if he line is highlighted or not
		 *   for regular text I use FOREGROUND
		 *   for highlighted text I use BLUE
		 *   this is very helpful for searching on the database
		 *
		 */

    	ALLEGRO_COLOR fcolor = (highlight[i]?BLUE:FOREGROUND);

        sprintf(printString, "%d", i+1);
        /*
		 * @author   mlambiri
		 * @date     Nov. 23, 2019
		 *   i changed the distance between prints to 50+i*fontSize_c
		 *   this prints the lines 1 'fontSize_c' apart.
		 *   the previous fixed value was too large and did not
		 *   allow all entries in the db to be printed on screen
		 *
		 *   all al_draw_text lines were changed
		 */

        al_draw_text(font, fcolor, COL_1, 50 + i*fontSize_c, ALLEGRO_ALIGN_LEFT, printString);

        //prints name to the buffer
        sprintf(printString, "%s %s", p[i].name, p[i].regnal);
        al_draw_text(font, fcolor, COL_2, 50 + i*fontSize_c, ALLEGRO_ALIGN_LEFT, printString);

        //prints birth day to the buffer
        sprintf(printString, "%d %s %d", p[i].birth.year, p[i].birth.month, p[i].birth.day);
        al_draw_text(font, fcolor, COL_3, 50 + i*fontSize_c, ALLEGRO_ALIGN_LEFT, printString);

        //prints death day to the buffer
        sprintf(printString, "%d %s %d", p[i].death.year, p[i].death.month, p[i].death.day);
        al_draw_text(font, fcolor, COL_4, 50 + i*fontSize_c, ALLEGRO_ALIGN_LEFT, printString);

        //prints age to the buffer
        sprintf(printString, "%d", calculateAge(&(p[i])));
        al_draw_text(font, fcolor, COL_5, 50 + i*fontSize_c, ALLEGRO_ALIGN_LEFT, printString);

    }

}

/**prints the title to the top of the screen*/


/*
 * @author   mlambiri
 * @date     Nov. 23, 2019
 *
 *   We now have 5 columns:
 *   - first is the line number, this is used to delete entries
 *   - last is the age of the monarch
 */

void printTitle(ALLEGRO_FONT *font){

    char title[200] = "Index";
    //print Monarch title
    al_draw_text(font, FOREGROUND, COL_1, 5, ALLEGRO_ALIGN_LEFT, title);

    strcpy(title, "Monarch");
    al_draw_text(font, FOREGROUND, COL_2, 5, ALLEGRO_ALIGN_LEFT, title);

    strcpy(title, "Birth Date");
    //print Monarch title
    al_draw_text(font, FOREGROUND, COL_3, 5, ALLEGRO_ALIGN_LEFT, title);

    strcpy(title, "Death Date");
    //print Monarch title
    al_draw_text(font, FOREGROUND, COL_4, 5, ALLEGRO_ALIGN_LEFT, title);

    strcpy(title, "Age");
    //print Monarch title
    al_draw_text(font, FOREGROUND, COL_5, 5, ALLEGRO_ALIGN_LEFT, title);

}

