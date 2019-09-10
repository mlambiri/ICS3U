/**************************************************************
 * Name: Michael C Lambiri       ICS3U       Fall 2019        *
 *                                                            *
 * Date:                                     Sep 10, 2019     *
 * Description:                                               *
 *   Funny headlines program                                  *
 **************************************************************/
#include<stdio.h>

int main () {

	char funnyJoke[11];
	char surfinBird[11];
	char bigLebowski[11];
	char rickMorty[11];
	char okKo[11];

	printf("Test1");
	fgets(funnyJoke, 10, stdin);
	printf("\nTest2");
	fgets(surfinBird, 10, stdin);
	printf("\nTest3");
	fgets(bigLebowski, 10, stdin);
	printf("\nTest4");
	fgets(rickMorty, 10, stdin);
	printf("\nTest5");
	fgets(okKo, 10, stdin);

	printf("%s", funnyJoke);
	printf("%s", surfinBird);
	printf("%s", bigLebowski);
	printf("%s", rickMorty);
	printf("%s", okKo);

	return 0;
}
