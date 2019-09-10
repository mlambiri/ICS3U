/**************************************************************
 * Name: Michael C Lambiri       ICS3U       Fall 2019        *
 *                                                            *
 * Age, Weight, Height Program                                *
 **************************************************************/


#include <stdio.h>


int main() {

	//Variables declared and initialized
	int age = 0;
	int height = 0;
	float weight = 0;

	//Questionnaire and recording results
	printf("Please answer the following questions.");

	printf("\n\nHow old are you? (yrs) ");
	scanf("%d", &age);

	printf("\nHow tall are you? (cm) ");
	scanf("%d", &height);

	printf("\nWhat is your weight? (kg) ");
	scanf("%f", &weight);

	//Displaying results
	printf("\nage (yrs)      height (cm)    weight (kg)");
	printf("\n--------       ----------     ----------- ");
	printf("\n%-3d              %-6d         %-3.1f", age, height, weight);


	//Fun add-ons
	printf("\n\nThanks for using this program. Your data has been recorded!\n\n");
}


