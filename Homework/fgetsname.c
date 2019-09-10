/**************************************************************
 * Name: Michael C Lambiri       ICS3U       Fall 2019        *
 *                                                            *
 * fgets program                                              *
 **************************************************************/
#include <stdio.h>

int main(void) {

 //character declaration
 char nameYour[21];

 //asking questions
 printf("What is your full name? ");

 //fgets
 fgets(nameYour, 20, stdin);

 //printing results
 printf("Your name is %s", nameYour);

 return 0;
}

