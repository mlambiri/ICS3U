/*****************************************************************************
 *	Name:     Lindsay Cullum                                                 *
 *	Date:     April 2018                                                     *
 *                                                                           *
 *	Purpose: Monarchs Template                                               *
 *	                                                                         *
 *	Usage:   Starter code for the Monarchs assignment - using Allegro        *
 *	                                                                         *
 *	Revision History:                                                        *
 *	                                                                         *
 *	Known Issues:                                                            *
 *	                                                                         *
 *****************************************************************************/
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>                   // For allegro, must be in compiler search path.
#include <allegro5/allegro_native_dialog.h> 		// for message box
#include "monarchs.h"

// NOTE: argc, argv parameters are required.
int main(int argc, char *argv[]) {

    //***************ALLEGRO SETUP REQUIREMENTS******************
    //initialize Allegro
    initializeAllegro();

    //declare and initialize display and font, and check they have been setup properly
    ALLEGRO_DISPLAY *display = al_create_display(SCREEN_W, SCREEN_H);
    ALLEGRO_FONT *font = al_load_ttf_font("SF_Cartoonist_Hand.ttf", 18, 0);
    checkSetup(display, font);

    //set window title
	al_set_window_title(display, "Monarchs");

	//******************POPULATE DATABASE*************************
	//create database of monarchs, reads in data from text file.
    Person monarchs[maxMonarchs_c];
    int monarchAges[maxMonarchs_c];

    int number = 0;
    if(readFile(monarchs, monarchAges, number, NULL, display) == 0) {
    	printf("Error reading file\n");
    	//return 22;
    }

    else {
		//*********************PRINTS OUTPUT TO SCREEN*******************
		//Builds screen by printing to the buffer
		al_clear_to_color(BACKGROUND);
		printTitle(font);
		printDatabase(font, monarchs, monarchAges, number);

		//flips the buffer to the screen
		al_flip_display();

		//wait for 20 seconds.
		al_rest(20.0);
    }

    // Free up memory taken by display.
    al_destroy_display(display);

    // Exit with no errors
	return 0;
}
