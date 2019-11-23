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
   @mname   regnalToNumeral
   @details
    transform regnal to numeral. see the link below
     https://helloacm.com/how-to-convert-roman-to-integer-in-cc/
	  \n
  --------------------------------------------------------------------------
 */
int regnalToNumeral(const Person* p) {
	const char* c = p->regnal;

	int r = 0;
	int prev =0;
	while(*c) {
		int value = 0;
		switch (*c++) {
		case 'X':
		case 'x':
			value = 10;
			break;
		case 'i':
		case 'I':
			value = 1;
			break;
		case 'M':
		case 'm':
			value = 1000;
			break;
		case 'C':
		case 'c':
			value = 100;
			break;
		case 'V':
		case 'v':
			value = 5;
			break;
		case 'L':
		case 'l':
			value = 50;
			break;
		case 'd':
		case 'D':
			value = 500;
			break;
		default:
			return -1;
		}

		r += value;
		if(prev < value) {
			r -= 2*prev;
		}
		prev = value;
	}

	if(r< 0) r = 0;
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
int monthToInt(const Date* d) {
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
int calculateAge(const Person* p) {

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
   @date    Nov 22, 2019
   @mname   monarchNameCompareIncreasing
   @details
      compare names and then regnals
	  \n
  --------------------------------------------------------------------------
 */
int monarchNameCompareIncreasing(const void * a, const void * b){

	const Person* ap = (Person*)a;
	const Person* bp = (Person*)b;

	if ( strcicmp(ap->name, bp->name) < 0) return -1;
	if( strcicmp(ap->name, bp->name) > 0) return 1;

	if(strcicmp(ap->name, bp->name) == 0) {
		// need to compare regnal
		int ar = regnalToNumeral(ap);
		int br = regnalToNumeral(bp);
		if(ar < br) return -1;
		if(ar > br) return 1;
		return 0;
	}

	return 0;
}

/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 22, 2019
   @mname   monarchNameCompareDecreasing
   @details
      compare names and then regnals
	  \n
  --------------------------------------------------------------------------
 */
int monarchNameCompareDecreasing(const void * a, const void * b){

	const Person* ap = (Person*)a;
	const Person* bp = (Person*)b;

	if ( strcicmp(ap->name, bp->name) > 0) return -1;
	if( strcicmp(ap->name, bp->name) < 0) return 1;

	if(strcicmp(ap->name, bp->name) == 0) {
		// need to compare regnal
		int ar = regnalToNumeral(ap);
		int br = regnalToNumeral(bp);
		if(ar < br) return 1;
		if(ar > br) return -1;
		return 0;
	}

	return 0;
}


/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 22, 2019
   @mname   ageCompareIncreasing
   @details
      compare ages
	  \n
  --------------------------------------------------------------------------
 */
int ageCompareIncreasing(const void * a, const void * b){

	const Person* ap = (Person*)a;
	const Person* bp = (Person*)b;

	int ageA = calculateAge(ap);
	int ageB = calculateAge(bp);

	if ( ageA < ageB) return -1;
	if( ageA > ageB ) return 1;
	if( ageA == ageB) return 0;

	return 0;
}



/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 22, 2019
   @mname   ageCompareIncreasing
   @details
      compare ages
	  \n
  --------------------------------------------------------------------------
 */
int ageCompareDecreasing(const void * a, const void * b){

	const Person* ap = (Person*)a;
	const Person* bp = (Person*)b;

	int ageA = calculateAge(ap);
	int ageB = calculateAge(bp);

	if ( ageA > ageB) return -1;
	if( ageA < ageB ) return 1;
	if( ageA == ageB) 	return 0;

	return 0;
}

/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 22, 2019
   @mname   regnalCompareIncreasing
   @details
      compare ages
	  \n
  --------------------------------------------------------------------------
 */
int regnalCompareIncreasing(const void * a, const void * b){

	const Person* ap = (Person*)a;
	const Person* bp = (Person*)b;

	int regnalA = regnalToNumeral(ap);
	int regnalB = regnalToNumeral(bp);

	if ( regnalA < regnalB) return -1;
	if( regnalA > regnalB ) return 1;

	if( regnalA == regnalB) return 0;

	return 0;
}

/**
  ---------------------------------------------------------------------------
   @author  mlambiri
   @date    Nov 22, 2019
   @mname   regnalCompareIncreasing
   @details
      compare ages
	  \n
  --------------------------------------------------------------------------
 */
int regnalCompareDecreasing(const void * a, const void * b){

	const Person* ap = (Person*)a;
	const Person* bp = (Person*)b;

	int regnalA = regnalToNumeral(ap);
	int regnalB = regnalToNumeral(bp);

	if ( regnalA > regnalB) return -1;
	if( regnalA < regnalB ) return 1;

	if( regnalA == regnalB) return 0;

	return 0;
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
int readFile(Person p[],  int &counter, char* fileName, ALLEGRO_DISPLAY *display) {

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
			p[counter].regnal[maxRegnal_c-1] = 0;
			if(regnalToNumeral(&(p[counter])) <= 0) {
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
writeFile(Person p[], int &counter, char* fileName, ALLEGRO_DISPLAY *display) {

	char* tempName = fileName;
	const int bufferSize_c = 200;
	char errorText[bufferSize_c];

	FILE* fptr = NULL;
	if (fileName == NULL) {
		tempName = configName;
		fptr = fopen(tempName, "w");
	} else {
		fptr = fopen(tempName, "w");
		if (fptr == NULL) {
			tempName = configName;
			fptr = fopen( tempName, "w");
		} //end-of-if(fptr == NULL)
	}


	if(fptr == NULL){
		sprintf(errorText, "Error opening file %s for writing", tempName);
		al_show_native_message_box(display, "Error", "Error", errorText,
				nullptr, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}

	for (int i = 0; i < counter; i++ ) {
		fprintf(fptr, "%s %s %d %s %d to %d %s %d\n",
				p[i].name,
				p[i].regnal,
				p[i].birth.year,
				p[i].birth.month,
				p[i].birth.day,
				p[i].death.year,
				p[i].death.month,
				p[i].death.day);
	} //end-of-for
	// write operation goes here
	fclose(fptr);

	return 1;
} // end-of-function recordResult



