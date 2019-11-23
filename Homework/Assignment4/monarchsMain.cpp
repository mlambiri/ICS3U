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
    ALLEGRO_FONT *font = al_load_ttf_font("SF_Cartoonist_Hand.ttf", fontSize_c, 0);
    checkSetup(display, font);

    //set window title
	al_set_window_title(display, "Monarchs");

	//******************POPULATE DATABASE*************************
	//create database of monarchs, reads in data from text file.
    Person monarchs[maxMonarchs_c];
    int monarchAges[maxMonarchs_c];
    bool highlight[maxMonarchs_c];

    int number = 0;
    if(readFile(monarchs, number, NULL, display) == 0) {
    	printf("Error reading file\n");
    	//return 22;
    }

    else {
		//*********************PRINTS OUTPUT TO SCREEN*******************
		//Builds screen by printing to the buffer
    	int opType = -1;
    	bool runLoop = true;

		for (int i = 0; i < maxMonarchs_c; i++ ) {
			highlight[i] = false;
		} //end-of-for

		while(runLoop) {
			al_clear_to_color(BACKGROUND);
			printTitle(font);
			printDatabase(font, monarchs, highlight,  number);
			//flips the buffer to the screen
			al_flip_display();

			for (int i = 0; i < maxMonarchs_c; i++ ) {
				highlight[i] = false;
			} //end-of-for
			printf("Database operations:\n");
			printf("[1] Add monarch\n");
			printf("[2] Delete monarch\n");
			printf("[3] Find monarch\n");
			printf("[4] Sort increasing by name\n");
			printf("[5] Sort decreasing by name\n");
			printf("[6] Sort increasing by age\n");
			printf("[7] Sort decreasing by age\n");
			printf("[8] Sort increasing regnal\n");
			printf("[9] Sort decreasing regnal\n");
			printf("[22] Exit\n");

			scanf("%d", &opType);
			switch(opType) {
			case 1:
				break;
			case 2:
				break;
			case 3:
				// find clarence for now
				for (int i = 0; i < number; i++ ) {
					if(strcicmp(monarchs[i].name, "clarence") == 0) {
						highlight[i] = true;
					}
				} //end-of-for
				break;
			case 4:
				//sort the file
				qsort(monarchs, number, sizeof(Person), monarchNameCompareIncreasing);
				writeFile(monarchs, number, NULL, display);
				break;
			case 5:
				//sort the file
				qsort(monarchs, number, sizeof(Person), monarchNameCompareDecreasing);
				writeFile(monarchs, number, NULL, display);
				break;
			case 6:
				//sort the file
				qsort(monarchs, number, sizeof(Person), ageCompareIncreasing);
				writeFile(monarchs, number, NULL, display);
				break;
			case 7:
				//sort the file
				qsort(monarchs, number, sizeof(Person), ageCompareDecreasing);
				writeFile(monarchs, number, NULL, display);
				break;
			case 8:
				//sort the file
				qsort(monarchs, number, sizeof(Person), regnalCompareIncreasing);
				writeFile(monarchs, number, NULL, display);
				break;
			case 9:
				//sort the file
				qsort(monarchs, number, sizeof(Person), regnalCompareDecreasing);
				writeFile(monarchs, number, NULL, display);
				break;
			case 22:
				runLoop = false;
				break;
			default:
				break;
			}
		}
    }

    // Free up memory taken by display.
    al_destroy_display(display);

    // Exit with no errors
	return 0;
}
