/*******************************************************************************
 *  Name: Michael Lambiri             Course ICS3U          Date Sept 23 2019  *
 *                                                                             *
 *  Instructor Ms. Lindsay Cullum                                              *
 *                                                                             *
 *   Program to calculate cost and calories of a subway order                  *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() {
    bool veggie;
    char order[21] = "";
    char strSandwich[21];
    char strSnack[21] = "";
    char strDrink[21] = "";
    bool bVeggie = false;
    bool bChips = false;
    bool bCookies = false;
    bool bDrink = false;
    bool bBottle = false;
    bool bTooManySnacks = false;
    float fCost = 0.0;
    int iCalories = 0;

    // Input Section
    printf("Welcome to the Sandwich Shop\n");

    /*
     * The original program did not have a response to other characters apart from v.
     * I made it so that it could tell you to respond with another character if the one
     * you used is not one of the two (v or c).
     */
    while(true){
        printf("Would you like a veggie or roast chicken Sub? (v/c) ");
        scanf("%s", order);
        if (tolower(order[0]) == 'v') {
            bVeggie = true;
            strcpy(strSandwich, "veggie");
            break;
        }
        else if(tolower(order[0]) == 'c') {
            bVeggie = false;
            strcpy(strSandwich, "roast chicken");
            break;
        } else{
            printf("Invalid choice. Please pick again.\n");
        }
    } //end-while

    /*
     * Similar purpose as the first while loop. Prevents you from entering random
     * characters that the program would read strangely.
     */
    while(true) {
        printf("Would you like chips? (y/n) ");
        scanf("%s", order);
        if (tolower(order[0]) == 'y') {
            bChips = true;
            strcpy(strSnack, "and chips");
            break;
        }
        else if (tolower(order[0]) == 'n') {
            bChips = false;
            break;
        }
        else {
            printf("Please use a valid response\n");
        }
    } //end-while
    while(true){
        printf("Would you like cookies? (y/n) ");
        scanf("%s", order);
        if (tolower(order[0]) == 'y') {
            bCookies = true;
            strcpy(strSnack, "and cookies");
            break;
        }
        else if (tolower(order[0])== 'n') {
            bCookies = false;
            break;
        }
        else {
            printf("Please use a valid response\n");
        }

    } //end-while
    while(true){
        printf("Would you like a drink? (y/n) ");
        scanf("%s", order);
        if (tolower(order[0]) == 'y') {
            bDrink =  true;
            printf("Would you like a bottle or a fountain drink? (b/f) ");
            scanf("%s", order);
            if (tolower(order[0]) == 'b') {
                bBottle = true;
                strcpy(strDrink, "and bottle drink");
            }
            else {
                bBottle = false;
                strcpy(strDrink, "and fountain drink");
            }
            break;
        }
        else if(tolower(order[0]) == 'n'){
            bDrink = false;
            break;
        }
        else {
            printf("Please use a valid response\n");
        }
    } //end-while


    // Process Section

    /*************************************************************
    You will be coding the process section of the code.

    You need to calculate:
        1.  the cost of the order
        2.  the number of calories in the order.

    Please use the following information:
        1.  The user is not allowed to purchase both a bag of chips and cookies.
        2.  Veggie subs are 460 calories, and cost $5.50.
        3.  Roast chicken subs are 620 calories, and cost $6.75.
        4.  Chips are 222 calories, and cost $1.10.
        5.  Cookies are 440 calories, and cost $1.60.
        6.  Bottle drinks are 187 calories, and cost $1.80.
        7.  Fountain drinks are 260 calories, and cost $1.30.
        8.  If you buy a combo, you get $1.00 off.
            (A combo consists of a sub, a bag of chips OR cookies, and a drink.)

    Note: As this is an assignment about if statements, you should be implementing
    the requirements using if statements.

    ENTER YOUR CODE HERE
     **************************************************************/

    if (bTooManySnacks){
        printf("Order Error: You can't order both cookies and chips, please  try again\n");
        return 22;
    }

    // Output
    //Calculating calories and cost for the menu
    if (bVeggie) {
        fCost += 5.50;
        iCalories += 460;
    }
    else {
        fCost += 6.75;
        iCalories += 620;
    }

    if (bChips) {
        fCost += 1.10;
        iCalories += 222;
    }

    if (bCookies) {
        fCost += 1.60;
        iCalories += 440;
    }

    if (bDrink) {
        if (bBottle) {
            fCost += 1.80;
            iCalories += 187;
        }
    }

    if (bDrink && (bCookies || bChips)) {
        fCost -= 1.00;
    }


    if (bDrink) {
        if (bBottle == false) {
            fCost += 1.30;
            iCalories += 260;
        }
    }

    //Preventing you from having an unhealthy meal.
    if (bChips && bCookies) {
        (bTooManySnacks = true);
    }


    printf("\n\nYour order of %s sandwich %s %s is\n ",
            strSandwich, strSnack, strDrink);
    printf("\t$%.2f and contains %d calories\n",
            fCost, iCalories);
    /*
     * The average teenager/adult's diet requires 2000 calories.
     * In order to display percents, the number is 100 times bigger than the decimal. So I made it divide by 20.
     */
    printf("This meal represents %2.2f%% of your daily recommended calories\n\n",
            iCalories/20.0);

    return 0;
}
