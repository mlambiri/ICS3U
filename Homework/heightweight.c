//Height,weight and age program.

//Stuff to remember
#include <stdio.h>
int main() {

//Variables declared
    int age;
    int height;
    float weight;

//Variables initialized
    age = 0;
    height = 0;
    weight = 0;

//Questionnaire and recording results
    printf("Please answer the following questions.");
    printf("\n\nHow old are you?");
    scanf("%d", &age);
    printf("\nHow tall are you?");
    scanf("%d", &height);
    printf("\nWhat is your weight?");
    scanf("%f", &weight);
    //Displaying results
    printf("\n\nage (yrs)");
    printf("      height (cm)");
    printf("    weight (kg)");
    printf("\n--------       ----------     ----------- ");
    printf("\n%-3d", age);
    printf("              %-6d", height);
    printf("         %-3.1f", weight);
    //Fun add-ons
    printf("\n\nThanks for using this program. Your data has been recorded");
}

