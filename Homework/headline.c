/**************************************************************
 * Name: Michael C Lambiri       ICS3U       Fall 2019        *
 * Instructor: Mrs Cullum                                     *
 * Date:                                     Sep 10, 2019     *
 * Description:                                               *
 *   Funny headlines program                                  *
 **************************************************************/

#include<stdio.h>
#include<string.h>

int main () {

	// Variable declaration
	// I am using 4 character arrays
	char justAnArray[21];
	char favoriteSong[31];
	char favoriteMovie[31];
	char favoriteCharacter[31];

	// The program is also using 2 int and 2 float variables
	int numberOfCharacters= 0;
	int yearOfRelease = 0;

	float characterWeight = 0;
	float characterHeight = 0;

	//Questions and recording results
	printf("What is your favorite song? ");
	fgets(favoriteSong, 30, stdin);

	// I need to remove the end of line character from the character array
	favoriteSong[strlen(favoriteSong)-1] = 0;

	printf("\nWhat movie does the song '%s' make you think of? ", favoriteSong);
	fgets(favoriteMovie, 30, stdin);
	favoriteMovie[strlen(favoriteMovie)-1] = 0;

	// I have tried to use scanf after fgets
	// That does not work
	// I have googled the problem and
	// I found a solution on stackoverflow
	// If I put the response in a char array
	// I can then use sscanf to get the data

	printf("\nWhen was '%s' released in theaters? ", favoriteMovie);
	fgets(justAnArray, 20, stdin);
	sscanf(justAnArray, "%d", &yearOfRelease);

	printf("\nHow many main characters does the movie '%s' have? ", favoriteMovie);
	fgets(justAnArray, 20, stdin);
	sscanf(justAnArray, "%d", &numberOfCharacters);


	printf("\nWhat is your favorite character in the movie '%s'? ", favoriteMovie);
	fgets(favoriteCharacter, 30, stdin);
	favoriteCharacter[strlen(favoriteCharacter) -1] = 0;


	printf("\nHow much, in kg, do you think '%s' weighs? ", favoriteCharacter);
	fgets(justAnArray, 20, stdin);
	sscanf(justAnArray, "%f", &characterWeight);

	printf("\nHow tall, in meters, do you think '%s' is? ", favoriteCharacter);
	fgets(justAnArray, 20, stdin);
	sscanf(justAnArray, "%f", &characterHeight);


	//Printing the funny headlines
	printf("================= Today's Headlines ==========================\n");
	printf("Hollywood studio asks %s to gain %2.1f Kg to play in 'The Astronaut Does Not Fit in The Rocket' sequel!\n", favoriteCharacter, characterWeight);
	printf("%s announces '%s 2': 'Electric Bugaloo' will finally be released after %d years of anticipation\n", favoriteCharacter, favoriteSong, yearOfRelease);
	printf("Optical illusions test. %f of people fail!\n", characterHeight);
	printf("%s reveals hidden truth about %d types of birds.\n" , favoriteMovie, numberOfCharacters);

	return 0;
}
