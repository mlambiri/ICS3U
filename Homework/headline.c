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

	//Initializing variables and assigning values
	char buffer[21];
	char funnyJoke[21];
	char bigLebowski[11];
	char rickMorty[11];
	char okKo[12];

	int surfinBird = 0;
	float test01 = 0;
	float test02 = 0;

	//Questions and recording results
	printf("What is your favourite song? ");
	fgets(funnyJoke, 20, stdin);
	//fflush(stdin);
	printf("\nWhat is the lowest grade you've ever seen? ");
	fgets(buffer, 20, stdin);
	sscanf(buffer, "%d", &surfinBird);
	//fflush(stdin);
	printf("\nName your arch-nemesis! ");
	fgets(bigLebowski, 10, stdin);

	printf("\nWhat colour of bird reminds you the most of the song surfing bird? ");
	fgets(buffer, 20, stdin);
	sscanf(buffer, "%f", &test01);

	printf("\nHow would you describe your math teacher in one word? ");
	fgets(okKo, 11, stdin);

	//Ensuring that it all gets printed on the same line
	funnyJoke[strlen(funnyJoke) - 1] = '\0';
	bigLebowski[strlen(bigLebowski) - 1] = '\0';
	rickMorty[strlen(rickMorty) - 1] = '\0';
	okKo[strlen(okKo) - 1] = '\0';

	//Printing the funny headlines
	printf("A %s %s sang %s %d times, in the most %s way imaginable.\n", rickMorty, bigLebowski, funnyJoke, surfinBird, okKo);

	return 0;
}
