#include <stdio.h>
#include <ctype.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>                       // For allegro, must be in compiler search path.
#include <allegro5/allegro_native_dialog.h> 		// for message box
#include "monarchs.h"


static char configName[] = "MonarchsData.txt";

/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 22, 2019
   @mname   checkRegnal
   @details
     check if the regnal string is an actual roman numeral
     do not accept other types of strings
     check each letter in the string and see if it is
     one of 'I', 'V', 'X', 'M', 'C' both upper and lower case
	  \n
  --------------------------------------------------------------------------
 */
bool checkRegnal(Person* p) {
	char* c = p->regnal;

	bool r = true;

	while(*c) {
		switch (*c++) {
		case 'X':
		case 'x':
		case 'i':
		case 'I':
		case 'M':
		case 'm':
		case 'C':
		case 'c':
		case 'V':
		case 'v':
			break;;
		default:
			return false;
		}
	}

	return r;
}

// this function is copied from here
//https://stackoverflow.com/questions/5820810/case-insensitive-string-comp-in-c#5820991
int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 22, 2019
   @mname   checkDate
   @details
     transform month into a number
     return zero if invalid month string
	  \n
  --------------------------------------------------------------------------
 */
int monthToInt(Date* d) {
	int month = 0;

	if(strcicmp(d->month, "january") == 0) {
		month = 1;
	}
	else 	if(strcicmp(d->month, "january") == 0) {
		month = 1;
	}
	else 	if(strcicmp(d->month, "february") == 0) {
		month = 2;
	}
	else 	if(strcicmp(d->month, "march") == 0) {
		month = 3;
	}
	else 	if(strcicmp(d->month, "april") == 0) {
		month = 4;
	}
	else 	if(strcicmp(d->month, "may") == 0) {
		month = 5;
	}
	else 	if(strcicmp(d->month, "june") == 0) {
		month = 6;
	}
	else 	if(strcicmp(d->month, "july") == 0) {
		month = 7;
	}
	else 	if(strcicmp(d->month, "august") == 0) {
		month = 8;
	}
	else 	if(strcicmp(d->month, "september") == 0) {
		month = 9;
	}
	else 	if(strcicmp(d->month, "october") == 0) {
		month = 10;
	}
	else 	if(strcicmp(d->month, "november") == 0) {
		month = 11;
	}
	else 	if(strcicmp(d->month, "december") == 0) {
		month = 12;
	}

	return month;
}

/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 22, 2019
   @mname   checkDate
   @details
     check if the date is actually valid
	  \n
  --------------------------------------------------------------------------
 */
bool checkDate(Date* d) {

	if(d->year <= 0) return false;
	if(d->day <= 0 || d->day >= 32) return false;

	int month = monthToInt(d);
	int daysInMonth[] = {
			31,28,31,30,31,30,31,31,30,31,30,31
	};

	if (month == 0) {
		return false;
	}

	if(d->day > daysInMonth[month-1]) {
		return false;
	}

	return true;
}

/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 22, 2019
   @mname   calculate age
   @details

	  \n
  --------------------------------------------------------------------------
 */
int calculateAge(Person* p) {

	int age = p->death.year - p->birth.year;
	if(monthToInt(&(p->death)) < monthToInt(&(p->birth))) {
		age--;
	}

	if(monthToInt(&(p->death)) == monthToInt(&(p->birth))
		&& p->death.day < p->birth.day	) {
		age--;
	}

	return age;
}

/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 20, 2019
   @mname   readFile
   @details
    This function reads in the data from the textfile and saves it to the database
    Parameters: p[] is the array of Persons that will be populated by this function.
    counter is the number of monarchs in the array.
    Returns a 0 if all good, 1 if there is an error reading the text file.
	  \n
  --------------------------------------------------------------------------
 */
int readFile(Person p[], int monarchAges[], int &counter, char* fileName, ALLEGRO_DISPLAY *display) {

	const int bufferSize_c = 200;
	char text[bufferSize_c];
	char errorText[bufferSize_c];
	counter = 0;
	char* tempName = fileName;

	FILE* fptr = NULL;
	if (fileName == NULL) {
		tempName = configName;
		fptr = fopen(tempName, "r");
	} else {
		fptr = fopen(tempName, "r");
		if (fptr == NULL) {
			tempName = configName;
			fptr = fopen( tempName, "r");
		} //end-of-if(fptr == NULL)
	}

	char* buffer = text;
	const char* delim = "\t\r\n =";

	if(fptr == NULL){
		//if file is not found return 0
		sprintf(errorText, "File %s not found", tempName);
    	al_show_native_message_box(display, "Error", "Error", errorText,
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}

	//i am reading in a buffer
	while(fgets(buffer, bufferSize_c, fptr)) {
		 //increment the counter
		 if(counter >= maxMonarchs_c) {
			 // I can store only maxMonarch_c lines in the array
			 // if the file has more line I need to stop reading
			sprintf(errorText, "Cannot store more than %d monarchs", counter);
		    al_show_native_message_box(display, "Warning", "Warning", errorText,
		                                 nullptr, ALLEGRO_MESSAGEBOX_WARN);
		    return 1;
		 }

		// printf ("%s\n",buffer);
		 char * pch = strtok (buffer, delim);
		 if (pch != NULL) {
			 //printf ("%s\n",pch);
			 strncpy(p[counter].name, pch, maxName_c);
			 //make sure we terminate the name with a zero!!
			 //the text file might contain looooooong names
			 p[counter].name[maxName_c - 1] = 0;
		 } //end-of-if(pch == NULL)
		 else {
			 // error
				sprintf(errorText, "Read error in line %d", counter+1);
		    	al_show_native_message_box(display, "Error", "Error", errorText,
		                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }
		 pch = strtok (NULL, delim);
		 if (pch != NULL) {
			 //printf ("%s\n",pch);
			 strncpy(p[counter].regnal, pch, maxRegnal_c);
			 //make sure we terminate the name with a zero!!
			 //the text file might contain looooooong names
			 p[counter].name[maxRegnal_c-1] = 0;
			 if(checkRegnal(&(p[counter])) == false) {
				 //error
				sprintf(errorText, "Invalid Regnal in line %d", counter+1);
			    al_show_native_message_box(display, "Error", "Error", errorText,
			                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
				 return 0;
			 }
		 } //end-of-if(pch == NULL)
		 else {
			 //error
			sprintf(errorText, "Read error in line %d", counter+1);
		    al_show_native_message_box(display, "Error", "Error", errorText,
		                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }
		 pch = strtok (NULL, delim);
		 if (pch != NULL) {
			 //printf ("%s\n",pch);
			  sscanf(pch, "%d", &(p[counter].birth.year));
		 } //end-of-if(pch == NULL)
		 else {
			 //error
			sprintf(errorText, "Read error in line %d", counter+1);
		    al_show_native_message_box(display, "Error", "Error", errorText,
		                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }
		 pch = strtok (NULL, delim);
		 if (pch != NULL) {
			 //printf ("%s\n",pch);
			 strncpy(p[counter].birth.month, pch, maxMonth_c);
			 //make sure we terminate the name with a zero!!
			 //the text file might contain looooooong names
			 p[counter].birth.month[maxMonth_c - 1] = 0;
		 } //end-of-if(pch == NULL)
		 else {
			 //error
			sprintf(errorText, "Read error in line %d", counter+1);
		    al_show_native_message_box(display, "Error", "Error", errorText,
		                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }
		 pch = strtok (NULL, delim);
		 if (pch != NULL) {
			 //printf ("%s\n",pch);
			  sscanf(pch, "%d", &(p[counter].birth.day));
		 } //end-of-if(pch == NULL)
		 else {
			 //error
			sprintf(errorText, "Read error in line %d", counter+1);
		    al_show_native_message_box(display, "Error", "Error", errorText,
		                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }

		 if(checkDate(&(p[counter].birth)) == false) {
			 //error
			sprintf(errorText, "Birthday format error in line %d", counter+1);
		    al_show_native_message_box(display, "Error", "Error", errorText,
		                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }

		 pch = strtok (NULL, delim);
		 if (pch != NULL) {
			 //printf ("%s\n",pch);
			 if(strcmp(pch, "to")!=0) {
				 //error
				sprintf(errorText, "Missing 'to' word in line %d", counter+1);
			    al_show_native_message_box(display, "Error", "Error", errorText,
			                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
				 return 0;
			 }
		 } //end-of-if(pch == NULL)
		 else {
			 //error
			sprintf(errorText, "Read error in line %d", counter+1);
			al_show_native_message_box(display, "Error", "Error", errorText,
			                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }
		 pch = strtok (NULL, delim);
		 if (pch != NULL) {
			 //printf ("%s\n",pch);
			  sscanf(pch, "%d", &(p[counter].death.year));
		 } //end-of-if(pch == NULL)
		 else {
			 //error
			sprintf(errorText, "Read error in line %d", counter+1);
			al_show_native_message_box(display, "Error", "Error", errorText,
			                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }
		 pch = strtok (NULL, delim);
		 if (pch != NULL) {
			 //printf ("%s\n",pch);
			 strncpy(p[counter].death.month, pch, maxMonth_c);
			 //make sure we terminate the name with a zero!!
			 //the text file might contain looooooong names
			 p[counter].death.month[maxMonth_c -1] = 0;
		 } //end-of-if(pch == NULL)
		 else {
			 //error
			sprintf(errorText, "Read error in line %d", counter+1);
			al_show_native_message_box(display, "Error", "Error", errorText,
			                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }
		 pch = strtok (NULL, delim);
		 if (pch != NULL) {
			 //printf ("%s\n",pch);
			  sscanf(pch, "%d", &(p[counter].death.day));
		 } //end-of-if(pch == NULL)
		 else {
			 //error
			 sprintf(errorText, "Read error in line %d", counter+1);
			 al_show_native_message_box(display, "Error", "Error", errorText,
			                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }

		 if(checkDate(&(p[counter].death)) == false) {
			 //error
			sprintf(errorText, "Death date format error in line %d", counter+1);
		    al_show_native_message_box(display, "Error", "Error", errorText,
		                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }

		 if(p[counter].birth.year > p[counter].death.year) {
			 //error
			sprintf(errorText, "Causal issue with dates in line %d", counter+1);
		    al_show_native_message_box(display, "Error", "Error", errorText,
		                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }

		 if((p[counter].birth.year == p[counter].death.year)
				&& (monthToInt(&(p[counter].birth)) > monthToInt(&(p[counter].death)))) {
			 //error
			sprintf(errorText, "Causal issue with dates in line %d", counter+1);
		    al_show_native_message_box(display, "Error", "Error", errorText,
		                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }

		 if((p[counter].birth.year == p[counter].death.year)
				&& (monthToInt(&(p[counter].birth)) == monthToInt(&(p[counter].death)))
				&& (p[counter].birth.day > p[counter].death.day)) {
			 //error
			sprintf(errorText, "Causal issue with dates in line %d", counter+1);
		    al_show_native_message_box(display, "Error", "Error", errorText,
		                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			 return 0;
		 }

		 monarchAges[counter] = calculateAge(&(p[counter]));

		 counter++;

	} //end-of-while(fgets(buffer, bufferSize_c, fptr))
	fclose(fptr);
	return 1;
} // end-of-function readFile


/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 20, 2019
   @mname   recordResult
   @detail
  --------------------------------------------------------------------------
 */
int
writeFile(Person p[], int &counter, char* fileName) {

	FILE* fptr;
	fptr = fopen(fileName, "w");
	if(fptr == NULL){
		return 0;
	}

	// write operation goes here
	fclose(fptr);

	return 1;
} // end-of-function recordResult



