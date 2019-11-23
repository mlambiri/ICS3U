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

	char errorText[200];
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
    	int found = 0;
    	int index = -1;
		char sname[bufferSize_c];
		char buffer[bufferSize_c];

		for (int i = 0; i < number; i++ ) {
			highlight[i] = false;
		} //end-of-for



		/*
		 * @author   mlambiri
		 * @date     Nov. 23, 2019
		 *   the program runs in a loop until the user enters '22'
		 *   at the begining of the loop the program displays the information
		 *   it then asks the user for an operation and does the operation
		 */

		while(runLoop) {
			al_clear_to_color(BACKGROUND);
			printTitle(font);
			printDatabase(font, monarchs, highlight,  number);
			//flips the buffer to the screen
			al_flip_display();

			if(opType == 3) {
				if(found == 0) {
					sprintf(errorText, "Cannot find '%s' in database.", sname);
					al_show_native_message_box(display, "Error", "Error", errorText,
							nullptr, ALLEGRO_MESSAGEBOX_ERROR);
				}
				else {
					sprintf(errorText, "Found %d monarchs names '%s' in database.", found, sname);
					al_show_native_message_box(display, "INFO", "INFO", errorText,
							nullptr, ALLEGRO_MESSAGEBOX_WARN);
				}
			}

			// clear the highlighting

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
			printf(">");

			fgets(buffer, bufferSize_c, stdin);

			if( sscanf(buffer, "%d", &opType) != 1) {
				// cannot decode the input
				sprintf(errorText, "Invalid input: '%s'", buffer);
				al_show_native_message_box(display, "Error", "Error", errorText,
						nullptr, ALLEGRO_MESSAGEBOX_ERROR);
				opType = -1;
				continue;
			}
			switch(opType) {
			case 1:
				if(number >= maxMonarchs_c) {
					// database is full
					// cannot add
					// display an error message and continue
					sprintf(errorText, "Database is full. Max entries: %d", maxMonarchs_c);
					al_show_native_message_box(display, "Error", "Error", errorText,
							nullptr, ALLEGRO_MESSAGEBOX_ERROR);
				}
				else {
					// add a new entry
					Person* p = &(monarchs[number]);
					printf("Enter monarch name: ");
					fgets(buffer, bufferSize_c, stdin);
					if(sscanf(buffer, "%s", p->name) != 1) {
						// cannot decode the input
						sprintf(errorText, "Invalid name: '%s'", buffer);
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}
					printf("Enter regnal: ");
					fgets(buffer, bufferSize_c, stdin);
					if(sscanf(buffer, "%s", p->regnal) != 1) {
						// cannot decode the input
						sprintf(errorText, "Invalid regnal: '%s'", buffer);
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}
					if( regnalToNumeral(p) == -1) {
						//error
						sprintf(errorText, "Invalid regnal: '%s'", buffer);
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}
					printf("Enter birth year: ");
					fgets(buffer, bufferSize_c, stdin);
					if(sscanf(buffer, "%d", &(p->birth.year)) != 1) {
						// cannot decode the input
						sprintf(errorText, "Invalid birth year: '%s'", buffer);
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}
					printf("Enter birth month: ");
					fgets(buffer, bufferSize_c, stdin);
					if(sscanf(buffer, "%s", p->birth.month) != 1) {
						// cannot decode the input
						sprintf(errorText, "Invalid birth month: '%s'", buffer);
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}
					printf("Enter birth day: ");
					fgets(buffer, bufferSize_c, stdin);
					if(sscanf(buffer, "%d", &(p->birth.day)) != 1) {
						// cannot decode the input
						sprintf(errorText, "Invalid birth day: '%s'", buffer);
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}

					if( checkDate(&(p->birth)) == false) {
						//error
						sprintf(errorText, "Invalid birth date");
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}

					printf("Enter death year: ");
					fgets(buffer, bufferSize_c, stdin);
					if(sscanf(buffer, "%d", &(p->death.year)) != 1) {
						// cannot decode the input
						sprintf(errorText, "Invalid death year: '%s'", buffer);
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}
					printf("Enter death month: ");
					fgets(buffer, bufferSize_c, stdin);
					if(sscanf(buffer, "%s", p->death.month) != 1) {
						// cannot decode the input
						sprintf(errorText, "Invalid death month: '%s'", buffer);
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}
					printf("Enter death day: ");
					fgets(buffer, bufferSize_c, stdin);
					if(sscanf(buffer, "%d", &(p->death.day)) != 1) {
						// cannot decode the input
						sprintf(errorText, "Invalid death day: '%s'", buffer);
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}

					if( checkDate(&(p->death)) == false) {
						//error
						sprintf(errorText, "Invalid death date");
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}
					if( causalityCheck(p) == false) {
						//error
						sprintf(errorText, "Causal issue with birth/death dates");
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}
					number++;
					writeFile(monarchs, number, NULL, display);
				}
				break;
			case 2:
				if(number == 0) {
					// database is empty
					// cannot delete
					// display an error message and continue
					sprintf(errorText, "Database is empty");
					al_show_native_message_box(display, "Error", "Error", errorText,
							nullptr, ALLEGRO_MESSAGEBOX_ERROR);
				}
				else {
					// delete an entry
					printf("Enter line number to delete: ");
					fgets(buffer, bufferSize_c, stdin);
					if(sscanf(buffer, "%d", &index) != 1) {
						// cannot decode the input
						sprintf(errorText, "Invalid line: '%s'", buffer);
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}
					// the program displays "lines"
					// we need to substract one for the array index
					index--;
					if(index < 0 || index >= number) {
						// cannot decode the input
						sprintf(errorText, "Invalid line: %d", index+1);
						al_show_native_message_box(display, "Error", "Error", errorText,
								nullptr, ALLEGRO_MESSAGEBOX_ERROR);
						break;
					}
					else {
						// delete entry by moving all entries after 1 up
						for (int i = index; i < number; i++ ) {
							Person* d = &(monarchs[i]);
							Person* s = &(monarchs[i+1]);

							strcpy(d->name, s->name);
							strcpy(d->regnal, s->regnal);
							d->birth.year = s->birth.year;
							d->birth.day = s->birth.day;
							strcpy(d->birth.month, s->birth.month);
							d->death.year = s->death.year;
							d->death.day = s->death.day;
							strcpy(d->death.month, s->death.month);
						} //end-of-for
						number--;
						writeFile(monarchs, number, NULL, display);
					}
				}
				break;
			case 3:
				found = 0;
				printf("Enter monarch name: ");
				fgets(buffer, bufferSize_c, stdin);
				if( sscanf(buffer, "%s", sname) != 1) {
					// cannot decode the input
					sprintf(errorText, "Invalid input: '%s'", buffer);
					al_show_native_message_box(display, "Error", "Error", errorText,
							nullptr, ALLEGRO_MESSAGEBOX_ERROR);
					break;
				}
				for (int i = 0; i < number; i++ ) {
					if(strcicmp(monarchs[i].name, sname) == 0) {
						highlight[i] = true;
						found++;
					}
				} //end-of-for
				break;
			case 4:
				/*
				 * @author   mlambiri
				 * @date     Nov. 23, 2019
				 *   all sort functionality uses 'quicksort'
				 *   an explanation of the function use is here:
				 *      http://www.cplusplus.com/reference/cstdlib/qsort/?kw=qsort
				 *   a description of the 'quicksort' algorithm is here:
				 *      https://en.wikipedia.org/wiki/Quicksort
				 *   to make this work I need to provide a compare function for
				 *   each type of sort I want to do.
				 *   the last parameter in the qsort call is that function
				 *   I have created compare functions for name & regnal, regnal alone and age.
				 *   Depending on the compare we can sort 'increasing' or 'decreasing'
				 *   All name comparisons are case insensitive. I found it much easier to
				 *   deal with the compares. Otherwise 'Maud' and 'maud' would be different names
				 *
				 *   Accordying to
				 */

				qsort(monarchs, number, sizeof(Person), monarchNameCompareIncreasing);
				//writeFile(monarchs, number, NULL, display);
				break;
			case 5:
				//sort the file
				qsort(monarchs, number, sizeof(Person), monarchNameCompareDecreasing);
				//writeFile(monarchs, number, NULL, display);
				break;
			case 6:
				//sort the file
				qsort(monarchs, number, sizeof(Person), ageCompareIncreasing);
				//writeFile(monarchs, number, NULL, display);
				break;
			case 7:
				//sort the file
				qsort(monarchs, number, sizeof(Person), ageCompareDecreasing);
				//writeFile(monarchs, number, NULL, display);
				break;
			case 8:
				//sort the file
				qsort(monarchs, number, sizeof(Person), regnalCompareIncreasing);
				//writeFile(monarchs, number, NULL, display);
				break;
			case 9:
				//sort the file
				qsort(monarchs, number, sizeof(Person), regnalCompareDecreasing);
				//writeFile(monarchs, number, NULL, display);
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
